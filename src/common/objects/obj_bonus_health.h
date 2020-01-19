/***************************************************************************
  obj_bonus_health.h  -  Header for health bonus
 ---------------------------------------------------------------------------
  begin                : 2000-02-20
  by                   : Daniel Burrows
  email                : dburrows@debian.org

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Daniel Burrows
  email                : dburrows@debian.org

 ***************************************************************************/

#ifndef OBJ_BONUS_HEALTH_H
#define OBJ_BONUS_HEALTH_H

#include "obj_bonus.h"

//! @author Daniel Burrows
//! @brief A health bonus
class Obj_bonus_health:public Obj_bonus {
public:
	//! Contructor
	Obj_bonus_health(World *world_, int id);
	//! returns ot_bonus_health as type
	Object_type gettype() {return ot_bonus_health;}
	//! initialises the object at a specific location
	void init(int xpos_, int ypos_);
	//! gets called if a player picks up the bonus
	bool pickup(Obj_player_avatar *who);
	//! draw the health bonus
	void draw(Viewport *viewport);
	//! radius of the health bonus
	float getradius() { return 6; }
protected:
	//FIXME: how much amount??
	//! health amount
	float quantity;
	//! ???
	typedef Graphic *Frames[1];
	//! ???
	static Frames frames;
};

#endif
