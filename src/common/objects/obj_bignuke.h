/***************************************************************************
  obj_bignuke.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-15
  by                   : Anders Rosendal
  email                : qute@klog.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Anders Rosendal
  email                : qute@klog.dk
  
 ***************************************************************************/

#ifndef OBJ_BIGNUKE_H
#define OBJ_BIGNUKE_H

#include "obj_base.h"

//! @author Anders Rosendal
//! @brief Class for a big nuke
class Obj_bignuke : public Obj_base  {
public:
	//! Constructor
	Obj_bignuke(World *world_, int id);
	//! Destructor
	virtual ~Obj_bignuke();
	//! ???
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, int count_, float power_);
	//! ???
	virtual Object_type gettype(){return ot_bignuke;}
	//! ???
	virtual void draw(Viewport *viewport);
	//! ???
	virtual void serialize(Serializer *serializer);
	//! ???
 	virtual void server_think();
protected:
	//! ???
	Graphic *sprite;
	//! ???
	float timer;
	//! ???
	float power;
	//! ???
	int count;
};

#endif
