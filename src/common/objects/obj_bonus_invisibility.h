/***************************************************************************
  obj_bonus_invisibility.h  -  header for the invisibility extra
 ---------------------------------------------------------------------------
  begin                : 2003-09-01
  by                   : Uwe Fabricius
  email                : uwe.f@bricius.de

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2003 by Uwe Fabricius
  email                : uwe.f@bricius.de

 ***************************************************************************/

#ifndef OBJ_BONUS_INVISIBILITY_H
#define OBJ_BONUS_INVISIBILITY_H

#include "obj_bonus.h"

//! @author Uwe Fabricius
//! @brief An invisibility extra
class Obj_bonus_invisibility : public Obj_bonus {
public:
	//! Constructor
	Obj_bonus_invisibility( World *world_, int id );
	//! returns ot_bonus_invisibility as type
	Object_type gettype() { return ot_bonus_invisibility; }
	//! initialises an invisiblity bonus at a specific location
	void init(int xpos_, int ypos_);
	//! called when a player picks up
	bool pickup(Obj_player_avatar *who);
	//! draws the bonus
	void draw(Viewport *viewport);
	//! radius of the bonus
	float getradius() { return 6; }

protected:
	//! how long should the player be invisible (seconds)
	float m_time;
	//! ???
	typedef Graphic *Frames[1];
	//! ???
	static Frames    frames;
};

#endif
