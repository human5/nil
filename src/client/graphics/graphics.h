/***************************************************************************
  graphics.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-17
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "graphic.h"

class Graphic;
class Loaded_graphics;
class Loader;

//! @author Flemming Frandsen
//! @brief Class for loading and storing sprites
class Graphics {
public: 
	//! Constructor setting loader to NULL and creating a new graphic
	Graphics();
	//! delete our graphic
	~Graphics();
	/*! Store the reference to the loader
	    @param loader_ The reference to the loader */
	void init(Loader *loader_);
	/*! returns the pointer to the named graphic the name is just the
	    relative filename also used when talking to the loader
	    --something is used for internal names?
	    this will load the graphic from file if it hasn't been loaded before
	    or simply return the pointer to the previously loaded one.
	    the users (read Obj_sometings) should store the Graphic * to keep the
	    use of this function down to a minimum.
	    @param name The name of the graphic
	    @return ??? */
	Graphic *getgraphic(const char *name);
	/*! This creates a new graphics object with a certain name, but it doesn't load anything
	    @param name The name of the graphic
	    @return ??? */
	Graphic *new_graphic(const char *name);
	/*! delete a graphic
	    @param name The name of the graphic */
	void delete_graphic(const char *name);
protected:
	//! Storage for the reference to the loader
	Loader *loader;
	//! ???
	Loaded_graphics *graphics;
};

#endif
