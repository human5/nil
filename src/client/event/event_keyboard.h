/***************************************************************************
  event_keyboard.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)

  copyright            : (C) 2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#ifndef _NIL_EVENT_KEYBOARD_H_
#define _NIL_EVENT_KEYBOARD_H_

#ifndef WITHOUT_SDL

#include "event.h"

//! @author Flemming Frandsen
//! @brief Class for handling keyboard events
class Event_keyboard_key : public Nil_inputevent {
public:
	/*! Save the key in a local variable
	    @param keyboard_key_ The assigned key of the keyboard */
	Event_keyboard_key(SDL_KeyboardEvent *keyboard_key_);
	/*! Destructor */
	virtual ~Event_keyboard_key() {};
	/*! ???
	    @param target The target */
	virtual bool happen(Event_target *target);
	//! contains our event trigger
	SDL_KeyboardEvent *keyboard_key;
	//! returns if the key is pressed or released
	bool is_pressed();
};

#endif

#endif
