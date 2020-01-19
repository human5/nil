/***************************************************************************
  nil_console.h  -  Header for display of nil system messages
 ---------------------------------------------------------------------------
  begin                : 2003-06-17
  by                   : Nils Thuerey (n_t@gmx.de)

  last changed         : 2004-03-30
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Don't bother with the whole file without SDL.

  last changed         : 2004-10-24
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Declare destructor to throw no exceptions.

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Move getShowConsole and setShowConsole from logmsg.h

  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Move again, with huge API cleanup

  copyright            : (C) 2003 by Nils Thuerey (n_t@gmx.de)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#ifndef _NIL_CONSOLE_H_
#define _NIL_CONSOLE_H_

#ifndef WITHOUT_SDL

#include "client/graphics/graphic.h"
#include "client/graphics/graphic_msg.h"
#include "common/console/logmsg.h"
#include "common/console/console.h"
#include "common/configuration/configuration.h"
#include "common/misc.h"
#include "common/systemheaders.h"

/*! \author Nils Thuerey
    \brief simple object for sending messages */
class Nil_console : public Console {
public:
	/*! Constructor that initializes pointers and deletes all messages
	    \param screen_ Handle to a surface we can draw the console to
	    \param screen_size_ Dimension of the console
	    \param logo_ The logo that we draw to the background of the console
	    \param font_ The console font */
	Nil_console(SDL_Surface *screen_, Config_xy screen_size_, Graphic *logo_, Font_instance *font_);
	//! Destructor (empty)
	~Nil_console() throw();
	/*! add a new message line
	    @param message The message that will be added as new line
	    @param color The color of the message */
	void add_message(char *message, int32 color);
	/*! display the console
	    @param screen The surface we will draw to (if NULL, the stored surface is used) */
	void draw(SDL_Surface *screen);
	//! determine if the console will be shown or not (also announced as global console)
	void show_console(bool show);
	//! find out if the current console is active
	bool is_active();

protected:
	//! Output method for console. (wrapper around add_message
	void _put_output(Console::Severity severity, const char* source, const char* output) throw();

	//! number of lines to remember
	static const unsigned int LINENUM = 64;
	//! length of a line
	static const unsigned int LINELENGTH = 256;
	//! color of each msg 
	int32 msg_color[LINENUM];       
	//! text of all messages
	char  msg_text[LINENUM][LINELENGTH];
	//! pointer to the screen surface
	SDL_Surface *screen;
	//! console size
	Config_xy screen_size;
	//! The graphics logo to display (centered of console behind text)
	Graphic * logo;
	//! a graphics message to display the text
	Graphic_msg graphic_msg;
};

Nil_console * get_active_console();

#endif
#endif
