/***************************************************************************
  event_mouse.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)

  copyright            : (C) 2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#ifndef _NIL_EVENT_MOUSE_H_
#define _NIL_EVENT_MOUSE_H_

#ifndef WITHOUT_SDL

#include "event.h"

//! @author Flemming Frandsen
//! @brief Class for handling mouse movement
class Event_mouse_motion : public Nil_inputevent {
public:
	/*! ???
	    @param mouse_motion_ ??? */
	Event_mouse_motion(SDL_MouseMotionEvent *mouse_motion_) { mouse_motion = mouse_motion_; }
	/*! Destructor */
	virtual ~Event_mouse_motion() {};
	/*! ???
	    @param target ??? */
	virtual bool happen(Event_target *target);
	//! contains our mouse movement
	SDL_MouseMotionEvent *mouse_motion;
};

//! @author Flemming Frandsen
//! @brief Class for handling mousebuttons
class Event_mouse_key : public Nil_inputevent {
public:
	/*! ???
	    @param mouse_key_ ??? */
	Event_mouse_key(SDL_MouseButtonEvent *mouse_key_) { mouse_key = mouse_key_; }
	/*! Destructor */
	virtual ~Event_mouse_key() {};
	/*! ???
	    @param target ??? */
	virtual bool happen(Event_target *target);
	//! contains our mousebuttons
	SDL_MouseButtonEvent *mouse_key;
	//! returns if mousebutton has been pressed or released
	bool key_pressed();
};

#endif

#endif
