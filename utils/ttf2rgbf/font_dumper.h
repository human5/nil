/***************************************************************************
                          font_dumper.h  -  description
                             -------------------
    begin                : Sun Jan 30 2000
    copyright            : (C) 2000 by Flemming Frandsen
    email                : dion@swamp.dk

    This program is free software; you can redistribute it and/or modify  
    it under the terms of the GNU General Public License as published by  
    the Free Software Foundation; either version 2 of the License, or     
    (at your option) any later version.                                   
                                                                          
 ***************************************************************************/

#ifndef FONT_DUMPER_H
#define FONT_DUMPER_H

#include <vector.h>
#include <freetype.h>
#include <stdio.h>
#include "../../src/raw_surface.h"


//A single character mapping extracted from the font
//I only understand unicode and ascii (aka roman)
typedef struct Char_mapping {
	typedef enum Type {
		UNICODE,
		ASCII
	};

	Type type;
	TT_CharMap mapping;	
};

typedef std::vector<Char_mapping> Char_mappings;

typedef struct Glyph_spec {
	unsigned char ch;
	int bearing_x;
	int bearing_y;
	int advance;
};

typedef std::vector<Glyph_spec> Glyph_specs;

class Font_dumper {
public: 
	Font_dumper();
	~Font_dumper();

	int init(char *font_file, int height);
	int dump(char ch, char *rgbf_file);
	int dump_spec(char *spec_file);

protected:
	void add_mapping(TT_Face face, int index, Char_mapping::Type type);
	TT_UShort get_glyph_index(char ch);
	int find_mappings(TT_Face face);

	Char_mappings char_mappings;
	Glyph_specs glyph_specs;

	TT_Glyph		glyph;
  TT_Engine 	engine;
  TT_Face  		face;
  TT_Instance instance;
};

#endif
