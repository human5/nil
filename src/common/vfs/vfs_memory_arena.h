/***************************************************************************
  vfs_memory_arena.h  - Base class of memory arenas.
  --------------------------------------------------------------------------
  begin                : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _vfs_memory_arena_h_
#define _vfs_memory_arena_h_

#include "common/smartpointer/pointer.h"
#include <stdexcept>
#include <cstdlib>

namespace VFS
{
/**
 * \brief Memory arenas
 *
 * This is the base class of all memory arena types. Memory arenas are chunks
 * of process virtual memory address space.
 *
 * Memory_arena has finalize()-method so it and all its subclasses can be
 * put into smart pointers. Furthermore, the finalize()-method does nothing,
 * so as far as Memory_arena is concerned, there is no diffrence between strong
 * and weak pointers.
 *
 * For Memory_arena, copy constructor and assignment operators make no sense
 * so they may not be used. This is case for all its subclasses too.
 *
 */
class Memory_arena
{
public:
/**
 * \brief Get base address of the arena.
 *
 * This function gets the base address of the arena.
 * 
 * \return The base address.
 *
 */
const void* get_base() throw();

/**
 * \brief Get size of the arena.
 *
 * This function gets the size of the arena.
 * 
 * \return The size.
 *
 */
size_t get_size() throw();

/**
 * \brief Virtual destructor.
 *
 */
	virtual ~Memory_arena() throw();

	//!Finalizer.
	void finalize() throw();
protected:
/**
 * \brief Construct new uninitialized arena.
 *
 * This constructor allocates new memory arena that is uninitialized.
 *
 */
	Memory_arena() throw();

/**
 * \brief Initialize the arena.
 *
 * This method initializes the memory arena parameters. It must be called
 * before calling any public methods of Memory_arena(), and must be called
 * only once.
 *
 * \param base The base address of the arena.
 * \param size The size of the arena.
 *
 */
	void initialize(const void* base, size_t size) throw();
private:
	//!Has this arena been initialized?
	bool arena_initialized;
	//!Base address of the arena.
	const void* arena_base;
	//!Size of the arena.
	size_t arena_size;
	//!No copy constructor or assignment operators!
	Memory_arena(const Memory_arena&);
	//!No copy constructor or assignment operators!
	Memory_arena& operator = (const Memory_arena&);

};

}

#endif
