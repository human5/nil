/***************************************************************************
  obj_player_data.cpp  -  the player
 ---------------------------------------------------------------------------
  begin                : 1999-11-11
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

    copyright            : (C) 1999 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_player_data.h"
#include "common/objects/obj_player_avatar.h"
#include "common/objects/obj_settings.h"
#include "common/configuration/configuration.h"
#include "common/console/logmsg.h"

//-----------------------------------------------------------------------------
Obj_player_data::Obj_player_data(World *world_, int id) : Obj_base(world_, id)
{
	strcpy(name,"zombie");
	color = PIXEL(255,128,128);
	avatar_objectid = -1;
	for (int c=0;c<WEAPON_SLOTS;c++)
		weapons[c] = -1;

	team = -1;
	animator = NULL;
	current_sequence = SEQUENCE::DUMMY;
}

//-----------------------------------------------------------------------------
Obj_player_data::~Obj_player_data()
{
	//find the avatar and kill it!
	Obj_player_avatar *avatar = get_player_avatar();
	if (avatar) {
  	avatar->bedone();
	}	

	for (int slot = 0;slot <WEAPON_SLOTS;slot++) {
		if (weapons[slot] >= 0)	{
  		Obj_weapon *old_weapon = get_weapon(slot);
			if (old_weapon) {
				old_weapon->bedone();
				weapons[slot] = -1;
			}
		}
	}

	if (animator) delete animator;

	//logmsg(lt_debug," +++ player data %d at %u deleted +++ ", getid(), (int)this );
}

//-----------------------------------------------------------------------------
void Obj_player_data::serialize(Serializer *serializer)
{
	controls.serialize(serializer);
	serializer->rw(color);
	serializer->rw(team);
	serializer->rw(avatar_objectid);
	for (int c=0;c<WEAPON_SLOTS;c++)
		serializer->rw(weapons[c]);
	serializer->rwstr(name,sizeof(name));
}

//-----------------------------------------------------------------------------
void Obj_player_data::set_weapon(int slot,Object_type object_type)
{
	if (world->isclient()) {
		logmsg(lt_error,"set_weapon called on client");
		return;
	}

	if (slot < 0 || slot >= WEAPON_SLOTS) {
		logmsg(lt_error,"An invalid slot was selected for set_weapon(): %i",slot);
		return;
	}

	//if there is a weapon in the slot then tell it to terminate
	if (weapons[slot] >= 0)	{
  	Obj_weapon *old_weapon = get_weapon(slot);
		if (old_weapon) {
			old_weapon->bedone();
			weapons[slot] = -1;
		}
	}

	//create the new weapon and save its id:
	Serverworld *serverworld = (Serverworld *)world;
 	Obj_weapon *new_weapon = (Obj_weapon *)serverworld->newobject(object_type);
	if (new_weapon) {
		new_weapon->init(objectid);
		weapons[slot] = new_weapon->getid();
	}
}

//-----------------------------------------------------------------------------
Obj_weapon *Obj_player_data::get_weapon(int slot)
{
	if (slot < 0 || slot >= WEAPON_SLOTS) {
		logmsg(lt_error,"An invalid slot was selected for get_weapon(): %i",slot);
		return NULL;
	}
	
	int weapon_objectid = weapons[slot];

	if (weapon_objectid < 0) {
//		logmsg(lt_error,"An empty slot was selected for get_weapon(): %i",slot);
		return NULL;
	} else
		return (Obj_weapon *)world->get_object(weapon_objectid,ot_error);
}

//-----------------------------------------------------------------------------
void Obj_player_data::respawn()
{
	if (world->isclient()) return;
	Serverworld *serverworld = (Serverworld *)world;
	Obj_player_avatar *player_avatar = (Obj_player_avatar *)serverworld->newobject(ot_player_avatar);
	if (!player_avatar) return;

	bool found=false;
	int x;
	int y;
	while (!found) {
		found = true;
  	x = rnd(16,world->getSizeX()-16);	
  	y = rnd(16,world->getSizeY()-16);	
    logmsg(lt_debug,"Trying to spawn the avatar at: %i,%i",x,y);
		for (int ty=0;ty < 16;ty++) {
			for (int tx=0;tx < 16;tx++) {
				int px = x+tx-8;
				int py = y+ty-8;
				const int32 *pixel = world->getMapBuffer()->get_pixels_read()+px+py*world->getMapBuffer()->get_xsize();
      	if (IS_STONE( *pixel )) {
      	//if (IS_STONE( world->getMapEntry(px,py) )) {
        	found = false;	
				}
			}
		}
	}

	// init avatar
	player_avatar->init(objectid,x,y);
	avatar_objectid = player_avatar->getid();
	setdirty();

	// reload weapons
	for(int i=0; i<WEAPON_SLOTS; ++i) {
		Obj_weapon *weapon = get_weapon(i);
		if (weapon) {
			weapon->setReadyness( 1.0 );
			weapon->setdirty();
		}
	}

}
	
//-----------------------------------------------------------------------------
void Obj_player_data::init(Net_joinrequest::player_info *request,
			   Weapon_style weapon_style, const int *gameweapons)
{
	//This object is its own owner
	ownerid = objectid;

	// player name,color
	strcpy(name,request->name);
	color = request->color;

	if(weapon_style==ws_rand) {
		// random weapons
	  for(int i=0; i<WEAPON_SLOTS; ++i)
	    set_weapon(i, (Object_type) rnd(ot_weapon_first, ot_weapon_last));
	} else {
		// user defined settings
	  for(int i=0; i<WEAPON_SLOTS; ++i)
	    set_weapon(i, (Object_type) gameweapons[i]);
	}
	
	team = request->team;
	if(world->getServerSettings()->getGameMode() == Obj_settings::gm_teamplay) {
		// check team settings
		if((team<0)||(team>3)) {
			logmsg(lt_warning,"Invalid team setting %d, assigning random team id", team);
			team = rnd(0,3);
		}
		color = mkrndcolor(teamColorsMin[team], teamColorsMax[team]);
	}

	//spawn us a new player:
	respawn();
}

//-----------------------------------------------------------------------------
void Obj_player_data::server_think()
{
	Obj_player_avatar *avatar = (Obj_player_avatar *)world->get_object(avatar_objectid,ot_player_avatar);
	
	if (!avatar) {
  	//The avatar has died, respawn
		respawn();
	} else {
  	//ooh we are still alive, lets hope we stay that way...
	}
}

//-----------------------------------------------------------------------------
void Obj_player_data::client_think()
{
}

//-----------------------------------------------------------------------------
Graphic *Obj_player_data::get_current_frame(char sequence)
{
	if (!animator) {
		animator = world->getLoader()->animations.new_animator("gfx/skins/ripped/worm/skin.script",color);
	}

	if (current_sequence != sequence) {
		switch (sequence) {
			case SEQUENCE::INIT: animator->start_sequence("init"); break;
			case SEQUENCE::STOP: animator->start_sequence("stop"); break;
			case SEQUENCE::WALK: animator->start_sequence("walk"); break;
			case SEQUENCE::JUMP: animator->start_sequence("jump"); break;
			case SEQUENCE::LAND: animator->start_sequence("land"); break;
			default: {
				int i=sequence;
				logmsg(lt_error, "Invalid sequence code passed to Obj_player_data::get_current_frame(): %i", i);
				sequence = -1;
			}
		}
		if (sequence >= 0) current_sequence = sequence;
	}

	Graphic *gfx = animator->get_current_frame(
		world->gettime(),
		controls.get_elevation(),
		controls.get_flag(Controls::cf_direction_east)?1:-1
	);

  if (!gfx) {
		animator->start_sequence("init");
		gfx = animator->get_current_frame(
			world->gettime(),
			controls.get_elevation(),
			controls.get_flag(Controls::cf_direction_east)?1:-1
		);	
	}

	return gfx;
}






