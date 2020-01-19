/***************************************************************************
  obj_weapon_jetpack.h  - header for the jetpack weapon
  ---------------------------------------------------------------------------
  begin                : 2002-??-??
  by                   : Uwe Fabricius, Thomas Pohl
  email                : ???
    
  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2002 by Uwe Fabricius, Thomas Pohl
  email                : ???

 ***************************************************************************/


#ifndef OBJ_WEAPON_JETPACK_H
#define OBJ_WEAPON_JETPACK_H

#include "obj_weapon.h"

//! @author Uwe Fabricius, Thomas Pohl
//! @brief The jetpack
class Obj_weapon_jetpack : public Obj_weapon {
public:
	//! Constructor
	Obj_weapon_jetpack( World *world_, int id );
	//! Destructor
	virtual ~Obj_weapon_jetpack();
	//! returns ot_weapon_jetpack as type
	virtual Object_type gettype() { return ot_weapon_jetpack; }
	//! this is for handling client side effects of weapons, like jetpack smoke etc. 
	virtual void clientFire(float x, float y, float aim_angle);

protected:
	//! fires the weapon
	virtual void dofire(int x, int y, Vector aim_vector, float  aim_angle);
};

#endif
