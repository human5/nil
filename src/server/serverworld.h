/***************************************************************************
  serverworld.h  -  Header for NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 1999-10-20
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-07
  by                   : Nils Thuerey
  email                : n_t@gmx.de

    copyright            : (C) 1999 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/


#ifndef SERVERWORLD_H
#define SERVERWORLD_H

#include "common/world.h"
#include "common/systemheaders.h"
#include "common/net/netpackets.h"
#include "common/serializer/serializer_reader.h"
#include "common/serializer/update_queue.h"
#include "common/objects/obj_base.h"
#include "common/console/logmsg.h"

//! @author Flemming Frandsen
//! @brief Class for the serversided part of the world
class Serverworld : public World  {
public: 
	//! Constructor
	Serverworld(Loader *loader_, Soundsystem *soundsystem_, bool always_reload_, Weapon_style weaponstyle_, const int *gameweapons_);
	//! Destructor
	virtual ~Serverworld();
	//! distuingish client and server programs
	virtual bool isclient() { return false; }
	/*! Called by the server to do all the thinking that needs to be done. This will clear
	    and then set the thoughts that need to be sent to the clients.. Sends a quit message
	    when sendQuit is true
	    @param update_queue ???
	    @param sendQuit ??? */
	void server_think(Update_queue *update_queue, bool sendQuit);
	/*! create a server objects (all the not-anonymous ones)
	    @param objecttype ???
	    @return ??? */
	Obj_base *newobject(Object_type objecttype);
	//! adds a seperately created object to the world array, used e.g. for Obj_settings, returns true on success
	bool addObject(Obj_base *newObject);
	//! make a hole in the world
	virtual void makehole(int x, int y, int diameter);
	//! make all objects dirty to transfer the via network (unused, was used for new player joining)
	void make_all_dirty();
	//! handle a new incoming join request 
	bool handle_join_request(Net_joinrequest &joinrequest, int &nplayers, Obj_player_data **new_players, Net_joinreply &joinreply, char *&map_data);
	//! save original map
	void save_original_map();
	//! length of the last reader
	int last_reader_length;
	//! Returns an ID of an arbitrary object of the given type within the given radius.  (-1 if nonesuch)
	int find_radius_object(float xpos, float ypos, float radius, Object_type type);
	//! damages all objects within a radius
	void do_radius_damage(int hurter_id, float xpos, float ypos, float radius, float damage);
	//! check for intersection with objects, return hit position via references, just calls function below without leave owner checks
	bool hit_object(float old_xpos, float old_ypos, float xpos, float ypos, int &cx, int &cy, int &id);
	//! same as above but additionally checks for intersection with owner of object (for bullets), pass -1 as leaveid to switch checks off
	bool hit_object(float old_xpos, float old_ypos, float xpos, float ypos, int &cx, int &cy, int &id, int &leaveid);

protected:
	//! kill some lines with lonely leftover earth bits...
	void kill_lonely_bits(Serializer_reader *serializer_reader);
	//! current kill lonely bits line
	int killer_line;
	//! ???
	int32 *original_map;
	//! ID of next object to create
	int next_free_objectid;
	//! what weapons, fixed, random ?
	Weapon_style weapon_style;
	//! the selected weapons
	int gameweapons[WEAPON_SLOTS];
};

#endif
