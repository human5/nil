/***************************************************************************
  obj_weapon_minigun.cpp  -  the minigun
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

#include "common/objects/obj_weapon_minigun.h"
#include "common/objects/obj_projectile.h"
#include "common/sound/soundsystem.h"

Obj_weapon_minigun::Obj_weapon_minigun(World *world_, int id) :Obj_weapon(world_, id) {
	reload_speed        = 0.2;
	shot_cost           = 0.05;
	reshoot_time        = 0.0;
	knock_back          = 5.0;
	shot_speed          = 1;
	bullet_speed        = 200;
	bullet_speed_spread = 10;
	bullet_angle_spread = 40;
	subshots            = 4;
}

Obj_weapon_minigun::~Obj_weapon_minigun() {}

void Obj_weapon_minigun::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;
	float frame_delta = world->get_last_frame_delta();

	//FIXME: maybe play another sound for every 3 bullets?
	world->startSound("sound/machgun.wav", 0, 1, x, y);

	for (int c=0; c<subshots; c++) {
		float bullet_angle = aim_angle+rnd(-bullet_angle_spread,bullet_angle_spread)/1000.0;
		Vector bullet_vector = Math::rad2vector(bullet_angle);
		bullet_vector.scale(rnd((1000-bullet_speed_spread/3)*bullet_speed,(1000+bullet_speed_spread)*bullet_speed)/1000.0);

		knock_back_direction.add(bullet_vector);

		// no barrel correction needed anymore, this is handled by mLeaveOwnerID
		Obj_projectile *bullet = (Obj_projectile *)serverworld->newobject(ot_projectile);
		if (bullet) {
			bullet->init(ownerid, x, y, xspeed+bullet_vector.x, yspeed+bullet_vector.y, 20, 7, 2, frame_delta*c/(subshots*1.0));
		}
	}
}
