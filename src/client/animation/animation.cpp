/***************************************************************************
  animation.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-08
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "client/animation/animation.h"

#include "common/console/logmsg.h"
#include "common/vfs/loader.h"
#include "common/misc.h"
#include "common/nil_math.h"
#include "common/vfs/loader.h"
#include "client/animation/anim_helpers.h"

#include <vector>
#include <map>
#include <string>


/*
skins/<skin>/<frame>.<direction><elevation code>.frame.rgbf
	1. pass
		save labels (- index + name)
		load+transform graphics
		save distinct aimframes
		save sequences
		save instructions with -indexes to labels
	2. pass
		change label jumps from -indexes to direct next jumps
		change sequence jumps from -indexes to direct next jumps

	create a uniformly distributed oversize array for fast lookup of aimframes

	output to an animator:
		1) instructions
		2) graphics
		3) sequences
		4) aimframes
		5) aimframe lookup table
*/

Animation::Animation() {
	sequences = new Anim_sequences;
	aimangles = new Anim_aimangles;
	instructions = new Anim_instructions;
	loaded_graphics = new Anim_graphics;
}

Animation::~Animation() {
	Anim_graphics::iterator i=loaded_graphics->begin();
	while (i!=loaded_graphics->end()) {
		delete i->second;
		i++;
	}
	delete sequences;
	delete aimangles;
	delete instructions;
	delete loaded_graphics;
}

bool Animation::create(Loader *loader_, char *name, int32 color) {
	player_color = color;
	loader = loader_;

	int filesize = loader->filesize(name);
	
	if (filesize <= 0) {
		logmsg(lt_error,"Unable to find the animator script %s",name);
		return false;
	}

	char root[200];
	strcpy(root,name);
	char *last_slash = strrchr(root,'/');
	if (last_slash)
		*(last_slash+1) = 0;
	else
		root[0] = 0;

	bool result = false;
	char *script = (char *)malloc(filesize+1);
	if (loader->fileread(name,script,filesize,0)) {
		logmsg(lt_debug,"Loading the script %s and loading graphics from %s",name,root);
		script[filesize] = 0;
		result = read_script(root,script);		
	} else {
	result = false;
		logmsg(lt_error,"Unable to load the script %s",name);
	}
	free(script);
	
	return result;
}

bool Animation::read_script(char *root, char *script) {
	char flip_code = 'n';

	Anim_labels	labels;
	labels.tmp_index = -1;

	Anim_handle_maker frame_handles;
	Anim_handle_maker aim_handles;	

	//translate the tokens to Instructions.
	Anim_tokenizer tokenizer(script);	
	Anim_token *cur = tokenizer.getcurrent();
    // XXX Unused variable
    //Anim_token *next = tokenizer.getnext();
	bool error = false;
	while (cur->gettype() != Anim_token::THEEND && cur->gettype() != Anim_token::ANIMERROR) {
		int line_number = cur->getline();

		switch (cur->gettype()) {

			//-----------------------------------------
			case Anim_token::FLIP: { // direction (r,l,no)
				flip_code = cur->getparameter(0)[0];
				logmsg(lt_debug,"AS %03i flip %c",line_number,flip_code);
				break;
			}

			//-----------------------------------------
			case Anim_token::SEQUENCE: {	// name, label
				char *name = cur->getparameter(0);
				char *label = cur->getparameter(1);
				int label_index = labels.get_label_index(label);
				logmsg(lt_debug,"AS %03i sequence %s %s(%i)",line_number,name,label,label_index);

				Anim_sequences::iterator i=sequences->find(name);
				if (i != sequences->end()) {
					i->second.push_back(label_index);
				} else {
					Anim_sequence_versions versions;
					versions.push_back(label_index);
					sequences->insert(std::pair<std::string, Anim_sequence_versions>(name,versions));
				}				
       	
				break;
			}

			//-----------------------------------------
			case Anim_token::AIMANGLE:{	// elevation deg, elevation code
				float elev_deg = -1;
				if (sscanf(cur->getparameter(0),"%f",&elev_deg) != 1) {
					logmsg(lt_error,"Unable to convert parameter 1  (\"%s\") of the aimangle command in line %i to a float",cur->getparameter(0),line_number);
					error = true;
					break;
				}
				int aimcode = aim_handles.get_handle(cur->getparameter(1));

				Anim_aimangle aimangle;	

				aimangle.elevation = PI*elev_deg/180;
				aimangle.aim_id = aimcode;
				aimangles->push_back(aimangle);
				logmsg(lt_debug,"AS %03i aimangle %.2f %i (%i)",line_number,elev_deg,aimcode,aimangles->size()-1);

				break;
			}

			//-----------------------------------------
			case Anim_token::LABEL:{	// label
				char *label = cur->getparameter(0);

				logmsg(lt_debug,"AS %03i label %s",line_number,label);

				labels.set_label_index(label,instructions->size());

				break;
			}

			//-----------------------------------------
			case Anim_token::FRAME:{	// frame, noaim, ms delay
				char *frame = cur->getparameter(0);

				//aim
				bool aim = false;
				if (strcasecmp(cur->getparameter(1),"noaim") == 0)
					aim = false;
				else if (strcasecmp(cur->getparameter(1),"aim") == 0)
					aim = true;
				else {
					logmsg(lt_error,"At line %i there is a frame command with an <aim> field which is not valid, it is %s",line_number,cur->getparameter(1));
					error = true;
					break;
				}

				//delay
				int delay = -1;
				if (sscanf(cur->getparameter(2),"%i",&delay) != 1) {
					logmsg(lt_error,"At line %i there is a frame command with ac invalid <delay> field, it is %s",line_number,cur->getparameter(2));
					error = true;
					break;
				}

				logmsg(lt_debug,"AS %03i frame %s",line_number,frame);

				Anim_instruction instruction;
				instruction.opcode   = Anim_instruction::FRAME;
				instruction.frame_id = frame_handles.get_handle(frame,aim?1:2);
				instruction.delay    = delay/1000.0;
				instruction.next     = instructions->size()+1;
			
				instructions->push_back(instruction);

				break;
			}

			//-----------------------------------------
			case Anim_token::GOTO:{	// label
				char *label = cur->getparameter(0);
				int label_index = labels.get_label_index(label);

				logmsg(lt_debug,"AS %03i goto %s(%i)",line_number,label,label_index);

				Anim_instruction instruction;
				instruction.opcode   = Anim_instruction::GOTO;
				instruction.frame_id = -1;
				instruction.delay    = -1;
				instruction.next     = label_index;
			
				instructions->push_back(instruction);

				break;
			}

			//-----------------------------------------
			case Anim_token::STOP:{	// n/a
				logmsg(lt_debug,"AS %03i stop",line_number);

				Anim_instruction instruction;
				instruction.opcode   = Anim_instruction::STOP;
				instruction.frame_id = -1;
				instruction.delay    = -1;
				instruction.next     = -1;
			
				instructions->push_back(instruction);

				break;
			}

			case Anim_token::THEEND:
			case Anim_token::ANIMERROR:
			{
				//This cannot happen.
			}
			
		}

		tokenizer.next_token();
		cur = tokenizer.getcurrent();
        // XXX Unused variable
		//next = tokenizer.getnext();
		line_number++;
	}

	if (cur->gettype() == Anim_token::ANIMERROR) {
		logmsg(lt_error,"Error in line %i: %s",tokenizer.getcurrent()->getline(),tokenizer.getcurrent()->getparameter(0));
		return false;
	}

	if (error) {
		logmsg(lt_error,"Errors found in script, giving up");		
		return false;
	}

	logmsg(lt_debug,"Animation script loaded, resulting in %i instructions",instructions->size());

	//check if the last instruction was a goto or a stop (it has to be)
	Anim_instructions::iterator last=instructions->end()-1;
	if (last->opcode != Anim_instruction::STOP && last->opcode != Anim_instruction::GOTO) {
		logmsg(lt_error,"Error in animator script: the last instruction was not goto or stop.");		
		return false;  	
	}

	// run through all labels and check for undefined ones,
	// bitch and moan if one is still undefined.
	{
		Anim_labels::iterator i=labels.begin();
		while (i != labels.end()) {
			if (i->second->index < 0) {
				logmsg(lt_error,"Error in animator script: the label \"%s\" was not defined anywhere.",i->second->label);		
				return false;  	
			}
			i++;
		}
	}

	//go through sequences and fix up the label indexes
	{
		Anim_sequences::iterator s = sequences->begin();
		while (s != sequences->end()) {
			Anim_sequence_versions::iterator v=s->second.begin();
			while (v!=s->second.end()) {
				if (*v < 0)	*v = labels.tmp_index_to_index(*v);
				v++;
			}
			s++;
		}
	}

	//go through instructions and fix up the next's that are unsatisfied label indexes
	{
		Anim_instructions::iterator i=instructions->begin();
		while (i!=instructions->end()) {
			if (i->opcode == Anim_instruction::FRAME || i->opcode == Anim_instruction::GOTO) {
				if (i->next < 0) {
					i->next = labels.tmp_index_to_index(i->next);
				}
			}
			i++;
		}
	}

	//check aimframes for sanity
	{
		int hs = aim_handles.size();
		int fs = aimangles->size();
		if (hs != fs) {
			logmsg(lt_error,"ARGH! the number of aim_handles (%i) is different from the number of aimangles (%i)",hs,fs);
			return false;
		}
		logmsg(lt_debug,"AS: aim angles in skin: %i",fs);
	}

	//optimize code (copy stop.next and goto.next to prev instruction if it is a frame)

	//load the graphics and do the coloring and then flipping.
	Anim_handle_maker::iterator curframe=frame_handles.begin();
	while (curframe!=frame_handles.end()) {
		bool noaim = curframe->second.flags & 2;
		bool aim   = curframe->second.flags & 1;

		if (noaim) {
			char frame_name[200];
			sprintf(frame_name,"%s%s.rgbf",root,curframe->first.c_str());
			load_frame(frame_name, curframe->second.handle, -1, 0);
		}

		if (aim) {
			Anim_handle_maker::iterator curaim = aim_handles.begin();	
			while (curaim != aim_handles.end()) {
				char frame_name[200];
				int frame_id = curframe->second.handle;
				int aim_id = curaim->second.handle;
				if (flip_code != 'n') {
					int existing_dir = (flip_code=='r')?1:-1;
					sprintf(frame_name,"%s%s.%c%s.rgbf",root,curframe->first.c_str(),flip_code,curaim->first.c_str());
					load_frame(frame_name, frame_id, aim_id, existing_dir);
					//flip the frame to get the mirror image
					flip_frame(frame_id, aim_id, existing_dir);
				} else {
					sprintf(frame_name,"%s%s.r%s.rgbf",root,curframe->first.c_str(),curaim->first.c_str());
					load_frame(frame_name, frame_id, aim_id, 1);
					sprintf(frame_name,"%s%s.l%s.rgbf",root,curframe->first.c_str(),curaim->first.c_str());
					load_frame(frame_name, frame_id, aim_id, -1);
				}
				curaim++;
			}
		}
		curframe++;
	}

	//clean up the temp data
	Anim_labels::iterator i=labels.begin();
	while (i != labels.end()) {
		delete i->second;
		i++;
	}

	return true;
}

//-------------------------------------------------------------------
bool Animation::load_frame(char *frame_name, int frame_id, int aim_id, int dir)
{
	Graphic *graphic = loader->graphics.getgraphic(frame_name);
	if (!graphic) {
		logmsg(lt_error,"Argh! unable the load the animator frame %s",frame_name);
		return false;
	}

	Anim_graphic_key gk;
	gk.set(frame_id,aim_id,dir);               	
	if (loaded_graphics->find(gk) != loaded_graphics->end()) {
		logmsg(lt_error,"Argh! the animator frame %s was already loaded",frame_name);
		return false;	
	}

	//create a copy of the original frame, color it and insert it into our map.
	Graphic *frame = new Graphic;
	frame->create(graphic);
	
	if (player_color != NOCOLOR) {
		//apply the coloring to the graphics
		frame->apply_color_under_offset( player_color, 168 );
	}

	logmsg(lt_debug,"AS: Loaded %s for (%i:%i:%i)",frame_name,frame_id,aim_id,dir);
	loaded_graphics->insert(std::pair<Anim_graphic_key,Graphic*>(gk,frame));

	return true;
}

/*
//-------------------------------------------------------------------
Graphic *Animation::get_frame(int frame_id, int aim_id, int dir)
{
	Anim_graphic_key gk;
	gk.set(frame_id,aim_id,dir);               	

	Anim_graphics::iterator frame = loaded_graphics->find(gk);
	if (frame != loaded_graphics->end())
		return frame->second;
	else {
		logmsg(lt_error,"Unable to load the frame: %i:%i:%i",frame_id,aim_id,dir);
		return NULL;
	}
}
*/
//-------------------------------------------------------------------
bool Animation::flip_frame(int frame_id, int aim_id, int dir)
{
	Anim_graphic_key gk;
	gk.set(frame_id,aim_id,dir);
	Anim_graphics::iterator g = loaded_graphics->find(gk);

	if (g == loaded_graphics->end()) {
		logmsg(lt_error,"Argh! unable to find the frame to flip");
		return false;
	}	

	gk.set(frame_id,aim_id,-dir);
	if (loaded_graphics->find(gk) != loaded_graphics->end()) {
		logmsg(lt_error,"Argh! the flipped frame could not be stored, a frame was already loaded in it's place");
		return false;	
	}

	Graphic *graphic = new Graphic;
	int xsize = g->second->get_xsize();
	int ysize = g->second->get_ysize();
	graphic->create(xsize,ysize);
	const int32 *source = g->second->get_pixels_read();
	int32 *target = graphic->get_pixels_write();

	for (int y=0;y<ysize;y++)
		for (int x=0;x<xsize;x++)
			target[(xsize-1-x)+y*xsize] = source[x+y*xsize];
	loaded_graphics->insert(std::pair<Anim_graphic_key,Graphic*>(gk,graphic));
	return true;
}
