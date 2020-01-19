/***************************************************************************
  animator.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-01-19
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-28
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "client/graphics/graphic.h"

//! @author Flemming Frandsen
//! @brief Class for animation
class Animation;

//! @author Flemming Frandsen
//! @brief Class of animator
class Animator {
public:
	//! Constructor
	Animator();
	//! Destructor
	~Animator();
	//! Loads the script from file:
	bool create(Animation *animation_);
	//! Returns the frame that should currently be used for skins that have a direction:
	Graphic *get_current_frame(float newtime, float elevation, int dir);
	//! Returns the frame that should currently be used for directionless animations:
	Graphic *get_current_frame(float newtime);
	//! Finds a random sequence by this name and jumps to it
	bool start_sequence(const char *sequence_name);
protected:
	//! returns the frame that should currently be used
	Graphic *get_current_frame(float curtime_, int aim_id, int dir);
	//! returns the frame that should be used
	Graphic *get_frame(int frame_id, int aim_id, int dir);
	//! jumps to a specific instrruction
	bool goto_instruction(int instruction_index);
	//! the time the current frame started
	float last_frame_change;
	//! the time the last frame was returned.
	float curtime;
	//! ???
	int current_instruction;
	//! ???
	int current_frame_id;
	//! container for the animation
	Animation *animation;
};

#endif
