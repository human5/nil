/***************************************************************************
  read-png.h  - Read PNG files.
  --------------------------------------------------------------------------
  begin                : 2004-11-14
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last changed         : 2004-11-19
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Write Doxygen comment blocks.
                         Use fread instead of mapping for compatiblity.

  copyright            : (C) 2004 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/
#ifndef _READ_PNG__H__INCLUDED__
#define _READ_PNG__H__INCLUDED__

#include <cstdlib>
#include <png.h>
#include <cstdio>

/**
 * Check PNG file validity. This function checks that PNG file has required
 * magic, IHDR and IEND chunks and no chunk fails CRC check or has invalid
 * type. It reads the file to end, so if you want to use it, rewind the
 * stream.
 *
 * \param file File handle pointing to suspected PNG file.
 * 
 * \return TRUE if file is valid, FALSE otherwise (and errormessage is set to
 *	static string describing the error.)
 *
 * \author H. Ilari Liusvaara
 *
 */
bool is_valid_png(FILE* file);

/**
 * Read 'niLM' chunk from PNG file. This function locates and reads the 'niLM'
 * chunk value contained in PNG file, or reports that no such value exists.
 * this function reads to indeterminate point in PNG file (so rewind if
 * necressary after calling.)
 *
 * \param file File handle pointing to PNG file.
 *
 * \returns The 'niLM' chunk value, or -1 if file has no 'niLM' chunk (this is
 *	\b not considered an error.), -2 if some kind of true error occurs
 *	(in this case errormessage is set to point to static string
 *	describing error).
 *
 * \author H. Ilari Liusvaara
 *
 */
int read_png_with_niLM(FILE* file);

/** 
 * State of PNG reader. This structure contains the state that is kept between
 * calls to PNG reader.
 *
 * \author H. Ilari Liusvaara
 *
 */
struct png_read_state
{
	///Width of image.
	png_uint_32 width;
	///Height of image.
	png_uint_32 height;
	///LIBPNG PNG reader structure.
	png_structp png;
	///LIBPNG PNG info structure.
	png_infop info;
	///The file handle to PNG file.
	FILE* file;
	///Bitdepth of image.
	int depth;
	///Color type of image (greyscale/paletted/truecolor/etc...).
	int colortype;
	///Interlace method of image
	int interlace;
};

/** 
 * Construct PNG reader structure for paletted PNG image. This function 
 * constructs the PNG reader structure and verifies that image is paletted
 * and noninterlaced (interlacing is not yet supported).
 *
 * \param file File handle to PNG file.
 *
 * \return the reader structure (non-NULL) on success or NULL on failure
 *	(errormessage is set to static string describing error).
 *
 * \author H. Ilari Liusvaara
 *
 */
struct png_read_state* start_read_png_paletted(FILE* file);

/** 
 * Construct PNG reader structure for PNG image of any color type. 
 * This function constructs the PNG reader structure and verifies that image is
 * noninterlaced (interlacing is not yet supported). It also sets up filters
 * apporiately so data will come out as 32-bit RGBA.
 *
 * \param file File handle to PNG file.
 *
 * \return the reader structure (non-NULL) on success or NULL on failure
 *	(errormessage is set to static string describing error).
 *
 * \author H. Ilari Liusvaara
 *
 * \todo Test that this function really sets up correct filters for various 
 *	PNG image types and colordepths.
 *
 */
struct png_read_state* start_read_png_freeform(FILE* file);

/**
 * Fetch scanline from image. This function reads scanline (row) from image
 * and stores it to suppiled buffer.
 *
 * \param state The PNG reading state
 * \param buffer Buffer to store the read data to. This parameter should point
 *	to at least 4 * image-width bytes if reading structure was established
 *	by start_read_png_freeform, and to at least image-width bytes if
 *	reading strucure was established by start_read_png_paletted.
 *	Each pixel is presented by consequitive 4 bytes in nonpaletted mode and
 *	1 byte in paletted mode. In nonpaletted mode, first byte is red 
 *	intensity, second is green intensity, third is blue intensity and 
 *	fourth is transparency. All of these values are given on scale from 0 
 *	(black/transparent) to 255 (bright/opaque). In paletted mode, each
 *	byte contains palette index of color in corresponding pixel.
 *
 * \returns 0 on success, -1 on failure (errormessage is set to point to
 *	static string describing error occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
int read_png_scanline(struct png_read_state* state, unsigned char* buffer);

/** 
 * End reading PNG images and free reading structure. This function frees
 * the PNG reading structure. But it does \b not close the file passed to
 * start_read_png_* functions.
 *
 * \param state The PNG read state.
 *
 * \author H. Ilari Liusvaara
 * 
 */
void end_read_png(struct png_read_state* state);

#endif
