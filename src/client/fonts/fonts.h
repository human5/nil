/***************************************************************************
  fonts.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-07
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de
  
  last changed         : 2004-11-06
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : added new constructor

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef _NIL_FONTS_H_
#define _NIL_FONTS_H_

#include "common/systemheaders.h"
#include "client/graphics/raw_surface.h"

class Font_instances;
class Font_instance;
class Loader;

/*! \author Flemming Frandsen
    \brief Class for handling fonts
    \ingroup fonts */
class Fonts {
public:
	//! Constructor creating a new list for loaded fonts
	Fonts();
	/*! Constructor creating a new list for loaded fonts and sets a pointer to the loader
	    \param loader_ The reference to a loader */
	Fonts(Loader *loader_);
	//! Destructor recursively freeing loaded fonts
	~Fonts();
	/*! simply sets the pointer Loader
	    \param loader_ The reference to a loader */
	void init(Loader *loader_);
	/*! Get a handler to a font (the font with that size is loaded if it hasn't been used before)
	    \param name The name of the font
	    \param size The size of the font
	    \return Return a handle to the font instance */
	Font_instance *getfont(const char *name, int size);
	/*! returns a handler to a font (the font with that size is loaded if it hasn't been used before)
	    \param name The name of the font
	    \param size The size of the font
	    \param antialize A flag to see if a font should be antialized or not
	    \return Return a handle to the font instance */
	Font_instance *getfont(const char *name, int size, bool antialize);
protected:
	//! A list of loaded fonts (also called font instances)
	Font_instances *font_instances;
	//! A refernce to the loader
	Loader *loader;
};

#endif
