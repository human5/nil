/***************************************************************************
  sounds.h
  --------------------------------------------------------------------------
  begin                : 2002-08-15
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2005-01-07
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Clean header, API and documenation

  copyright            : (C) 2002 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#ifndef SOUNDS_H
#define SOUNDS_H

#include "sound.h"

class Loader;
class Sounds_loaded;

//! @author Flemming Frandsen
//! @brief Class that uses Sound class and stores
class Sounds {
public:
	//! Constructor that initializes the hash map
	Sounds();
	//! Destructor that closes audio and free's the loaded sounds
	~Sounds();
	/*! Initializer that stores the loader
	    @param loader_ A handle to the loader */
	void init(Loader *loader_);
	/*! Return a pointer to an already loaded sound or load it on demand
	    @param name The filename of the sound
	    @param load_chunk Really load the sound or just validate it's a WAV-file
	    @return Returns a handle to the sound */
#ifndef WITHOUT_SDL
	Sound *get_sound(char *name, bool load_chunk);
#endif
protected:
	//! storage for the handle to a loader
	Loader *loader;
	//! hash map of all loaded sounds
	Sounds_loaded *sounds;
};

#endif
