/***************************************************************************
  icon_graph.cpp  -  network traffic graph
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

#include "client/graphics/icon_graph.h"
#include "common/nil_math.h"
#include "common/misc.h"

Icon_graph::Icon_graph() {}

Icon_graph::~Icon_graph() {}

void Icon_graph::init(int xpos_, int ypos_,int xsize_, int ysize_, int32 color_, int32 graphcolor_) {
	create(xsize_, ysize_);
	color = color_;
	graphcolor = graphcolor_;
	xpos = xpos_;
	ypos = ypos_;
	draw_rectangle(0, 0, xsize, ysize, color, Raw_surface::NOBLEND);
}

void Icon_graph::addsample(float percent) {
	// move the pixels
	int32 *pixel = pixels;
	int32 *end = pixel+xsize*ysize;
	while (pixel < end) {
		*pixel = *(pixel+1);
		pixel++;
	}

	// add a new colum
	if (percent < 0) percent = 0;
	if (percent > 1) percent = 1;
	int split = ysize-ROUND(ysize*percent);
	draw_vertical_line(xsize-1, 0, split, color, Raw_surface::NOBLEND);
	draw_vertical_line(xsize-1, split, ysize, graphcolor, Raw_surface::NOBLEND);
}

void Icon_graph::draw(Mutable_raw_surface *target) {
	draw_alpha(target, xpos, ypos);
}
