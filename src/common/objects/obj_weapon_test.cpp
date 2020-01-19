/***************************************************************************
  obj_weapon_test.cpp  -  the test weapon
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

#include "obj_weapon_test.h"
#include "obj_bignuke.h"

Obj_weapon_test::Obj_weapon_test(World *world_, int id) :Obj_weapon(world_,id) {
	reload_speed        = 1;
	shot_cost           = 0.5;
	reshoot_time        = 1.0;
	knock_back          = 1.0;
	//shot_speed          = 1;
	bullet_speed        = 500;
	//bullet_speed_spread = 200;
	//bullet_angle_spread = 150;
	subshots            = 40;
}

Obj_weapon_test::~Obj_weapon_test() {}

void Obj_weapon_test::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;
	aim_vector.scale(bullet_speed);

	knock_back_direction.add(aim_vector);

	Obj_bignuke *nuke = (Obj_bignuke *)serverworld->newobject(ot_bignuke);
	if (nuke && nuke->gettype() == ot_bignuke) {
		nuke->init(ownerid, x, y, xspeed+aim_vector.x, yspeed+aim_vector.y, 30, 100);
	}
}
