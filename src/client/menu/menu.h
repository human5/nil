/***************************************************************************
  menu.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-10
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-03-29
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Don't bother with the whole file without SDL.

  last changed         : 2004-10-18
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Rewrote key handling

  last changed         : 2004-10-23
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Enhanced menu to get it working and addes submenus
  
  last changed         : 2004-01-06
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Cleanups and add centered menu

  last changed         : 2004-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : mouse cursor and basics for joystick support

  last changed         : 2004-01-15
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : cleanup and API fixes and make use of
                         event_target's methods

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#ifndef _NIL_MENU_H_
#define _NIL_MENU_H_

#ifndef WITHOUT_SDL

#include "client/event/event_target.h"
#include "client/fonts/font_instance.h"

class Menu_items;
class Game_client;

//! @author Christoph Brill, Flemming Frandsen
//! @brief Class for displaying the menu
class Menu : public Event_target {
public:
	/*! Consturctor that creates new menu-items and sets some indexes to 0
	    \param font_ The font that will be used to draw the menu
	    \param game_client_ A handler to the game_client (which is the storage for client options) */
	Menu(Font_instance *font_, Game_client *game_client_);
	//! Desturctor that deletes our items
	~Menu();
	/*! draw menu
	    \param target The surface that the menu will be shown on */
	void draw_menu(Mutable_raw_surface *target);
	/*! Determine how to handle an event
	    \param event The event that wants to be handled
	    \return Returns true, if the event has been handled */
	virtual bool handle(Nil_inputevent *event);
	/*! handle a keyboard button
	    \param key_event The key that will be processed
	    \return Returns true, if the event has been handled */
	virtual bool handle_keyboard_key(Event_keyboard_key *key_event);
	/*! handle a mouse button event (currently defunc)
	    \param mouse_key_event The mouse button that will be processed
	    \return Returns true, if the event has been handled */
	virtual bool handle_mouse_key(Event_mouse_key *mouse_key_event);
	/*! handle a mouse motion event (currently defunc)
	    \param mouse_motion_event The mouse movement that will be processed
	    \return Returns true, if the event has been handled */
	virtual bool handle_mouse_motion(Event_mouse_motion *mouse_motion_event);
	/*! handle a mouse button event (currently defunc)
	    \param joystick_key_event The joystick button that will be processed
	    \return Returns true, if the event has been handled */
	virtual bool handle_joystick_key(Event_joystick_key *joystick_key_event);
	/*! handle a mouse motion event (currently defunc)
	    \param joystick_motion_event The joystick movement that will be processed
	    \return Returns true, if the event has been handled */
	virtual bool handle_joystick_motion(Event_joystick_motion *joystick_motion_event);

private:
	//! index of the topmost item in the menu.
	int top_index;
	//! The items in this menu
	Menu_items *items;
	//! A handle to the font
	Font_instance *font;
	//! A handle to the game client
	Game_client *game_client;
	//! space between menu-items
	static const int MENU_ITEM_SPACING = 5;
	//! The cursor used for the menu
	SDL_Cursor *menu_cursor;
	//! create a new cursor from input
	SDL_Cursor *init_cursor(const char *image[]);
	//! enter a submenu
	bool enter_submenu();
	//! return to the menu above
	bool leave_submenu();
	//! get currently active entry and activate the one above
	bool activate_previous();
	//! get currently active entry and activate the one below
	bool activate_next();
	//! get the heigth of the currently drawn menu
	int get_active_menu_height();
	//! get the heigth of the currently drawn menu (including the separators between the entries)
	int get_active_menu_height_with_spaces();
	//! get the width of the currently drawn menu
	int get_active_menu_width();
	//! get the width of the currently drawn menu
	int get_active_menu_count();
	//! add an entry to the end of the menu
	bool add_item(int type, char* label);
	/*! add an entry to the end of the menu and activate it
	    \param type The type of the item
	    \param label The label of the item
	    \return Returns true if the item was added */
	bool add_active_item(int type, char* label);
	/*! get the type of the currently selected menu item
	    \return Returns the type */
	int get_active_item_type();
	/*! main function to handle movement within the menu
	    \param event The event that wants to be handled
	    \return Returns true, if the event has been handled */
	bool handle_input(Nil_inputevent *event);
};

#endif

#endif
