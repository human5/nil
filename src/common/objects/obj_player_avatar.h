/***************************************************************************
  obj_player.h  -  Header for the player
 ---------------------------------------------------------------------------
  begin                : 1999-10-30
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/


#ifndef OBJ_PLAYER_H
#define OBJ_PLAYER_H

#include "client/controls/controls.h"
#include "common/objects/obj_base.h"

#define DEFAULT_WALKSPEED 60.0    // walking, jump speed, def 30.0

class Obj_player_data;

//! @author Flemming Frandsen
//! @author Class for handeling the players avatar
class Obj_player_avatar : public Obj_base  {
public:
	//! Constructor
	Obj_player_avatar(World *world_, int id);
	//! Destructor
	virtual ~Obj_player_avatar();
	/*! Initialize (set some default values and play a sound)
	    @param owner The owner of this player avatar
	    @param x The x coordinate of this avatar
	    @param y The y coordinate of this avatar */
	void init(int owner, int x, int y);
	/*! returns ot_player_avatar as type
	    @return Returns player avatar as object type */
	virtual Object_type gettype() { return ot_player_avatar; }
	//! ???
	virtual void server_think();
	/*! ???
	    @param viewport The viewport that will be drawn to */
	virtual void draw(Viewport *viewport);
	/*! ???
	    @param serializer The serializer doing the serialization work needed */
	virtual void serialize(Serializer *serializer);
	//! handle digging and bleeding (calls newton update)
	virtual void client_think();
	/*! Hurt the avatar
	    @param hitpoints The value of hitpoints that a player will loose
	    @param speed ???
	    @param hurter ??? */
	virtual void hurt(float hitpoints, Vector speed, int hurter);
	//! players are only hurtable when they are alive...
	virtual bool hurtable();
	/*! Heal the avatar (This could be done via "hurt" with a negative "hitpoints"
	    value, but I feel that that's asking for trouble.
	    @param hitpoints The value of hitpoints that should be healed */
	void heal(float hitpoints);
	/*! Hide the avatar for the given time
	    @param time The time */
	void cloak(float time) { invisible_time += time; }
	/*! Get the visible status of the player
	    @return Returns true if the the avatar is currenty visible */
	bool is_visible() { return invisible_time <= 0; }
	/*! collision function, called redefined from obj_base::is_passable
	    @param x The x coordinate of the position
	    @param y The y coordinate of the position */
	virtual bool is_passable(int x, int y);
	//! state of the players avatar
	typedef enum State {
		//! before spawning the avatar
		PRESPAWNING,
		//! avatar spawning
		SPAWNING,
		//! avatar's default status while playing
		LIVING,
		//! avatar is dying
		DYING
	} TD_state;
	/*! Get the state of this avatar
	    @return The state of the avatar*/
	State getstate() { return (State)state; }
	/*! Get the avatars health
	    @return The health of the avatar*/
	float gethealth() { return health; }
	/*! Get the complete player data
	    @return The player data */
        Obj_player_data* get_player_data() { return player_data; }

protected:
	//! fire the hook (create new hook for the player)
	void hook_fire();
	//! release the hook (delete hook if it exists)
	void hook_release();
	//! shorten the rope (get the hook object with hook_id from world and work with it)
	void hook_in();
	//! lengthen the rope (get the hook object with hook_id from world and work with it)
	void hook_out();
	//! use the jetpack
	void jetpack();
	//! ???
	void setup_constants();
	//! ???
	float elevation;
	//! walking direction
	int direction;
	//! ???
	Controls *controls;
	//! ???
	Obj_player_data *player_data;
	//! The players hook object ID
	int hook_id;
	//! ???
	int hurter_id;
	//! player dying, respawning etc...
	Sint8 state;
	// Graphic *sprite[3][17];
	//! ???
	Sint8 animation_sequence;
	//! ???
	Sint8 cur_walk_frame;	
	//! ???
	Sint8 dig_now;
	//! hitpoints of the player
	float health;
	//! remember time of death for respawn delay
	float deathtime;
	//! hitpoints accumulated in this frame
	float damage;
	//! ???
	Vector damage_speed;
	//! ???
	float vaporize;
	//! time left to be invisible
	float invisible_time;
};

#endif
