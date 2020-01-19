/***************************************************************************
  obj_larpa.h  -  Header for the larpa (what ever this is ....)
 ---------------------------------------------------------------------------
  begin                : 2000-02-22
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_LARPA_H
#define OBJ_LARPA_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief Class for the larpa (what ever this is ....)
class Obj_larpa : public Obj_base  {
public: 
	//! Constructor
	Obj_larpa(World *world_, int id);
	//! Destructor
	virtual ~Obj_larpa();
	//! ???
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float trail_, float power_, int size_);
	//! ???
	virtual Object_type gettype(){return ot_larpa;}
	//! ???
	virtual void draw(Viewport *viewport);
	//! ???
	virtual void serialize(Serializer *serializer);
	//! ???
	virtual void server_think();
protected:
	//! ???
	Graphic *sprite[8];
	//! ???
	float trail;
	//! ???
	float power;
	//! ???
	float last;
	//! ???
	int size;
};

#endif
