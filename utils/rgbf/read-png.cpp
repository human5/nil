/***************************************************************************
  read-png.cpp  - Read PNG files.
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
#include "read-png.h"
#include <zlib.h>
#include <cstring>
#include <zlib.h>
#include <png.h>

extern char* errormessage;

#define PNG_MAGIC_SIZE 8
#define PNG_CHUNK_TYPE_SIZE 4
#define PNG_CHUNK_HEADER_SIZE 8
#define PNG_CHUNK_TYPE_OFFSET 4
#define PNG_CRC_SIZE 4
#define PNG_READ_BLOCK_SIZE 1024

unsigned char IHDR[PNG_CHUNK_TYPE_SIZE] = {73, 72, 68, 82};
unsigned char IEND[PNG_CHUNK_TYPE_SIZE] = {73, 69, 78, 68};
unsigned char niLM[PNG_CHUNK_TYPE_SIZE] = {110, 105, 76, 77};
unsigned char rightmagic[PNG_MAGIC_SIZE] = {137, 80, 78, 71, 13, 10, 26, 10};


///////////////////////////////////////////////////////////////////////////////

namespace
{
/**
 * Read unsigned in big-endian order from buffer. This function extracts big-
 * endian (network byte order) 32-bit quantity from buffer.
 *
 * \param addr Pointer to bytes forming the number. At least 4 bytes should
 *	be addressable.
 *
 * \return The value read.
 *
 * \author H. Ilari Liusvaara
 *
 */
	unsigned read_unsigned(const unsigned char* addr)
	{
		unsigned n1 = (unsigned char)addr[0];
		unsigned n2 = (unsigned char)addr[1];
		unsigned n3 = (unsigned char)addr[2];
		unsigned n4 = (unsigned char)addr[3];

		return (n4) | (n3 << 8) | (n2 << 16) | (n1 << 24);
	}

/**
 * Check if chunk next read from file is valid. This function reads a PNG
 * chunk from file and checks legality of its type and its CRC. File position
 * is expected to be just before beginning chunk on call, and it will be
 * positioned on end of chunk on successful return. If return is unsuccessful,
 * position is indeterminate.
 *
 * \param file File handle to read the chunk from.
 * \param chunktype Buffer to store the chunk type to. Should have at least
 *	4 writable bytes.
 * \param firstbyte First byte of chunk gets stored here, or -1 if no first
 *	byte exists.
 * 
 * \return TRUE if chunk is valid, FALSE otherwise.
 *
 * \author H. Ilari Liusvaara
 *
 */
	bool is_valid_chunk(FILE* file, unsigned char* chunktype,
		int& firstbyte)
	{
		unsigned char chunk_header[PNG_CHUNK_HEADER_SIZE];
		unsigned chunklength;
		unsigned x;
		unsigned whole_blocks;
		unsigned partial_block_modulus;
		unsigned char read_buffer[PNG_READ_BLOCK_SIZE];
		unsigned computed_crc = crc32(0, NULL, 0);
		unsigned char crc_buffer[PNG_CRC_SIZE];
		unsigned claimed_crc;
		bool got_first_byte = false;

		if((x = fread(chunk_header, 1, PNG_CHUNK_HEADER_SIZE, file)) !=
			PNG_CHUNK_HEADER_SIZE)
		{
			if(ferror(file))
				errormessage = "Error while reading file.";
			else if(feof(file) && x > 0)
				errormessage = "File ends in middle of chunk.";
			else if(feof(file))
				errormessage = "File ends without IEND chunk.";
			else
				errormessage = "Unknown error";
			return false;
		}

		chunklength = read_unsigned(chunk_header);
		for(unsigned i = PNG_CHUNK_TYPE_OFFSET; 
			i < PNG_CHUNK_TYPE_OFFSET + PNG_CHUNK_TYPE_SIZE; i++)
		{
			if(chunk_header[i] < 65 || (chunk_header[i] > 90 &&
				chunk_header[i] < 97) || chunk_header[i] > 122)
			{
				errormessage = "Bad chunk type field value";
				return false;
			}
		}

		//Yes, chunk type is included in CRC.
		memmove(chunktype, chunk_header +
			PNG_CHUNK_TYPE_OFFSET, PNG_CHUNK_TYPE_SIZE);
		computed_crc = crc32(computed_crc, chunktype,
			PNG_CHUNK_TYPE_SIZE);

		whole_blocks = chunklength / PNG_READ_BLOCK_SIZE;
		partial_block_modulus = chunklength % PNG_READ_BLOCK_SIZE;

		//Read whole blocks.
		for(unsigned i = 0; i < whole_blocks; i++)
		{
			if(fread(read_buffer, 1, PNG_READ_BLOCK_SIZE, file) !=
				PNG_READ_BLOCK_SIZE)	
			{
				if(ferror(file))
					errormessage = "Error while reading "
						"file.";
				else if(feof(file))
					errormessage = "File ends in midle of "
						"chunk.";
				else
					errormessage = "Unknown error";
				return false;
			}
			computed_crc = crc32(computed_crc, read_buffer,
				PNG_READ_BLOCK_SIZE);

			if(!got_first_byte)
			{
				got_first_byte = true;
				firstbyte = read_buffer[0];
			}
		}

		//Then the rest, if any.
		if(partial_block_modulus > 0)
		{
			if(fread(read_buffer, 1, partial_block_modulus, file) 
				!= partial_block_modulus)	
			{
				if(ferror(file))
					errormessage = "Error while reading "
						"file.";
				else if(feof(file))
					errormessage = "File ends in midle of "
						"chunk.";
				else
					errormessage = "Unknown error";
				return false;
			}
			computed_crc = crc32(computed_crc, read_buffer,
				partial_block_modulus);

			if(!got_first_byte)
			{
				got_first_byte = true;
				firstbyte = read_buffer[0];
			}
		}

		//Empty block.
		if(!got_first_byte)
			firstbyte = -1;

		if(fread(crc_buffer, 1, PNG_CRC_SIZE, file) != PNG_CRC_SIZE)
		{
			if(ferror(file))
				errormessage = "Error while reading file.";
			else if(feof(file))
				errormessage = "File ends in midle of chunk.";
			else
				errormessage = "Unknown error";
			return false;
		}
		claimed_crc = read_unsigned(crc_buffer);

		//Check CRC.		
		if(claimed_crc != computed_crc)
		{
			errormessage = "Block fails CRC check.";
			return false;
		}

		return true;
	}
}

///////////////////////////////////////////////////////////////////////////////
int read_png_with_niLM(FILE* file)
{
	unsigned char chunktype[PNG_CHUNK_TYPE_SIZE];
	unsigned char magicbuf[PNG_MAGIC_SIZE];
	int value = -1;
	int y;

	//First, read the magic from file.
	if(fread(magicbuf, 1, PNG_MAGIC_SIZE, file) != PNG_MAGIC_SIZE)
	{
		if(ferror(file))
			errormessage = "Error while reading file.";
		else if(feof(file))
			errormessage = "File ends in middle of magic.";
		else
			errormessage = "Unknown error";
		return false;
	}
	for(unsigned i = 0; i < PNG_MAGIC_SIZE; i++)
		if(magicbuf[i] != rightmagic[i])
		{
			errormessage = "File is not a PNG image.";
			return false;
		}

	//PNG file must have IHDR chunk as first. check its validity and
	//and the fact it really is IHDR.
	if(!is_valid_chunk(file, chunktype, y))
	{
		//Error is set.
		return false;
	}
	for(unsigned i = 0; i < PNG_CHUNK_TYPE_SIZE; i++)
		if(chunktype[i] != IHDR[i])
		{
			errormessage = "First chunk is not IHDR.";
			return false;
		}
		

	//Check rest, until we find IEND or bad chunk.
	bool found_iend = false;
	bool found_nilm = false;
	do
	{
		if(!is_valid_chunk(file, chunktype, y))
		{
			//Error is set.
			return false;
		}
		//Set this temporarily, it is easier to detect if it is not
		//IEND than to detect if it is...
		found_iend = true;
		found_nilm = true;
		for(unsigned i = 0; i < PNG_CHUNK_TYPE_SIZE; i++)
		{
			if(chunktype[i] != IEND[i])
				found_iend = false;
			if(chunktype[i] != niLM[i])
				found_nilm = false;

			if(found_nilm)
				value = y;
		}
	} while(!found_iend && !found_nilm);

	return value;
}

///////////////////////////////////////////////////////////////////////////////
bool is_valid_png(FILE* file)
{
	unsigned char chunktype[PNG_CHUNK_TYPE_SIZE];
	unsigned char magicbuf[PNG_MAGIC_SIZE];
	int y;

	//First, read the magic from file.
	if(fread(magicbuf, 1, PNG_MAGIC_SIZE, file) != PNG_MAGIC_SIZE)
	{
		if(ferror(file))
			errormessage = "Error while reading file.";
		else if(feof(file))
			errormessage = "File ends in middle of magic.";
		else
			errormessage = "Unknown error";
		return false;
	}
	for(unsigned i = 0; i < PNG_MAGIC_SIZE; i++)
		if(magicbuf[i] != rightmagic[i])
		{
			errormessage = "File is not a PNG image.";
			return false;
		}

	//PNG file must have IHDR chunk as first. check its validity and
	//and the fact it really is IHDR.
	if(!is_valid_chunk(file, chunktype, y))
	{
		//Error is set.
		return false;
	}
	for(unsigned i = 0; i < PNG_CHUNK_TYPE_SIZE; i++)
		if(chunktype[i] != IHDR[i])
		{
			errormessage = "First chunk is not IHDR.";
			return false;
		}
		

	//Check rest, until we find IEND or bad chunk.
	bool found_iend = false;
	do
	{
		if(!is_valid_chunk(file, chunktype, y))
		{
			//Error is set.
			return false;
		}
		//Set this temporarily, it is easier to detect if it is not
		//IEND than to detect if it is...
		found_iend = true;
		for(unsigned i = 0; i < PNG_CHUNK_TYPE_SIZE; i++)
			if(chunktype[i] != IEND[i])
				found_iend = false;
	} while(!found_iend);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
struct png_read_state* start_read_png_paletted(FILE* file)
{
	struct png_read_state* state = (struct png_read_state*)
		malloc(sizeof(struct png_read_state));
	if(!state)
	{
		errormessage = "Not enough memory to allocate PNG reading "
			"context.";
		return NULL;
	}
	
	//Create reader structure.
	state->png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, 
		NULL);
	if(!state->png)
	{
		errormessage = "Can't create LIBPNG PNG reading context "
			"structure.";
		goto error_state;
	}
		
	//Create info structure.
	state->info = png_create_info_struct(state->png);
	if(!state->info)
	{
		errormessage = "Can't create LIBPNG PNG information "
			"structure.";
		goto error_info;
	}

	//Set up error handling.
	if(setjmp(state->png->jmpbuf))
	{
		//Errormessage set (or not)
		if(errormessage == NULL)
			errormessage = "LIBPNG encountered fatal error trying "
				"to read PNG image.";
		goto error_info;
	}

	//Set up reader.
	png_init_io(state->png, file);

	//Read PNG headers.
	png_read_info(state->png, state->info);
	png_get_IHDR(state->png, state->info, &(state->width), 
		&(state->height), &(state->depth), &(state->colortype),
		&(state->interlace), NULL, NULL);

	//No, we don't want nonpaletted images or interlaced images.
	if(state->colortype != PNG_COLOR_TYPE_PALETTE || 
		state->interlace != PNG_INTERLACE_NONE)
	{
		errormessage = "Image must be paletted and noninterlaced.";
		goto error_info;
	}
	
	//Set packing so we don't get packed pixels, and update the info.
	png_set_packing(state->png);
	png_read_update_info(state->png, state->info);
	png_get_IHDR(state->png, state->info, &(state->width), 
		&(state->height), &(state->depth), &(state->colortype),
		&(state->interlace), NULL, NULL);

	//This far it can be done in initializer.
	return state;
error_info:
	png_destroy_read_struct(&(state->png), &(state->info), NULL);
	goto error_state;
error_png:
	png_destroy_read_struct(&(state->png), NULL, NULL);
error_state:
	free(state);
	return NULL;	
}


///////////////////////////////////////////////////////////////////////////////
struct png_read_state* start_read_png_freeform(FILE* file)
{
	struct png_read_state* state = (struct png_read_state*)
		malloc(sizeof(struct png_read_state));
	if(!state)
	{
		errormessage = "Not enough memory to allocate PNG reading "
			"context.";
		return NULL;
	}
	
	//Create reader structure.
	state->png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, 
		NULL);
	if(!state->png)
	{
		errormessage = "Can't create LIBPNG PNG reading context "
			"structure.";
		goto error_state;
	}

	//Create info structure.
	state->info = png_create_info_struct(state->png);
	if(!state->info)
	{
		errormessage = "Can't create LIBPNG PNG information "
			"structure.";
		goto error_info;
	}

	//Set up error handling.
	if(setjmp(state->png->jmpbuf))
	{
		//Errormessage set (or not)
		if(errormessage == NULL)
			errormessage = "LIBPNG encountered fatal error trying "
				"to read PNG image.";
		goto error_info;
	}

	//Set up reader.
	png_init_io(state->png, file);

	//Read PNG headers.
	png_read_info(state->png, state->info);
	png_get_IHDR(state->png, state->info, &(state->width), 
		&(state->height), &(state->depth), &(state->colortype),
		&(state->interlace), NULL, NULL);

	//No, we don't want interlaced images.
	if(state->interlace != PNG_INTERLACE_NONE)
	{
		errormessage = "Image must be noninterlaced.";
		goto error_info;
	}

	//Set packing so we always get RRGGBBAA, and update the info.
	switch(state->colortype)
	{
		case PNG_COLOR_TYPE_GRAY:
			//To 8-bits pixel RGB and filler after -> RGBA.
			png_set_strip_16(state->png);
			png_set_packing(state->png);
			png_set_gray_to_rgb(state->png);
			png_set_filler(state->png, 255, PNG_FILLER_AFTER);
			break;
		case PNG_COLOR_TYPE_RGB:
			//To 8-bits pixel and filler after -> RGBA.
			png_set_strip_16(state->png);
			png_set_filler(state->png, 255, PNG_FILLER_AFTER);
			break;
		case PNG_COLOR_TYPE_PALETTE:
			//Expand palette.
			if(png_get_valid(state->png, state->info, 
					PNG_INFO_tRNS))
				png_set_expand(state->png);
			break;			
		case PNG_COLOR_TYPE_GA:
			//To 8-bits pixel and expand to rgb -> RGBA.
			png_set_strip_16(state->png);
			png_set_gray_to_rgb(state->png);
			break;			
		case PNG_COLOR_TYPE_RGBA:
			//To 8-bits pixel.
			png_set_strip_16(state->png);
			break;			
	}

	png_read_update_info(state->png, state->info);
	png_get_IHDR(state->png, state->info, &(state->width), 
		&(state->height), &(state->depth), &(state->colortype),
		&(state->interlace), NULL, NULL);


	//This far it can be done in initializer.
	return state;
error_info:
	png_destroy_read_struct(&(state->png), &(state->info), NULL);
	goto error_state;
error_png:
	png_destroy_read_struct(&(state->png), NULL, NULL);
error_state:
	free(state);
	return NULL;	
}

///////////////////////////////////////////////////////////////////////////////
int read_png_scanline(struct png_read_state* state, unsigned char* buffer)
{
	if(setjmp(state->png->jmpbuf))
	{
		if(errormessage == NULL)
			errormessage = "LIBPNG encountered fatal error trying "
				"to read PNG image.";
		return -1;
	}

	png_read_row(state->png, buffer, NULL);
	return 0;	
}

///////////////////////////////////////////////////////////////////////////////
void end_read_png(struct png_read_state* state)
{
	png_destroy_read_struct(&(state->png), &(state->info), NULL);
	free(state);
}

