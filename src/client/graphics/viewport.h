/***************************************************************************
  viewport.h  -  Header for place to stick all the info needed for rendering
                 a view of the arena
  --------------------------------------------------------------------------
  begin                : 1999-10-11
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

/***************************************************************************
  Note: some viewports are "owned" by an object.  This is used for a few
  things such as drawing crosshairs.  (it used to be that this was stored
  in the world, but since multiple players can be on one computer, this no
  longer works)
 ***************************************************************************/
 
#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "common/systemheaders.h"
#include "client/graphics/graphics.h"
#include "client/graphics/raw_surface.h"

class Graphics;

//! @author Flemming Frandsen
//! @brief This class handels what a player can see
class Viewport : public Mutable_raw_surface {
public:
	//! Constructor
	Viewport(int width, int height, int owner, int map_xsize_, int map_ysize_);
	//! Destructor
	~Viewport();
	//! drop shadows
	void drop_shadow();
	/*! set position
	    @param x The x coordinates of the desired position
	    @param y The y coordinates of the desired position */
	void setpos(int x, int y);
	//! x coordinats of the topleft corner on the bit map
	int xpos;
	//! y coordinats of the topleft corner on the bit map
	int ypos;
	//! y size of the bit map
	int map_xsize;
	//! y size of the bit map
	int map_ysize;
	/*! ???
	    @param x ???
	    @param y ???
	    @param dt ??? */
	void addsample(int x, int y, float dt);
	/*! Get the owner ID of the viewport
	    @return Returns the owner id of this viewport */
	int getowner() { return ownerid; }

protected:
	//! owner of this viewport.
	int ownerid;
	//! ???
	float xesum;
	//! ???
	float yesum;
	//! ???
	float xspeed;
	//! ???
	float yspeed;
	//! ???
	float xcenter;
	//! ???
	float ycenter;
};

#endif
