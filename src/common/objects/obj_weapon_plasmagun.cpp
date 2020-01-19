/***************************************************************************
  obj_weapon_plasmagun.cpp  -  the plasmagun
  ---------------------------------------------------------------------------
  begin                : 2000-02-03
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_weapon_plasmagun.h"
#include "common/objects/obj_plasmaball.h"
#include "common/sound/soundsystem.h"

Obj_weapon_plasmagun::Obj_weapon_plasmagun(World *world_, int id) :Obj_weapon(world_,id) {
	reload_speed        = 0.1;
	shot_cost           = 0.1;
	reshoot_time        = 0.1;
	knock_back          = 5.0;
	shot_speed          = 1;
	bullet_speed        = 60;
	bullet_speed_spread = 60;
	bullet_angle_spread = 20;
	subshots            = 10;
}

Obj_weapon_plasmagun::~Obj_weapon_plasmagun() {}

void Obj_weapon_plasmagun::dofire(int x, int y, Vector aim_vector, float aim_angle)
{
	Serverworld *serverworld = (Serverworld *)world;
	// float frame_delta = world->get_last_frame_delta();

	//FIXME: Play the sound for the weapon
	//world->startSound( "sound/NAME.wav" , 0, 1, x, y );
	
	for (int c=0; c<subshots; c++) {
		int balltype = ROUND(0.5+3*readyness);
		if (c < 3) balltype-=rnd(0,1);
		if (c > 7) balltype+=rnd(0,1);
		float bullet_angle = aim_angle+rnd(-bullet_angle_spread,bullet_angle_spread)/1000.0;
		Vector bullet_vector = Math::rad2vector(bullet_angle);
		bullet_vector.scale(rnd((1000-bullet_speed_spread/3)*bullet_speed,(1000+bullet_speed_spread)*bullet_speed)/1000.0);

		knock_back_direction.add(bullet_vector);
		
		//FIXME: better? use time displacement as for the minigun?
		Vector displacement=bullet_vector;
		displacement.normalize();
		displacement.scale(/*5*rnd(0,1)+*/c*0.1*bullet_speed/subshots);
		Obj_plasmaball *bullet = (Obj_plasmaball *)serverworld->newobject(ot_plasmaball);
		if (bullet) {
			bullet->init(ownerid, displacement.x+x, displacement.y+y, xspeed+bullet_vector.x, yspeed+bullet_vector.y, 50, balltype);
		}
	}
}
