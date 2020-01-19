/***************************************************************************
  nil_math.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-01-09
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef NIL_MATH_H
#define NIL_MATH_H

#include "systemheaders.h"

//some general purpose macros:
#define SIGN(x) (((x) > 0)?1:(((x) < 0)?-1:0))
#define ABS(x) ((x) < 0?-(x):(x))
#define SQR(x) ((x)*(x))
#define ROUND(x) ((int)((x)+0.5))
#define CLAMP(x,min,max) ((x)<(min)?(min):((x)>(max)?(max):(x)))

#define PI (float)3.14159265

#define STEP (PI/8)
#define STEP_HALF (STEP/2)
#define MINELEVATION (STEP*1)
#define MAXELEVATION PI

//! @author Flemming Frandsen
//! @brief Class for vector-handling
class Vector {
public:
	//! the x-coordinate of the vector
	float x;
	//! the y-coordinate of the vector
	float y;
	/*! Constructor that stores given x and y values
	    @param nx The x coordinate
	    @param ny The y coordinate */
	Vector(const float nx, const float ny): x (nx), y (ny) {}
	//! Constructor
	Vector(): x (0), y (0) {}
	//! Set the vector to be (0,0)
	void zero() { x = 0; y = 0; }
	/*! Add a vector to the current vector
	    @param v The vector */
	void add(const Vector &v);
	/*! Get the length of this vector
	    @returns The length of the vector */
	float length() const { return sqrt(SQR(x) + SQR(y)); }
	//! Set the length of the vector to 1
	void normalize() { const float len = length(); if (len == 0) return; x /= len; y /= len; }
	/*! Lengthen/Shorten the vector by the given factor
	    @param factor The scaling factor */
	void scale(const float factor) {x *= factor; y *= factor;}
	/*! Rotate a Vector by the given angle
	    @param angle The angle*/
	void rotate(const float angle);
};

class Math {
public:
	/*! calculate the distance between 2 point
	    @param x0 X-coordinate of the starting point
	    @param y0 Y-coordinate of the starting point
	    @param x1 X-coordinate of the ending point
	    @param y1 Y-coordinate of the ending point
	    @return Returns the distance */
	static float dist(float x0, float y0, float x1, float y1);
	/*! calculate a vector based on a radius
	    @param rad The radius we need the vector for
	    @return The vector that represents the radius */
	static Vector rad2vector(float rad);
	/*! Rotate a vector by 90° counterclockwise
	    @param v The vector we want to rotate
	    @return The new vector*/
	static Vector rot90ccw(Vector v);
	//! ???
	static int elev2dir(float elev, int dir);
	//! ???
	static float elev2rad(float elev, int dir);
	//! ???
	static Vector normalize(Vector v);
	//! ???
	static int triangle_direction(float ax, float ay, float bx, float by, float cx, float cy);
	//! ???
	static bool point_in_triangle(float px, float py, float ax, float ay, float bx, float by, float cx, float cy);
	/*! lines_intersect, stolen from Graphic Gems 2
	    This function computes whether two line segments,
	    respectively joining the input points (x1,y1) -- (x2,y2)
	    and the input points (x3,y3) -- (x4,y4) intersect.
	    If the lines intersect, the output variables x, y are
	    set to coordinates of the point of intersection.
	
	    All values are in integers.  The returned value is rounded
	    to the nearest integer point.
	
	    If non-integral grid points are relevant, the function
	    can easily be transformed by substituting floating point
	    calculations instead of integer calculations.
	    @param x1 Coordinates of endpoints of one segment.
	    @param y1 Coordinates of endpoints of one segment.
	    @param x2 Coordinates of endpoints of one segment.
	    @param y2 Coordinates of endpoints of one segment.
	    @param x3 Coordinates of endpoints of other segment.
	    @param y3 Coordinates of endpoints of other segment.
	    @param x4 Coordinates of endpoints of other segment.
	    @param y4 Coordinates of endpoints of other segment.
	    @param x Coordinates of intersection point.
	    @param y Coordinates of intersection point.
	    @return DONT_INTERSECT    0
	            DO_INTERSECT      1
	            COLLINEAR         2
	    Error conditions:
	
	    Depending upon the possible ranges, and particularly on 16-bit
	    computers, care should be taken to protect from overflow.

	    In the following code, 'long' values have been used for this
	    purpose, instead of 'int'. */
	static bool lines_intersect(long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long *x, long *y);
	//! ???
	static int min(int x,int y);
};

#endif
