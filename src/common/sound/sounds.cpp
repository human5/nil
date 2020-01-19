/***************************************************************************
  sounds.cpp
  --------------------------------------------------------------------------
  begin                : 2002-08-15
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2005-01-07
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Clean header, API and documenation

  copyright            : (C) 2002 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#include "common/misc.h"
#include "common/systemheaders.h"
#include "common/sound/sounds.h"
#include "common/sound/sound.h"
#include "common/vfs/loader.h"
#include "common/console/logmsg.h"
#include <map>
#include <string>

//! A hash map of all loaded sounds
class Sounds_loaded : public std::map<std::string,Sound*>{};

Sounds::Sounds() {
	sounds = new Sounds_loaded;
	loader = NULL;
}

Sounds::~Sounds() {
	// In case soundsystem didn't close it already, Else the mess might segfault
	Mix_CloseAudio();

	Sounds_loaded::iterator i = sounds->begin();
	while (i != sounds->end())
		delete (i++)->second;
	delete sounds;
}

void Sounds::init(Loader *loader_) {
	loader = loader_;
}

Sound *Sounds::get_sound(char *name, bool loadchunk) {
	Sounds_loaded::iterator i = sounds->find(name);

	// Sounds was already loaded
	if (i != sounds->end())
		return i->second;

	Sound *sound = new Sound;
	sounds->insert(std::pair<std::string,Sound*>(name,sound));

	if (!sound->load(loader,name,loadchunk)) {
		logmsg(lt_error,"sound %s could _NOT_ be loaded. No soup for you!",name);
		i = sounds->find(name);
		if (i != sounds->end())
			sounds->erase(i);
		delete sound;
		return NULL;
	}

	logmsg(lt_debug,"sound %s loaded",name);
	return sound;
}
