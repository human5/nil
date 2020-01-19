/***************************************************************************
  obj_bonus_invisibility.cpp  -  the invisibility extra
 ---------------------------------------------------------------------------
  begin                : 2003-09-01
  by                   : Uwe Fabricius
  email                : uwe.f@bricius.de

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2003 by Uwe Fabricius
  email                : uwe.f@bricius.de

 ***************************************************************************/


#include "obj_bonus_invisibility.h"
#include "obj_player_avatar.h"

Obj_bonus_invisibility::Frames Obj_bonus_invisibility::frames = { NULL };

Obj_bonus_invisibility::Obj_bonus_invisibility(World *world_, int id) :Obj_bonus(world_, id) {
	if( !frames[0] && world->isclient() ) {
		frames[0] = world->getLoader()->graphics.getgraphic( "gfx/obj/extras/invisibility.png" );
	}
	m_time = 20;
}

void Obj_bonus_invisibility::init( int xpos_, int ypos_ ) {
	Obj_bonus::init( xpos_, ypos_, 60 );
}

/***************************************************/

void Obj_bonus_invisibility::draw(Viewport *viewport) {
	Graphic *frame = frames[0];

	if( !frame )  return;

	int x = ROUND( xpos-viewport->xpos );
	int y = ROUND( ypos-viewport->ypos );

	if(x >= 0 && x < viewport->get_xsize() && y >= 0 && y < viewport->get_ysize()) {
		frame->draw_view( viewport, x, y );
	}
}

bool Obj_bonus_invisibility::pickup(Obj_player_avatar *who) {
	who->cloak( m_time );
	who->heal( MAX_HEALTH / 4 );
	bedone();
	return true;
}
