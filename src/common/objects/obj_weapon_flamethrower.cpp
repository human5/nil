/***************************************************************************
  obj_weapon_flamethrower.cpp  -  a flamethrower
 ---------------------------------------------------------------------------
  begin                : 1999-12-20
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "obj_weapon_flamethrower.h"
#include "obj_fireball.h"

Obj_weapon_flamethrower::Obj_weapon_flamethrower(World *world_, int id) :Obj_weapon(world_, id) {
	reload_speed        = 0.1;
	shot_cost           = 0.01;
	reshoot_time        = 0.075;
	knock_back          = 3.0;
	shot_speed          = 1;
	bullet_speed        = 80;
	bullet_speed_spread = 10;
	bullet_angle_spread = 40;
	subshots            = 4;
}

Obj_weapon_flamethrower::~Obj_weapon_flamethrower() {}

void Obj_weapon_flamethrower::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;

	for (int c = 0; c < subshots; c++) {
		float bullet_angle = aim_angle + rnd (-bullet_angle_spread, bullet_angle_spread)/1000.0;
		Vector bullet_vector = Math::rad2vector(bullet_angle);
		bullet_vector.scale(rnd((1000-bullet_speed_spread/3)*bullet_speed,(1000+bullet_speed_spread)*bullet_speed)/1000.0);

		knock_back_direction.add(bullet_vector);

		//FIXME: Play the sound for the weapon
		//world->startSound( "sound/NAME.wav" , 0, 1, x, y );

		Obj_fireball *bullet = (Obj_fireball *)serverworld->newobject(ot_fireball);
		if (bullet) {
			bullet->init(ownerid, x, y, xspeed+bullet_vector.x, yspeed+bullet_vector.y, 6, 7, 2, 1);
		}
	}
}
