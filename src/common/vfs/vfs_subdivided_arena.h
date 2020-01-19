/***************************************************************************
  vfs_subdivided_arena.h  - Subdivided arenas
  --------------------------------------------------------------------------
  begin                : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _vfs_subdivided_arena_h_
#define _vfs_subdivided_arena_h_

#include "common/vfs/vfs_memory_arena.h"
#include "common/smartpointer/pointer.h"
#include <stdexcept>
#include <cstdlib>

namespace VFS
{
/**
 * \brief Exception thrown when subdivided arena would go outside its parent.
 *
 * This exception class is thrown when subdivision arena created would contain
 * chunks of memory space outside its parent.
 *
 */
class Subdivision_outside_parent : public std::invalid_argument
{
public:
/**
 * \brief Constructor taking message string.
 *
 * This constructor constructs exception, taking exception description string
 * directly as parameter.
 *
 * \param description The exception description.
 *
 * \exception std::bad_alloc Not enough memory to copy the exception 
 *	description.
 * 
 */
	Subdivision_outside_parent(const std::string& description) 
		throw(std::bad_alloc);
};

/**
 * \brief Subdivide existing arena.
 * 
 * This helper routine creates new subdivided arena encompassing specified
 * poritions of its specified parent, downcasts it to be ordinary memory 
 * arena, and wraps it in smart pointer. 
 * 
 * \param parent The parent arena.
 * \param offset Offset from start of parent arena to start of subdivision.
 * \param size Size of the new arena.
 *
 * \return The new subdivision of parent arena, downcasted to memory arena.
 *
 * \exception std::bad_alloc Not enough memory to complete operation.
 * \exception Subdivision_outside_parent The new arena would go outside limits
 *	of its parent.
 * \exception std::invalid_argument Parent arena is pointer to nowhere.
 * 
 */
pointer::Strong<Memory_arena> new_subdivided_arena(
	pointer::Strong<Memory_arena> parent, size_t offset, size_t size)
	throw(std::bad_alloc, Subdivision_outside_parent, 
	std::invalid_argument);


/**
 * \brief Class of subdivided arenas.
 *
 * Subdivided arenas are arenas overlaying parts of other arenas pinning them 
 * into memory. That is, arena is not freed until all its subdivisions are
 * freed. 
 * 
 */
class Subdivided_arena : public Memory_arena
{
public:
/**
 * \brief Create new subdivided arena.
 *
 * This constructor takes pointer to parent arena, offset and size, creating
 * arena that pins the parent arena, is of specified size, and begins at 
 * specified offset from the parent arena. The parent arena is pinned in 
 * memory.
 * 
 * \param parent The parent arena.
 * \param offset Offset from start of parent arena to start of subdivision.
 * \param size Size of the new arena.
 *
 * \exception std::bad_alloc Not enough memory to complete operation.
 * \exception Subdivision_outside_parent The new arena would go outside limits
 *	of its parent.
 * \exception std::invalid_argument Parent arena is pointer to nowhere.
 * 
 */
	Subdivided_arena(pointer::Strong<Memory_arena> parent, size_t offset, 
		size_t size) throw(std::bad_alloc, Subdivision_outside_parent, 
		std::invalid_argument);

/**
 *
 * \brief Destructor.
 *
 */
	~Subdivided_arena() throw();
private:
	//!Pinned parent arena.
	pointer::Strong<Memory_arena> parent_arena;
	//!No copy constructor or assignment operators!
	Subdivided_arena(const Subdivided_arena&);
	//!No copy constructor or assignment operators!
	Subdivided_arena& operator = (const Subdivided_arena&);

};

}

#endif
