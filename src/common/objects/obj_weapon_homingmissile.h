/***************************************************************************
  obj_weapon_homingmissile.cpp  -  header for the homing missile
 ---------------------------------------------------------------------------
  begin                : 2003-07-17
  by                   : Uwe Fabricius
  email                : uwe.f@bricius.de

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2003 by Uwe Fabricius
  email                : uwe.f@bricius.de

 ***************************************************************************/

#ifndef OBJ_WEAPON_HOMIMG_MISSILE_H
#define OBJ_WEAPON_HOMIMG_MISSILE_H

#include "obj_weapon.h"

//! @author Uwe Fabricius
//! @brief The Flamethrower
class Obj_weapon_homingmissile : public Obj_weapon {
public:
	//! Constructor
	Obj_weapon_homingmissile(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_homingmissile();
	//! returns ot_weapon_homingmissile as type
	virtual Object_type gettype() { return ot_weapon_homingmissile; }
protected:
	//! fires the weapon
	void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif // OBJ_WEAPON_HOMIMG_MISSILE_H
