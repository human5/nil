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

#ifndef _NIL_EVENT_JOYSTICK_H_
#define _NIL_EVENT_JOYSTICK_H_

#ifndef WITHOUT_SDL

#include "event.h"

//! @author Christoph Brill
//! @brief Class for handling joystick movement
class Event_joystick_motion : public Nil_inputevent {
public:
	/*! ???
	    @param joystick_motion_ ??? */
	Event_joystick_motion(SDL_JoyAxisEvent *joystick_motion_) { joystick_motion = joystick_motion_; }
	/*! Destructor */
	virtual ~Event_joystick_motion() {};
	/*! ???
	    @param target ??? */
	virtual bool happen(Event_target *target);
	//! contains our mouse movement
	SDL_JoyAxisEvent *joystick_motion;
};

//! @author Christoph Brill
//! @brief Class for handling joystick buttons
class Event_joystick_key : public Nil_inputevent {
public:
	/*! ???
	    @param joystick_key_ ??? */
	Event_joystick_key(SDL_JoyButtonEvent *joystick_key_) { joystick_key = joystick_key_; }
	/*! Destructor */
	virtual ~Event_joystick_key() {};
	/*! ???
	    @param target ??? */
	virtual bool happen(Event_target *target);
	//! contains our joystick buttons
	SDL_JoyButtonEvent *joystick_key;
	//! returns if mousebutton has been pressed or released
	bool key_pressed();
};

#endif

#endif
