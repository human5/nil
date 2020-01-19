/***************************************************************************
  viewport.cpp  -  place to stick all the info needed for rendering a view
                   of the arena
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

#include "client/graphics/viewport.h"
#include "common/misc.h"
#include "common/systemheaders.h"
#include "common/nil_math.h"

#define DROP 2

Viewport::Viewport(int width, int height, int owner, int map_xsize_, int map_ysize_) : Mutable_raw_surface() {
	ownerid = owner;
	xesum = 0;
	yesum = 0;
	xspeed = 0;
	yspeed = 0;
	xpos = ypos = 0;

	xcenter = width/2;
	ycenter = height/2;
	map_xsize = map_xsize_;
	map_ysize = map_ysize_;

	setpos(ROUND(xcenter),ROUND(ycenter));

// 	surface = new Surface(width,height,PIXELFORMAT);
	create(width, height);
}

Viewport::~Viewport() {
	//TODO: is there an oposite of create?
}

void Viewport::drop_shadow() {
	const int32 *source = get_pixels_read();
	int32 *target = get_pixels_write();
	int x_count, y_count;

	source += xsize*ysize-(DROP+DROP*xsize);
	target += xsize*ysize;

	for (y_count=ysize-DROP; y_count>0; --y_count) {
		for (x_count=xsize-DROP; x_count>0; --x_count) {
			source--;
			target--;
			if (!IS_PASSABLE(*source) && IS_PASSABLE(*target))
				*target = (*target>>1) & 0x7f7f7f;
		}
		source-= DROP;
		target-= DROP;
	}
}

void  Viewport::addsample(int x, int y, float dt) {
	if ( xcenter + 1 == xcenter )
		xcenter = x;
	else {
		float xe = x-xcenter;
		xcenter += xe * ( 0.7 + xe*xe*0.0001) * dt;
	}

	if ( ycenter + 1 == ycenter )
		ycenter = y;
	else {
		float ye = y-ycenter;
		ycenter += ye * ( 0.7 + ye*ye*0.0001) * dt;
	}

	setpos(ROUND(xcenter),ROUND(ycenter));
}

void Viewport::setpos(int x, int y) {
	x -= xsize/2;
	y -= ysize/2;
	if (x < 0) { x = 0; xesum=0; }
	if (y < 0) { y = 0; yesum=0; }
	if (x > map_xsize-xsize) { x = map_xsize-xsize; xesum = 0; }
	if (y > map_ysize-ysize) { y = map_ysize-ysize; yesum = 0; }
	xpos = x;
	ypos = y;
}

#ifdef this_comment_stretches_to_eof

//Figure out what area code a point is in
Viewport::Area_code Viewport::area_code(int x, int y) {
	Area_code code = AC_INSIDE;
	if (y > raw_target.ysize-1)
		code = AC_BOTTOM;
	else if (y < 0)
		code = AC_TOP;

	if (x > raw_target.xsize-1)
		code |= AC_RIGHT;
	else if (x < 0)
		code |= AC_LEFT;
	return code;
}

/* A bit shorter version:
Area_code Viewport::area_code(int x, int y) {
	return ((y > ypos+raw_target.ysize-1)?AC_BOTTOM:((y < ypos)?AC_TOP :0)) | ((x > xpos+raw_target.xsize-1)?AC_RIGHT :((x < xpos)?AC_LEFT:0));
}
*/

#define ERRORBITS 16
/**
	Get down with bresenham

	This responds badly to endpoints outside of the screen,
	use the clipping version in stead...

	This routine is a highly perverted bresenham line algorithm
	it has been optimized in various non-logical ways,
	be very careful when fooling with it.
	note that the y component is scaled by xsize in the inner loops,
	so we must add or substract in xsize increments.
*/
void Viewport::drawline_unclipped(int x0,int y0, int x1,int y1, int32 color, unsigned int style) {
	//clamp in the y direction (but not in x, overflowing in x will not segfault:)
	if (y1 > raw_target.ysize-1) y1 = raw_target.ysize-1;
	if (y0 > raw_target.ysize-1) y0 = raw_target.ysize-1;
	if (y1 < 0) y1 = 0;
	if (y0 < 0) y0 = 0;

	//get delta values
	int dx = x1-x0;
	int dy = y1-y0;

	//zero length is boring so get rid of it.
	if (dx == 0 && dy == 0) return;

	if (ABS(dx) > ABS(dy)) {
		// x major
	
		int ymove = raw_target.xsize*SIGN(dy); //The step to take with y when error hits 1
		dy = (ABS(dy) << ERRORBITS)/ABS(dx);   //The value to add to error each x
		int e = 0;                             //error, fixed point with errorbits bits between 0 and 1.
		int x = x0;                            //x is just the x coordinate
		int y = y0*raw_target.xsize;           //y is offset into buffer, not the y coordinate (this saves many muls)
		dx = SIGN(dx);                         //we just need the sign of dx
		int old_y = y;

		x1 += dx;
		while (x != x1) {

			//set the pixel
			raw_target.pixels[x+y] = color;

			//tally the error and ajust
			e += dy;
			if (e >= (1 << ERRORBITS)) {
				old_y = y;
				y += ymove;
				e -= (1 << ERRORBITS);
			}

			//move x one pixel in the right direction
			x += dx;

		} //while (x != x1) {
	
	} else {
		//y major

		int xmove = SIGN(dx);                //The step to take with x when error hits 1 << ERRORBITS
		dx = (ABS(dx) << ERRORBITS)/ABS(dy); //The value to add to error each y
		int e = 0;                           //error, fixed point with ERRORBITS bits between 0 and 1.
		int x = x0;                          //x is just the x coordinate
		int y = y0*raw_target.xsize;         //y is offset into buffer, not the y coordinate (this saves many muls)
		int sign_dy = SIGN(dy);
		dy = raw_target.xsize*sign_dy;       //we just need the sign of dy scaled to one pixel up or down
		y1 = (y1+sign_dy)*raw_target.xsize;
		int old_x = x;

		while (y != y1) {

			//set the color
			raw_target.pixels[x+y] = color;

			//tally the error and ajust
			e += dx;
			if (e >= (1 << ERRORBITS)) {
				old_x = x;
				x += xmove;
				e -= (1 << ERRORBITS);
			}

			//move y one pixel in the right direction
			y += dy;

		} //while (y != y1) {

	}

}

/**
	Party with Cohen and sutherland

  This came from: http://magic.hurrah.com/~sabre/graphpro/line6.html#ClipCode

	This routine clips a line to the current clip boundaries and
 	then calls drawline_unclipped() with the clipped coordinates. If the line
 	lies completely outside of the clip boundary, then the Line routine is not
 	called.  This procedure uses the well known Cohen-Sutherland line clipping
 	algorithm to clip each coordinate.
*/
void Viewport::drawline(int x0,int y0,int x1,int y1, int32 color, unsigned int style) {
	x0 -= xpos;
	y0 -= ypos;
	x1 -= xpos;
	y1 -= ypos;

	Area_code code0 = area_code(x0,y0);
	Area_code code1 = area_code(x1,y1);

	while (code0 != AC_INSIDE || code1 != AC_INSIDE) {
		// While both ends are not inside the clipping region

		if (code0 & code1) {
			return; // Trivial Reject (each end is off to the same side)
		} else {
			// Failed both tests, so calculate the line segment to clip
			Area_code codeout;
			if (code0)
				codeout = code0; // Clip the first point
			else
				codeout = code1; // Clip the last point
			int x=0;
			int y=0;
			if (codeout & AC_BOTTOM) {
				// Clip the line to the bottom of the viewport
				y = raw_target.ysize-1;
				x = x0+(x1-x0)*(y-y0) / (y1 - y0);
			} else if (codeout & AC_TOP) {
				// Clip the line to the top of the viewport
				y = 0;
				x = x0+(x1-x0)*(y-y0) / (y1 - y0);
			} else if (codeout & AC_RIGHT) {
				// Clip the line to the right edge of the viewport
				x = raw_target.xsize-1;
				y = y0+(y1-y0)*(x-x0) / (x1-x0);
			} else if (codeout & AC_LEFT) {
				// Clip the line to the left edge of the viewport
				x = 0;
				y = y0+(y1-y0)*(x-x0) / (x1-x0);
			}
			if (codeout == code0) {
				// Modify the first coordinate
				x0 = x; y0 = y;
				code0 = area_code(x0, y0);
			} else {
				//Modify the second coordinate
				x1 = x; y1 = y;
				code1 = area_code(x1, y1);
			}
		} // if (!(code0 & code1))
	} // while (code0 <> 0 || code1 <> 0) do {
	drawline_unclipped(x0,y0, x1,y1, color, style);
}

#endif
