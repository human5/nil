/***************************************************************************
  systemheaders.h  -  includes all external include files
  --------------------------------------------------------------------------
  begin                : 2000-01-10
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-05-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Add SDL headers here.
  
  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/
/***************************************************************************
  Note: Individual source files don't have to include external files; it
  should also be easier to figure out which files to replace if something
  blows up.
 ***************************************************************************/

#ifndef SYSTEMHEADERS_H
#define SYSTEMHEADERS_H

// zlib
#include <zlib.h>

//POSIX.1 | ANSI C
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>
#include <iostream>
#include <signal.h>

//POSIX.1b

#ifdef _WIN32
	#include "common/win32_system.h"
#else
	#include <sys/mman.h>
	#include <unistd.h>
#endif
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//POSIX threads
#include <pthread.h>
#include <semaphore.h>

//misc, 4.4BSD
#ifndef _WIN32
	#include <sys/time.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <netdb.h>
	#include <pwd.h>
#endif

//SDL.
#ifndef WITHOUT_SDL
	#include "SDL.h"
	#include "SDL_mixer.h"
	#include "SDL_main.h"
	#include "SDL_types.h"
	#include "SDL_ttf.h"
	#include "SDL_image.h"
	#include "SDL_endian.h"
#else
	//These come up so often it deserves it even without SDL.
	typedef unsigned Uint32;
	typedef signed Sint32;
	typedef unsigned short Uint16;
	typedef signed short Sint16;
	typedef unsigned char Uint8;
	typedef signed char Sint8;
#endif


//#include "zmalloc.h"


/*
#ifdef MEMORY_DEBUG
#define DEBUG_NEW new(__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#define new DEBUG_NEW


#ifdef MEMORY_DEBUG
inline void * operator new(unsigned int size, const char *file, int line) {
	void *ptr = (void *)malloc(size);
	AddTrack((int)ptr, size, file, line);
	return(ptr);
}

inline void operator delete(void *p) {
	RemoveTrack((int)p);
	free(p);
}
#endif

typedef struct {
	int	address;
	int	size;
	char	file[64];
	int	line;
} ALLOC_INFO;

typedef list<ALLOC_INFO*> AllocList;

AllocList *allocList;

void AddTrack(int addr,  int asize,  const char *fname, int lnum)
{
	ALLOC_INFO *info;

	if(!allocList) {
		allocList = new(AllocList);
	}

	info = new(ALLOC_INFO);
	info->address = addr;
	strncpy(info->file, fname, 63);
	info->line = lnum;
	info->size = asize;
	allocList->insert(allocList->begin(), info);
};

void RemoveTrack(int addr) {
	AllocList::iterator i;

	if(!allocList)
		return;
	for(i = allocList->begin(); i != allocList->end(); i++) {
		if((*i)->address == addr) {
			allocList->remove((*i));
			break;
		}
	}
};

void DumpUnfreed() {
	AllocList::iterator i;
	int totalSize = 0;
	char buf[1024];

	if(!allocList)
		return;

	for(i = allocList->begin(); i != allocList->end(); i++) {
		sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n",
			(*i)->file, (*i)->line, (*i)->address, (*i)->size);
		OutputDebugString(buf);
		totalSize += (*i)->size;
	}
	sprintf(buf, "-----------------------------------------------------------\n");
	OutputDebugString(buf);
	sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
	OutputDebugString(buf);
};

*/

#endif
