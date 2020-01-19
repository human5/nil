/***************************************************************************
  write-png.cpp  -  Write PNG files.
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
#include "write-png.h"
#include <zlib.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern char* errormessage;

namespace
{
/**
 * Write unsigned to buffer in big-endian order. This function writes a 32-bit
 * quantity to buffer in big-endian byte order (network byte order).
 *
 * \param addr The address to store the value. It should point to at least
 *	4 writable bytes.
 * \param number The number to write to buffer.
 *
 * \author H. Ilari Liusvaara
 *
 */
	void write_unsigned(unsigned char* addr, unsigned number)
	{
		addr[0] = (number >> 24) & 0xFF;
		addr[1] = (number >> 16) & 0xFF;
		addr[2] = (number >>  8) & 0xFF;
		addr[3] = (number >>  0) & 0xFF;
	}

#define IDAT_TYPE "IDAT"

/**
 * Flush output data from PNG context to file. This function flushes the PNG
 * context output buffer to PNG file.
 *
 * \param context The context which has output buffer in need of flush.
 * \param force If true, the buffer is flushed anyway. If false, the buffer
 *	is not flushed unless it is full.
 *
 * \returns 0 on success, -1 on failure (errormessage is set to point to
 *	static string describing error occured).
 *
 * \author H. Ilari Liusvaara
 *
 */
	int flush_compression_context(struct png_compress_context* context, 
		bool force)
	{
		size_t data_available = (char*)context->stream.next_out - 
			context->output_buffer;

		//If no data, do nothing.
		if(data_available == 0)
			return 0;

		//If space still left and not forced, do nothing.
		if(context->stream.avail_out > 0 && !force)
			return 0;

		if(save_png_chunk(context->file, IDAT_TYPE, 
				context->output_buffer, data_available))
			return -1;

		context->stream.next_out = (Bytef*)context->output_buffer;
		context->stream.avail_out = CONTEXT_OUTPUT_SIZE;

		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
int save_png_chunk(FILE* file, const char* chunkname, const char* chunkdata, 
	size_t chunkdatalen)
{
	//Extra space for length, type and CRC.
	unsigned char* chunkbuffer = (unsigned char*)alloca(chunkdatalen + 12);

	//First four bytes are chunk data length.
	write_unsigned(chunkbuffer + 0, chunkdatalen);

	//Second four bytes are chunk type.
	chunkbuffer[4] = (unsigned char)chunkname[0];
	chunkbuffer[5] = (unsigned char)chunkname[1];
	chunkbuffer[6] = (unsigned char)chunkname[2];
	chunkbuffer[7] = (unsigned char)chunkname[3];

	//Then the data.
	if(chunkdatalen > 0)
		memmove(chunkbuffer + 8, chunkdata, chunkdatalen);

	//Finally compute and store the CRC.
	write_unsigned(chunkbuffer + chunkdatalen + 8, crc32(crc32(0, NULL, 0),
		(const Bytef*)chunkbuffer + 4, chunkdatalen + 4));

	//Write the chunk.
	if(fwrite(chunkbuffer, 1, chunkdatalen + 12, file) == 
			chunkdatalen + 12)
		return 0;

	errormessage = "Failed to write chunk to PNG file.";
	return -1;
}

#define IHDR_TYPE "IHDR"
#define IHDR_LENGTH 13   

#define PLTE_TYPE "PLTE"
#define PLTE_LENGTH 768

#define IEND_TYPE "IEND"
#define IEND_LENGTH 0

///////////////////////////////////////////////////////////////////////////////
struct png_compress_context* save_png_header(FILE* file,
	const struct image_data* imagedata)
{
	//Write the magic.
	static const unsigned char magic[] = {137, 80, 78, 71, 13, 10, 26, 10};

	//Write the magic.
	if(fwrite(magic, 1, 8, file) != 8)
	{
		errormessage = "Failed to write header magic to PNG file.";
		return NULL;
	}
	
	unsigned char* ihdr_chunk = (unsigned char*)alloca(IHDR_LENGTH);
	unsigned char* plte_chunk = NULL;

	//Fill in the IHDR chunk.
	write_unsigned(ihdr_chunk + 0, imagedata->width);
	write_unsigned(ihdr_chunk + 4, imagedata->height);
	//Bit depth is always 8 here.
	ihdr_chunk[8] = 8;
	//Select paletted / truecolor with alpha.
	ihdr_chunk[9] = imagedata->uses_palette ? 3 : 6;
	//Fixed values.
	ihdr_chunk[10] = 0;
	ihdr_chunk[11] = 0;
	ihdr_chunk[12] = 0;
	
	//Write it.
	if(save_png_chunk(file, IHDR_TYPE, (const char*)ihdr_chunk, 
		IHDR_LENGTH))
	{
		errormessage = "Failed to write header chunk to PNG file.";
		return NULL;
	}

	//Fill in the PLTE chunk.
	if(imagedata->uses_palette)
	{
		plte_chunk = (unsigned char*)alloca(PLTE_LENGTH);
		for(unsigned i = 0; i < 256; i++)
		{
			plte_chunk[3 * i + 0] = imagedata->palette[i].red;
			plte_chunk[3 * i + 1] = imagedata->palette[i].green;
			plte_chunk[3 * i + 2] = imagedata->palette[i].blue;
		}

		//Write it.
		if(save_png_chunk(file, PLTE_TYPE, (const char*)plte_chunk, 
			PLTE_LENGTH))
		{
			errormessage = "Failed to write palette chunk to PNG "
				"file.";
			return NULL;
		}
	}

	//Create context.
	struct png_compress_context* ctx = (png_compress_context*)
		malloc(sizeof(struct png_compress_context));
	if(ctx == NULL)
	{
		errormessage = "Not enough memory to allocate PNG writing "
			"context.";
		return NULL;
	}

	ctx->stream.next_in = NULL;
	ctx->stream.avail_in = 0;
	ctx->stream.next_out = NULL;
	ctx->stream.avail_out = 0;
	ctx->stream.zalloc = NULL;
	ctx->stream.zfree = NULL;
	ctx->stream.opaque = NULL;

	if(deflateInit2(&(ctx->stream), Z_BEST_COMPRESSION, Z_DEFLATED, 
		MAX_WBITS < 15 ? MAX_WBITS : 15, 8, Z_DEFAULT_STRATEGY) != 
		Z_OK)
	{
		errormessage = "Failed to initialize ZLIB compression stream.";
		delete ctx;
		return NULL;
	}

	ctx->stream.next_in = (Bytef*)ctx->input_buffer;
	ctx->stream.avail_in = 0;
	ctx->stream.next_out = (Bytef*)ctx->output_buffer;
	ctx->stream.avail_out = CONTEXT_OUTPUT_SIZE;

	ctx->file = file;
	ctx->end = false;
	ctx->imagedata = *imagedata;
	ctx->singletype = -1;
	return ctx;
}

#define NILM_TYPE "niLM"
#define NILM_LENGTH 1

///////////////////////////////////////////////////////////////////////////////
int finish_png(struct png_compress_context* context)
{
	char tbyte = context->singletype;

	if(write_png_data(context, NULL, 0) < 0)
	{
		errormessage = "Failed to flush PNG image data stream to "
			"file.";
		deflateEnd(&(context->stream));
		fclose(context->file);
		delete context;
		return -1;
	}

	deflateEnd(&(context->stream));

	//Save the magic chunk if apporiate.
	if(context->singletype >= 0)
		if(save_png_chunk(context->file, NILM_TYPE, &tbyte, 
			NILM_LENGTH))
		{
			errormessage = "Failed to save niLM chunk to PNG "
				"file.";
			fclose(context->file);
			delete context;
			return -1;
		}

	if(save_png_chunk(context->file, IEND_TYPE, NULL, IEND_LENGTH))
	{
		errormessage = "Failed to write stream end chunk to PNG file.";
		fclose(context->file);
		delete context;
		return -1;
	}

	if(fclose(context->file))
	{
		errormessage = "Failed to close PNG file.";
		delete context;
		return -1;
	}

	delete context;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int write_png_data(png_compress_context* context, const char* data,
	size_t length)
{
	if(length == 0)
		context->end = true;

	//Write as much as possible.
	while(length || context->stream.avail_in > 0 || context->end)
	{
		//Do input preload if possible. Yes, pointer comparision there.
		if(context->stream.avail_in > 0 && 
			(void*)context->input_buffer != 
			(void*)context->stream.next_in)
		{
			memmove(context->input_buffer, context->stream.next_in,
				context->stream.avail_in);
		}
		context->stream.next_in = (Bytef*)context->input_buffer;
		if(context->stream.avail_in < CONTEXT_INPUT_SIZE)
		{
			//How much can we preload?
			size_t preloadable = CONTEXT_INPUT_SIZE - 
				context->stream.avail_in;
			if(length < preloadable)
				preloadable = length;

			memmove(context->input_buffer + 
				context->stream.avail_in, data, preloadable);
			data += preloadable;
			length -= preloadable;
			context->stream.avail_in += preloadable;
		}
		
		//Now that input buffer is full of data, do compress.
		int r = deflate(&(context->stream), 
			context->end ? Z_FINISH : 0);

		if(r == Z_OK)
		{
			//Normal. Do output flush.
			if(flush_compression_context(context, false))
				return -1;
		}
		else if(r == Z_STREAM_END)
		{
			//Stream ends. Do forced flush.
			if(flush_compression_context(context, true))
				return -1;
			return 1;
		}
		else
		{
			//Error.
			errormessage = "ZLIB error while compressing "
				"datablock.";
			return -1;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int write_png_scanline(struct png_compress_context* context,
	const char* scanline)
{
	size_t scanline_length =  context->imagedata.width * 
		(context->imagedata.uses_palette ? 1 : 4) + 1;
	char* scanline_buffer = (char*)alloca(scanline_length);

	scanline_buffer[0] = 0;		//No filter.
	memmove(scanline_buffer + 1, scanline, scanline_length - 1);

	return write_png_data(context, scanline_buffer, scanline_length);
}
