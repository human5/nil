/***************************************************************************
  graphic_bar.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-12-25
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "client/graphics/graphic_bar.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/nil_math.h"
#include <string.h>

void Graphic_bar::setgeom(int xpos_, int ypos_, int xsize_, int ysize_) {
	if (get_xsize() != xsize_ || get_ysize() != ysize_)
		create(xsize_, ysize_);
	xpos = xpos_;
	ypos = ypos_;
}

void Graphic_bar::draw(Mutable_raw_surface *raw_target) {
	int target_x = xpos + (get_xsize() >> 1);
	int target_y = ypos + (get_ysize() >> 1);
	draw_view(raw_target, target_x, target_y);
}

Graphic_bar::Graphic_bar() : Graphic() {
	bar_color = PIXELA(255,0,0,6);
	border_color = PIXELA(255,255,255,15);
	bar_length = 5;
	setpos(0.5);
	xpos = 0;
	ypos = 0;
}

Graphic_bar::~Graphic_bar() {}

// FIXME: consolidate ALL rendering gadgets in the rawbuffer class
// like line and horline and stuff
inline void horline(int32 *pixels, int length, int32 color) {
	for(int x=0;x<length;x++) *(pixels+x) = color;
}

void Graphic_bar::setpos(float pos) {
	// check value sanity, <0 often happens with weapons reload
	if (pos < 0) {
		//logmsg(lt_debug,"Invalid pos value passed to Graphic_bar::setpos(): %.3f, set to zero",pos);
		pos = 0.0;
	}
	if (pos > 1) {
		//logmsg(lt_error,"Invalid pos value passed to Graphic_bar::setpos(): %.3f, set to one",pos);
		pos = 1.0;
	}

	int new_bar_length = ROUND((get_xsize()-2)*pos);

	if (new_bar_length != bar_length) {
		bar_length = new_bar_length;
		render();
	}
}

void Graphic_bar::render() {
	int xsize = get_xsize();
	int ysize = get_ysize();
	int32 *pixels = get_pixels_write();

	if (xsize<=0 || ysize<=0 || !pixels) return;
	
	//Start by clearing the area:
	memset(pixels,0,xsize*ysize*sizeof(int32));
	//First make the border
	get_mutable_surface()->draw_rectangle_border(0, 0, xsize-1, ysize-1, border_color, Raw_surface::NOBLEND);
	//Draw the bar
	get_mutable_surface()->draw_rectangle(1, 1, bar_length+1, ysize-2+1, bar_color, Raw_surface::NOBLEND);
	//Draw the top of the bar
	get_mutable_surface()->draw_vertical_line(bar_length+1, 1, xsize-1, border_color, Raw_surface::NOBLEND);
}
