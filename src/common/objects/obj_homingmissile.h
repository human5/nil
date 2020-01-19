/***************************************************************************
  obj_homingmissile.h  -  Header for homing missile
 ---------------------------------------------------------------------------
  begin                : 2003-07-17
  by                   : Uwe Fabricius
  email                : ???@???.???

  last changed         : 2004-05-16
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2003 by Uwe Fabricius
  email                : qute@klog.dk

 ***************************************************************************/

#ifndef OBJ_HOMING_MISSILE_H
#define OBJ_HOMING_MISSILE_H

#include "obj_base.h"

//! @author Uwe Fabricius
//! @brief The homing missile
class Obj_homingmissile : public Obj_base {
public: 
	//! ???
	Obj_homingmissile(World *world_, int id);
	//! ???
	virtual ~Obj_homingmissile();
	//! ???
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float trail_, float power_, int size_, Vector& aimVector_);
	//! return homing missle as type
	virtual Object_type gettype(){ return ot_homingmissile; }
	//! ???
	virtual void draw(Viewport *viewport);
	//! ???
	virtual void serialize(Serializer *serializer);
	//! create smoke on the clients and move the rocket
	virtual void client_think();
	//! ???
	virtual void server_think();
protected:
	//! ???
	Graphic *sprite[8];
	//! ???
	float    trail;
	//! ???
	float    power;
	//! ???
	float    last;
	//! ???
	int      size;
	//! ???
	int      signalcount;
	//! ???
	float    fuel;
	//! ???
	Vector aimVector;
	//! ???
	int32    smoke_color;
};

#endif // OBJ_HOMING_MISSILE_H
