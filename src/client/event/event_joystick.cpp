/***************************************************************************
  event_joystick.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : fix copy and paste error

  copyright            : (C) 2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "event_mouse.h"
#include "event_target.h"

bool Event_joystick_key::happen(Event_target *target) {
	return target->handle_joystick_key(this);
}

bool Event_joystick_key::key_pressed() {
	return joystick_key->type == SDL_JOYBUTTONDOWN;
}

bool Event_joystick_motion::happen(Event_target *target) {
	return target->handle_joystick_motion(this);
}
