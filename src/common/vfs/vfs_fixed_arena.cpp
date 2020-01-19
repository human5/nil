/***************************************************************************
  vfs_fixed_arena.cpp  -  Fixed arenas
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
  changedescription    : Adopt parts as vfs_fixed_arena.cpp.
                         
  copyright            : (C) 2004, 2005 by H. Ilari Liusvaara 
                         (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "common/vfs/vfs_fixed_arena.h"
#include "common/misc.h"
#include "common/console/console.h"
#include <sys/types.h>
#include <fcntl.h>
#include <map>

namespace VFS
{
pointer::Strong<Memory_arena> new_fixed_arena(const void* base, size_t size)
	throw(std::bad_alloc)

{
	return pointer::Strong<Memory_arena>(static_cast<Memory_arena*>(
		new Fixed_arena(base, size)));
}

Fixed_arena::Fixed_arena(const void* base, size_t size) throw()
{
	initialize(base, size);
}

Fixed_arena::~Fixed_arena() throw()
{
}

}


