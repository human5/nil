/***************************************************************************
  obj_explosion.h  -  Header for the explosion
 ---------------------------------------------------------------------------
  begin                : 1999-10-08
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_EXPLOSION_H
#define OBJ_EXPLOSION_H

#include "common/objects/obj_base.h"
#include "client/graphics/graphics.h"

//! @author Flemming Frandsen
//! @brief An explosion
class Obj_explosion : public Obj_base {
public:
	//! Constructor
	Obj_explosion(World *world_,int id);
	//! initialize
	void init(int owner, int xcenter, int ycenter, int size, int hitpoints_);
	//! return ot_explosion as type
	virtual Object_type gettype() { return ot_explosion; }
	//! ???
	virtual void server_think();
	//! ???
	virtual void client_think();
	/*! ???
	    @param viewport The viewport we are drawing to */
	virtual void draw(Viewport *viewport);
	/*! ???
	    @param serializer The serializer that is doing the job */
	virtual void serialize(Serializer *serializer);
protected:
	//! ???
	typedef Graphic *Frames[16];
	//! ???
	static Frames frames;
	//! ???
	int state;
	//! ???
	float frame;
	//! ???
	float maxsize;
	//! ???
	int hitpoints;
	//! ???
	float peak_time;
};

#endif
