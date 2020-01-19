/***************************************************************************
  obj_weapon_jetpack.cpp  -  the jetpack weapon
  ---------------------------------------------------------------------------
  begin                : 2002-??-??
  by                   : Uwe Fabricius, Thomas Pohl
  email                : ???
    
  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2002 by Uwe Fabricius, Thomas Pohl
  email                : ???

 ***************************************************************************/

#include "common/objects/obj_smoke.h"
#include "common/objects/obj_weapon_jetpack.h"
#include "client/clientworld.h"
#include "common/sound/soundsystem.h"


// speed /strength of jetpack thrust
const int   JET_SPEED  = 200;

// angle of jetpack force
const float JET_ANGLE  = (10*(2*3.1415/360.0));

Obj_weapon_jetpack::Obj_weapon_jetpack(World *world_, int id) : Obj_weapon(world_, id) {
	reload_speed        = 0.5;
	shot_cost           = 0.01;
	reshoot_time        = 0.0;
	knock_back          = 5.0;
	//shot_speed          = 1;
	//bullet_speed        = 500;
	//bullet_speed_spread = 60;
	//bullet_angle_spread = 20;
	//subshots            = 10;
}

Obj_weapon_jetpack::~Obj_weapon_jetpack() {}

void Obj_weapon_jetpack::dofire(int x, int y, Vector aim_vector, float aim_angle) {
	// make player move
	float jet_angle = aim_angle < (3.1415/2) ? (3.1415/2) + JET_ANGLE :
		                                	   (3.1415/2) - JET_ANGLE ;
	Vector jet_vector = Math::rad2vector( jet_angle );
	jet_vector.scale( JET_SPEED );
	knock_back_direction.add( jet_vector );
	
	//FIXME: Play the sound for the jetpack
	//FIXME: disabled due to too many playbacks, Tom
	//world->startSound( "sound/jetpack.wav" , 0, 1, x, y );
}

void Obj_weapon_jetpack::clientFire(float x, float y, float aim_angle) {
	Vector aimVector = Math::rad2vector(aim_angle);
	aimVector.scale(shot_speed);

	// make it smoke
	Clientworld *clientworld = (Clientworld *)world;
	Obj_smoke *smoke = (Obj_smoke*)clientworld->newanonobject( ot_smoke );
	aimVector.scale( 5.0 );
	if( smoke ) {
		smoke->init(ownerid, x - aimVector.x + (aim_angle < (3.1415/2) ? +3 : -3 ), y - aimVector.y + 2, 10);
	}
}
