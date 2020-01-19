/***************************************************************************
  obj_weapon_larpa.h  -  Header for the larpa (what ever it my be ...)
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

#ifndef OBJ_WEAPON_LARPA_H
#define OBJ_WEAPON_LARPA_H

#include "obj_weapon.h"

//! @author Flemming Frandsen
//! @brief The larpa
class Obj_weapon_larpa : public Obj_weapon {
public:
	//! Constructor
	Obj_weapon_larpa(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_larpa();
	//! returns ot_weapon_larpa as type
	virtual Object_type gettype() { return ot_weapon_larpa; }
protected:
	//! fires the weapon
	void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif
