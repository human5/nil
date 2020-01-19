/***************************************************************************
  font.h  -  Headerfile for a single font
 ---------------------------------------------------------------------------
  begin                : 2000-01-30
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-02-04
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Some identing fixes.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Don't bother with the whole file without SDL.

  last changed         : 2004-11-06
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : added get_text_height and get_text_width

  last changed         : 2005-01-15
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : add return value for get_text_geom

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)
                         (C) 2004 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/


#ifndef _NIL_FONT_INSTANCE_H_
#define _NIL_FONT_INSTANCE_H_

#ifndef WITHOUT_SDL

#include "common/systemheaders.h"
#include "client/graphics/graphic.h"
#include "client/graphics/graphics.h"

/*! \author Flemming Frandsen
    \brief Class to access a single font
    \ingroup fonts */
class Font_instance {
public:
	//! Constructor that creates an empty font
	Font_instance();
	//! Destructor that free's the font
	~Font_instance();
	/*! load a new font
	    \param loader The refernce to the loader
	    \param font_name The name of the font that should be loaded
	    \param size_ The size we want the font to be loaded in
	    \return Returns true is the font was loaded */
	bool load(Loader *loader, char *font_name, int32 size_);
	/*! get the geometry of a text
	    \param text The text that will be analized
	    \param length The length of the text will be stored in here
	    \param height The height of the text will be stored in here
	    \return Return false if the text couldn't be rendered */
	bool get_text_geom(char *text, int *length , int *height);
	/*! get the height of a text
	    \param text The text that will be analized
	    \return Return the height of the text */
	int get_text_height(char *text);
	/*! get the width of a text
	    \param text The text that will be analized
	    \return Returns the width of the text */
	int get_text_width(char *text);
	//FIXME: Bug in raw_surface copying workaround: attribute if text is copied directly or not
	/*!
	    @param xpos The x coordinate of where to put the text
	    @param ypos The y coordinate of where to put the text
	    @param text The text that will be drawn
	    @param target The surface that the text will be drawn to
	    @param direct_copy Bug in raw_surface copying workaround: atribute if text is copied directly or not
	    @param red The red value of the color
	    @param green The green value of the color
	    @param blue The blue value of the color */
	bool put_text(int xpos, int ypos, const char *text, Mutable_raw_surface *target , bool direct_copy , uint8_t red , uint8_t green , uint8_t blue );
	//init(Loader *loader, char *name, int size, int32 color=PIXELA(255,255,255,15), bool antialize=true);
protected:
	//! The font we work with
	TTF_Font *font;
	//! The size of the font that we use
	int size;
	//! The unique name of the font
	char *name;
};

#endif

#endif
