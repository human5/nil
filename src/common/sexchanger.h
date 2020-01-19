/***************************************************************************
  sexchanger.h  -  Header for code to revert integers
  --------------------------------------------------------------------------
  begin                : 2000-02-01
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-07
  by                   : Nils Thuerey
  email                : n_t@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Include SDL headers via systemheaders.h

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef _SEXCHANGER_H_
#define _SEXCHANGER_H_

#include "systemheaders.h"
//#include "config.h"

// use fast gnu routines if possible
#ifdef HAVE_BYTESWAP_H
#include "byteswap.h"
#endif

inline Uint16 bytesex_change(Uint16 value) {
	return	((value << 8) & 0xff00) | ((value >> 8) & 0x00ff);
}

inline Uint32 bytesex_change(Uint32 value) {
	return ((value << 24) & 0xff000000) | ((value <<  8) & 0x00ff0000) | ((value >>  8) & 0x0000ff00) | ((value >> 24) & 0x000000ff);
}


// some defines to make handling byte order changes easier
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

// this is for machine with e.g. MacOS
#ifdef HAVE_BYTESWAP_H
#define getChangedByteOrder16(x) bswap_16(x)
#define getChangedByteOrder32(x) bswap_32(x)
#else
#define getChangedByteOrder16(x) ((( (x) << 8) & 0xff00) | (( (x) >>8) & 0x00ff))
#define getChangedByteOrder32(x) ((( (x) << 24) & 0xff000000) | (( (x) <<  8) & 0x00ff0000) | (( (x) >>  8) & 0x0000ff00) | (( (x) >> 24) & 0x000000ff))
#endif

inline Uint16 changeByteOrder16(Uint16 &x) { x = getChangedByteOrder16(x); return x; }
inline Uint32 changeByteOrder32(Uint32 &x) { x = getChangedByteOrder32(x); return x; }

#else

// on Linux/Win32 machines nothing has to be done
#define getChangedByteOrder16(x) (x)
#define getChangedByteOrder32(x) (x)
inline Uint16 changeByteOrder16(Uint16 &x) { return x; }
inline Uint32 changeByteOrder32(Uint32 &x) { return x; }

#endif

#endif
