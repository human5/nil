/***************************************************************************
  keymapper.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-11
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : add joystick and mouse bindings

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "client/controls/keymapper.h"
#include "client/controls/keymapper_bindings.h"
#include "common/misc.h"
#include "common/console/logmsg.h"

Evtmapper::Evtmapper() {
	bindings = new Evtmapper_bindings;

	//general purpose keys:
	bindings->add_keybinding(SDLK_F1,          CMDFUNCTION::TOGGLE_CONSOLE,                      "key_Toggle_console",       "Console (on/off)");
	bindings->add_keybinding(SDLK_F2,          CMDFUNCTION::SCREEN_SHOOT,                        "key_Screen_shoot",         "Save screen shot");
	bindings->add_keybinding(SDLK_F3,          CMDFUNCTION::TOGGLE_LOGO,                         "key_Toggle_logo",          "Logo (on/off)");
	bindings->add_keybinding(SDLK_F4,          CMDFUNCTION::TOGGLE_DEATHCAM,                     "key_Toggle_deathcam",      "Deathcam (on/off)");
	bindings->add_keybinding(SDLK_F5,          CMDFUNCTION::TOGGLE_TRAFFIC,                      "key_Toggle_traffic",       "Traffic graph (on/off)");
	bindings->add_keybinding(SDLK_F6,          CMDFUNCTION::TOGGLE_FRAMERATE,                    "key_Toggle_framerate",     "Framerate (on/off)");
	bindings->add_keybinding(SDLK_F7,          CMDFUNCTION::TOGGLE_SHADOW,                       "key_Toggle_shadow",        "Drop shadow (on/off)");
	bindings->add_keybinding(SDLK_F8,          CMDFUNCTION::TOGGLE_SMOOTH,                       "key_Toggle_smooth",        "Bilinear smoothing (on/off)");
	bindings->add_keybinding(SDLK_ESCAPE,      CMDFUNCTION::MENU,                                "key_Menu",                 "Bring up the menu");
	bindings->add_keybinding(SDLK_RETURN,      CMDFUNCTION::CHAT,                                "key_chat",                 "Enter chat mode");
	bindings->add_keybinding(SDLK_BACKSPACE,   CMDFUNCTION::BACKSPACE,                           "key_backspace",            "Delete a character from chat message");
	bindings->add_keybinding(SDLK_TAB,         CMDFUNCTION::FRAGTABLE,                           "key_fragtable",            "Display the frag table while pressed");

	//player 1 keys:
	bindings->add_keybinding(SDLK_UP,          CMDFUNCTION::PLAYER1 | CMDFUNCTION::UP,           "key_Player1_up",           "p1 Up");
	bindings->add_keybinding(SDLK_DOWN,        CMDFUNCTION::PLAYER1 | CMDFUNCTION::DOWN,         "key_Player1_down",         "p1 Down");
	bindings->add_keybinding(SDLK_LEFT,        CMDFUNCTION::PLAYER1 | CMDFUNCTION::LEFT,         "key_Player1_left",         "p1 Left");
	bindings->add_keybinding(SDLK_RIGHT,       CMDFUNCTION::PLAYER1 | CMDFUNCTION::RIGHT,        "key_Player1_right",        "p1 Right");
	bindings->add_keybinding(SDLK_LCTRL,       CMDFUNCTION::PLAYER1 | CMDFUNCTION::FIRE,         "key_Player1_fire",         "p1 Fire");
	bindings->add_keybinding(SDLK_LSHIFT,      CMDFUNCTION::PLAYER1 | CMDFUNCTION::JUMP,         "key_Player1_jump",         "p1 Jump");
	bindings->add_keybinding(SDLK_LALT,        CMDFUNCTION::PLAYER1 | CMDFUNCTION::DIG,          "key_Player1_dig",          "p1 Dig");
	bindings->add_keybinding(SDLK_z,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::HOOK_FIRE,    "key_Player1_hook_fire",    "p1 Fire hook");
	bindings->add_keybinding(SDLK_x,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::HOOK_RELEASE, "key_Player1_hook_release", "p1 Release hook");
	bindings->add_keybinding(SDLK_a,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::HOOK_IN,      "key_Player1_hook_in",      "p1 reel out hook");
	bindings->add_keybinding(SDLK_s,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::HOOK_OUT,     "key_Player1_hook_out",     "p1 reel in hook");
	bindings->add_keybinding(SDLK_c,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::JETPACK,      "key_Player1_jetpack",      "p1 Jetpack");
	bindings->add_keybinding(SDLK_1,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::WEAPON1,      "key_Player1_weapon1",      "p1 weapon 1");
	bindings->add_keybinding(SDLK_2,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::WEAPON2,      "key_Player1_weapon2",      "p1 weapon 2");
	bindings->add_keybinding(SDLK_3,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::WEAPON3,      "key_Player1_weapon3",      "p1 weapon 3");
	bindings->add_keybinding(SDLK_4,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::WEAPON4,      "key_Player1_weapon4",      "p1 weapon 4");
	bindings->add_keybinding(SDLK_5,           CMDFUNCTION::PLAYER1 | CMDFUNCTION::WEAPON5,      "key_Player1_weapon5",      "p1 weapon 5");
	bindings->add_keybinding(SDLK_PERIOD,      CMDFUNCTION::PLAYER1 | CMDFUNCTION::NEXTWEAPON,   "key_Player1_nextweapon",   "p1 next weapon");
	bindings->add_keybinding(SDLK_COMMA,       CMDFUNCTION::PLAYER1 | CMDFUNCTION::PREVWEAPON,   "key_Player1_prevweapon",   "p1 prev weapon");

	//player 2 keys
	bindings->add_keybinding(SDLK_KP5,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::UP,           "key_Player2_up",           "p2 Up");
	bindings->add_keybinding(SDLK_KP2,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::DOWN,         "key_Player2_down",         "p2 Down");
	bindings->add_keybinding(SDLK_KP1,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::LEFT,         "key_Player2_left",         "p2 Left");
	bindings->add_keybinding(SDLK_KP3,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::RIGHT,        "key_Player2_right",        "p2 Right");
	bindings->add_keybinding(SDLK_KP4,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::FIRE,         "key_Player2_fire",         "p2 Fire");
	bindings->add_keybinding(SDLK_KP7,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::JUMP,         "key_Player2_jump",         "p2 Jump");
	bindings->add_keybinding(SDLK_KP8,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::DIG,          "key_Player2_dig",          "p2 Dig");
	bindings->add_keybinding(SDLK_KP_MULTIPLY, CMDFUNCTION::PLAYER2 | CMDFUNCTION::HOOK_FIRE,    "key_Player2_hook_fire",    "p2 Fire hook");
	bindings->add_keybinding(SDLK_KP_DIVIDE,   CMDFUNCTION::PLAYER2 | CMDFUNCTION::HOOK_RELEASE, "key_Player2_hook_release", "p2 Release hook");
	bindings->add_keybinding(SDLK_KP_PLUS,     CMDFUNCTION::PLAYER2 | CMDFUNCTION::HOOK_IN,      "key_Player2_hook_in",      "p2 reel out hook");
	bindings->add_keybinding(SDLK_KP_MINUS,    CMDFUNCTION::PLAYER2 | CMDFUNCTION::HOOK_OUT,     "key_Player2_hook_out",     "p2 reel in hook");
	bindings->add_keybinding(SDLK_KP0,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::JETPACK,      "key_Player2_jetpack",      "p2 Jetpack");
	bindings->add_keybinding(SDLK_DELETE,      CMDFUNCTION::PLAYER2 | CMDFUNCTION::WEAPON1,      "key_Player2_weapon1",      "p2 weapon 1");
	bindings->add_keybinding(SDLK_HOME,        CMDFUNCTION::PLAYER2 | CMDFUNCTION::WEAPON2,      "key_Player2_weapon2",      "p2 weapon 2");
	bindings->add_keybinding(SDLK_PAGEUP,      CMDFUNCTION::PLAYER2 | CMDFUNCTION::WEAPON3,      "key_Player2_weapon3",      "p2 weapon 3");
	bindings->add_keybinding(SDLK_PAGEDOWN,    CMDFUNCTION::PLAYER2 | CMDFUNCTION::WEAPON4,      "key_Player2_weapon4",      "p2 weapon 4");
	bindings->add_keybinding(SDLK_END,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::WEAPON5,      "key_Player2_weapon5",      "p2 weapon 5");
	bindings->add_keybinding(SDLK_KP9,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::NEXTWEAPON,   "key_Player2_nextweapon",   "p2 next weapon");
	bindings->add_keybinding(SDLK_KP6,         CMDFUNCTION::PLAYER2 | CMDFUNCTION::PREVWEAPON,   "key_Player2_prevweapon",   "p2 prev weapon");

	//Optimized for a logitech 10 button gamepad
	//FIXME: joystick 0 is hardcoded to player 1
	bindings->add_joybuttonbinding(0, 0, CMDFUNCTION::PLAYER1 | CMDFUNCTION::JUMP, "key_Player1_jump", "p1 Jump");
	bindings->add_joybuttonbinding(0, 1, CMDFUNCTION::PLAYER1 | CMDFUNCTION::FIRE, "key_Player1_fire", "p1 Fire");
	bindings->add_joybuttonbinding(0, 2, CMDFUNCTION::PLAYER1 | CMDFUNCTION::DIG, "key_Player1_jetpack", "p1 Dig");
	bindings->add_joybuttonbinding(0, 3, CMDFUNCTION::PLAYER1 | CMDFUNCTION::JETPACK, "key_Player1_dig", "p1 Jetpack");
	bindings->add_joybuttonbinding(0, 4, CMDFUNCTION::PLAYER1 | CMDFUNCTION::PREVWEAPON, "key_Player1_prevweapon", "p1 prev weapon");
	bindings->add_joybuttonbinding(0, 5, CMDFUNCTION::PLAYER1 | CMDFUNCTION::NEXTWEAPON, "key_Player1_nextweapon", "p1 next weapon");
	bindings->add_joybuttonbinding(0, 6, CMDFUNCTION::PLAYER1 | CMDFUNCTION::HOOK_RELEASE, "key_Player1_hook_release", "p1 Release hook");
	bindings->add_joybuttonbinding(0, 7, CMDFUNCTION::PLAYER1 | CMDFUNCTION::HOOK_FIRE, "key_Player1_hook_fire", "p1 Fire hook");
	bindings->add_joybuttonbinding(0, 8, CMDFUNCTION::PLAYER1 | CMDFUNCTION::HOOK_OUT, "key_Player1_hook_out", "p1 reel out hook");
	bindings->add_joybuttonbinding(0, 9, CMDFUNCTION::PLAYER1 | CMDFUNCTION::HOOK_IN, "key_Player1_hook_in", "p1 reel in hook");

	bindings->add_mousebuttonbinding(1, CMDFUNCTION::PLAYER1 | CMDFUNCTION::FIRE, "key_Player1_fire", "p1 Fire");
	bindings->add_mousebuttonbinding(2, CMDFUNCTION::PLAYER1 | CMDFUNCTION::DIG, "key_Player1_dig", "p1 Jump");
	bindings->add_mousebuttonbinding(3, CMDFUNCTION::PLAYER1 | CMDFUNCTION::JUMP, "key_Player1_jump", "p1 Dig");
}

Evtmapper::~Evtmapper() {
	delete bindings;
}

bool Evtmapper::lookup (Nil_inputevent &inputevent) {
	if (!inputevent.sdl_inputevent) {
		inputevent.cmdfunction = CMDFUNCTION::NONE;
		return false;
	}

	switch(inputevent.sdl_inputevent->type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			Evtmapper_bindings::iterator i = bindings->find_by_sdlsym(((SDL_KeyboardEvent *) inputevent.sdl_inputevent)->keysym.sym);
			if (i == bindings->end()) {
				inputevent.cmdfunction = CMDFUNCTION::NONE;
				return false;
			}
			inputevent.cmdfunction = (*i)->cmdfunction;
			inputevent.pressed=(inputevent.sdl_inputevent->type==SDL_KEYDOWN);
			return true;
		}

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP: {
			SDL_MouseButtonEvent *evt=(SDL_MouseButtonEvent *) inputevent.sdl_inputevent;
			Evtmapper_bindings::iterator i = bindings->find_by_mouse_button(evt->button);
			if (i == bindings->end()) {
				inputevent.cmdfunction = CMDFUNCTION::NONE;
				return false;
			}
			inputevent.cmdfunction = (*i)->cmdfunction;
			inputevent.pressed = (inputevent.sdl_inputevent->type == SDL_MOUSEBUTTONDOWN);
			return true;
		}

		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP: {
			SDL_JoyButtonEvent *evt=(SDL_JoyButtonEvent *) inputevent.sdl_inputevent;
			Evtmapper_bindings::iterator i = bindings->find_by_joy_button(evt->which, evt->button);
			if (i == bindings->end()) {
				inputevent.cmdfunction = CMDFUNCTION::NONE;
				return false;
			}
			inputevent.cmdfunction = (*i)->cmdfunction;
			inputevent.pressed = (inputevent.sdl_inputevent->type == SDL_JOYBUTTONDOWN);
			return true;
		}

		case SDL_MOUSEMOTION:
		case SDL_JOYAXISMOTION: {
			// this is not a button, so no binding is found, but true as return value is needed
			inputevent.cmdfunction = CMDFUNCTION::NONE;
			return true;
		}
	} // switch

	// nothing found...
	inputevent.cmdfunction = CMDFUNCTION::NONE;
	return false;
}

void Evtmapper::init(Configuration *config_) {
	config_attach(config_);
	config_changed();
}

Evtmapper_bindings * Evtmapper::get_bindings() {
	return bindings;
};

// FIXME: split the parsing out from this loop
void Evtmapper::config_changed() {
	Evtmapper_bindings::iterator i = bindings->begin();
	while (i != bindings->end()) {
		//! after calling strtol() this is what was stored in val, right?
		char *endptr;
		const char *val=configuration->get_option((char *)(*i)->config_name.c_str());
		if(!val) {
			++i;
			continue;
		}
		int intkey=strtol(val, &endptr, 0);
		if(!*endptr) { // The whole thing was a number
			if (intkey>322 || intkey<-1)
				logmsg(lt_error, "Keysym out of bounds: %i", intkey);
			else if (intkey>=0) {
				logmsg(lt_debug, "Binding %i to %s", intkey, (*i)->config_name.c_str());
				(*i)->type = binding_key;
				(*i)->sdlsym = (SDLKey) intkey;
			}
		} else if(endptr==val) { // No number at the beginning.
			// Look for something of the form joyX_buttonY
			if(val[0]=='j' && val[1]=='o' && val[2]=='y') {
				int joystick=strtol(val+3, &endptr, 0);
				if(!*endptr || endptr==val) {
					logmsg(lt_error, "Badly formed binding: %s", val);
				} else {
					if(endptr[0]=='_' && endptr[1]=='b' && endptr[2]=='u' && endptr[3]=='t' && endptr[4]=='t' && endptr[5]=='o' && endptr[6]=='n') {
						const char *start2=endptr+7;
						int button=strtol(start2, &endptr, 0);
						if(*endptr || endptr==start2) {
							logmsg(lt_error, "Badly formed binding: %s", val);
						} else {
							logmsg(lt_debug, "Binding joystick %i button %i to %s", joystick, button, (*i)->config_name.c_str());
							(*i)->type=binding_joybutton;
							(*i)->joybuttoninfo.joystick=joystick;
							(*i)->joybuttoninfo.button=button;
						}
					} else
						logmsg(lt_error, "Badly formed binding: %s", val);
				}
			} else if(val[0]=='S' && val[1]=='D' && val[2]=='L' && val[3]=='K' && val[4]=='_') {
				//FIXME: bind key here
				logmsg(lt_debug, "Binding %s to %s", val, (*i)->config_name.c_str());
				//(*i)->type = binding_key;
				//(*i)->sdlsym = val;
			} else {
				logmsg(lt_error, "Badly formed binding: %s", val);
			}
		}
		else
			logmsg(lt_error, "Badly formed binding: %s", val);
		++i;
	}
}

void Evtmapper::bindkey(CMDFUNCTION::Function function, SDLKey sdlsym) {
	//see if this key is alreay in use somewhere
	Evtmapper_bindings::iterator i = bindings->find_by_sdlsym(sdlsym);
	if (i != bindings->end()) {

		//get the currently bound key for this function:
		Evtmapper_bindings::iterator j = bindings->find_by_function(function);
		if (j == bindings->end()) return;

		//bind the holder of the new key to the old key of the function to update
		char i_str[20];
		sprintf(i_str,"%i",(int)(*j)->sdlsym);

		//bind the specified function to the
		char j_str[20];
		sprintf(j_str,"%i",(int)sdlsym);

		configuration->set_option((char *)(*j)->config_name.c_str(),j_str);
		configuration->set_option((char *)(*i)->config_name.c_str(),i_str);
	} else {
		Evtmapper_bindings::iterator j = bindings->find_by_function(function);
		char j_str[20];
		sprintf(j_str,"%i",(int)sdlsym);
		configuration->set_option((char *)(*j)->config_name.c_str(),j_str);
	}
}
