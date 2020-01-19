/***************************************************************************
  obj_weapon.h  -  Header for the weapon main class
 ---------------------------------------------------------------------------
  begin                : 1999-12-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/


#ifndef OBJ_WEAPON_H
#define OBJ_WEAPON_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief Generic Weapon
class Obj_weapon : public Obj_base  {
public:
	//! Constructor for all weapons with some default values
	Obj_weapon(World *world_, int id);
	//! Destructor
	virtual ~Obj_weapon();
	//! weapons have to return their own type
	virtual Object_type gettype() = 0;
	//! initialises a weapon
	void init(int ownerid_);
	//! Users of weapons call this on the server to fire, fire calls dofire() to get the actual fireing done.
	bool fire(float x, float y, float aim_angle);
	//! this is for handling client side effects of weapons, like jetpack smoke etc. 
	virtual void clientFire(float x, float y, float aim_angle);
	//! is the weapon able to fire this timeframe?
	bool canFire() {return (!reloading) && (readyness >= shot_cost);}
	//! returns: 0..1, where 0=depleted and 1=fully ready
	float getReadyness() {return readyness;}
	//! set readyness (0..1), e.g. for respawning
	void setReadyness(float set) { readyness = set; if(set<1.0) reloading = true; else reloading = false; }
	//! ???
	virtual void server_think();
	//! ???
	virtual void client_think() {};
	//! ???
	virtual void serialize(Serializer *serializer);
	//! Object is always invisible
	virtual void draw(Viewport *viewport) {};
	//! ???
	bool verify();
protected:
	//! This is what you need to implement to make a new weapon, it is called by fire()
	virtual void dofire(int x, int y, Vector aim_vector, float aim_angle)=0;
	//! The vector that happens to the player when he shoots
	Vector knock_back_direction;
	//! ???
	float old_age;
	//! weapon ready to fire again (0..1)?
	float readyness;
	//! weapon reloading?
	bool reloading;
	//! semiconstants, to be set in the constructor of new weapons:
	float reload_speed;
	//! The amount of readyness spent fireing 
	float shot_cost;
	//! The the weapon needs to lauch the bullets
	float shot_speed;
	//! Time between shots
	float reshoot_time;
	//! The time when the weapon was fired the last time
	float last_shot_time;
	//! Speed addition from one shot 
	float knock_back;
	//! speed of the bullets
	int bullet_speed;
	//! variation in bullet speed
	int bullet_speed_spread;
	//! variation in bullet angle
	int bullet_angle_spread;
	//! count of bullets fired by one shot
	int subshots;
};

#endif
