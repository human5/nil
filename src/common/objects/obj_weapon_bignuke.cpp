/***************************************************************************
  obj_weapon_bignuke.cpp  -  a big nuke
 ---------------------------------------------------------------------------
  begin                : 1999-12-18
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Anders Rosendal
  email                : qute@klog.dk

 ***************************************************************************/

#include "obj_weapon_bignuke.h"
#include "obj_bignuke.h"

Obj_weapon_bignuke::Obj_weapon_bignuke(World *world_, int id) :Obj_weapon(world_,id) {
	reload_speed        = 0.2;
	shot_cost           = 0.0;
	shot_speed          = 0.3;
	reshoot_time        = 0.1;
	knock_back          = 1.0;
	shot_speed          = 0.3;
	bullet_speed        = 500;
	//bullet_speed_spread = 60;
	//bullet_angle_spread = 20;
	subshots            = 200;
}

Obj_weapon_bignuke::~Obj_weapon_bignuke() {}

void Obj_weapon_bignuke::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;
	aim_vector.scale(bullet_speed);

	knock_back_direction.add(aim_vector);

	//FIXME: Play the sound for the weapon
	//world->startSound( "sound/NAME.wav" , 0, 1, x, y );

	Obj_bignuke *nuke = (Obj_bignuke *)serverworld->newobject(ot_bignuke);
	if (nuke) {
		nuke->init(ownerid, x, y, xspeed + aim_vector.x, yspeed + aim_vector.y, subshots, 100);
	}
}

