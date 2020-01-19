/***************************************************************************
  write-png.h  -  Write PNG files.
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
#ifndef _WRITE_PNG__H__INCLUDED__
#define _WRITE_PNG__H__INCLUDED__

#include <cstdio>
#include <zlib.h>

/**
 * Palette entry. This structure groups one entry in palette of image.
 * all components range from 0 (black) to 255 (bright).
 */
struct palette_entry
{
	///Red component of color.
	unsigned char red;
	///Green component of color.
	unsigned char green;
	///Blue component of color.
	unsigned char blue;	
};

/**
 * Structure describing image parameters. This structure describes various
 * parameters needed to write an image.
 */
struct image_data
{
	///Width of image to write.
	unsigned width;
	///Height of image to write.
	unsigned height;
	///Is the image paletted (as opposed to truecolor with alpha channel)?
	bool uses_palette;
	///If image is paletted, this field contains palette image uses. If
	///it is not paletted, this field is ignored.
	palette_entry palette[256];	/**< If image is paletted, this field
						contains the palette image
						uses. */
};

#define CONTEXT_INPUT_SIZE 1024
#define CONTEXT_OUTPUT_SIZE 1024

/**
 * PNG writer context structure. This structure groups state kept by PNG
 * writer between calls to it.
 */
struct png_compress_context
{
	///ZLIB compression context.
	z_stream stream;
	///File handle to write the image to.		
	FILE* file;
	///Input buffer for compression.
	char input_buffer[CONTEXT_INPUT_SIZE];
	///Output buffer for compression.
	char output_buffer[CONTEXT_OUTPUT_SIZE];
	///The image_data structure passed to save_png_header.
	image_data imagedata;
	///If negative, this field is ignored. If nonnegative, it signifies
	///that 'niLM'-chunk with value specified shall be emitted to output
	///file. Valid nonnegative values are 0-15, and their meanings are
	///described in write_rgbf_scanline. 
	int singletype;
	///If true, it signifies that end of image stream has been signaled,
	///if false, image stream will still continue with more data.
	bool end;
};

/**
 * Write IDAT chunks to PNG file. This function writes image data to PNG file.
 * As per PNG specification, all IDAT chunks must be adjacent in file, so
 * don't write any other chunk types before stream ends. And don't write any
 * IDATs to PNG after stream ends.
 * 
 * \param context The PNG writer context to use.
 * \param data The uncompressed data to write. If length is 0, this value is
 *	ignored.
 * \param length The length of data to write. 0 signifies end of image data
 *	stream. 0 also causes pending data to be flushed to file.
 *
 * \returns 0 on success, -1 on failure (errormessage is set to point to
 *	static string describing error occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
int write_png_data(png_compress_context* context, const char* data, 
	size_t length);

/**
 * Write chunk to PNG file. This function writes any kind of chunk to PNG
 * file.
 *
 * \param file File handle to write the chunk to.
 * \param chunkname The type of chunk. This parameter must point to at least 4 
 *	addressable bytes, first four of which must be alphabetics in ASCII
 *	character set. Case of four characters will determine type of chunk
 *	written as per PNG standard.
 * \param chunkdata The data to write to chunk. This data is not altered in
 *	any way (e.g. compressed even if chunk would require compressed data)
 *	if chunkdatalength is 0 this parameter is ignored.
 * \param chunkdatalen The length of data to write to chunk. It can also
 *	legally be 0 in which case empty chunk is written.
 *
 * \returns 0 on success, -1 on failure (errormessage is set to point to
 *	static string describing error occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
int save_png_chunk(FILE* file, const char* chunkname, const char* chunkdata, 
	size_t chunkdatalen);

/**
 * Write magic and IHDR or IHDR/PLTE structures to file and create PNG writing
 * context. This function writes PNG magic to file followed by image header
 * and if needed palette structure. Additionally, it creates PNG writing
 * context.
 * 
 * \param file File handle to file the PNG is to be written to.
 * \param imagedata Image_data structure to use in writing PNG file.
 *
 * \returns Compression context (non-null) on success, NULL on failure
 *	(errormessage is set to point to static string describing error 
 *	occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
struct png_compress_context* save_png_header(FILE* file, 
	const struct image_data* imagedata);

/**
 * Write PNG trailer chunk to file, close the file and destroy PNG writing'
 * context. This function writes PNG trailer (IEND) to file and closes it.
 * Additionally it will destroy the PNG writing context.
 *
 * \param context The PNG writing context to use.
 *
 * \returns 0 on success, -1 on failure (errormessage is set to point to
 *	static string describing error occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
int finish_png(struct png_compress_context* context);

/**
 * Write scanline to PNG image. This function writes a scanline (row) to PNG
 * image.
 *
 * \param context The PNG writing context.
 * \param scanline The scanline data to write. This parameter should point to 
 *	at least 4 * image-width bytes if image is not paletted or at least
 *	image-width bytes if it is. Each pixel is presented by consequitive
 *	4 bytes in nonpaletted mode and 1 byte in paletted mode. In nonpaletted
 *	mode, first byte is red intensity, second is green intensity, third
 *	is blue intensity and fourth is transparency. All of these values are
 *	given on scale from 0 (black/transparent) to 255 (bright/opaque). In
 *	paletted mode each byte contains palette index of color in 
 *	corresponding pixel.
 *
 * \returns 0 on success, -1 on failure (errormessage is set to point to
 *	static string describing error occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
int write_png_scanline(struct png_compress_context* context, 
	const char* scanline);

#endif
