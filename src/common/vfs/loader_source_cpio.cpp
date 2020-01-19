/***************************************************************************
  loader_source_cpio.cpp  - New file loader cpio routines.
  --------------------------------------------------------------------------
  begin                : 2004-07-16
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add version of constructor that interprets 
                         memory_arena as CPIO archive.

  last changed         : 2004-08-02
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Nicer errors. Fix bug when CPIO precheck fails.

  last changed         : 2004-08-04
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Moved lump saving to loader.cpp as it needs special
                         info about internal state of loader.

  last changed         : 2004-08-04
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up error reporting.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : SECURITY: Limit memory allocations necressary for
                         loading a cpio.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-11-11
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : SECURITY: Limit memory allocations necressary for
                         loading a cpio (really this time).

  last changed         : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up so it compiles after VFS rework.

  copyright            : (C) 2004-2005 by H. Ilari Liusvaara 
                         (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "common/vfs/loader_source_cpio.h"
#include "common/vfs/vfs_subdivided_arena.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/exception.h"
#include <map>

#define MAXIMUM_FILES		16384
#define MAXIMUM_NAMESIZE	524288
#define CPIO_FILE_HEADER_SIZE 110
#define CPIO_FILE_HEADER_MODE 1
#define CPIO_FILE_HEADER_LINKS 4
#define CPIO_FILE_HEADER_BODYLEN 6
#define CPIO_FILE_HEADER_NAMELEN 11

typedef unsigned int cpio_header_t[13];

//! this class is thrown when we revieve an invalid cpio file
class invalid_cpio_file : public nil_exception  {
public:
	//! ???
	invalid_cpio_file(const char* ctx, const char* msg, bool sdctx = false,
		bool sdmsg = false);
};

invalid_cpio_file::invalid_cpio_file(const char* ctx, const char* msg, 
	bool sdctx, bool sdmsg) : nil_exception(ctx, msg, sdctx, sdmsg) {}

namespace
{
	//! Parse header of CPIO file (new SVR4 format) from ptr and store the 
	//! reslut to header-parameter.
	void parse_cpio_file_header(const char* ptr, cpio_header_t header)
	{
		//I don't know what the "070701" is, but I presume it some kind
		//of sync pattern.
		//check its existence.
		if(strncmp(ptr, "070701", 6)) {
			char msg[512];

			sprintf(msg, "File 'sync pattern' is bad (archive "
				"corrupt or is not CPIO file.)");
			throw invalid_cpio_file("parse_cpio_file_header", msg, 
				false, true);
		}

		unsigned i, s;
		for(i = 0, s = 6; i < 13; ++i, s += 8)
		{
			char tmp[] = "00000000";
			char* tmpe;
			strncpy(tmp, ptr + s, 8);
			header[i] = strtoul(tmp, &tmpe, 16);
			
			//Validity check.
			if(*tmp == '\0' || *tmpe != '\0' || tmp + 8 != tmpe)
			{
				char msg[512];

				sprintf(msg, "File header contains bad "
					"hexadecimal number \"%s\".", tmp);
				throw invalid_cpio_file(
					"parse_cpio_file_header", msg, false, 
					true);
			}
		}
	}

	//! Compute length of whole entry for file in bytes.
	unsigned compute_entry_size(cpio_header_t header)
	{
		unsigned esize = CPIO_FILE_HEADER_SIZE;

		//Next, take the name and padding for it.
		esize += header[CPIO_FILE_HEADER_NAMELEN];
		if(esize < header[CPIO_FILE_HEADER_NAMELEN])
			goto arithmetic_overflow;
		while(esize & 3)
			++esize;

		if(esize == 0)
			goto arithmetic_overflow;

		//Next, take file contents and padding for it.
		esize += header[CPIO_FILE_HEADER_BODYLEN];
		if(esize < header[CPIO_FILE_HEADER_BODYLEN])
			goto arithmetic_overflow;
		while(esize & 3)
			++esize;

		if(esize == 0)
			goto arithmetic_overflow;
		
		return esize;
arithmetic_overflow:
		throw invalid_cpio_file("compute_entry_size", "Arithmetic "
			"overflow computing entry size.");
	}

	//! Build memory arena for file. Check that entiere file fits first!
	pointer::Strong<VFS::Memory_arena> build_arena_for_file(
		pointer::Strong<VFS::Memory_arena> cpio_arena, 
		unsigned long long foffset, cpio_header_t header)
	{
		//Compute offset of file contents from start of file entry.
		unsigned coffset = CPIO_FILE_HEADER_SIZE;
		coffset += header[CPIO_FILE_HEADER_NAMELEN];
		while(coffset & 3)
			++coffset;

		return VFS::new_subdivided_arena(cpio_arena, 
			foffset + coffset, header[CPIO_FILE_HEADER_BODYLEN]);
	}

	//! Check validity of file name. Check that entiere file fits first!
	void check_file_entry_name_validity(cpio_header_t header,
		 const char* hptr)
	{
		//Name offset.
		unsigned nameoffs = CPIO_FILE_HEADER_SIZE;
		
		//Offset of final character in name.
		int fncoffs = nameoffs + header[CPIO_FILE_HEADER_NAMELEN] - 1;

		//Check that length doesn't go negative.
		if(fncoffs < 0)
			throw invalid_cpio_file("check_file_entry_validity", 
				"Bad filename.");
		
		//Check that final character is NUL.
		if(hptr[fncoffs] != '\0')
			throw invalid_cpio_file("check_file_entry_validity", 
				"Bad filename.");
	}

	//Precheck the file.
	void precheck_file(const char* fptr, size_t filesize)
	{
		unsigned long long files = 0, namesize = 0;

		while(1) {
			cpio_header_t header;
			unsigned esize;			

			//Check that there's space for header.
			if(filesize < CPIO_FILE_HEADER_SIZE)
				throw invalid_cpio_file("precheck_file", 
					"File truncated.");

			//Parse the header.
			parse_cpio_file_header(fptr, header);

			//Check that the file entry fits in file.
			if(filesize < (esize = compute_entry_size(header)))
				throw invalid_cpio_file("precheck_file", 
					"File truncated.");
			
			//Check that the name is valid.
			check_file_entry_name_validity(header, fptr);

			//Okay, the entry is valid. Check for special trailer 
			//entry.
			if(!strcmp(fptr + CPIO_FILE_HEADER_SIZE, "TRAILER!!!"))
				break;

			if(files++ == MAXIMUM_FILES)
				throw invalid_cpio_file("precheck_file", 
					"Too many files.");
			if((namesize += header[CPIO_FILE_HEADER_NAMELEN]) > 
					MAXIMUM_NAMESIZE)
				throw invalid_cpio_file("precheck_file", 
					"List of files too large.");

			//Advance.
			fptr += esize;
			filesize -= esize;
		}

		//Ok, file is prechecked, ready to really parse it.
	}
}

void Loader_source_cpio::initialize(VFS::Files_dictionary_t dict)
{
	char errorbuf[8192];

	try {
		logmsg(lt_debug, "Prechecking CPIO file validity.");
		precheck_file((const char*)cpio_arena->get_base(), 
			cpio_arena->get_size());
	} catch(nil_exception& e) {
		char errorbuf[8192];
		snprintf(errorbuf, 8192, "File fails CPIO file validity check:"
			" %s", e.get_errormessage());
		throw nil_exception("Loader_source_cpio::initialize", errorbuf,
			false, true);
	} catch(...) {
		logmsg(lt_assert, "Not supposed to happen, prechecking file "
			"causes unknown exception to be thrown.");
		exit(1);
	}

	logmsg(lt_debug, "CPIO file passed precheck.");

	const char* fptr = (const char*)cpio_arena->get_base();
	size_t fsize = cpio_arena->get_size();
	size_t foffs = 0;

	while(1) {
		cpio_header_t header;
		unsigned esize;			

		//Parse the header.
		parse_cpio_file_header(fptr, header);


		//Check trailer entry.
		if(!strcmp(fptr + CPIO_FILE_HEADER_SIZE, "TRAILER!!!"))
			break;

		//Scope, so goto is legal.
		{
			//Silently ignore directories.
			if(S_ISDIR(header[CPIO_FILE_HEADER_MODE]))
				goto noadd;

			//Complain about non-regular files (directories are 
			//allowed too.)
			if(!S_ISREG(header[CPIO_FILE_HEADER_MODE])) {
				snprintf(errorbuf, 8192, "Entity \"%s\" is of "
					"unsupported type: Not regular file "
					"or directory. ", 
					fptr + CPIO_FILE_HEADER_SIZE);
				goto throw_error;
			}

			//Complain about multiply-linked files.
			if(header[CPIO_FILE_HEADER_LINKS] != 1) {
				snprintf(errorbuf, 8192, "Entity \"%s\" is of "
					"unsupported type: Multiply linked.", 
					fptr + CPIO_FILE_HEADER_SIZE);
				goto throw_error;
			}

			//Construct the arena for file.
			pointer::Strong<VFS::Memory_arena> arena = 
				build_arena_for_file(cpio_arena, foffs, 
				header);

			//Add the arena to filelist.
			try {
				add_file(fptr + CPIO_FILE_HEADER_SIZE, true,
					arena);
			} catch(...) {
				//Assume this is OOM.
				snprintf(errorbuf, 8192, "Can't add entity "
					"\"%s\": Out of memory.", 
					fptr + CPIO_FILE_HEADER_SIZE);
				goto throw_error;
			}
	
		}
noadd:
		esize = compute_entry_size(header);
		fptr += esize;
		foffs += esize;
		fsize -= esize;
	}
	return;

throw_error:
	//Throwing error deletes the parent object. We just need to get rid 
	//of our arena.
	throw nil_exception("Loader_source_cpio::initialize", errorbuf, false, 
		true);
}

Loader_source_cpio::Loader_source_cpio(VFS::Files_dictionary_t dict, 
	pointer::Strong<VFS::Memory_arena> arena)
	: Loader_source(dict)
{
	cpio_arena = arena;
	initialize(dict);
}

Loader_source_cpio::~Loader_source_cpio()
{
	//Free the mapping of the file.
}
