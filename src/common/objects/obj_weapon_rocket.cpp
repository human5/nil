/***************************************************************************
  obj_weapon_rocket.cpp  -  the rocket launcher
  ---------------------------------------------------------------------------
  begin                : 2000-02-25
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk
  
 ***************************************************************************/

#include "server/serverworld.h"
#include "common/sound/soundsystem.h"
#include "common/objects/obj_weapon_rocket.h"
#include "common/objects/obj_rocket.h"

Obj_weapon_rocket::Obj_weapon_rocket(World *world_, int id) :Obj_weapon(world_, id) {
	reload_speed        = 0.4;
	shot_cost           = 0.2;
	reshoot_time        = 0.45;
	knock_back          = 0.0;
	shot_speed          = 1;
	bullet_speed        = 50;
	//bullet_speed_spread = 60;
	//bullet_angle_spread = 20;
	//subshots            = 10;
}

Obj_weapon_rocket::~Obj_weapon_rocket() {}

void Obj_weapon_rocket::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;
	Vector start_speed = aim_vector;
	start_speed.scale(bullet_speed);

	//knock_back_direction.add(knockBack);

	world->startSound( "sound/rocket.wav" , 0, 1, x, y );

	Obj_rocket *rocket = (Obj_rocket *)serverworld->newobject(ot_rocket);
	if (rocket) {
		rocket->init(ownerid, x, y, xspeed + start_speed.x, yspeed + start_speed.y, 0.0, 750, 16, aim_vector);
	}
}
