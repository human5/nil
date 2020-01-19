/***************************************************************************
  obj_weapon_flamethrower.h  -  Header for the flamethrower
 ---------------------------------------------------------------------------
  begin                : 1999-12-20
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_WEAPON_FLAMETHROWER_H
#define OBJ_WEAPON_FLAMETHROWER_H

#include "obj_weapon.h"

//! @author Flemming Frandsen
//! @brief A flamethrower
class Obj_weapon_flamethrower : public Obj_weapon {
public:
	//! Constructor
	Obj_weapon_flamethrower(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_flamethrower();
	//! returns ot_weapon_flamethrower as type
	virtual Object_type gettype() { return ot_weapon_flamethrower; }
protected:
	//! fires the weapon
	virtual void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif












