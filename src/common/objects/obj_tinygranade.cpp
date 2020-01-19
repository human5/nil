/***************************************************************************
  obj_tinygranade.cpp  -  a tiny grenade
 ---------------------------------------------------------------------------
  begin                : 2000-02-13
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

    copyright            : (C) 2000 by Anders Rosendal
    email                : qute@klog.dk

 ***************************************************************************/

#include "client/clientworld.h"
#include "common/objects/obj_tinygranade.h"
#include "common/objects/obj_explosion.h"
#include "common/objects/obj_particle.h"

//------------------------------------------------------------------
Obj_tinygranade::Obj_tinygranade(World *world_, int id) : Obj_base(world_,id)
{
	timer=20;
}

//------------------------------------------------------------------
Obj_tinygranade::~Obj_tinygranade()
{
}

//------------------------------------------------------------------
void Obj_tinygranade::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int explosion_size_, float init_age, float timer_, bool explode_on_impact_)
{
	ownerid = owner;
	xpos = xpos_;
	ypos = ypos_;
	xspeed = xspeed_;
	yspeed = yspeed_;
	explosion_damage = explosion_damage_;
	explosion_size = explosion_size_;
	air_resistance = 0;
	yaccel = DEFAULT_ACCEL/3;
	curtime = world->gettime()-init_age;
	createtime=world->gettime();
	timer=timer_;
	explode_on_impact=explode_on_impact_;
}

//------------------------------------------------------------------
void Obj_tinygranade::draw(Viewport *viewport)
{
	int x = ROUND(xpos)-viewport->xpos;
	int y = ROUND(ypos)-viewport->ypos;
 	//viewport->draw_pixel_opaque(x,y,PIXEL(255,255,255) | PF_TYPE_EARTH);
 	viewport->setRawPixelCheck(x,y,PIXEL(255,255,255) | PF_TYPE_EARTH);
}

//------------------------------------------------------------------
void Obj_tinygranade::serialize(Serializer *serializer)
{
	Obj_base::serialize(serializer);
	
	serializer->rw(explosion_damage);
	serializer->rw(explosion_size);
	serializer->rw(timer);
}

//------------------------------------------------------------------
void Obj_tinygranade::client_think()
{
	newton_update(world->gettime());

	if (age()<0.2) return;
	
	if(world->isclient()) {
		Clientworld *clientworld = (Clientworld *)world;
		
		int puffcount = 3;
		float dx = (xpos-old_xpos)/puffcount;
		float dy = (ypos-old_ypos)/puffcount;
        while (ABS(dx) + ABS(dy) < 1 && puffcount > 1) {
			dx = (xpos-old_xpos)/puffcount;
			dy = (ypos-old_ypos)/puffcount;
			puffcount--;
        }
                               
		for (int i=0;i<puffcount;i++) {
			Obj_particle_smoke *smoke = (Obj_particle_smoke *)clientworld->newanonobject(ot_particle_smoke);
			//Obj_particle_smoke *smoke = NULL;
			if(smoke) {
				smoke->init(
					old_xpos+dx*i,
					old_ypos+dy*i,
					xspeed,
					yspeed
				);
			}   	
		}
	}
}
//------------------------------------------------------------------
void Obj_tinygranade::server_think()
{
	Serverworld *serverworld = (Serverworld *)world;
	Map_hit hit = update_simple(true); //this calls client_think() for us and checks for collisions
	bool blow=false;
	
	if (hit.hittype != ht_none) {
		net_dirty = true;
		if(explode_on_impact)
			blow=true;
		else
			bogo_bounce();
	}

	if (age()>timer || blow) {
		//do the explosion
		{
			Vector speed;
			speed.x = old_xspeed;
			speed.y = old_yspeed;
			speed.normalize();

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
		done=true;
	}
}


