/***************************************************************************
  vfs_decompress.cpp  - decompressor generic defines & dispatcher.
  --------------------------------------------------------------------------
  begin                : 2005-01-29
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include<cstring>

#include "common/vfs/vfs_decompress.h"
#include "common/vfs/vfs_decompress_gzip.h"
#include "common/vfs/vfs_decompress_bzip2.h"
#include "common/console/console.h"

namespace VFS
{
Decompression_too_large::Decompression_too_large(
	const std::string& description) throw()
	: overflow_error(description)
{
}

Decompression_error::Decompression_error(
	const std::string& description) throw()
	: std::range_error(description)
{
}

pointer::Strong<Memory_arena> decompress_data(
	pointer::Strong<Memory_arena> data) throw()
{
	if(data->get_size() >= 2 && !strncmp((const char*)data->get_base(), 
			"BZ", 2)) {
		cdebug << "Is BZIP2 compressed.\n";
		return decompress_bzip2(data);
	} else if(data->get_size() >= 2 && 
		!strncmp((const char*)data->get_base(), "\037\213", 2)) {
		cdebug << "Is GZIP compressed.\n";
		return decompress_gzip(data);
	} else {
		return data;
	}	
}

}
