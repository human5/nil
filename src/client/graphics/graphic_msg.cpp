/***************************************************************************
  graphic_msg.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2002-04-24
  by                   : Maciek Fijalkowski (fijaljr@linuxfreemail.com)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : make use of font_instance::get_text_geom()'s
                         return value and some cleanup

  copyright            : (C) 2002 by Maciek Fijalkowski  (fijaljr@linuxfreemail.com)

 ***************************************************************************/

#include "client/graphics/graphic_msg.h"
#include "common/misc.h"
#include "common/nil_math.h"
#include <string.h>

Graphic_msg::Graphic_msg() : Graphic() {
	xpos = 0;
	ypos = 0;
	text = NULL;
	font_instance = NULL;
}

Graphic_msg::~Graphic_msg() {
	if (text)
		free(text);
}

void Graphic_msg::render( ) {
	int xsize = get_xsize();
	int ysize = get_ysize();

	//Start by clearing the area:
	int32 *pixels = get_pixels_write();

	memset(pixels,0,xsize*ysize*sizeof(int32));
	font_instance->put_text(0, 0, text, get_mutable_surface(), false, text_color_red, text_color_green, text_color_blue);
}

void Graphic_msg::setgeom(int xpos_, int ypos_, char *str , int32 red , int32 green , int32 blue ) {
	text_color_red = red;
	text_color_green = green;
	text_color_blue = blue;
	if ((text) && (str) && (strcmp(str, text) == 0 )) {
		xpos = xpos_;
		ypos = ypos_;
		return;
	}
	if ((!font_instance) || (!str))
		return;
	if (text)
		free (text);
	text = (char*)malloc(strlen(str) + 1);
	strcpy(text, str);
	int xsize_, ysize_;
	if (!font_instance->get_text_geom(text, &xsize_, &ysize_)) {
		//we couldn't render the text, let's try some bogus but safe values
		xsize_ = 1;
		ysize_ = 1;
	}
	if (get_xsize() != xsize_ || get_ysize() != ysize_)
		create(xsize_, ysize_);
	xpos = xpos_;
	ypos = ypos_;
}

void Graphic_msg::set_font(Font_instance *font) {
	font_instance = font;
}

bool Graphic_msg::is_font() {
	return (font_instance != 0);
}

void Graphic_msg::draw(Mutable_raw_surface *raw_target) {
	int target_x = xpos + (get_xsize() >> 1);
	int target_y = ypos + (get_ysize() >> 1);
	//draw_copy(raw_target, 0 , 0 , get_xsize() , get_ysize() , target_x, target_y );
	draw_view ( raw_target , target_x , target_y );
}
