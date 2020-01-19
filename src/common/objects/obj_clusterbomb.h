/***************************************************************************
  obj_clusterbomb.cpp  -  Header for the clusterbomb
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

#ifndef OBJ_clusterbomb_H
#define OBJ_clusterbomb_H

#include "obj_base.h"

//! @author Anders Rosendal
//! @brief Class for a bomd splitting into more little ones
class Obj_clusterbomb : public Obj_base  {
public:
	//! Constructor
	Obj_clusterbomb(World *world_, int id);
	//! Destructor (empty)
	virtual ~Obj_clusterbomb();
	//! initialize with the given arguments
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, int count_, float damage_, float speed_);
	//! returns ot_clusterbomb as type
	virtual Object_type gettype() { return ot_clusterbomb; }
	//! draws the clusterbomb
	virtual void draw(Viewport *viewport);
	//! calls obj_base's serializer and serializes frame, timer, count, damage and speed
	virtual void serialize(Serializer *serializer);
	//! bounce or explode
 	virtual void server_think();
protected:
	//! ???
	Graphic *sprite[4];
	//! the starting frame (frame=0)
	int frame;
	//! ???
	float last_frame_shift;
	//! timer until explosion
	// (FIXME: currently fixed to 3 seconds, better add to void init(...))
	float timer;
	//! the damage this clusterbomb does
	float damage;
	//! the speed we throw it
	float speed;
	//! the count of pieces the bomb explodes into
	int count;
};

#endif
