/***************************************************************************
  obj_weapon_mininuke.h  -  Header for the mininuke
  ---------------------------------------------------------------------------
  begin                : 2000-02-21
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Anders Rosendal
  email                : qute@klog.dk

 ***************************************************************************/

#ifndef OBJ_WEAPON_MININUKE_H
#define OBJ_WEAPON_MININUKE_H

#include "obj_weapon.h"

//! @author Andreas Rosendal
//! @brief The mininuke
class Obj_weapon_mininuke : public Obj_weapon {
public: 
	//! Constructor
	Obj_weapon_mininuke(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_mininuke();
	//! returns ot_weapon_mininuke as type
	virtual Object_type gettype(){return ot_weapon_mininuke;}
protected:
	//! fires the weapon
	void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif

