/***************************************************************************
                          font_dumper.cpp  -  description
                             -------------------
    begin                : Sun Jan 30 2000
    copyright            : (C) 2000 by Flemming Frandsen
    email                : dion@swamp.dk
                                                                          
    This program is free software; you can redistribute it and/or modify  
    it under the terms of the GNU General Public License as published by  
    the Free Software Foundation; either version 2 of the License, or     
    (at your option) any later version.                                   
                                                                          
 ***************************************************************************/

#include "font_dumper.h"

//============================================================================
Font_dumper::Font_dumper()
{
}

//============================================================================
Font_dumper::~Font_dumper()
{
	TT_Done_FreeType(engine);
}

//============================================================================
int Font_dumper::find_mappings(TT_Face face)
{
	int number_mappings = TT_Get_CharMap_Count(face);
	printf("The font has %i char mappings:\n",number_mappings);

	for (int i = 0; i < number_mappings; i++) {
		unsigned short platform = 0xffff;
		unsigned short encoding = 0xffff;
		TT_Get_CharMap_ID(face, i, &platform, &encoding);

		switch (platform) {
		
	 		//----------------------------------------------------------------------
   		case (0) : { //Apple
				printf("Platform: Apple (0) ");

        //All of the apple mappings seem to be unicode
				printf("Encoding: UNICODE (%i) ",encoding);
        add_mapping(face, i, Char_mapping::UNICODE);

				break;
			} 	

	 		//----------------------------------------------------------------------
			case (1): { //More Apple
				printf("Platform: Apple (1) ");

				if (encoding == 0) {
					printf("Encoding: ASCII (0) ");
  	      add_mapping(face, i, Char_mapping::ASCII);
        	
				} else {
        	printf("Encoding: Unknown (%i)",encoding);
				}

				break;
			}

	 		//----------------------------------------------------------------------
			case (2): { //ISO
				printf("Platform: ISO (2) ");

				switch (encoding) {
	       	case (0) : {
						printf("Encoding: ASCII (0) ");
  		      add_mapping(face, i, Char_mapping::ASCII);
						break;
					}
        	case (1) : {
						printf("Encoding: UNICODE (1) ");
  		      add_mapping(face, i, Char_mapping::UNICODE);
						break;
					}
        	default : {
	        	printf("Encoding: Unknown (%i)",encoding);
					}
				}

				break;
			}

	 		//----------------------------------------------------------------------
			case (3): { //Windows
				printf("Platform: Windows (3) ");

				switch (encoding) {
        	case (0) :
        	case (1) : {
						printf("Encoding: UNICODE (%i) ",encoding);
  		      add_mapping(face, i, Char_mapping::UNICODE);
						break;						
					}
        	default : {
	        	printf("Encoding: Unknown (%i)",encoding);
					}
				}

				break;
			}

			default: {
				printf("Platform: unknown (%i)",platform);
			}
		}

		printf("\n");
  }

	if (char_mappings.size() == 0)
    return -1;
	
	return 0;
}

//============================================================================
void Font_dumper::add_mapping(TT_Face face, int index, Char_mapping::Type type)
{
	Char_mapping mapping;
	mapping.type = type;
	
	int error = TT_Get_CharMap(face, index, &mapping.mapping);
	if (error) {
		printf("error TT_Get_CharMap() returned: %i",error);
  	return;
	}
	
	char_mappings.push_back(mapping);

	printf("added");
}

//============================================================================
TT_UShort Font_dumper::get_glyph_index(char ch)
{
	//try to find an ascii mapping
	Char_mappings::iterator i = char_mappings.begin();
	while (i != char_mappings.end() && i->type != Char_mapping::ASCII)
		i++;

	//if there were no ascii mapping, just take the first one
	if (i == char_mappings.end())
  	i = char_mappings.begin();

	//if we found a mapping at all:
	if (i != char_mappings.end()) {
    TT_UShort char_code = (unsigned)ch;
		return TT_Char_Index(i->mapping, char_code);
	} else
		return 0xffff;	
}

//============================================================================
int Font_dumper::init(char *font_file, int height)
{
	char_mappings.clear();
	glyph_specs.clear();

  int error = TT_Init_FreeType( &engine );
  if (error) {
		printf( "could not create engine instance\n");
    return 1;
 	}

  error = TT_Open_Face( engine, font_file, &face );
  if ( error ) {
		printf( "could not open the file\n" );
    return 2;
  }

  error = TT_New_Instance( face, &instance );
  if ( error ) {
		printf( "could not create instance\n" );
    return 3;
	}


	error = TT_Set_Instance_Resolutions(instance, 72, 72);
  if ( error ) {
		printf( "could not set dpi\n" );
    return 4;
	}

	error = TT_Set_Instance_CharSize( instance, height*64 );
  if ( error ) {
		printf( "could not set point size\n" );
    return 4;
	}

  error = find_mappings(face);
	if ( error ) {
		printf( "could not find a suitable character mapping\n" );
		return 5;
	}

	error = TT_New_Glyph( face, &glyph );
	if ( error ) {
		printf( "could not create glyph\n" );
		return 6;
	}

	//Set the palette:
	TT_Byte palette[5];
	palette[0] = 0;
  palette[1] = 4;
  palette[2] = 8;
  palette[3] = 12;
  palette[4] = 15;
 	TT_Set_Raster_Gray_Palette(engine, palette );
	
	return 0;
}

//============================================================================
int Font_dumper::dump(char ch, char *rgbf_file)
{
	//render a glyph
	TT_UShort glyph_index = get_glyph_index(ch);

	if (glyph_index == 0xffff) {
		int ich = ch;
		printf("could not find a glyph in the font for %c (%i), skipping\n",ch,ich);
    return -1;
	}

	int error = TT_Load_Glyph(instance, glyph, glyph_index, TTLOAD_DEFAULT );
  if ( error ) {
		printf("could not load the glyph\n");
    return 7;
  }

	TT_Glyph_Metrics metrics;
  TT_Get_Glyph_Metrics(glyph, &metrics );

	//fucked up weirdness here!
	TT_Raster_Map pixel_map;
  pixel_map.width = (metrics.bbox.xMax-metrics.bbox.xMin)/64; //xsize
  pixel_map.rows  = (metrics.bbox.yMax-metrics.bbox.yMin)/64; //xsize

	//All this crap should be done automagicly somewhere:
  pixel_map.cols = (pixel_map.width+3) & -4;
  pixel_map.flow = TT_Flow_Down;
  pixel_map.size = pixel_map.rows*pixel_map.cols;
  pixel_map.bitmap = malloc(pixel_map.size);
	memset(pixel_map.bitmap,0,pixel_map.size);

 	error = TT_Get_Glyph_Pixmap( glyph, &pixel_map, -metrics.bbox.xMin, -metrics.bbox.yMin );
	if ( error ) {
		printf("could not rasterize the glyph\n");
    return 8;
  }

	unsigned char *pix_map = (unsigned char *)pixel_map.bitmap;
	Raw_surface surface;
	surface.create(pixel_map.width,pixel_map.rows);
  for (int y=0;y<surface.get_ysize();y++) {

		unsigned char *pix = pix_map + y*pixel_map.cols;
    int32 *pixel = surface.get_pixels() + y*surface.get_xsize();

	  for (int x=0;x<surface.get_xsize();x++) {

			*pixel = PIXELA(255,255,255,*pix & 15);
			pix++;

			pixel++;
		}
	}

	if (!surface.save_to_file(rgbf_file)) return 9;

	//save the specs of this font:
	Glyph_spec spec;
	spec.advance = metrics.advance/64;
	spec.bearing_x = metrics.bearingX/64;
	spec.bearing_y = metrics.bearingY/64;
	spec.ch = ch;
	glyph_specs.push_back(spec);
	
	return 0;
}

//============================================================================
int Font_dumper::dump_spec(char *spec_file)
{
	FILE *f = fopen(spec_file,"w");

	if (!f) {
    printf("Unable to write spec to %s",spec_file);
		return 10;
	}

	Glyph_specs::iterator i=glyph_specs.begin();
	while (i!=glyph_specs.end()) {
		int ich = i->ch;
  	fprintf(f,"%i,%i,%i,%i\n",ich,i->advance,i->bearing_x,i->bearing_y);
		i++;
	}

	fclose(f);

	return 0;
}

