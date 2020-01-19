/***************************************************************************
  loader.h  -  Header for the class for loading files
 ---------------------------------------------------------------------------
  begin                : 1999-10-17
  by                   : Flemming Frandsen
(dion@swamp.dk

  last changed         : 2004-02-02
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Remove getfilename()-method from Loader-class.
                         Add getrwops()-method to Loader-class (conditional
                         on WITHOUT_SDL).

  last changed         : 2004-07-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Rework to be header for new loader.

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Export expand_path.

  last changed         : 2004-08-05
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : save_lump reports how much space would be needed
                         on failure.

  last changed         : 2004-07-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Rework to be header for new loader.

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Export expand_path.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add method to loader_source so entries can be 
                         released.

  last changed         : 2004-09-22
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add method to load the locations. Addsource to take
                         const char*

  last changed         : 2005-01-14
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up so it compiles after vfs_pathexpander.h
                         changes.

  last changed         : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up so it compiles after VFS rework.

  last changed         : 2005-03-07
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Split parts into vfs_file_table.*

  copyright            : (C) 1999 by Flemming Frandsen (dion@swamp.dk)
                        2004-2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  
 ***************************************************************************/

#ifndef LOADER_H
#define LOADER_H

#include "client/animation/animations.h"
#include "client/fonts/fonts.h"
#include "client/graphics/graphics.h"
#include "common/exception.h"
#include "common/vfs/vfs_memory_arena.h"
#include "common/vfs/vfs_file_table.h"
#include "common/smartpointer/pointer.h"
#include "common/sound/sounds.h"
#include <list>
#include <map>


//! @brief ENOENT exception.
class enoent {};

//! @author Flemming Frandsen
//! @brief A class for loading files
class Loader_source {
public:
	//! ???
	Loader_source(VFS::Files_dictionary_t dict);
	//! ???
	virtual ~Loader_source();
	//! ???
	void release_files();
	/*! ???
	    @return Returns the filecount */
	unsigned get_filecount() { return filecount; }
protected:
	//! ???
	VFS::Files_dictionary_t dictionary;
	//! ???
	void add_file(const char* name, bool mapped,
		pointer::Strong<VFS::Memory_arena> arena);
private:
	//! ???
	pointer::Strong<VFS::File_instance> files;
	//! ???
	pointer::Weak<VFS::File_instance> last_file;
	//! ???
	unsigned filecount;
};

//! @author Flemming Frandsen
//! @brief A class for loading files
class Loader {
public:
	//! Constructor
	Loader();
	//! Destructor
	~Loader();
	/*! This can accept either .lump files or directories as parameters, if a directory
	    is specified, but not found it will try to load a lump file with the same name as
	    the directory, but with .lump added.
	    @param path A lump file or a directory
	    @return Returns >=0 on success */
	int addsource(const char *path);
	/*! adds several sources in one go, separated by a tab-character
	    @param paths A list of tabulator separeted pathes
	    @return Returns >=0 on success */
	int addsources(const char *paths);
	/*! traditional copy; returns false on error.
	    @param name The name of the file
	    @param buffer ???
	    @param buffer_size ???
	    @param offset ???
	    @return Returns true when the file was read*/
	bool fileread(char *name, void *buffer, unsigned int buffer_size, unsigned int offset = 0);
	/*! mmap()'ed direct access; returns a pointer to the file or NULL
	    @param name The name of the file */
	const void *getfile(const char *name);
#ifndef WITHOUT_SDL
	/*! Get the SDL_RWops for file.
	    @param name The name of the file
	    @return The SDL_RWops for this file */
	SDL_RWops *getrwops(const char *name);
#endif
	/*! Gets the size of the file
	   @param name Thename of the file
	   @return Returns -1 on error and the size on success. */
	int filesize(const char *name);
	/*! Saving all the loaded data to a .lump.
	    @param buffer The buffer
	    @param buffer_size The size of the buffer
	    @return returns the number of bytes that would be put into the buffer if it is
	            big enough*/
	int save_lump(char *buffer, int buffer_size);
	//! Highlevel caches, use these to access the kinds of data they manage.
	Graphics graphics;
	//! ???
	Fonts fonts;
	//! ???
	Animations animations;
	//! ???
	Sounds sounds;
protected:
	//! ???
	std::list<Loader_source*> sources;
	//! ???
	VFS::Files_dictionary_t dictionary;
};

/*! Load files from standard places.
    @param loader A handle to a loader
    @param datasetting ??? */
void load_files_standard(Loader& loader, const char* datasetting);

#endif
