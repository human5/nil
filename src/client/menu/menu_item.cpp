/***************************************************************************
  menu_item.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-20
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)
  
  last changed         : 2004-10-23
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : make draw_item work

  last changed         : 2004-01-06
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Cleanups and add centered menu
  
  last changed         : 2005-01-15
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : use event_target's width and height instead
                         of our own
  
  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : use get_text_geom()'s return value

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)
                        2004-2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "client/menu/menu_item.h"
#include "common/console/logmsg.h"

Menu_item::Menu_item(Font_instance *font_) {
	active = false;
	type = it_notype;
	font = font_;
	strcpy(label, "NOT GIVEN!");
	calculate_geom(label, font);
}

Menu_item::Menu_item(int type_, Font_instance *font_) {
	active = false;
	type = type_;
	font = font_;
	strcpy(label, "NOT GIVEN!");
	calculate_geom(label, font);
}

Menu_item::Menu_item(int type_, const char* label_, Font_instance *font_) {
	active = false;
	type = type_;
	font = font_;
	strncpy(label, label_, MAX_MENU_ITEM_LEN);
	if (strlen(label_) > MAX_MENU_ITEM_LEN) {
		logmsg(lt_fatal, "Label is to long, cutting of after 80 chars!");
		label[MAX_MENU_ITEM_LEN-1] = 0x00;
	}
	calculate_geom(label, font);
}

Menu_item::~Menu_item() {
	free(label);
}

bool Menu_item::handle(Nil_inputevent *event) {
	return false;
}
bool Menu_item::handle_keyboard_key(Event_keyboard_key *key_event) {
	return false;
}
bool Menu_item::handle_mouse_key(Event_mouse_key *mouse_key_event) {
	return false;
}
bool Menu_item::handle_mouse_motion(Event_mouse_motion *mouse_motion_event) {
	return false;
}
bool Menu_item::handle_joystick_key(Event_joystick_key *joystick_key_event) {
	return false;
}
bool Menu_item::handle_joystick_motion(Event_joystick_motion *joystick_motion_event) {
	return false;
}

void Menu_item::calculate_geom(char* text, Font_instance *font) {
	int width, height;
	if (!font->get_text_geom(text, &width, &height)) {
		// ouch, font rendering failed, let's imagine some values
		width = 180;
		height = 20;
	}
	setsize(width, height);
}

void Menu_item::setState(bool state) {
	active = state;
}

bool Menu_item::getState() {
	return active;
}

void Menu_item::setType(int type_) {
	type = type_;
}

int Menu_item::getType() {
	return type;
}

void Menu_item::setLabel(char* label_) {
	strncpy(label, label_, MAX_MENU_ITEM_LEN);
	if (strlen(label_) >= MAX_MENU_ITEM_LEN) {
		label[MAX_MENU_ITEM_LEN-1] = '\0';
	}
}

char* Menu_item::getLabel() {
	return label;
}

void Menu_item::draw_item(Mutable_raw_surface *target, int xpos_, int ypos_) {
	xpos = xpos_;
	ypos = ypos_;
	/*switch(type) {
		case it_int:
			font->put_text(xpos+1-20, ypos+1, "i", target, true, 0,0,0 );
			font->put_text(xpos-20, ypos, "i", target, true, 255,255,255 );
			break;
		case it_string:
			font->put_text(xpos+1-20, ypos+1, "s", target, true, 0,0,0 );
			font->put_text(xpos-20, ypos, "s", target, true, 255,255,255 );
			break;
		case it_submenu:
			font->put_text(xpos+1-20, ypos+1, "S", target, true, 0,0,0 );
			font->put_text(xpos-20, ypos, "S", target, true, 255,255,255 );
			break;
		default:
			font->put_text(xpos+1-20, ypos+1, "D", target, true, 0,0,0 );
			font->put_text(xpos-20, ypos, "D", target, true, 255,255,255 );
			break;
		}*/
	font->put_text(xpos+1, ypos+1, (const char *)label, target, true, 0,0,0 );
	if (type == it_sepstart) {
		font->put_text(xpos+3, ypos+3, label, target, true, 0,0,0 );
	}
	if (active)
		font->put_text(xpos, ypos, label, target, true, 255,255,255 );
	else
		font->put_text(xpos, ypos, label, target, true, 180,180,180 );
}
