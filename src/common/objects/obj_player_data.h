/***************************************************************************
  obj_player_data.h  -  Header for the player
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

#ifndef OBJ_PLAYER_DATA_H
#define OBJ_PLAYER_DATA_H

#include "client/animation/animator.h"
#include "client/controls/controls.h"
#include "common/objects/obj_base.h"
#include "common/objects/obj_weapon.h"
#include "common/console/logmsg.h"
#include "common/net/netpackets.h"

//! animation sequence constants for a player
namespace SEQUENCE {
	//! the player initilizes
	const char INIT=0;
	//! the player is stopped
	const char STOP=1;
	//! the player walks
	const char WALK=2;
	//! the player jumps
	const char JUMP=3;
	//! the player lands
	const char LAND=4;
	//! the player shoots
	const char SHOT=5; 
	//! a dummy state
	const char DUMMY=6;
};

//! @author Flemming Frandsen
//! @brief This class holds the cross-life data for players, handles respawning, stores weapons etc.
class Obj_player_data : public Obj_base  {
public: 
	//! returns the ot_player_data types
	virtual Object_type gettype() {return ot_player_data;}
	/*! ???
	    @param world_ ???
	    @param id ??? */
	Obj_player_data(World *world_, int id);
	//! ???
	virtual ~Obj_player_data();
	//! ???
	virtual void serialize(Serializer *serializer);
	//! this is where we handle respawning and stuff.
	virtual void server_think();
	//! ???
	void init(Net_joinrequest::player_info *joinrequest, Weapon_style weapon_style, const int *default_weapons);
	//! This object is purely for storing data, so the visual stuff doesn't apply:
	virtual void draw(Viewport *viewport){};
	//! ???
	virtual void client_think();
	//! ???
	Controls controls;
	//! ???
	int getavatarid() {return avatar_objectid;}
	//! ???
	int32 getcolor() {return color;}
	//! set weapon for a specific slot
	void set_weapon(int slot,Object_type object_type);
	//! returns weapon of the slot
	Obj_weapon *get_weapon(int slot);
	//! ???
	Obj_player_avatar *get_player_avatar() {return (Obj_player_avatar *)world->get_object(avatar_objectid,ot_player_avatar);}
	//! ???
	Graphic *get_current_frame(char sequence);
	//! returns player name string
	char *get_player_name() { return name; }
	//! returns team id
	Sint8 getTeam() { return team; }

protected:
	//! current animation frame
	char current_sequence;
	//! place player into map
	void respawn();
	//! ???
	Animator *animator;
	//! ???
	bool anim_walking;

	// synced data, start -----
	//! name of this player
	Playername name;
	//! player color
	int32 color;
	//! team id
	Sint8 team;
	//! avator object id
	int avatar_objectid;
	//! ids for wepaons
	int weapons[WEAPON_SLOTS];
	//synced data, end -----

};

#endif
