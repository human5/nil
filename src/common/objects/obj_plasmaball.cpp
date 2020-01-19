/***************************************************************************
  obj_plasmaball.cpp  -  a plasmaball
 ---------------------------------------------------------------------------
  begin                : 2000-02-03
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "obj_plasmaball.h"
#include "obj_player_data.h"
#include "obj_explosion.h"

//These are the globally allocated and loaded graphics
Obj_plasmaball::Frames Obj_plasmaball::frames={NULL,NULL,NULL,NULL};

Obj_plasmaball::Obj_plasmaball(World *world_, int id) : Obj_base(world_, id) {
	explosion_damage = 100;
	yaccel           = 0;
	air_resistance   = 0;
	type             = 1;

	if (!frames[0] && world->isclient()) {
		for (int i=0;i<4;i++) {
			char str[100];
			sprintf(str,"gfx/obj/plasma/sample%i.png", i+1);
			frames[i] = world->getLoader()->graphics.getgraphic(str);
		}
	}
}

Obj_plasmaball::~Obj_plasmaball() {}

void Obj_plasmaball::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int type_) {
	ownerid=owner;
	xpos=xpos_;
	ypos=ypos_;
	xspeed=xspeed_;
	yspeed=yspeed_;
	//FIXME: Is this better?
	//this.explosion_damage = explosion_damage;
	explosion_damage=explosion_damage_;
	type = type_;
	if (type < 1) type = 1;
	if (type > 4) type = 4;

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
}

void Obj_plasmaball::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
	serializer->rw(explosion_damage);
	serializer->rw(type);
	serializer->rw(mLeaveOwnerID);
}

void Obj_plasmaball::server_think() {
	Serverworld *serverworld = (Serverworld *)world;
	Map_hit hit = update_simple(true); //this calls client_think() for us and checks for collisions

	switch (hit.hittype) {
		case ht_object: {
			Obj_base *victim = world->get_object(hit.objectid,ot_error);
			if (victim) {
				Vector v;
				v.x = xspeed;
				v.y = yspeed;
				victim->hurt(50,v,ownerid);
			}
			done = true;
			break;
		}

		case ht_left:
		case ht_right:
		case ht_top:
		case ht_bottom:
		case ht_map : {
			Obj_explosion *explosion = (Obj_explosion *)serverworld->newobject(ot_explosion);
			if (explosion) {
				int size = (rnd(0,100) > 80)?8:5;
				explosion->init(
					ownerid,
					hit.xc,
					hit.yc,
					size,
					30
				);
			}
			done = true;
			break;
		}
		case ht_none: break;
	}
}

void Obj_plasmaball::draw(Viewport *viewport) {
	Graphic *frame = frames[type-1];
	if (!frame) return;

	int x=ROUND(xpos-viewport->xpos);
	int y=ROUND(ypos-viewport->ypos);

	if (x >= 0 && x < viewport->get_xsize() &&
			y >= 0 && y < viewport->get_ysize()) {

		frame->draw_view(viewport,x,y);
	}
}
