/***************************************************************************
  world.h  -  Header for NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 1999-10-10
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-05-16
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef WORLD_H
#define WORLD_H

#include "common/systemheaders.h"
#include "client/graphics/viewport.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/vfs/loader.h"
#include "client/graphics/graphic.h"
#include "client/graphics/graphics.h"
#include "common/sound/soundsystem.h"

#define RNDTABLE_SIZE 15001

//! entry for the map, contains info about this location (walkable, hole, etc...)
//  this used to be encoded in the world bitmap alpha channel
typedef Uint8 mapEntry;

//! different map entry types
#define MAP_HOLE    0
#define MAP_EARTH   1
#define MAP_STONE   2
#define MAP_HURTXSM 3
#define MAP_HURTSM  4
#define MAP_HURTM   5
#define MAP_HURTXL  6
#define MAP_HURTXXL 7

class Obj_base;
class Obj_player_avatar;
class World_objects;
class Obj_scoreboard;
class Obj_settings;

//! @author Flemming Frandsen
//! @brief World knows about the map of the world, as well as the objects in it.
class World {
public:
	//! Default constructor
	World();
	/*! Constructor
	    @param loader_ Reference to the loader instance
	    @param soundsystem_ Reference to the soundsystem*/
	World(Loader *loader_, Soundsystem *soundsystem_);
	/*! Constructor
	    @param loader_ Reference to the loader instance
	    @param soundsystem_ Reference to the soundsystem
	    @param always_reload_ Flag to say if weapons should be always realoded */
	World(Loader *loader_, Soundsystem *soundsystem_, bool always_reload_);
	//! default destructor
	virtual ~World();
	/*! stores the references to loader and soundsystem
	    @param loader_ The reference to the loader
	    @param soundsystem_ The refernce to the soundsystem */
	void init(Loader *loader_, Soundsystem *soundsystem_);
	/*! Returns true if this world is a client (it is important to know if we are in a client
	    or a server when making up our minds about important stuff */
	virtual bool isclient()=0;
	//! in the server this will simply make a hole, on the client it will spawn some debris
	virtual void makehole(int x, int y, int diameter)=0;
	/*! Creates a blank map in the desired size
	    @param width The width of the map
	    @param height The height of the map */
	void initmap(int width, int height);
	/*! Creates a filled map in the desired size
	    @param width The width of the map
	    @param height The height of the map
	    @param randomseed_ A random seed used for map generation
	    @param stuff For stuff on the map (stones, bones, sandstones, holes) */
	void createmap(int width, int height, unsigned int randomseed_, int stuff);
	/*! creates a map from liero map file
	    @param map_filename The name of the map */
	void createmap(char *map_filename);
	/*! newobject is a factory that creates concrete objects
	   @param objecttype The object type to spawn
	   @param objectid If you want to set the object id, this is the what you want on a client (not a server)
	   @return Returns the pointer to the new object */
	Obj_base *newobject(Object_type objecttype, int objectid);
	/*! Simple and fast algo, it is also kind of inccurate +-radius, use for bombs and missiles.
	    @param x0 ???
	    @param y0 ???
	    @param x1 ???
	    @param y1 ???
	    @param xc ???
	    @param yc ???
	    @param radius ???
	    @return Returns true if a collision happened */
	bool collision(int x0, int y0, int x1, int y1, int &xc, int &yc, int radius);
	/*! The same algo, however it calls is_passable on object to test for collision,
	    this is accurate, but somewhat slower, this was written for Obj_player in mind,
	    so speed is not that important here.
	    @param x0 ???
	    @param y0 ???
	    @param x1 ???
	    @param y1 ???
	    @param xc ???
	    @param yc ???
	    @param object ???
	    @return Returns true if a collision happened */
	bool collision(int x0, int y0, int x1, int y1, int &xc, int &yc, Obj_base *object);
	/*! collision with stones
	    @param x0 ???
	    @param y0 ???
	    @param x1 ???
	    @param y1 ???
	    @param xc ???
	    @param yc ???
	    @param radius ???
	    @return Returns true if a collision happened */
	bool stoneCollision(int x0, int y0, int x1, int y1, int &xc, int &yc, int radius);
	//! time of last frame
	float get_last_frame_delta() const { return curtime - last_frame_time;}
	//! display all server objects in the world, debugging
	void dump_objects();
	//! print current time via logmsg, debugging
	void dump_time () { logmsg(lt_message, "Now is %f AD", gettime()); }
	//! ???
	void assert_object_type(int objectid, Object_type expected_type);
	//! start a sound via the soundsystem->start_sound function
	int startSound(char *sample_name, int flags, float volume) { return soundsystem->start_sound(sample_name,flags,volume);}
	//! start a sound via the soundsystem->start_sound function
	int startSound(char *sample_name, int flags, float volume, float x_pos, float y_pos) const { return soundsystem->start_sound(sample_name, flags, volume, x_pos, y_pos); }
	//! start a sound via the soundsystem->start_sound function
	int startSound(char *sample_name, int flags, float volume, int objectid) const { return soundsystem->start_sound(sample_name, flags, volume, objectid); }
	/*! check if position x,y is passable, safe variant, does boundary checks
	    @param x X coordinate of the position
	    @param y Y coordinate of the position
	    @return Returns true if the position is passable */
	bool isPassable(int x, int y);
	/*! check if position x,y is a stone, warning doesnt perform boundary checks
	    @param x X coordinate of the position
	    @param y Y coordinate of the position
	    @return Returns true if the position is a stone */
	bool isStone(int x, int y);
	//! This is set by avatars when they die, used for death cam
	float time_of_death;

	// various info functions
	//! These are a few sugar coated ways of getting to objects in the world.
	int getobjcount();
	//! always reload weapons?
	bool get_always_reload();
	//! returns object with objectid
	Obj_base *get_object(int objectid, Object_type expected_type);
	/*! get player avatar for a player
	    @param playerId*/
	Obj_player_avatar *get_player_avatar(int playerId);
	/*! get player position based on id of player
	    @param playerId The ID of the player
	    @param x The X coordinate of the player
	    @param y The Y coordinate of the player
	    @return Return true if the playerposition is returned */
	bool get_player_pos(int playerId, int &x, int &y);
	//! get no. of players (brute force search for all player avatar objects)
	int get_player_count();
	/*! Get the player number
	    @param playerId The player number
	    @return Returns the reference to the player if it exists, otherwise return NULL */
	Obj_player_avatar *get_player_number(int playerId);

	// various (inline) access functions
	//! get the current time
	inline float gettime() const { return curtime; }
	/*! set world time (remember the time for this frame for calculating the delta time later)
	    NOTE: This will only work correctly if time is only set once per frame... */
	void settime(float newtime) { last_frame_time = curtime; curtime = newtime; }
	//! returns the size of the world in x dir, inited in createmap
	inline Sint32 getSizeX() const { return mSizeX; }
	//! returns the size of the world in y dir, inited in createmap
	inline Sint32 getSizeY() const { return mSizeY; }
	//! returns pointer to the world objects
	inline World_objects *getObjects() const { return objects; }
	//! returns pointer to the loader
	inline Loader *getLoader() const { return mpLoader; }
	//! returns a buffer to the world map (e.g. sparks make stains)
	inline Mutable_raw_surface *getMapBuffer() { return &map_buffer; }
	//! get one entry of the map (to e.g. check for stones), doesnt check bounds!
	inline Pixel getMapEntryOld(int x, int y) const { return *(map_buffer.get_pixels_read()+x+y*map_buffer.get_xsize()); }
	//! set one entry of the map, doesnt check bounds!
	inline void setMapEntry(int x, int y, mapEntry set){ mpMapFlag[x+y*mSizeX] = set; }
	//! get one entry of the map (to e.g. check for stones), doesnt check bounds!
	inline mapEntry getMapEntry(int x, int y) const { return mpMapFlag[x+y*mSizeX]; }
	//! get the scoreboard object
	inline Obj_scoreboard *getscoreboard() { return (Obj_scoreboard *)get_object(scoreboard_id,ot_scoreboard);}
	//! get the scoreboard object
	inline Obj_settings *getServerSettings() const { return mpSettings; }
	//! set the scoreboard object
	inline void setServerSettings(Obj_settings *set) { mpSettings = set; }

protected:
	//! FIXME Make this an std::multimap (or hash_multimap?) instead:
	World_objects *objects;
	//! the loader object for getting graphics and sounds
	Loader *mpLoader;
	//! unmodified world size
	Sint32 mSizeX, mSizeY;
	//! the gaming world
	Mutable_raw_surface map_buffer;
	//! background that gets visible when the map gets holes
	Mutable_raw_surface bgmap_buffer;
	//! flag array to check for obstacles, earth etc...
	mapEntry *mpMapFlag;
	//! Sound system
	Soundsystem *soundsystem;
	//! the hole sprites for makehole
	Graphic *holes[16];
	//! delete done objects
	void remove_dead_objects();
	//! used for calculating how much time the last frame took 
	float last_frame_time; 
	//! current time in the world
	float curtime;
	//! object id of the scoreboard object
	int scoreboard_id;
	//! id of the server settings object
	Obj_settings *mpSettings;
	//! The randomseed used for the map:
	unsigned int randomseed;
	//! If true, weapons are reloaded whenever possible; otherwise, only a player's selected weapon is reloaded.
	bool always_reload;
};

#endif
