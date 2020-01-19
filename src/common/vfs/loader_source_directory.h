/***************************************************************************
  loader_source_directory.h  - New file loader directory routines.
  --------------------------------------------------------------------------
  begin                : 2004-07-16
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2005-03-07
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up so it compiles after VFS rework.

  copyright            : (C) 2004 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef LOADER_SOURCE_DIRECTORY_H
#define LOADER_SOURCE_DIRECTORY_H

#include "loader.h"

//! @author H. Ilari Liusvaara
//! @brief This class enables loading from a directory
class Loader_source_directory : public Loader_source {
public:
	/*! recursively loads the given directory
	    @param dict The files to go
	    @param file The directory path */
	Loader_source_directory(VFS::Files_dictionary_t dict, 
		const char* file);
	//! Empty destructor
	~Loader_source_directory();
private:
	/*! recursively loads the given directory
	    @param root The directory path
	    @param path The Path relative to 'root' */
	void recurse_directory(const char* root, const char* path);
};

#endif
