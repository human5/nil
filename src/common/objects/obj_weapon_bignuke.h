/***************************************************************************
  obj_weapon_bignuke.h  -  Header for the big nuke
 ---------------------------------------------------------------------------
  begin                : 2000-02-13
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Anders Rosendal
  email                : qute@klog.dk

 ***************************************************************************/

#ifndef OBJ_WEAPON_BIGNUKE_H
#define OBJ_WEAPON_BIGNUKE_H

#include "obj_weapon.h"

//! @author Anders Rosendal
//! @brief A big nuke
class Obj_weapon_bignuke : public Obj_weapon {
public: 
	//! Constructor
	Obj_weapon_bignuke(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_bignuke();
	//! return ot_weapon_bignuke as type
	virtual Object_type gettype() { return ot_weapon_bignuke; }
	//! fires the weapon
	void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif

