/***************************************************************************
  event.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-21
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Don't bother with the whole file without SDL.

  last changed         : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : move out device specific event handling to
                         seperate files and merge parts from keymapper.h

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#ifndef _NIL_EVENT_H
#define _NIL_EVENT_H

#ifndef WITHOUT_SDL

#include "common/systemheaders.h"

class Event_target;

//! This is used to determine which key-event has happend
namespace CMDFUNCTION {
	//! ???
	typedef enum Function {
		TARGET_MASK   = 0xff00,
		HOTKEY        = 0x0000,
		PLAYER1       = 0x0100,
		PLAYER2       = 0x0200,
		FUNCTION_MASK = 0x00ff,
		NONE          = 0x0000,
		//player controls:
		UP,
		DOWN,
		LEFT,
		RIGHT,
		FIRE,
		JUMP,
		DIG,
		HOOK_FIRE,
		HOOK_RELEASE,
		HOOK_IN,
		HOOK_OUT,
		JETPACK,
		WEAPON1,
		WEAPON2,
		WEAPON3,
		WEAPON4,
		WEAPON5,
		NEXTWEAPON,
		PREVWEAPON,
		//other hotkeys:
		TOGGLE_CONSOLE,
		SCREEN_SHOOT,
		TOGGLE_LOGO,
		TOGGLE_DEATHCAM,
		TOGGLE_TRAFFIC,
		TOGGLE_FRAMERATE,
		TOGGLE_SHADOW,
		TOGGLE_SMOOTH,
		MENU,
		CHAT,
		BACKSPACE,
		FRAGTABLE
	} TD_function;
};

//! @author Flemming Frandsen
//! @brief class for event handling (used for keyboard, mouse and joystick events).
class Nil_inputevent {
public:
	/*! Destructor */
	virtual ~Nil_inputevent() {};
	/*! An event should happen
	    @param target The target the event happens to */
	virtual bool happen(Event_target *target) { return false; }
	//! our event
	SDL_Event *sdl_inputevent;
	//! ???
	CMDFUNCTION::Function cmdfunction;
	//! to determine if an eventtrigger (a key) was pressed or released
	bool pressed;
};

/*! Creates an Event object from an SDL event
    @param event An sdl event */
Nil_inputevent *new_event(SDL_Event *event);

#endif

#endif
