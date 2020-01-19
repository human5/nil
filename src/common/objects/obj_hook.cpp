/***************************************************************************
  obj_hook.cpp  -  a hook
 ---------------------------------------------------------------------------
  begin                : 1999-12-23
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_hook.h"
#include "common/objects/obj_player_avatar.h"
#include "common/sound/soundsystem.h"
#include "common/console/logmsg.h"

Obj_hook::Obj_hook(World *world_, int id) : Obj_base(world_,id) {
	air_resistance = DEFAULT_DRAG;
	rope_length = -1;
}

Obj_hook::~Obj_hook() {}

void Obj_hook::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
	serializer->rw(stuck);
	serializer->rw(avatar_id);
	serializer->rw(rope_length);
}

void Obj_hook::draw(Viewport *viewport) {
	int x = ROUND(xpos)-viewport->xpos;
	int y = ROUND(ypos)-viewport->ypos;

	//Draw the rope:
	Obj_player_avatar *avatar = (Obj_player_avatar *)world->get_object(avatar_id,ot_player_avatar);
	if (avatar) {
		float ax,ay;
		avatar->getpos(ax,ay);
		viewport->draw_line(x, y,
		                    ROUND(ax)-viewport->xpos, ROUND(ay-4)-viewport->ypos,
		                    PIXEL(150,128,100),
		                    Raw_surface::NOBLEND);
	}	

	//Draw the hook:
	//if (x >= 0 && y >= 0 && x < viewport->get_xsize() && y < viewport->get_ysize()) {
		//*(viewport->get_pixels()+x+y*viewport->get_xsize()) = PIXEL(255,255,0);
	//}
	viewport->setRawPixelCheck(x,y,PIXEL(255,255,0));
}

void Obj_hook::server_think() {
	// Serverworld *serverworld = (Serverworld *)world;
	Obj_player_avatar *avatar = (Obj_player_avatar *)world->get_object(avatar_id,ot_player_avatar);
	if (!avatar) {
		bedone();
		return;
	}
	const float dt = world->gettime()-curtime;  // time step
	float ax,ay;
	// prepare push rope vector
	avatar->getpos(ax,ay);
	Vector rope_vec (xpos-ax, ypos-ay);

	float current_length = rope_vec.length();

	if (stuck) {
		//check to see if the hook has come loose
		if (is_passable(ROUND(xpos),ROUND(ypos))) {
			// FIXME: something strange happens when we do this, Tom
			yaccel = DEFAULT_ACCEL;
			xspeed = yspeed = 0.0;
			stuck = false;
			setdirty();
			logmsg(lt_debug, "rope is loose again");
			//bedone();
			return;
		}

		//Pull on the owner avatar
		if (avatar && current_length > rope_length) {
			rope_vec.scale(-0.2*DEFAULT_ACCEL*dt*(rope_length-current_length)/current_length);
			avatar->push(rope_vec);
		}
		curtime = world->gettime(); // this is usually done by newton_update
	} else {
		//pull towards owner a little bit
		//rope_vec.scale(-0.1); // Tom??
		//push(rope_vec);       // Tom??
		
		//see if we have hit something:
		Map_hit hit = update_simple(); //this calls client_think() for us and checks for collisions

		if (hit.hittype != ht_none) {
			if (rope_length == -1) rope_length = 0.7*current_length;
			logmsg(lt_debug, "rope is sticking (rope_length: %.0f)", rope_length);
			stuck = true;
			//find a place to position the hook securely
			Vector speed (old_xspeed, old_yspeed);
			speed.normalize();
			Vector speedOffset;
			for (float testOffset = 1.0; testOffset < 6.0; testOffset += 1.0) {
				speedOffset = speed;
				speedOffset.scale(testOffset);
				if ( ! is_passable(hit.xc+ROUND(speedOffset.x), hit.yc+ROUND(speedOffset.y))) break;
			}

			xpos = hit.xc+ROUND(speedOffset.x);
			ypos = hit.yc+ROUND(speedOffset.y);
			logmsg(lt_debug, "rope position (%.0f, %.0f)", xpos, ypos);

			xspeed = 0;
			yspeed = 0;
			yaccel = 0; //dont fall.

			setdirty();
		}
	}
}

void Obj_hook::init(int owner,float xpos_, float ypos_, float xspeed_, float yspeed_) {
	ownerid = owner;
	xpos = xpos_;
	ypos = ypos_;
	xspeed = xspeed_;
	yspeed = yspeed_;
	stuck = false;	
	Obj_player_avatar *avatar = get_owner_avatar();
	if (avatar) avatar_id = avatar->getid();

	setdirty();
	//FIXME: play a usefull sound when launching a hook
	//FIXME: should this really happen here?
	//world->soundsystem->start_sound("sound/tarzan.wav",0,1,objectid);
}

void Obj_hook::reel_out() {
	if (stuck) {
		rope_length += 4;
		if (rope_length > ROPE_MAX_LENGTH)
			rope_length = ROPE_MAX_LENGTH;
		setdirty();
		logmsg(lt_debug,"rope: %.0f", rope_length);
	}
}

void Obj_hook::reel_in() {
	if (stuck) {
		rope_length -= 4;
		if (rope_length < 1)
			rope_length = 1;
		setdirty();
		logmsg(lt_debug,"rope: %.0f", rope_length);
	}
}
