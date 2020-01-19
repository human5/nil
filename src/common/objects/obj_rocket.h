/***************************************************************************
  obj_rocket.h  -  Header for the rocket
 ---------------------------------------------------------------------------
  begin                : 2000-02-22
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_ROCKET_H
#define OBJ_ROCKET_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief Class for a rocket
class Obj_rocket : public Obj_base  {
public:
	//! Constructor
	Obj_rocket(World *world_, int id);
	//! Destructor (empty)
	virtual ~Obj_rocket();
	//! initialize a new rocket with its values
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float trail_, float power_, int size_, Vector& aimVector_);
	//! return ot_rocket as type
	virtual Object_type gettype() { return ot_rocket; }
	//! ???
	virtual void draw(Viewport *viewport);
	//! ???
	virtual void serialize(Serializer *serializer);
	//! ???
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
	float    fuel;
	//! ???
	Vector   aimVector;
};

#endif
