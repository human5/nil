/***************************************************************************
  nil_math.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-01-09
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

    copyright            : (C) 2000 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

#include "nil_math.h"

/**************************************************************
 *    NOTE:  The following macro to determine if two numbers  *
 *    have the same sign, is for 2's complement number        *
 *    representation.  It will need to be modified for other  *
 *    number systems.                                         *
 **************************************************************/

#define SAME_SIGNS( a, b ) \
 (((long) ((unsigned long) a ^ (unsigned long) b)) >= 0 )



/* ***************************************************

0: n
1: nne
2: ne
3: ene
4: e

5:

rad: 0=north, +=cw.

dir: -1: west, +1: east
*************************************************** */

float Math::dist(float x0, float y0, float x1, float y1) {
	return sqrt(SQR(x1-x0)+SQR(y1-y0));
}

Vector Math::rad2vector(float rad) {
	rad = -rad-PI/2;
	Vector v;
	v.x = sin(rad);
	v.y = -cos(rad);
	return v;
}

Vector Math::rot90ccw(Vector v) {
	Vector vr;
	vr.x = -v.y;
	vr.y = v.x;
	return vr;
}

int Math::elev2dir(float elev, int dir) {
	if (elev > PI) elev = PI;
	if (elev < 0) elev = 0;

	if (dir > 0)
		return ROUND(8-8*elev/PI);
	else
		return ROUND(8+8*elev/PI);
}

float Math::elev2rad(float elev, int dir) {
	if (dir < 0)
		return PI/2-elev;
	else
		return PI/2+elev;
}

Vector Math::normalize(Vector v) {
	float length = sqrt(SQR(v.x)+SQR(v.y));
	if (length != 0) {
		v.x /= length;
		v.y /= length;
	}
	return v;
}

int Math::triangle_direction(float ax, float ay, float bx, float by, float cx, float cy) {
	float abx = ax-bx;
	float aby = ay-by;
	float cbx = cx-bx;
	float cby = cy-by;

	float d  = abx*cby-aby*cbx;

	if (d > 0)
		return 1;
	else
		return -1;
}

bool Math::point_in_triangle(float px, float py, float ax, float ay, float bx, float by, float cx, float cy) {
	int d1 = triangle_direction(px,py,ax,ay,bx,by);
	int d2 = triangle_direction(px,py,bx,by,cx,cy);
	int d3 = triangle_direction(px,py,cx,cy,ax,ay);

	return (d1 == d2) && (d2 == d3);
}

bool Math::lines_intersect(long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long *x, long *y) {
	// (x1, y1)->(x2, y2) is the first line segment
	// (x2, y2)->(x3, y3) is the second line segment
	// *x, *y are the points of intersection (output)
	
	// Compute a1, b1, c1, where line joining points 1 and 2 is "a1 x + b1 y + c1 = 0"
	long a1 = y2 - y1;
	long b1 = x1 - x2;
	long c1 = x2 * y1 - x1 * y2;
	// Compute r3 and r4.
	long r3 = a1 * x3 + b1 * y3 + c1;
	long r4 = a1 * x4 + b1 * y4 + c1;
	/* Check signs of r3 and r4.  If both point 3 and point 4 lie on same side of line 1,
	   the line segments do not intersect. */
	if ( r3 != 0 && r4 != 0 && SAME_SIGNS( r3, r4 ))
		return false;
	// Compute a2, b2, c2
	long a2 = y4 - y3;
	long b2 = x3 - x4;
	long c2 = x4 * y3 - x3 * y4;
	// Compute r1 and r2
	long r1 = a2 * x1 + b2 * y1 + c2;
	long r2 = a2 * x2 + b2 * y2 + c2;
	/* Check signs of r1 and r2.  If both point 1 and point 2 lie on same side of second
	   line segment, the line segments do not intersect. */
	if ( r1 != 0 && r2 != 0 && SAME_SIGNS( r1, r2 ))
		return false;
	// Line segments intersect: compute intersection point.
	long denom = a1 * b2 - a2 * b1;
	if ( denom == 0 )
		return false;
	long offset = denom < 0 ? - denom / 2 : denom / 2;
	/* The denom/2 is to get rounding instead of truncating.  It is added or subtracted to
	   the numerator, depending upon the sign of the numerator. */
	long num = b1 * c2 - b2 * c1;
	*x = ( num < 0 ? num - offset : num + offset ) / denom;
	num = a2 * c1 - a1 * c2;
	*y = ( num < 0 ? num - offset : num + offset ) / denom;
	return true;
}

int Math::min(int x,int y) {
	if (x < y)
		return x;
	else
		return y;
}

void Vector::rotate(float angle) {
	float newx = x*sin(angle) - y*cos(angle);
	float newy = y*sin(angle) + x*cos(angle);
	x = newx;
	y = newy;
}

void Vector::add(const Vector &v) {
	x += v.x; y += v.y;
}
