/***************************************************************************
  obj_hook.h  -  Header for the hook
 ---------------------------------------------------------------------------
  begin                : 1999-12-23
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2004 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/


#ifndef OBJ_HOOK_H
#define OBJ_HOOK_H

#include "obj_base.h"

// max length of rope, for obj_hook.cpp
//#define ROPE_MAX_LENGTH 250

//! @author Flemming Frandsen
//! @brief Rope tool object 
class Obj_hook : public Obj_base  {
public:
	//! Constructor that sets air resistance and a negative rope length
	Obj_hook(World *world_, int id);
	//! Destructor (empty)
	virtual ~Obj_hook();
	//! returns ot_hook as type
	virtual Object_type gettype() {return ot_hook;}
	//! call obj_base's serializer and serialize the stuck status, the owner and the rope length
	virtual void serialize(Serializer *serializer);
	//! draws a rope from the owner to the hook
	virtual void draw(Viewport *viewport);
	//! if the player is alive, lengthen the rope until we hit something
	virtual void server_think();
	//! initialize rope
	void init(int owner,float xpos_, float ypos_, float xspeed_, float yspeed_);
	//! make rope longer
	void reel_out();
	//! shorten rope
	void reel_in();
	//! returns stuck flag
	bool isstuck() { return stuck; }
	//! constant: number of messages
	static const int ROPE_MAX_LENGTH = 250;
protected:
	//! is the hook stuck to a wall?
	bool stuck;
	//! whom does it belong to?
	int avatar_id;
	//! current length
  float rope_length;
};

#endif
