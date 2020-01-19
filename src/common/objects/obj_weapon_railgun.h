/***************************************************************************
  obj_weapon_railgun.h  -  Header for the railgun
  ---------------------------------------------------------------------------
  begin                : 2000-01-05
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_WEAPON_RAILGUN_H
#define OBJ_WEAPON_RAILGUN_H

#include "obj_weapon.h"

//! @author Flemming Frandsen
//! @brief The railgun 
class Obj_weapon_railgun : public Obj_weapon  {
public: 
	//! Constructor
	Obj_weapon_railgun(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_railgun();
	//! returns ot_weapon_railgun as type
	virtual Object_type gettype() { return ot_weapon_railgun; }

protected:
	//! fires the weapon
	virtual void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif
