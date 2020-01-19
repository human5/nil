/***************************************************************************
  rgbf2png.cpp  - Convert RGBFs to PNGs.
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
#include "write-png.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define VERSION 0.9.1
#define VERSION2(VERSION3) #VERSION3

char* errormessage;

int main(int argc, char** argv)
{
	errormessage = NULL;

	printf("RGBF->PNG converter, Version %s.\n", VERSION2(VERSION));
	if(argc == 1)
		fprintf(stderr, "Nothing to do; give basenames to convert on "
			"command line.\n");

	for(int i = 1; i < argc; i++)
	{
		int opaque_type = 16;			//16 is wildcard.
		size_t argsize = strlen(argv[i]);

		char* image_buffer;
		char* type_buffer;

		rgbf_header h;
		png_compress_context* ctx_image = NULL;
		png_compress_context* ctx_type = NULL;
		image_data _image_data;
		image_data _type_data;

		char* inname = (char*)alloca(argsize + 6);   	//".rgbf"
		char* outiname = (char*)alloca(argsize + 11);   //".image.png"
		char* outtname = (char*)alloca(argsize + 10);   //".type.png"

		strcpy(inname, argv[i]);
		strcat(inname, ".rgbf");

		strcpy(outiname, argv[i]);
		strcat(outiname, ".image.png");

		strcpy(outtname, argv[i]);
		strcat(outtname, ".type.png");

		FILE* input = fopen(inname, "rb");
		FILE* output_image = fopen(outiname, "wb");
		FILE* output_type = fopen(outtname, "wb");

		if(!input)
		{
			fprintf(stderr, "Failed to open \"%s\".\n", inname);
			goto close_files;
		}

		if(!output_image)
		{
			fprintf(stderr, "Failed to open \"%s\".\n", outiname);
			goto close_files;
		}

		if(!output_type)
		{
			fprintf(stderr, "Failed to open \"%s\".\n", outtname);
			goto close_files;
		}


		h = read_rgbf_header(input);
		if(h.format == -1)
		{
			fprintf(stderr, "Failed to read header of \"%s\": "
				"%s\n", inname, errormessage);
			goto close_files;
		}

		_image_data.width = h.width;
		_image_data.height = h.height;
		_image_data.uses_palette = false;

		_type_data.width = h.width;
		_type_data.height = h.height;
		_type_data.uses_palette = true;
		_type_data.palette[0].red = 0;
		_type_data.palette[0].green = 0;
		_type_data.palette[0].blue = 0;
		_type_data.palette[1].red = 255;
		_type_data.palette[1].green = 0;
		_type_data.palette[1].blue = 0;
		_type_data.palette[2].red = 0;
		_type_data.palette[2].green = 255;
		_type_data.palette[2].blue = 0;
		_type_data.palette[3].red = 0;
		_type_data.palette[3].green = 0;
		_type_data.palette[3].blue = 255;

		image_buffer = (char*)alloca(4 * h.width);
		type_buffer = (char*)alloca(1 * h.width);
		
		ctx_image = save_png_header(output_image, &_image_data);
		if(!ctx_image)
		{
			fprintf(stderr, "Failed to write header and create "
				"context of \"%s\": %s\n", outiname, 
				errormessage);
			goto close_files_destroy_streams;
		}

		ctx_type = save_png_header(output_type, &_type_data);
		if(!ctx_type)
		{
			fprintf(stderr, "Failed to write header and create "
				"context of \"%s\": %s\n", outtname, 
				errormessage);
			goto close_files_destroy_streams;
		}

		for(unsigned i = 0; i < h.height; i++)
		{
			if(read_rgbf_scanline(input, image_buffer, type_buffer,
				h.width, opaque_type))
			{
				fprintf(stderr, "Failed to read scanline from "
					"\"%s\": %s\n", inname, errormessage);
				goto close_files_destroy_streams;
			}

			if(write_png_scanline(ctx_image, image_buffer) < 0)
			{
				fprintf(stderr, "Failed to write scanline to "
					"\"%s\": %s\n", outiname, 
					errormessage);
				goto close_files_destroy_streams;
			}

			if(write_png_scanline(ctx_type, type_buffer) < 0)
			{
				fprintf(stderr, "Failed to write scanline to "
					"\"%s\": %s\n", outtname, 
					errormessage);
				goto close_files_destroy_streams;
			}
		}

		if(finish_png(ctx_image) < 0)
		{
			fprintf(stderr, "Failed to finish PNG file \"%s\": "
				"%s\n", outiname, errormessage);
			finish_png(ctx_type);
			goto close_files;
		}

		ctx_type->singletype = opaque_type;
		if(finish_png(ctx_type) < 0)
		{
			fprintf(stderr, "Failed to finish PNG file \"%s\": "
				"%s\n", outtname, errormessage);
			goto close_files;
		}

		if(opaque_type == -1)
			printf("\"%s\" converted, no single type in nonopaque "
				"pixels.\n", inname);
		else
			printf("\"%s\" converted, all nonopaque pixels are of "
				"type %i.\n", inname, opaque_type);
		goto close_files;

close_files_destroy_streams:
		if(ctx_image)
			finish_png(ctx_image);
		if(ctx_type)
			finish_png(ctx_type);
close_files:
		if(input)
			fclose(input);
	}
}
