/***************************************************************************
  vfs_memory_arena.cpp  -  Base class of memory arenas.
  --------------------------------------------------------------------------
  begin                : 2004-07-15
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add PROT_WRITE when mapping anonymous arenas. 
                         Previously was totally useless. Also initialize
                         name (so we don't segfault.)

  last changed         : 2004-08-03
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add resize_arena() helper.

  last changed         : 2004-08-05
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Recognize special "/proc/self/fd/<fd> syntax..

  last changed         : 2004-08-06
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add capablity to back arenas by fd.

  last changed         : 2004-08-06
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Report true size of anonymous areas. Current 
                         behaviour is confusing.

  last changed         : 2004-09-03
  by                   : Phil Howlett (phowlett@bigpond.net.au)
  changedescription    : Fixed up a few errors that was preventing
                         compilation in Win32.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add possiblity to make areas with fixed addresses.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.
                         
  last changed         : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Adapt parts as vfs_memory_arena.cpp

  copyright            : (C) 2004, 2005 by H. Ilari Liusvaara 
                         (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "common/vfs/vfs_memory_arena.h"
#include "common/misc.h"
#include "common/console/console.h"
#include <sys/types.h>
#include <fcntl.h>

namespace VFS
{
void Memory_arena::initialize(const void* base, size_t size) throw()
{
	if(arena_initialized)
		cassert << "Attempted to initialize arena twice!";
	arena_initialized = true;
	arena_base = base;
	arena_size = size;
}

Memory_arena::Memory_arena() throw()
{
	arena_initialized = false;
}

Memory_arena::~Memory_arena() throw()
{
}

void Memory_arena::finalize() throw()
{
}


const void* Memory_arena::get_base() throw()
{
	if(!arena_initialized)
		cassert << "Attempted to get base address of uninitialzed "
			"arena!";
	return arena_base;
}

size_t Memory_arena::get_size() throw()
{
	if(!arena_initialized)
		cassert << "Attempted to get size of uninitialzed "
			"arena!";
	return arena_size;
}

}
