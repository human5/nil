/***************************************************************************
  graphic.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-17
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-05-16
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "client/graphics/graphic.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/systemheaders.h"
#include "common/vfs/loader.h"

Graphic::Graphic() {
	surface=NULL;
}

Graphic::~Graphic() {
	delete surface;
}

void Graphic::create(int xsize, int ysize) {
	if (surface && !surface->is_mutable()) {
		delete surface;
		surface=0;
	}
	if (!surface) {
		surface=new Mutable_raw_surface();
	}
	dynamic_cast<Mutable_raw_surface *>(surface)->create(xsize,ysize);
}

void Graphic::create(Graphic *graphic) {
	if (surface && !surface->is_mutable()) {
		delete surface;
		surface=0;
	}
	if (!surface) {
		surface=new Mutable_raw_surface();
	}
	dynamic_cast<Mutable_raw_surface *>(surface)->create(graphic->get_surface());
}

bool Graphic::load(Loader *loader, char *name) {
	char *file = (char*)loader->getfile(name);
	if (!file) return false;
	if (surface && !is_mutable()) {
		delete surface;
		surface=NULL;
	}
	if (!surface) {
		surface=new Mutable_raw_surface();
	}
	Rgbf_header header;
	memcpy(&header,file,sizeof(header));

	//read old format:
	// FIXME: RGBF_HEADER_MAGIC_OLD: integer conversion resulted in a change of sign
	if (header.magic == RGBF_HEADER_MAGIC_OLD) {
		logmsg(lt_warning,"The file %s is in the old intel centric format, fix it!",name);
		dynamic_cast<Mutable_raw_surface *>(surface)->attach(header.xsize, header.ysize, (int32 *)(file+sizeof(header)));
		return true;
	}

	//swap and read new format:	
	header.swap();
	if (header.magic == RGBF_HEADER_MAGIC) {
		dynamic_cast<Mutable_raw_surface *>(surface)->attach(header.xsize, header.ysize, (int32 *)(file+sizeof(header)));
		return true;
	}

	SDL_RWops *src=SDL_RWFromMem(file, loader->filesize(name));
	SDL_Surface *tmpsurface=IMG_Load_RW(src, 1);

	if(tmpsurface) {
		// Convert the surface to the internal format.
		// FIXME: I assume this is the same as the display format!
		SDL_Surface *oursurface=SDL_DisplayFormatAlpha(tmpsurface);
		// Free the original surface
		SDL_FreeSurface(tmpsurface);
		// Tricky memory games: attach a surface to the SDL image (which we
		// won't manage), then copy it to our own surface, and finally be
		// sure to free the temporary raw surface first!
		Mutable_raw_surface *tmpraw=new Mutable_raw_surface;
		tmpraw->attach(oursurface->w, oursurface->h, (int32 *) oursurface->pixels);
		// Note that we have ensured that this is mutable.
		dynamic_cast<Mutable_raw_surface *>(surface)->create(tmpraw);
		delete tmpraw;
		SDL_FreeSurface(oursurface);
		return true;
	} else {
		logmsg(lt_error, "Unable to load %s", name);
		return false;
	}
}

bool Graphic::create ( SDL_Surface *source ) {
	if (surface && !is_mutable()) {
		delete surface;
		surface=NULL;
	}
	if (!surface) {
		surface=new Mutable_raw_surface();
	}
	if(source) {
		// Convert the surface to the internal format.
		// FIXME: I assume this is the same as the display format!
		SDL_Surface *oursurface=SDL_DisplayFormatAlpha(source);

		// Free the original surface
		// SDL_FreeSurface(source);

		// Tricky memory games: attach a surface to the SDL image (which we
		// won't manage), then copy it to our own surface, and finally be
		// sure to free the temporary raw surface first!
		Mutable_raw_surface *tmpraw=new Mutable_raw_surface;
		tmpraw->attach(oursurface->w, oursurface->h, (int32 *) oursurface->pixels);

		// Note that we have ensured that this is mutable.
		dynamic_cast<Mutable_raw_surface *>(surface)->create(tmpraw);
    
		delete tmpraw;
		SDL_FreeSurface(oursurface);

		return true;
	} else {
		logmsg(lt_error, "No surface!" );
		return false;
	}
}

void Graphic::maketransparent() {
	if (!is_mutable()) {
		return;
	}
	int32 *pixels=get_pixels_write();
	int xsize=get_xsize();
	int ysize=get_ysize();
	for (int yc=0; yc < ysize; yc++) {
		for (int xc=0; xc < xsize; xc++) {
			if (IS_OPAQUE(pixels[yc*xsize+xc])) {
				pixels[yc*xsize+xc] = (pixels[yc*xsize+xc] & 0x0fffffff) | PF_ALPHA_BIT0 | PF_ALPHA_BIT1;
			}
		}
	}
}

/*
s t r    rgb

v v v 3  -
h v h 2  -
e v e 2  -
s v s 2  -

v h H 3  b
h h h 2  -
e h e 2  s
s h s 2  s

v e E 3  t
h e h 2  b
e e e 2  -
s e s 2  s

v s S 1  t
h s S 1  t
e s S 1  t
s s s 1  s
*/

