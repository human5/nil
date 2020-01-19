/***************************************************************************
  exception.h  -  Exception class defintions
  --------------------------------------------------------------------------
  begin                : 2004-02-07
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-07-17
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Enhance exception class by adding possiblity
                         to strdup contexts/messages.

  copyright            : (C) 2004 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/

#ifndef EXCEPTION_H
#define EXCEPTION_H

//! @author H. Ilari Liusvaara
//! @brief Class for exception definitions
class nil_exception {
public:
	//! Constructor
	nil_exception(const char* context, const char* message, bool strdup_ctx = false, bool strdup_msg = false);
	//! Destructor
	~nil_exception();
	//! Copy constructor.
	nil_exception(const nil_exception& e);
	//! returns the error-content
	const char* get_errortcontext();
	//! returns the error-message
	const char* get_errormessage();
private:
	//! Forbid assignment
	nil_exception& operator= (const nil_exception& e);
	//! the error content
	const char* errorcontext;
	//! the error message
	const char* errormessage;
	//! ???
	mutable bool copied;
	//! ???
	mutable bool iscopy;
	//! ???
	bool ctx_strdupped;
	//! ???
	bool msg_strdupped;
};

//! out of memory
void throw_nil_exception_OOM(const char* context);
//! default system error
void throw_nil_exception_syserror(const char* context);

#endif
