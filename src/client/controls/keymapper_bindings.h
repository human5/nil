/***************************************************************************
  keymapper_bindings.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-11
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-05-17
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Don't bother with the whole file without SDL.
                         Added include guard.

  last changed         : 2004-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : add joystick and mouse bindings support

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#ifndef KEYMAPPER_BINDINGS_H
#define KEYMAPPER_BINDINGS_H

#ifndef WITHOUT_SDL

#include "keymapper.h"
#include <list>
#include <string>

//! the source of this event
enum Evtmapper_bindingtype {
	//! the source was the keyboard
	binding_key,
	//! the source was the joystick
	binding_joybutton,
	//! the source was the mouse
	binding_mousebutton
};

//! struct to save joystick-number and button
typedef struct Evtmapper_joybuttoninfo {
	//! the joystick that caused this event
	int joystick;
	//! the joystick button that caused this event
	int button;
} TD_evtmapper_joybuttoninfo;

//! struct to save mousebutton number
typedef struct Evtmapper_mousebuttoninfo {
	//! the joystick button that caused this event
	int button;
} TD_evtmapper_mousebuttoninfo;

//! ???
typedef struct Evtmapper_binding {
	//! the type of this event
	Evtmapper_bindingtype type;
	//! ???
	union {
		//! the SDLKey
		SDLKey sdlsym;
		//! the joystick-button
		Evtmapper_joybuttoninfo joybuttoninfo;
		//! the joystick-button
		Evtmapper_mousebuttoninfo mousebuttoninfo;
	};
	//! ???
	CMDFUNCTION::Function cmdfunction;
	//! ???
	std::string config_name;
	//! ???
	std::string display_name;
} TD_evtmapper_binding;

//! @author Flemming Frandsen
//! @brief just a class for storing the actual mappings,
class Evtmapper_bindings : public std::list<Evtmapper_binding*> {
public:
	//! ???
	~Evtmapper_bindings();
	/*! Add a key binding
	    @param sdlsym The symbolic SDL name for this key
	    @param cmdfunction The commandfunction for this key
	    @param config_name ???
	    @param display_name ??? */
	void add_keybinding(SDLKey sdlsym, int cmdfunction, const char *config_name, const char *display_name);
	/*! Add a joystick button binding
	    @param joystick The number of the joystick
	    @param button The number of the joystick button
	    @param cmdfunction ???
	    @param config_name ???
	    @param display_name ??? */
	void add_joybuttonbinding(int joystick, int button, int cmdfunction, const char *config_name, const char *display_name);
	/*! Add a mouse button binding
	    @param button The number of the joystick button
	    @param cmdfunction ???
	    @param config_name ???
	    @param display_name ??? */
	void add_mousebuttonbinding(int button, int cmdfunction, const char *config_name, const char *display_name);
	/*! Find a key based on its symbolic SDL keyname
	    @param sdlsym The symbolic SDL name for a key
	    @return Returns the iterator*/
	iterator find_by_sdlsym(SDLKey sdlsym);
	/*! Find a key based on a joystick button
	    @param joystick The number of the joystick
	    @param button The number of the button
	    @return Returns the iterator*/
	iterator find_by_joy_button(int joystick, int button);
	/*! Find a key based on a mouse button
	    @param button The number of the button
	    @return Returns the iterator */
	iterator find_by_mouse_button(int button);
	/*! Find a key based on its command function
	    @param cmdfunction The commandfunction we are searching for
	    @return Returns the iterator*/
	iterator find_by_function(CMDFUNCTION::Function cmdfunction);
};

#endif
#endif
