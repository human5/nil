/***************************************************************************
  obj_plasmaball.h  -  Header for the plasmaball
 ---------------------------------------------------------------------------
  begin                : 2000-02-03
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/


#ifndef OBJ_PLASMABALL_H
#define OBJ_PLASMABALL_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief Class for a plasmaball
class Obj_plasmaball : public Obj_base  {
public:
	/*! Constructor
	    @param world_ The world that we attach the plasmaball to
	    @param id The ID of the plasmaball */
	Obj_plasmaball(World *world_, int id);
	//! Destructor
	virtual ~Obj_plasmaball();
	/*! initialize
	    @param owner The owner of this plasmaball
	    @param xpos_ The x coordinate of the spawning position
	    @param ypos_ The y coordinate of the spawning position
	    @param xspeed_ The speed in x direction
	    @param yspeed_ The speed in y direction
	    @param explosion_damage_ The damage this plasmaball does
	    @param type_ The type of this object (which is plasmaball) */
	void init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int type_);
	/*! returns ot_plasmaball as type
	    @return Returns plasma ball as object type */
	virtual Object_type gettype() { return ot_plasmaball; }
	/*! draw the plasmaball
	    @param viewport The viewport we will draw to */
	virtual void draw(Viewport *viewport);
	//! call obj_base's serializer and serialize damage, type and owner
	virtual void serialize(Serializer *serializer);
	//! checks if we hit something or the edges of the world
	virtual void server_think();
protected:
	//! type of the plasmaball (1<=type<=4)
	int type;
	//! ???
	typedef Graphic *Frames[4];
	//! ???
	static Frames frames;
	//! how much damage we have
	//FIXME: do we really need a float for that???
	float explosion_damage;
};

#endif
