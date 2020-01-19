/***************************************************************************
  nil_files.h  -  Header for structures used in files around NiL
 ---------------------------------------------------------------------------
  begin                : 2000-02-07
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-28
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Include SDL headers via systemheaders.h

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

/***************************************************************************
Note: always call swap on the structs before writing and after reading them.
 ***************************************************************************/


#ifndef NIL_FILES_H
#define NIL_FILES_H

#ifdef _MSC_VER
#pragma warning (disable : 4091)
#endif

#include "common/systemheaders.h"

/*! ???
    @param value ??? */
void swap32(Sint32 &value);
/*! ???
    @param value ??? */
void swap16(Sint16 &value);
//! unique identifier used in .rgbf files
const Sint32 NIL_MAGIC = 0xbabe0000;

/* Lump files are constructed like this:

Lump_header
data for file 1
data for file 2
data for file 3

Lump_directory_header

Lump_directory_item for file 1
file name of file 1

Lump_directory_item for file 2
file name of file 2

Lump_directory_item for file 3
file name of file 3

*/

//! ???
const Sint32 LUMP_HEADER_MAGIC = NIL_MAGIC+1;
//! the definition of the lump-header
typedef struct Lump_header {
	//! ???
	Sint32 magic;
	//! ???
	Sint32 directory_offset;
	//! ???
	void swap() {
		swap32(magic);
		swap32(directory_offset);
	}
} TD_lump_header;

//! ???
const Sint32 LUMP_DIRECTORY_HEADER_MAGIC = NIL_MAGIC+2;
//! the definition of the lump-directory-header
typedef struct Lump_directory_header {
	//! ???
	Sint32 magic;
	//! ???
	Sint32 items;
	//! ???
	void swap() {
		swap32(magic);
		swap32(items);
	}
} TD_lump_directory_header;

//! ???
const Sint32 LUMP_DIRECTORY_ITEM_MAGIC = NIL_MAGIC+3;
//! the definition of the lump-directory-item-header
typedef struct Lump_directory_item {
	//! ???
	Sint32 magic;
	//! ???
	Sint32 file_offset;
	//! ???
	Sint32 file_length;
	//! ???
	Sint32 name_length;
	//! ???
	void swap() {
		swap32(magic);
		swap32(file_offset);
		swap32(file_length);
		swap32(name_length);
	}
} TD_lump_directory_item;

//! ???
const Sint32 RGBF_HEADER_MAGIC = NIL_MAGIC+4;
//! the definition of the rgbf-header
typedef struct Rgbf_header {
	//! ???
	Sint32 magic;
	//! ???
	Sint32 xsize;
	//! ???
	Sint32 ysize;
	//! ???
	void swap() {
		swap32(magic);
		swap32(xsize);
		swap32(ysize);
	}
} TD_rgbf_header;

//! ???
const Sint32 RGBF_HEADER_MAGIC_OLD = 0xdeadbeef;
//! the definition of old rgbf-header
typedef struct Rgbf_header_old {
	//! ???
	unsigned int magic; //! set to 0xDEADBEEF
	//! ???
	int xsize;
	//! ???
	int ysize;
} TD_rgbf_header_old;

#endif
