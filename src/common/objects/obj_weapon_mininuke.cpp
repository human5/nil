/***************************************************************************
  obj_weapon_mininuke.cpp  -  the mininuke
  ---------------------------------------------------------------------------
  begin                : 2000-02-21
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Anders Rosendal
  email                : qute@klog.dk

 ***************************************************************************/

#include "common/objects/obj_weapon_mininuke.h"
#include "common/objects/obj_bignuke.h"
#include "common/sound/soundsystem.h"
#include "server/serverworld.h"

Obj_weapon_mininuke::Obj_weapon_mininuke(World *world_, int id) : Obj_weapon(world_,id) {
	reload_speed        = 0.4;
	shot_cost           = 0.1;
	reshoot_time        = 1.0;
	knock_back          = 0.5;
	shot_speed          = 0.3;
	bullet_speed        = 500;
	//bullet_speed_spread = 10;
	//bullet_angle_spread = 40;
	subshots            = 25;
}

Obj_weapon_mininuke::~Obj_weapon_mininuke() {}

void Obj_weapon_mininuke::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;
	aim_vector.scale(bullet_speed);

	knock_back_direction.add(aim_vector);

	world->startSound("sound/grenade_pin.wav", 0, 1, x, y);

 	Obj_bignuke *nuke = (Obj_bignuke *)serverworld->newobject(ot_bignuke);
	if (nuke) {
		nuke->init(ownerid, x, y, xspeed+aim_vector.x, yspeed+aim_vector.y, subshots, 100);
	}
}
