/***************************************************************************
  menu.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-10
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

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

  copyright            : (C) 2004 by Flemming Frandsen (dion@swamp.dk)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "client/menu/menu.h"
#include "client/menu/menu_item.h"
#include "common/console/logmsg.h"
#include "client/controls/keymapper.h"
#include "client/game_client.h"
#include <vector>

//! @author Christoph Brill
//! @brief A list of menu items
class Menu_items : public std::vector<Menu_item *>{};

static const char *arrow[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "X                               ",
  "XX                              ",
  "X.X                             ",
  "X..X                            ",
  "X...X                           ",
  "X....X                          ",
  "X.....X                         ",
  "X......X                        ",
  "X.......X                       ",
  "X........X                      ",
  "X.....XXXXX                     ",
  "X..X..X                         ",
  "X.X X..X                        ",
  "XX  X..X                        ",
  "X    X..X                       ",
  "     X..X                       ",
  "      X..X                      ",
  "      X..X                      ",
  "       XX                       ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,0"
};

Menu::Menu(Font_instance *font_, Game_client *game_client_) {
	items = new Menu_items;
	top_index = 0;
	font = font_;
	game_client = game_client_;
	// add some default menu entries
	add_item(it_sepstart,  "Mainmenu");
	add_active_item(it_int, "Number of local players: 1");
	add_item(it_string,    "Game Type: Deathmatch");
	add_item(it_submenu,   "Gameoptions"); // NOTE 1 - has to be the same
	add_item(it_submenu,   "Options");
	add_item(it_sepend,    "---");
	
	add_item(it_sepstart,  "Gameoptions"); // NOTE 1 - has to be the same
	add_item(it_int,       "Colored blood: 1");
	add_item(it_sepend,    "Mainmenu"); // NOTE 2 - has to be the menu we return to
	
	add_item(it_sepstart,  "Options");
	add_item(it_int,       "Fullscreen: 1");
	add_item(it_submenu,   "Graphical Options");
	add_item(it_sepend,    "Mainmenu");
	
	add_item(it_sepstart,  "Graphical Options");
	add_item(it_string,    "Alpha Blending: yes");
	add_item(it_sepend,    "Options");

	menu_cursor = init_cursor(arrow);
	if (menu_cursor)
		SDL_SetCursor(menu_cursor);
}

Menu::~Menu() {
	delete items;
	if (menu_cursor)
		SDL_FreeCursor(menu_cursor);
}

SDL_Cursor *Menu::init_cursor(const char *image[]) {
	int i, row, col;
	Uint8 data[4*32];
	Uint8 mask[4*32];
	int hot_x, hot_y;

	i = -1;
	for ( row=0; row<32; ++row ) {
		for ( col=0; col<32; ++col ) {
			if ( col % 8 ) {
				data[i] <<= 1;
				mask[i] <<= 1;
			} else {
				++i;
				data[i] = mask[i] = 0;
			}
			switch (image[4+row][col]) {
				case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
				case '.':
					mask[i] |= 0x01;
					break;
				case ' ':
					break;
			}
		}
	}
	sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

bool Menu::add_item(int type, const char* label) {
	// create a new item and add to the end of the entrylist
	Menu_item *newitem = new Menu_item(type, label, font);
	items->push_back(newitem);
	return true;
}

bool Menu::add_active_item(int type, const char* label) {
	// create a new item, activate it and add to the end of the entrylist
	Menu_item *newitem = new Menu_item(type, label, font);
	if (type != it_sepstart && type != it_sepend)
		newitem->setState(true);
	//FIXME: do sanity check here that only one item is active
	items->push_back(newitem);
	return true;
}

bool Menu::activate_previous() {
	Menu_items::iterator selected = items->begin();
	// go through the list
	while (selected != items->end()) {
		if ((*selected)->getState()) {
			// we found the active item
			(*selected)->setState(false);
			if (selected != items->begin()) {
				// select previous if it ain't the first of a submenu
				selected--;
				if ((*selected)->getType() == it_sepstart)
					selected++;
			}
			(*selected)->setState(true);
			return true;
		}
		selected++;
	}
	return false;
}

bool Menu::activate_next() {
	Menu_items::iterator selected = items->begin();
	// go through the list
	while (selected != items->end()) {
		if ((*selected)->getState()) {
			// we found the active item
			(*selected)->setState(false);
			if (selected+1 != items->end()) {
				// select next if it ain't the last of a submenu
				selected++;
				if ((*selected)->getType() == it_sepend)
					selected--;
			}
			(*selected)->setState(true);
			return true;
		}
		selected++;
	}
	return false;
}

bool Menu::enter_submenu() {
	char tString[80];
	Menu_items::iterator selected = items->begin();
	// go through the menu until we find an active item
	while (selected != items->end()) {
		if ((*selected)->getState())
			if ((*selected)->getType() == it_submenu) {
				// get the label of the submenu and deactivate it
				strcpy (tString, (*selected)->getLabel());
				(*selected)->setState(false);
				break;
				}
		selected++;
	}

	/* if we hit the end, we didn't find an active item that has points
	   to a submenu */
	if (selected == items->end())
		return false;

	int i = 0;
	selected = items->begin();
	/* go through the menu and find a submenu starting point that has the same
	   label as we got above */
	while (selected != items->end()) {
		if (strcmp((*selected)->getLabel(), tString) == 0)
			if ((*selected)->getType() == it_sepstart) {
				// activate the first entry of the submenu
				if (selected+1 != items->end()) {
					selected++;
					if ((*selected)->getType() == it_sepend) {
						selected--;
						return false;
					}
					(*selected)->setState(true);
					selected--;
					break;
				}
				return false;
			}
		i++;
		selected++;
	}
	top_index = i;
	return true;
}

bool Menu::leave_submenu() {
	Menu_items::iterator selected = items->begin();
	char tString[80];
	// find the active menu
	while (selected != items->end()) {
		if ((*selected)->getState()) {
			(*selected)->setState(false);
			break;
		}
		selected++;
	}

	if (selected == items->end())
		return false;

	while (selected != items->end()) {
		if ((*selected)->getType() == it_sepend) {
			strcpy(tString, (*selected)->getLabel());
			break;
		}
		selected++;
	}

	int i = 0;
	selected = items->begin();
	while (selected != items->end()) {
		if (strcmp((*selected)->getLabel(), tString) == 0) {
			if ((*selected)->getType() == it_sepstart) {
				selected++;
				(*selected)->setState(true);
				selected--;
				top_index = i;
			}
		}
		i++;
		selected++;
	}
	return true;
}

int Menu::get_active_menu_height() {
	setsize(getxsize(), 0);
	Menu_items::iterator selected = items->begin() + top_index;
	while (selected != items->end()) {
		if ((*selected)->getType() == it_sepend)
			return getysize();
		setsize(getxsize(), getysize() + (*selected)->getysize());
		selected++;
	}
	return getysize();
}

int Menu::get_active_menu_height_with_spaces() {
	setsize(getxsize(), 0);
	Menu_items::iterator selected = items->begin() + top_index;
	while (selected != items->end()) {
		if ((*selected)->getType() == it_sepend)
			return getysize();
		setsize(getxsize(), getysize() + (*selected)->getysize() + MENU_ITEM_SPACING);
		selected++;
	}
	return getysize();
}

int Menu::get_active_menu_width() {
	setsize(0, getysize());
	Menu_items::iterator selected = items->begin() + top_index;
	while (selected != items->end()) {
		if ((*selected)->getType() == it_sepend)
			return getxsize();
		setsize(getxsize() + (*selected)->getxsize(), getysize());
		selected++;
	}
	return getxsize();
}

int Menu::get_active_menu_count() {
	int count = 0;
	Menu_items::iterator selected = items->begin() + top_index;
	while (selected != items->end()) {
		if ((*selected)->getType() == it_sepend)
			return count;
		count++;
		selected++;
	}
	return count;
}

int Menu::get_active_item_type() {
	Menu_items::iterator selected = items->begin();
	while (selected != items->end()) {
		if ((*selected)->getState())
			return (*selected)->getType();
		selected++;
	}
	return it_notype;
}

bool Menu::handle(Nil_inputevent *event) {
	if (!event) {
		logmsg(lt_error, "No event given, cannot handle it");
		return false;
	}
	switch (event->sdl_inputevent->type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			// we do not access any member of Event_keyboard_key, only from Nil_inputevent, so casting is suitable
			return handle_keyboard_key((Event_keyboard_key *)event);

		case SDL_MOUSEMOTION:
			return handle_mouse_motion(new Event_mouse_motion(&event->sdl_inputevent->motion));

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			return handle_mouse_key(new Event_mouse_key(&event->sdl_inputevent->button));
		
		case SDL_JOYAXISMOTION:
			return handle_joystick_motion(new Event_joystick_motion(&event->sdl_inputevent->jaxis));

		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			return handle_joystick_key((Event_joystick_key *)event);

		default : return false;
	}
	return false;
}

bool Menu::handle_input(Nil_inputevent *event) {
	int function = event->cmdfunction & CMDFUNCTION::FUNCTION_MASK;
	switch (function) {
		case CMDFUNCTION::UP:
			activate_previous();
			break;
		case CMDFUNCTION::DOWN:
			activate_next();
			break;
		case CMDFUNCTION::LEFT:
			switch(get_active_item_type()) {
				case it_bool: logmsg (lt_debug , "Turn on/off"); break;
				case it_int: logmsg (lt_debug , "Decrease"); break;
				case it_string: logmsg (lt_debug , "Switch Value left"); break;
				case it_submenu: logmsg (lt_debug , "Do Nothing"); break;
				default: logmsg (lt_debug , "Unkown type"); break;
			}
			break;
		case CMDFUNCTION::RIGHT:
		case CMDFUNCTION::CHAT:
			switch(get_active_item_type()) {
				case it_bool: logmsg (lt_debug , "Turn on/off"); break;
				case it_int: logmsg (lt_debug , "Increase"); break;
				case it_string: logmsg (lt_debug , "Switch Value right"); break;
				case it_submenu: enter_submenu(); break;
				default: logmsg (lt_debug , "Unkown type"); break;
			}
			break;
		case CMDFUNCTION::MENU:
			if (top_index > 0) {
				leave_submenu();
				break;
			} else {
				game_client->set_quit();
				break;
			}
		case CMDFUNCTION::BACKSPACE:
			if (top_index > 0) {
				leave_submenu();
				break;
			} else {
				game_client->menu_visible = false;
				game_client->set_event_handler(game_client);
				break;
			}
		default: {
			if(event->sdl_inputevent->type == SDL_KEYUP || event->sdl_inputevent->type == SDL_KEYDOWN) {
				SDLKey sdlkey = ((SDL_KeyboardEvent *) event->sdl_inputevent)->keysym.sym;
				logmsg(lt_error, "Unhandled key: 0x%x (%s) -> 0x%x",sdlkey,SDL_GetKeyName(sdlkey),function);
			} else {
				logmsg(lt_warning, "Mouse or Joystick button not handled");
			}
			return false;
		}
	}
	return true;
}

bool Menu::handle_keyboard_key(Event_keyboard_key *key_event) {
	if (!key_event->pressed)
		return true;
	SDL_ShowCursor(SDL_DISABLE);
	return handle_input(key_event);
}

bool Menu::handle_mouse_key(Event_mouse_key *mouse_key_event) {
	// FIXME: handle mouse key events
	SDL_ShowCursor(SDL_ENABLE);
	logmsg (lt_debug , "UNIMPLEMENTED: Menu mouse button pressed");
	return false;
}

bool Menu::handle_mouse_motion(Event_mouse_motion *mouse_motion_event) {
	SDL_ShowCursor(SDL_ENABLE);
	return true;
}

bool Menu::handle_joystick_key(Event_joystick_key *joystick_key_event) {
	if (!joystick_key_event->pressed)
		return true;
	SDL_ShowCursor(SDL_DISABLE);
	return handle_input(joystick_key_event);
}

bool Menu::handle_joystick_motion(Event_joystick_motion *joystick_motion_event) {
	SDL_ShowCursor(SDL_DISABLE);
	Uint8 axis = joystick_motion_event->joystick_motion->axis;
	Sint16 value = joystick_motion_event->joystick_motion->value;
	value /= 10;
	Nil_inputevent nil_inputevent;
	nil_inputevent.sdl_inputevent = joystick_motion_event->sdl_inputevent;
	if (axis == 0 || axis == 2 || axis == 4) {
		if (value > 0)
			nil_inputevent.cmdfunction = CMDFUNCTION::UP;
		else if (value < 0)
			nil_inputevent.cmdfunction = CMDFUNCTION::DOWN;
	} else if (axis == 1 || axis == 3 || axis == 5) {
		if (value > 0) {
			nil_inputevent.cmdfunction = CMDFUNCTION::RIGHT;
		}
		else if (value < 0) {
			nil_inputevent.cmdfunction = CMDFUNCTION::LEFT;
		}
	} else {
		logmsg(lt_error, "Wheter X nor Y axis(%i) has been moved", axis);
		return false;
	}
    // XXX This was disabled in original source code, no clue why
	return handle_input(&nil_inputevent);
}

void Menu::draw_menu(Mutable_raw_surface *target) {
	int curpos = (target->get_ysize() - get_active_menu_height_with_spaces())/2;
	Menu_items::iterator selected = items->begin() + top_index;
	while (selected != items->end()) {
		if ((*selected)->getType() == it_sepend)
			return;
		(*selected)->draw_item(target, ((target->get_xsize()/2) - ((*selected)->getxsize()/2)), curpos);
		curpos += (*selected)->getysize();
		curpos += MENU_ITEM_SPACING;
		selected++;
	}
}
