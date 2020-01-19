/***************************************************************************
  fluff.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-09
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-03-15
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : If error occurs in drawing message string to console,
                         the error is most likely not transistent, so don't
                         try to draw error message to console, as its very
                         likely that the drawing will fail, leading to 
                         infinite recursion. Add a flag to mark when we are 
                         drawing message to console, and don't draw message to
                         console when that flag is set.

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Remove expand_path. Loader.cpp has the new version.


  last changed         : 2004-08-06
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Don't print previous errors. They are annoying, and
                         mostly unneeded now. Also protect terminal against
                         maliscous control characters.

  last changed         : 2004-03-15
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : If error occurs in drawing message string to console,
                         the error is most likely not transistent, so don't
                         try to draw error message to console, as its very
                         likely that the drawing will fail, leading to 
                         infinite recursion. Add a flag to mark when we are 
                         drawing message to console, and don't draw message to
                         console when that flag is set.

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Remove expand_path. Loader.cpp has the new version.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Remove functions related to logging. logmsg.cpp has the
                         new versions.

  last changed         : 2004-10-24
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add strlcpy and strlcat

  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : header fix, moved out timer stuff
  
  copyright            : (C) 1999 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#include "common/misc.h"
#include "common/systemheaders.h"
#include "client/graphics/raw_surface.h"
#include "common/randomstream.h"

//! set by win32_system for quitting, checked by nil_server/client
bool request_quit = false;

//! team defaults
#define TEAM_NUM 4

//! minimum colors for the teams
Sint32 teamColorsMin[TEAM_NUM] = {
	0x0000AA,
	0xAA0000,
	0x00AA00,
	0x000000 };

//! maximum colors for the teams
Sint32 teamColorsMax[TEAM_NUM] = {
	0x2244FF,
	0xFF4422,
	0x44FF44,
	0x555555 };

//! names for the teams
char *teamNames[TEAM_NUM] = {
	"Blue Bashers",
	"Red Ravers",
	"Green Guys",
	"Dark Dorks" };

//! default weapon init,
// usually the standard values from the weapon configuration
// parameters are used, so these are inited from the 
// corresponding object names
const int weaponinit_default[WEAPON_SLOTS]= {
	ot_weapon_shotgun,
	ot_weapon_railgun,
	ot_weapon_mininuke,
	ot_weapon_homingmissile,
	ot_weapon_jetpack
};
// corresponding object names
const int weaponinit_oldnil[WEAPON_SLOTS]= {
	ot_weapon_shotgun,
	ot_weapon_minigun,
	ot_weapon_rocket,
	ot_weapon_mininuke,
	ot_weapon_jetpack
};

//! determine object type from given name
Object_type ot_from_name(const char *name) {
	for(int i=0; i<ot_error; ++i)
		if(!strcasecmp(OBJECT_NAMES[i], name))
			return (Object_type) i;
	return ot_error;
}

//FIXME: The following functions are useless wrappers, IMO

//! a random stream that can be used by everyone
Random_stream global_rand_stream(time(NULL));

//! get random number between min and max, inclusive min&max! min<=n<=max , be careful with map sizes...
int rnd(int min, int max) {
	return global_rand_stream.get_integer(min,max);
	/* OLD if (min > max)
		return max + (random() % (1+min-max));
	else
		return min + (random() % (1+max-min));*/
}

//! create a random color between the two given ones
unsigned int mkrndcolor(unsigned int color_min, unsigned int color_max) {
	return global_rand_stream.get_color(color_min, color_max);
	/*int r = global_rand_stream.getInteger(GETRED(c1),   GETRED(c2));
	int g = global_rand_stream.getInteger(GETGREEN(c1), GETGREEN(c2));
	int b = global_rand_stream.getInteger(GETBLUE(c1),  GETBLUE(c2));
	int color = ((r & 0xff)<<16) + ((g & 0xff)<<8) + (b & 0xff);
	return color;*/
}

//! make background color for e.g. chat messages
unsigned int makeBackgroundColor(unsigned int player_color) {
	int total = 0;

	total += GETRED(player_color);
	total += GETGREEN(player_color);
	total += GETBLUE(player_color);

	int32 backcol;
	if(total>(255+128)) {
		backcol = PIXEL(0,0,0);
	} else {
		backcol = PIXEL(255,255,255);
		// currently black works best...
		backcol = PIXEL(0,0,0);
	}
	return backcol;
}

//! make blood color with player color, that is randomized a bit
unsigned int mk_bloodcolor(unsigned int player_color) {
	//NT create darker and brighter colors from player color
	int32 color_range = 40;
	int32 dark_color = player_color; 
	int32 dark_red, dark_green, dark_blue;
	if(GETRED(dark_color)>255-color_range) dark_red = 255;
	else dark_red = GETRED(dark_color) + color_range;
	if(GETGREEN(dark_color)>255-color_range) dark_green = 255;
	else dark_green = GETGREEN(dark_color) + color_range;
	if(GETBLUE(dark_color)>255-color_range) dark_blue = 255;
	else dark_blue = GETBLUE(dark_color) + color_range;
	dark_color = PIXEL(dark_red, dark_green, dark_blue);

	int32 light_color = player_color;
	int32 light_red, light_green, light_blue;
	if(GETRED(light_color)<color_range) light_red = 0;
	else light_red = GETRED(light_color) - color_range;
	if(GETGREEN(light_color)<color_range) light_green = 0;
	else light_green = GETGREEN(light_color) - color_range;
	if(GETBLUE(light_color)<color_range) light_blue = 0;
	else light_blue = GETBLUE(light_color) - color_range;
	light_color = PIXEL(light_red, light_green, light_blue);

	return mkrndcolor(dark_color, light_color);
}

//! make blood with color from earth
unsigned int mk_dark_bloodcolor(unsigned int player_color) {
	int32 ered = (GETRED(player_color)+178)/2;
	int32 egreen = (GETGREEN(player_color)+104)/2;
	int32 eblue = (GETBLUE(player_color)+0  )/2;
	int32 player_color2 = PIXEL(ered,egreen,eblue);

	//NT create darker and brighter colors from player color
	int32 color_range = 20;
	int32 dark_color = player_color2; 
	int32 dark_red, dark_green, dark_blue;
	if(GETRED(dark_color)>255-color_range) dark_red = 255;
	else dark_red = GETRED(dark_color) + color_range;
	if(GETGREEN(dark_color)>255-color_range) dark_green = 255;
	else dark_green = GETGREEN(dark_color) + color_range;
	if(GETBLUE(dark_color)>255-color_range) dark_blue = 255;
	else dark_blue = GETBLUE(dark_color) + color_range;
	dark_color = PIXEL(dark_red, dark_green, dark_blue);
	int32 light_color = player_color2;
	int32 light_red, light_green, light_blue;
	if(GETRED(light_color)<color_range) light_red = 0;
	else light_red = GETRED(light_color) - color_range;
	if(GETGREEN(light_color)<color_range) light_green = 0;
	else light_green = GETGREEN(light_color) - color_range;
	if(GETBLUE(light_color)<color_range) light_blue = 0;
	else light_blue = GETBLUE(light_color) - color_range;
	light_color = PIXEL(light_red, light_green, light_blue);

	return mkrndcolor(dark_color, light_color);
}

//-----------------------------------------------------------------------------
// string handling

inline bool is_whitespace(char ch) {
	return (ch == '\t') || (ch == ' ') || (ch == '\n') || (ch == '\r');
}

char *strtrim(char *string) {
	//start by removing trailing whitespace:
	int last_char = strlen(string)-1;
	while (last_char >= 0 && is_whitespace(string[last_char])) {
		string[last_char] = 0;
		last_char--;
	}

	//then remove leading whitespace:
	while (*string && is_whitespace(*string)) {
		string++;
 	}
	return string;
}

size_t strlcpy(char* dst, const char* src, size_t dstsize)
{
	while(dstsize > 1 && *src)
		*(dst++) = *(src++), dstsize--;
	if(dstsize > 0)		//It could be 0 originally.
		*dst = '\0';
	return strlen(src);
}

size_t strlcat(char* dst, const char* src, size_t dstsize)
{
	size_t dstchars = 0;
	size_t srcsize;

	//Specification for strlcat specifies that if first dstsize elements
	//of dst doesn't have NUL, then the routine shall do nothing except
	//returning dstsize + strlen(src).
	while(dstsize > 0 && *dst)
		dst++, dstsize--, dstchars++;

	srcsize = strlen(src);

	//Copy the appended string. strlcpy has just the right schemantics here.
	strlcpy(dst, src, dstsize);
	return dstchars + srcsize;

	
}
