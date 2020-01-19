/***************************************************************************
  obj_test.h  -  Header for the test object
 ---------------------------------------------------------------------------
  begin                : 1999-10-20
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_TEST_H
#define OBJ_TEST_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief Class for a test object
class Obj_test : public Obj_base {
public:
	//! Constructor
	Obj_test(World *world_, int id);
	//! Destructor
	virtual ~Obj_test();
	//! returns ot_testobject as type
	virtual Object_type gettype() { return ot_testobject; }
	//! ???
	virtual void server_think();
	//! ???
	virtual void draw(Viewport *viewport);
	//! ???
	void init(int owner, int level_, float xpos_,float ypos_,float xspeed_,float yspeed_,float yaccel_, float air_resistance_);
protected:
	//! ???
	Graphic *frames[8];
	//! ???
	int level;
	//! ???
	float rotate_speed;
	//! ???
	bool unspawned;
};

#endif
