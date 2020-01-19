/***************************************************************************
  graphic_bar.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-12-25
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef GRAPHIC_BAR_H
#define GRAPHIC_BAR_H

#include "graphic.h"

//! @author Flemming Frandsen
//! @brief Class for a graphical bar (used for health-bar or weapons status)
class Graphic_bar : public Graphic  {
public:
	//! Constructor
	Graphic_bar();
	//! Destructor
	~Graphic_bar();
	//! ???
	void setpos(float pos);
	//! renders the bar
	void render();
	//! the color of the bar
	int32 bar_color;
	//! the border color of the bar
	int32 border_color;
	//! set the geometry
	void setgeom(int xpos_, int ypos_, int xsize_, int ysize_);
	//! draw our bar to a mutable raw surface
	void draw(Mutable_raw_surface *raw_target);
//for pre placed (widget-like)
	//! the x position of the bar
	int xpos;
	//! the y position of the bar
	int ypos;
	//! the length of the bar
	int bar_length;
};

#endif
