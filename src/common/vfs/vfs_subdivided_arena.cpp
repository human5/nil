/***************************************************************************
  vfs_subdivided_arena.cpp  - Subdivided arenas
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
  changedescription    : Adopt parts as vfs_subdivided_arena.cpp.
                         
  copyright            : (C) 2004, 2005 by H. Ilari Liusvaara 
                         (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "common/vfs/vfs_subdivided_arena.h"
#include "common/misc.h"
#include "common/console/logmsg.h"

namespace VFS
{

Subdivision_outside_parent::Subdivision_outside_parent(
	const std::string& description) throw()
	: invalid_argument(description)
{
}

pointer::Strong<Memory_arena> new_subdivided_arena(
	pointer::Strong<Memory_arena> parent, size_t offset, size_t size)
	throw()
{
	return pointer::Strong<Memory_arena>(static_cast<Memory_arena*>(
		new Subdivided_arena(parent, offset, size)));
}

Subdivided_arena::Subdivided_arena(pointer::Strong<Memory_arena> parent, 
	size_t offset, size_t size) throw()
{
	//Sanity check.
	if(!parent)
		throw std::invalid_argument("No parent arena in subdivision.");

	//Check bounds of new arena.
	if(size > parent->get_size())
		throw Subdivision_outside_parent("New arena larger than "
			"parent.");

	//Check the offset.
	if(offset > parent->get_size())
		throw Subdivision_outside_parent("New arena offset out of "
			"range.");

	//Check combined effect of size and offset.
	if(offset + size > parent->get_size())
		throw Subdivision_outside_parent("New arena goes off the "
			"end of parent.");

	//Ok, create the arena.
	parent_arena = parent;
	initialize((void*)((char*)parent->get_base() + offset), size);
}

Subdivided_arena::~Subdivided_arena() throw()
{
}

}
