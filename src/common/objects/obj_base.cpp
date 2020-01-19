/***************************************************************************
  obj_base.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-08
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-05-08
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_base.h"
#include "common/misc.h"
#include "common/world.h"
#include "common/nil_math.h"
#include "common/systemheaders.h"
#include "common/objects/obj_player_data.h"
#include "common/console/logmsg.h"

void Obj_base::newton_update(float newtime) {
	if (curtime == -1) {
		//This shouldn't happen, if it does then it means that the object was not initialized
		curtime = newtime; 
		createtime = newtime;
		return;
	}

	//if (world->isclient()) 
	{
		//save the old physical state for later
		old_xpos   = xpos;
		old_ypos   = ypos;
		old_xspeed = xspeed;
		old_yspeed = yspeed;
	}
	
	float delta = newtime - curtime;

	//speed
	if (air_resistance > 0.0) {
		xspeed = xspeed - SQR(xspeed) * SIGN(xspeed) * air_resistance * delta;
		yspeed = yspeed - SQR(yspeed) * SIGN(yspeed) * air_resistance * delta;
	}
	xspeed += xaccel * delta;
	yspeed += yaccel * delta;

	//position
	xpos = xpos + xspeed * delta;
	ypos = ypos + yspeed * delta;

	//time
	curtime = newtime;
}


/*! does standard movement, usually calls client_think (on server). can now also be used by non-server
    objects, be careful: when these objects are called on a server the recursion wont stop (client_think
    calls update_simple which again calls client_think), is e.g. used by gib objects */
Map_hit Obj_base::update_simple(bool check_object_collision) {
	Map_hit map_hit;
	map_hit.hittype = ht_none; //if nothing happens we didn't hit anything

	//save the old physical state for later
	old_xpos   = xpos;
	old_ypos   = ypos;
	old_xspeed = xspeed;
	old_yspeed = yspeed;
	
	// update position and stuff like that, usually calls newton_update
	if(!world->isclient()) {
		// only on server
		client_think();
	} else {
		// directly do a newton update otherwise
		newton_update(world->gettime());
	}

	//check to see if we have hit a pixel
	int xc=0;
	int yc=0;
	bool collided = false;
	if (radius > 0) {
		collided = world->collision(ROUND(old_xpos),ROUND(old_ypos), ROUND(xpos), ROUND(ypos), xc, yc, radius);
	} else {
		collided = world->collision(ROUND(old_xpos),ROUND(old_ypos), ROUND(xpos), ROUND(ypos), xc, yc, this);
	}

 	if (collided) {
		map_hit.hittype = ht_map; //we hit a pixel on the map
		map_hit.xc = xc;
		map_hit.yc = yc;	
		// we hit something, set non-colliding position
		xpos = xc;
		ypos = yc;
	};

	// check collision with world boundaries
	if (ROUND(xpos) < 0) {
		map_hit.hittype = ht_left;

		ypos = old_ypos;// TODO: calc edge intersection in stead
		xpos = 0;

		map_hit.xc = ROUND(xpos);
		map_hit.yc = ROUND(ypos);	
	}

	if (ROUND(ypos) < 0) {
		map_hit.hittype = ht_top;

		ypos = 0;
		xpos = old_xpos; // TODO: calc edge intersection in stead

		map_hit.xc = ROUND(xpos);
		map_hit.yc = ROUND(ypos);	
	}

	if (ROUND(xpos) >= world->getSizeX()) {
		map_hit.hittype = ht_right;

		ypos = old_ypos; // TODO: calc edge intersection in stead
		xpos = world->getSizeX()-1;

		map_hit.xc = ROUND(xpos);
		map_hit.yc = ROUND(ypos);	
	}

	if (ROUND(ypos) >= world->getSizeY()) {
		map_hit.hittype = ht_bottom;

		xpos = old_xpos; // TODO: calc edge intersection in stead
		ypos = world->getSizeY()-1;

		map_hit.xc = ROUND(xpos);
		map_hit.yc = ROUND(ypos);	
	}

	//check to see if we have hit any hurtable objects
	if (check_object_collision) {
		
		int lastLOID = mLeaveOwnerID;
		if (((Serverworld*)world)->hit_object(
				old_xpos, old_ypos, xpos, ypos,
				map_hit.xc, map_hit.yc, map_hit.objectid, mLeaveOwnerID)) {

			map_hit.hittype = ht_object;
			xpos = map_hit.xc;
			ypos = map_hit.yc;
		}
		
		// did mLeaveOwnerID change?
		if(lastLOID != mLeaveOwnerID) { setdirty(); }
	}

	//sanity check: for world boundaries
	if (ROUND(xpos) >= world->getSizeX())
		xpos = world->getSizeX()-1;
		
	if (ROUND(ypos) >= world->getSizeY())
			ypos = world->getSizeY()-1;

	if (ROUND(ypos) < 0)
		ypos = 0;

	if (ROUND(xpos) < 0)
		xpos = 0;

	return map_hit;
}

//! standard object init
Obj_base::Obj_base(World *world_, int id) {
	world = world_;
	done = false;
	net_dirty = true;

	objectid = id;

	xpos   = 0;
	ypos   = 0;
	xspeed = 0;
	yspeed = 0;
	xaccel = 0;
	yaccel = DEFAULT_ACCEL;
	air_resistance = DEFAULT_DRAG;

	// the time this data was updated.
	curtime = -1;

	ownerid = -1;
	mLeaveOwnerID = -1;

	radius = 1;

	createtime = world->gettime();
}

Obj_base::~Obj_base() {
//	logmsg(lt_debug,"deleted Obj_base");
}

void Obj_base::serialize(Serializer *serializer) {
	serializer->rw(ownerid);
	serializer->rw(xaccel);
	serializer->rw(yaccel);
	serializer->rw(air_resistance);
	serializer->rw(createtime);
	serializer->rw(curtime);
	serializer->rw(xpos);
	serializer->rw(ypos);
	serializer->rw(xspeed);
	serializer->rw(yspeed);
}

void Obj_base::client_think() {
	newton_update(world->gettime());
}

void Obj_base::bogo_bounce() {
	bool stopIt = true; // completely stop?

	// old pos is often the same anyway
	xpos = old_xpos;
	ypos = old_ypos;

	// assume last frame delta, TODO: better use real changed positions?
	float delta = world->get_last_frame_delta();
	float check_xpos = xpos+xspeed*delta;
	float check_ypos = ypos+yspeed*delta;

	// if we hit anything, come back
	//bounce if we are moving very fast, otherwise just sit still.
	if (ABS(xspeed) > 15 || ABS(yspeed) > 15) {
		// check each direction seperately
		// TODO: check for boundaries of world -> otherwise is_passable can segfault

		if (ROUND(check_xpos) >= world->getSizeX())
			check_xpos = world->getSizeX()-1;

		if (ROUND(check_ypos) >= world->getSizeY())
			check_ypos = world->getSizeY()-1;

		if (ROUND(check_ypos) < 0)
			check_ypos = 0;

		if (ROUND(xpos) < 0)
			check_xpos = 0;

		bool checkx = is_passable( ROUND(check_xpos), ROUND(ypos) );
		bool checky = is_passable( ROUND(xpos), ROUND(check_ypos) );

		// nothing to do?
		if((checkx)&&(checky)) {
			// try to check for diagonal bounce
			if(!is_passable( ROUND(check_xpos), ROUND(check_ypos) ) ) {
				xspeed *= -0.4;
				yspeed *= -0.4;
				stopIt = false;
			} else {
				// no bounce???
			}
		}

		// hit in x direction
		if(!checkx) {
			xspeed *= -0.4;
			yspeed *=  0.8; // slow down a bit, too
			stopIt = false;
		}

		// hit in y direction
		if(!checky) {
			yspeed *= -0.4;
			xspeed *=  0.8; // slow down a bit, too
			stopIt = false;
		}
	} 
	
	if(stopIt){
		// we're too slow, dont bounce just stop any motion
		xspeed = 0;
		yspeed = 0;
	}
}

bool Obj_base::canmove() {
	//return world->canmove(ROUND(xpos),ROUND(ypos));
	return true;
}

bool Obj_base::onground() {
	//return !IS_PASSABLE(world->map_buffer.get_pixels_read()[ROUND(xpos)+(ROUND(ypos)+1)*world->map_buffer.get_xsize()]);
	return  ! world->isPassable( ROUND(xpos) , ROUND(ypos)+1 );
}

//! Used for collisiondetection, alter it if you need to have another shape than a point.
bool Obj_base::is_passable(int x, int y) { 
	// for world boundaries, to prevent segfaults
	/*if (x >= world->getSizeX()) return false;
	if (y >= world->getSizeY()) return false;
	if (x < 0) return false;
	if (y < 0) return false;

	int offset = x+y*world->map_buffer.get_xsize(); 
	return IS_PASSABLE(world->map_buffer.get_pixels_read()[offset]); */
	return world->isPassable(x,y);
}

Obj_player_data *Obj_base::get_owner_data() {
	return (Obj_player_data *)world->get_object(ownerid,ot_player_data);
}

Obj_player_avatar *Obj_base::get_owner_avatar() {
	Obj_player_data * od = get_owner_data();
	if (od)
		return od->get_player_avatar();
	else
		return NULL;
}
