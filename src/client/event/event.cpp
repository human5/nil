/***************************************************************************
  event.cpp  -  Headerfile to include all the object headers
 ---------------------------------------------------------------------------
  begin                : 2000-02-21
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : move out device specific event handling to
                         seperate files

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#include "event.h"
#include "event_target.h"

Nil_inputevent *new_event(SDL_Event *event) {
	switch (event->type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			return new Event_keyboard_key(&event->key);

		case SDL_MOUSEMOTION:
			return new Event_mouse_motion(&event->motion);

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			return new Event_mouse_key(&event->button);
		
		case SDL_JOYAXISMOTION:
			return new Event_joystick_motion(&event->jaxis);

		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			return new Event_joystick_key(&event->jbutton);

		default : return NULL;
	}
}
