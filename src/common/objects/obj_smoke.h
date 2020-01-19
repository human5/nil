/***************************************************************************
  obj_smoke.h  -  Header for the smoke object
 ---------------------------------------------------------------------------
  begin                : 2000-04-05
  by                   : Maciek Fijalkowski
  email                : fijaljr@linuxfreemail.com

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Maciek Fijalkowski
  email                : fijaljr@linuxfreemail.com

 ***************************************************************************/

#ifndef OBJ_SMOKE_H
#define OBJ_SMOKE_H

#include "obj_base.h"

#define SMOKE_SIZE 7

//! @author Maciek Fijalkowski
//! @brief A smoke object
class Obj_smoke : public Obj_base {
public: 
	//! augmented contructor by color argument (uwe)
	Obj_smoke(World *world_, int id, int32 color = 0xAAAAAA );
	//! ???
	virtual ~Obj_smoke();
	//! ???
	void init(int owner, float xpos_, float ypos_, int size_, int32 color = 0xAAAAAA);
	//! ???
	virtual Object_type gettype(){return ot_smoke;}
	//! ???
	virtual void draw(Viewport *viewport);
	//! ???
	virtual void serialize(Serializer *serializer);
	//! ???
	virtual void client_think();
	//! ???
	virtual void server_think();
	//! ???
	int32 getColor() const { return m_color; }
	//! ???
	void  setColor( int32 color ) { m_color = color; }
protected:
	//! ???
	int32 m_color;
	//! ???
	int   size;
	//! ???
	bool  smoke_map_ready;
	//! ???
	int   smoke_map [ 2*SMOKE_SIZE + 1 ] [ 2*SMOKE_SIZE + 1 ];
};

#endif
