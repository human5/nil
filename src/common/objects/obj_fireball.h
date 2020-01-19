/***************************************************************************
  obj_fireball.h  -  Header for a fireball
 ---------------------------------------------------------------------------
  begin                : 1999-12-19
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_FIREBALL_H
#define OBJ_FIREBALL_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief Class for drawing and handling a fireball
class Obj_fireball : public Obj_base  {
public:
	//! Constructor that loads the graphic
	Obj_fireball(World *world_, int id);
	//! Destructor (empty)
	virtual ~Obj_fireball();
	//! initialize the fireball with some given values
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int explosion_size_, int penetration_, int max_age);
	//! returns ot_fireball as type
	virtual Object_type gettype() { return ot_fireball; }
	//! draws the fireball to a viewport using xpos and ypos
	virtual void draw(Viewport *viewport);
	//! simply calls the serializer of obj_base
	virtual void serialize(Serializer *serializer);
	//! checks if the fireball hit us (after checking its age against max_age)
 	virtual void server_think();
protected:
	//! ???
	static Graphic *frame;
	//! damage that comes from an explosion
	float explosion_damage;
	//! size of the explosion of the fireball
	int explosion_size;
	// FIXME: where is this used ???
	//int penetration;
	//! the maximum age of a fireball
	int max_age;
};

#endif
