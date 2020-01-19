/***************************************************************************
  obj_weapon_plasmagun.cpp  -  the plasmagun
  ---------------------------------------------------------------------------
  begin                : 2000-02-03
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_WEAPON_PLASMAGUN_H
#define OBJ_WEAPON_PLASMAGUN_H

#include "obj_weapon.h"

//! @author Flemming Frandsen
//! @brief The plasmagun
class Obj_weapon_plasmagun : public Obj_weapon {
public: 
	//! Constructor
	Obj_weapon_plasmagun(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_plasmagun();
	//! returns ot_weapon_plasmagun as type
	virtual Object_type gettype(){ return ot_weapon_plasmagun; }

protected:
	//! fires the weapon
	virtual void dofire(int x, int y, Vector aim_vector, float aim_angle);
};


#endif

