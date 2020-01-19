/***************************************************************************
  graphics.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-17
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "client/graphics/graphics.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/systemheaders.h"
#include "common/vfs/loader.h"
#include <string>
#include <map>

//! This should also be a hash_map
class Loaded_graphics :public std::map<std::string,Graphic *>{};

Graphics::Graphics() {
	graphics = new Loaded_graphics;
	loader = NULL;
}

Graphics::~Graphics() {
	Loaded_graphics::iterator i=graphics->begin();
	while	(i != graphics->end()) {
		delete i->second;
		i++;
	}
	delete graphics;
}

void Graphics::init(Loader *loader_) {
	loader = loader_;
}

Graphic *Graphics::getgraphic(char *name) {
	Loaded_graphics::iterator i=graphics->find(name);
	if (i != graphics->end())
		return i->second;
	Graphic *graphic = new_graphic(name);
	if (graphic && !graphic->load(loader,name)) {
		delete_graphic(name);
		graphic = NULL;
	}
	if (graphic != NULL) {
		logmsg(lt_debug,"graphic %s loaded",name);
	} else {
		logmsg(lt_error,"graphic %s could _NOT_ be loaded, expect a crash soon",name);
	}
	return graphic;
}

Graphic *Graphics::new_graphic(char *name) {
	Loaded_graphics::iterator i=graphics->find(name);
	if (i == graphics->end()) {

		Graphic *graphic = new Graphic;
		graphics->insert(std::pair<std::string,Graphic*>(name,graphic));

		return graphic;
	} else
		return NULL;
}

void Graphics::delete_graphic(char *name) {
	Loaded_graphics::iterator i=graphics->find(name);
	if (i != graphics->end()) {
		graphics->erase(i);
	} else {
		logmsg(lt_error,"tried to delete a non-existant graphic: %s",name);
	}
}
