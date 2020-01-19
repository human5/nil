/***************************************************************************
  obj_weapon_minigun.h  -  Header for the minigun
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

#ifndef OBJ_WEAPON_MINIGUN_H
#define OBJ_WEAPON_MINIGUN_H

#include "obj_weapon.h"

//! @author Flemming Frandsen
//! @brief The minigun
class Obj_weapon_minigun : public Obj_weapon {
public:
	//! Constructor
	Obj_weapon_minigun(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_minigun();
	//! returns ot_weapon_minigun as type
	virtual Object_type gettype(){return ot_weapon_minigun;}

protected:
	//! fires the weapon
	virtual void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif
