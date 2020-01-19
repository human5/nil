/***************************************************************************
  vfs_mapped_arena.h  - Mapped arenas
  --------------------------------------------------------------------------
  begin                : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _vfs_mapped_arena_h_
#define _vfs_mapped_arena_h_

#include "common/vfs/vfs_memory_arena.h"
#include "common/smartpointer/pointer.h"
#include <stdexcept>
#include <cstdlib>

namespace VFS
{
/**
 * \brief Exception thrown when memory mapping file fails for some reason.
 *
 * This exception class is thrown when mapping file fails.
 *
 */
class Mmap_failed : public std::runtime_error
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
	Mmap_failed(const std::string& description) 
		throw();
};

/**
 * \brief Memory map file.
 * 
 * This helper routine creates new mapped arena mapping specified file, 
 * downcasts it to be ordinary memory arena, and wraps it in smart pointer.
 * 
 * \param filename The name of file to map, given in OS-dependent notation.
 *
 * \return The new mapped arena mapping specified file, downcasted to memory 
 *	arena.
 *
 * \exception std::bad_alloc Not enough memory to complete operation.
 * \exception Mmap_failed Mapping file failed for some reason.
 * 
 */
pointer::Strong<Memory_arena> new_mapped_arena(const char* filename)
	throw();


/**
 * \brief Class of mapped arenas.
 *
 * Mapped arenas are arenas overlaying chunk of memory space containing
 * mapping of some file. They are read-only. On destruction of the arena
 * the memory space is unmapped. If OS doesn't support memory mapping, memory
 * mapping is emulated by allocating chunk of memory large enough to hold
 * the contents and then reading the contents to that buffer.
 * 
 */
class Mapped_arena : public Memory_arena
{
public:
/**
 * \brief Create new mapped arena.
 *
 * This constructor takes name of file and maps (possibly faking it if
 * necressary) it overlaying arena on to the mapping.
 * 
 * \param filename The name of file to map in OS-depenedent notation.
 *
 * \exception std::bad_alloc Not enough memory to complete operation.
 * \exception Mmap_failed Mapping file failed for some reason.
 * 
 */
	Mapped_arena(const char* filename) throw();

/**
 *
 * \brief Destructor.
 *
 */
	~Mapped_arena() throw();
private:
	//!No copy constructor or assignment operators!
	Mapped_arena(const Mapped_arena&);
	//!No copy constructor or assignment operators!
	Mapped_arena& operator = (const Mapped_arena&);
	//!True size of arena, used when freeing.
	size_t truesize;
};

}

#endif
