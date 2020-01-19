/***************************************************************************
  obj_weapon_railgun.cpp  -  the railgun
  ---------------------------------------------------------------------------
  begin                : 2000-01-05
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "obj_weapon_railgun.h"
#include "obj_railslug.h"

Obj_weapon_railgun::Obj_weapon_railgun(World *world_, int id) : Obj_weapon(world_, id) {
	reload_speed        = 0.15;
	shot_cost           = 0.99;
	reshoot_time        = 0.4;
	knock_back          = 10.0;
	shot_speed          = 1;
	bullet_speed        = 2000;
	//bullet_speed_spread = 60;
	//bullet_angle_spread = 20;
	//subshots            = 10;
}

Obj_weapon_railgun::~Obj_weapon_railgun() {}

void Obj_weapon_railgun::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;
	aim_vector.scale(bullet_speed);

	knock_back_direction.add(aim_vector);
	
	world->startSound( "sound/railgun.wav" , 0, 1, x, y );
	
 	Obj_railslug *slug = (Obj_railslug *)serverworld->newobject(ot_railslug);
	if (slug)
	{
		slug->init(ownerid, x, y, xspeed+aim_vector.x, yspeed+aim_vector.y);
	}
}

