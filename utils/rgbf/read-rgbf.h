/***************************************************************************
  read-rgbf.h  - Read RGBF files.
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
#ifndef _READ_RGBF__H__INCLUDED__
#define _READ_RGBF__H__INCLUDED__

#include <cstdio>

/**
 * Structure describing the header data of RGBF file.
 *
 * \author H. Ilari Liusvaara
 *
 */
struct rgbf_header
{
	///Width of image.
	unsigned width;
	///Height of image.
	unsigned height;
	///Format of image. 1 for RGBF v1, 2 for RGBF v2 and negative for
	///invalid format.
	int format;
};

/**
 * Read header of RGBF file. This function reads the header from RGBF file
 * and fills the header information structure with obtained values.
 *
 * \param file File handle of image file to read.
 *
 * \return The header information structure. On error, format field is set to
 *	-1 and errormessage is set to static string describing error.
 *
 * \author H. Ilari Liusvaara 
 *
 */
rgbf_header read_rgbf_header(FILE* file);

/**
 * Read scanline from RGBF file. This function reads scanline (row) from RGBF
 * file and store the read values to buffers after splitting them to image
 * and type components.
 *
 * \param file File handle of file to read.
 * \param imageline Buffer to store the image component of read scanline to.
 *	It should point to at least 4 * width writable bytes. Each pixel is
 *	represented by 4 consequtive bytes. First of them is red intensity,
 *	second is green intensity, third is blue intensity and fourth is
 *	transparency. All these are on scale from 0 (black/transparent) to
 *	255 (bright/opaque).
 * \param typeline Buffer to store the type component of read scanline to.
 *	It should point to at least width writable bytes. Each pixel is 
 *	represented by only single byte. See write_rgbf_scanline for
 *	legal types and their meanings.
 * \param width The width of scanline. This value should be the same as width
 *	field read from image header.
 * \param opaque_type If this is 16 before the call and all nontransparent 
 *	pixels on scanline are of same type, it is set to that type. If row 
 *	contains nontransparent pixels of multiple types, this is set to -1. 
 *	If this is anything other than 16 and nonnegative and nontransparent 
 *	pixel of diffrent type than the value in it before call is encountered,
 *	this is set to -1, otherwise it is not touched. It is useful for 
 *	computing what to put in 'niLM' chunk.
 *
 * \returns 0 on success, -1 on failure (errormessage is set to point to
 *	static string describing error occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
int read_rgbf_scanline(FILE* file, char* imageline, char* typeline, 
	unsigned width, int& opaque_type);

#endif
