/***************************************************************************
  soundsystem.h  -  Header for the soundsystem
  --------------------------------------------------------------------------
  begin                : 2000-02-21
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2005-01-17
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : doxygen update and move includes to cpp

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#ifndef _NIL_SOUNDSYSTEM_H_
#define _NIL_SOUNDSYSTEM_H_

/* Sound flags */
#define SOUND_LOOPING 0x01
#define SOUND_GLOBAL  0x02
#define SOUND_STATIC  0x04
#define SOUND_OBJECT  0x08
#define SOUND_DONE    0x10
#define SOUND_DIRTY   0x20

class Game_client;
class Configuration;
class Serializer_reader;
class Serializer_writer;
class Sound_event;
class Sound_events_handle;
class Sound_handles_server;
class Sound;
class Loader;

//! @author Flemming Frandsen
//! @brief The Soundsystem of NiL
class Soundsystem {
public:
	//! Constructor that stores the handles (and check's if sound should be enabled)
	Soundsystem(Loader *loader_, Game_client *game_client_, Configuration *config);
	//! ???
	virtual ~Soundsystem();
	//! ???
	void server_think(Serializer_reader *serializer_reader);
	//! ???
	void client_think(Serializer_writer &serializer_writer);
	//! ???
	void cleanup();
	/*! Start a sound that is unbound (global, like music)
	    \param sample_name The unique name of the sample
	    \param flags ???
	    \param volume The volume of the sound
	    \return Returns a handle to the sound */
	int start_sound(const char *sample_name, int flags, float volume);
	/*! Start a sound that is bound to a position
	    \param sample_name The unique name of the sample
	    \param flags ???
	    \param volume The volume of the sound
	    \param x_pos The x-coordinate of the sound
	    \param y_pos The y-coordinate of the sound
	    \return Returns a handle to the sound */
	int start_sound(const char *sample_name, int flags, float volume, float x_pos, float y_pos);
	/*! Start a sound that is bound to an object
	    \param sample_name The unique name of the sample
	    \param flags ???
	    \param volume The volume of the sound
	    \param objectid The idea of the object
	    \return Returns a handle to the sound */
	int start_sound(const char *sample_name, int flags, float volume, int objectid);
	/*! Stop a sound
	    \param sound_handle The handle to the sound (obtained by start_sound)
	    \return Returns true if the sound has been stopped and false if the sound has not been found */
	bool stop_sound(int sound_handle);
	//! ???
	bool sound_halted(int channel);
	/*! ???
	    \param sample_name ???
	    \return ??? */
	bool get_sample_id(char *sample_name);
	/*! Find out of sound is enabled
	    \return Returns true if sound is disabled */
	bool is_enabled();
	/*! Get the number of available channels
	    \return Returns number of available channels */ 
	int get_channels();
	/*! Get a sound by given name
	    @param name The name of the soundfile
	    @param load_chunk Really load the sound or just test if it is a sound
	    @return The wanted sound */
	Sound *get_sound(char *name, bool load_chunk);
protected:
	//! A handle to the game client
	Game_client *game_client;
	//! A handle to the loader
	Loader *loader;
	//! The number of available channels
	int channels;
	//! Is the sound on or off
	bool have_sound;
	//! ???
	Sound_events_handle *sound_events_handle;
	//! ???
	Sound_handles_server *sound_handles_server;
	//! ???
	int next_sound_handle;
	/*! Start a sound that is bound to a position
	    \param sound_event The soundevent
	    \return Returns a handle to the sound */
	int start_sound(Sound_event *sound_event);
	//! ???
	void update();
};

#endif
