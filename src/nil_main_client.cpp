/***************************************************************************
  nil_main.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-11-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-28
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  last changed         : 2004-09-22
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Use load_files_standard().

  last changed         : 2004-10-23
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Make simple to stdout console and wire it to master.

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Use Console::set_source instead of setmsgsource.
                         Use Console::set_chattiness instead of setchattiness.
                         Use Console::put_output instead of fprintf.

  last changed         : 2004-01-14
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Fix it so it compiles after vfs_pathexpander.h
                         changes.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "client/graphics/raw_surface.h"
#include "client/graphics/view_rgbf.h"
#include "client/nil_client.h"
#include "client/bot.h"
#include "common/console/logmsg.h"
#include "common/console/console.h"
#include "common/configuration/configuration.h"
#include "common/misc.h"
#include "common/systemheaders.h"
#include "common/vfs/loader.h"
#include "server/nil_server.h"
#include <csignal>
#include <vector>

#ifdef _WIN32
int win32_main(int argc, char *argv[]);
#define NILRC "nil.cfg"
#else
#define NILRC "~/.nilrc"
#endif

//! @author H. Ilari Liusvaara
//! @brief Simple print-to-stdout console.
class Console_stdout : public Console
{
	/*! ???
	    @param severity ???
	    @param source ???
	    @param output ??? */
	void _put_output(Console::Severity severity, const char* source, const char* output) throw() {
	//No need to mask control characters as Console::put_output does it automatically.
#ifdef DONT_USE_COLORS
		const char* severities[] = {"Debug", "Message", "Warning", "Error", "Fatal", "Everything's fucked up", 
			"Assertion violation" };
		printf("%s: %s: %s", source, severities[severity], output);
#else
		const char* severities[] = {"\033[1;32m", "\033[1;37m", "\033[1;33m", "\033[1;31m", "\033[0;31m", "\033[1;35m",
			"\033[1;34m" };
		printf("%s%s: %s\033[0m", severities[severity], source, output);
#endif
	}
};

#define NILRC_LAST NILRC ".last_good"

static void signal_handler(int sig)
{
	raise(SIGSEGV);
}

//! the "real" nil main function
int nil_main(int argc,char **argv) {
	// Wire our stdout console to master.
	Console_stdout stdout_console;
	master_console.add_slave(stdout_console);

	signal(SIGABRT, signal_handler);

	// The string that appears before the log messages
	master_console.set_source("NiL");

	// check a few things:
	if (strcmp(OBJECT_NAMES[ot_error],"ERROR")) {
		// error object not last in array -> needed for return values?
		logmsg(lt_fatal,"You have fucked up, OBJECT_NAMES[ot_error] was not ERROR, it was: \"%s\" take a look at fluff.h",OBJECT_NAMES[ot_error]);
		return 1;
	}

	master_console.put_output(Console::MESSAGE, "NiL - Worms, Violence and the GPL\n\n");
	Configuration config;
	
	if (!config.load_file(NILRC)) {
		master_console.put_output(Console::WARNING, "Unable to read " NILRC ":\n%s\n",config.get_error());
	}

	if (!config.parse_cmdline(argc,argv)) {
		master_console.put_output(Console::FATAL, "Unable to understand commandline: %s\n",config.get_error());
		master_console.put_output(Console::FATAL, "\nUsage: nil [<option>=<value>]\n");
		master_console.put_output(Console::FATAL, "Options can be given in " NILRC " configuration file or\n");
		master_console.put_output(Console::FATAL, "overridden from the command line ('nil mode=server').\n\n");
		master_console.put_output(Console::FATAL, "The options I understand (on commandline or in ~/.nilrc) are:\n");
		config.print_manual(stdout);
		return 1;
	}

	if (!config.check_options()) {
		master_console.put_output(Console::FATAL, "A value failed the secondary check:\n%s\n",config.get_error());
		return 2;
	}

	config.save_options(NILRC_LAST);

	const char* chattiness = config.get_option("chattiness");
	Console::Severity chattiness_numeric = Console::MESSAGE;
	if(!strcasecmp(chattiness, "debug") || !strcasecmp(chattiness, "4"))
		chattiness_numeric = Console::DEBUG;
	else if(!strcasecmp(chattiness, "message") || !strcasecmp(chattiness, "3"))
		chattiness_numeric = Console::MESSAGE;
	else if(!strcasecmp(chattiness, "warning") || !strcasecmp(chattiness, "2"))
		chattiness_numeric = Console::WARNING;
	else if(!strcasecmp(chattiness, "error") || !strcasecmp(chattiness, "1"))
		chattiness_numeric = Console::ERROR;
	else if(!strcasecmp(chattiness, "fatal") || !strcasecmp(chattiness, "0"))
		chattiness_numeric = Console::FATAL;

	master_console.set_chattiness(chattiness_numeric);

	const char *mode = config.get_option("mode");

	run_client(config);
	return 0;
}

#ifdef _WIN32
//! main function to determine what to do...
extern "C" int SDL_main(int argc, char *argv[]) {
	// prepare win32 stuff (also calls nil_main)...
	return win32_main(argc, argv);
}
#else
//! main function to determine what to do...
int main(int argc, char **argv) {
	// unix, simply start
	return nil_main(argc, argv);
}
#endif
