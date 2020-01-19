/***************************************************************************
  png2rgbf.cpp  - Convert PNGs to RGBFs.
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
#include "write-rgbf.h"

#define VERSION 0.9.1
#define VERSION2(VERSION3) #VERSION3

char* errormessage;

int main(int argc, char** argv)
{
	errormessage = NULL;

	printf("PNG->RGBF converter, Version %s.\n", VERSION2(VERSION));
	if(argc == 1)
		fprintf(stderr, "Nothing to do; give basenames to convert on "
			"command line.\n");

	for(int i = 1; i < argc; i++)
	{
		int opaque_type = -1;		//-1 is none.
		size_t argsize = strlen(argv[i]);

		char* image_buffer;
		char* type_buffer;

		struct png_read_state* inistate;
		struct png_read_state* intstate = NULL;

		char* outname = (char*)alloca(argsize + 6);	//".rgbf"
		char* ininame = (char*)alloca(argsize + 11);	//".image.png"
		char* intname = (char*)alloca(argsize + 10);	//".type.png"

		strcpy(outname, argv[i]);
		strcat(outname, ".rgbf");

		strcpy(ininame, argv[i]);
		strcat(ininame, ".image.png");

		strcpy(intname, argv[i]);
		strcat(intname, ".type.png");

		FILE* inifile;
		FILE* intfile;
		FILE* output = fopen(outname, "wb");
		if(!output)
		{
			fprintf(stderr, "Failed to open \"%s\".\n", outname);
			continue;
		}
		
		inifile = fopen(ininame, "rb");
		if(!inifile)
		{
			fprintf(stderr, "Failed to open \"%s\": %s\n", ininame,
				errormessage);
			goto close_out;
		}

		intfile = fopen(intname, "rb");
		if(intfile == NULL)
		{
			fprintf(stderr, "Failed to open \"%s\": %s\n", intname,
				errormessage);
			goto close_ini;
		}

		if(!is_valid_png(inifile))
		{
			fprintf(stderr, "\"%s\" is not valid PNG file: %s\n", 
				ininame, errormessage);
			goto close_int;
		}
		rewind(inifile);

		if(!is_valid_png(intfile))
		{
			fprintf(stderr, "\"%s\" is not valid PNG file: %s\n", 
				intname, errormessage);
			goto close_int;
		}
		rewind(intfile);

		opaque_type = read_png_with_niLM(intfile);
		if(opaque_type >= 0)
		{
			//We won't need this.
			fclose(intfile);
			intfile = NULL;
		}
		else
		{
			rewind(intfile);
			intstate = start_read_png_paletted(intfile);
			if(!intstate)
			{
				fprintf(stderr, "Failed to initialize reader "
					"for \"%s\": %s\n", intname, 
					errormessage);
				goto close_int;
			}
		}
	
		inistate = start_read_png_freeform(inifile);
		if(!inistate)
		{
			fprintf(stderr, "Failed to initialize reader for "
				"\"%s\": %s\n", ininame, errormessage);
			goto close_ints;
		}

		if(opaque_type < 0 && (inistate->width != intstate->width && 
			inistate->height != intstate->height))
		{
			fprintf(stderr, "Sizes of \"%s\" and \"%s\" don't "
				"match.\n", ininame, intname);
			goto close_inis;
		}

		image_buffer = (char*)alloca(inistate->width * 4);
		if(opaque_type < 0)
			type_buffer = (char*)alloca(intstate->width);

		if(write_rgbf_header(output, inistate->width, 
			inistate->height))
		{
			fprintf(stderr, "Failed to write header of \"%s\": "
				"%s\n", outname, errormessage);
			goto close_inis;
		}

		for(unsigned ik = 0; ik < inistate->height; ik++)
		{
			if(read_png_scanline(inistate, 
				(unsigned char*)image_buffer))
			{
				fprintf(stderr, "Failed to read row from "
					"\"%s\": %s\n", ininame, errormessage);
				goto close_inis;
			}

			if(opaque_type < 0 && read_png_scanline(intstate, 
				(unsigned char*)type_buffer))
			{
				fprintf(stderr, "Failed to read row from "
					"\"%s\": %s\n", intname, errormessage);
				goto close_inis;
			}

			if(write_rgbf_scanline(output, 
				(unsigned char*)image_buffer, 
				(unsigned char*)type_buffer, inistate->width,
				 opaque_type))
			{
				fprintf(stderr, "Failed to write row to \"%s\""
					": %s\n", outname, errormessage);
				goto close_inis;
			}
		}

		printf("%s.*.png converted.\n", argv[i]);
close_inis:
		end_read_png(inistate);
close_ints:	
		if(intstate)
			end_read_png(intstate);
close_int:
		if(intfile != NULL)
			fclose(intfile);
close_ini:
		fclose(inifile);
close_out:
		fclose(output);

	}
}
