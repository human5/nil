/***************************************************************************
  obj_weapon_shotgun.cpp  -  the shotgun
  ---------------------------------------------------------------------------
  begin                : 1999-12-23
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_weapon_shotgun.h"
#include "common/sound/soundsystem.h"
#include "common/objects/obj_projectile.h"

Obj_weapon_shotgun::Obj_weapon_shotgun(World *world_, int id) :Obj_weapon(world_, id) {
	reload_speed        = 0.2;
	shot_cost           = 0.33;
	reshoot_time        = 1.0;
	knock_back          = 50.0;
	//shot_speed          = 1;
	bullet_speed        = 300;
	bullet_speed_spread = 200;
	bullet_angle_spread = 150;
	subshots            = 40;
}

Obj_weapon_shotgun::~Obj_weapon_shotgun() {}

void Obj_weapon_shotgun::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;

	//FIXME: soundfile needed
	world->startSound( "sound/explode.wav" , 0, 1, x, y );

	for (int c=0;c<subshots;c++) {

		float bullet_angle = aim_angle+rnd(-bullet_angle_spread,bullet_angle_spread)/1000.0;
		Vector bullet_vector = Math::rad2vector(bullet_angle);
		bullet_vector.scale(rnd((1000-bullet_speed_spread/3)*bullet_speed,(1000+bullet_speed_spread)*bullet_speed)/1000.0);

		knock_back_direction.add(bullet_vector);
		
	 	Obj_projectile *bullet = (Obj_projectile *)serverworld->newobject(ot_projectile);
		if (bullet) {
			bullet->init(ownerid, x, y, xspeed+bullet_vector.x, yspeed+bullet_vector.y, 20, 8, 1, 0);
		}
	}
}
