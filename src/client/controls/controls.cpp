/***************************************************************************
  controls.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-30
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2005-01-03
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : directory move
                         remove code from header

  copyright            : (C) 1999 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#include "controls.h"

Controls::Controls() {
	elevation = (MINELEVATION+MAXELEVATION)/2; 
	controlflags = 0; 
	weapon=0; 
	owner=-1; 
}

void Controls::set_flag(int flag, bool state) {
	if (state)
		controlflags |= flag;
	else
		controlflags &= ~flag;
}

void Controls::clear_flag(int flag) {
	controlflags &= ~flag;
}

bool Controls::get_flag(int flag) {
	return (controlflags & flag) != 0;
}

float Controls::get_elevation() {
	return elevation;
}

void Controls::set_elevation(float elev) {
	elevation = elev;
	if (elevation < MINELEVATION)
		elevation = MINELEVATION;
	else if (elevation > MAXELEVATION)
		elevation = MAXELEVATION;
}

char Controls::get_weapon() {
	return weapon;
}

void Controls::set_weapon(char weapon_) {
	weapon = weapon_;
}

void Controls::clear_impulses() {
	controlflags &= cf_not_impulse;
}

void Controls::clear_all() {
	controlflags = 0;
}

void Controls::serialize(Serializer *serializer) {
	serializer->rw(owner);
	serializer->rw(controlflags);
	serializer->rw(elevation);
	serializer->rw(weapon);
	//if we are setting the state of the controls object do some extra checking
	if (!serializer->is_reading()) {
		if (elevation < MINELEVATION)
			elevation = MINELEVATION;
		else if (elevation > MAXELEVATION)
			elevation = MAXELEVATION;
	}
}

void Controls::print(char *msg) {
	logmsg(lt_debug, "%s * controlflags: %x, elevation: %.2f, weapon: %i", msg, controlflags, elevation, weapon);
}

void Controls::set_owner(char new_owner) {
	owner = new_owner;
}

char Controls::get_owner() {
	return owner;
}
