/***************************************************************************
  obj_grenade.cpp  -  a grenade
 ---------------------------------------------------------------------------
  begin                : 2000-02-13
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

    copyright            : (C) 2000 by Anders Rosendal
    email                : qute@klog.dk

 ***************************************************************************/

#include "server/serverworld.h"
#include "client/clientworld.h"
#include "common/objects/obj_grenade.h"
#include "common/objects/obj_explosion.h"
#include "common/console/logmsg.h"

//This is the actual place where we store the pointers to our graphics.
//all objects of this type share this:
Obj_grenade::Frames Obj_grenade::frames;

Obj_grenade::Obj_grenade(World *world_, int id) : Obj_base(world_,id)
{
	for(int i=0; i<40; i++)
		frames[i] = 0;
	
	float_time_hack=1;
	if (world->isclient()) {
		animator = world->getLoader()->animations.new_animator("gfx/obj/grenade/grenade.script", NOCOLOR);
	} else {
		animator = NULL;
	}
		
	if (!animator && world->isclient()) {
		logmsg(lt_debug,"No animator available");
		sleep(2);
	}
	
	if (animator) {
		animator->start_sequence("init");
//		animator->start_sequence("turn");
	}
}

Obj_grenade::~Obj_grenade() {}

//------------------------------------------------------------------
void Obj_grenade::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int explosion_size_, float timer_)
{
	ownerid = owner;
	xpos = xpos_;
	ypos = ypos_;
	xspeed = xspeed_;
	yspeed = yspeed_;
	air_resistance = 0;
	yaccel = DEFAULT_ACCEL/3;
	createtime=world->gettime();
	timer=timer_;
	explosion_damage = explosion_damage_;
	explosion_size = explosion_size_;
	float_time_hack=rnd(700,2000)/1000.0;
}

//------------------------------------------------------------------
void Obj_grenade::draw(Viewport *viewport)
{
	if (!animator) {
		logmsg(lt_assert,"No animator was found and Obj_grenade::draw() was called");	
		return;
	}

	Graphic *sprite = animator->get_current_frame(age()*float_time_hack);
	if (!sprite) {
		logmsg(lt_error,"Unable to get graphic for grenade");
		return;
	}

	int x=ROUND(xpos-viewport->xpos);
	int y=ROUND(ypos-viewport->ypos);

	if (x >= 0 && x < viewport->get_xsize() &&
			y >= 0 && y < viewport->get_ysize())
		sprite->draw_view(viewport,x,y);
}

//------------------------------------------------------------------
void Obj_grenade::serialize(Serializer *serializer)
{
	Obj_base::serialize(serializer);
	
	serializer->rw(float_time_hack);
}

void Obj_grenade::server_think()
{
	Serverworld *serverworld = (Serverworld *)world;
	Map_hit hit = update_simple(true); //this calls client_think() for us and checks for collisions
	
	if (hit.hittype != ht_none) {
		net_dirty = true;
		bogo_bounce();
		float_time_hack *= 0.9;
	}

	if (age()>timer) {
		//do the explosion
		done=true;
		Obj_explosion *explosion = (Obj_explosion *)serverworld->newobject(ot_explosion);
		if (explosion) {
			explosion->init(
				ownerid,
				(int)xpos,
				(int)ypos,
				explosion_size,
				(int)explosion_damage
			);
		}
	}

}


