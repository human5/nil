/***************************************************************************
  configuration.cpp  -  a place to store the configuration and value of an option
 ---------------------------------------------------------------------------
  begin                : 2000-02-01
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-05-16
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-08-03
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Add includes to include loader_pathexpander for
                         expand_path.

  last changed         : 2004-09-03
  by                   : Phil Howlett
  email                : phowlett@bigpond.net.au
  changedescription    : Fixed typo in paths for Win32

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Update to include new chattiness options.
  
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

#include "common/configuration/configuration.h"
#include "common/misc.h"
#include <map>
#include <string>
#include <set>

Config_value::Config_value(char *description_) {
	description = strdup(description_);
	value = "";
	append_value = false;
	integer_value = false;
	min = 0;
	max = 0;
}

Config_value::~Config_value() {
	free(description);
}

const char* Config_value::get_description() {
	return description;
}

const char* Config_value::get_value() {
	return value.c_str();
}

void Config_value::clear_value() {
	value = "";
}

void Config_value::set_value(char *newvalue) {
	if (append_value) {
		if (value.size() > 0)
			value += "\t";
		value += newvalue;
	} else {
		value = newvalue;
	}
}

//! A list of config values
class Config_values : public std::map<std::string, Config_value *>{};

Configuration_observer::Configuration_observer() {
	configuration = NULL;
}

Configuration_observer::~Configuration_observer() {
	detatch();
}

void Configuration_observer::detatch() {
	if (configuration) configuration->detach(this);
	configuration = NULL;
}

void Configuration_observer::config_attach(Configuration *config) {
	detatch();
	configuration = config;
	if (configuration) configuration->attach(this);
}

//! ???
class Config_observers : public std::set<Configuration_observer *>{};

void Configuration::attach(Configuration_observer *observer) {
	observers->insert(observer);	
}

void Configuration::detach(Configuration_observer *observer) {
	observers->erase(observer);	
}

void Configuration::notify_observers() {
  Config_observers::iterator i=observers->begin();
	while (i != observers->end()) {
		(*i)->config_changed();
		i++;
	}
}

Configuration::Configuration() {
	memset(error,0,sizeof(error));
	values = new Config_values;
	observers = new Config_observers;

	add_option(
		"randomseed",
		"The random seed passed to the map generator (server only) (default: 42)",
		"42"
	);

	add_option(
		"data",
#ifdef _WIN32
		"The filename of either the .lump file or the directory where data can be found (default: data)",
		"default.lump\t..\\data",
#else
		"The filename of either the .lump file or the directory where data can be found (default: ~/nil/data)",
		"default.lump\t../data\t~/nil/data\t/usr/share/games/nil/default.lump",
#endif
		0, 0 , false,
		true
	);

	add_option(
		"mode",
		"What to do: [server, client, clientserver, serverclient, bot, cleanup, savelump, viewfile] (default: clientserver)",
		"clientserver"
	);

	add_option(
		"game_mode",
		"What type of game to play: [deathmatch, teamplay] (default: deathmatch)",
		"deathmatch"
	);

	add_option(
		"server",
		"The address of the server (default: localhost)",
		"localhost"
	);

	add_option(
		"server_port",
		"The port to contact the server on or for the server to listen on (default: 0xBABE)",
		"0xbabe",
		1,0xffff,true
	);

	add_option(
		"map",
		"Map file name (server only) (default: -=random=-)",
		"-=random=-"
	);

	add_option(
		"world_size",
		"The size of the map in pixels.(server only) (default: 700x600)",
		"700x600"
	);

	add_option(
		"armageddon",
		"Make the world go down after this time (default: 0 (=off))",
		"0",
		0,100,true
	);

	add_option(
		"player_name",
		"The name presented to other players (default: A. Coward)",
		"A. Coward"
	);

	add_option(
		"player_color",
		"The color of your worm (default: 0x80A0B0)",
		"0x80a0b0",
		0,0xffffff,true
	);

	add_option(
		"screen_size",
		"The size in pixels of the game screen (default: 640x480)",
		"640x480"
	);

	add_option(
		"view_quality",
		"The number of game pixels pr screen pixel. 640x480 with quality 2 gives 320x240 in game resolution. (default: 2)",
		"2",
		1,5,true
	);

	add_option(
		"output_lump",
		"What file to save the loaded data to, this is used by developers to create the .lump files (default: default.lump)",
		"default.lump"
	);

	add_option(
		   "inputfile",
		   "File to read rgbf data from. Used by developers for debugging (default: gfx/logos/nil.rgbf)",
		   "gfx/logos/nil.rgbf"
	);

	add_option(
		"fullscreen",
		"Should the game run in fullscreen(1) or in a window (0) (default: 1)",
		"1",
		0,1,true
	);

	add_option(
		"coredump",
		"Should the game dump a core if something goes bad(1) or recover the system(0) (defaut: 0)",
		"0",
		0,1,true
	);

	add_option(
		"net_multithread",
		"Should the server run an extra thread for each client(1) or run the network in a single thread(0) (default: 1)",
		"1",
		0,1,true
	);

	add_option(
		"net_normal_backlog",
		"The number of updates a client can be behind and still be considered uptodate, set to 5 for internet games (default: 4)",
		"4",
		0,20,true
	);

	add_option(
		"net_abnormal_backlog",
		"When a client gets this far behind it is kicked (default: 20)",
		"20",
		5,50,true
	);

	add_option(
		"net_max_wait_time",
		"The maximum time in ms to wait for all clients to become uptodate (default: 200)",
		"200",
		50,500,true
	);

	add_option(
		"chattiness",
		"The number of messages that NiL surts out. [4/debug, 3/message, 2/warning, 1/error, 0/fatal] (default: debug)",
		"debug"
	);

	add_option(
		"local_players", 
		"The number of players on this computer (default: 1)", 
		"1", 
		1, 4, true
	);

	add_option(
		"team",
		"Team ID for this player, only used when team play is activated on the server without auto team assign (default: -1)",
		"-1",
		-1,3, true
	);

	// weapon configuration
	add_option("weapon_style",
		"How to assign weapons. [rand, randidentical, or fixed] (server only) (default: fixed)",
		"fixed"
	);

	char weaponName[100]; // init weapon name into temp buffer
	strncpy(weaponName, OBJECT_NAMES[ weaponinit_default[0] ], 100 );
	add_option("weapon1", "The first weapon (if weapons are fixed)", weaponName );
	strncpy(weaponName, OBJECT_NAMES[ weaponinit_default[1] ], 100 );
	add_option("weapon2", "The second weapon (if weapons are fixed)", weaponName );
	strncpy(weaponName, OBJECT_NAMES[ weaponinit_default[2] ], 100 );
	add_option("weapon3", "The third weapon (if weapons are fixed)", weaponName );
	strncpy(weaponName, OBJECT_NAMES[ weaponinit_default[3] ], 100 );
	add_option("weapon4", "The fourth weapon (if weapons are fixed)", weaponName );
	strncpy(weaponName, OBJECT_NAMES[ weaponinit_default[4] ], 100 );
	add_option("weapon5", "The fifth weapon (if weapons are fixed)", weaponName );

	add_option(
		"always_reload",
		"Whether to reload all of a player's weapons (if set to 1), or only the current one. (server only) (default: 1)",
		"1",
		0, 1, true);

	add_option(
		"sound",
		"Sound on (1) or off (0). (default: 1)",
		"1",
		0, 1, true);

	add_option(
		"coloredblood",
		"Sets the blood color to the one of the worm (1) or to red (0). (default: 1)",
		"1",
		0, 1, true);
	
	add_option(
		"benchmark",
		"Perform benchmark (1) before running the game or start immediately (0). (default: 0)",
		"0",
		0, 1, true);
	
	//general purpose keys:
	add_option("key_Toggle_console",      "The key bound to this function","-1");
	add_option("key_Screen_shoot",        "The key bound to this function","-1");
	add_option("key_Toggle_logo",         "The key bound to this function","-1");
	add_option("key_Toggle_deathcam",     "The key bound to this function","-1");
	add_option("key_Toggle_traffic",      "The key bound to this function","-1");
	add_option("key_Toggle_framerate",    "The key bound to this function","-1");
	add_option("key_Toggle_shadow",       "The key bound to this function","-1");
	add_option("key_Toggle_smooth",       "The key bound to this function","-1");
	add_option("key_Menu",                "The key bound to this function","-1");
	add_option("key_chat",                "The key bound to this function","-1");
	add_option("key_backspace",           "The key bound to this function","-1");
	add_option("key_fragtable",           "The key bound to this function","-1");

	//player 1 keys:
	add_option("key_Player1_up",          "The key bound to this function","-1");
	add_option("key_Player1_down",        "The key bound to this function","-1");
	add_option("key_Player1_left",        "The key bound to this function","-1");
	add_option("key_Player1_right",       "The key bound to this function","-1");
	add_option("key_Player1_fire",        "The key bound to this function","-1");
	add_option("key_Player1_jump",        "The key bound to this function","-1");
	add_option("key_Player1_dig",         "The key bound to this function","-1");
	add_option("key_Player1_hook_fire",   "The key bound to this function","-1");
	add_option("key_Player1_hook_release","The key bound to this function","-1");
	add_option("key_Player1_hook_in",     "The key bound to this function","-1");
	add_option("key_Player1_hook_out",    "The key bound to this function","-1");
	add_option("key_Player1_jetpack",     "The key bound to this function","-1");
	add_option("key_Player1_weapon1",     "The key bound to this function","-1");
	add_option("key_Player1_weapon2",     "The key bound to this function","-1");
	add_option("key_Player1_weapon3",     "The key bound to this function","-1");
	add_option("key_Player1_weapon4",     "The key bound to this function","-1");
	add_option("key_Player1_weapon5",     "The key bound to this function","-1");
	add_option("key_Player1_nextweapon",  "The key bound to this function","-1");
	add_option("key_Player1_prevweapon",  "The key bound to this function","-1");

	//player 2 keys
	add_option("key_Player2_up",          "The key bound to this function","-1"); 
	add_option("key_Player2_down",        "The key bound to this function","-1"); 
	add_option("key_Player2_left",        "The key bound to this function","-1"); 
	add_option("key_Player2_right",       "The key bound to this function","-1"); 
	add_option("key_Player2_fire",        "The key bound to this function","-1"); 
	add_option("key_Player2_jump",        "The key bound to this function","-1"); 
	add_option("key_Player2_dig",         "The key bound to this function","-1"); 
	add_option("key_Player2_hook_fire",   "The key bound to this function","-1");
	add_option("key_Player2_hook_release","The key bound to this function","-1");
	add_option("key_Player2_hook_in",     "The key bound to this function","-1"); 
	add_option("key_Player2_hook_out",    "The key bound to this function","-1"); 
	add_option("key_Player2_jetpack",     "The key bound to this function","-1");
	add_option("key_Player2_weapon1",     "The key bound to this function","-1"); 
	add_option("key_Player2_weapon2",     "The key bound to this function","-1"); 
	add_option("key_Player2_weapon3",     "The key bound to this function","-1"); 
	add_option("key_Player2_weapon4",     "The key bound to this function","-1"); 
	add_option("key_Player2_weapon5",     "The key bound to this function","-1"); 
	add_option("key_Player2_nextweapon",  "The key bound to this function","-1"); 
	add_option("key_Player2_prevweapon",  "The key bound to this function","-1"); 

	// Assign joysticks by default.
	// FIXME: doesn't allow for more than 255 joysticks attached
	//       to the computer at once
	add_option("Player1_joystick", "Which joystick player 1 should use to move (-1 for none)","0",-1,255,true);
	add_option("Player2_joystick", "Which joystick player 2 should use to move (-1 for none)","1",-1,255,true);
	add_option("Player3_joystick", "Which joystick player 3 should use to move (-1 for none)","2",-1,255,true);
	add_option("Player4_joystick", "Which joystick player 4 should use to move (-1 for none)","3",-1,255,true);

	//NT
	// amount of blood
	add_option(
		"gorelevel",
		"Amount of blood (default: 0)",
		"5",
		0, 10, true);
	add_option(
		"map_stuff",
		"How much stuff in the level? (server only) (default: 10)",
		"10",
		0, 100, true);
	// add_option(char *name, char *description, char *default_value, int min, int max, bool integer_value, bool append_value)
}

//-----------------------------------------------------------------------------
#define CHECK_MIN(value,min)\
  if (value < min) {sprintf(error,"%s is %i this is less than the miniumum %s (%i)",#value,value,#min,min); return false;}
#define CHECK_MAX(value,max)\
  if (value > max) {sprintf(error,"%s is %i this is more than the maximum %s (%i)",#value,value,#max,max); return false;}


//-----------------------------------------------------------------------------
//This does secondary checks on the options:
bool Configuration::check_options()
{
	Config_xy screen_size = get_option_xy("screen_size");
	CHECK_MIN(screen_size.x,320)
	CHECK_MAX(screen_size.x,1280)
	CHECK_MIN(screen_size.y,200)
	CHECK_MAX(screen_size.y,1024)

	// limit world size
	Config_xy world_size = get_option_xy("world_size");
	CHECK_MIN(world_size.x,100) CHECK_MAX(world_size.x,5000)
	CHECK_MIN(world_size.y,50 ) CHECK_MAX(world_size.y,5000)
	//CHECK_MIN(world_size.x,640) CHECK_MAX(world_size.x,5000)
	//CHECK_MIN(world_size.y,480) CHECK_MAX(world_size.y,5000)

	// check if window is too large for world, if yes resize
	int view_quality = get_option_int("view_quality");
	bool scrsChanged = false;
	if( (world_size.x * view_quality)< screen_size.x){ screen_size.x = (world_size.x * view_quality); scrsChanged=true; }
	if( (world_size.y * view_quality)< screen_size.y){ screen_size.y = (world_size.y * view_quality); scrsChanged=true; }
	if(scrsChanged) {
		logmsg(lt_warning,"Screen size changed - map too small...");
		char setscrs[100];
		snprintf(setscrs, 100, "%dx%d", screen_size.x, screen_size.y);
		// and change globally
		set_option("screen_size", setscrs);
	}

	// check nil mode
	const char *mode = get_option("mode");
	if( strcasecmp(mode,"client") &&	strcasecmp(mode,"server") &&
	    strcasecmp(mode,"clientserver") && strcasecmp(mode,"serverclient") &&
	    strcasecmp(mode,"cleanup") && strcasecmp(mode,"savelump") && strcasecmp(mode,"bot") &&
	    strcasecmp(mode,"viewfile") ) {
		sprintf(error,"The mode \"%s\" was not understood, valid modes are: server, client, clientserver,bot, serverclient, savelump, cleanup, and viewfile",mode);
		return false;
	}

	// check game mode
	const char *gamemode = get_option("game_mode");
	if( strcasecmp(gamemode,"deathmatch") &&
	    strcasecmp(gamemode,"teamplay") ) {
		sprintf(error,"The game mode \"%s\" was not understood, valid modes are: deathmatch and teamplay",mode);
		return false;
	}

	//Check chattiness.
	const char* chattiness = get_option("chattiness");
	if(strcasecmp(chattiness, "0") && strcasecmp(chattiness, "1")  && strcasecmp(chattiness, "2")  && 
		strcasecmp(chattiness, "3") && strcasecmp(chattiness, "4") && strcasecmp(chattiness, "fatal") && 
		strcasecmp(chattiness, "error")  && strcasecmp(chattiness, "warning")  && 
		strcasecmp(chattiness, "message") && strcasecmp(chattiness, "debug")) {
			sprintf(error, "Chattiness \"%s\" was not understood, valid values are: 0, 1, 2, 3, 4, fatal, error, "
				"warning, message, debug", chattiness);
			return false;
	}
		
	return true;
}

Configuration::~Configuration() {
	delete observers;
	Config_values::iterator i=values->begin();
	while (i!=values->end()) {
		delete i->second;
		i++;
	}	
	delete values;
}

void Configuration::add_option(char *name, char *description, char *default_value, int min, int max, bool integer_value, bool append_value) {
	Config_value *newvalue = new Config_value(description);
	newvalue->min = min;
	newvalue->max = max;
	newvalue->integer_value = integer_value;
	newvalue->append_value = append_value;
	newvalue->set_value(default_value);
	values->insert(std::pair<std::string, Config_value *>(name,newvalue));
}

bool Configuration::parse_cmdline(int argc,char **argv) {
	//run through all argv's and parse_option() on them
	for (int c=1;c<argc;c++)
		if (!parse_option(argv[c])) return false;
		
	return true;
}

bool Configuration::load_file(char *filename) {
	char expanded_filename[200];
	try {
		VFS::expand_path(expanded_filename, sizeof(expanded_filename),
			filename);
	} catch(std::exception& e) {
		cerror << "Unable to expand the path \"" <<filename << "\": "
			<< e.what() << "\n";
		return false;
	}

	FILE *f=fopen(expanded_filename,"r");
	if (!f) {
		sprintf(error,"Unable to open the configuration file %s",expanded_filename);
		return false;
	}

	fseek(f,0,SEEK_END);
	int length = ftell(f);
	fseek(f,0,SEEK_SET);
	
	char *file_contents = (char*)malloc(length+1);
	if ((length = fread(file_contents,1,length,f)) == 0) {
		sprintf(error,"Unable to read the configuration file %s",expanded_filename);
		free(file_contents);
		fclose(f);
		return false;
	}
	fclose(f);
	file_contents[length] = 0; //terminate the file

	bool result = parse_file(file_contents);
	if (!result)
		sprintf(error+strlen(error)," (while parsing %s)",expanded_filename);

	free(file_contents);

	return result;
}

bool Configuration::parse_file(char *file_contents) {
	char *lines = strdup(file_contents);
	char *lines_saved = lines;
	char *line=strsep(&lines, "\n");
	while (line) {
		//eat leading whitepace:
		line = strtrim(line);

		//deal with the line if it is not blank and not a comment
		if (*line && *line != '#')
			if (!parse_option(line)) {
				free(lines_saved);
				return false;
			}
		line=strsep(&lines,"\n");
	}
	free(lines_saved);
	return true;
}

bool Configuration::parse_option(char *line) {
	char *tmp = strdup(line);
	char *tmp_saved = tmp;

	//split on '=' and trim whitespace off of both the name and the value
	char *name = strsep(&tmp,"=");
	char *value = tmp;

	//note: values are allowed to be empty
	if (!name || !value || !*name) {
		sprintf(error,"Unable to parse the option: \"%s\" options must all be on the form: name=value",line);
		free(tmp_saved);
		return false;
	}
	
	name = strtrim(name);
	value = strtrim(value);

	if (!*name) {
		sprintf(error,"Unable to find a valid option name in the option: \"%s\" options must all be on the form: name=value",line);
		free(tmp_saved);
		return false;
	}

	bool result = set_option(name,value);
	
	free(tmp_saved);
	return result;
}

//-----------------------------------------------------------------------------
bool Configuration::clear_option(char *name)
{
	//find the option and set it to ""
	Config_values::iterator i=values->find(name);
	if (i == values->end()) {
		sprintf(error,"Unable to find an option named %s to clear",name);
		return false;
	}

	i->second->clear_value();

	notify_observers();

	return true;
}

//-----------------------------------------------------------------------------
bool Configuration::set_option(char *name, char *value)
{
	if (!value || !*value) return clear_option(name);

	//check limits and set the value.
	Config_values::iterator i=values->find(name);
	if (i == values->end()) {
		sprintf(error,"Unable to find an option named %s to set",name);
		return false;
	}

	if (i->second->integer_value) {

		int ival = -1;
		if (sscanf(value,"%i",&ival) != 1) {
			sprintf(error,"Unable to set the option %s to %s (the value must be an integer)",name,value);
			return false;
		}
		
		if (ival < i->second->min) {
			sprintf(error,"Unable to set the option %s to %s (%i), the minimum is %i",name,value,ival,i->second->min);
			return false;
		}
			
		if (ival > i->second->max) {
			sprintf(error,"Unable to set the option %s to %s (%i), the maximum is %i",name,value,ival,i->second->max);
			return false;
		}
			
		//normalize the string so that it is always stored as decimal
		char tmp_value[40];
		sprintf(tmp_value,"%i",ival);
		
		i->second->set_value(tmp_value);

	} else {
		//string values:
		i->second->set_value(value);
	}

	notify_observers();

	return true;
}

//-----------------------------------------------------------------------------
const char *Configuration::get_option(char *name)
{
	Config_values::iterator i=values->find(name);
	if (i == values->end()) {
		sprintf(error,"Unable to find an option named %s to get",name);
		logmsg(lt_warning,error);
		return NULL;
	}

	return i->second->get_value();
}

//-----------------------------------------------------------------------------
int Configuration::get_option_int(char *name)
{
	const char *value = get_option(name);
	int result = -1;
	if (value)
		sscanf(value,"%i",&result);
	return result;
}

//-----------------------------------------------------------------------------
Config_xy Configuration::get_option_xy(char *name)
{
	const char *value = get_option(name);
	Config_xy result;
	result.x = result.y = -1;
	if (value)
		sscanf(value,"%ix%i",&result.x,&result.y);
	return result;
}

//-----------------------------------------------------------------------------
void Configuration::print_manual(FILE *target)
{
	//find the length of the longest option name:
	int name_width = 0;
	{
		Config_values::iterator i=values->begin();
		while (i != values->end()) {
			if (strncmp(i->first.c_str(),"key",3) != 0) {
				int len=i->first.size();
				if (len > name_width)	name_width = len;
			}
			i++;
		}
	}

	//make up a format so the values will be neatly aligned:
	char format[500];
	sprintf(format,"%%%is : %%s",name_width);

	//actually print the stuff:
	{
		Config_values::iterator i=values->begin();
		while (i != values->end()) {
			if (strncmp(i->first.c_str(),"key",3) != 0) {
				fprintf(target,format,i->first.c_str(),i->second->get_description());
				if (i->second->integer_value)
					fprintf(target," Must be in the range (%i..%i)",i->second->min,i->second->max);
				fprintf(target,"\n");	
			}
			i++;
		}
	}
}

//-----------------------------------------------------------------------------
bool Configuration::save_options(char *filename)
{
	char expanded_filename[200];
	try {
		VFS::expand_path(expanded_filename, sizeof(expanded_filename),
			filename);
	} catch(std::exception& e) {
		cerror << "Unable to expand the path \"" <<filename << "\": "
			<< e.what() << "\n";
		return false;
	}

	FILE *f = fopen(expanded_filename,"w");
	if (!f) {
		sprintf(error,"Unable to open %s for saving the options",expanded_filename);
		return false;
	}

	print_options(f);	
	fclose(f);

	return true;
}

//-----------------------------------------------------------------------------
//dumps a nicely commented configuration file
void Configuration::print_options(FILE *target)
{
	fprintf(target,"# NiL options:\n\n");
	Config_values::iterator i=values->begin();
	while (i != values->end()) {

		fprintf(target,"# %s\n",i->second->get_description());	
		if (i->second->integer_value)
			fprintf(target,"# Must be an integer in the range (%i..%i)\n",i->second->min,i->second->max);
		fprintf(target,"%s=%s\n\n",i->first.c_str(),i->second->get_value());	
		i++;
	}
}
