/***************************************************************************
  graphic.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-17
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-05-17
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Include SDL headers via systemheaders.h

    copyright            : (C) 1999 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "raw_surface.h"
#include "common/systemheaders.h"

class Loader;

//! @author Flemming Frandsen
//! @brief Class to hold a single image
class Graphic {
public:
	//! Constructor creating a new empty surface
	Graphic();
	//! Destructor deleting the surface
	~Graphic();
	/*! create an new mutable raw surface
	    @param xsize The x size of the new surface
	    @param ysize The y size of the new surface */
	void create(int xsize, int ysize);
	/*! create a new mutable raw surface
	    @param graphic A reference to a graphic*/
	void create(Graphic *graphic);
	/*! return if the surface is mutable
	    @return Returns true, if a surface exists and is mutable */
	bool is_mutable() const { return surface && surface->is_mutable(); }
	/*! load a rgbf to a surface
	    @param loader A reference to the loader
	    @param name The name of the graphic to load
	    @return Returns true, if sucessfully loaded */
	bool load(Loader *loader, const char *name);
#ifndef WITHOUT_SDL
	/*! for font handling
	    @param source An SDL surface*/
	bool create ( SDL_Surface *source );
#endif
	//! make transparent parts of a surface transparent
	void maketransparent();
	/*! Get a handle to the current surface
	    @return Returns the handle for the current surface */
	inline const Raw_surface *get_surface() const { return surface; }
	/*! Get a handle to the current surface, if mutable
	    @return Returns the handle for the current surface */
	inline Mutable_raw_surface *get_mutable_surface() const {
		if (get_surface() && get_surface()->is_mutable()) {
			return (Mutable_raw_surface *)get_surface();
		} else {
			return 0;
		}
	}
	/*! ???
	    @return Returns get_pixels_write if a surface exists, otherwise null */
	int32 *get_pixels_write() const { return get_surface()?get_surface()->get_pixels_write():0; }
	/*! ???
	    @param color The color
	    @param base ??? */
	void apply_color_under_offset( int32 color, int base ) {
		if (!get_mutable_surface()) { return; }
		get_mutable_surface()->apply_color_under_offset(color,base);
	}
	//! ???
	const int32 *get_pixels_read() const { return get_surface()?get_surface()->get_pixels_read():0; }

	//! returns the x size of the surface
	int get_xsize() const { return get_surface()?get_surface()->get_xsize():-1; }
	//! returns the y size of the surface
	int get_ysize() const { return get_surface()?get_surface()->get_ysize():-1; }

	//! ???
	void draw_copy_stretch(Mutable_raw_surface *target, int source_xpos,  int source_ypos, int source_xsize, int source_ysize, int target_xpos,  int target_ypos, int enlargement = 1) const {
		if (!get_surface()) { return; }
		get_surface()->draw_copy_stretch(target, source_xpos,source_ypos, source_xsize,source_ysize, target_xpos,target_ypos, enlargement);
	}

	//! ???
	void draw_copy(Mutable_raw_surface *target, int target_xpos,  int target_ypos) const {
 		if (!get_surface()) { return; }
 		get_surface()->draw_copy(target, 0, 0, get_xsize(),get_ysize(), target_xpos,target_ypos);
 	}

	//! ???
	void draw_alpha(Mutable_raw_surface *target, int xpos, int ypos) const {
		if (!get_surface()) { return; }
		get_surface()->draw_alpha(target, xpos, ypos);
	}
	//! ???
	void draw_view(Mutable_raw_surface *target, int xpos, int ypos) const {
		if (!get_surface()) { return; }
		get_surface()->draw_view(target,xpos,ypos);
	}
	//! ???
	void draw_shadow_only( Mutable_raw_surface *target, int xpos, int ypos ) const {
		if( !get_surface() ) { return; }
		get_surface()->draw_shadow_only( target, xpos, ypos );
	}
	//! ???
	void draw_map(Mutable_raw_surface *target_map, Mutable_raw_surface *target_bgmap, int xpos, int ypos) const {
		if (!get_surface()) { return; }
		get_surface()->draw_map(target_map,target_bgmap,xpos,ypos);
	}

protected:
	//! the surface that contains the graphic
	Raw_surface *surface;
};

#endif
