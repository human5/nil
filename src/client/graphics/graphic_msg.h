/***************************************************************************
  graphic_msg.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2002-04-24
  by                   : Maciek Fijalkowski (fijaljr@linuxfreemail.com)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Don't bother with the whole file without SDL.

  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : some cleanup's

  copyright            : (C) 2002 by Maciek Fijalkowski (fijaljr@linuxfreemail.com)

 ***************************************************************************/

#ifndef _NIL_GRAPHIC_MSG_H_
#define _NIL_GRAPHIC_MSG_H_

#ifndef WITHOUT_SDL

#include "client/graphics/graphic.h"
#include "client/fonts/font_instance.h"

//! @author Maciek Fijalkowski
//! @brief Class for a graphical message
class Graphic_msg : public Graphic  {
public:
	//! Constructor setting position to 0,0 and text and font_instace to none
	Graphic_msg();
	//! Destructor that free's our text
	~Graphic_msg();
	//! render the message
	void render();
	//! text color values
	int32 text_color_red, text_color_green, text_color_blue;
	//! set the position, text and color for the message
	void setgeom(int xpos_, int ypos_, char *text , int32 red, int32 green, int32 blue);
	//! set a font
	void set_font(Font_instance *font);
	//! check if we already have a font
	bool is_font();
	//! draw our message
	void draw(Mutable_raw_surface *raw_target);
	//for pre placed (widget-like)
	//! The text
	char *text;
	//! The x position
	int xpos;
	//! The y position
	int ypos;
protected:
	//! the font we use
	Font_instance* font_instance;
};

#endif

#endif
