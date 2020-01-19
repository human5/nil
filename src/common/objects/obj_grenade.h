/***************************************************************************
  obj_grenade.h  -  Header for grenade
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


#ifndef OBJ_grenade_H
#define OBJ_grenade_H

#include "obj_base.h"

//! @author Anders Rosendal
//! @brief Class for grenades
class Obj_grenade : public Obj_base  {
public:
	//! Constructor that loads the grenade script
	Obj_grenade(World *world_, int id);
	//! Destructor
	virtual ~Obj_grenade();
	//! ???
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int explosion_size_, float timer_);
	//! ???
	virtual Object_type gettype(){return ot_grenade;}
	/*! ???
	    @param viewport The viewport we are drawing to */
	virtual void draw(Viewport *viewport);
	/*! ???
	    @param serializer The serializer that is doing the job */
	virtual void serialize(Serializer *serializer);
	//! ???
 	virtual void server_think();
protected:
	//! ???
	typedef Graphic *Frames[40];
	//! ???
	static Frames frames;
	//! ???
	Animator *animator;
	//! ???
	float float_time_hack;
	//! ???
	float timer;
	//! ???
	float explosion_damage;
	//! ???
	int explosion_size;
};

#endif

