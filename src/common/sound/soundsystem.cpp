/***************************************************************************
  soundsystem.cpp  -  the soundsystem
  --------------------------------------------------------------------------
  begin                : 2000-02-21
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2005-01-17
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : move SDL_mixer initialization and includes here 

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#include "common/sound/soundsystem.h"
#include "common/configuration/configuration.h"
#include "common/serializer/serializer_writer.h"
#include "common/serializer/serializer_reader.h"
#include "common/sound/sound_event.h"

Soundsystem::Soundsystem(Loader *loader_, Game_client *game_client_, Configuration *config) {
	next_sound_handle = 1;
	sound_events_handle = new Sound_events_handle;
	sound_handles_server = new Sound_handles_server;
	loader = loader_;

	//NT sound on/off
	have_sound = config->get_option_int("sound");

	int frequency = 22050;
	Uint16 format = AUDIO_S16SYS;
	int wanted_channels = 48;

	if (have_sound) {
	    Mix_OpenAudio(frequency, format, 2, 1024);
	    channels = Mix_AllocateChannels(wanted_channels);
	    logmsg(lt_debug, "Mixer initialized, %i channels.", channels);
	    have_sound = !(Mix_QuerySpec(&frequency, &format, &channels) < 1);
	} else {
	    logmsg(lt_debug, "Not using sound, mixer not initialized.");
	}

	if (have_sound) {
		logmsg(lt_debug,"Sound system initialized in quiet mode.");
		game_client = NULL;
	} else {
		logmsg(lt_debug,"Sound system initialized, mixing %i channels at %iHz.",channels,frequency);
		game_client = game_client_;
	}
}

Soundsystem::~Soundsystem() {
	if (have_sound)
		Mix_CloseAudio();

	Sound_events_handle::iterator i = sound_events_handle->begin();
	while (i != sound_events_handle->end())
		delete (i++)->second;
	
	delete sound_events_handle;
	delete sound_handles_server;
}

void Soundsystem::update() {
	Sound_events_handle::iterator i = sound_events_handle->begin();

	//struct timeval now; gettimeofday(&now,NULL);
	unsigned int now = time_ms(); // check sound expire times

	while (i != sound_events_handle->end()) {
		if (!(i->second->flags & SOUND_DONE)) {
			i->second->check_timer(now);
		}
		if (i->second->flags & SOUND_DONE) {
			Sound_handles_server::iterator k;

			// Can't we just search for a value here?
			for (k = sound_handles_server->begin();
				k != sound_handles_server->end() && k->second != i->first;
				k++);
			if (k != sound_handles_server->end())
				sound_handles_server->erase(k);

			Sound_events_handle::iterator j = i++;
			delete j->second;
			sound_events_handle->erase(j);
		} else {
			if (have_sound && game_client != NULL)
				i->second->update(game_client);
			i++;
		}
	}
}

void Soundsystem::server_think(Serializer_reader *serializer_reader) {
	Sound_events_handle::iterator i=sound_events_handle->begin();
	while (i != sound_events_handle->end()) {
		if (i->second->flags & SOUND_DIRTY) {
			i->second->flags &= ~SOUND_DIRTY;
			if (i->second->flags & SOUND_DONE) {
				serializer_reader->writer_int(TT_SOUND_STOP);
				serializer_reader->writer_int(i->first);
			} else {
				serializer_reader->writer_int(TT_SOUND_START);
				serializer_reader->writer_int(i->first);
				i->second->send_event(serializer_reader);
			}
		}
		i++;
	}

	update();
}

void Soundsystem::client_think(Serializer_writer &serializer_writer) {
	while (1) {
		if (serializer_writer.next_int() == TT_SOUND_START) {
			serializer_writer.eat_int(TT_SOUND_START);
			int server_handle = serializer_writer.reader_int();
			int name_len = serializer_writer.reader_int()+1;
			char *sample_name = new char[name_len];
			serializer_writer.rwstr(sample_name,name_len);
			float volume = serializer_writer.reader_float();
			int flags = serializer_writer.reader_int();
			float x_pos = 0;
			float y_pos = 0;
			int objectid = 0;
			if (flags & SOUND_OBJECT) objectid = serializer_writer.reader_int();
			else if (flags & SOUND_STATIC) {
				x_pos = serializer_writer.reader_float();
				y_pos = serializer_writer.reader_float();
			}
			int sound_handle = start_sound(new Sound_event(sample_name,flags,volume,x_pos,y_pos,objectid,this));
			delete [] sample_name;
			sound_handles_server->insert(std::pair<int,int>(server_handle,sound_handle));
		} else if (serializer_writer.next_int() == TT_SOUND_STOP) {
			serializer_writer.eat_int(TT_SOUND_STOP);
			int sound_handle = serializer_writer.reader_int();
			Sound_handles_server::iterator i = sound_handles_server->find(sound_handle);
			if (i != sound_handles_server->end())
				stop_sound(i->second);
		} else
			break;
	}

	update();
}

int Soundsystem::start_sound(Sound_event *sound_event) {
	int sound_handle = next_sound_handle++;
	sound_events_handle->insert(std::pair<int, Sound_event*>(sound_handle,sound_event));
	return sound_handle;
}

int Soundsystem::start_sound(char *sample_name, int flags, float volume) {
	logmsg(lt_debug, "Playing sound %s (global)", sample_name);
	return start_sound(new Sound_event(sample_name, flags|SOUND_GLOBAL, volume,0,0,0,this));
}

int Soundsystem::start_sound(char *sample_name, int flags, float volume, float x_pos, float y_pos) {
	logmsg(lt_debug, "Playing sound %s (position)", sample_name);
	return start_sound(new Sound_event(sample_name,flags|SOUND_STATIC,volume,x_pos,y_pos,0,this));
}

int Soundsystem::start_sound(char *sample_name, int flags, float volume, int objectid) {
	logmsg(lt_debug, "Playing sound %s (object)", sample_name);
	return start_sound(new Sound_event(sample_name,flags|SOUND_OBJECT,volume,0,0,objectid,this));
}

bool Soundsystem::stop_sound(int sound_handle) {
	Sound_events_handle::iterator i = sound_events_handle->find(sound_handle);
	if (i == sound_events_handle->end()) {
		logmsg(lt_debug, "Unable to stop non-existant sound with handle %d.", sound_handle);
		return false;
	}
	i->second->bedone();
	return true;
}

bool Soundsystem::sound_halted(int channel) {
	logmsg(lt_warning, "Soundsystem::sound_halted() not implemented");
	return false;
}

bool Soundsystem::get_sample_id(char *sample_name) {
	logmsg(lt_warning, "Soundsystem::get_sample_id() not implemented");
	return false;
}

bool Soundsystem::is_enabled() {
	return have_sound;
}

int Soundsystem::get_channels() {
	return channels;
}

Sound* Soundsystem::get_sound(char *name, bool load_chunk) {
	return loader->sounds.get_sound(name, load_chunk);
}
