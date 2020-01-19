/***************************************************************************
  obj_weapon_rocket.cpp  -  Header for the rocket launcher
  ---------------------------------------------------------------------------
  begin                : 2000-02-22
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/


#ifndef OBJ_WEAPON_ROCKET_H
#define OBJ_WEAPON_ROCKET_H

#include "obj_weapon.h"

//! @author Flemming Frandsen
//! @brief The rocket launcher
class Obj_weapon_rocket : public Obj_weapon {
public:
	//! Constructor
	Obj_weapon_rocket(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_rocket();
	//! returns ot_weapon_tocket as type
	virtual Object_type gettype() { return ot_weapon_rocket; }
protected:
	//! fires the weapon
	void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif
