/***************************************************************************
  sound_event.h  -  Header for NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2002-08-17
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  copyright            : (C) 2002 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#ifndef SOUND_EVENT_H
#define SOUND_EVENT_H

#include <time.h>
#include <map>

#define SOUND_LOOPING 0x01
#define SOUND_GLOBAL  0x02
#define SOUND_STATIC  0x04
#define SOUND_OBJECT  0x08
#define SOUND_DONE    0x10
#define SOUND_DIRTY   0x20

class Game_client;
class Soundsystem;
class Serializer_reader;

//! @author Flemming Frandsen
//! @brief A sound event
class Sound_event {
public:
	//! Constructor
	Sound_event(const char *sample_name_, int flags_, float volume_, float x_pos_, float y_pos_, int objectid_, Soundsystem *soundsystem_);
	//! Destructor
	~Sound_event();
	//! ???
	void update(Game_client *game_client);
	//! ???
	void send_event(Serializer_reader *serializer_reader);
	//! ???
	void bedone();
	//void check_timer(struct timeval &now);
	//! ???
	void check_timer(unsigned int now);
	//! ???
	int flags;
protected:
	//! ???
	Soundsystem *soundsystem;
	//! ???
	char *name;
	//! ???
	int channel;
	//! ???
	float volume;
	//struct timeval expire;
	//! ???
	unsigned int expire;
	//! ???
	int objectid;
	//! ???
	float x_pos;
	//! ???
	float y_pos;
};

//FIXME: Move to .cpp files like every other std::map, too?

//! List of sound event handle
class Sound_events_handle : public std::map<int, Sound_event*> {};
//! List of sound events handled by the server
class Sound_handles_server : public std::map<int, int> {};

#endif
