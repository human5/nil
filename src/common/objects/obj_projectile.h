/***************************************************************************
  obj_projectile.h  -  Header for projectile
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


#ifndef OBJ_PROJECTILE_H
#define OBJ_PROJECTILE_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief Class for projectiles
class Obj_projectile : public Obj_base  {
public:
	//! Constructor (empty)
	Obj_projectile(World *world_, int id);
	//! Destructor (empty)
	virtual ~Obj_projectile();
	//! add a projectile with some arguments
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int explosion_size_, int penetration_, float init_age);
	//! returns ot_projectile as type
	virtual Object_type gettype() { return ot_projectile; }
	//! draw a single white pixel representing the projectile
	virtual void draw(Viewport *viewport);
	//! call obj_base's serializer and serilalize the owner of the projectile
	virtual void serialize(Serializer *serializer);
	//! check if we hit something. if so: explode
	virtual void server_think();
protected:
	//! damage of the explosion
	float explosion_damage;
	//! size of the explostion
	int explosion_size;
	//! FIXME: penetration no longer needed?
	int penetration;
};

#endif
