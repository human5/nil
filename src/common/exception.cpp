/***************************************************************************
  exception.cpp  -  Exception class defintions.
  --------------------------------------------------------------------------
  begin                : 2004-02-07
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-07-15
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Add support for strdupping exception contexts and
                         messages.

  copyright            : (C) 2004 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/

#include "exception.h"
#include <string>
#include <cstring>
#include <cerrno>

nil_exception::nil_exception(const char* context, const char* message, bool strdup_ctx, bool strdup_msg) {

	//Rather mark context and message as strdupped.
	ctx_strdupped = false;
	msg_strdupped = false;

	if(!strdup_ctx) {
		errorcontext = context;
	} else {
		errorcontext = strdup(context);
		if(!errorcontext)
			errorcontext = "<insufficent memory to report context>";
		else
			ctx_strdupped = true;
	}
	if(!strdup_msg) {
		errormessage = message;
	} else {
		errormessage = strdup(message);
		if(!errormessage)
			errormessage = "<insufficent memory to report message>";
		else
			msg_strdupped = true;
	}
	
	copied = false;
	iscopy = false;
}

nil_exception::~nil_exception() {

	//Don't free resources in objects in originals having a copy.
	if(copied && !iscopy)
		return;

	if(ctx_strdupped)
		free((void*)errorcontext);

	if(msg_strdupped)
		free((void*)errormessage);
}

nil_exception::nil_exception(const nil_exception& e)
{
	//fprintf(stderr, "Copy constructor for nil_exception called.\n");

	/*
	if(iscopy || copied) {
		//BAD! Force a segfault.
		*(int*)NULL = 0;
		throw nil_exception("nil_exception::<copy constructor>", "Trying to make more than one copy of object");
	}
	*/

	ctx_strdupped = e.ctx_strdupped;
	msg_strdupped = e.msg_strdupped;
	errorcontext = e.errorcontext;
	errormessage = e.errormessage;
	e.copied = true;
	iscopy = true;
}


const char* nil_exception::get_errortcontext() {
	return errorcontext;
}

const char* nil_exception::get_errormessage() {
	return errormessage;
}

void throw_nil_exception_OOM(const char* context) {
	throw nil_exception(context, "Out of memory");
}

void throw_nil_exception_syserror(const char* context) {
	throw nil_exception(context, strerror(errno)); 
}
