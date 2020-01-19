/***************************************************************************
  chat.h  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2005-01-12
  by                   : Christoph Brill (egore@gmx.de)

  copyright            : (C) 2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/  

#include "client/event/event_target.h"

class Menu;
class Game_client;

class Chat : public Event_target {
public:
	Chat(Game_client *game_client_, Menu *menu_);
	~Chat();
	//! This does the routing of events (the default is to let the event happen to this object)
	virtual bool handle(Nil_inputevent *event);
	//! keyboard events that a target can handle
	virtual bool handle_keyboard_key(Event_keyboard_key *key_event);
	//! mouse button events that a target can handle
	virtual bool handle_mouse_key(Event_mouse_key *mouse_key_event);
	//! mouse movement events that a target can handle
	virtual bool handle_mouse_motion(Event_mouse_motion *mouse_motion_event);
	//! mouse button events that a target can handle
	virtual bool handle_joystick_key(Event_joystick_key *joystick_key_event);
	//! mouse movement events that a target can handle
	virtual bool handle_joystick_motion(Event_joystick_motion *joystick_motion_event);
private:
	Game_client *game_client;
	Menu *menu;

};
