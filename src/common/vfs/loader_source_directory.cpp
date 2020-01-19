/***************************************************************************
  loader_source_directory.cpp  - New file loader directory routines.
  --------------------------------------------------------------------------
  begin                : 2004-07-16
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up so it compiles after VFS rework.

  copyright            : (C) 2004-2005 by H. Ilari Liusvaara 
                         (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "common/vfs/loader_source_directory.h"
#include "common/vfs/vfs_mapped_arena.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include <dirent.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>

Loader_source_directory::Loader_source_directory(VFS::Files_dictionary_t dict, 
	const char* file)
	: Loader_source(dict)
{
	recurse_directory(file, "");
}

Loader_source_directory::~Loader_source_directory() {}

void Loader_source_directory::recurse_directory(const char* root, 
	const char* path)
{
	char* this_directory = (char*)alloca(strlen(root) + strlen(path) + 2);
	strcpy(this_directory, root);
	strcat(this_directory, "/");
	strcat(this_directory, path);

	logmsg(lt_debug, "Descending to directory \"%s\".", this_directory);

	//Try to open the directory.
	DIR* directory = opendir(this_directory);
	if(!directory)
	{
		logmsg(lt_error, "Unable to obtain file listing for: \"%s\". "
			"Skipping. Reason: %s.", this_directory, 
			strerror(errno));
		return;
	}
	
	struct dirent* dentry;

	//Obtain listing of all files.
	while((dentry = readdir(directory)) != NULL)
	{
		char* dentry_path = (char*)alloca(strlen(path) + 
			strlen(dentry->d_name) + 2);
		strcpy(dentry_path, path);
		if(*path)
			strcat(dentry_path, "/");
		strcat(dentry_path, dentry->d_name);

		char* dentry_full_path = (char*)alloca(strlen(root) + 
			strlen(dentry_path) + 2);
		strcpy(dentry_full_path, root);
		strcat(dentry_full_path, "/");
		strcat(dentry_full_path, dentry_path);

		struct stat s;
		if(stat(dentry_full_path, &s) < 0)
		{
			logmsg(lt_error, "Can't stat \"%s\", skipping it. "
				"Reason: %s", dentry_full_path, 
				strerror(errno));
			continue;
		}

		if(S_ISREG(s.st_mode))
		{
			pointer::Strong<VFS::Memory_arena> mapper;

			//Regular file, map and add.
			try {
				mapper = VFS::new_mapped_arena(
					dentry_full_path);
				add_file(dentry_path, false, mapper);
			} catch(std::bad_alloc& e) {
				logmsg(lt_error, "Can't add \"%s\", "
					"skipping it due out of memory.", 
					dentry_full_path);
			} catch(VFS::Mmap_failed& e) {
				logmsg(lt_error, "Can't map \"%s\": %s.", 
					dentry_full_path, e.what());
			}
		} else if(S_ISDIR(s.st_mode)) {
			//If name consists of '.' or '..' skip it. 
			//Otherwise recurse.
			if(strcmp(dentry->d_name, ".") && 
				strcmp(dentry->d_name, ".."))
			{
				recurse_directory(root, dentry_path);
			}
		}
	}

	closedir(directory);
}

