/***************************************************************************
  obj_weapon_clusterbomb.h  -  Header for the clusterbomb
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

#ifndef OBJ_WEAPON_clusterbomb_H
#define OBJ_WEAPON_clusterbomb_H

#include "obj_weapon.h"

//! @author Anders Rosendal
//! @brief A clusterbomb
class Obj_weapon_clusterbomb : public Obj_weapon {
public:
	//! Constructor
	Obj_weapon_clusterbomb(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon_clusterbomb();
	//! return ot_weapon_clusterbomb as type
	virtual Object_type gettype() { return ot_weapon_clusterbomb; }
	//! fires the weapon
	void dofire(int x, int y, Vector aim_vector, float aim_angle);
};

#endif

