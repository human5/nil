/***************************************************************************
  raw_surface.h  -  Header for code used to store the lowlevel graphics
                    rendering routines
  --------------------------------------------------------------------------
  begin                : 2000-01-03
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Pixel data is little endian.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

/***************************************************************************
  Note that pixels are ALWAYS 32 bit and in the NiL format
 ***************************************************************************/

#ifndef RAW_SURFACE_H
#define RAW_SURFACE_H

#include "common/nil_files.h"
#include <endian.h>

//! this must always become a 32bit integer
typedef unsigned int Pixel;

//! emulate ptc names
typedef Pixel int32;

#if __BYTE_ORDER == __LITTLE_ENDIAN

//! pixelflags used in the map:
#define PF_TYPE_BIT0 0x01000000
#define PF_TYPE_BIT1 0x02000000
#define PF_TYPE_BITS (PF_TYPE_BIT1 | PF_TYPE_BIT0)

//unused:
#define PF_FLAG2 0x04000000
#define PF_FLAG3 0x08000000

//Alpha:
#define PF_ALPHA_BIT0 0x10000000
#define PF_ALPHA_BIT1 0x20000000
#define PF_ALPHA_BIT2 0x40000000
#define PF_ALPHA_BIT3 0x80000000
#define PF_ALPHA_BITS 0xF0000000

//RGB
#define PF_RGB_BITS 0x00FFFFFF

//---------------------------------------------------------
//Define a couple of macros to access the pixel data
#define GETFLAG(x)  ((x) & 0xFF000000)
#define GETRED(x)   (((x) & 0x00FF0000) >> 16)
#define GETGREEN(x) (((x) & 0x0000FF00) >> 8)
#define GETBLUE(x)  (((x) & 0x000000FF))

#define GETALPHA(x) ((x) >> (32-4))

#define IS_INVISIBLE(x) (((x) & PF_ALPHA_BITS) == 0x00000000)
#define IS_OPAQUE(x) (((x) & PF_ALPHA_BITS) == 0xf0000000)

#define PIXEL(r,g,b) ( (((r) & 0xff)<<16) | (((g) & 0xff)<<8) | ((b) & 0xff) )
#define PIXELA(r,g,b,a) ((((a) & 0x0f)<<28) | (((r) & 0xff)<<16) | (((g) & 0xff)<<8) | ((b) & 0xff) )

//! returns the pixel format that is in use
#define PIXELFORMAT Format(32,0x00FF0000,0x0000FF00,0x000000FF)

#else

//! pixelflags used in the map:
#define PF_TYPE_BIT0 0x00000001
#define PF_TYPE_BIT1 0x00000002
#define PF_TYPE_BITS (PF_TYPE_BIT1 | PF_TYPE_BIT0)

//unused:
#define PF_FLAG2 0x00000004
#define PF_FLAG3 0x00000008

//Alpha:
#define PF_ALPHA_BIT0 0x00000010
#define PF_ALPHA_BIT1 0x00000020
#define PF_ALPHA_BIT2 0x00000040
#define PF_ALPHA_BIT3 0x00000080
#define PF_ALPHA_BITS 0x000000F0

//RGB
#define PF_RGB_BITS 0xFFFFFF00

//---------------------------------------------------------
//Define a couple of macros to access the pixel data
#define GETFLAG(x)  ((x) & 0x000000FF)
#define GETRED(x)   (((x) & 0x0000FF00) >> 8)
#define GETGREEN(x) (((x) & 0x00FF0000) >> 16)
#define GETBLUE(x)  (((x) & 0xFF000000) >> 24)

#define GETALPHA(x) (((x) >> (4)) & 0x0F)

#define IS_INVISIBLE(x) (((x) & PF_ALPHA_BITS) == 0x00000000)
#define IS_OPAQUE(x) (((x) & PF_ALPHA_BITS) == 0x000000F0)

#define PIXEL(r,g,b) ( (((r) & 0xff)<<8) | (((g) & 0xff)<<16) | ((b) & 0xff) << 24 )
#define PIXELA(r,g,b,a) ((((a) & 0x0f)<<4) | (((r) & 0xff)<<8) | (((g) & 0xff)<<16) | ((b) & 0xff) << 24 )

//! returns the pixel format that is in use
#define PIXELFORMAT Format(32,0x0000FF00,0x00FF0000,0xFF000000)


#endif



//---------------------------------------------------------
// Meaning of the pixel types bits:
#define PF_TYPE_VOID 0
#define PF_TYPE_HOLE PF_TYPE_BIT0
#define PF_TYPE_EARTH PF_TYPE_BIT1
#define PF_TYPE_STONE (PF_TYPE_BIT0 | PF_TYPE_BIT1)


//pixel tests:
#define IS_VOID(x)  (((x) & PF_TYPE_BITS) == PF_TYPE_VOID)
#define IS_HOLE(x)  (((x) & PF_TYPE_BITS) == PF_TYPE_HOLE)
#define IS_EARTH(x) (((x) & PF_TYPE_BITS) == PF_TYPE_EARTH)
#define IS_STONE(x) (((x) & PF_TYPE_BITS) == PF_TYPE_STONE)

#define IS_PASSABLE(x) (((x) & PF_TYPE_BITS) == PF_TYPE_HOLE)

class Mutable_raw_surface;

//! @brief Class for a raw surface
//! @author Flemming Frandsen
class Raw_surface {
public:
	//! Constructor
	Raw_surface();
	//! Destructor
	virtual ~Raw_surface();
	//! Create a surface by copying another
	void create(Raw_surface *original);
	//! attach to a surface that is managed outside this object
	void attach(int xsize_, int ysize_, int32 *pixels_);
	// These routines will draw on another Raw_surface:
	//! blending
	enum Blend {
		//! copy
		COPY,
		//! noblend=copy
		NOBLEND=COPY,
		//! alpha
		ALPHA,
		//! average
		AVERAGE,
		//! sat_add
		SAT_ADD,
		//! alpha_sat_add
		ALPHA_SAT_ADD
	};
	//! Copy a streched version of this surface on another Raw_surface
	virtual void draw_copy_stretch(Mutable_raw_surface *target, int source_xpos, int source_ypos, int source_xsize, int source_ysize, int target_xpos, int target_ypos, int target_xsize, int target_ysize, const Blend &blend) const;
	//! Copy a streched version of this surface on another Raw_surface
	virtual void draw_copy_stretch(Mutable_raw_surface *target, int source_xpos,  int source_ypos, int source_xsize, int source_ysize, int target_xpos,  int target_ypos, int enlargement = 1) const;
	//! Copy a rectangle to another Raw_surface, without handling flags or alpha (byte for byte copy)
	void draw_copy(Mutable_raw_surface *target, int source_xpos,  int source_ypos, int source_xsize, int source_ysize, int target_xpos,  int target_ypos) const;
	//! Render this to another Raw_surface, without handling the flags
	virtual void draw_alpha(Mutable_raw_surface *target, int xpos, int ypos) const;
	//! Render this to another Raw_surface, with viewport-centric handling of the flags
	virtual void draw_view(Mutable_raw_surface *target, int xpos, int ypos) const;
	//! Render the shadow of this surface to another Raw_surface
	virtual void draw_shadow_only(Mutable_raw_surface *target, int xpos, int ypos) const;
	/*! Render this to another Raw_surface, with map-centric handling of the flags
	    This is the slowest of the drawing routines */
	virtual void draw_map(Mutable_raw_surface *target_map, Mutable_raw_surface *target_bgmap, int xpos, int ypos) const;
	// Loading and saving routines:
	//! Save to a file
	bool save_to_file(char *filename);
	// These routines are helpers for others, they may be of limited use to most people
	/*! Use this in line drawing routines to have the target surface clip the line for the source;
	    returns false if the line is not visible at all */
	bool clip_line(int &x0, int &y0, int &x1, int &y1) const;
	// Use this to access the image directly:
	//! ???
	virtual const int32 *get_pixels_read() const { return 0; }
	//! ???
	virtual int32 *get_pixels_write() const { return 0; }
	//! ???
	virtual int get_xsize() const { return xsize; }
	//! ???
	virtual int get_ysize() const { return ysize; }
	//! return that this surface is NOT mutable
	virtual bool is_mutable() const { return false; }
protected:
	//! area code, to tell where we are
	enum Area_code {
		//! inside the world
		AC_INSIDE = 0,
		//! bottom of the world
		AC_BOTTOM = 1,
		//! top of the world
		AC_TOP    = 2,
		//! left
		AC_LEFT   = 4,
		//! right
		AC_RIGHT  = 8
	};
	//! ???
	Area_code area_code(int x, int y) const;
	// utility functions
	//! xsize for the bitmap
	int xsize;
	//! ysize for the bitmap
	int ysize;
};

// DO NOT SUBCLASS Mutable_raw_surface!
//! @author Flemming Frandsen
//! @brief a raw surface, this one is mutable
class Mutable_raw_surface : public Raw_surface {
public:
	//! Constructor
	Mutable_raw_surface();
	//! Destructor
	virtual ~Mutable_raw_surface();
	//! Create a surface that is managed by this object
	virtual void create(int xsize_, int ysize_);
	//! Create a surface by copying another
	virtual void create(const Raw_surface *original);
	//! attach to a surface that is managed outside this object
	virtual void attach(int xsize_, int ysize_, int32 *pixels_);
	//! load a surface from file, returns true on success
	bool load_from_file(char *filename);
	/* These routines will draw on this Mutable_raw_surface: The _raw versions will draw
	   the color directly, without regard to alpha, the normal ones will alphablend */
	//! Draws a bresenham line with clipping
	void draw_line(int x0, int y0, int x1, int y1, int32 color,const Blend &bf,unsigned int style =0);
	/*! draws a horiontal line (more quickly than the full bresenham routine)
	    Use this rather than draw_vertical_line() if practical, it is faster. */
	void draw_horizontal_line(int x0, int y0, int x1, int32 color,const Blend &bf);
	//! Draws a vertical line (more quickly than the full bresenham routine)
	void draw_vertical_line(int x0, int y0, int y1, int32 color,const Blend &bf);
	//! Draws a transparent rectangle, this is somewhat faster than using the draw_alpha() routine
	void draw_rectangle(int x0, int y0, int x1, int y1, int32 color,const Blend &bf);
	//! Draws a rectangle (just the lines, not filled)
	void draw_rectangle_border(int x0, int y0, int x1, int y1, int32 color,const Blend &bf);
	//! Directly set a pixel at some offset in the map without any checks
	inline void setRawPixel(int offset, int32 color);
	//! Directly set a pixel in the map without any checks
	inline void setRawPixel(int x,int y, int32 color);
	//! Directly set a pixel in the map, but perform clipping
	inline void setRawPixelCheck(int x,int y, int32 color);
	//! Modify a pixel type in the map, but perform clipping
	inline void setRawPixelType(int x,int y, int32 color);
	//! Directly set a pixel in the map without any checks, only modify color (not alpha bits)
	inline void setRawColor(int x,int y, int32 color);
	//! ???
	void apply_color_under_offset( int32 color, int base );
	// These routines are helpers for others, they may be of limited use to most people.
	//! returns that this surface is mutable
	virtual bool is_mutable() const { return true; }
	//! ???
	virtual int32 *get_pixels_write() const { return pixels; }
	//! ???
	virtual const int32 *get_pixels_read() const { return pixels; }
	// analogs of the above 4 functions that are not virtual for performance reasons.
	//! ???
	inline int32 *_get_pixels_write() const { return pixels; }
	//! ???
	inline const int32 *_get_pixels_read() const { return pixels; }
	//! ???
	inline int _get_xsize()	const	{ return xsize; }
	//! ???
	inline int _get_ysize()	const	{ return ysize; }
protected:
	//! ???
	void free_pixels();
	//! ???
	int32 *pixels; 	
	//! if true, don't free the buffer
	bool attached;
};

//! Directly set a pixel at some offset in the map without any checks
inline void Mutable_raw_surface::setRawPixel(int offset, int32 color) {
	*(pixels + offset) = color;
}

//! Directly set a pixel in the map without any checks
inline void Mutable_raw_surface::setRawPixel(int x,int y, int32 color) {
	*(pixels + y*xsize+ x) = color;
}

//! Directly set a pixel in the map, but perform clipping
inline void Mutable_raw_surface::setRawPixelCheck(int x, int y, int32 color) {
	if(x<0) return;
	if(y<0) return;
	if(x>=xsize) return;
	if(y>=ysize) return;
	*(pixels + y*xsize+ x) = color;
}

//! Directly set a pixel in the map without any checks
inline void Mutable_raw_surface::setRawColor(int x,int y, int32 color) {
	int32 *pixel = (pixels + y*xsize+ x);
	*pixel = color | (*pixel & (PF_ALPHA_BITS | PF_TYPE_BITS));
}

//! Modify a pixel type in the map, but perform clipping
inline void Mutable_raw_surface::setRawPixelType(int x,int y, int32 color) {
	if(x<0) return;
	if(y<0) return;
	if(x>=xsize) return;
	if(y>=ysize) return;
	int32 *pix = (pixels + y*xsize+ x);
	*pix = (color & PF_TYPE_BITS) | ( *pix & ~PF_TYPE_BITS);
}


#endif
