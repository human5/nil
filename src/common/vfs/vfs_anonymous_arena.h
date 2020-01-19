/***************************************************************************
  vfs_anonymous_arena.h  - Anonymous arenas
  --------------------------------------------------------------------------
  begin                : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _vfs_anonymous_arena_h_
#define _vfs_anonymous_arena_h_

#include "common/vfs/vfs_memory_arena.h"
#include "common/smartpointer/pointer.h"
#include <stdexcept>
#include <cstdlib>

namespace VFS
{
/**
 * \brief Allocate new anonymous arena.
 * 
 * This helper routine creates new anonymous arena, downcasts it to be ordinary
 * memory arena, and wraps it in smart pointer.
 * 
 * \param size The size of arena to allocate.
 *
 * \return The new anonymous arena, downcasted to memory arena.
 *
 * \exception std::bad_alloc Not enough memory to allocate the arena.
 * 
 */
pointer::Strong<Memory_arena> new_anonymous_arena(size_t size)
	throw();

/**
 * \brief Resize anonymous arena.
 * 
 * This helper routine creates new arena of specified size and copies contents
 * of existing arena to it. If new arena is longer, the rest is padded with
 * zeroes. If it is shorter, the data is truncated. If old arena is pointer
 * to nowhere, it is assumed to be 0 bytes in length (and thus this routine
 * will allocate new anonymous arena of specified size, filled with zeroes).
 * 
 * \param old The arena to resize.
 * \param size The size of arena to allocate.
 *
 * \return The new anonymous arena, downcasted to memory arena.
 *
 * \exception std::bad_alloc Not enough memory to allocate the arena.
 * 
 */
pointer::Strong<Memory_arena> resize_arena(pointer::Strong<Memory_arena> old,
	size_t size) throw();


/**
 * \brief Class of anonymous arenas.
 *
 * Anonymous arenas are arenas overlaying chunk of memory allocated from heap
 * or by mapping anonymous memory. They can be both read and written, unlike 
 * the rest. On destruction of the arena, the memory is freed.
 * 
 */
class Anonymous_arena : public Memory_arena
{
public:
/**
 * \brief Create new anonymous arena.
 *
 * This constructor takes size of anonymous arena to make and allocates
 * memory chunk of that size and overlays arena on top of that.
 * 
 * \param size The size of arena to allocate.
 *
 * \exception std::bad_alloc Not enough memory to allocate the arena.
 * 
 */
	Anonymous_arena(size_t size) throw();

/**
 *
 * \brief Destructor.
 *
 */
	~Anonymous_arena() throw();
private:
	//!No copy constructor or assignment operators!
	Anonymous_arena(const Anonymous_arena&);
	//!No copy constructor or assignment operators!
	Anonymous_arena& operator = (const Anonymous_arena&);
	
};

}

#endif
