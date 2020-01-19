/***************************************************************************
  obj_bonus_health.cpp  -  health bonus
 ---------------------------------------------------------------------------
  begin                : 2000-02-20
  by                   : Daniel Burrows
  email                : dburrows@debian.org

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Daniel Burrows
  email                : dburrows@debian.org

 ***************************************************************************/

#include "obj_bonus_health.h"
#include "obj_player_avatar.h"

Obj_bonus_health::Frames Obj_bonus_health::frames={NULL};

Obj_bonus_health::Obj_bonus_health(World *world_, int id) :Obj_bonus(world_, id) {
	if(!frames[0] && world->isclient()) {
		frames[0]=world->getLoader()->graphics.getgraphic("gfx/obj/extras/health.png");
	}
	quantity = rnd(MAX_HEALTH/4,MAX_HEALTH);
}

void Obj_bonus_health::init(int xpos_, int ypos_) {
	Obj_bonus::init(xpos_, ypos_, 60);
}

void Obj_bonus_health::draw(Viewport *viewport) {
	Graphic *frame=frames[0];

	// I don't think this ought to ever happen!
	if(!frame)
		return;

	int x=ROUND(xpos-viewport->xpos);
	int y=ROUND(ypos-viewport->ypos);

	// hmmm, is this clipping right for the health bonus?  (will it occasionally
	// not be drawn when it's just off the screen?)
	if(x>=0 && x<viewport->get_xsize() && y>=0 && y<viewport->get_ysize())
		frame->draw_view(viewport,x,y);
}

bool Obj_bonus_health::pickup(Obj_player_avatar *who) {
	if(who->gethealth()<MAX_HEALTH) {
		who->heal(quantity);
		bedone();
		return true;
	}
	return false;
}
