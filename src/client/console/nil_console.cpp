/***************************************************************************
  nil_console.cpp  -  display of nil system messages
 ---------------------------------------------------------------------------
  begin                : 2003-06-17
  by                   : Nils Thuerey (n_t@gmx.de)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-08
  by                   : Phil Howlett (phowlett@bigpond.net.au)
  description          : Changed SDL_UpdateRect to SDL_Flip so that
                         double buffering works correctly.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-10-24
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Turned Nil_console into type of console.

  last changed         : 2004-10-24
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Declare destructor to throw no exceptions.

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Move getShowConsole and setShowConsole from 
                         logmsg.cpp
  
  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Move again, with huge API cleanup

  copyright            : (C) 2003 by Nils Thuerey (n_t@gmx.de)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "client/console/nil_console.h"
#include "client/fonts/font_instance.h"

Nil_console* global_console = NULL;

Nil_console::Nil_console(SDL_Surface *screen_, Config_xy screen_size_, Graphic *logo_, Font_instance *font) {
	// init pointers
	screen = screen_;
	screen_size = screen_size_;
	logo = logo_;
	// delete all messages
	for(unsigned int i=0; i < Nil_console::LINENUM; i++) {
		msg_color[i] = 0;
		msg_text[i][0] = 0;
	}
	graphic_msg.set_font(font);
	master_console.add_slave(*this);
}

Nil_console::~Nil_console() throw() {
	master_console.remove_slave(*this);
	//FIXME: what about graphic_msg?
}

void Nil_console::add_message(char *message, int32 color) {
	for (int i=Nil_console::LINENUM-1; i>0; i--) {
		msg_color[i] = msg_color[i-1];
		if((msg_text[i-1][0]!=0)) {
			strcpy(msg_text[i], msg_text[i-1]); 
		} else {
			msg_text[i][0] = 0;
		}
	}
	strncpy(msg_text[0], message, Nil_console::LINELENGTH);
	if (strlen(message) >= Nil_console::LINELENGTH)
		msg_text[0][Nil_console::LINELENGTH-1] = '\0';
	msg_color[0] = color;
}

void Nil_console::draw(SDL_Surface *screen_) {
	if(screen_ == NULL)
		screen_ = screen;

	Mutable_raw_surface tmp; 
	if (SDL_MUSTLOCK(screen_)) {
		if (SDL_LockSurface(screen_) < 0)
			logmsg(lt_error, "Unable to lock SDL surface");
	}
	tmp.attach(screen_size.x, screen_size.y, (int32 *)screen_->pixels);
	// draw logo as background
	SDL_Rect rect; 
	rect.x = rect.y = 0; 
	rect.w = screen_size.x;
	rect.h = screen_size.y;
	SDL_FillRect(screen_, &rect, PIXEL(0,0,0));
	logo->draw_copy( &tmp, (tmp.get_xsize()-logo->get_xsize())/2, (tmp.get_ysize()-logo->get_ysize())/2 );

	// size to print messages in
	int screen_height = screen_size.y-2;
	// offset from bottom of screen_
	int y_offset = 40;
	// height of a single line
	//FIXME: get from font_instance->get_text_geom
	int font_height = 13;
	for(unsigned int i=0; i<Nil_console::LINENUM; i++) {
		// display only if font is ok, msg contains text, and time is not over
		if((graphic_msg.is_font()) && (strlen(msg_text[i])>0)) {
			// draw background
			int32 msgbackcol = PIXEL(100, 100, 100);
			graphic_msg.setgeom (10+1, screen_height-y_offset-i*font_height+1, msg_text[i], GETRED(msgbackcol), GETGREEN(msgbackcol), GETBLUE(msgbackcol) );
			graphic_msg.render();
			graphic_msg.draw(&tmp);
			// draw message
			graphic_msg.setgeom (10, screen_height-y_offset-i*font_height, msg_text[i], GETRED(msg_color[i]), GETGREEN(msg_color[i]), GETBLUE(msg_color[i]) );
			graphic_msg.render();
			graphic_msg.draw(&tmp);
		} // font there?

		// is the next line still visible? -> if yes abort
		if((screen_height - y_offset-((signed)i+1)*font_height) < -font_height) {
			i = LINENUM;
		}
	}
	if ( SDL_MUSTLOCK(screen_) ) {
		SDL_UnlockSurface(screen_);
	}
	// Replacing SDL_UpdateRect with SDL_Flip so that double buffering works
	// - Phil Howlett 8th Sept 2004
	//
	//SDL_UpdateRect(screen_, 0,0, mScreenSize.x, mScreenSize.y);
	SDL_Flip(screen_);
}

void Nil_console::_put_output(Console::Severity severity, const char* source, const char* output) throw() {
	char buffer[LINELENGTH];
	int32 consolecolor;

	switch(severity) {
		case Console::DEBUG:		consolecolor = PIXEL(  0, 255,   0);	break;
		case Console::MESSAGE:		consolecolor = PIXEL(255, 255, 255);	break;
		case Console::WARNING:		consolecolor = PIXEL(255, 255,   0);	break;
		case Console::ERROR:		consolecolor = PIXEL(255,   0,   0);	break;
		case Console::FATAL:		consolecolor = PIXEL(128,   0,   0);	break;
		case Console::FUBAR:		consolecolor = PIXEL(255,   0, 255);	break;
		case Console::ASSERTION:	consolecolor = PIXEL(  0,   0, 255);	break;
		default:			consolecolor = PIXEL(  0, 255, 255);	break;
	}

	do {
		char* sep;
		if(output == NULL || *output == '\0')
			break;

		strlcpy(buffer, output, LINELENGTH);
		sep = strchr(buffer, '\n');
		if(sep != NULL)
			*sep = '\0';
		output = strchr(output, '\n');
		if(output != NULL)
			output++;

		this->add_message(buffer, consolecolor);
	} while(1);

	//Redraw if needed.
	if (get_active_console())
		this->draw(NULL);
}

void Nil_console::show_console(bool show) {
	if (show)
		global_console = this;
	else
		global_console = NULL;
}

bool Nil_console::is_active() {
	return (this == global_console);
}

Nil_console* get_active_console() {
	return global_console;
}
