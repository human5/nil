/***************************************************************************
  vfs_fixed_arena.h  - Fixed arenas
  --------------------------------------------------------------------------
  begin                : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _vfs_fixed_arena_h_
#define _vfs_fixed_arena_h_

#include "common/vfs/vfs_memory_arena.h"
#include "common/smartpointer/pointer.h"
#include <stdexcept>
#include <cstdlib>

namespace VFS
{
/**
 * \brief Allocate new fixed arena.
 * 
 * This helper routine creates new fixed arena, downcasts it to be ordinary
 * memory arena, and wraps it in smart pointer.
 * 
 * \param base base address of fixed arena.
 * \param size Size of fixed arena.
 *
 * \return The new fixed arena, downcasted to memory arena.
 *
 * \exception std::bad_alloc Not enough memory to allocate the arena.
 * 
 */
pointer::Strong<Memory_arena> new_fixed_arena(const void* base, size_t size)
	throw(std::bad_alloc);


/**
 * \brief Class of fixed arenas.
 *
 * Fixed arenas are arenas overlaying chunk of process memory space. There
 * are no aliasing checks or freeing actions, so the memory space should be
 * part of data section of the program.
 *
 */
class Fixed_arena : public Memory_arena
{
public:
/**
 * \brief Create new fixed arena.
 *
 * This constructor takes base and size of chunk of process VM space to overlay
 * arena top on.
 *
 * \param base The base address of the arena
 * \param size The size of arena.
 *
 * \exception std::bad_alloc Not enough memory to allocate the arena.
 * 
 */
	Fixed_arena(const void* base, size_t size) throw();

/**
 *
 * \brief Destructor.
 *
 */
	~Fixed_arena() throw();
private:
	//!No copy constructor or assignment operators!
	Fixed_arena(const Fixed_arena&);
	//!No copy constructor or assignment operators!
	Fixed_arena& operator = (const Fixed_arena&);
};

}

#endif
