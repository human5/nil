/***************************************************************************
  write-rgbf.cpp  -  Write RGBF files.
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
#include "write-rgbf.h"
#include <cstdlib>

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
	void write_unsigned(char* addr, unsigned number)
	{
		addr[0] = (number >> 24) & 0xFF;
		addr[1] = (number >> 16) & 0xFF;
		addr[2] = (number >>  8) & 0xFF;
		addr[3] = (number >>  0) & 0xFF;
	}
}

///////////////////////////////////////////////////////////////////////////////
int write_rgbf_header(FILE* file, unsigned width, unsigned height)
{
	char header[12];

	write_unsigned(header + 0, 0xBABE0004);
	write_unsigned(header + 4, width);
	write_unsigned(header + 8, height);

	if(fwrite(header, 1, 12, file) == 12)
		return 0;

	errormessage = "Failed to write RGBF file header.";
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
int write_rgbf_scanline(FILE* file, unsigned char* imageline, 
	unsigned char* typeline, unsigned width, int forcetype)
{
	unsigned char* scanline = (unsigned char*)alloca(width * 4);
	unsigned char ftype = forcetype;

	for(unsigned i = 0; i < width; i++)
	{
		scanline[4 * i + 0] = imageline[4 * i + 0];
		scanline[4 * i + 1] = imageline[4 * i + 1];
		scanline[4 * i + 2] = imageline[4 * i + 2];
		if(forcetype < 0)
			scanline[4 * i + 3] = (imageline[4 * i + 3] & 0xF0) | 
				(typeline[i]);
		else if(imageline[4 * i + 3])
			scanline[4 * i + 3] = (imageline[4 * i + 3] & 0xF0) |
				ftype;
		else
			scanline[4 * i + 3] = (imageline[4 * i + 3] & 0xF0);
	}

	if(fwrite(scanline, 1, 4 * width, file) == 4 * width)
		return 0;

	errormessage = "Failed to write RGBF file scanline.";
	return -1;
}
