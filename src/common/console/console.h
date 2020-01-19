/***************************************************************************
 console.h  -  The base for consoles
 ---------------------------------------------------------------------------
  begin                : 2004-10-25
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Rename put_output to put_output2. Move iostreams
                         interface from logmsg.h

  copyright            : (C) 2004 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/

#ifndef _CONSOLE__H__INCLUDED__
#define _CONSOLE__H__INCLUDED__

#include <set>
#include <cstdarg>
#include <ostream>

//! Thrown when console to be slaved is already a slave.
class Already_slave {};

//! Thrown when console that is expected to be slave is not a slave.
class Not_slave {};

//! Thrown when slaveness relations would from a loop after adding slave.
class Slave_loop {};

#define MSGSOURCELEN 128

//! @author H. Ilari Liusvaara
//! @brief The base class for consoles
class Console {
public:
	//! The severity that a console supports
	enum Severity
	{
		DEBUG     = 0,
		MESSAGE   = 1,
		WARNING   = 2,
		ERROR     = 3,
		FATAL     = 4,
		FUBAR     = 5,
		ASSERTION = 6
	};
	//! Constructor.
	Console() throw();
	//! Destructor.
	virtual ~Console() throw();
	/*! Add new slave.
	    @param slave The slave-console that should be attached */
	void add_slave(Console& slave) throw();
	/*! Remove slave.
	    @param slave The slave-console that should be removed */
	void remove_slave(Console& slave) throw();
	/*! Get command.
	    @param buffer A string buffer that stores the command
	    FIXME: Is it possible to autodetect this?
	    @param buffersize The size of the buffer
	    @return Return 1 if command was filled, 0 if no command. */
	int get_command(char* buffer, size_t buffersize) throw();
	/*! Put text.
	    @param severity The severity of the output
	    @param format The format of the output */
	void put_output(Severity severity, const char* format, ...) throw();
	/*! Put text.
	    @param severity The severity of the output
	    @param format The format of the output
	    @param vargs ??? */
	void put_output2(Severity severity, const char* format, va_list vargs) throw();
	/*! Set quit function/data. Remember: Quit function of the console that
	    injected the messag runs.
	    @param data ???
	    @param function ??? */
	void set_quit(void (*function)(void* arg), void* data);
	/*! Set source text.
	    @param source The message source */
	void set_source(const char* source);
	/*! Set chattiness.
	    @param minimum The minimum severity of the shown messages */
	void set_chattiness(Console::Severity minimum);
private:
	/*! Propagate text in slave. No formatting, and no quit on assert/fubar.
	    @param severity The severity of this message 
	    @param source The source of this message
	    @param text The message itself */
	void put_output_slave(Severity severity, const char* source, const char* text) throw();
	/*! Get command in slave. Don't perform string unescaping.
	    @param buffer The buffer storing the possible command message
	    @param buffersize The size of the buffer
	    @return Return 1 if command was filled, 0 if no command. */
	int get_command_slave(char* buffer, size_t buffersize) throw();
	/*! Notify about new slave relationship.
	    @param newmaster The new master for the slave */
	void notify_slave(Console* newmaster) throw();
	/*! Low-level get command routine.
	    @param buffer The buffer storing the possible command message
	    @param buffersize The size of the buffer
	    @return Return 1 if command was filled, 0 if no command. */
	virtual int _get_command(char* buffer, size_t buffersize) throw();
	/*! Low-level put output routine. DO NOT handle quit-on assertion here!
	    @param severity The severity of the message
	    @param source The source of the message
	    @param output ??? */
	virtual void _put_output(Severity severity, const char* source, const char* output) throw();
	//! Traceback ultimate master.
	Console* trace_master() throw();
	//! A list of slaves
	std::set<Console*> slaves;
	//! Handle to the master of this console
	Console* master;
	//! For quitting.
	void (*quit_function)(void* arg);
	//! ???
	void* quit_data;
	//! Chattiness.
	Console::Severity chattiness;
	//! Source.
	char msgsource[MSGSOURCELEN];
};

//! Master console.
extern Console master_console;

/*! Escape string control characters. If allow_safe is TRUE, characters HT,
    LF and ESC[...m are allowed.
    @param out ???
    @param outsize ???
    @param in ???
    @param allow_safe ??? */
void escape_string(char* out, size_t outsize, char* in, bool allow_safe = false);

/*! Unescape string.
    @param out ???
    @param outsize ???
    @param in ??? */
void unescape_string(char* out, size_t outsize, char* in);

//C++ cout-style interface to logger.
extern std::ostream cfatal, cerror, cwarning, cmessage, cdebug, cassert, cfubar;


#endif
