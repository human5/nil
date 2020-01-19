/***************************************************************************
  clientworld.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-20
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-05-18
  by                   : Nils Thuerey
  email                : n_t@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/systemheaders.h"
#include "client/clientworld.h"
#include "common/objects/obj_base.h"
#include "common/objects/obj_particle.h"
#include "common/objects/obj_player_avatar.h"
#include "common/objects/obj_player_data.h"
#include "common/objects/obj_scoreboard.h"
#include "common/objects/obj_smoke.h"
#include "common/objects/obj_gib.h"
#include "common/console/logmsg.h"
#include "common/world_objects.h"
#include "common/sound/soundsystem.h"
#include "common/ripped.h"

Clientworld::Clientworld() {
	mpLoader = NULL;
	gorelevel = 3;
	anon_objects_inuse = 0;

	for(int i=0; i<Obj_chatmessage::MSGNUM; i++) {
		gameMsgColor[i] = PIXEL(255,255,255);
		gameMsgText[i][0] = 0;
	}
}

Clientworld::Clientworld(Loader *loader_, Soundsystem *soundsystem_) {
	anon_objects_inuse = 0;
	gorelevel = 3;
	init(loader_, soundsystem_);
}

void Clientworld::init(Loader *loader_, Soundsystem *soundsystem_) {
	mpLoader = loader_;
	World::init(mpLoader, soundsystem_);
}

Clientworld::~Clientworld() {}

// FIXME: put these into individual functions once the World class is split up into Server_world and Client_world
//! process or dispatch all thoughts
void Clientworld::client_think() {
	//First update the state of a few lucky objects:
	while (serializer_writer.next_int() == TT_OBJECT_STATE) {
		serializer_writer.eat_int(TT_OBJECT_STATE);

		int winners_id = serializer_writer.reader_int();
		Object_type winners_type = (Object_type)serializer_writer.reader_int();

		Obj_base *update_target = get_object(winners_id,winners_type);
		if (update_target)
			update_target->serialize(&serializer_writer);
		else {
		
			//If the new object is the scoreboard, remember it for later:
			if (winners_type == ot_scoreboard) scoreboard_id = winners_id;

			//Now create the object and hand it it's data:
			Obj_base *lucky_winner = newobject(winners_type, winners_id);
			lucky_winner->serialize(&serializer_writer);
		}
	} //while (game->peek(cc_object_updates){

	//Then kill the losers
	while (serializer_writer.next_int() == TT_OBJECT_DELETE) {

		serializer_writer.eat_int(TT_OBJECT_DELETE);
		int losers_id = serializer_writer.reader_int();

		Obj_base *looser = get_object(losers_id,ot_error);
		if (looser)
			looser->bedone();
		else
			logmsg(lt_warning,"Recived a kiss-of-death destined for a nonexistant object (id: %i)",losers_id);
	}
	
	// Kill dead pixels
	while (serializer_writer.next_int() == TT_DEAD_PIXEL) {

		serializer_writer.eat_int(TT_DEAD_PIXEL);
		int x = serializer_writer.reader_int();
		int y = serializer_writer.reader_int();
		int32 pixel = x+y*bgmap_buffer.get_xsize();
		int32 savepixel=map_buffer.get_pixels_write()[pixel];
		map_buffer.get_pixels_write()[pixel]=bgmap_buffer.get_pixels_read()[pixel];

		Obj_particle_debris *debris = (Obj_particle_debris *)newanonobject(ot_particle_debris);
		if (debris) {
			debris->setcolor(savepixel);
			debris->init(x, y, ROUND(rnd(20,-20)), ROUND(rnd(20,-20)));
		} else
			logmsg(lt_assert,"Could not make debris");
	}
	
	//Play sound events
	soundsystem->client_think(serializer_writer);
		
	//get world time
	if (serializer_writer.next_int() == TT_END_GAME) {
		// quit...
		serializer_writer.eat_int(TT_END_GAME);
		logmsg(lt_message,"Server is shutting down...!!!");
		logmsg(lt_debug,"Got TT_END_GAME message, requesting quit");
		request_quit = true;
	} else {
		serializer_writer.eat_int(TT_WORLD_STATE);
	}
	settime(serializer_writer.reader_float());

	//Check for errors:
	if (!serializer_writer.is_empty()) {
		logmsg(lt_error,"The serializer_writer was not empty!");
	}
		if (serializer_writer.empty_read()) {
		logmsg(lt_error,"More bytes were requested than the writer contained!");
	}

	// let the objects do some thinking:
	// this should happen now, before the rest of the updates (similar to the server way)?
	{
		World_objects::iterator i = objects->begin();
		while (i != objects->end()) {
			if (!i->second->isdone()) {
				i->second->client_think();
			} else {
				//NT check if its a chat message, if yes store
				if(i->second->gettype() == ot_chatmessage) {
					Obj_chatmessage *msg = dynamic_cast<Obj_chatmessage *>(i->second);
					add_message( msg->get_message(), msg->get_color() );
				}
			}
			i++;
		}
	}

	//also let the anonymous objects have a go:
	//Note: if the machine is short on cycles, this may be a good place to save a few (like: if (weak_ass_cpu) limit(anon_objects))
	int c;
	for (c=0; c<anon_objects_inuse;c++) {
		if (!anon_objects[c]->isdone())
			anon_objects[c]->client_think();
	}

	remove_dead_objects();

	//Boring array gymnastics here, I know that handling these arrays ought to be hidden away in some class somewhere,
	//but for now they are simply handled by hand, b/b+ trees may be a bit overkill though...
	//Bring out yer dead...
	{ //anon
		for (c=0; c<anon_objects_inuse;c++)
			if (anon_objects[c]->isdone()) {
					delete anon_objects[c];				
					anon_objects[c] = NULL;
			}
	
		// clean up pointers
		int scribble=0;
		for (int c=0; c<anon_objects_inuse;c++) {
			if (anon_objects[c]) {
				anon_objects[scribble] = anon_objects[c];
				scribble++;
			}
		}
		anon_objects_inuse = scribble;
	}

	// display frag messages (and remove from scoreboard)
	Obj_scoreboard *sb = getscoreboard ();
	char fragmsg[256];
	while( sb -> get_fragmessage(fragmsg) ) {
		add_message(fragmsg, Obj_chatmessage::SYSMSGCOL); 
	}

}

void Clientworld::draw(Viewport *viewport) {
	map_buffer.draw_copy(viewport, viewport->xpos,viewport->ypos, viewport->get_xsize(),viewport->get_ysize(), 0,0);

	for (int c=0; c<anon_objects_inuse;c++) {
		if (!anon_objects[c]->isdone())
			anon_objects[c]->draw(viewport);
	}

	World_objects::iterator i = objects->begin();
	while (i != objects->end()) {
		if (!i->second->isdone())
			i->second->draw(viewport);
		i++;
	}
/*	
	for (int c=0; c<real_objects_inuse;c++)
		if (!real_objects[c]->isdone())
			real_objects[c]->draw(viewport);
*/
}

Obj_base *Clientworld::newanonobject(Object_type objecttype) {
	if (anon_objects_inuse+1 >= MAX_ANON_OBJECTS) {
		logmsg(lt_error,"There are now %i anonymous objects in the world, not going to create a %s", anon_objects_inuse, OBJECT_NAMES[objecttype]);
		return NULL;
	}

	Obj_base *newobject = NULL;
	switch (objecttype) {
		case ot_weapon_test          :
		case ot_weapon_minigun       :
		case ot_weapon_shotgun       :
		case ot_weapon_megashotgun   :
		case ot_weapon_railgun       :
		case ot_weapon_plasmagun     :
		case ot_weapon_flamethrower  :
		case ot_weapon_larpa         :
		case ot_weapon_rocket        :
		case ot_weapon_homingmissile :
		case ot_weapon_mininuke      :
		case ot_weapon_bignuke       :
		case ot_weapon_clusterbomb   :
		case ot_bignuke              :
		case ot_rocket               :
		case ot_homingmissile        :
		case ot_grenade              :
		case ot_clusterbomb          :
		case ot_plasmaball           :
		case ot_railslug             :
		case ot_hook                 :
		case ot_scoreboard           :
		case ot_error                :
		case ot_tinygranade          :
		case ot_testobject           :
		case ot_explosion            :
		case ot_player_avatar        :
		case ot_player_data          :
		case ot_projectile           :
		case ot_chatmessage          :
		case ot_bonus_health         :
		case ot_weapon_jetpack       :
		case ot_fireball             :
		case ot_larpa                :
		case ot_settings             :
			logmsg(lt_assert, "Someone tried to create an non-anonymous with newanonobject()");
			return NULL; //these are not anon objects
		case ot_particle_smoke       : newobject = new Obj_particle_smoke(this, -1); break;
		case ot_particle_blood       : newobject = new Obj_particle_blood(this, -1); break;
		case ot_particle_debris      : newobject = new Obj_particle_debris(this, -1); break;
		case ot_particle_railtrail   : newobject = new Obj_particle_railtrail(this, -1); break;
		case ot_particle_spark       : newobject = new Obj_particle_spark(this, -1); break;
		case ot_gib                  : newobject = new Obj_gib(this, -1); break; // D8
		case ot_smoke                : newobject = new Obj_smoke(this, -1); break; // D8
		default:
			logmsg(lt_assert,"Unknown object type (%d) ", objecttype);
			return NULL; //these are not anon objects
	}
	anon_objects[anon_objects_inuse++] = newobject;
	return newobject;
}

//! display all anonymous objects in the world
void Clientworld::dumpAnonObjects()
{
	logmsg(lt_debug,"Number of anonymous objects: %i", anon_objects_inuse );
	for(int c=0; c<anon_objects_inuse;c++) {
		logmsg(lt_debug,"%i: (done %d) %s",c,anon_objects[c]->isdone(),anon_objects[c]->getname());
	}
}

void Clientworld::makehole(int x, int y, int diameter) {
	// this might happen, as usually two holes are drawn...
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	
	if (diameter > 16) diameter = 16;
	if (diameter < 1) diameter = 1;
	
	//debris start corner ( can't be at the far edge of the screen )
	int dx = x;
	int dy = y;
	
	if (dx > map_buffer.get_xsize()-5) dx = map_buffer.get_xsize()-5;
	if (dy > map_buffer.get_ysize()-5) dy = map_buffer.get_ysize()-5;

	// TODO (not very important) - take the real radius into account
	//find a couple of colors to give to the debris
	int32 pcolors[16];
	int pcolors_inuse = 0;

	const int32 *ppix = map_buffer.get_pixels_read() + dx + dy*map_buffer.get_xsize();	
	for (int yc=0;yc < 5 && pcolors_inuse < 16;yc++) {
	
		for (int xc=0;xc < 5 && pcolors_inuse < 16;xc++) {
			if (IS_EARTH(*ppix) && pcolors_inuse < 16)
				pcolors[pcolors_inuse++] = *ppix;
			ppix++;
		}
		
		//one up, and five back
		ppix += map_buffer.get_xsize() - 5;
	}

	//do the actual hole putting
	holes[diameter-1]->draw_map(&map_buffer,&bgmap_buffer,x,y);

	//spawn the debris:
	if (pcolors_inuse) {
		int radius = diameter/2;
		for (int c=0; c <radius;c++) {
			Obj_particle_debris *debris = (Obj_particle_debris *)newanonobject(ot_particle_debris);

			if (debris) {
				debris->setcolor(pcolors[rnd(0,pcolors_inuse-1)]);
				debris->init(x+rnd(-2,2), y+rnd(-2,2), ROUND(rnd(20,-20)),	ROUND(rnd(20,-20)));
			}
		}
	}
}

// that's not diff any more!!!!
bool Clientworld::createmap(Net_joinreply &joinreply, char *map_data) {
	logmsg(lt_debug,"Restoring map state from a %i byte diff",joinreply.compressed_size);

	unsigned int pixelcount = joinreply.map_xsize*joinreply.map_ysize;
	unsigned int pixelbytes = pixelcount*sizeof(int32);

	//unpack the map_data into the patch.
	int32 *diff = (int32*)malloc(pixelbytes);

	if (joinreply.zcompressed) {
		unsigned long uncompressed_bytes = pixelbytes;
		int zerror = uncompress(
				(unsigned char *)diff,
				&uncompressed_bytes,
				(const unsigned char*)map_data,
				joinreply.compressed_size
			);

		if (zerror == Z_OK) {
			logmsg(lt_debug,"uncompressed the diff alright");
		} else {
			logmsg(lt_error,"Unable to uncompress the diff! the error was:%i",zerror);
		}

	} else {
		logmsg(lt_warning,"received uncompressed diff !?");
		memcpy(diff,map_data,pixelbytes);
	}

	World::initmap ( joinreply.map_xsize , joinreply.map_ysize );

	//apply the patch to the map
	//for (unsigned int i=0;i<pixelcount;i++) {
	for(int y=0;y < mSizeY;y++)
		for(int x=0;x < mSizeX;x++) {
		//int32 op = map_buffer.get_pixels_read()[i]; //current pixel
		//int32 dp = diff[i];
		//map_buffer.get_pixels_write()[i] = diff [ i ];
		map_buffer.setRawPixel(x,y, diff [ y*mSizeX+x ] );
		//if( IS_PASSABLE ( dp ) ) {
		if( isPassable(x,y) ) {
			//bgmap_buffer.get_pixels_write()[i] = diff [ i ];
			bgmap_buffer.setRawPixel(x,y, diff [ y*mSizeX+x ] );
		} else {
			// init something nicer here...
			//bgmap_buffer.get_pixels_write()[i] = mkrndcolor(0x341800, 0x602000) | PF_TYPE_HOLE;
			bgmap_buffer.setRawPixel(x,y, mkrndcolor(0x341800, 0x602000) | PF_TYPE_HOLE );
		}
	}
	//memcpy(map_buffer.pixels,map_data,joinreply.compressed_size);

	free(diff);

	return true;
}

void Clientworld::add_message(char *txt, int32 col) {
	for(int i=Obj_chatmessage::MSGNUM-1; i>0; i--) {
		gameMsgColor[i] = gameMsgColor[i-1];
		gameMsgTime[i] = gameMsgTime[i-1];
		if((gameMsgText[i-1][0]!=0)) {
			strcpy( gameMsgText[i], gameMsgText[i-1] ); 
		} else {
			gameMsgText[i][0] = 0;
		}
	}
	strcpy( gameMsgText[0], txt );
	gameMsgColor[0] = col;
	gameMsgTime[0] = gettime();
}

void Clientworld::add_suicidemessage(int pkid) {
	Obj_player_data *pk = (Obj_player_data *)get_object(pkid, ot_player_data);
	char str[100];
	snprintf (str, 100, "%s commited suicide", pk->get_player_name());
	add_message(str, Obj_chatmessage::SYSMSGCOL);
}

void Clientworld::add_fragmessage(int pkid, int pvid) {
	Obj_player_data *pk = (Obj_player_data *)get_object(pkid,ot_player_data);
	Obj_player_data *pv = (Obj_player_data *)get_object(pvid,ot_player_data);
	char str[100];
	snprintf (str, 100, "%s massacred %s" , pk->get_player_name(), pv->get_player_name());
	add_message(str, Obj_chatmessage::SYSMSGCOL);
}
