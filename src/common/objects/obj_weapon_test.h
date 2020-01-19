/***************************************************************************
  obj_weapon_test.h  -  Header for the test weapon
  ---------------------------------------------------------------------------
  begin                : 1999-12-18
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/


#ifndef OBJ_WEAPON_TEST_H
#define OBJ_WEAPON_TEST_H

#include "obj_weapon.h"

//! @author Flemming Frandsen
//! @brief A test weapon
class Obj_weapon_test : public Obj_weapon  {
public:
	//! Constructor
	Obj_weapon_test(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_test();
	//! returns ot_weapon_test as type
	virtual Object_type gettype() { return ot_weapon_test; }
protected:
	//! fires the weapon
	virtual void dofire(int x, int y, Vector aim_vector, float aim_angle);
	//! ???
	int timer;
};

#endif
