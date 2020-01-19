/***************************************************************************
  vfs_decompress_bzip2.h  - BZIP2 decompressor.
  --------------------------------------------------------------------------
  begin                : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _vfs_decompress_bzip2_h_
#define _vfs_decompress_bzip2_h_

#include "common/vfs/vfs_decompress.h"

namespace VFS
{
/**
 * \brief Decompress BZIP2-compressed data.
 *
 * This routine takes memory arena containing BZIP2-compressed data, and 
 * passes back new anonymous arena containing uncompression of the data passed
 * in. Maximum allowed size of uncompressed data is 8MiB.
 *
 * \param data The compressed data to uncompress.
 * 
 * \return New anonymous memory arena containing the uncompressed data.
 *
 * \exception Decompression_too_large The decompressed data is too big 
 *	(>=8MiB).
 * \exception Decompression_error Error was encountered during decompression.
 *	Usually occurs because the data to be uncompressed is either corrupt 
 *	or not compressed with BZIP2.
 * \exception std::bad_alloc Not enough memory to allocate buffer to hold
 *	uncompressed data.
 *
 */
pointer::Strong<Memory_arena> decompress_bzip2(
	pointer::Strong<Memory_arena> data) throw();
}

#endif
