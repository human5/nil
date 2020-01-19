/***************************************************************************
  obj_weapon_megashotgun.cpp  -  the mega shotgun
  ---------------------------------------------------------------------------
  begin                : 2000-01-02
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/misc.h"
#include "common/objects/obj_weapon_megashotgun.h"
#include "common/objects/obj_projectile.h"

Obj_weapon_megashotgun::Obj_weapon_megashotgun(World *world_, int id) :Obj_weapon(world_, id) {
	reload_speed        = 0.1;
	shot_cost           = 0.66;
	reshoot_time        = 1.0;
	knock_back          = 100.0;
	shot_speed          = 1;
	bullet_speed        = 400;
	bullet_speed_spread = 300;
	bullet_angle_spread = 175;
	subshots            = 75;
}

Obj_weapon_megashotgun::~Obj_weapon_megashotgun() {}

void Obj_weapon_megashotgun::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;

	//FIXME: soundfile needed
	world->startSound( "sound/bexpl.wav" , 0, 1, x, y );

	for (int c = 0; c < subshots; c++)
	{
		float bullet_angle = aim_angle + rnd (-bullet_angle_spread, bullet_angle_spread)/1000.0;
		Vector bullet_vector = Math::rad2vector(bullet_angle);
		bullet_vector.scale(rnd((1000-bullet_speed_spread/3)*bullet_speed,(1000+bullet_speed_spread)*bullet_speed)/1000.0);

		knock_back_direction.add(bullet_vector);
		
	 	Obj_projectile *bullet = (Obj_projectile *)serverworld->newobject(ot_projectile);
		if (bullet) {
			bullet->init(ownerid, x, y, xspeed+bullet_vector.x, yspeed+bullet_vector.y, 30, 8, 2, 0);
		}
	}
}
