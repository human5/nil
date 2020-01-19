/***************************************************************************
  loader_source_cpio.h  - New file loader cpio routines.
  --------------------------------------------------------------------------
  begin                : 2004-07-16
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add version of constructor that interprets 
                         memory_arena as CPIO archive.

  last changed         : 2004-08-02
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add filename parameters for nicer errors.

  last changed         : 2004-08-04
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up error reporting.

  last changed         : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up so it compiles after VFS rework.

  last changed         : 2005-03-07
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up so it compiles after VFS rework.

  copyright            : (C) 2004, 2005 by H. Ilari Liusvaara 
                         (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef LOADER_SOURCE_CPIO_H
#define LOADER_SOURCE_CPIO_H

#include "loader.h"
#include "common/vfs/vfs_memory_arena.h"
#include "common/smartpointer/pointer.h"

//! @author H. Ilari Liusvaara
//! @brief CPIO file loading
class Loader_source_cpio : public Loader_source {
public:
	/*! Load cpio file from memory arena.
	    @param dict The dictionary
	    @param arena The memory arena */
	Loader_source_cpio(VFS::Files_dictionary_t dict, 
		pointer::Strong<VFS::Memory_arena> arena);
	//! Frees cpio_arena
	~Loader_source_cpio();
private:
	/*! ???
	    @param dict The dictionary */
	void initialize(VFS::Files_dictionary_t dict);
	//! ???
	pointer::Strong<VFS::Memory_arena> cpio_arena;
};

#endif
