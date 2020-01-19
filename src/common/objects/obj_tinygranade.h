/***************************************************************************
  obj_tinygranade.h  -  Header for the tiny grenade
 ---------------------------------------------------------------------------
  begin                : 2000-02-13
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Anders Rosendal
  email                : qute@klog.dk

 ***************************************************************************/

#ifndef OBJ_TINYGRANADE_H
#define OBJ_TINYGRANADE_H

#include "obj_base.h"

//! @author Anders Rosendal
//! @brief Class for a tiny grenade
class Obj_tinygranade : public Obj_base  {
public:
	//! Constructor
	Obj_tinygranade(World *world_, int id);
	//! Destructor
	virtual ~Obj_tinygranade();
	//! ???
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int explosion_size_, float init_age, float timer_, bool explode_on_impact);
	//! ???
	virtual Object_type gettype(){return ot_tinygranade;};
	//! ???
	virtual void client_think();
	//! ???
	virtual void draw(Viewport *viewport);
	//! ???
	virtual void serialize(Serializer *serializer);
	//! ???
	virtual void server_think();
protected:
	//! ???
	float timer;
	//! ???
	float explosion_damage;
	//! ???
	int explosion_size;
	//! ???
	bool explode_on_impact;
};

#endif
