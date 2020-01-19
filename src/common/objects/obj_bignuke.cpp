/***************************************************************************
  obj_bignuke.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-15
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

    copyright            : (C) 2000 by Anders Rosendal
    email                : qute@klog.dk

 ***************************************************************************/

#include "common/objects/obj_bignuke.h"
#include "common/objects/obj_tinygranade.h"
#include "common/objects/obj_player_data.h"
#include "common/sound/soundsystem.h"

Obj_bignuke::Obj_bignuke(World *world_, int id) :Obj_base(world_,id) {
	sprite = NULL;
}

Obj_bignuke::~Obj_bignuke() {}

void Obj_bignuke::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, int count_, float power_) {
	xpos=xpos_;
	ypos=ypos_;
	xspeed=xspeed_;
	yspeed=yspeed_;
	ownerid=owner;
	createtime=world->gettime();
	count=count_;
	power=power_;
	timer=3;
	
	// init object time, this is needed for the first call to newton_update
	curtime = world->gettime() - world->get_last_frame_delta();
	
	// init mLeaveOwerID to prevent the rocket from hitting its owner immediatly
	Obj_player_data *player_data = (Obj_player_data *)world->get_object(ownerid,ot_player_data);
	if(player_data) {
		mLeaveOwnerID = player_data->getavatarid();
	} else {
		mLeaveOwnerID = -1; // this should not happen
	}
}

void Obj_bignuke::draw(Viewport *viewport) {
	int x = ROUND(xpos)-viewport->xpos;
	int y = ROUND(ypos)-viewport->ypos;
	
	if (!sprite) {
		sprite = world->getLoader()->graphics.getgraphic("gfx/obj/bignuke.rgbf");
		if (!sprite) return;
	}
	
	sprite->draw_view(viewport,x,y);
}

void Obj_bignuke::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
	serializer->rw(timer);
	serializer->rw(count);
	serializer->rw(power);
	serializer->rw(mLeaveOwnerID);
}

void Obj_bignuke::server_think() {
	float x,y;
	Serverworld *serverworld = (Serverworld *)world;
	
	Map_hit hit = update_simple(true); //this calls client_think() for us and checks for collisions
	
	if (hit.hittype != ht_none) {
		net_dirty = true;
		bogo_bounce();
	}
	
	if(age()>timer) {
		done=true;
	    getpos(x,y);
	
		world->startSound("sound/bgun.wav", 0, 1, objectid);
		for(float angle =0; angle<2*PI; angle+=2*PI/(count+1)) {
			float bullet_angle = angle;
			Vector bullet_vector = Math::rad2vector(bullet_angle);
			bullet_vector.scale(power);
			// knock_back_direction.add(bullet_vector);
	
//TODO: should the nuke make a spiral(like now) or a circle?
//TODO: make a array of vectors and choose(create) them random.
			
			Obj_tinygranade *bullet = (Obj_tinygranade *)serverworld->newobject(ot_tinygranade);
			if (bullet) {
					bullet->init(
					ownerid,
					x,y,
					xspeed+bullet_vector.x,
					yspeed+bullet_vector.y,
					40,
					8,
					0,
					42,
					true
				);
			}
		}
	}
}
