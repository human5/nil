/***************************************************************************
  icon_graph.h  -  Header for the network traffic graph
 ---------------------------------------------------------------------------
  begin                : 2000-01-27
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef ICON_GRAPH_H
#define ICON_GRAPH_H

#include "icon_base.h"

//! @author Flemming Frandsen
//! @brief The network traffic display
class Icon_graph : public Icon_base  {
public: 
	//! empty Constructor
	Icon_graph();
	//! empty Destructor
	virtual ~Icon_graph();
	/*! draws the graph on a mutable raw surface
	    @param target The surface that we will draw to */
	virtual void draw(Mutable_raw_surface *target);
	/*! initialises the graph at a specific location
	    @param xpos_ ???
	    @param ypos_ ???
	    @param xsize_ ???
	    @param ysize_ ???
	    @param color_ ???
	    @param graphcolor_ ??? */
	void init(int xpos_, int ypos_, int xsize_, int ysize_, int32 color_, int32 graphcolor_);
	/*! moves the current graph on to the left and draws a new column
	    @param percent ??? */
	void addsample(float percent);
protected:
	//! the color of the border
	int32 color;
	//! the color of the graph itself
	int32 graphcolor;
	//! The x coordinate of the graph
	int xpos;
	//! The y coordinate of the graph
	int ypos;
};

#endif
