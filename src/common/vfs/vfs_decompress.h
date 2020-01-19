/***************************************************************************
  vfs_decompress.h  - decompressor generic defines & dispatcher.
  --------------------------------------------------------------------------
  begin                : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _vfs_decompress_h_
#define _vfs_decompress_h_

#include "common/vfs/vfs_memory_arena.h"
#include "common/smartpointer/pointer.h"
#include <stdexcept>

namespace VFS
{
/**
 * \brief Exception thrown when uncompression of data would be too large.
 *
 * This exception class is thrown when uncompressing data that is too large.
 *
 */
class Decompression_too_large : public std::overflow_error
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
	Decompression_too_large(const std::string& description) 
		throw(std::bad_alloc);
};

/**
 * \brief Exception thrown when error occurs during uncompression.
 *
 * This exception class is thrown when uncompressing data triggers error
 * in decompressor.
 *
 */
class Decompression_error : public std::range_error
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
	Decompression_error(const std::string& description) 
		throw(std::bad_alloc);
};


/**
 * \brief Decompress possibly compressed data.
 *
 * This routine takes memory arena containing possibly compressed data, tries
 * to guess compression format used, and passes back memory arena containing 
 * uncompressed version of the data passed in. Maximum allowed size of 
 * uncompressed data is 8MiB. Note that if this routine is unable to guess the
 * compression method, it assumes the file is uncompressed and just passes
 * back the arena it was given.
 *
 * \param data The possibly compressed data to uncompress.
 * 
 * \return Memory arena containing the uncompressed data.
 *
 * \exception Decompression_too_large The decompressed data is too big 
 *	(>=8MiB).
 * \exception Decompression_error Error was encountered during decompression.
 *	Usually occurs because the data to be uncompressed is corrupt.
 * \exception std::bad_alloc Not enough memory to allocate buffer to hold
 *	uncompressed data.
 *
 */
pointer::Strong<Memory_arena> decompress_data(
	pointer::Strong<Memory_arena> data) throw(Decompression_too_large,
	Decompression_error, std::bad_alloc);
}

#endif
