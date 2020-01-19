/***************************************************************************
  win32_system.h  -  Header for win32 helper funcions
  --------------------------------------------------------------------------
  begin                : ????-??-??
  by                   : ???
  email                : ???

  last changed         : 2004-05-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  last changed         : 2004-09-03
  by                   : Phil Howlett
  email                : phowlett@bigpond.net.au
  description		   : Added in the define for alloca beacuse win32 doesn't
                         have such a beast. Also fixed the 'group' and 'other'
                         CPIO flags to 0

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Guard the whole thing in #ifdefs

  copyright            : (C) 2004 by Christoph Brill (egore)
  email                : egore@gmx.de

 ***************************************************************************/

#ifndef WIN32_SYSTEM_H
#define WIN32_SYSTEM_H

#ifdef _WIN32

// 2004-05-16, egore, already defined by os_defines.h from MinGW
//#define NOMINMAX

#include <windows.h>
#include <process.h>
#include <io.h>
#include <malloc.h>
#include "common/systemheaders.h"

// a pi constant, e.g. for sounds
#define M_PI    3.14159265358979323846

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

// redefine alloca() here to _alloca()
#ifndef alloca
#define alloca _alloca
#endif

// 'Group' and 'Other' file permissions aren't used on win32, disabling them here 
// without modifying the original code
#define S_IRGRP 0
#define S_IXGRP 0
#define S_IROTH 0
#define S_IXOTH 0

// Disable some annoying warnings
#pragma warning (disable : 4244)
#pragma warning (disable : 4305)
#pragma warning (disable : 4786)
#pragma warning (disable : 4800)
#pragma warning (disable : 4091)

// Used for stat
#define S_ISMT(x)  ((x) & S_IFMT )
#define S_ISDIR(x) ((x) & S_IFDIR)
#define S_ISCHR(x) ((x) & S_IFCHR)
#define S_ISREG(x) ((x) & S_IFREG)

// for nil_map 
#define MAP_SHARED 0
#define MAP_FAILED (void*)-1
#define PROT_READ 0

// own implementation in win32_system.cpp
char *strsep(char **stringp, const char *delim);
#define snprintf _snprintf

#ifndef INT32_DEFINED
#define INT32_DEFINED
typedef unsigned int int32;
#endif

#ifdef __cplusplus
inline void sleep(unsigned long sec) { Sleep((sec)*1000); }
inline void usleep(unsigned long usec) { Sleep((usec)/1000);}
#endif

/*#ifdef __cplusplus
extern "C" 
#endif
	int SDL_main(int argc, char *argv[]);

int NiL_main(int argc, char *argv[]);
#ifdef main
#undef main
#endif
#define main NiL_main
*/

#endif
#endif
