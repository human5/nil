/***************************************************************************
  animations.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-09
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "client/animation/animations.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/vfs/loader.h"
#include <map>
#include <string>

//! ???
typedef struct Loaded_animations_key {
	//! A name
	std::string name;
	//! The color
	int32 color;
} TD_loaded_animations_key;

//! ???
struct Compare_lak {
	//! ???
	bool operator()(const Loaded_animations_key one, const Loaded_animations_key other) const {
		if (one.name < other.name) return true;
		if (one.color < other.color) return true;
		return false;
	}
};

//! A list of loaded animations
class Loaded_animations :public std::map<const Loaded_animations_key,Animation *,Compare_lak>{};

Animations::Animations() {
	animations = new Loaded_animations;
}

Animations::~Animations() {
	Loaded_animations::iterator i=animations->begin();
	while (i != animations->end()) {
		delete i->second;
		i++;
	}
	delete animations;
}

void Animations::init(Loader *loader_) {
	loader = loader_;
}

Animator *Animations::new_animator(char	*name, int32 color) {
	Loaded_animations_key key;
	key.name = name;
	key.color = color;

	//first see if we have a hit:
	Loaded_animations::iterator i=animations->find(key);
	if (i != animations->end()) {
		Animator *newanimator = new Animator;
		newanimator->create(i->second);
		return newanimator;
	}

	//Load the animation
	Animation *newanimation = new Animation;
	if (!newanimation->create(loader,name,color)) {
		logmsg(lt_error,"Unable to load the animation %s with color 0x%x",name,color);
		delete newanimation;
		return NULL;
	}

	//save the animation in the cache for later
	animations->insert(std::pair<Loaded_animations_key,Animation *>(key,newanimation));	
	
	//return a new animator
	Animator *newanimator = new Animator;
	newanimator->create(newanimation);
	return newanimator;
}
