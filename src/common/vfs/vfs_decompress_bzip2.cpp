/***************************************************************************
  vfs_decompress_bzip2.cpp  -  VFS BZIP2 decompressor.
  --------------------------------------------------------------------------
  begin                : 2004-08-03
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2004-08-04
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Clean up error reporting.

  last changed         : 2004-09-03
  by                   : Phil Howlett (phowlett@bigpond.net.au)
  changedescription    : Fixed the function load_bzip2 definition for Win32.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-11-20
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Fix double-free vulernablity

  last changed         : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Adapt as vfs_decompress_bzip2.cpp

  copyright            : (C) 2004, 2005 by H. Ilari Liusvaara 
                         (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "common/console/console.h"
#include "common/vfs/vfs_decompress_bzip2.h"
#include "common/vfs/vfs_anonymous_arena.h"
#include "common/misc.h"
#if !defined(_WIN32) && defined(USE_LIBBZIP2)
#include "bzlib.h"
#endif


namespace VFS
{
#if !defined(_WIN32) && defined(USE_LIBBZIP2)

#define COMPRESS_OBUFSZ 1048576	/* 1MB */

namespace
{
	//!Get error message for BZIP2 error
	const char* bzip2_geterrorstr(int r)
	{
		switch(r)
		{
			case BZ_CONFIG_ERROR:
				return "Configuration error";
			case BZ_OUTBUFF_FULL:
				return "Output buffer full";
			case BZ_UNEXPECTED_EOF:
				return "Compressed file truncated";
			case BZ_IO_ERROR:
				return "I/O error (disk failure)";
			case BZ_DATA_ERROR_MAGIC:
				return "Incorrect magic number (file corrupt "
					"or not BZIP2 compressed)";
			case BZ_DATA_ERROR:
				return "Incorrect checksum (file corrupt)";
			case BZ_MEM_ERROR:
				return "Out of memory";
			case BZ_PARAM_ERROR:
				return "Invalid parameters";
			case BZ_SEQUENCE_ERROR:
				return "Sequencing error";
			case BZ_OK:
				return "Ready for more data";
			case BZ_RUN_OK:
				return "Run OK???";
			case BZ_FLUSH_OK:
				return "Flush OK???";
			case BZ_FINISH_OK:
				return "Finish OK???";
			case BZ_STREAM_END:
				return "Entiere BZIP2 stream successfully "
					"decoded";
			default:
				return "Unknown error";
		}
	}
}

#define UNCOMPRESS_MAXSIZE (8 * 1024 * 1024)

pointer::Strong<Memory_arena> decompress_bzip2(
	pointer::Strong<Memory_arena> data) throw(Decompression_too_large,
	Decompression_error, std::bad_alloc)
{
	//Altough repeated copying of the data is expensive, BZIP2 decoding
	//is even more so...
	bz_stream stream;
	char dbuf[COMPRESS_OBUFSZ];
	pointer::Strong<Memory_arena> uarena;
	size_t outsize = 0;

	cdebug << "Initializing BZIP2 decompressor.\n";
	//Initialize the memory allocators.
	stream.bzalloc = NULL;
	stream.bzfree = NULL;

	//Initialize BZIP2 decoding.
	int r = BZ2_bzDecompressInit(&stream, 0, 0);
	if(r < 0) {
		//This is BAD! Definitely qualifies for assertion failure 
		//since no data has been input yet. This should never fail!
		cassert << "Failed to initialized BZIP2 decoder: " 
			<< bzip2_geterrorstr(r) << ".";
	}

	//Input stream.
	stream.next_in = (char*)data->get_base();
	stream.avail_in = (unsigned int)data->get_size();

	//Init output stream.
	stream.avail_out = 0;

	cdebug << "BZIP2 decompressor ready to uncompress file.\n";

	while(1) {
		if(stream.avail_out == 0) {

			cdebug << "Reinitializing output buffer.\n";

			//See that the decompressed size is sane. DOS attack 
			//preventor.
			if(outsize + COMPRESS_OBUFSZ > UNCOMPRESS_MAXSIZE) {
				BZ2_bzDecompressEnd(&stream);
				throw Decompression_too_large("Uncompression "
					"too large.");
			}
	
			stream.next_out = dbuf;
			stream.avail_out = COMPRESS_OBUFSZ;
		}

		cdebug << "Decompressing " << stream.avail_out << " bytes "
			"from BZIP2 compressed file.\n";

		//Decompress block. Ignore certain return values.
		r = BZ2_bzDecompress(&stream);
		if(r != BZ_OK && r != BZ_OUTBUFF_FULL && r != BZ_STREAM_END) {
			//Failed to decompress.
		  	char buf[8192];
			BZ2_bzDecompressEnd(&stream);
			sprintf(buf, "Decompressor error: %s",  
				bzip2_geterrorstr(r));
			throw Decompression_error(buf);
		}

		cdebug << "BZIP2 decomprsor returned '" << bzip2_geterrorstr(r)
			<< "' with " << stream.avail_out << " bytes left in "
			<< "output buffer.\n"; 
	
		if(r == BZ_STREAM_END) {
			cdebug << "End of BZIP2 compressed stream reached.\n";
			BZ2_bzDecompressEnd(&stream);

			//Okay, merge the final block.
			cdebug << "Enlarging output buffer to "
				<< outsize + COMPRESS_OBUFSZ - 
				stream.avail_out <<" bytes.\n", 
			uarena = resize_arena(uarena, outsize + 
				COMPRESS_OBUFSZ - stream.avail_out);

			//Copy the data.
			cdebug << "Copying data to new output buffer.\n";
			memmove((char*)uarena->get_base() + outsize, dbuf, 
				COMPRESS_OBUFSZ - stream.avail_out);

			break;
		}
		if(stream.avail_out == 0) {
			//Merge the block.
			try {
				cdebug << "Enlarging output buffer to "
					<< outsize + COMPRESS_OBUFSZ 
					<< " bytes.\n", 
				uarena = resize_arena(uarena, 
					outsize + COMPRESS_OBUFSZ);
			} catch(...) {
				BZ2_bzDecompressEnd(&stream);
				throw;
			}

			//Copy the data.
			cdebug << "Copying data to new output buffer.\n";
			memmove((char*)uarena->get_base() + outsize, dbuf, 
				COMPRESS_OBUFSZ);
		}
	}

	return uarena;
}
#else

pointer::Strong<Memory_arena> decompress_bzip2(
	pointer::Strong<Memory_arena> data) throw(Decompression_too_large,
	Decompression_error, std::bad_alloc)
{
	throw Decompression_error("BZIP2 decompression support disabled.");
}
#endif

}
