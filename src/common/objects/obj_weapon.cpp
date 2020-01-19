/***************************************************************************
  obj_weapon.cpp  -  the weapon main class
 ---------------------------------------------------------------------------
  begin                : 1999-12-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_weapon.h"
#include "common/objects/obj_player_avatar.h"
#include "common/objects/obj_player_data.h"
#include "common/console/logmsg.h"

Obj_weapon::Obj_weapon(World *world_, int id) : Obj_base(world_, id) {
	readyness           = 1;
	reloading           = false;
	last_shot_time      = 0;
	old_age             = age();

	//Set some sort of defaults, ALL weapons must set their own values
	reload_speed        = 0.1;
	shot_cost           = 0.5;
	shot_speed          = 1;
	reshoot_time        = 0.5;
	knock_back          = 0;
	bullet_speed        = 60;
	bullet_speed_spread = 60;
	bullet_angle_spread = 20;
	subshots            = 10;
}

Obj_weapon::~Obj_weapon() {}

void Obj_weapon::init(int ownerid_) {
	ownerid = ownerid_;
	readyness = 0;
	reloading = false;
	curtime = world->gettime();
	old_age = age();
}

void Obj_weapon::serialize(Serializer *serializer) {
	serializer->rw(ownerid);
	serializer->rw(readyness);
	serializer->rw(reloading);
}

bool Obj_weapon::fire(float x, float y, float aim_angle) {
	if (world->isclient())
		return false;
	if (!canFire())
		return false;
	if (reshoot_time > age()-last_shot_time)
		return false;

	Obj_player_avatar *avatar = get_owner_avatar();
	if (avatar)
		avatar->getspeed(xspeed,yspeed);	
	
	Vector v = Math::rad2vector(aim_angle);
	knock_back_direction.zero();
	v.scale(shot_speed);
	// fire the waepon
 	dofire(ROUND(x), ROUND(y-4), v, aim_angle); // correction not necessary for barrel
	readyness -= shot_cost;
	last_shot_time = age();

	//tell the client about the new decrease in readyness
	setdirty();

	//apply the knock back
	knock_back_direction.normalize();
	knock_back_direction.scale(-knock_back);
	if (avatar) {
		avatar->push(knock_back_direction);
	} else {
		logmsg(lt_debug,"Unable to locate the avatar for knockback");		
	}
	
	return true;
}

void Obj_weapon::clientFire(float x, float y, float aim_angle) {}

void Obj_weapon::server_think() {
	client_think();

	//see if we should reload:
	if (!reloading && readyness < shot_cost) {
 		reloading = true;
		setdirty();
		logmsg(lt_debug,"Reloading %s",getname());
		old_age = age();
	}

	// Don't reload if this weapon isn't being used.  (should we
	// be allowed to reload while dead?)
	if(world->get_always_reload() || get_owner_data()->get_weapon(get_owner_data()->controls.get_weapon())==this) {
		//Reload the gun:
		if (reloading) {
			//calc time since last update:
			float delta_time = age()-old_age;

			readyness += reload_speed * delta_time;

			//see if we should stop reloading
			if (readyness >= 1) {
				readyness = 1;
				reloading = false;
				logmsg(lt_debug,"Done reloading %s",getname());
			}
			setdirty();
		}
	}
	old_age = age();
}

bool Obj_weapon::verify() {
	return !((readyness < -0.1) || (readyness > 1.1));
}
