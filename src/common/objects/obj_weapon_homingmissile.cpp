/***************************************************************************
  obj_weapon_homingmissile.cpp  -  the homing missile
 ---------------------------------------------------------------------------
  begin                : 2003-07-17
  by                   : Uwe Fabricius
  email                : ???

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2003 by Uwe Fabricius
  email                : ???

 ***************************************************************************/

#include "server/serverworld.h"
#include "common/sound/soundsystem.h"
#include "common/objects/obj_weapon_homingmissile.h"
#include "common/objects/obj_homingmissile.h"

Obj_weapon_homingmissile::Obj_weapon_homingmissile(World *world_, int id) : Obj_weapon(world_, id) {
	reload_speed        = 0.3;
	shot_cost           = 1.0;
	reshoot_time        = 0.45;
	knock_back          = 0.0;
	shot_speed          = 1;
	bullet_speed        = 50;
	//bullet_speed_spread = 10;
	//bullet_angle_spread = 40;
	//subshots            = 4;
}

Obj_weapon_homingmissile::~Obj_weapon_homingmissile() {}

void Obj_weapon_homingmissile::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;
	Vector start_speed = aim_vector;
	start_speed.scale( bullet_speed );

	knock_back_direction.add(aim_vector);

	world->startSound( "sound/rocket.wav" , 0, 1, x, y );

	Obj_homingmissile *missile = (Obj_homingmissile*)serverworld->newobject(ot_homingmissile);
	if (missile) {
		missile->init(ownerid, x, y, xspeed + start_speed.x, yspeed + start_speed.y, 0.0, 750, 16, aim_vector);
	}
}
