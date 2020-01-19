/***************************************************************************
  fonts.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-07
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  last changed         : 2004-11-06
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : added new constructor

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "client/fonts/fonts.h"
#include "client/fonts/font_instance.h"
#include "common/vfs/loader.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#ifdef HAS_STL_HASHMAP
#include <hash_map>
#else
#include <map>
#endif

#ifdef HAS_STL_HASHMAP
#define Streq_ Streq
//! a list of loaded fonts
class Font_instances : public std::hash_map<const char *,Font_instance *,hash<const char*>,Streq_>{};
#else
#define Strless_ Strless
//! a list of loaded fonts
class Font_instances : public std::map<const char *,Font_instance *,Strless_>{};
#endif

Fonts::Fonts() {
	font_instances = new Font_instances;
	loader = NULL;
}

Fonts::Fonts(Loader *loader_) {
	font_instances = new Font_instances;
	loader = loader_;
}

void Fonts::init(Loader *loader_) {
	loader = loader_;
}

Fonts::~Fonts() {
	Font_instances::iterator i, e, t;
	i = font_instances->begin();
	e = font_instances->end();
	while (i != e) {
		//FIXME: This causes some memleak!
		delete i->first;
		delete i->second;
		t = i;
		++i;
		font_instances->erase(t);
	}
	delete font_instances;
}

Font_instance *Fonts::getfont(const char *name, int size ) {
	// the default font is not antialized
	return this->getfont(name, size, false);
}

Font_instance *Fonts::getfont(const char *name, int size, bool antialize) {
	/* FIXME: The ttf2rgbf project could be merged into this class so that
	   prerendering fonts is not needed. */
	// FIXME: check for antialize

	char font_name[80];
	sprintf(font_name, "%s/%i", name, size);

	// check if the font was already loaded
	Font_instances::iterator i = font_instances->find(name);
	if (i != font_instances->end())
		return i->second;

	// it wasn't loaded yet, let's load it
	Font_instance *newfont = new Font_instance;
	if (!newfont->load(loader, (char*)name, size)) {
		delete newfont;
		logmsg(lt_error, "Unable to load the font %s, wonkyness ahead", name);
		return NULL;
	}

	char *namecopy = new char[strlen(font_name)+1];
	strcpy(namecopy, font_name);
	font_instances->insert(std::pair<const char *,Font_instance *>(namecopy, newfont));
	/* Unnecessary double check
	i=font_instances->find(name);
	if (i!=font_instances->end())
		return i->second;
	return NULL;*/
	return newfont;
}
