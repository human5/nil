/***************************************************************************
  obj_explosion.cpp  -  draws an explosion around a position
 ---------------------------------------------------------------------------
  begin                : 1999-10-08
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

    copyright            : (C) 1999 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_explosion.h"
#include "common/objects/obj_player_avatar.h"
#include "common/objects/obj_particle.h"
#include "client/clientworld.h"
#include "common/systemheaders.h"

//! This is the actual place where we store the pointers to our graphics. all objects of this type share this.
Obj_explosion::Frames Obj_explosion::frames;

void Obj_explosion::serialize(Serializer *serializer) {
	serializer->rw(xpos);
	serializer->rw(ypos);
	serializer->rw(curtime);
	serializer->rw(state);
	serializer->rw(createtime);
	serializer->rw(maxsize);
	serializer->rw(ownerid);
	serializer->rw(hitpoints);
}

void Obj_explosion::init(int owner, int xcenter, int ycenter, int size, int hitpoints_) {
	if (ycenter < 0) ycenter=0;
	if (xcenter < 0) xcenter=0;

	curtime = world->gettime();
	createtime = curtime;
	xpos = xcenter;
	ypos = ycenter;
	ownerid = owner;
	maxsize = size;
	state = 0;
	hitpoints = hitpoints_;

	if (maxsize > 15) maxsize = 15;
	if (maxsize < 0) maxsize = 0;

	/*
		This is an evil hack to put the hole immediatly rather than wait for the server_think()
		at the end of the think run, this is to allow other explosive objects that come right
		after this one to go through the hole that this explostions creator makes when impacting.
	*/
	frame = maxsize+1;
	world->makehole(ROUND(xpos),ROUND(ypos),(int)maxsize);
}

Obj_explosion::Obj_explosion(World *world_, int id) : Obj_base(world_,id)
{
	xpos = 100;
	ypos = 100;
	maxsize = 16;
	frame = -1;
	hitpoints = 100;
	state = 0;

	//check to see if we should load the graphics for this object...
	if (!frames[0] && world->isclient()) {

		for (int c=0; c<16; c++) {
			char gname[100];
			sprintf(gname,"gfx/obj/explosion/ripped/explosion_%i.rgbf",c+1);
			frames[c] = world->getLoader()->graphics.getgraphic(gname);
		}
	}
}

void Obj_explosion::server_think()
{
	client_think();
	if (state == 1) {
		((Serverworld*)world)->do_radius_damage(ownerid, xpos, ypos, maxsize, hitpoints);
	} //if state == 1

	if (state == 3) bedone();
}

/*
state:
0: growing.
1: hurting
2: shrinking
3: done
*/

void Obj_explosion::client_think()
{
	curtime = world->gettime();

	//this can happen on the first frame.
	if (frame < 0) frame = 0;

	//go directly from peaking to shrinking
	if (state == 1) state = 2;

	//either grow or shrink
	if (state == 0)	frame = age()*16*16;
	if (state == 2) frame = maxsize-(age()-peak_time)*16;

	//detect peak:
	if (frame > maxsize) {
		state = 1;
		peak_time = age();
		frame = maxsize;
	}

	//if we are at a peak put the actual hole
	if (state == 1) {
		world->makehole(ROUND(xpos),ROUND(ypos),(int)maxsize);

		if (world->isclient()) {
			Clientworld *clientworld = (Clientworld *)world;

			// create some sparks
			int n = rnd(-10, (int)(maxsize/2.0) ); //D8 def 3
			if(maxsize>10) { // always some sparks for bigger explosions
				if(n<0) n =  (int)(maxsize/1.5);
				else    n += (int)(maxsize/1.5);
			}
			
			for (int i=0;i<n;i++) {
  	  	Vector v;
				v.x = v.y = rnd(200,500);
				v.rotate(2*PI*rnd(-1000,1000)/1000.0);
				Obj_particle_spark *spark = (Obj_particle_spark *)clientworld->newanonobject(ot_particle_spark);
				if (spark) {
		    	spark->init(xpos, ypos, v.x, v.y);
				}			
			}
		} // client

	}

	//detect when we have shrunk to less than zero (that is we are done shrinking)
	if (frame < 0) {
		frame = 0;
		state = 3;
	}
}

void Obj_explosion::draw(Viewport *viewport)
{
	if (ROUND(frame) < 0 || ROUND(frame) > 15) return;

	int x = ROUND(xpos) - viewport->xpos;
	int y = ROUND(ypos) - viewport->ypos;
	if (x > frame/2 &&
			y > frame/2 &&
			x < viewport->get_xsize()-frame/2 &&
			y < viewport->get_ysize()-frame/2) {

		frames[ROUND(frame)]->draw_view(viewport,x,y);
	}
}
