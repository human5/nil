/***************************************************************************
  obj_railslug.h  -  Header for the railslug
 ---------------------------------------------------------------------------
  begin                : 2000-01-05
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_RAILSLUG_H
#define OBJ_RAILSLUG_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief Class for a railslug
class Obj_railslug : public Obj_base  {
public:
	//! Constructor
	Obj_railslug(World *world_, int id);
	//! Destructor
	virtual ~Obj_railslug();
	//! ????
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_);
	//! returns ot_railslug as type
	virtual Object_type gettype() { return ot_railslug; }
	//! ???
	virtual void draw(Viewport *viewport);
	//! ???
 	virtual void server_think();
	//! ???
	virtual void client_think();
	//! ???
	virtual void serialize(Serializer *serializer);
protected:
	//! ???
	bool havehit;
	//! ???
	float xhit;
	//! ???
	float yhit;
	//! ???
	float xstart;
	//! ???
	float ystart;
};

#endif
