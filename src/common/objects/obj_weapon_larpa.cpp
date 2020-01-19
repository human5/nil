/***************************************************************************
  obj_weapon_larpa.cpp  -  the larpa (what ever it my be ...)
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

#include "obj_weapon_larpa.h"
#include "obj_larpa.h"

Obj_weapon_larpa::Obj_weapon_larpa(World *world_, int id) :Obj_weapon(world_,id) {
	reload_speed        = 0.4;
	shot_cost           = 0.2;
	reshoot_time        = 0.45;
	knock_back          = 2.0;
	shot_speed          = 0.7;
	bullet_speed        = 80;
	//bullet_speed_spread = 60;
	//bullet_angle_spread = 20;
	//subshots            = 10;
}

Obj_weapon_larpa::~Obj_weapon_larpa() {}

void Obj_weapon_larpa::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;
	aim_vector.scale(bullet_speed);

	knock_back_direction.add(aim_vector);

	//FIXME: Play the sound for the weapon
	//world->startSound( "sound/NAME.wav" , 0, 1, x, y );
               
	Obj_larpa *larpa = (Obj_larpa *)serverworld->newobject(ot_larpa);
	if (larpa) {
		larpa->init(ownerid, x, y, xspeed+aim_vector.x, yspeed+aim_vector.y, 0.05, 200, 11);
	}
}
