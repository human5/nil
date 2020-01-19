/***************************************************************************
  keymapper_bindings.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-11
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-05-17
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : add joystick and mouse bindings support

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "client/controls/keymapper_bindings.h"
#include "client/event/event.h"

Evtmapper_bindings::~Evtmapper_bindings() {
	iterator i=begin();
	while (i != end()) {
		delete *i;
		i++;
	}
}	

void Evtmapper_bindings::add_keybinding(SDLKey sdlsym, int cmdfunction, const char *config_name, const char *display_name) {
	Evtmapper_binding *newbinding = new Evtmapper_binding;

	newbinding->type = binding_key;
	newbinding->sdlsym = sdlsym;

	newbinding->cmdfunction = (CMDFUNCTION::Function)cmdfunction;
	newbinding->config_name = config_name;
	newbinding->display_name = display_name;
	push_back(newbinding);
}

void Evtmapper_bindings::add_joybuttonbinding(int joystick, int button, int cmdfunction, const char *config_name, const char *display_name) {
	Evtmapper_binding *newbinding = new Evtmapper_binding;

	newbinding->type=binding_joybutton;
	newbinding->joybuttoninfo.joystick=joystick;
	newbinding->joybuttoninfo.button=button;

	newbinding->cmdfunction = (CMDFUNCTION::Function)cmdfunction;
	newbinding->config_name = config_name;
	newbinding->display_name = display_name;
	push_back(newbinding);
}

void Evtmapper_bindings::add_mousebuttonbinding(int button, int cmdfunction, const char *config_name, const char *display_name) {
	Evtmapper_binding *newbinding = new Evtmapper_binding;

	newbinding->type=binding_mousebutton;
	newbinding->mousebuttoninfo.button=button;

	newbinding->cmdfunction = (CMDFUNCTION::Function)cmdfunction;
	newbinding->config_name = config_name;
	newbinding->display_name = display_name;
	push_back(newbinding);
}

Evtmapper_bindings::iterator Evtmapper_bindings::find_by_sdlsym(SDLKey sdlsym) {
	iterator i=begin();
	while (i != end()) {
		if ((*i)->type==binding_key && (*i)->sdlsym == sdlsym)
			return i;
		i++;
	}
	return end();
}

Evtmapper_bindings::iterator Evtmapper_bindings::find_by_joy_button(int joy, int button) {
	iterator i=begin();
	while (i != end()) {
		if ((*i)->type==binding_joybutton && (*i)->joybuttoninfo.joystick==joy && (*i)->joybuttoninfo.button==button)
			return i;
		i++;
	}
	return end();
}

Evtmapper_bindings::iterator Evtmapper_bindings::find_by_mouse_button(int button) {
	iterator i=begin();
	while (i != end()) {
		if ((*i)->type==binding_mousebutton && (*i)->mousebuttoninfo.button==button)
			return i;
		i++;
	}
	return end();
}

Evtmapper_bindings::iterator Evtmapper_bindings::find_by_function(CMDFUNCTION::Function cmdfunction) {
	iterator i=begin();
	while (i != end()) {
		if ((*i)->cmdfunction == cmdfunction)
			return i;
		i++;
	}
	return end();
}
