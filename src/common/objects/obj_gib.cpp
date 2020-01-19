/***************************************************************************
  obj_gib.cpp  -  a gib
 ---------------------------------------------------------------------------
  begin                : 1999-11-25
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_gib.h"
#include "common/console/logmsg.h"
#include <stdio.h>

Obj_gib::Obj_gib(World *world_, int id) :Obj_base(world_,id) {
	rotate_speed = 5;
	puttomap = false;
	air_resistance = DEFAULT_DRAG;
	yaccel = DEFAULT_ACCEL;	
	radius = 1;
	frames[0] = NULL;
}

Obj_gib::~Obj_gib() {}

void Obj_gib::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
	serializer->rw(rotate_speed);
	serializer->rw(puttomap);
}

void Obj_gib::server_think() {}

void Obj_gib::client_think()
{
	//make sure the graphics are loaded.
	if (!frames[0]) {
		for (int c=0; c<8; c++) {
			char gname[100];
			sprintf(gname,"gfx/misc/ripped/ripped_%i.rgbf",c+164);
			frames[c] = world->getLoader()->graphics.getgraphic(gname);
		}
	}

	// move along...
	if(world->isclient()) {
		Map_hit hit = update_simple();
		if (puttomap) {
			done = true;
			return;
		}
		if (hit.hittype != ht_none) {
			net_dirty = true;
			bogo_bounce();
			if (ABS(xspeed) < 0.01 && ABS(yspeed) < 0.01)
				puttomap = true;
		}
	}
}

void Obj_gib::draw(Viewport *viewport) {
	int x=ROUND(xpos-viewport->xpos);
	int y=ROUND(ypos-viewport->ypos);
	if (x >= 0 && x < viewport->get_xsize() && y >= 0 && y < viewport->get_ysize()) {
		int frame = ROUND(rotate_speed*age()) & 7;
		if (!frames[frame]) {
			logmsg(lt_error, "The graphic for the Obj_gib was not loaded, this is bad.");
			return;
		}
		frames[frame]->draw_view(viewport,x,y);
	}
}

void Obj_gib::init(float x, float y, float xspeed_, float yspeed_, int32 color)
{
	xpos = x;
	ypos = y;
	xspeed = xspeed_;
	yspeed = yspeed_;
	//FIXME: use the color parameter to gennerate a gib with the correct color
}
