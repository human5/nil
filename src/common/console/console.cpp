/***************************************************************************
  console.cpp -  Implementation.
  --------------------------------------------------------------------------
  begin                : 2004-10-09
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Rename put_output to put_output2. Move iostreams
                         interface from logmsg.cpp

  copyright            : (C) 2004 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/
#include "console.h"
#include <cstdarg>
#include <cctype>
#include <iostream>
#include <cstring>

Console master_console;

#define BUFFERSIZE 16384

unsigned char hex_table[] = "0123456789ABCDEF";

static int hexval(unsigned char c)
{
	switch(c)
	{
		case '0':			return 0;
		case '1':			return 1;
		case '2':			return 2;
		case '3':			return 3;
		case '4':			return 4;
		case '5':			return 5;
		case '6':			return 6;
		case '7':			return 7;
		case '8':			return 8;
		case '9':			return 9;
		case 'A':	case 'a':	return 10;
		case 'B':	case 'b':	return 11;
		case 'C':	case 'c':	return 12;
		case 'D':	case 'd':	return 13;
		case 'E':	case 'e':	return 14;
		case 'F':	case 'f':	return 15;
		default:			return -1;
	}
}

static void _default_quit_function(void* arg)
{
	master_console.put_output(Console::FATAL, "Exiting due to unrecoverable error...");
	abort();
}

void Console::notify_slave(Console* newmaster) throw()
{
	//Do nothing if master won't change.
	if(master == newmaster)
		return;

	//Check if we are already slave of something.
	if(newmaster != NULL && master != NULL)
		throw Already_slave();

	//Don't let slaves to loop. No need to trace our master as we know that we have no master, or are disconnecting
	//from master.
	if(newmaster && newmaster->trace_master() == this)
		throw Slave_loop();

	master = newmaster;
}

Console::Console() throw()
{
	//Defaults in set are OK.
	master = NULL;
	quit_function = _default_quit_function;
	quit_data = NULL;
	chattiness = Console::DEBUG;

	strcpy(msgsource, "<unknown>");
}

Console::~Console() throw()
{
	//Remove ourselves.
	if(master)
		master->remove_slave(*this);

	//Notify our slaves that they no longer have a master.
	for(std::set<Console*>::iterator i = slaves.begin(); i != slaves.end(); ++i)
		(*i)->notify_slave(NULL);

	//The set will go out with the object.
}

int Console::_get_command(char* buffer, size_t buffersize) throw()
{
	//No command.
	return 0;
}

void Console::_put_output(Severity severity, const char* source, const char* output) throw()
{
	//Ignore output.
}

//! Add new slave.
void Console::add_slave(Console& slave) throw()
{
	//Do we already have it?
	if(slaves.count(&slave) > 0)
		return;

	//First, add it to our lists.
	slaves.insert(&slave);

	//Second, try to notify it is our new slave.
	try {
		slave.notify_slave(this);
	} catch(Already_slave& e) {
		slaves.erase(&slave);
	}
}

void Console::remove_slave(Console& slave) throw()
{
	//Do we have such slave?
	if(slaves.count(&slave) == 0)
		throw Not_slave();

	//Remove it from our lists and notify it is no longer a slave.
	slaves.erase(&slave);
	slave.notify_slave(NULL);
}

int Console::get_command(char* buffer, size_t buffersize) throw()
{
	int x;
	char tmp[BUFFERSIZE];
	x = get_command_slave(tmp, BUFFERSIZE);
	unescape_string(buffer, buffersize, tmp);
	return x;
}

int Console::get_command_slave(char* buffer, size_t buffersize) throw()
{
	//See if we have command.
	if(_get_command(buffer, buffersize))
		return 1;

	//Nope, check slaves.
	for(std::set<Console*>::iterator i = slaves.begin(); i != slaves.end(); ++i)
		if((*i)->get_command_slave(buffer, buffersize))
			return 1;

	//Nope, no input.
	return 0;
}


void Console::put_output2(Severity severity, const char* format, va_list vargs) throw()
{
	char buffer[BUFFERSIZE];
	char buffer2[BUFFERSIZE];

	if(severity < chattiness)
		return;

	vsnprintf(buffer, BUFFERSIZE, format, vargs);

	escape_string(buffer2, BUFFERSIZE - 1, buffer, true);

	//Write trailing '\n' to buffer.
	size_t bufferlen = strlen(buffer2);
	buffer2[bufferlen + 0] = '\n'; 
	buffer2[bufferlen + 1] = '\0';

	//Output it. Technically, put_output_slave is not supposed to be used this way,
	//but this use can't be distingunished from its master calling it.
	put_output_slave(severity, msgsource, buffer2);

	//Quit on assertion.
	if(severity == ASSERTION || severity == FUBAR)
		quit_function(quit_data);
}

void Console::put_output(Severity severity, const char* format, ...) throw()
{
	//Forward it.
	va_list args;
	va_start(args, format);
	put_output2(severity, format, args);
	va_end(args);
}


void Console::put_output_slave(Severity severity, const char* source, const char* output) throw()
{
	_put_output(severity, source, output);
	for(std::set<Console*>::iterator i = slaves.begin(); i != slaves.end(); ++i)
		(*i)->put_output_slave(severity, source, output);
}

//! Traceback ultimate master.
Console* Console::trace_master() throw()
{
	Console* ultimate_master = this;

	while(ultimate_master)
		ultimate_master = ultimate_master->master;

	return ultimate_master;
}

void Console::set_quit(void (*function)(void* arg), void* data)
{
	quit_function = function;
	quit_data = data;
}

void escape_string(char* out, size_t outsize, char* in, bool allow_safe)
{
	unsigned char* _in = (unsigned char*)in;
	unsigned char* _out = (unsigned char*)out;

	unsigned char toescape = 0;
	unsigned char toemit = 0;
	bool first_hex = false;

	while(outsize >= 1 && (*_in || toescape != 0))
	{
		unsigned char input;
		if(toescape != 0)
		{
			if(first_hex)
			{
				first_hex = false;
				toemit = hex_table[(toescape >> 4) & 0xF];
				goto emit;
			}
			else
			{
				toemit = hex_table[toescape & 0xF];
				toescape = 0;
				goto emit;
			}
		}

		input = *(_in++);

		if((input & 0x60) != 0 && input != '\\')
		{
			//Direct copy.
			toemit = input;
			goto emit;
		}
		else
		{			
			if(!allow_safe)
				goto unsafe;

			if(input == '\n' || input == '\t')
			{
				//Direct copy.
				toemit = input;
				goto emit;
			}
			//Unsafes, INCLUDING \.
unsafe:
			toescape = input;
			first_hex = true;
			toemit = '\\';
			goto emit;
		}

emit:
		*(_out++) = toemit;
		outsize--;
	}

	*_out = '\0';
}

void unescape_string(char* out, size_t outsize, char* in)
{
	unsigned char* _in = (unsigned char*)in;
	unsigned char* _out = (unsigned char*)out;
	unsigned toemit;

	while(outsize >= 1 && *_in)
	{
		unsigned char input = *(_in++);

		if(input != '\\')
		{
			//Not escaped.
			toemit = input;
			goto emit;
		}
		else
		{
			//Escaped.
			if(isxdigit(_in[1]) && isxdigit(_in[2]))
			{
				//Hex escape.
				toemit = hexval(_in[1]) * 16 + hexval(_in[2]);
			}
			else
			{
				master_console.put_output(Console::WARNING, "Bad escape \%c%c.", _in[1], _in[2]);
				toemit = 0;
			}
			_in++;
			_in++;
			goto emit;

		}
emit:
		if(toemit)
		{
			*(_out++) = toemit;
			outsize--;
		}
	}

	*_out = '\0';
		
}

//! Set source text.
void Console::set_source(const char* source)
{
	strncpy(msgsource, source, MSGSOURCELEN);
}

void Console::set_chattiness(Console::Severity minimum)
{
	if(minimum >= Console::DEBUG && minimum <= Console::FATAL)
		chattiness = minimum;
}

#define BUFFERLENGTH 2048

//! @author H. Ilari Liusvaara
//! @brief The logger implemented in streamio
class logger : public std::streambuf {
public:
	//! ???
	logger(Console::Severity severity);
	//! ???
	virtual std::streamsize xsputn(const char* s, std::streamsize n);
	//! ???
	virtual int overflow(int c = traits_type::eof());
	//! ???
	virtual int sync();
private:
	//! ???
	Console::Severity severity;
	//! ???
	char buffer[BUFFERLENGTH];
	//! ???
	size_t bufferlength;
};

logger::logger(Console::Severity severity)
{
	this->severity = severity;
	this->bufferlength = 0;
}

int logger::overflow(int c)
{
	char dummy = c;
	this->xsputn(&dummy, 1);
	return c;
}

int logger::sync()
{
	this->buffer[this->bufferlength] = '\0';
	master_console.put_output(this->severity, "%s", this->buffer);
	this->bufferlength = 0;
	//FIXME: is this a good value to return?
	return 1;
}

std::streamsize logger::xsputn(const char* s, std::streamsize n)
{
	std::streamsize orig_n = n;

	//Do character to character copy to buffer.
	while(n) {
		if(*s == '\n') {
			//Flush it.
			this->sync();
		} else {
			//Add char.
			if(this->bufferlength < BUFFERLENGTH - 1)
				this->buffer[this->bufferlength++] = *s;
		}

		s++;
		n--;
	}
	return orig_n;
}

logger stfatal(Console::FATAL), sterror(Console::ERROR), stwarning(Console::WARNING), stmessage(Console::MESSAGE), 
	stdebug(Console::DEBUG), stassert(Console::ASSERTION), stfubar(Console::FUBAR);
std::ostream cfatal(&stfatal), cerror(&sterror), cwarning(&stwarning), cmessage(&stmessage), cdebug(&stdebug), cassert(&stassert),
	cfubar(&stfubar);
