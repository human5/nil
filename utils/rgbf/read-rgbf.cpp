/***************************************************************************
  read-rgbf.cpp  - Read RGBF files.
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
#include "read-rgbf.h"
#include <cstring>
#include <cstdlib>

extern char* errormessage;

namespace
{
/**
 * Read unsigned in little-endian order from buffer. This function extracts 
 * little-endian (X86 byte order) 32-bit quantity from buffer.
 *
 * \param addr Pointer to bytes forming the number. At least 4 bytes should
 *	be addressable.
 *
 * \return The value read.
 *
 * \author H. Ilari Liusvaara
 *
 */
	unsigned read_unsigned_le(const char* addr)
	{
		unsigned n1 = (unsigned char)addr[0];
		unsigned n2 = (unsigned char)addr[1];
		unsigned n3 = (unsigned char)addr[2];
		unsigned n4 = (unsigned char)addr[3];

		return (n1) | (n2 << 8) | (n3 << 16) | (n4 << 24);
	}

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
	unsigned read_unsigned_be(const char* addr)
	{
		unsigned n1 = (unsigned char)addr[0];
		unsigned n2 = (unsigned char)addr[1];
		unsigned n3 = (unsigned char)addr[2];
		unsigned n4 = (unsigned char)addr[3];

		return (n4) | (n3 << 8) | (n2 << 16) | (n1 << 24);
	}
}

#define NIL_RGBF_HEAD_SIZE 12

///////////////////////////////////////////////////////////////////////////////
rgbf_header read_rgbf_header(FILE* file)
{
	rgbf_header h;
	h.format = -1;
	char headerbuffer[NIL_RGBF_HEAD_SIZE];

	if(fread(headerbuffer, 1, NIL_RGBF_HEAD_SIZE, file) != 
		NIL_RGBF_HEAD_SIZE)
	{
		if(feof(file))
			errormessage = "File truncated.";
		else if(ferror(file))
			errormessage = "Error reading header from file.";
		else
			errormessage = "Unknown error.";
		return h;
	}

	unsigned magic = read_unsigned_be(headerbuffer);
	if(magic == 0xBABE0004)
	{
		//New format.
		h.format = 2;
		h.width = read_unsigned_be(headerbuffer + 4);
		h.height = read_unsigned_be(headerbuffer + 8);
	}
	else if(magic == 0xEFBEADDE)
	{
		//Old format.
		h.format = 1;
		h.width = read_unsigned_le(headerbuffer + 4);
		h.height = read_unsigned_le(headerbuffer + 8);
	}
	else
	{
		//Bad magic.
		errormessage = "File is not RGBF v1 or v2 file.";
	}
	
	return h;
}

///////////////////////////////////////////////////////////////////////////////
int read_rgbf_scanline(FILE* file, char* imageline, char* typeline, 
	unsigned width, int& opaque_type)
{
	unsigned char* scanbuffer = (unsigned char*)alloca(width * 4);
	if(fread(scanbuffer, 1, 4 * width, file) != 4 * width)
	{
		if(feof(file))
			errormessage = "File truncated.";
		else if(ferror(file))
			errormessage = "Error reading scanline from file.";
		else
			errormessage = "Unknown error.";
		return -1;
	}

	if(imageline)
	{
		for(unsigned i = 0; i < width; i++)
		{
			imageline[4 * i + 0] = (char)scanbuffer[4 * i + 0];
			imageline[4 * i + 1] = (char)scanbuffer[4 * i + 1];
			imageline[4 * i + 2] = (char)scanbuffer[4 * i + 2];
			imageline[4 * i + 3] = (char)(scanbuffer[4 * i + 3] & 
				0xF0);
		}
	}
	if(typeline)
	{
		for(unsigned i = 0; i < width; i++)
		{
			typeline[i] = (char)(scanbuffer[4 * i + 3] & 0x0F);
			if(scanbuffer[4 * i + 3] & 0xF0 != 0)
				if(opaque_type != 16 && opaque_type != 
						typeline[i])
					opaque_type = -1;
				else if(opaque_type == 16)
					opaque_type = typeline[i];
		}
	}
	return 0;
}
