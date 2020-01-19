/***************************************************************************
  obj_weapon_shotgun.h  -  Header for the shotgun
  ---------------------------------------------------------------------------
  begin                : 1999-12-23
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_WEAPON_SHOTGUN_H
#define OBJ_WEAPON_SHOTGUN_H

#include "obj_weapon.h"

//! @author Flemming Frandsen
//! @brief The shotgun
class Obj_weapon_shotgun : public Obj_weapon {
public:
	//! Constructor
	Obj_weapon_shotgun(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_shotgun();
	//! returns ot_weapon_shotgun as type
	virtual Object_type gettype() { return ot_weapon_shotgun; }

protected:
	//! fires the weapon
	virtual void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif
