/***************************************************************************
  event_keyboard.cpp  -  
 ---------------------------------------------------------------------------
  begin                : 2005-01-12
  by                   : Christoph Brill (egore@gmx.de)

  copyright            : (C) 2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "event_keyboard.h"
#include "event_target.h"

Event_keyboard_key::Event_keyboard_key(SDL_KeyboardEvent *keyboard_key_) {
	keyboard_key = keyboard_key;
}

bool Event_keyboard_key::happen(Event_target *target) {
	return target->handle_keyboard_key(this);
}

bool Event_keyboard_key::is_pressed() {
	return keyboard_key->type == SDL_KEYDOWN;
}
