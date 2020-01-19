/***************************************************************************
  loader.cpp  -  New file loader.
  --------------------------------------------------------------------------
  begin                : 2004-07-16
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Enhance expand_path so it can expand '~name' 
                         notation. Also ifndef out name expansion code
                         on win32.

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Export expand_path.

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add message outputs to expand_path.

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add load_file to dispatch file loadings based on
                         type.

  last changed         : 2004-08-02
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up errors to be more understandable.

  last changed         : 2004-08-03
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Move BZIP2 decompression and path expansion 
                         to its own file.

  last changed         : 2004-08-04
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Moved lump saving heare as it needs special
                         info about internal state of loader.

  last changed         : 2004-08-04
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up error reporting.

  last changed         : 2004-08-05
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : save_lump reports how much space would be needed
                         on failure.

  last changed         : 2004-08-05
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : save_lump emits necressary directory entries.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add method to loader_source so entries can be 
                         released.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-09-22
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Implement builtin lumps. Fix bugs when allocations
                         fail. Add routine to preform standard loading.
                         changed addsource to take const char*

  last changed         : 2004-10-24
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Fix warning.

  last changed         : 2004-11-20
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Fix bugs in loading faulty builtin CPIO lumps.

  last changed         : 2004-01-14
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Fix it so it compiles after vfs_pathexpander.h
                         changes. Fix bugs in load_files_standard.

  last changed         : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up so it compiles after VFS rework.

  last changed         : 2005-03-07
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Split parts into vfs_file_table.*

  copyright            : (C) 2004-2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "common/console/logmsg.h"
#include "common/vfs/loader.h"
#include "common/vfs/loader_source_cpio.h"
#include "common/vfs/loader_source_directory.h"
#include "common/vfs/vfs_decompress.h"
#include "common/vfs/vfs_pathexpander.h"
#include "common/vfs/vfs_mapped_arena.h"
#include "common/vfs/vfs_fixed_arena.h"
#include "common/misc.h"
#include <cerrno>
#include <map>
#include <set>

Loader_source::Loader_source(VFS::Files_dictionary_t dict) 
{
	dictionary = dict;
	filecount = 0;
}

Loader_source::~Loader_source()
{
	last_file.nullify();
	//Start the chain...
	files.nullify();
}

void Loader_source::release_files()
{
	cdebug << "Blowing all files from source." << std::endl;

	last_file.nullify();
	//Start the chain...
	files.nullify();
	filecount = 0;
}


//!Add file from this loader source as top-priority version of given file. Accepts responsiblity about arena.
void Loader_source::add_file(const char* name, bool mapped, 
	pointer::Strong<VFS::Memory_arena> arena)
{
	pointer::Strong<VFS::File_instance> new_last;

	new_last = VFS::add_file_instance(dictionary, name, arena,  
		last_file, mapped);

	if(!files)
		files = new_last;
	last_file = new_last;
	filecount++;
}

namespace
{
	//!Load a regular file, trying to guess its format. 
	Loader_source* load_file(VFS::Files_dictionary_t dict, 
		const char* path)
	{
		pointer::Strong<VFS::Memory_arena> arena;
		try {
			//Mapping files to process memory space is much less 
			//harmful as decompressing them, as "decompression 
			//bombs" exist.
			logmsg(lt_debug, "Mapping file \"%s\" to process "
				"memory space.", path);
			arena = VFS::new_mapped_arena(path);
		} catch(VFS::Mmap_failed& e) {
			logmsg(lt_error, "Can't map file \"%s\": %s", path, 
				e.what());
			throw;
		} catch(std::bad_alloc& e) {
			throw nil_exception("load_file", "Out of memory.");
		}

		try {	
			//Try to decompress it first.
			arena = decompress_data(arena);

			//Assume all these take responsiblity about the arena.
			if(arena->get_size() >= 6 && 
				!strncmp((const char*)arena->get_base(), 
					"070701", 6)) {
				logmsg(lt_debug, "\"%s\" is a CPIO archive.", 
					path);
				return new Loader_source_cpio(dict, arena);
			} else {
				throw nil_exception("load_file", "Can not "
					"figure out the type of file.");
			}
		} catch(VFS::Decompression_too_large& e) {
			throw nil_exception("load_file", "Decompression is too"
				"large.");
		} catch(VFS::Decompression_error& e) {
			throw nil_exception("load_file", e.what(), false, 
				true);
		} catch(std::bad_alloc& e) {
			throw nil_exception("load_file", "Out of memory.");
		}
	}

	//!Load a FSO. 
	Loader_source* load_fso(VFS::Files_dictionary_t dict, const char* path)
	{
		Loader_source* source;
		struct stat s;
		if(stat(path, &s) < 0) {
			//Special-case some errors so we can give 
			//understandable messages for most common errors.
			if(errno == EACCES)
				logmsg(lt_error, "You don't have permission "
					"to access \"%s\".", path);
			else if(errno == ENOENT)
				logmsg(lt_error, "\"%s\" does not exist.", 
					path);
			else
				logmsg(lt_error, "Unable to obtain type "
					"of \"%s\": %s", path, 
					strerror(errno));
			return NULL;
		}

		logmsg(lt_message, "Going to load \"%s\".", path);

		try {
			if(S_ISREG(s.st_mode)) {
				logmsg(lt_debug, "\"%s\" is a regular file.", 
					path);
				source = load_file(dict, path);
			} else if(S_ISDIR(s.st_mode)) {
				logmsg(lt_debug, "\"%s\" is a directory.", 
					path);
				source = new Loader_source_directory(dict, 
					path);
			} else {
				throw nil_exception("load_fso", "Is not "
					"regular file or directory");
			}
		} catch(nil_exception& e) {
			logmsg(lt_error, "Can't load \"%s\": %s", path, 
				e.get_errormessage());
			return NULL;
		}
		
		logmsg(lt_message, "Loaded %u files from \"%s\".", 
			source->get_filecount(), path);
		return source;
	}
}

extern char _binary_builtin_cpio_start;
extern char _binary_builtin_cpio_size;

Loader::Loader()
{
	//Create new dictionary for files.
	dictionary = new std::map<VFS::Inplace_alterable, 
		pointer::Weak<VFS::File> >;

	Loader_source* builtin_lump;
	pointer::Strong<VFS::Memory_arena> builtin;
	bool builtin_load = true;

	//Load builtin lump.
	try {
		//OBJDUMP markers are _WEIRD_. The size is stored as address, 
		//_NOT_ as number!
		builtin = VFS::new_fixed_arena(&_binary_builtin_cpio_start, 
			(size_t)&_binary_builtin_cpio_size);
	} catch(...) {
		cerror << "Unable to load builtin CPIO archive: Not enough "
			"memory to create builtin area object." << std::endl;
		builtin_load = false;
		//Don't rethrow.
	}
	try {
		if(builtin_load)
			builtin_lump = new Loader_source_cpio(dictionary, 
				builtin);
	} catch(nil_exception& e) {
		cerror << "Unable to load builtin CPIO archive:" 
			<< e.get_errormessage() << std::endl;
		builtin_load = false;
		//Don't rethrow.
	} catch(std::bad_alloc& e) {
		cerror << "Unable to load builtin CPIO archive: Out of memory."
			<< std::endl;
		builtin_load = false;
		//Don't rethrow.
	}
	try {
		if(builtin_load)
			sources.push_back(builtin_lump);
	} catch(...) {
		logmsg(lt_error, "Unable to add builtin CPIO archive to list "
			"of data sources.");
		//Don't rethrow.
	}

	//Init caches.
	graphics.init(this);
	fonts.init(this);
	animations.init(this);
	sounds.init(this);
}

Loader::~Loader()
{
	//Blow up all data sources.
	for(std::list<struct Loader_source*>::iterator i = sources.begin(); i != sources.end(); ++i)
	{
		cdebug << "Releasing loader_source at " << *i << "." << std::flush;
		(*i)->release_files();
		delete *i;
	}

	//Does the dictionary have any more files? It should not unless something is fucked.
	if(!dictionary->empty()) {
		logmsg(lt_assert, "All data sources for loader are gone but not all files are?");
		exit(1);
	}

	//Delete the dictionary.
	delete dictionary;
}

int Loader::addsource(const char* path)
{
	char path_expansion[8192];

	try {
		VFS::expand_path(path_expansion, sizeof(path_expansion),
			path);
	} catch(std::exception& e) {
		cerror << "Unable to expand the path \"" << path << "\": "
			<< e.what() << "\n";
		return 0;
	}

	Loader_source* loaded_fso = load_fso(dictionary, path_expansion);
	if(loaded_fso == NULL)
		return 0;

	try {
		sources.push_back(loaded_fso);
	} catch(...) {
		logmsg(lt_error, "Unable to add \"%s\" to list of data sources, unloading it.", path);
		delete loaded_fso;
		return 0;
	}
	return 1;
}

int Loader::addsources(const char* paths)
{
	char* tmp = (char*)alloca(strlen(paths) + 1);
	strcpy(tmp, paths);

	while(1) {
		char* path = strsep(&tmp, "\t");
		if(!path || !*path)
			break;

		addsource(path);
	}
	
	return 1;
}

bool Loader::fileread(char* name, void* buffer, unsigned int bufsize, unsigned int offset)
{
	pointer::Weak<VFS::File_instance> instance = 
		lookup_instance(dictionary,name);

	if(!instance)
	{
		logmsg(lt_warning, "File \"%s\" does not exist in any known "
			"data source.", name);
		return false;	//ENOENT.
	}

	//Check size.
	if(bufsize > instance->mapped_at->get_size())
		return false;

	//Check offset.
	if(offset > instance->mapped_at->get_size())
		return false;

	//Check offset + size.
	if(offset + bufsize > instance->mapped_at->get_size())
		return false;

	memmove(buffer, ((char*)instance->mapped_at->get_base()) + offset, 
		bufsize);
	return true;
}

const void* Loader::getfile(char* name)
{
	pointer::Weak<VFS::File_instance> instance = 
		lookup_instance(dictionary,name);

	if(!instance)
	{
		logmsg(lt_warning, "File \"%s\" does not exist in any known "
			"data source.", name);
		return NULL;	//ENOENT.
	}

	return instance->mapped_at->get_base();
}

int Loader::filesize(char* name)
{
	pointer::Weak<VFS::File_instance> instance = 
		lookup_instance(dictionary,name);

	if(!instance)
	{
		logmsg(lt_warning, "File \"%s\" does not exist in any known "
			"data source.", name);
		return -1;	//ENOENT.
	}

	return instance->mapped_at->get_size();
}

#ifndef WITHOUT_SDL
static int nowrite(SDL_RWops* a, const void* b, int c, int d)
{
	return -1;
}

SDL_RWops* Loader::getrwops(char* name)
{
	pointer::Weak<VFS::File_instance> instance = 
		lookup_instance(dictionary,name);

	if(!instance)
	{
		logmsg(lt_warning, "File \"%s\" does not exist in any known "
			"data source.", name);
		return NULL;	//ENOENT.
	}

	//Do some dirty hackery: SDL_RWFromMem requires first argument to be
	//modifiable, but we can't allow that. So cast constness away (yuck!),
	//and then override the write method, so nobody can modify it.
	SDL_RWops* ops = SDL_RWFromMem(const_cast<void*>(instance->mapped_at->
		get_base()), instance->mapped_at->get_size());
	if(ops == NULL)
	{
		logmsg(lt_error, "SDL_RWFromMem() Failed. Out of memory?");
		return NULL;
	}
	ops->write = nowrite;

	return ops;
}
#endif

//Defaults for some of CPIO fields.
unsigned int cpio_defaults[13] = 
{
	0x00000000,	//Inode, can't be fixed.
	S_IFREG | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,	//Mode, fixed.
	0x00000000,	//UID, fixed.
	0x00000000,	//GID, fixed.
	0x00000001,	//Links, fixed.
	0x4017E861,	//Checksum???, fixed.
	0x00000000,	//Length, can't be fixed.
	0x00000000,	//Dev major, fixed.
	0x00000001,	//Dev minor, fixed.
	0x00000000,	//Dev node major, fixed.
	0x00000000,	//Dev mode minor, fixed.
	0x00000000,	//Filename length, can't be fixed.
	0x00000000,	//Zero, fixed.
};

//Defaults for some of CPIO fields for directories.
unsigned int cpio_defaults_directory[13] = 
{
	0x00000000,	//Inode, can't be fixed.
	S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,	//Mode, fixed.
	0x00000000,	//UID, fixed.
	0x00000000,	//GID, fixed.
	0x00000001,	//Links, fixed.
	0x4017E861,	//Checksum???, fixed.
	0x00000000,	//Length, fixed.
	0x00000000,	//Dev major, fixed.
	0x00000001,	//Dev minor, fixed.
	0x00000000,	//Dev node major, fixed.
	0x00000000,	//Dev mode minor, fixed.
	0x00000000,	//Filename length, can't be fixed.
	0x00000000,	//Zero, fixed.
};

//Field offsets.
#define CPIO_F_INODE		0
#define CPIO_F_LENGTH		6
#define CPIO_F_NAMELEN		11

#define CPIO_HEADERLEN		(13 * 8 + 6)

namespace
{
	//!Memcpy what fits and advance dest.
	int fit_memcpy(char** dest, const char* src, int len, int emitted, int tbufsize)
	{
		int olen = len;
		if(emitted + len > tbufsize)
			len = tbufsize - emitted;
		
		if(len > 0)
			memmove(*dest, src, len);

		*dest = *dest + len;

		return emitted + olen;
	}

	//!strnpy what fits and advance dest.
	int fit_strncpy(char** dest, const char* src, int len, int emitted, int tbufsize)
	{
		int olen = len;
		if(emitted + len > tbufsize)
			len = tbufsize - emitted;
		
		if(len > 0)
			strncpy(*dest, src, len);

		*dest = *dest + len;

		return emitted + olen;
	}
}

int Loader::save_lump(char* buf, int bufsize)
{
	std::set<std::string> directories_set;
	unsigned int inode = 0;
	char nullpad = 0;
	int emitted = 0;
	char headerbuffer[CPIO_HEADERLEN + 1];
	unsigned int headervalues[13];

	logmsg(lt_debug, "Composing directory set...");

	//First, do a prescan pass in which all directories needed are 
	//identified.
	for(std::map<VFS::Inplace_alterable, pointer::Weak<VFS::File> 
		>::iterator i = dictionary->begin(); i != dictionary->end(); 
		++i) {

		const char* fname = i->first.get_string();
		char* fnamecpy = (char*)malloc(strlen(fname) + 1);
		char* tmp = fnamecpy;
		char* last_slash;
		if(!fnamecpy) 
			throw std::bad_alloc();
		
		strcpy(fnamecpy, fname);

		//Just in case...
		if(*tmp == '/')
			tmp++;
			
		while((last_slash = strrchr(tmp, '/'))) {
			*last_slash = '\0';
			std::string s(tmp);
			try {
				directories_set.insert(s);
			} catch(...) {
				free(fnamecpy);
				throw;
			}
		}
	
		free(fnamecpy);
	}

	logmsg(lt_debug, "Emiting directory set...");

	//Then emit all directory entries needed. Sets order entries 
	//lexiographically, so parents occur before
	//children (preorder transversal).
	for(std::set<std::string>::iterator i = directories_set.begin(); 
		i != directories_set.end(); ++i) {

		inode++;
		for(unsigned k = 0; k < 13; k++)
			headervalues[k] = cpio_defaults_directory[k];

		headervalues[CPIO_F_INODE] = inode;
		headervalues[CPIO_F_NAMELEN] = i->length() + 1;		

		sprintf(headerbuffer, "070701%08x%08x%08x%08x%08x%08x%08x%08x"
			"%08x%08x%08x%08x%08x",
			headervalues[0], headervalues[1], headervalues[2], 
			headervalues[3], headervalues[4], headervalues[5], 
			headervalues[6], headervalues[7], headervalues[8], 
			headervalues[9], headervalues[10], headervalues[11], 
			headervalues[12]);

		emitted = fit_strncpy(&buf, headerbuffer, CPIO_HEADERLEN, 
			emitted, bufsize);

		//Copy the name.
		emitted = fit_strncpy(&buf, i->c_str(), 
			headervalues[CPIO_F_NAMELEN], emitted, bufsize);

		//Pad it.
		while(emitted & 3)
			emitted = fit_strncpy(&buf, &nullpad, 1, emitted, 
				bufsize);
	}

	logmsg(lt_debug, "Emiting file set...");
	
	for(std::map<VFS::Inplace_alterable, pointer::Weak<VFS::File> 
		>::iterator i = dictionary->begin(); i != dictionary->end(); 
		++i) {

		pointer::Weak<VFS::File> filp = i->second;
		const char* fname = i->first.get_string();

		inode++;
		for(unsigned k = 0; k < 13; k++)
			headervalues[k] = cpio_defaults[k];

		headervalues[CPIO_F_INODE] = inode;

		if(!filp->instances) {
			//Bad.
			logmsg(lt_assert, "File header with no instances?");
			exit(1);
		}

		headervalues[CPIO_F_LENGTH] = filp->instances->mapped_at->
			get_size();
		headervalues[CPIO_F_NAMELEN] = strlen(fname) + 1;
		sprintf(headerbuffer, "070701%08x%08x%08x%08x%08x%08x%08x"
			"%08x%08x%08x%08x%08x%08x",
			headervalues[0], headervalues[1], headervalues[2], 
			headervalues[3], headervalues[4], headervalues[5], 
			headervalues[6], headervalues[7], headervalues[8], 
			headervalues[9], headervalues[10], headervalues[11], 
			headervalues[12]);

		emitted = fit_strncpy(&buf, headerbuffer, CPIO_HEADERLEN, 
			emitted, bufsize);

		//Copy the name.
		emitted = fit_strncpy(&buf, fname, 
			headervalues[CPIO_F_NAMELEN], emitted, bufsize);

		//Pad it.
		while(emitted & 3)
			emitted = fit_strncpy(&buf, &nullpad, 1, emitted, 
				bufsize);

		//Copy the actual contents of file.
		emitted = fit_memcpy(&buf, 
			(const char*)filp->instances->mapped_at->get_base(), 
			headervalues[CPIO_F_LENGTH], emitted, bufsize);

		//Pad it. We pad at end instead of beginning as TRAILER!!! 
		//needs padding too.
		while(emitted & 3)
			emitted = fit_strncpy(&buf, &nullpad, 1, emitted, 
				bufsize);

	}

	//Copy the entiere entry (strncpy nul-pads)!
	emitted = fit_strncpy(&buf, "0707010000000000000000000000000000000"
		"000000001000000000000000000000000000000000000000"
		"0000000000000000b00000000TRAILER!!!", CPIO_HEADERLEN + 14, 
		emitted, bufsize);

	//Pad it.
	while(emitted & 512)
		emitted = fit_strncpy(&buf, &nullpad, 1, emitted, bufsize);

	return emitted;
}

#define NIL_POSTFIX "nil"
#define DEFAULT_XDG_DATA_HOME "~/.local/share"
#define DEFAULT_XDG_DATA_DIRS "/usr/local/share:/usr/share"

void load_files_standard(Loader& loader, const char* datasetting)
{
	//No XDG on WIN32.
#ifndef _WIN32
	const char* xdg_data_home = getenv("XDG_DATA_HOME");
	const char* xdg_data_dirs = getenv("XDG_DATA_DIRS");

	if(xdg_data_home == NULL || *xdg_data_home == '\0')
		xdg_data_home = DEFAULT_XDG_DATA_HOME;
	if(xdg_data_dirs == NULL || *xdg_data_dirs == '\0')
		xdg_data_dirs = DEFAULT_XDG_DATA_DIRS;

	//Yuck. Due to VFS ordering, we must load from least to most important 
	//and scan backwards.
	char* xdg_data_dirs_copy = (char*)alloca(strlen(xdg_data_dirs) + 1);
	strcpy(xdg_data_dirs_copy, xdg_data_dirs);
	
	char* last_entry;
	while(1)
	{
		bool do_exit = false;
		last_entry = strrchr(xdg_data_dirs_copy, ':');
		if(last_entry == NULL)
		{
			last_entry = xdg_data_dirs_copy;
			do_exit = true;
		}

		char* source = (char*)alloca(strlen(last_entry) + 
			strlen(NIL_POSTFIX) + 2);
		//If do_exit == false, last_entry includes the ':'. Strip it
		//off.
		strcpy(source, last_entry + (do_exit ? 0 : 1));
		strcat(source, "/");
		strcat(source, NIL_POSTFIX );

		loader.addsource(source);
		if(do_exit)
			break;
		else
			*last_entry = '\0';
	}

	//Finally add the XDG_DATA_HOME entry.
	char* source = (char*)alloca(strlen(xdg_data_home) + 
		strlen(NIL_POSTFIX) + 2);
	strcpy(source, xdg_data_home);
	strcat(source, "/");
	strcat(source, NIL_POSTFIX );
	loader.addsource(source);
#endif
	//Add directories pointed by data setting.
	loader.addsources(datasetting);	
}
