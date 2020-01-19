/***************************************************************************
  vfs_mapped_arena.cpp  - Mapped arenas
  --------------------------------------------------------------------------
  begin                : 2004-07-15
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2004-08-01
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add PROT_WRITE when mapping anonymous arenas. 
                         Previously was totally useless. Also initialize
                         name (so we don't segfault.)

  last changed         : 2004-08-03
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add resize_arena() helper.

  last changed         : 2004-08-05
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Recognize special "/proc/self/fd/<fd> syntax..

  last changed         : 2004-08-06
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add capablity to back arenas by fd.

  last changed         : 2004-08-06
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Report true size of anonymous areas. Current 
                         behaviour is confusing.

  last changed         : 2004-09-03
  by                   : Phil Howlett (phowlett@bigpond.net.au)
  changedescription    : Fixed up a few errors that was preventing
                         compilation in Win32.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add possiblity to make areas with fixed addresses.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Adopt parts as vfs_mapped_arena.cpp.

  copyright            : (C) 2004, 2005 by H. Ilari Liusvaara
                         (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include <cstring>

#include "common/vfs/vfs_mapped_arena.h"
#include "common/console/console.h"
#ifndef _WIN32
#include <sys/mman.h>
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <map>
#include <cerrno>

#ifdef EMULATED_MMAP_ONLY
#undef _POSIX_MAPPED_FILES
#endif

namespace VFS
{

Mmap_failed::Mmap_failed(const std::string& description) throw()
	: runtime_error(description)
{
}

namespace {
	void throw_map_failed(const char* operation, const char* filename, 
		bool oom) 
	{
		const char* err = strerror(errno);

		if(oom)
			err = "Out of memory.";

		size_t bufsize = strlen(operation) + strlen(filename) + 
			strlen(err)  + 11;
		char* buf = (char*)alloca(bufsize);

		strcpy(buf, "Can't ");
		strcat(buf, operation);
		strcat(buf, " \"");
		strcat(buf, filename);
		strcat(buf, "\":");
		strcat(buf, strerror(errno));

		throw Mmap_failed(buf);
	}
}

//Create new object that mmaps given file for read-only.
Mapped_arena::Mapped_arena(const char* mappath) throw()
{
	size_t true_size;
	size_t fake_size;
	void* base_addr;
#ifdef _POSIX_MAPPED_FILES
	int fd = -1;
	struct stat s;

	fd = open(mappath, O_RDONLY);
	if(fd < 0)
		throw_map_failed("open", mappath, false);

	if(fstat(fd, &s)) {
		close(fd);
		throw_map_failed("fstat", mappath, false);
	}

	//Compute sizes.
	fake_size = s.st_size;
	size_t psize = getpagesize();
	true_size = (s.st_size + psize - 1) / psize * psize;

	//Check for too large.
	if(fake_size != (size_t)s.st_size) {
		errno = EFBIG;
		throw_map_failed("mmap", mappath, false);
	}

	base_addr = mmap(NULL, true_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(base_addr == (void*)MAP_FAILED)
	{
		close(fd);
		throw_map_failed("mmap", mappath, false);
	}

	close(fd);
#else
	FILE* fp;
	long len;

	fp = fopen(mappath, "rb");
	if(!fp)
		throw_map_failed("fopen", mappath, false);

	if(fseek(fp, 0, SEEK_END) < 0)
	{
		fclose(fp);
		throw_map_failed("fseek to end", mappath, false);
	}

	if((len = ftell(fp)) < 0)
	{
		fclose(fp);
		throw_map_failed("ftell", mappath, false);
	}

	rewind(fp);

	fake_size = true_size = len;
	base_addr = malloc(len);
	if(!base_addr)
	{
		fclose(fp);
		throw_map_failed("allocate buffer to map", mappath, true);
	}

	if((int)fread((void*)base_addr, 1, len, fp) < (int)len)
	{
		if(ferror(fp))
		{
			fclose(fp);
			free((void*)base_addr);
			throw_map_failed("fread", mappath, false);
		}
	}

	fclose(fp);
#endif
	truesize = true_size;
	initialize(base_addr, fake_size);
}


//Destructor. Frees any resources allocated.
Mapped_arena::~Mapped_arena() throw()
{
#ifdef _POSIX_MAPPED_FILES
	munmap((void*)get_base(), truesize);
#else
	free((void*)get_base());
#endif
}

pointer::Strong<Memory_arena> new_mapped_arena(const char* filename)
	throw()
{
	return pointer::Strong<Memory_arena>(static_cast<Memory_arena*>(
		new Mapped_arena(filename)));
}

}
