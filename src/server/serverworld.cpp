/***************************************************************************
  serverworld.cpp  -  NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 1999-10-20
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-21
  by                   : Nils Thuerey
  email                : n_t@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "server/serverworld.h"
#include "common/objects/obj_scoreboard.h"
#include "common/objects/obj_player_data.h"
#include "common/objects/obj_bonus.h"
#include "common/objects/obj_bonus_health.h"
#include "common/objects/obj_bonus_invisibility.h"
#include "common/objects/obj_chatmessage.h"
#include "common/objects/obj_settings.h"
#include "common/world_objects.h"
#include "common/sound/soundsystem.h"
#include "common/configuration/configuration.h"

// About four bonuses per minute.
#define BONUS_PER_SEC (1.0/15.0)
// intersection distance, better use bounding box
#define LIDISTANCE 4


void Serverworld::save_original_map() {
	if (original_map) free(original_map);

	int bytes_to_move = map_buffer.get_xsize()*map_buffer.get_ysize()*sizeof(int32);
	original_map = (int32*)malloc(bytes_to_move);	
	memcpy(original_map,map_buffer.get_pixels_read(),bytes_to_move);	
}

Serverworld::Serverworld(Loader *loader_, Soundsystem *soundsystem_, bool always_reload_, Weapon_style weapon_style_, const int *gameweapons_) : World(loader_, soundsystem_, always_reload_) {
	weapon_style=weapon_style_;
	for(int i=0; i<WEAPON_SLOTS; ++i)
		gameweapons[i]=gameweapons_[i];

	next_free_objectid = 1;

	Obj_scoreboard *sb = (Obj_scoreboard *)newobject(ot_scoreboard);
	if (sb)
		scoreboard_id = sb->getid();

	original_map = NULL;

	killer_line = 1;
	killer_line = 201;
	last_reader_length = 0;
}

Serverworld::~Serverworld() {
	if (original_map) free(original_map);
}

Obj_base *Serverworld::newobject(Object_type objecttype) {
	Obj_base *newone = World::newobject(objecttype,  next_free_objectid);
	next_free_objectid++;

	return newone;
}

bool Serverworld::addObject(Obj_base *newObject) {
	objects->insert(std::pair<int,Obj_base*>(next_free_objectid,newObject));
	next_free_objectid++;
	//logmsg(lt_debug,"#%i added: %s",objectID,newObject->getname());
	return true;
}

// prepare a standard game update
void Serverworld::server_think(Update_queue *update_queue, bool sendQuit) {
	Serializer_reader *serializer_reader = update_queue->new_reader();
	serializer_reader->clear();

	//Let the objects do their thinking (this collects thoughts)
	{
		World_objects::iterator i = objects->begin();
		while (i != objects->end()) {
			if (!i->second->isdone())
				i->second->server_think();
 	
			i++;
		}
	}

	// place bonus: health, invisibility
	if( rnd(0, 100000) <= 100000.0 * BONUS_PER_SEC * get_last_frame_delta()) {
		bool done   = false;
		int  ntries = 20;

		// TODO: let bonus place itself?
		logmsg(lt_debug, "Trying to place bonus");
		while(!done && ntries>0) {
			int x = rnd( 20, (map_buffer.get_xsize()-20) );
			int y = rnd( 20, (map_buffer.get_ysize()-20) );

			if( isPassable(x,y) ) {
				done = true;
				float type_roulette = rnd(0, 100000);
				
				if( type_roulette < 30000 ) {
					Obj_bonus_invisibility *cloak = (Obj_bonus_invisibility*) newobject(ot_bonus_invisibility);
					cloak->init( x, y );
				} else {
					Obj_bonus_health *health = (Obj_bonus_health*) newobject(ot_bonus_health);
					health->init( x, y );
				}
			}
		}
	}

	//collect up all the changed states of the surviving objects to send to clients
	{
		World_objects::iterator i = objects->begin();
		while (i != objects->end()) {
			if (i->second->isdirty()) {
				serializer_reader->writer_int(TT_OBJECT_STATE);
				serializer_reader->writer_int(i->second->getid());
				serializer_reader->writer_int(i->second->gettype());
				i->second->serialize(serializer_reader);
				i->second->beclean();
				// 2004-06-05, egore: causes way to much log
				//logmsg(lt_debug, "[ sent %d %d ",i->second->getid(), i->second->gettype() );
			}
			i++;
		}
	}

	//Tell the clients to let the dead objects have it.
	{
		World_objects::iterator i = objects->begin();
		while (i != objects->end()) {
			if (i->second->isdone()) {
				serializer_reader->writer_int(TT_OBJECT_DELETE);
				serializer_reader->writer_int(i->second->getid());
			}
			i++;
		}
	}

	//clean out the dead objects
	remove_dead_objects();

	// remove lonely pixels from map
	kill_lonely_bits(serializer_reader);
	
	//Tell the clients about sound events
	soundsystem->server_think(serializer_reader);
	
	//send the time:
	if(!sendQuit) {
		// normal...
		serializer_reader->writer_int(TT_WORLD_STATE);
	} else {
		serializer_reader->writer_int(TT_END_GAME);
	}
	serializer_reader->writer_float(gettime());
	
	//feed the stats
	last_reader_length = serializer_reader->get_length();

	//let the update_queue at the serialized data:
	update_queue->submit_update(serializer_reader);
}

void Serverworld::makehole(int x, int y, int diameter) {
	if (diameter > 16) diameter = 16;
	if (diameter < 1) diameter = 1;

	//do the actual hole putting
	holes[diameter-1]->draw_map(&map_buffer,&bgmap_buffer,x,y);
}

void Serverworld::make_all_dirty() {
	World_objects::iterator i = objects->begin();
	while (i != objects->end()) {
		i->second->setdirty();
 		i++;
	}
}

bool Serverworld::handle_join_request(Net_joinrequest &joinrequest, int &nplayers, Obj_player_data **new_players, Net_joinreply &joinreply, char *&map_data) {
	nplayers=joinrequest.nplayers;

	Obj_player_data *first_player = NULL;
	Obj_scoreboard *sb = (Obj_scoreboard *)get_object(scoreboard_id, ot_scoreboard );
	for (int i=0; i<nplayers; ++i) {
		Obj_player_data *new_player = (Obj_player_data *)newobject(ot_player_data);
		if(sb)
			sb->initFrag(new_player->getid() );
		if (!new_player)
			return false;
		new_player->init(&joinrequest.players[i],
			     weapon_style, gameweapons);
		new_players[i]=new_player;
		logmsg(lt_message, "Added player \"%s\" to the game (team id %d)", joinrequest.players[i].name, joinrequest.players[i].team );
		joinreply.player_ids[i]=new_player->getid();
		//NT remember first player for join message
		if (!first_player) {
			first_player = new_player;
		}
	}
	joinreply.map_xsize = map_buffer.get_xsize();
	joinreply.map_ysize = map_buffer.get_ysize();
	joinreply.randomseed = randomseed;

	unsigned int pixelcount = joinreply.map_xsize*joinreply.map_ysize;
	unsigned int pixelbytes = pixelcount*sizeof(int32);

	// we don't need diff
	//Create the diff:
	int32 *diff = (int32*)malloc(pixelbytes);
	for (unsigned int i=0;i<pixelcount;i++) {
		//int32 op = original_map[i];               //original pixel
		int32 mp = map_buffer.get_pixels_read()[i]; //current pixel
		//if (IS_PASSABLE(mp) && !IS_PASSABLE(op)) {
		//	diff[i] = PF_TYPE_HOLE; //invert the hole bit
		//} else
		//	diff[i] = mp ^ op;
		diff [ i ] = mp;
	}

	//compress the data:
	unsigned int buffer_size = ROUND(pixelbytes*1.01)+12; //read zlib for the extra space.
	map_data = (char*)malloc(buffer_size);

	unsigned long compressed_size = pixelbytes;
	int zerror = compress((unsigned char *)map_data, &compressed_size, (const unsigned char*)diff, pixelbytes);

	if (zerror == Z_OK) {
		joinreply.zcompressed = true;
		joinreply.compressed_size = compressed_size;
		logmsg(lt_debug, "Compressed the diff to %i bytes", joinreply.compressed_size);
	} else {
		memcpy(map_data, diff, pixelbytes);
		joinreply.zcompressed = false;
		joinreply.compressed_size = pixelbytes;
		logmsg(lt_error, "Unable to compress the diff, the error was %i", zerror);
	}

	free(diff);

	// send join message
	char joinmsg[Obj_chatmessage::MSGLENGTH];
	snprintf(joinmsg, Obj_chatmessage::MSGLENGTH, "System: Player %s joined the game!", first_player->get_player_name() );
	Obj_chatmessage *sendmsg = dynamic_cast<Obj_chatmessage *>(newobject(ot_chatmessage));
	sendmsg->initmessage( joinmsg, Obj_chatmessage::SYSMSGCOL );
	sendmsg->bedone();
	sendmsg->setdirty();

	return true;
}

void Serverworld::kill_lonely_bits(Serializer_reader *serializer_reader) {
	//logmsg(lt_debug,"kill lonely bits enable");
	return;

	int w = bgmap_buffer.get_xsize();
	const int32 *bgpixel = bgmap_buffer.get_pixels_read();
	int32 *fgpixel = map_buffer.get_pixels_write();
	bgpixel+= (killer_line*w);
	fgpixel+= (killer_line*w);

	// clean two lines...
	for (int i=0;i<2;i++) {

		// dont handle borders (would require other stencil)
		fgpixel++;
		bgpixel++;
		for (int x=1;x<(w-1);x++) {
			if (
				IS_EARTH(*fgpixel) &&
				IS_PASSABLE(*(fgpixel-1)) +
				IS_PASSABLE(*(fgpixel+1)) +

				IS_PASSABLE(*(fgpixel-1-w)) +
				IS_PASSABLE(*(fgpixel  -w)) +
				IS_PASSABLE(*(fgpixel+1-w)) +

				IS_PASSABLE(*(fgpixel-1+w)) +
				IS_PASSABLE(*(fgpixel  +w)) +
				IS_PASSABLE(*(fgpixel+1+w)) >4) 
			{
				//remove pixel here and tell the client about it.
				serializer_reader->writer_int(TT_DEAD_PIXEL);
				serializer_reader->writer_int(x);
				serializer_reader->writer_int(killer_line);
				*fgpixel = *bgpixel;				
				
				//logmsg(lt_debug,"killed one! x,y:%d,%d",x,killer_line+i);
			}
			fgpixel++;
			bgpixel++;
		}
		fgpixel++;
		bgpixel++;

		killer_line++;
		if (killer_line >= bgmap_buffer.get_ysize()-2) {
			// we're done for now, dont try to reinit fg/bg pixels
			killer_line = 1;
			i = 10000;
		}
	}
	//logmsg(lt_debug,"kill lonely bits done");
}

int Serverworld::find_radius_object(float xpos, float ypos, float radius, Object_type type) {
	World_objects::iterator i = objects->begin();
	while (i != objects->end()) {
		if(i->second->gettype()==type) {
			float xp=0;
			float yp=0;
			i->second->getpos(xp,yp);
			float distancesq = SQR(xp-xpos)+SQR(yp-ypos);
			if(distancesq<SQR(radius))
				return i->second->getid();
		}
		++i;
	}
	return -1;
}

void Serverworld::do_radius_damage(int hurter_id, float xpos, float ypos, float radius, float damage) {
	//find all the objects that need to be hurt by us.
	World_objects::iterator i = objects->begin();
	while (i != objects->end()) {
		if (i->second->hurtable()) {
			float xp=0;
			float yp=0;
			i->second->getpos(xp,yp);
			float distance = sqrt(SQR(xp-xpos)+SQR(yp-ypos));

			if (distance < radius*2) {
				float hitpoints;
				if (distance < radius)
					hitpoints = damage;
				else
					hitpoints = damage*(1-(distance-radius) / radius);
				Vector v;
				v.y = yp-ypos;
				v.normalize();
				v.scale(radius);
				i->second->hurt(hitpoints,v,hurter_id);
			}
		}
		i++;
	}
}

// called from obj_base::update_simple, when object collision is switched on (e.g. bullet hits)
bool Serverworld::hit_object(float old_xpos, float old_ypos, float xpos, float ypos, int &cx, int &cy, int &id) {
	int dummy = -1; // use dummy integer, that switches off leaver owner checks
	return hit_object(old_xpos,old_ypos,xpos,ypos,cx,cy,id, dummy);
}

bool Serverworld::hit_object(float old_xpos, float old_ypos, float xpos, float ypos, int &cx, int &cy, int &id, int &leaveid) {
	int closest_objectid = -1;
	int closest_xc = -1;
	int closest_yc = -1;
	float closest_dist = 9999;
		
	World_objects::iterator i = objects->begin();
	while (i != objects->end()) {
		if (i->second->hurtable()) {
			float vx;
			float vy;
			i->second->getpos(vx,vy);
			long xhit = ROUND(xpos);
			long yhit = ROUND(ypos);

			//logmsg(lt_debug,"%f, oid:%d leaveid:%d pos:%f,%f  old:%f,%f   obj_pos:%f,%f", gettime(), i->second->getid(), leaveid, xpos, ypos, old_xpos, old_ypos, vx,vy ); // DEBUG D2

			//Then see if the path crosses the victim object..
			// TODO better use a bounding box here
			if ((Math::lines_intersect(
					ROUND(vx),ROUND(vy-LIDISTANCE),ROUND(vx),ROUND(vy+LIDISTANCE),
					ROUND(old_xpos),ROUND(old_ypos),ROUND(xpos),ROUND(ypos),
					&xhit,&yhit)) ||
					(Math::lines_intersect(
					ROUND(vx-LIDISTANCE),ROUND(vy),ROUND(vx+LIDISTANCE),ROUND(vy),
					ROUND(old_xpos),ROUND(old_ypos),ROUND(xpos),ROUND(ypos),
					&xhit,&yhit)))
			{

				// check leave id
				if(i->second->getid() == leaveid) {
					// object is still on owner -> dont hit owner
				} else {

					// check distance
					float thisdist = Math::dist(xhit,yhit,old_xpos,old_ypos);

					if (thisdist<closest_dist) {
						closest_dist = thisdist;
						closest_objectid = i->second->getid();
						closest_xc = xhit;
						closest_yc = yhit;
					}
				} // now owner
			} else {
				// no hit, still check leave id
				if(i->second->getid() == leaveid) {
					// only delete mLeaveOwnerID if far enough away, Tom
					if (Math::dist(old_xpos, old_ypos, vx, vy) > 10.0) {
						leaveid = -1;
					}
				}
			}
		} // hurtable ?
		i++;
	}

	//report the findings:
	if (closest_objectid >= 0) {
		cx = closest_xc;
		cy = closest_yc;
		id = closest_objectid;
		return true;
	} else {
		return false;
	}
}
