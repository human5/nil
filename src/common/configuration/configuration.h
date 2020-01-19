/***************************************************************************
  configuration.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-01
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-05-16
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  last changed         : 2005-01-03
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : directory move and #inlcude cleanup

  last changed         : 2005-01-14
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Clean up so it compiles after vfs_pathexpander.h
                         changes.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "common/systemheaders.h"
#include "common/console/logmsg.h"
#include "common/vfs/vfs_pathexpander.h"
#include "common/vfs/loader.h"

//! return values in format "123x456"
typedef struct Config_xy {
	//! x coordinate
	int x;
	//! y coordinate
	int y;
} TD_config_xy;

class Configuration_observer;
class Config_values;
class Config_observers;

//! @author Flemming Frandsen
//! @brief This class is a place to store the configuration and value of an option
class Config_value {
public:
	/*! Constructor that creates a new config value with the given description
	    @param description_ The description */
	Config_value(const char *description_);
	//! Destructor that free's the despcription
	~Config_value();
	//! ???
	const char* get_description();
	//! ???
	const char* get_value();
	//! ???
	void set_value(const char *newvalue);
	//! ???
	void clear_value();
	/*! if true, then setting this value means appending it to the old value, separated by
	    a tab-character. */
	bool append_value;
	//! true if this is an integer
	bool integer_value;
	//! minimum for both strings and ints.
	int min;
	//! minimum for both strings and ints.
	int max;
protected:
	//! the configuration-option's description
	char *description;
	//! the configuration-option's value
	std::string value;
};

//! @author Flemming Frandsen
//! @brief This class is for handling the configuration
class Configuration {
public:
	//! Constructor that sets up all the options that we need to understand
	Configuration();
	//! Destructor for configuartion
	~Configuration();
	//! function to fill up the object from commandline
	bool parse_cmdline(int argc,char **argv);
	//! function to fill up the object from file
	bool parse_file(char *file_contents);
	//! function to load a file
	bool load_file(const char *filename);
	//! parser to check if options are given in correct format and if an option exists
	bool parse_option(const char *line);
	//! resets an option
	bool clear_option(const char *name);
	//! sets an option
	bool set_option(const char *name, const char *value);
	//! gets an option
	const char *get_option(const char *name);
	//! gets an option and returns as integer
	int get_option_int(const char *name);
	//! gets an option and returns as "123x456" formated string
	Config_xy get_option_xy(const char *name);
	//! error reporting handling
	char *get_error() { return error; }
	//! lists all the options
	void print_manual(FILE *target);
	//! checks the highlevel constraints
	bool check_options();
	//! save our options to .nilrc
	bool save_options(char *filename);
	//! list our options with their values from a file
	void print_options(FILE *target);
	//! observer/subject attacher
	void attach(Configuration_observer *observer);
	//! observer/subject detacher
	void detach(Configuration_observer *observer);
protected:
	//! ???
	Config_observers *observers;
	//! called internally, notifes observers
	void notify_observers();
	//! a value
	Config_values *values;
	//! adds an option
	void add_option(const char *name, const char *description, const char *default_value, int min=0, int max=0, bool integer_value=false, bool append_value=false);
	//! place to store an error string
	char error[200];
};

//! @author Flemming Frandsen
//! @brief inherit from this if you want to be notified when the config changes:
class Configuration_observer {
public:
	//! Constructor
	Configuration_observer();
	//! Destructor
	virtual ~Configuration_observer();
	//! attach a config
	void config_attach(Configuration *config);
	//! impelement this to get notified by the configuration when it changes
	virtual void config_changed()=0;
protected:
	//! detach a config
	void detatch();
	//! This is the configuration that we are watcing:
	Configuration *configuration;
};

#endif
