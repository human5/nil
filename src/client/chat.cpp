/***************************************************************************
  chat.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2005-01-12
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : make chat as event target work

  copyright            : (C) 2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/ 

#include "client/chat.h"
#include "common/console/logmsg.h"
#include "client/game_client.h"
#include "client/menu/menu.h"

Chat::Chat(Game_client *game_client_, Menu *menu_) {
	game_client = game_client_;
	menu = menu_;
}

Chat::~Chat() {
}

bool Chat::handle(Nil_inputevent *event){
	if (!event) {
		logmsg(lt_error, "No event given, cannot handle it");
		return false;
	}
	switch (event->sdl_inputevent->type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
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
			return handle_joystick_key(new Event_joystick_key(&event->sdl_inputevent->jbutton));

		default : return false;
	}
	return false;
}

bool Chat::handle_keyboard_key(Event_keyboard_key *key_event) {
	if (key_event->sdl_inputevent->type==SDL_KEYDOWN) {
		int function = key_event->cmdfunction & CMDFUNCTION::FUNCTION_MASK;
		switch (function)
		{
			case (CMDFUNCTION::MENU): {
				game_client->set_event_handler(menu);
				game_client->chat_visible = false;
				game_client->menu_visible = true;
				break;
			}
			case (CMDFUNCTION::CHAT): {
				game_client->set_event_handler(game_client);
				game_client->chat_visible = false;
				game_client->add_chatmessage();
				break;
			}
			case (CMDFUNCTION::BACKSPACE): {
				int chatlen = strlen( game_client->getChatString() );
				if( chatlen>0 )
					game_client->getChatString()[ chatlen-1 ] = 0;
				break;
			}
			default: {
				char copyChar[2];
				copyChar[0] = ((SDL_KeyboardEvent *) key_event->sdl_inputevent)->keysym.sym;
				copyChar[1] = 0;
				if((int)strlen(game_client->getChatString()) < (int)(Obj_chatmessage::MSGLENGTH-5-strlen(game_client->get_player()[0].player_name)))
					strcat( game_client->getChatString() , copyChar);
			}
		} 
	}
	return true;
}

bool Chat::handle_mouse_key(Event_mouse_key *mouse_key_event) {
	logmsg(lt_warning, "UNIMPLEMENTED Chat mouse key");
	return false;
}

bool Chat::handle_mouse_motion(Event_mouse_motion *mouse_motion_event) {
	logmsg(lt_warning, "UNIMPLEMENTED Chat mouse motion");
	return false;
}

bool Chat::handle_joystick_key(Event_joystick_key *joystick_key_event) {
	logmsg(lt_warning, "UNIMPLEMENTED Chat joystick key");
	return false;
}

bool Chat::handle_joystick_motion(Event_joystick_motion *joystick_motion_event) {
	logmsg(lt_warning, "UNIMPLEMENTED Chat joystick motion");
	return false;
}
