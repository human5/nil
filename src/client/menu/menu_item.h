/***************************************************************************
  menu_item.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-20
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Don't bother with the whole file without SDL.
  
  last changed         : 2004-10-18
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : Added type

  last changed         : 2004-01-06
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : Cleanups and add centered menu

  last changed         : 2005-01-15
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : use event_target's width and height instead
                         of our own

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef MENU_ITEM_H
#define MENU_ITEM_H

#ifndef WITHOUT_SDL

#include "client/event/event_target.h"
#include "client/fonts/font_instance.h"

//! This one stores the type of a menuitem
enum item_type {
	//! The item has no type
	it_notype = 0,
	//! The item is a bool
	it_bool,
	//! The item is an int
	it_int,
	//! The item is a string
	it_string,
	//! The item is a submenu
	it_submenu,
	//! The item is a submenu entry point
	it_sepstart,
	//! The item is a submenu end point
	it_sepend
};

//! @author Christoph Brill, Flemming Frandsen
//! @brief Class for a menu-item
class Menu_item : public Event_target {
public:
	//! Constructor that adds a deactivated menu-item
	Menu_item(Font_instance *font_);
	/*! Constructor that adds a deactivated menu-item with given type
	    @param type_ The type of the menuitem
	    @param font A handle to the font that will be used for this menu */
	Menu_item(int type_, Font_instance *font_);
	//! Constructor that adds a deactivated menu-item with given type
	Menu_item(int type_, const char* label_, Font_instance *font_);
	//! Destructor (empty)
	~Menu_item();
	//! determines if an item wants to handle itself or should be handled here
	virtual bool handle(Nil_inputevent *event);
	/*! handle a keyboard button
	    @param key_event The key that will be processed
	    @return */
	virtual bool handle_keyboard_key(Event_keyboard_key *key_event);
	//! handle a mouse button event (currently defunc)
	virtual bool handle_mouse_key(Event_mouse_key *mouse_key_event);
	//! handle a mouse motion event (currently defunc)
	virtual bool handle_mouse_motion(Event_mouse_motion *mouse_motion_event);
	//! handle a mouse button event (currently defunc)
	virtual bool handle_joystick_key(Event_joystick_key *joystick_key_event);
	//! handle a mouse motion event (currently defunc)
	virtual bool handle_joystick_motion(Event_joystick_motion *joystick_motion_event);
	//! Set heigth and width
	void calculate_geom(char* text, Font_instance *font);
	/*! Activate or deavtivate this item
	    @param state The state that we will change to */
	void setState(bool state);
	/*! Get the current status
	    @return Returns true if the item is currently active */
	bool getState();
	/*! Change the type of this item (use with care)
	    @param type_ The type we will change to */
	void setType(int type_);
	/*! Get the type of the item
	    @return The type */
	int getType();
	/*! Change the label of this item
	    @param label_ The new label for this item */
	void setLabel(char* label_);
	/*! Get the label of the item
	    @return The label*/
	char* getLabel();
	/*! the draw function for an item 
	    @param target The surface we draw the menu item to
	    @param font_ The font the menuitem will be drawn in */
	void draw_item(Mutable_raw_surface *target, int xpos_, int ypos_);
private:
	//! The maximum length of a name for a menu item
	static const unsigned int MAX_MENU_ITEM_LEN = 80;
	//! the active-status
	bool active;
	//! the type (int, string, ..)
	int type;
	//! the label
	char label[MAX_MENU_ITEM_LEN];
	//! A reference to the font instance
	Font_instance *font;
};

#endif
#endif
