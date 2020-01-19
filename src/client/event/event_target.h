/***************************************************************************
  event_target.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-21
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Don't bother with the whole file without SDL.

  last changed         : 2005-01-15
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Improve API to have less public functions

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#ifndef _NIL_EVENT_TARGET_H_
#define _NIL_EVENT_TARGET_H_

#ifndef WITHOUT_SDL
#include "event.h"
#include "event_keyboard.h"
#include "event_mouse.h"
#include "event_joystick.h"

class Raw_surface;

//! @author Flemming Frandsen
//! @brief If an event occurs, this class is what will handle it
class Event_target {
public:
	//! Constructor
	Event_target();
	//! Destructor
	virtual ~Event_target();
	//! This does the routing of events (the default is to let the event happen to this object)
	virtual bool handle(Nil_inputevent *event) = 0;
	//! keyboard events that a target can handle
	virtual bool handle_keyboard_key(Event_keyboard_key *key_event) = 0;
	//! mouse button events that a target can handle
	virtual bool handle_mouse_key(Event_mouse_key *mouse_key_event) = 0;
	//! mouse movement events that a target can handle
	virtual bool handle_mouse_motion(Event_mouse_motion *mouse_motion_event) = 0;
	//! mouse button events that a target can handle
	virtual bool handle_joystick_key(Event_joystick_key *joystick_key_event) = 0;
	//! mouse movement events that a target can handle
	virtual bool handle_joystick_motion(Event_joystick_motion *joystick_motion_event) = 0;
	/*! Gets the stored x and y size
	    @param xpos_ The x size
	    @param ypos_ The y size */
	void getpos(int &xpos_, int &ypos_);
	/*! Get the stored x and y size
	    @param xsize_ The x size
	    @param ysize_ The y size */
	void getsize(int &xsize_, int &ysize_);
	//! Get the stored y size
	int getysize();
	//! Get the stored x size
	int getxsize();
	/*! Checks if a given point is in the stored range
	    @param x x coordinate
	    @param y y coordinate */
	bool isinside(int x, int y);
protected:
	//! Stored x position
	int xpos;
	//! Stored y position
	int ypos;
	//! Stored x size
	int xsize;
	//! Stored y size
	int ysize;

	/*! Sets a static x and y position of the event target
	    @param xpos_ The x coordinates
	    @param ypos_ The y coordinates */
	void setpos(int xpos_, int ypos_);
	/*! Sets a static x and y size of the event target
	    @param xsize_ The x size
	    @param ysize_ The y size */
	void setsize(int xsize_, int ysize_);
};

#endif

#endif
