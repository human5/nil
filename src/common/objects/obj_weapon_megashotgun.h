/***************************************************************************
  obj_weapon_megashotgun.h  -  Header for the mega shotgun
  ---------------------------------------------------------------------------
  begin                : 2000-01-02
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_WEAPON_MEGASHOTGUN_H
#define OBJ_WEAPON_MEGASHOTGUN_H

#include "obj_weapon.h"

//! @author Flemming Frandsen
//! @brief The mega shotgun
class Obj_weapon_megashotgun : public Obj_weapon {
public:
	//! Constructor
	Obj_weapon_megashotgun(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_megashotgun();
	//! returns ot_weapon_megashotgun as type
	virtual Object_type gettype(){return ot_weapon_megashotgun;}

protected:
	//! fires the weapon
	virtual void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif
