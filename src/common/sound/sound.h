/***************************************************************************
  sound.h  -  Header for NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2002-08-15
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-03-27
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Don't bother with the whole file without SDL.

  last changed         : 2005-01-07
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Clean header, API and documenation

  copyright            : (C) 2002 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#ifndef SOUND_H
#define SOUND_H

#ifndef WITHOUT_SDL

#include "common/systemheaders.h"

class Loader;

//! @author Flemming Frandsen
//! @brief This class loads and stores a single sound (WAV-File) per instance
class Sound {
public:
	//! Constructor that takes care that the chunk is empty
	Sound();
	//! Destructor that free's the loaded sound
	~Sound();
	/*! load a WAV-file
	    @param loader A handle to a loader
	    @param name The filename of the sound
	    @param load_chunk Really load the file or just test if it is valid 
	    @return Returns true if the sound file is valid (and if load_chunk was true if the sound was loaded) */
	bool load(Loader *loader, char *name, bool load_chunk);
	//! returns the stored WAV-file
	Mix_Chunk *get_chunk();
	//! returns the length of the sound (in seconds)
	float get_len();
protected:
	//! A pointer to the stored WAV-File
	Mix_Chunk *chunk;
	//! length of the sound (in seconds)
	float length;
};

#endif
#endif
