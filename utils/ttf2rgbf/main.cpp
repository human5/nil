/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sun Jan 30 13:46:39 CET 2000
    copyright            : (C) 2000 by Flemming Frandsen
    email                : dion@swamp.dk
                                                                          
    This program is free software; you can redistribute it and/or modify  
    it under the terms of the GNU General Public License as published by  
    the Free Software Foundation; either version 2 of the License, or     
    (at your option) any later version.                                   
                                                                          
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>

#include "font_dumper.h"

int main(int argc, char *argv[])
{
  printf("ttf2rgbf truetype pre-rasterrizer for NiL.\n");

	if (argc != 4) {
    printf("Syntax:\n");
    printf("ttf2rgbf <ttf file> <target dir> <height>\n");
    printf("<ttf file>  : The file containing the ttf font\n");
    printf("<target dir>: The name this font gets  <height>\n");
    printf("<height>    : The height of the font in pixels\n");
  	return 12;
	}	

	char *ttf_file 			= argv[1];
	char *target 				= argv[2];
	char *height_string = argv[3];
	int height = -1;
	if (sscanf(height_string,"%i",&height) != 1) {
		printf("Error unable to understand font height parameter: %s\n",height_string);
  	return 13;
	};
	
	Font_dumper dumper;

	dumper.init(ttf_file,height);

	for (int ch=31;ch < 128;ch++) {
		char fn[400];
		sprintf(fn,"%s/%i.rgbf",target,ch);
		dumper.dump(ch,fn);
	}

	char fn[400];
	sprintf(fn,"%s/font.spec",target);
	dumper.dump_spec(fn);

	printf("Font saved\n");

  return EXIT_SUCCESS;
}
