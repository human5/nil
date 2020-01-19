/***************************************************************************
  obj_test.cpp  -  a test object
 ---------------------------------------------------------------------------
  begin                : 1999-10-20
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/systemheaders.h"
#include "common/objects/obj_test.h"
#include "common/objects/obj_explosion.h"


Obj_test::Obj_test(World *world_, int id) : Obj_base(world_,id)
{
	unspawned = true;
	xpos = 100;
	ypos = 100;	
	if (world->isclient()) rotate_speed = rnd(-1000,1000)/100.0;
	for (int c=0; c<8;c++) frames[c]=NULL;
}

void Obj_test::init(int owner, int level_, float xpos_,float ypos_,float xspeed_,float yspeed_,float yaccel_, float air_resistance_)
{
	level = level_;
	ownerid = owner;
	curtime = world->gettime();
	createtime = curtime;
	xpos = xpos_;
	ypos = ypos_;
	xspeed = xspeed_;
	yspeed = yspeed_;
	yaccel = yaccel_;
	radius = 5;
	air_resistance = air_resistance_;	
}

Obj_test::~Obj_test()
{

}

void Obj_test::server_think()
{
  Serverworld *serverworld = (Serverworld *)world;
	
//  int x0 = ROUND(xpos);
//  int y0 = ROUND(ypos);
	
	//do the reflexes
	Map_hit hit = update_simple(true); //this calls client_think() for us and checks for collisions
//	client_think();

	switch (hit.hittype) {
		case ht_object: {
     	Obj_base *victim = world->get_object(hit.objectid,ot_error);
			if (victim) {
				Vector v;
				v.x = xspeed;
				v.y = yspeed;
      	victim->hurt(200,v,ownerid);
			}

			Obj_explosion *explosion = (Obj_explosion *)serverworld->newobject(ot_explosion);
			if (explosion && explosion->gettype() == ot_explosion)
				explosion->init(
					ownerid,
					hit.xc,
					hit.yc,
					8,
					200
				);
			done = true;
			break;
		}

		case ht_left:
		case ht_right:
		case ht_top:
		case ht_bottom:
  	case ht_map : {
			bogo_bounce();
			setdirty();
			break;
		}

		case ht_none: break;
	}

	//and then the higherlevel thinking

	if (age() > 0.5 && unspawned) {
		unspawned = false;
		Vector v;
		v.x = xspeed;
		v.y = yspeed;
		v = Math::normalize(v);

		if (level <= 10) {
			int children = 1;
			if (level < 3) children = 3;
			for (int c=0;c<children;c++) {
  	
				Obj_test *test = (Obj_test *)serverworld->newobject(ot_testobject);
				if (test)
					test->init(
						ownerid,
						level+1,
						xpos,
						ypos,
						rnd(-100,100),
						rnd(-100,100),
						DEFAULT_ACCEL, DEFAULT_DRAG
					);
			}
		}
	}

	if (age() > 3 && rnd(0,100) > 75) {
		Obj_explosion *explosion = (Obj_explosion *)serverworld->newobject(ot_explosion);
		if (explosion && explosion->gettype() == ot_explosion)
			explosion->init(
				ownerid,
				ROUND(xpos),
				ROUND(ypos),
				8,
				200
			);
		done = true;		
	}
}

void Obj_test::draw(Viewport *viewport)
{
	//create intersection routine for the objects...
  if (!frames[0])
		for (int c=0; c<8;c++) {
			char gname[100];
			sprintf(gname,"gfx/obj/molotow/molotow_%i.rgbf",c);
			frames[c]=world->getLoader()->graphics.getgraphic(gname);
		}
	
	int x=ROUND(xpos-viewport->xpos);
	int y=ROUND(ypos-viewport->ypos);

	if (x >= 0 && x < viewport->get_xsize() &&
			y >= 0 && y < viewport->get_ysize()) {

		frames[ROUND(rotate_speed*age()) & 7]->draw_view(viewport,x,y);
//		viewport->raw_target.p ixels[x+y*viewport->raw_target.xsize] = 0xffff90;
	}
}
