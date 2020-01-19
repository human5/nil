/***************************************************************************
  animator.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-01-19
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-28
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "client/animation/animator.h"
#include "client/animation/animation.h"
#include "client/animation/anim_helpers.h"
#include "common/console/logmsg.h"
#include "common/vfs/loader.h"
#include "common/misc.h"
#include "common/nil_math.h"
#include <vector>
#include <map>
#include <string>

Animator::Animator() {
	last_frame_change = -1;
	curtime = -1;
	current_instruction = -1;
	current_frame_id = -1;
}

Animator::~Animator() {}

bool Animator::create(Animation *animation_) {
	animation = animation_;
	return true;
}

Graphic *Animator::get_frame(int frame_id, int aim_id, int dir) {
	Anim_graphic_key gk;
	gk.set(frame_id,aim_id,dir);               	

	Anim_graphics::iterator frame = animation->loaded_graphics->find(gk);
	if (frame != animation->loaded_graphics->end())
		return frame->second;
	else {
		logmsg(lt_error,"Unable to load the frame: %i:%i:%i",frame_id,aim_id,dir);
		return NULL;
	}
}

Graphic *Animator::get_current_frame(float newtime, float elevation, int dir) {
	Anim_aimangle *aimangle = animation->aimangles->getaim(elevation);
	if (aimangle) {
		return get_current_frame(newtime, aimangle->aim_id, dir);
	} else {
		return NULL;
	}
}

Graphic *Animator::get_current_frame(float newtime) {
	return get_current_frame(newtime, -1, 0);
}

Graphic *Animator::get_current_frame(float newtime, int aim_id, int dir) {
	if (current_instruction < 0) return NULL;
	if (curtime == -1)
	last_frame_change = newtime;
	curtime = newtime;

	//advance through instructions until curtime-last_frame_change < instruction->duration
	bool keep_going = true;
	do {
		switch ((*animation->instructions)[current_instruction].opcode) {

			case (Anim_instruction::STOP): {
				keep_going = false;
				//logmsg(lt_debug,"AS: halted at %i",current_instruction);		
				break;
			}

			case (Anim_instruction::GOTO): {
				goto_instruction((*animation->instructions)[current_instruction].next);
				//logmsg(lt_debug,"AS: goto %i",current_instruction);		
				break;
			}

			case (Anim_instruction::FRAME): {
				//make sure that the current frame is available
				current_frame_id = (*animation->instructions)[current_instruction].frame_id;
			
				//handle optimized halts:
				if ((*animation->instructions)[current_instruction].next == current_instruction) {
					//logmsg(lt_debug,"AS: Halted at %i",current_instruction);		
					keep_going = false;
				}

				//see if we should advance to the next instruction:
				float delay = (*animation->instructions)[current_instruction].delay;
				float frame_age = (curtime-last_frame_change);
				if (frame_age > delay) {
					last_frame_change += delay;
					goto_instruction((*animation->instructions)[current_instruction].next);
					//logmsg(lt_debug,"AS: moving to %i",current_instruction);
				} else
					keep_going = false;

				break;
			}

		} //switch (instructions[current_instruction].opcode) {

	} while (keep_going);

	return get_frame(current_frame_id, aim_id, dir);	
}

bool Animator::goto_instruction(int instruction_index) {
	if (0 > instruction_index || (unsigned)instruction_index >= animation->instructions->size()) {
		logmsg(lt_error,"Unable to goto instruction %i",instruction_index);
		return false;
	}
	current_instruction = instruction_index;
	return true;
}

bool Animator::start_sequence(char *sequence_name)
{
	Anim_sequences::iterator s = animation->sequences->find(sequence_name);
	if (s != animation->sequences->end() && s->second.size() > 0) {
		int version_index = rnd(0,s->second.size()-1);
		curtime = -1;
		goto_instruction(s->second[version_index]);	
		return true;
	} else {
		logmsg(lt_debug,"sequence %s not found, sticking with the current one",sequence_name);
	}
	return false;
}
