/***************************************************************************
  fluff.h  -  Headerfile that defines a lot of unreleated stuff that is
              commonly used, but had no other logical place to be.
 ---------------------------------------------------------------------------
  begin                : 1999-10-09
  by                   : Flemming Frandsen (dion@swamp.dk)

  expansion            : 2000-01-30
  by                   : Eero Tamminen (t150315@cc.tut.fi)

  last changed         : 2004-03-21
  by                   : Nils Thuerey (n_t@gmx.de)

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Remove expand_path. Loader.cpp has the new version.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Remove declarations related to to logging.

  last changed         : 2004-10-24
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add strlcpy and strlcat.

  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : header fix, moved out timer stuff
  
  copyright            : (C) 1999 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#ifndef FLUFF_H
#define FLUFF_H

#include "systemheaders.h"


//The maximum number of objects on the server that is synced down to the clients
#define MAX_REAL_OBJECTS (2 << 16)

//The maximum number of objects that only exist on the client
#define MAX_ANON_OBJECTS (2 << 16)

//! default port to listen for games
#define IP_PORT 0xBABE

//physics defaults:
#define DEFAULT_ACCEL 60.0        // weapon acceleration, def 0.01
#define DEFAULT_DRAG 0.01         // def 0.01
#define KNOCK_BACK 0.0005         // def 0.001
#define RADIUS_DAMAGE 2.0         // radius for damage/hurting
#define MAX_HEALTH 2000           // max life energy, default 1000
#define RESPAWN_TIME  3.0         // time before respawn (in seconds)

//! The number of weapons carried
#define WEAPON_SLOTS 5

//! compares two strings and returns which one is smaller
struct Strless {
	//! ???
	bool operator() (const char* s1, const char* s2) const {
		return strcmp(s1, s2) < 0;
	}
};

//! compares two strings and returns is they are the same
struct Streq {
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) == 0;
	}
};

//! strlcpy (the standard one).
size_t strlcpy(char* dst, const char* src, size_t dstsize);

//! strlcat (the standard one).
size_t strlcat(char* dst, const char* src, size_t dstsize);

//--------------------------------------------------------------------------

//! types of objects that lives in the world:
typedef enum Object_type {
	//player:
	ot_player_avatar=0,
	ot_player_data,
	ot_gib,
	ot_chatmessage,
	// bonuses:
	ot_bonus_health,
	ot_bonus_invisibility,
	//weapon invisibles:
	ot_weapon_test,
	ot_weapon_minigun,
	ot_weapon_shotgun,
	ot_weapon_megashotgun,
	ot_weapon_railgun,
	ot_weapon_plasmagun,
	ot_weapon_larpa,
	ot_weapon_rocket,
	ot_weapon_homingmissile,
	ot_weapon_mininuke,
	ot_weapon_bignuke,
	ot_weapon_clusterbomb,
	ot_weapon_flamethrower,
	ot_weapon_jetpack,
	//Other objects
	ot_testobject,
	ot_explosion,
	ot_projectile,
	ot_smoke,
	ot_hook,
	ot_scoreboard,
	ot_railslug,
	ot_plasmaball,
	ot_fireball,
	ot_tinygranade,
	ot_bignuke,
	ot_rocket,
	ot_homingmissile,
	ot_larpa,
	ot_clusterbomb,
	ot_grenade,
	ot_settings,
	//Anon objects:
	ot_particle_blood,
	ot_particle_debris,
	ot_particle_railtrail,
	ot_particle_spark,
	ot_particle_smoke,
	//ot_error MUST be the last object type, as it is also the number of objecttypes
	//used for a flag when returnined from a function that didn't
	ot_error
} objetcs;

// The range of legal weapons.
const int ot_weapon_first=ot_weapon_test+1;
const int ot_weapon_last=ot_testobject-1;

//! Default weapons
extern const int weaponinit_default[WEAPON_SLOTS];
extern const int weaponinit_oldnil[WEAPON_SLOTS];

//! Weapon modes
enum Weapon_style {
	//! weapons are fixed (defined in fluff.cpp)
	ws_fixed = 0,
	//! weapons are fixed, using old definition from fluff.cpp
	ws_old_nil,
	//! weapons are generated randomly for every client
	ws_rand,
	//! weapons are generated randomly, but the same for all clients
	ws_randidentical
};

//! Active game mode
enum Active_mode {
	//! we are in game
	am_game = 0,
	//! we are in menu
	am_menu,
	//! we are in chat
	am_chat
};

//! names of all the objects (has to correspond to Object_type order)
const char OBJECT_NAMES[ot_error+1][40] = {
	"Player Avatar",
	"Player Data",
	"Giblet",
	"Chat Message",
	
	"Health Bonus",
	"Invisibility Bonus",

	"Experimental gun",
	"Minigun",
	"Shotgun",
	"Mega Shotgun",
	"Rail gun",
	"Plasma gun",
	"Larpa gun",
	"Rocket Launcher",
	"Homing Missile Launcher",
	"Mini nukegun",
	"Big nukegun",
	"Cluster bomb launcher",
	"Flame Thrower",
	"Jetpack",

	"Test object",
	"Explosion",
	"Projectile",
	"Smoke",
	"Hook",
	"Scoreboard",
	"Railslug",
	"Plasmaball",
	"Fireball",
	"TinyGranade",
	"BigNuke",
	"Rocket",
	"Homing Missile",
	"Larpa",
	"Clusterbomb",
	"Grenade",
	"ServerSettings",

	"Particle (blood)",
	"Particle (debris)",
	"Particle (railtrail)",
	"Particle (spark)",
	"Particle (smoke)",

	"ERROR"
};

//! get object id from name
Object_type ot_from_name (const char *name);

//! team defaults
#define TEAM_NUM 4
extern Sint32 teamColorsMin[TEAM_NUM];
extern Sint32 teamColorsMax[TEAM_NUM];
extern char *teamNames[TEAM_NUM];

/*! gives a random integer in the range min..max
    @param min The minimum of the range
    @param max The maximum of the range
    @return The random int */
int rnd(int min, int max);
/*! makes a random color in the colorspace defined by c1 and c2
    @param c1 Minimum of the color range
    @param c2 Maximum of the color range
    @return The randomly created color */
unsigned int mkrndcolor(unsigned int c1, unsigned int c2);
/*! make background color for e.g. chat messages
    @param player_color The color of the player (used to calculate blood color)
    @return The calculated blood color */
unsigned int makeBackgroundColor(unsigned int player_color);
/*! makes a random color for the blood
    @param player_color The color of the player (used to calculate blood color)
    @return The calculated blood color */
unsigned int mk_bloodcolor(unsigned int player_color);
/*! makes a random color for the blood tinted with earth color
    @param player_color The color of the player (used to calculate blood color)
    @return The calculated blood color */
unsigned int mk_dark_bloodcolor(unsigned int player_color);
/*! Gets time of day in m
    @return The time of day in ms*/
unsigned int time_ms();
/*! Calculate the time between 2 timestamps
    @param t_old The old timestamp
    @param t_new The new timestamp */
unsigned int time_elapsed(unsigned int t_old, unsigned int t_new);
/*! Check if a char is a whitespace
    @param ch The character to test
    @return Returns true if the character is a whitespace */
bool is_whitespace(char ch);
/*! Remove trailing and leading whitespace
    @param string The string to trim
    @return The trimmed string */
char *strtrim(char *string);

//! true when the user wishes to quit the program
extern bool request_quit;

//! true when the user wishes to quit the program
extern bool colored_blood;

#endif
