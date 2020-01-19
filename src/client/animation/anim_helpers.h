/***************************************************************************
  anim_helpers.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-08
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef ANIM_HELPERS_H
#define ANIM_HELPERS_H

#include "client/graphics/graphic.h"
#ifdef _WIN32
#include "common/win32_system.h"
#endif
#include <vector>
#include <map>
#include <string>

//! Animation sequences
class Anim_sequence_versions : public std::vector<int>{};
//! Animation sequences
class Anim_sequences : public std::map<std::string, Anim_sequence_versions>{};

//! aiming directions
typedef struct Anim_aimangle {
	//! ???
	float elevation;
	//! ???
	int aim_id;
} TD_anim_aimangle;

//! ???
class Anim_aimangles : public std::vector<Anim_aimangle> {
public:
	/*! ???
	    @param elevation ??? */
	Anim_aimangle *getaim(float elevation);
};

//! The final stored code
typedef struct Anim_instruction {
	//! The opcodes
	enum {
		//! The frame
		FRAME,
		//! The goto
		GOTO,
		//! The stop
		STOP
	} opcode;

	//! index of the framename
	int frame_id;
	//! the time this frame lasts
	float delay;
	//! The number of the next instruction to jump to.
	int next;
} TD_anim_instruction;

//! This class can store a list of animations
class Anim_instructions : public std::vector<Anim_instruction>{};

//! The loaded graphics
typedef struct Anim_graphic_key {
	//! The frame id
	int frame_id;
	//! The aiming direction (Bit 0 is interpreted as direction)
	int aimdir;
	//! ???
	void set(int frame_id_, int aim_id, int direction) {
		frame_id = frame_id_;
		if (aim_id < 0) {
			aimdir = aim_id;
		} else {
			aimdir = (aim_id << 1);	
			if (direction == 1) aimdir |= 1;
		}
	}
} TD_anim_graphic_key;

//! ???
struct Anim_graphic_key_compare {
	//! ???
	bool operator()(const Anim_graphic_key one, const Anim_graphic_key other) const {
		if (one.frame_id < other.frame_id) return true;
		if ((one.frame_id == other.frame_id) && (one.aimdir < other.aimdir)) return true;
		return false;
	}
};

//! ???
class Anim_graphics : public std::map<Anim_graphic_key,Graphic*,Anim_graphic_key_compare>{};

//! @author Flemming Frandsen
//! @brief a single instruction line from the file.
class Anim_token {
public:
	//! ???
	typedef enum Type {
		//! n/a
		THEEND,
		//! direction (r,l,no)
		FLIP,
		//! name, label
		SEQUENCE,
		//! elevation deg, elevation code
		AIMANGLE,
		//! label
		LABEL,
		//! frame, ms delay
		FRAME,
		//! label
		GOTO,
		//! n/a
		STOP,
		//! error message
		ANIMERROR
	} TD_type;
	//! ???
	Anim_token();
	//! ???
	~Anim_token();
	//! ???
	bool interpret(int line_number, char *line);
	//! ???
	char *getparameter(int index);
	//! ???
	Type gettype() {return type;}
	//! ???
	int getline() {return line_number;}
protected:
	//! ???
	int line_number;
	//! ???
	void error(char *desc);
	//! ???
	char *source;
	//! ???
	char *source_save;
	//! ???
	char *parameters[10];
	//! ???
	int parameters_inuse;
	//! ???
	Type type;
};

//! @author Flemming Frandsen
//! @brief tokenizer
class Anim_tokenizer {
public:
	//! ???
	Anim_tokenizer(char *script);
	//! ???
	~Anim_tokenizer();
	//! ???
	void next_token();
	//! ???
	Anim_token *getcurrent(){return current;}
	//! ???
	Anim_token *getnext(){return next;}
protected:
	//! ???
	char *getnextline();
	//! ???
	int line_number;
	//! ???
	Anim_token *current;
	//! ???
	Anim_token *next;
	//! ???
	char *source;
};

//! @author Flemming Frandsen
//! @brief Class for comparing strings for case
class Strcasecmp {
public:
	//! ???
	bool operator()(char* s1, char* s2) const { return strcasecmp(s1, s2) < 0; }
};

//! ???
struct Anim_label {
	//! ???
	int tmp_index;
	//! ???
	int index;
	//! ???
	char label[100];
};

//! @author Flemming Frandsen
//! @brief ???
class Anim_labels : public std::map<char *, Anim_label *, Strcasecmp>{
public:
	//! ???
	int tmp_index;
	//! ???
	int get_label_index(char *label) {
		iterator i=find(label);
		if (i != end()) {
			return i->second->index;
		}
		
		Anim_label *newlabel = new Anim_label;
		
		strcpy(newlabel->label,label);
		newlabel->tmp_index = tmp_index;
		newlabel->index = tmp_index--;

		insert(std::pair<char *, Anim_label *>(newlabel->label,newlabel));

		return newlabel->index;
	}
	//! ???
	void set_label_index(char *label, int index) {
		iterator i=find(label);
		if (i != end()) {
			i->second->index = index;
			return;
		}
		
		Anim_label *newlabel = new Anim_label;
		
		strcpy(newlabel->label,label);
		newlabel->tmp_index = index;
		newlabel->index = index;

		insert(std::pair<char *, Anim_label *>(newlabel->label,newlabel));
	}
	//! ???
	int tmp_index_to_index(int tmp_label_index)
	{
		iterator i=begin();
		while (i != end()) {
			if (i->second->tmp_index == tmp_label_index) {
				return i->second->index;
			}
			i++;
		}
		return -1;
	}
};

//! turns strings into integers
typedef struct {
	//! ???
	int handle;
	//! ???
	int flags;
} Anim_handle_and_flags;

//! @author Flemming Frandsen
//! @brief ???
class Anim_handle_maker : public std::map<std::string, Anim_handle_and_flags> {
public:
	//! ???
	int new_handle;
	//! ???
	Anim_handle_maker() { new_handle = 0; }
	//! ???
	int get_handle(char *string, int setflags = 0) {
		iterator i = find(string);
		int handle = -1;
		if (i == end()) {
			handle = new_handle++;
			Anim_handle_and_flags hf;
			hf.handle = handle;
			hf.flags = setflags;
			insert(std::pair<std::string, Anim_handle_and_flags>(string,hf));
		} else {
			handle = i->second.handle;
			i->second.flags &= setflags;
		}
		return handle;
	}
};

#endif
