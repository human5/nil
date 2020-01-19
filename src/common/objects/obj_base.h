/***************************************************************************
  obj_base.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-08
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

    copyright            : (C) 1999 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

#ifndef BASEOBJECT_H
#define BASEOBJECT_H

class World;
class Obj_player_avatar;
class Obj_player_data;

#include "common/misc.h"
#include "common/nil_math.h"
#include "client/graphics/viewport.h"
#include "common/world.h"
#include "server/serverworld.h"
#include "common/serializer/serializable.h"

//! Kinds of hit
typedef enum Hit_type {
	//! didn't hit anything
	ht_none,
	//! hit an object
	ht_object,
	//! we hit a pixel on the map
	ht_map,
	//! hit left side of the map
	ht_left,
	//! hit top side of the map
	ht_top,
	//! hit right side of the map
	ht_right,
	//! hit bottom side of the map
	ht_bottom
} TD_hit_type;

//! store hit (position, type)
typedef struct Map_hit {
	//! the hit type
	Hit_type hittype;
	//! x-coords
	int xc;
	//! y-coords
	int yc;
	//! the object id
	int objectid;
} TD_map_hit;


//! @author Flemming Frandsen
//! @brief Base object, all objects that are to be drawn must inherit from this class
class Obj_base : public Serializable {
public:
	Obj_base(World *world_, int id);
	virtual ~Obj_base();

/**********************************************************************
 *                  *Start of MANDATORY functions*
 *      These fuctions MUST be implemented by ALL clild objects
 **********************************************************************/

	//! Must return the type of the concrete class
	virtual Object_type gettype() = 0;
	/*! Does the thinking needed for the object on the serverside.
	    Thinking on server: checking for collisions, becoming an explosion
	                        and hurting players in range of explosion, calls
	                        client_think() for updating of animation the object
	                        and applying physics */
	virtual void server_think() = 0;
	//! Draws the object into the viewport. This routine should handle clipping on its own.
	virtual void draw(Viewport *viewport) = 0;

/**********************************************************************
 *                   *End of MANDATORY functions*
 **********************************************************************/

	/*! called to read or write the state of the object
	    @param serializer The serializer doing the work */
	virtual void serialize(Serializer *serializer);
	/*! Does the thinking needed for the object on the client side (that is completely non-random)
	    Thinking means updating the physics and other deterministic things, such as animations.
	    MUST NOT alter other objects, this must be handled by server_think() */
	virtual void client_think();
	/*! return true if this object can be hurt, if it can it should also implement hurt()
	    Do you really want to hurt me? */
	virtual bool hurtable() { return false; }
	/*! called when something hurts the object, the default is to die if it is hurt at all, this is not
	    very practical for most objects:) */
	virtual void hurt(float hitpoints, Vector speed, int hurter) { if (hitpoints > 0) done = true; }
	/*! Used for collisiondetection, alter it if you need to have another shape than a point.
	    needs to check for sanity of x and y, replaces old check with only offset */
	virtual bool is_passable(int x, int y);
	//collision helper:
	//virtual bool is_passable(int offset) {return IS_PASSABLE(world->map_buffer.get_pixels_read()[offset]);}
	//! Be done, the object should make peace with it's maker and set the done flag...
	virtual void bedone() { done = true; }
	/*! Returns true once the object wants to die.
	    In the server this means that the object must be delete'ed and that the clients should be told
	    to do the same. */
	virtual bool isdone() { return done; }
	//! Misc access functions:
	/*! The objectid uniquely indentifies all objects in the system, pointers should only be used
	    inside a function as they cannot cross from the server to the client as the id's can and
	    pointers have a way of leading to madness once the target object dies (something that happens
	    quite often in this program) */
	int getid() { return objectid; };
	//! used while saving/sending state:
	bool isdirty() { return net_dirty; }
	//! used to remove the dirty-flag
	void beclean() { net_dirty = false;}
	//! used to set the dirty-flag
	void setdirty() { net_dirty = true; }
	//! return position
	void getpos(float &x, float &y) { x = xpos; y=ypos; }
	//! return speed
	void getspeed(float &xspeed_, float &yspeed_) { xspeed_=xspeed; yspeed_=yspeed; }
	//! add vector to speed of object
	void push(Vector &speed) { xspeed += speed.x; yspeed += speed.y; setdirty(); }
	//! Getting different kinds of "owners":
	int getowner() { return ownerid; }
	//! ???
	Obj_player_data *get_owner_data();
	//! ???
	Obj_player_avatar *get_owner_avatar();
	//! An easy way to get the name of an object
	const char *getname() { return OBJECT_NAMES[gettype()]; }
protected:
	//! Makes the object do a bogus bounce (not a bounce at all, it's an imitation)
	void bogo_bounce();
	//! Call from server_think to call client_think and check for collisions.
	Map_hit update_simple(bool check_object_collision = false);
	//! returns if an object can be moved
	bool canmove();
	//! returns it an object is on the ground
	bool onground();
	//! world in which the object lives (may be client or server)
	World *world;
	//! this object has had it's state updated in such a way that it should now be serialized and sent to the client
	bool net_dirty;
	//! delete object?
	bool done;
	//! id of this object, assigned by the server at creation
	int objectid;	
	
	//intenal helpers:
	//! updates the common physical data according to Newtons laws (although me and hawking know that they are bollox)
	void newton_update(float newtime);
	//! return age of object
	float age() {return world->gettime()-createtime;}

	///////////////////////////
	// Syncronized data Start//
	///////////////////////////
	//! the x position
	float xpos;
	//! the y position
	float ypos;
	//! the x speed
	float xspeed;
	//! the y speed
	float yspeed;
	//! The added acceleration in x-direction
	float xaccel;
	//! the y acceleration
	float yaccel;
	//! This works against xspeed and yspeed, and also leads to a terminal speed when falling
	float air_resistance;
	//! the time the object was created
	float createtime;
	//! the time this data was last updated
	float curtime;
	//! the player id of the owner of the object (there isn't always one)
	int ownerid;
	//! used by the collision routines <= 0 means that it should use is_passable() to test 
	int radius;

	///////////////////////////
	// Syncronized data end  //
	///////////////////////////
	/*! optional for synchronization, used for bullet etc., default = -1 = unused, if used it
	    is needed in update_simple checks for hits with owner, while object is still on
	    owner, hits are not counted, until the object once left its owner - useful for things
	    like bullets */
	int mLeaveOwnerID;
	// non-synced data, that is simply good to have around on the server (old pos,accel)...
	//! The old x position
	float old_xpos;
	//! The old y position
	float old_ypos;
	//! The old x speed
	float old_xspeed;
	//! The old y speed
	float old_yspeed;
};

#endif
