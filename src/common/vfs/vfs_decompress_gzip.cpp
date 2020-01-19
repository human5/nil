/***************************************************************************
  vfs_decompress_gzip.cpp  -  VFS GZIP decompressor.
  --------------------------------------------------------------------------
  begin                : 2004-08-04
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2004-08-04
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up error reporting.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-11-20
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Fix double-free vulernablity

  last changed         : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Adapt as vfs_decompress_gzip.cpp

  copyright            : (C) 2004, 2005 by H. Ilari Liusvaara 
                         (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "common/console/console.h"
#include "common/vfs/vfs_decompress_gzip.h"
#include "common/vfs/vfs_anonymous_arena.h"
#include "common/misc.h"

namespace VFS
{
namespace
{
	//Check header of GZIP file. Returns length of header.
	size_t check_header(pointer::Strong<Memory_arena> arena) throw(
		Decompression_error)
	{
		const unsigned char* file = (const unsigned char*)
			arena->get_base();

		size_t headerlen = 10;
		size_t arenasize = arena->get_size();
		size_t max_textlength;
		bool has_hcrc;
		bool has_extra;
		bool has_filename;
		bool has_comment;

		cdebug << "Checking header length of gzip file...\n";

		if(arenasize < 18) 
			goto is_truncated;

		//Check ID fields.
		if(file[0] != 037 || file[1] != 0213)
		  	throw Decompression_error("Incorrect magic (not gzip "
				"compressed).");

		//Check method.
		if(file[2] != 8) 
		  	throw Decompression_error("Unknown algorithm.");

		//Check flags
		if(file[3] & 0x1F != file[3]) 
			throw Decompression_error("Unknown flags set.");

		//Extract flags.
		has_hcrc = file[3] & 2;
		has_extra = file[3] & 4;
		has_filename = file[3] & 8;
		has_comment = file[3] & 16;

		if(has_extra) {
			if(arenasize < 20) 
				goto is_truncated;
	
			unsigned xlen = file[11] + 256 * file[12];

			if(arenasize < 20 + xlen)
				goto is_truncated;

	
			headerlen = 12 + xlen;
		}

		max_textlength = arenasize - headerlen - 8;

		if(has_filename)
		{
			for(; max_textlength < arenasize; headerlen++, 
					max_textlength--)
				if(file[headerlen] == 0)
					break;

			if(max_textlength >= arenasize)
				goto is_truncated;

			max_textlength--;
			headerlen++;
		}

		if(has_comment)
		{
			for(; max_textlength < arenasize; headerlen++, 
					max_textlength--)
				if(file[headerlen] == 0)
					break;

			if(max_textlength >= arenasize)
				goto is_truncated;

			max_textlength--;
			headerlen++;
		}

		if(has_hcrc)
			headerlen += 2;

		if(headerlen > arenasize - 8)
			goto is_truncated;

		cdebug << "file has " << headerlen <<" byte headers.\n";

		return headerlen;
is_truncated:
	  	throw Decompression_error("Compressed data truncated.");
	}
}

#define MAX_UNCOMPRESSED_SIZE (8 * 1024 * 1024)

namespace
{
	void* intr_z_alloc(void* x, unsigned int items, unsigned int size)
	{
		unsigned int toalloc = items * size;
		if(toalloc / size != items)
			return NULL;

		return calloc(toalloc, 1);
	}

	void intr_z_free(void* x, void* ptr)
	{
		free(ptr);
	}

	const char* zlib_errorstring(int error)
	{
		switch(error) {
			case Z_OK:
				return "No error.";
			case Z_STREAM_END:
				return "End of compressed stream.";
			case Z_ERRNO:
				return strerror(errno);
			case Z_STREAM_ERROR:
				return "Stream error.";
			case Z_DATA_ERROR:
				return "File is corrupt.";
			case Z_MEM_ERROR:
				return "Out of memory.";
			case Z_BUF_ERROR:
				return "Insufficent buffer size.";
			case Z_VERSION_ERROR:
				return "Zlib version mismatch.";
			default:
				return "Unknown error.";
		}
	}
}

pointer::Strong<Memory_arena> decompress_gzip(
	pointer::Strong<Memory_arena> data) throw()
{
	size_t hsize;
	z_stream stream;

	hsize = check_header(data);

	//Last four bytes of arena are uncompressed size. Before that is CRC32.
	const unsigned char* file = (const unsigned char*)data->get_base();
	size_t arenasize = data->get_size();
	size_t fsize = file[arenasize - 4] | (file[arenasize - 3] << 8) |
		 (file[arenasize - 2] << 16) | (file[arenasize - 1] << 24);
	unsigned crc_correct = file[arenasize - 8] | (file[arenasize - 7] << 8)
		|  (file[arenasize - 6] << 16) | (file[arenasize - 5] << 24);
	unsigned crc_computed = crc32(0, 0, 0);

	cdebug << arenasize << " bytes will be decompressed into " << fsize
		<< " bytes.\n";

	if(fsize > MAX_UNCOMPRESSED_SIZE)
		throw Decompression_too_large("Uncompressed size is too "
			"large.");

	const unsigned char* deflate_begin = file;
	size_t deflate_length = arenasize;

	//Allocate buffers.
	cdebug << "Allocating buffer for uncompressed version of file.\n";
	pointer::Strong<Memory_arena> uncompressed = 
		new_anonymous_arena(fsize);

	cdebug << "initializing decompressor.\n";
	stream.next_out = NULL;
	stream.avail_out = 0;
	stream.next_in = NULL;
	stream.avail_in = 0;
	stream.opaque = NULL;
	stream.zalloc = intr_z_alloc;
	stream.zfree = intr_z_free;
	int r = inflateInit2(&stream, -MAX_WBITS);
	if(r != Z_OK) {
		char buf[8192];
		sprintf(buf, "Failed to initialize uncompressor: %s", 
			zlib_errorstring(r));
		throw Decompression_error(buf);
	}

	stream.next_out = (unsigned char*)uncompressed->get_base();
	stream.avail_out = uncompressed->get_size();
	stream.next_in = (unsigned char*)deflate_begin + hsize;
	stream.avail_in = deflate_length - hsize - 4;

	cdebug << "Uncompressing file (avail_out = " << stream.avail_out 
		<< ").\n";
	
	r = Z_OK;
	while(r == Z_OK) {
		r = inflate(&stream, Z_NO_FLUSH);
	}

	if(stream.avail_out != 0)
		cwarning << "File does not fill the entiere buffer: " << 
			stream.avail_out << " bytes still free.\n";

	unsigned avail_out = stream.avail_out;

	inflateEnd(&stream);
	switch(r) {
		case Z_STREAM_END:
		case Z_OK:
			crc_computed = crc32(crc_computed, 
				(unsigned char*)uncompressed->get_base(),
				uncompressed->get_size() - avail_out);

			if(crc_computed != crc_correct) {
				throw  Decompression_error("File fails CRC "
					"check.");
			}

			cdebug << "Gzip compressed file successfully "
				"uncompressed.\n";
			break;
		case Z_MEM_ERROR:
			throw Decompression_error("Not enough memory.");
		case Z_DATA_ERROR:
			throw Decompression_error("File is corrupt.");
		case Z_BUF_ERROR:
			throw Decompression_error("File is corrupt or larger "
				"than 4GB uncompressed.");
		default:
			throw Decompression_error("Unknown error.");
	};

	return uncompressed;
}

}
