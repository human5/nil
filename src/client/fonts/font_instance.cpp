/***************************************************************************
  font.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-01-30
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-02-03
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Use getrwops() of Loader insted of getfilename().
                         some identation fixes.

  last changed         : 2004-02-06
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Try to work around SDL_ttf bug by using 
                         TTF_Render*_blended instead of TTF_Render*_Solid.
                         (Let's hope nobody tries to draw that to hardware
                         surface).

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-11-06
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : added get_text_height and get_text_width
  
  last changed         : 2005-01-15
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : add return value for get_text_geom

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#include "client/fonts/font_instance.h"
#include "common/systemheaders.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/vfs/loader.h"
#include <map>

Font_instance::Font_instance() {
	font = NULL;
}

Font_instance::~Font_instance() {
	if (font) {
		TTF_CloseFont ( font );
		free ( name );
	}
}

bool Font_instance::load(Loader *loader, char *font_name, int32 size_ ) {
	char font_file[100];
	sprintf(font_file, "gfx/fonts/ttf/%s.ttf", font_name);
	SDL_RWops* ops = NULL;;

	name = strdup(font_name);

	ops = loader->getrwops(font_file);
	if(!ops) {
		logmsg(lt_warning, "Unable to get RWops for font file.");
		return false;
	}

	font = NULL;
	font = TTF_OpenFontRW(ops, 1, size_);
	if (!font) {
		logmsg ( lt_error , "Unable to load TTF font %s, size %d" , font_name , size_ );
		return false;
	}

	logmsg ( lt_debug , "Loaded TTF font %s, size %d" , font_name , size_ );
	size = size_;
	return true;
}

bool Font_instance::get_text_geom(char *text, int *width, int *height) {
	// we need to display this text to know his length
	SDL_Color black = { 0 , 0 , 0 , 0 };
	SDL_Surface *text_surf = TTF_RenderText_Solid(font, text, black);
	if (!text_surf) {
		logmsg (lt_warning , "Unable to render text %s, using font %s" , text , name);
		return false;
	}
	*width = text_surf->w;
	*height = text_surf->h;
	SDL_FreeSurface ( text_surf );
	return true;
}

int Font_instance::get_text_width(char *text) {
	SDL_Color black = { 0 , 0 , 0 , 0 };
	SDL_Surface *text_surf = TTF_RenderText_Solid(font, text, black);
	if (!text_surf) {
		logmsg (lt_warning , "Unable to render text %s, using font %s" , text , name);
		return -1;
	}
	int tempwidth = text_surf->w;
	SDL_FreeSurface (text_surf);
	return tempwidth;
}

int Font_instance::get_text_height(char *text) {
	SDL_Color black = { 0 , 0 , 0 , 0 };
	SDL_Surface *text_surf = TTF_RenderText_Solid(font, text, black);
	if (!text_surf) {
		logmsg (lt_warning , "Unable to render text %s, using font %s" , text , name);
		return -1;
	}
	int tempheight = text_surf->h;
	SDL_FreeSurface (text_surf);
	return tempheight;
}

bool Font_instance::put_text(int xpos, int ypos, const char *text, Mutable_raw_surface *target, bool direct_copy, uint8_t red, uint8_t green, uint8_t blue ) {
	SDL_Color white = { red, green, blue, 0 };

	// FIXME: support for other font rendering styles
	SDL_Surface *text_surf = TTF_RenderText_Solid (font, text, white);
	if ( !text_surf ) {
		logmsg (lt_warning , "Unable to render text %s, using font %s" , text , name);
		return false;
	}

	Graphic *tmp_graphic = new Graphic;
	tmp_graphic->create(text_surf);
	if (direct_copy)
		tmp_graphic->draw_alpha(target, xpos, ypos );
	else
		tmp_graphic->draw_copy(target, xpos, ypos );

	// free that !!!
	SDL_FreeSurface (text_surf);
	delete tmp_graphic;
	return true;
}
