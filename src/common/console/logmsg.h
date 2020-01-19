/***************************************************************************
  logmsg.h  -  Message logger.
  --------------------------------------------------------------------------
  begin                : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Move [gs]etShowConsole to nil_console.h. Remove
                         setmsgsource and setchattiness. Move c++ iostreams
                         interface to console.h

  copyright            : (C) 2004 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/

#ifndef LOGMSG_H
#define LOGMSG_H

#include <iostream>

enum Logtype {
	lt_fatal   = 0,
	lt_error   = 1,
	lt_warning = 2,
	lt_message = 3,
	lt_debug   = 4,
	lt_assert  = 5
};

/*! ???
    @param logtype ???
    @param format ??? */
void logmsg(Logtype logtype, const char* format, ...);

extern std::ostream cfatal, cerror, cwarning, cmessage, cdebug, cassert;

#endif
