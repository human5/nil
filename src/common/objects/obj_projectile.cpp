/***************************************************************************
  obj_projectile.cpp  -  a projectile
 ---------------------------------------------------------------------------
  begin                : 1999-12-19
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "obj_projectile.h"
#include "obj_explosion.h"
#include "obj_player_data.h"

Obj_projectile::Obj_projectile(World *world_, int id) : Obj_base(world_,id) {}

Obj_projectile::~Obj_projectile() {}

void Obj_projectile::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int explosion_size_, int penetration_, float init_age) {
	ownerid = owner;
	xpos = xpos_;
	ypos = ypos_;
	xspeed = xspeed_;
	yspeed = yspeed_;
	explosion_damage = explosion_damage_;
	explosion_size = explosion_size_;
	penetration = penetration_;
	air_resistance = 0;
	yaccel = DEFAULT_ACCEL/5;

	// init object time, assume it was created at the begining of the last frame,
	// this is needed for the first call to newton_update
	//curtime = world->gettime()-init_age;
	curtime = world->gettime() - world->get_last_frame_delta()-init_age;
	
	// init mLeaveOwerID to prevent the rocket from hitting its owner immediatly
	Obj_player_data *player_data = (Obj_player_data *)world->get_object(ownerid,ot_player_data);
	if(player_data) {
		mLeaveOwnerID = player_data->getavatarid();
	} else {
		mLeaveOwnerID = -1; // this should not happen
	}
}

void Obj_projectile::draw(Viewport *viewport) {
	int x = ROUND(xpos)-viewport->xpos;
	int y = ROUND(ypos)-viewport->ypos;
	//viewport->draw_pixel_opaque(x,y,PIXEL(255,255,255) | PF_TYPE_EARTH);
	viewport->setRawPixelCheck(x,y,PIXEL(255,255,255) | PF_TYPE_EARTH);
}

void Obj_projectile::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
	serializer->rw(mLeaveOwnerID);
}

void Obj_projectile::server_think() {
	Serverworld *serverworld = (Serverworld *)world;
	Map_hit hit = update_simple(true); //this calls client_think() for us and checks for collisions

	if (hit.hittype != ht_none) {
		
		//do the explosion
		{
			Vector speed;
			speed.x = old_xspeed;
			speed.y = old_yspeed;
			speed.normalize();
			speed.scale(penetration);

			Obj_explosion *explosion = (Obj_explosion *)serverworld->newobject(ot_explosion);
			if (explosion) {
				explosion->init(
					ownerid,
					(int)(hit.xc+speed.x+1),
					(int)(hit.yc+speed.y),
					explosion_size,
					(int)explosion_damage
				);
			}
		}

		if (hit.hittype == ht_object) {
			Vector speed;
			speed.x = old_xspeed;
			speed.y = old_yspeed;
			Obj_base *victim = world->get_object(hit.objectid,ot_error);
			if (victim) {
				victim->hurt(explosion_damage,speed,ownerid);
			}
		}
		// FIXME: penetration no longer needed??
		/*
		penetration--;
		if (penetration < 0)*/
			done=true;
	}
}
