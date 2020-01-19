/***************************************************************************
  animation.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-08
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef ANIMATION_H
#define ANIMATION_H

#include "client/graphics/graphic.h"
#include "client/graphics/graphics.h"

const int32 NOCOLOR = 0x01010101;
const int AIMFRAME_OVERSIZE = 100;

//skins/<skin>/<frame>.<direction><aim code>.frame.rgbf

class Anim_sequences;
class Anim_aimangles;
class Anim_instructions;
class Anim_graphics;
class Animator;

//! @author Flemming Frandsen
//! @brief Class for handling animations
class Animation {
public:
	//! tell that we like Animator Class
	friend class Animator;
	//! Constructor that creates sequences, aimangles, instructions and loaded_graphics
	Animation();
	//! Destructor that deletes sequences, aimangles, instructions and loaded_graphics
	~Animation();
	//! Loads the script from file
	bool create(Loader *loader_, const char *name, int32 color = NOCOLOR);

	//Returns the frame that should currently be used
	/*
	//for skins that have a direction:
	Graphic *get_current_frame(float newtime, float elevation, int dir);
	//for direction less animations:
	//  Graphic *get_current_frame(float curtime_);
	//Finds a random sequence by this name and jumps to it
	bool start_sequence(char *sequence_name);
	*/
protected:
	//! reads the script
	bool read_script(char *root, char *script);
	//! loads a frame
	bool load_frame(char *frame_name, int frame_id, int aim_id, int dir);
	//! flips a frame
	bool flip_frame(int frame_id, int aim_id, int dir);
	//---------------------------------------------------------------------------
	//different pr instance:
	/*
	//the time the current frame started
	float last_frame_change;
	//the time the last frame was returned.
	float curtime;
	int current_instruction;
	int current_frame_id;
	*/
	//---------------------------------------------------------------------------
	//Constant after load:
	//! ???
	Anim_sequences *sequences;
	//! ???
	Anim_aimangles *aimangles;
	//! ???
	Anim_instructions *instructions;
	//! ???
	Anim_graphics *loaded_graphics;
	//---------------------------------------------------------------------------
	//Only used during load:
	//! ???
	int32 player_color;
	//! ???
	Loader *loader;
};

#endif
