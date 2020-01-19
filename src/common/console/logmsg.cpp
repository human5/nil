/***************************************************************************
  logmsg.cpp  -  Message logger.
  --------------------------------------------------------------------------
  begin                : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2004-10-24
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Wire this to master console code.

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Move [gs]etShowConsole to nil_console.cpp. Remove
                         setmsgsource and setchattiness. Move iostreams 
                         interface to console.cpp
 
  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : header fix and whitespace cleanup

  copyright            : (C) 2004 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "console.h"
#include "logmsg.h"
#include "client/console/nil_console.h"
#include <csignal>
#include <cstring>

bool logmsg_recursing_in_draw = false;

#define BUFFERLENGTH 2048

void logmsg(Logtype logtype, const char* format, ...) {
	Console::Severity severity;
  
	//Don't recurse.
	if(logmsg_recursing_in_draw)
		return;
	logmsg_recursing_in_draw = true;
  
	switch(logtype) {
		case lt_fatal:		severity = Console::FATAL;	break;
		case lt_error:		severity = Console::ERROR;	break;
		case lt_warning:	severity = Console::WARNING;	break;	
		case lt_message:	severity = Console::MESSAGE;	break;
		case lt_debug:		severity = Console::DEBUG;	break;
		default:		severity = Console::MESSAGE;	break;
	}
  
	va_list args;
	va_start(args, format);
	master_console.put_output2(severity, format, args);
	va_end(args);
  
	logmsg_recursing_in_draw = false;
}
