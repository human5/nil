/***************************************************************************
  anim_helpers.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-08
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-21
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "client/animation/anim_helpers.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/nil_math.h"

Anim_tokenizer::Anim_tokenizer(char *script) {
	source = script;
	line_number = 0;

	current = NULL;
	next = NULL;
	next_token();
	next_token();
}

Anim_tokenizer::~Anim_tokenizer() {
	if (current) delete current;
	if (next) delete next;
}

char *Anim_tokenizer::getnextline() {
	char *line = strsep(&source,"\n");
	line_number++;
	// logmsg(lt_debug,"line %i: %s",line_number,line);
	return line;
}

void Anim_tokenizer::next_token() {
	if (current) delete current;
	current = next;

	//read at least one line:
	char *line = getnextline();

	//ignore blank lines and comments;
	while (source && *source && line && (!*line || *line=='#'))
		line = getnextline();

	next = new Anim_token;

	//handle EOF:
	if (!line || !*line)
		next->interpret(line_number,NULL);
	else
		next->interpret(line_number,line);
}

Anim_token::Anim_token() {
	source = NULL;
	source_save = NULL;
	type = ANIMERROR;
	parameters_inuse = 0;
	line_number = -1;
}

Anim_token::~Anim_token() {
	if (source_save) free(source_save);
}

void Anim_token::error(const char *desc) {
	if (source_save) free(source_save);
	source_save = strdup(desc);
	parameters_inuse = 1;
	parameters[0] = source_save;
	type = ANIMERROR;
}

bool Anim_token::interpret(int line_number_, char *line) {
	line_number	= line_number_;

	if (!line || !*line) {
		type = THEEND;
		return true;
	}

	source = strdup(line);
	source_save = source;

	char *command = strsep(&source,"\t");
	if (!command || !*command) {
		error("No command found, this can't be right...");
		return false;
	}

	parameters_inuse = 0;
	char *parameter = strsep(&source,"\t");
	while (parameter) {
		parameters[parameters_inuse++] = parameter;
		parameter = strsep(&source,"\t");
		if (parameters_inuse >= 10) {
			error("Too many parameters on line");
			return false;
		}
	}	

	if (strcasecmp(command,"flip") == 0) {
		type = FLIP;
		if (parameters_inuse != 1) {
			error("flip takes one parameter: flip\t<existing direction>");
			return false;
		}

		char flip_code = parameters[0][0];
		parameters[0][1] = 0;

		if (flip_code != 'r' && flip_code != 'l' && flip_code != 'n') {
			error("the parameter for flip must be either 'right','left' or 'no' ");
			return false;
		}

	} else if (strcasecmp(command,"sequence") == 0) {
		type = SEQUENCE;
		if (parameters_inuse != 2) {
			error("sequence takes two parameters: sequence\t<name>\t<goto label>");
			return false;
		}

	} else if (strcasecmp(command,"aimangle") == 0) {
		type = AIMANGLE;
		if (parameters_inuse != 2) {
			error("aimangle takes two parameters: aimangle\t<elevation deg>\t<elevation code>");
			return false;
		}

	} else if (strcasecmp(command,"label") == 0) {
		type = LABEL;
		if (parameters_inuse != 1) {
			error("label takes one parameter: label\t<goto label>");
			return false;
		}

	} else if (strcasecmp(command,"frame") == 0) {
		type = FRAME;
		if (parameters_inuse != 3) {
			error("frame takes two parameters: frame\t<frame name>\t<noaim>\t<ms delay>");
			return false;
		}

	} else if (strcasecmp(command,"goto") == 0) {
		type = GOTO;
		if (parameters_inuse != 1) {
			error("goto takes one parameter: goto\t<goto label>");
			return false;
		}

	} else if (strcasecmp(command,"stop") == 0) {
		type = STOP;
		if (parameters_inuse != 0) {
			error("stop doesn't take parameters");
			return false;
		}

	} else {
		error("the command was not known");
		return false;
	}
	
	return true;
}

char *Anim_token::getparameter(int index) {
	if (index >= 0 || index < parameters_inuse)
		return parameters[index];
	else
		return NULL;
}

Anim_aimangle *Anim_aimangles::getaim(float elevation) {
	float dist=400;
	iterator hit=end();
	iterator i=begin();
	while (i != end()) {
		float d = ABS(i->elevation - elevation);
		if (d<dist) {
			dist = d;
			hit = i;
		}
		i++;
	}

	if (hit != end())
		return &(*hit);
	else {
		logmsg(lt_debug,"Unable to find a suitable aim_id for elevation: %.2f (%i angles were tried)",elevation,size());
		return NULL;
	}
}
