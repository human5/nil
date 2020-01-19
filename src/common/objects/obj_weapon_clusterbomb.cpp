/***************************************************************************
  obj_weapon_clusterbomb.cpp  -  a clusterbomb
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

#include "obj_weapon_clusterbomb.h"
#include "obj_clusterbomb.h"

Obj_weapon_clusterbomb::Obj_weapon_clusterbomb(World *world_, int id) :Obj_weapon(world_, id) {
	reload_speed        = 0.2;
	shot_cost           = 0.5;
	reshoot_time        = 2.0;
	knock_back          = 1.0;
	shot_speed          = 1;
	bullet_speed        = 500;
	//bullet_speed_spread = 60;
	//bullet_angle_spread = 20;
	//subshots            = 10;
}

Obj_weapon_clusterbomb::~Obj_weapon_clusterbomb() {}

void Obj_weapon_clusterbomb::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	Serverworld *serverworld = (Serverworld *)world;
	aim_vector.scale(bullet_speed);

	knock_back_direction.add(aim_vector);

	//FIXME: Play the sound for the weapon
	//world->startSound( "sound/NAME.wav" , 0, 1, x, y );

	Obj_clusterbomb *bomb = (Obj_clusterbomb *)serverworld->newobject(ot_clusterbomb);
	if (bomb) {
		bomb->init(ownerid, x, y, xspeed+aim_vector.x, yspeed+aim_vector.y, 5, 300, 100);
	}
}
