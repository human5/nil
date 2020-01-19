/***************************************************************************
  obj_railslug.cpp  -  the railslug
 ---------------------------------------------------------------------------
  begin                : 2000-01-05
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_railslug.h"
#include "common/objects/obj_player_data.h"
#include "common/objects/obj_explosion.h"
#include "common/objects/obj_particle.h"
#include "client/clientworld.h"
#include "common/systemheaders.h"

Obj_railslug::Obj_railslug(World *world_, int id) : Obj_base(world_,id)
{
	havehit = false;
	air_resistance = 0;
	yaccel = 0;
	radius = 2;
}

Obj_railslug::~Obj_railslug()
{
}

void Obj_railslug::serialize(Serializer *serializer)
{
	Obj_base::serialize(serializer);
	serializer->rw(havehit);
	if (havehit) {
		serializer->rw(xhit);		
		serializer->rw(yhit);		
		serializer->rw(xstart);		
		serializer->rw(ystart);
	}

	// init mLeaveOwerID to prevent the rocket from hitting its owner immediatly
	Obj_player_data *player_data = (Obj_player_data *)world->get_object(ownerid,ot_player_data);
	if(player_data) {
		mLeaveOwnerID = player_data->getavatarid();
	} else {
		mLeaveOwnerID = -1; // this should not happen
	}
}

void Obj_railslug::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_)
{
	ownerid = owner;
	xpos = xpos_;
	ypos = ypos_;
	xspeed = xspeed_;
	yspeed = yspeed_;
	xstart = xpos;
	ystart = ypos;
}

void Obj_railslug::draw(Viewport *viewport)
{
}

void Obj_railslug::server_think()
{

  Serverworld *serverworld = (Serverworld *)world;
	Map_hit hit = update_simple(true); //this calls client_think() for us and checks for collisions

	if (havehit) {
		done=true;
		return;
	}

	if (hit.hittype == ht_object) {
   	Obj_base *victim = world->get_object(hit.objectid,ot_error);
		if (victim) {
			Vector v;
			v.x = xspeed;
			v.y = yspeed;
     	victim->hurt(1000,v,ownerid);
		}
		havehit = true;
		xhit = hit.xc;
		yhit = hit.yc;
		setdirty();

	} else if (hit.hittype != ht_none) {
		int penetration = 2;

		//Set up a vector for the penetration:
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
				5,  //size
				500 //Damage
			);
		}
		havehit = true;
		xhit = hit.xc;
		yhit = hit.yc;
		setdirty();
	}
}

void Obj_railslug::client_think()
{
	Obj_base::client_think();

	if (!world->isclient()) return;
	if (!havehit) return;
	Clientworld *clientworld= (Clientworld *)world;

	int n = rnd(70,100);
	for (int i=0;i<n;i++) {
		Vector v;
		v.x = v.y = rnd(200,500);
		v.rotate(2*PI*rnd(-1000,1000)/1000.0);
		Obj_particle_spark *spark = (Obj_particle_spark *)clientworld->newanonobject(ot_particle_spark);
		if (spark) {
	  	spark->init(xpos, ypos, v.x, v.y);
		}
	}

	Vector dir;
	dir.x = xhit-xstart;
	dir.y = yhit-ystart;
	float length = dir.length();
	dir.normalize();

	int hmm=0;
	for (float t=0;t < length;t += 0.5) {
		
		{ //do the outer blue part:
			float x = xstart+dir.x*t+1.5*sin(t);
			float y = ystart+dir.y*t+1.5*cos(t);

			Obj_particle_railtrail *trail = (Obj_particle_railtrail *)clientworld->newanonobject(ot_particle_railtrail);
			if (trail) {
				trail->init(x,y,rnd(-700,700)/500.0,rnd(-700,700)/500.0);
				trail->setcolor(PIXEL(0,0,255));
			}
		}

		// do the inner white part:		
		if ((hmm++ % 2) == 0) {	
			float x = xstart+dir.x*t+0.5*sin(t*4);
			float y = ystart+dir.y*t+0.5*cos(t*4);

			Obj_particle_railtrail *trail = (Obj_particle_railtrail *)clientworld->newanonobject(ot_particle_railtrail);
			if (trail) {
				trail->init(x,y,rnd(-700,700)/1000.0,rnd(-700,700)/1000.0);
				trail->setcolor(PIXEL(180,180,200));
			}
		}

	}
}
