/***************************************************************************
  obj_bonus.h  -  Header for boni falling from the sky
 ---------------------------------------------------------------------------
  begin                : 2000-02-20
  by                   : Daniel Burrows
  email                : dburrows@debian.org

  last changed         : 2004-03-21
  by                   : Nils Thuerey
  email                : n_t@gmx.de

  copyright            : (C) 2000 by Daniel Burrows
  email                : dburrows@debian.org

 ***************************************************************************/

#ifndef OBJ_BONUS_H
#define OBJ_BONUS_H

#include "obj_base.h"

#define NOTIMEOUT 0

class Obj_player_avatar;

//! @author Daniel Burrows
//! @brief Boni falling from above
//! Only provides the basic behavior of bonuses; the children should handle additional stuff.
class Obj_bonus:public Obj_base {
public:
	//! Constructor
	Obj_bonus(World *world_, int id);
	//! return no type
	virtual Object_type gettype()=0;
	//! initialise the object at a specific location
	void init(int xpos_, int ypos_, float timeout_);

	// Child-overridden behaviors.
	//! Called when a player picks the bonus up. (returns whether pickup succeded)
	virtual bool pickup(Obj_player_avatar *who)=0;

	//! Called to determine the lifetime of the bonus.  Default: between 30 and 120 seconds.
	//! Default behavior is to vanish harmlessly.  Some bonuses might do something else.
	virtual void do_timeout();

	// movement.
	void server_think();
	void serialize(Serializer *serializer);
	bool hurtable() { return true; }

	//! Bonuses will "hit" anything within their radius.
	virtual float getradius()=0;
protected:
	//! At what time the bonus should vanish.
	float timeout;
};

#endif
