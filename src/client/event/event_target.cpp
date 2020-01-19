/***************************************************************************
  event_target.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-21
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-11-06
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : added getpos() and getxsize() in case they will
                         be needed anytime

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "event_target.h"

Event_target::Event_target() {}

Event_target::~Event_target() {}

bool Event_target::handle(Nil_inputevent *event) {
	return event->happen(this);
}

void Event_target::setpos(int xpos_, int ypos_) {
	xpos = xpos_;
	ypos = ypos_;
}

void Event_target::getpos(int &xpos_, int &ypos_) {
	xpos_ = xpos;
	ypos_ = ypos;
}

void Event_target::setsize(int xsize_, int ysize_) {
	xsize = xsize_;
	ysize = ysize_;
}

void Event_target::getsize(int &xsize_, int &ysize_) {
	xsize_ = xsize;
	ysize_ = ysize;
}

int Event_target::getysize() {
	return ysize;
}

int Event_target::getxsize() {
	return xsize;
}

bool Event_target::isinside(int x,int y) {
	return ((x > xpos && x < xpos+xsize) && (y > ypos && y < ypos+ysize));
}
