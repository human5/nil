/***************************************************************************
  world.cpp  -  NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 1999-10-10
  by                   : Flemming Frandsen
  email                : dion@swamp.d

  last changed         : 2004-05-16
  by                   : Nils Thuerey
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/console/logmsg.h"
#include "common/sound/soundsystem.h"
#include "common/systemheaders.h"
#include "common/world.h"
#include "common/misc.h"
#include "common/world_objects.h"
#include "common/ripped.h"
#include "common/randomstream.h"
#include <list>

// all (non-anon) objects headers are needed for creation
#include "common/objects/obj_test.h"

#include "common/objects/obj_explosion.h"
#include "common/objects/obj_larpa.h"
#include "common/objects/obj_particle.h"
#include "common/objects/obj_chatmessage.h"
#include "common/objects/obj_projectile.h"
#include "common/objects/obj_plasmaball.h"
#include "common/objects/obj_fireball.h"
#include "common/objects/obj_bignuke.h"
#include "common/objects/obj_hook.h"
#include "common/objects/obj_clusterbomb.h"
#include "common/objects/obj_tinygranade.h"
#include "common/objects/obj_rocket.h"
#include "common/objects/obj_homingmissile.h"
#include "common/objects/obj_grenade.h"
#include "common/objects/obj_player_avatar.h"
#include "common/objects/obj_player_data.h"
#include "common/objects/obj_scoreboard.h"
#include "common/objects/obj_settings.h"
#include "common/objects/obj_railslug.h"

#include "common/objects/obj_bonus.h"
#include "common/objects/obj_bonus_health.h"
#include "common/objects/obj_bonus_invisibility.h"

#include "common/objects/obj_weapon.h"
#include "common/objects/obj_weapon_test.h"
#include "common/objects/obj_weapon_minigun.h"
#include "common/objects/obj_weapon_shotgun.h"
#include "common/objects/obj_weapon_railgun.h"
#include "common/objects/obj_weapon_mininuke.h"
#include "common/objects/obj_weapon_rocket.h"
#include "common/objects/obj_weapon_homingmissile.h"
#include "common/objects/obj_weapon_jetpack.h"
#include "common/objects/obj_weapon_larpa.h"
#include "common/objects/obj_weapon_plasmagun.h"
#include "common/objects/obj_weapon_flamethrower.h"
#include "common/objects/obj_weapon_megashotgun.h"
#include "common/objects/obj_weapon_clusterbomb.h"
#include "common/objects/obj_weapon_bignuke.h"

World::World() {
	objects = new World_objects;
	always_reload = true;
	curtime = 0;
	init(NULL, NULL);
}

World::World(Loader *loader_, Soundsystem *soundsystem_) {
	objects = new World_objects;
	always_reload = true;
	curtime = 0;
	init(loader_, soundsystem_);
}

World::World(Loader *loader_, Soundsystem *soundsystem_, bool always_reload_) {
	objects = new World_objects;
	always_reload = always_reload_;
	curtime = 0;
	init(loader_, soundsystem_);
}

void World::init(Loader *loader_, Soundsystem *soundsystem_) {
	mSizeX = mSizeY = -1;
	mpLoader = loader_;
	soundsystem = soundsystem_;
	scoreboard_id = -1;
	mpSettings = NULL;
	mpMapFlag = NULL;
	time_of_death = -1;
	if (mpLoader) {
		for (int c=0; c<16; c++) {
			char gname[100];
			sprintf(gname,"gfx/map/ripped/hole_%i.rgbf",c+1);
			holes[c] = mpLoader->graphics.getgraphic(gname);
		}
	}
}

World::~World() {
	logmsg ( lt_debug , "Deleting world objects...");
	World_objects::iterator i=objects->begin();
	while (i != objects->end()) {
		// didnt help... if( (!isclient()) && (i->second->gettype() == ot_player_data) ) { } else { }
		delete i->second;
		i++;
	}
	delete objects;
	if(mpMapFlag) free(mpMapFlag);
	logmsg ( lt_debug , "All world objects are gone!");
}

Obj_base *World::newobject(Object_type objecttype, int objectid) {
	Obj_base *newobject = NULL;
	switch (objecttype) {
		//the objects that we can create:
		case ot_explosion            : newobject = new Obj_explosion(this, objectid); break;
		case ot_bonus_health         : newobject = new Obj_bonus_health(this, objectid); break;
		case ot_bonus_invisibility   : newobject = new Obj_bonus_invisibility(this, objectid); break;
		case ot_testobject           : newobject = new Obj_test(this, objectid); break;
		case ot_player_avatar        : newobject = new Obj_player_avatar(this, objectid); break;
		case ot_player_data          : newobject = new Obj_player_data(this, objectid); break;
		case ot_chatmessage          : newobject = new Obj_chatmessage(this, objectid); break;
		case ot_grenade              : newobject = new Obj_grenade(this, objectid); break;
		case ot_projectile           : newobject = new Obj_projectile(this, objectid); break;
		case ot_hook                 : newobject = new Obj_hook(this, objectid); break;
		case ot_scoreboard           : newobject = new Obj_scoreboard(this, objectid); break;
		case ot_tinygranade          : newobject = new Obj_tinygranade(this, objectid); break;
		case ot_bignuke              : newobject = new Obj_bignuke(this, objectid); break;
		case ot_rocket               : newobject = new Obj_rocket(this, objectid); break;
		case ot_weapon_rocket        : newobject = new Obj_weapon_rocket(this, objectid); break;
		case ot_weapon_mininuke      : newobject = new Obj_weapon_mininuke(this, objectid); break;
		case ot_weapon_test          : newobject = new Obj_weapon_test(this, objectid); break;
		case ot_weapon_minigun       : newobject = new Obj_weapon_minigun(this, objectid); break;
		case ot_weapon_shotgun       : newobject = new Obj_weapon_shotgun(this, objectid); break;
		case ot_weapon_jetpack       : newobject = new Obj_weapon_jetpack(this, objectid); break;
		case ot_settings             : newobject = new Obj_settings(this, objectid); break;
		case ot_homingmissile        : newobject = new Obj_homingmissile(this, objectid); break;
		case ot_weapon_homingmissile : newobject = new Obj_weapon_homingmissile(this, objectid); break;
		case ot_larpa                : newobject = new Obj_larpa(this, objectid); break;
		case ot_weapon_larpa         : newobject = new Obj_weapon_larpa(this, objectid); break;
		case ot_plasmaball           : newobject = new Obj_plasmaball(this, objectid); break;
		case ot_weapon_plasmagun     : newobject = new Obj_weapon_plasmagun(this, objectid); break;
		case ot_fireball             : newobject = new Obj_fireball(this, objectid); break;
		case ot_weapon_flamethrower  : newobject = new Obj_weapon_flamethrower(this, objectid); break;
		case ot_weapon_megashotgun   : newobject = new Obj_weapon_megashotgun(this, objectid); break;
		case ot_clusterbomb          : newobject = new Obj_clusterbomb(this, objectid); break;
		case ot_weapon_clusterbomb   : newobject = new Obj_weapon_clusterbomb(this, objectid); break;
		case ot_weapon_bignuke       : newobject = new Obj_weapon_bignuke(this, objectid); break;
		case ot_railslug             : newobject = new Obj_railslug(this, objectid); break;
		case ot_weapon_railgun       : newobject = new Obj_weapon_railgun(this, objectid); break;

		//The objects that we cannot:
		case ot_particle_debris     :
		case ot_particle_railtrail  :
		case ot_particle_spark      :
		case ot_particle_smoke      :
		case ot_smoke               : 
		case ot_gib                 :
		case ot_particle_blood      : newobject = NULL; return newobject; logmsg(lt_assert, "Someone tried to create an anonymous particle on the server, this is wrong!"); break;
		case ot_error               : newobject = NULL; return newobject; break;
		default : logmsg ( lt_fatal , "You fucked it up - there is no such object %i!" , objectid );
	}

	bool ok = objects->insert(std::pair<int,Obj_base*>(objectid,newobject)).second;
	if(!ok) {
		logmsg(lt_warning,"Unable to add %i (%s) - key already exists!",objectid,newobject->getname());
		return NULL;
	}
	//logmsg(lt_debug,"#id:%d,type:%d created: %s, %d",objectid,objecttype,newobject->getname(), newobject->gettype() );

	return newobject;
}

void World::initmap(int width, int height) {
	logmsg(lt_debug, "Creating a map that is %i x %i pixels", width, height);

	map_buffer.create(width, height);
	bgmap_buffer.create(width, height);
	mpMapFlag = (mapEntry *)calloc( sizeof(mapEntry), width*height );
	if(!mpMapFlag) {
		logmsg(lt_error, "World::initmap: Unable to allocate mpMapFlag buffer");
		exit(1);
	}
	// remember size
	mSizeX = width;
	mSizeY = height;
}

void World::createmap(int width, int height, unsigned int randomseed_, int stuff) {
	randomseed = randomseed_;
	//srandom(randomseed);
	Random_stream rstr(randomseed);
	//create map
	initmap(width, height);
	logmsg(lt_debug, "populating the map");
	// MAPDEBUG = some map debugging things...
	//stuff=0; // MAPDEBUG dont insert any stuff
	int x, y;
	for (y=0;y < height;y++)
		for (x=0;x < width;x++) {
			setMapEntry( x,y, MAP_EARTH );
			map_buffer.setRawPixel(x,y, rstr.get_color(0x784008, 0x985414) | PF_TYPE_EARTH);
#ifdef NO_EARTH
			map_buffer.setRawPixel(x,y, rstr.get_color(0x341800, 0x602000) | PF_TYPE_HOLE);
			setMapEntry( x,y, MAP_HOLE );
#endif
		}

	for (y=0;y < height;y++)
		for (x=0;x < width;x++) {
			bgmap_buffer.setRawPixel(x,y, rstr.get_color(0x341800, 0x602000) | PF_TYPE_HOLE);
		}

	//place some artifacts:
	Graphic *decor[20];	
	//bones:
	decor[0] = mpLoader->graphics.getgraphic("gfx/map/ripped/bones_1.rgbf");
	decor[1] = mpLoader->graphics.getgraphic("gfx/map/ripped/bones_2.rgbf");
	decor[2] = mpLoader->graphics.getgraphic("gfx/map/ripped/bones_3.rgbf");
	decor[3] = mpLoader->graphics.getgraphic("gfx/map/ripped/bones_4.rgbf");
	//clay:
	decor[4] = mpLoader->graphics.getgraphic("gfx/map/ripped/clay_1.rgbf");
	decor[5] = mpLoader->graphics.getgraphic("gfx/map/ripped/clay_2.rgbf");
	decor[6] = mpLoader->graphics.getgraphic("gfx/map/ripped/clay_3.rgbf");
	decor[7] = mpLoader->graphics.getgraphic("gfx/map/ripped/clay_4.rgbf");
	//stones:
	decor[11] = mpLoader->graphics.getgraphic("gfx/map/ripped/stone_1.rgbf");
	decor[12] = mpLoader->graphics.getgraphic("gfx/map/ripped/stone_2.rgbf");
	decor[13] = mpLoader->graphics.getgraphic("gfx/map/ripped/stone_3.rgbf");
	decor[14] = mpLoader->graphics.getgraphic("gfx/map/ripped/stone_4.rgbf");
	decor[15] = mpLoader->graphics.getgraphic("gfx/map/ripped/stone_5.rgbf");
	decor[16] = mpLoader->graphics.getgraphic("gfx/map/ripped/stone_5.rgbf");
	//put the decor:
	int c;
	// stones
	for (c=0; c < 3*stuff; c++) decor[rstr.get_integer(11,16)]->draw_map(&map_buffer,&bgmap_buffer,rstr.get_integer(12,width-12),rstr.get_integer(8,height-18));	
	// bones
	for (c=0; c < 5*stuff; c++) decor[rstr.get_integer(4,7)  ]->draw_map(&map_buffer,&bgmap_buffer,rstr.get_integer(8,width-8),rstr.get_integer(8,height-8));	
	// sandstones
	for (c=0; c < 3*stuff; c++) decor[rstr.get_integer(0,3)  ]->draw_map(&map_buffer,&bgmap_buffer,rstr.get_integer(8,width-8),rstr.get_integer(8,height-8));	
	// holes
	for (c=0; c < 15*stuff; c++) holes[rstr.get_integer(0,15)]->draw_map(&map_buffer,&bgmap_buffer,rstr.get_integer(8,width-8),rstr.get_integer(8,height-8));	

	// special armageddon init, dont touch the stone border!
	if(getServerSettings()->getArmageddonTime() > 0) {
		// init water
		for (y=height-10;y < height;y++)
			for (x=0;x < width;x++) {
				setMapEntry( x,y, MAP_HOLE );
				map_buffer.setRawPixel(x,y, rstr.get_color(0x000020, 0x000090) | PF_TYPE_HOLE);
				//map_buffer.setRawPixel(x,y, rstr.get_color(0xFFFF00, 0xAAAA00) | PF_TYPE_HOLE);
			}
	}

	//Frame the map with stone:
	for (x=0; x < map_buffer.get_xsize(); x++) {
		map_buffer.setRawPixelType(x, 0, PF_TYPE_STONE);
		map_buffer.setRawPixel(x, 0, 0x00000000|PF_TYPE_STONE);
	}

	for (x=0; x < map_buffer.get_xsize(); x++) {
		map_buffer.setRawPixelType(x, map_buffer.get_ysize()-1, PF_TYPE_STONE);
		map_buffer.setRawPixel(x, map_buffer.get_ysize()-1, 0x00000000|PF_TYPE_STONE);
	}

	for (y=0; y < map_buffer.get_ysize(); y++) {
		map_buffer.setRawPixelType(0,y,PF_TYPE_STONE);
		map_buffer.setRawPixelType(map_buffer.get_xsize()-1,y, PF_TYPE_STONE);
		map_buffer.setRawPixelType(0,y, 0x00000000|PF_TYPE_STONE);
		map_buffer.setRawPixelType(map_buffer.get_xsize()-1,y, 0x00000000|PF_TYPE_STONE);
	}

	logmsg(lt_debug,"Done creating map");
}

void World::createmap( char *map_filename ) {
	FILE *file;

	file = fopen ( map_filename , "r" );
	if ( !file ) {
		logmsg ( lt_warning , "Cannot find map '%s', falling back to random" , map_filename );
		createmap ( LIERO_MAP_SIZE_X , LIERO_MAP_SIZE_Y , 1, 10 );
		return;
	}
	initmap( LIERO_MAP_SIZE_X , LIERO_MAP_SIZE_Y );
	int bytes_read = 0;
	while (( !feof ( file ) ) && ( bytes_read < LIERO_MAP_SIZE_X * LIERO_MAP_SIZE_Y )) {
		int bits = 0;
		int byte = fgetc ( file );
		int x,y;
		x = bytes_read % LIERO_MAP_SIZE_X;
		y = bytes_read / LIERO_MAP_SIZE_X;
		if ( vDiggable [ byte ] )
			bits = PF_TYPE_EARTH;
		else if ( !vAir [ byte ] )
			bits = PF_TYPE_STONE;
		else
			bits = PF_TYPE_HOLE;
		map_buffer.setRawPixel( x , y , lieropal [ byte ] | bits );
		if ( bits != PF_TYPE_EARTH )
			bgmap_buffer.setRawPixel(x,y, lieropal [ byte ] | PF_TYPE_HOLE);
		else
			bgmap_buffer.setRawPixel(x,y, lieropal [ 1 ] | PF_TYPE_HOLE);
		bytes_read++;
	}
	logmsg ( lt_debug , "br: %d" , bytes_read );
	fclose ( file );

	//Frame the map with stone:
	/*int x,y;
	for (x=0; x < map_buffer.get_xsize(); x++)
		map_buffer.setRawPixelType(x, 0, PF_TYPE_STONE);

	for (x=0; x < map_buffer.get_xsize(); x++)
		map_buffer.setRawPixelType(x, map_buffer.get_ysize()-1, PF_TYPE_STONE);

	for (y=0; y < map_buffer.get_ysize(); y++) {
		map_buffer.setRawPixelType(0,y,PF_TYPE_STONE);
		map_buffer.setRawPixelType(map_buffer.get_xsize()-1,y, PF_TYPE_STONE);
	}*/

	if ( bytes_read != LIERO_MAP_SIZE_X * LIERO_MAP_SIZE_Y )
		logmsg ( lt_warning , "Map '%s' seems to be incomplete" , map_filename );
	else
		logmsg ( lt_message , "Map '%s' succesfully loaded" , map_filename );
}

//The bits allocated for error accumulation
#define ERRORBITS 16

bool World::collision(int x0, int y0, int x1, int y1, int &xc, int &yc, int radius) {
	/* This routine is a highly perverted bresenham line algorithm it has
	   been optimized in various non-logical ways, be very careful when
	   fooling with it, a slight imperfection will cause havoc. No scaling
	   anymore to check x,y components ( note that the y component is scaled
	   by xsize in the inner loops, so we must add or substract in xsize
	   increments. )
	
	   Warning: this routine relies on closed world boundaries if the map is
	   not surrounded with stone, the radius checks may segfault
	
	   clamp in the y direction (but not in x, overflowing in x will not
	   segfault :), well do it anyway :)*/
	
	//TODO: better calculate real line-border intersection
	if (y1 > map_buffer.get_ysize()-1) y1 = map_buffer.get_ysize()-1;
	if (y0 > map_buffer.get_ysize()-1) y0 = map_buffer.get_ysize()-1;
	if (y1 < 0) y1 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 > map_buffer.get_xsize()-1) x1 = map_buffer.get_xsize()-1;
	if (x0 > map_buffer.get_xsize()-1) x0 = map_buffer.get_xsize()-1;
	if (x1 < 0) x1 = 0;
	if (x0 < 0) x0 = 0;
	int xorg = x1, yorg = y1;

	//get delta values
	int dx = x1-x0;
	int dy = y1-y0;
	//const int mapSizeX = map_buffer.get_xsize();

	// always first check source position, this shouldnt happen, but you never know...
	// TODO: check whole radius?
	//if (!IS_PASSABLE(pixels[x0+y0*mapSizeX])) {
	if (!isPassable(x0,y0) ) {
		xc = x0;
		yc = y0;
		return true;	
	}
	
	// zero length is boring (and a dangerous special case) so get rid of it.
	if (dx == 0 && dy == 0) {
		return false;
	}

	// position tracking for line
	int x, y;
	// last good position in line
	int old_y, old_x;
	// precalculated offset to the interesting pixels
    // XXX Unused Variable
	//int offset;
	if (ABS(dx) > ABS(dy)) {
		// x major
		
		int ymove = SIGN(dy);                // The step to take with y when error hits 1
		dy = (ABS(dy) << ERRORBITS)/ABS(dx); // The value to add to error each x
		int e = 0;                           // error, fixed point with errorbits bits between 0 and 1.
		    x = x0;                          // x is just the x coordinate
		    y = y0;                          // y is offset into buffer, not the y coordinate (this saves many muls)
		    old_y = y;

		dx = SIGN(dx);                       // we just need the sign of dx
		x1 += dx;
		while (x != x1 ) {

			//calc the offset to the most interesting pixel
			//offset = x+y*mapSizeX; //saves radius*2 adds.

			// first check center pixel
			//if (!IS_PASSABLE(pixels[offset])) {
			if(!isPassable(x,y) ) {
				xc = x-dx;
				yc = old_y;
				return true;
			}
			// then check side pixels in y direction
			//for (int c=map_buffer.get_xsize();c<radius;c+=map_buffer.get_xsize()) {
			for (int c=1;c<radius;c++) {
				//if (!IS_PASSABLE(pixels[offset+c]) || !IS_PASSABLE(pixels[offset-c])) {
				if( (!isPassable(x,y+c)) || (!isPassable(x,y-c)) ) {
					xc = x-dx;
					//yc = old_y/map_buffer.get_xsize();
					yc = old_y;
					return true;
				}
			}

			//tally the error and ajust
			e += dy;
			if (e >= (1 << ERRORBITS)) {
				old_y = y;
				y += ymove;
				e -= (1 << ERRORBITS);
			}

			//move x one pixel in the right direction
			x += dx;

		} //while (x != x1) {

		// store last good x position for final check
		old_x = x-dx;
	} else {
		//y major

		int xmove = SIGN(dx);                // The step to take with x when error hits 1 << ERRORBITS
		dx = (ABS(dx) << ERRORBITS)/ABS(dy); // The value to add to error each y
		int e = 0;                           // error, fixed point with ERRORBITS bits between 0 and 1.
		    x = x0;                          // x is just the x coordinate
		    y = y0;                          // y is offset into buffer, not the y coordinate (this saves many muls)
		    old_x = x;

		dy = SIGN(dy);                       // we just need the sign of dy scaled to one pixel up or down
		y1 += dy; //NT
		while (y != y1 ) {

			//calc the offset to the most interesting pixel
			//offset = x+y*mapSizeX; //saves radius*2 adds.

			// first check center pixel
			if(!isPassable(x,y) ) {
			//if (!IS_PASSABLE(pixels[offset])) {
				xc = old_x;
				yc = (y-dy);
				return true;
			}
			// then the side pixels in x dir
			for (int c=1;c<radius;c++) {
				//if (!IS_PASSABLE(pixels[offset+c]) || !IS_PASSABLE(pixels[offset-c])) {
				if( (!isPassable(x+c,y)) || (!isPassable(x-c,y)) ) {
					xc = old_x;
					//yc = (y-dy)/map_buffer.get_xsize();
					yc = (y-dy);
					return true;
				}
			}

			//tally the error and ajust
			e += dx;
			if (e >= (1 << ERRORBITS)) {
				old_x = x;
				x += xmove;
				e -= (1 << ERRORBITS);
			}

			//move y one pixel in the right direction
			y += dy;

		} //while (y != y1) {

		// store last good y position for final check
		old_y = y-dy;
	}

	// NT the following check seems unecessary, but it can happen (due to roundoff errors?, I've been 
	// too lazy to really figure it out) that the final position in direction of the smaller delta
	// isnt checked, so we have to fall back to the last good position in the line, which is old_x,old_y...
	// TODO: also check whole radii here?
	//if (!IS_PASSABLE(pixels[xorg+yorg*map_buffer.get_xsize()])) {
	if (!isPassable(xorg,yorg)) {
		xc = old_x; yc = old_y;
		// just for safety, this should never happen!
		//if (!IS_PASSABLE(pixels[xc+yc*map_buffer.get_xsize()])) {
		if (!isPassable(xc,yc)) {
			logmsg(lt_error,"The collision check failed - new pos still collides...??? (%d,%d)", xc,yc);
		}
		return true;
	} //D8
	return false;
}

bool World::collision(int x0, int y0, int x1, int y1, int &xc, int &yc, Obj_base *object) {
	// This routine is a highly perverted bresenham line algorithm
	// it has been optimized in various non-logical ways,
	// be very careful when fooling with it, a slight imperfection will cause havoc.
	// no scaling anymore to check x,y components ( note that the y component is scaled by xsize in the inner loops,
	// so we must add or substract in xsize increments. )

	// clamp in the y direction (but not in x, overflowing in x will not segfault :), well do it anyway :)
	// TODO: better calculate real line-border intersection
	if (y1 > map_buffer.get_ysize()-1) y1 = map_buffer.get_ysize()-1;
	if (y0 > map_buffer.get_ysize()-1) y0 = map_buffer.get_ysize()-1;
	if (y1 < 0) y1 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 > map_buffer.get_xsize()-1) x1 = map_buffer.get_xsize()-1;
	if (x0 > map_buffer.get_xsize()-1) x0 = map_buffer.get_xsize()-1;
	if (x1 < 0) x1 = 0;
	if (x0 < 0) x0 = 0;
	int xorg = x1, yorg = y1;

	//get delta values
	int dx = x1-x0;
	int dy = y1-y0;

	// always first check source position, this shouldnt happen, but you never know...
	if (!object->is_passable(x0,y0)) {
		xc = x0;
		yc = y0;
		return true;
	}
	
	// zero length is boring (and a dangerous special case) so get rid of it.
	if (dx == 0 && dy == 0) {
		return false;
	}


	int x, y;         // position tracking for line
	int old_y, old_x; // last good position in line
	if (ABS(dx) > ABS(dy)) {
		// x major
		int ymove = SIGN(dy);                // The step to take with y when error hits 1
		dy = (ABS(dy) << ERRORBITS)/ABS(dx); // The value to add to error each x
		int e = 0;                           // error, fixed point with errorbits bits between 0 and 1.
		    x = x0;                          // x is just the x coordinate
		    y = y0;                          // y is offset into buffer, not the y coordinate (this saves many muls)
		    old_y = y;

		dx = SIGN(dx);                       // we just need the sign of dx
		x1 += dx;
		while (x != x1 ) {

			// Ask the object if it likes the position
			if (!object->is_passable(x,y)) {
				xc = x-dx;
				yc = old_y;
				return true;
			}

			// tally the error and ajust
			e += dy;
			if (e >= (1 << ERRORBITS)) {
				old_y = y;
				y += ymove;
				e -= (1 << ERRORBITS);
			}

			// move x one pixel in the right direction
			x += dx;

		} //while (x != x1) {

		// store last good x position for final check
		old_x = x-dx;
	} else {
		//y major

		int xmove = SIGN(dx);                // The step to take with x when error hits 1 << ERRORBITS
		dx = (ABS(dx) << ERRORBITS)/ABS(dy); // The value to add to error each y
		int e = 0;                           // error, fixed point with ERRORBITS bits between 0 and 1.
		    x = x0;                          // x is just the x coordinate
		    y = y0;                          // y is offset into buffer, not the y coordinate (this saves many muls)
		    old_x = x;

		dy = SIGN(dy);                       // we just need the sign of dy scaled to one pixel up or down
		y1 += dy; //NT
		while (y != y1 ) {

			// Ask the object if it likes the position
			if (!object->is_passable(x,y)) { 
				xc = old_x;
				yc = (y-dy);
				return true;
			}

			// tally the error and ajust
			e += dx;
			if (e >= (1 << ERRORBITS)) {
				old_x = x;
				x += xmove;
				e -= (1 << ERRORBITS);
			}

			// move y one pixel in the right direction
			y += dy;

		} //while (y != y1)

		// store last good y position for final check
		old_y = y-dy;
	}

	// NT the following check seems unecessary, but it can happen (due to roundoff errors?, I've been 
	// too lazy to really figure it out) that the final position in direction of the smaller delta
	// isnt checked, so we have to fall back to the last good position in the line, which is old_x,old_y...
	if(!object->is_passable(xorg,yorg)) { 
		xc = old_x; yc = old_y;
		// just for safety, this should never happen!
		if(!object->is_passable(xc,yc)) { 
			logmsg(lt_error,"The collision check failed - new pos still collides...??? (%d,%d)", xc,yc);
		}
		return true;
	} //D8
	return false;
}

bool World::stoneCollision(int x0, int y0, int x1, int y1, int &xc, int &yc, int radius) {
	// the same as collison, but only checks for stones

	//clamp in the y direction (but not in x, overflowing in x will not segfault :), well do it anyway :)
	if (y1 > map_buffer.get_ysize()-1) y1 = map_buffer.get_ysize()-1;
	if (y0 > map_buffer.get_ysize()-1) y0 = map_buffer.get_ysize()-1;
	if (y1 < 0) y1 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 > map_buffer.get_xsize()-1) x1 = map_buffer.get_xsize()-1;
	if (x0 > map_buffer.get_xsize()-1) x0 = map_buffer.get_xsize()-1;
	if (x1 < 0) x1 = 0;
	if (x0 < 0) x0 = 0;
	int xorg = x1, yorg = y1;

	//get delta values
	int dx = x1-x0;
	int dy = y1-y0;
	//FIXME: is this needed?
	//const int32 *pixels = map_buffer.get_pixels_read();
	//const int mapSizeX = map_buffer.get_xsize();
	// always first check source position, this shouldnt happen, but you never know...
	if (isStone(x0,y0) ) {
		xc = x0;
		yc = y0;
		return true;	
	}
	// zero length is boring (and a dangerous special case) so get rid of it.
	if (dx == 0 && dy == 0) {
		return false;
	}
	int x, y;         // position tracking for line
	int old_y, old_x; // last good position in line
    // XXX Unused Variable
	//int offset;       // precalculated offset to the interesting pixels
	if (ABS(dx) > ABS(dy)) {
		// x major
		int ymove = SIGN(dy);                // The step to take with y when error hits 1
		dy = (ABS(dy) << ERRORBITS)/ABS(dx); // The value to add to error each x
		int e = 0;                           // error, fixed point with errorbits bits between 0 and 1.
		x = x0;                              // x is just the x coordinate
		y = y0;                              // y is offset into buffer, not the y coordinate (this saves many muls)
		old_y = y;
		dx = SIGN(dx);                       // we just need the sign of dx
		x1 += dx;
		while (x != x1 ) {
			//calc the offset to the most interesting pixel
			//offset = x+y*mapSizeX; //saves radius*2 adds.
			// first check center pixel
			if(isStone(x,y) ) {
				xc = x-dx;
				yc = old_y;
				return true;
			}
			// then check side pixels in y direction
			for (int c=1;c<radius;c++) {
				if( (isStone(x,y+c)) || (isStone(x,y-c)) ) {
					xc = x-dx;
					//yc = old_y/map_buffer.get_xsize();
					yc = old_y;
					return true;
				}
			}
			//tally the error and ajust
			e += dy;
			if (e >= (1 << ERRORBITS)) {
				old_y = y;
				y += ymove;
				e -= (1 << ERRORBITS);
			}
			//move x one pixel in the right direction
			x += dx;
		} //while (x != x1) {
		// store last good x position for final check
		old_x = x-dx;
	} else {
		//y major
		int xmove = SIGN(dx);                // The step to take with x when error hits 1 << ERRORBITS
		dx = (ABS(dx) << ERRORBITS)/ABS(dy); // The value to add to error each y
		int e = 0;                           // error, fixed point with ERRORBITS bits between 0 and 1.
		x = x0;                              // x is just the x coordinate
		y = y0;                              // y is offset into buffer, not the y coordinate (this saves many muls)
		old_x = x;
		dy = SIGN(dy);                       // we just need the sign of dy scaled to one pixel up or down
		y1 += dy; //NT
		while (y != y1 ) {
			//calc the offset to the most interesting pixel
			//offset = x+y*mapSizeX; //saves radius*2 adds.
			// first check center pixel
			if(isStone(x,y) ) {
				xc = old_x;
				yc = (y-dy);
				return true;
			}
			// then the side pixels in x dir
			for (int c=1;c<radius;c++) {
				if( (isStone(x+c,y)) || (isStone(x-c,y)) ) {
					xc = old_x;
					//yc = (y-dy)/map_buffer.get_xsize();
					yc = (y-dy);
					return true;
				}
			}
			//tally the error and ajust
			e += dx;
			if (e >= (1 << ERRORBITS)) {
				old_x = x;
				x += xmove;
				e -= (1 << ERRORBITS);
			}
			//move y one pixel in the right direction
			y += dy;
		} //while (y != y1) {
		// store last good y position for final check
		old_y = y-dy;
	}
	if (isStone(xorg, yorg)) {
		xc = old_x; yc = old_y;
		// just for safety, this should never happen!
		if (isStone(xc,yc)) {
			logmsg(lt_error,"The collision check failed - new pos still collides...??? (%d,%d)", xc,yc);
		}
		return true;
	}
	return false;
}

Obj_player_avatar *World::get_player_avatar(int playerId) {
	Obj_player_data *player_data = (Obj_player_data *)get_object(playerId, ot_player_data);
	if (!player_data) {
		logmsg(lt_error,"Unable to get the Obj_player_data in World::get_player_avatar()");
		return NULL;
	}
	Obj_player_avatar *player_avatar = (Obj_player_avatar *)get_object(player_data->getavatarid(),ot_player_avatar);
	if (!player_avatar) {
		logmsg(lt_warning,"Unable to get the Obj_player_avatar in World::get_player_avatar() %i",player_data->getavatarid());
		return NULL;
	}
	return player_avatar;
}

bool World::get_player_pos(int playerId, int &x, int &y) {
	Obj_player_avatar *player_avatar = get_player_avatar(playerId);
	if (!player_avatar) {
		logmsg(lt_warning,"Unable to get the Obj_player_avatar in World::get_player_pos()");
		return false;
	}
	float xf=0;
	float yf=0;
	player_avatar->getpos(xf, yf);
	x = ROUND(xf);
	y = ROUND(yf);
	return true;
}

Obj_base *World::get_object(int objectid, Object_type expected_type) {
	World_objects::iterator i=objects->find(objectid);
	if (i == objects->end()) {
		return NULL;
	}
	//reality check: (if the expected type is ot_error ignore the type check)
	if (i->second->gettype() == expected_type || expected_type == ot_error)
		return i->second;
	else
		return NULL;
}

void World::assert_object_type(int objectid, Object_type expected_type) {
	//FIXME: Why is this function disabled
	return;

	World_objects::iterator i=objects->find(objectid);
	if (i == objects->end()) {
	logmsg(lt_assert,"The object %i was supposed to exist and be a %s (it didn't exist)",objectid,OBJECT_NAMES[expected_type]);
		return;
	}
	if (i->second->gettype() != expected_type)
		logmsg(lt_assert,"The object %i was supposed to exist and be a %s (it was a %s instead)",objectid,OBJECT_NAMES[expected_type],OBJECT_NAMES[i->second->gettype()]);
}


void World::dump_objects() {
	logmsg(lt_debug,"Number of objects: %i",objects->size());
	World_objects::iterator i=objects->begin();
	while (i != objects->end()) {
		char owner_name[80];
		Obj_player_data *owner = (Obj_player_data *)i->second->get_owner_data();
		if (owner)
			strcpy(owner_name,owner->get_player_name());
		else
			sprintf(owner_name," %i ?",i->second->getowner());		
		logmsg(lt_debug,"%i: owner: %s : %s",i->first,owner_name,i->second->getname());
		i++;
	}
}

void World::remove_dead_objects() {
	World_objects::iterator i = objects->begin();
	while (i != objects->end()) {
		if (i->second->isdone()) {
			World_objects::iterator j = i++;
			delete j->second;   //delete payload
			objects->erase(j);  //delete the item itself
		} else
			i++;
	}
}

int World::getobjcount() {
	return objects->size();
}

int World::get_player_count() {
	int count = 0;
	World_objects::iterator i = objects->begin();
	while ( i != objects->end() ) {
		if ( i->second->gettype() == ot_player_avatar ) count ++;
		i++;
	}
	return count;
}

bool World::get_always_reload() {
  return always_reload;
}

Obj_player_avatar *World::get_player_number(int playerId) {
	int count = -1;
	World_objects::iterator i = objects->begin();
	while ( i != objects->end() ) {
		if ( i->second->gettype() == ot_player_avatar )
			count++;
		if ( count == playerId )
			return (Obj_player_avatar *)i->second;
		i++;
	}
	return NULL;
}

bool World::isStone(int x, int y)  { 
	int offset = y*map_buffer.get_xsize() + x; 
	return IS_STONE(map_buffer.get_pixels_read()[offset]); 
}

bool World::isPassable(int x, int y)  { 
	// check for world boundaries, to prevent segfaults
	if (x >= getSizeX() || y >= getSizeY() || x < 0 || y < 0)
		return false;

	int offset = x+y*map_buffer.get_xsize(); 
	return IS_PASSABLE(map_buffer.get_pixels_read()[offset]); 
}
