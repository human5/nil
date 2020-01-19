/***************************************************************************
  view_rgbf.cpp  -  code handling rgbf format loading
  --------------------------------------------------------------------------
  begin                : 1999-10-11
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-28
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-08
  by                   : Phil Howlett
  email                : phowlett@bigpond.net.au
  description          : Changed SDL_UpdateRect to SDL_Flip so that
                         double buffering works correctly.

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

#include "common/systemheaders.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/vfs/loader.h"
#include "client/graphics/graphic.h"
#include "client/graphics/raw_surface.h"
#include "common/configuration/configuration.h"

void view_rgbf(Configuration &config) {
	int sdl_flags = /*SDL_INIT_AUDIO |*/ SDL_INIT_VIDEO;

	if (SDL_Init(sdl_flags) < 0) {
		logmsg(lt_fatal,"Unable to initialize SDL: %i", SDL_GetError());
		return;
	}
	atexit(SDL_Quit);

	Loader loader;
	loader.addsources(config.get_option("data"));

	Graphic *g=loader.graphics.getgraphic(strdup(config.get_option("inputfile")));

	int sdl_video_flags = SDL_HWSURFACE;

	SDL_Surface *screen = SDL_SetVideoMode(g->get_xsize(), g->get_ysize(), 32, sdl_video_flags);

	if (!screen) {
		logmsg(lt_fatal,"Couldn't set %ix%ix32 video mode: %s\n",g->get_xsize(),g->get_ysize(),SDL_GetError());
		return;
	}

	Mutable_raw_surface tmp;
	if (SDL_MUSTLOCK(screen)) {
		if ( SDL_LockSurface(screen) < 0 ) {
			logmsg(lt_error,"Unable to lock SDL surface");		
		}
	}
	tmp.attach(g->get_xsize(),g->get_ysize(),(int32 *)screen->pixels);

	g->draw_copy_stretch(&tmp, 0, 0, g->get_xsize(), g->get_ysize(), 0, 0);

	if ( SDL_MUSTLOCK(screen) ) {
		SDL_UnlockSurface(screen);
	}

	// Replacing SDL_UpdateRect with SDL_Flip so that double buffering works
	// - Phil Howlett 8th Sept 2004
	//
	//SDL_UpdateRect(screen, 0,0, g->get_xsize(), g->get_ysize());
	SDL_Flip(screen);

	bool done=false;

	while (!done) {
		SDL_PumpEvents();
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if(event.type==SDL_KEYDOWN && ((SDL_KeyboardEvent *) &event)->keysym.sym==SDLK_q)
			done=true;
		}
	}
}
