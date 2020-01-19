/***************************************************************************
  write-rgbf.h  -  Write RGBF files.
  --------------------------------------------------------------------------
  begin                : 2004-11-14
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last changed         : 2004-11-19
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Write Doxygen comment blocks.

  copyright            : (C) 2004 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/
#ifndef _WRITE_RGBF__H__INCLUDED__
#define _WRITE_RGBF__H__INCLUDED__

#include <cstdio>

/**
 * Write RGBF file header. This function writes RGBF v2 file header to 
 * specified file with specified values.
 *
 * \param file File handle of file to write the header to.
 * \param width Value to write in width field of image header (that is, the
 *	image width).
 * \param height Value to write in height field of image header (that is, the
 *	image height).
 *
 * \returns 0 on success, -1 on failure (errormessage is set to point to
 *	static string describing error occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
int write_rgbf_header(FILE* file, unsigned width, unsigned height);

/**
 * Write scanline to RGBF file. This function writes scanline (row) of RGBF 
 * image to file.
 *
 * \param file File handle of file to write scanline to.
 * \param imageline The image data to write. Buffer must contain at least
 *	4 * width of addressable bytes. Each pixel is presented by 4 
 *	consequtive bytes. First byte is red component, second byte is green
 *	component, third byte is blue component and fourth byte is 
 *	transparency. All these values are on scale 0-255 where 0 is black/
 *	transparent and 255 is bright/opaque.
 * \param typeline The type data to write. Buffer must contain at least
 *	width of addressable bytes (unless forcetype is nonnegative). Each 
 *	pixel is represented by one byte. 0 means void (pixels which are 
 *	totally transparent should be of this type and it also means typeless 
 *	pixel), 1 means hole (dig hole in ground), 2 means earth (diggable 
 *	material), and 3 means stone (indestructable). Values 4-15 are 
 *	reserved, and values 16 or greater should not be used (they can't be 
 *	written  properly). If forcetype is nonnegative, this parameter is 
 *	ignored.
 * \param width Width of scanline. This should be the same as which was passed
 *	to write_rgbf_header, or file may wind up being corrupt.
 * \param forcetype If this is negative, it is ignored. If it is nonnegative
 *	it causes all totally transparent pixels to have type field of 0 (void)
 *	and all others to have type specified by this parameter. See 
 *	description of typeline parameter to see valid values and their 
 *	meanings.
 *
 * \returns 0 on success, -1 on failure (errormessage is set to point to
 *	static string describing error occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
int write_rgbf_scanline(FILE* file, unsigned char* imageline, 
	unsigned char* typeline, unsigned width, int forcetype);

#endif
