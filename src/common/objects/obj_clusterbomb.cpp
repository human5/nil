/***************************************************************************
  obj_clusterbomb.cpp  -  a clusterbomb
 ---------------------------------------------------------------------------
  begin                : 2000-02-15
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2000 by Anders Rosendal
  email                : qute@klog.dk

 ***************************************************************************/

#include "common/objects/obj_explosion.h"
#include "common/objects/obj_clusterbomb.h"
#include "common/objects/obj_player_data.h"
#include "common/objects/obj_grenade.h"
#include "common/console/logmsg.h"

Obj_clusterbomb::Obj_clusterbomb(World *world_, int id) :Obj_base(world_,id)
{
	for(int i=0; i<4; i++)
		sprite[i] = 0;
	last_frame_shift=0;
	frame=0;
}

Obj_clusterbomb::~Obj_clusterbomb() {}

void Obj_clusterbomb::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, int count_, float damage_, float speed_) {
	xpos=xpos_;
	ypos=ypos_;
	xspeed=xspeed_;
	yspeed=yspeed_;
	ownerid=owner;
	createtime=world->gettime();
	count=count_;
	damage=damage_;
	speed=speed_;
	timer=3;
	frame=0;

	// init mLeaveOwerID to prevent the rocket from hitting its owner immediatly
	Obj_player_data *player_data = (Obj_player_data *)world->get_object(ownerid,ot_player_data);
	if(player_data) {
		mLeaveOwnerID = player_data->getavatarid();
	} else {
		mLeaveOwnerID = -1; // this should not happen
	}
}

void Obj_clusterbomb::draw(Viewport *viewport) {
	int x = ROUND(xpos)-viewport->xpos;
	int y = ROUND(ypos)-viewport->ypos;
	viewport->setRawPixelCheck(x,y,PIXEL(255,255,255) | PF_TYPE_EARTH);
	return;

	// FIXME: why is this not used??
	if(age()>last_frame_shift+0.5) {
		last_frame_shift=age();
		frame=(frame+1)%4;
		
	}
	
	
	if (!sprite[frame]) {
		char s[100]; //STATIC_STRING
		sprintf(s,"gfx/obj/clusterbomb/clusterbomb%i.rgbf",frame);
		sprite[frame] = world->getLoader()->graphics.getgraphic(s);
		if (!sprite[frame]) {
			logmsg(lt_warning,"couldn't load %s",s);
			return;
		}
	}
	
	sprite[frame]->draw_view(viewport,x,y);
}

void Obj_clusterbomb::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
	serializer->rw(last_frame_shift);
	serializer->rw(frame);
	serializer->rw(timer);
	serializer->rw(count);
	serializer->rw(damage);
	serializer->rw(speed);
}

void Obj_clusterbomb::server_think() {
	Serverworld *serverworld = (Serverworld *)world;
	
	Map_hit hit = update_simple(true); // this calls client_think() for us and checks for collisions
	
	if (hit.hittype != ht_none) {
		net_dirty = true;
		bogo_bounce();
	}
	
	if(age()>timer) {
		done=true;
		float x,y;
		getpos(x,y);
		for(float angle =0; angle<2*PI; angle+=2*PI/(count+1)) {
			float bullet_angle = angle;
			Vector bullet_vector = Math::rad2vector(bullet_angle);
			bullet_vector.scale(speed);
			
			Obj_grenade *grenade = (Obj_grenade *)serverworld->newobject(ot_grenade);
			if (grenade) {
				grenade->init(
					ownerid,
					x,y,
					xspeed+bullet_vector.x,
					yspeed+bullet_vector.y,
					damage,
					12,
					2.2+rnd(-200,200)/1000.0
				);
			}
		}
		Obj_explosion *explosion = (Obj_explosion *)serverworld->newobject(ot_explosion);
		if (explosion) {
			explosion->init(
				ownerid,
				ROUND(xpos),
				ROUND(ypos),
				8,
				75
			);
		}
	}
	
}
