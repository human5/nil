/***************************************************************************
  obj_gib.h  -  Header for gib
 ---------------------------------------------------------------------------
  begin                : 1999-11-25
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_GIB_H
#define OBJ_GIB_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief Class for a gib
class Obj_gib : public Obj_base  {
public:
	//! Constructor that defines rotation speed, air resistence, accelleration and radius
	Obj_gib(World *world_, int id);
	//! Destructor (empty)
	virtual ~Obj_gib();
	//! returns ot_gib as type
	virtual Object_type gettype() { return ot_gib; }
	//! a gib is not a server object, so nothing to do
	virtual void server_think();
	//! draws the gib if graphic loaded
	virtual void draw(Viewport *viewport);
	//! call obj_base's serializer and serialize rotation speed and puttomap
	virtual void serialize(Serializer *serializer);
	//! let our gib fly, bounce or hit something
	virtual void client_think();
	//! initialize with positon, speed and color
	void init(float x, float y, float xspeed_, float yspeed_, int32 color);

protected:
	//! ???
	typedef Graphic *Frame;
	//! ???
	Frame frames[8];
	//! the rotation speed
	//FIXME: do we really need a float for this?
	float rotate_speed;
	//! ???
	bool puttomap;
};

#endif
