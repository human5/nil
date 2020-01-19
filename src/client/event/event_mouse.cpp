/***************************************************************************
  event_mouse.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)

  copyright            : (C) 2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "event_mouse.h"
#include "event_target.h"

bool Event_mouse_key::happen(Event_target *target) {
	return target->handle_mouse_key(this);
}

bool Event_mouse_key::key_pressed() {
	return mouse_key->type == SDL_MOUSEBUTTONDOWN;
}

bool Event_mouse_motion::happen(Event_target *target) {
	return target->handle_mouse_motion(this);
}
