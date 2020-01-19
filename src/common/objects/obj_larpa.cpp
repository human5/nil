/***************************************************************************
  obj_larpa.cpp  -  a larpa (what ever this is ....)
 ---------------------------------------------------------------------------
  begin                : 2000-02-22
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_larpa.h"
#include "common/objects/obj_player_data.h"
#include "common/objects/obj_explosion.h"
#include "common/objects/obj_projectile.h"
#include "common/console/logmsg.h"

Obj_larpa::Obj_larpa(World *world_, int id) : Obj_base(world_,id) {
	for(int i=0; i<8; i++)
		sprite[i]=0;
}

Obj_larpa::~Obj_larpa() {}
       
void Obj_larpa::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float trail_, float power_, int size_) {
	ownerid = owner;
	xpos = xpos_;
	ypos = ypos_;
	xspeed = xspeed_;
	yspeed = yspeed_;
	yaccel = DEFAULT_ACCEL/3;
	createtime=world->gettime();
	trail=trail_;
	power=power_;
	size=size_;
	last=0;
	// init object time, assume it was created at the begining of the last frame,
	// this is needed for the first call to newton_update
	//curtime = world->gettime()-init_age;
	curtime = world->gettime() - world->get_last_frame_delta();
	
	// init mLeaveOwerID to prevent the rocket from hitting its owner immediatly
	Obj_player_data *player_data = (Obj_player_data *)world->get_object(ownerid,ot_player_data);
	if(player_data) {
		mLeaveOwnerID = player_data->getavatarid();
	} else {
		mLeaveOwnerID = -1; // this should not happen
	}
	air_resistance=0.003;
}

//The frames are numbered like this:
//234
//1 5
//076
void Obj_larpa::draw(Viewport *viewport) {
	int x = ROUND(xpos)-viewport->xpos;
	int y = ROUND(ypos)-viewport->ypos;
	
	double rad=atan2(yspeed,xspeed); // Calculated the direction
	// rad between PI and -PI, 0 = due right, clockwise +
	
	rad *= 4.0 / PI;
	// now between -4 and 4

	int frame = (int)(rad +5.5);
	frame %= 8;	

	if (!sprite[frame]) {
		// *WARNING* Could this be static??  Same goes for grep STATIC_STRING. email qute@cutey.com if you know the answer
		char s[100];
		sprintf(s,"gfx/obj/larpa/larpa%i.png",frame);
		//logmsg(lt_fatal,"fil: %s rad=%f\n  dir %d",s,rad,dir);
		sprite[frame] = world->getLoader()->graphics.getgraphic(s);
		if (!sprite[frame]) return;
	}
	
	sprite[frame]->draw_view(viewport,x,y);
}

void Obj_larpa::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
	serializer->rw(trail);
	serializer->rw(power);
	serializer->rw(last);
	serializer->rw(size);
}

void Obj_larpa::server_think() {
	Serverworld *serverworld = (Serverworld *)world;
       
	Map_hit hit = update_simple(true); //this calls client_think() for us and checks for collisions

	bool blow=false;

	if((age()>0.03) && (age()>last+trail)) {
		last+=trail;
		Obj_projectile *bullet = (Obj_projectile *)serverworld->newobject(ot_projectile);
		if (bullet) bullet->init(ownerid, xpos, ypos, xspeed/10+rnd(-15,15), 25+yspeed/5+rnd(-1,1), 100, 4, 3, 0);
	}

	if (hit.hittype != ht_none) {
		net_dirty = true;
		blow=true;
	}
       
	if(blow) {
		done=true;
		Obj_explosion *explosion = (Obj_explosion *)serverworld->newobject(ot_explosion);
		if (explosion) explosion->init(ownerid, (int)xpos, (int)ypos, size, (int)power);
	}
}
