/***************************************************************************
  sound_event.cpp  -  NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2002-08-17
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  copyright            : (C) 2002 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#include "common/sound/soundsystem.h"
#include "common/sound/sound.h"
#include "common/sound/sound_event.h"
#include "common/serializer/serializer_reader.h"
#include "common/console/logmsg.h"
#include "common/objects/obj_base.h"
#include "client/game_client.h"
#include <time.h>

//-------------------------------------------------------------------
// Nasty, but that's what the wrappers are for.
Sound_event::Sound_event(char *sample_name, int flags_, float volume_, float x_pos_, float y_pos_, int objectid_, Soundsystem *soundsystem_)
{
	name = new char[strlen(sample_name)+1];
	strcpy(name,sample_name);
	soundsystem = soundsystem_;
	x_pos = -1;
	y_pos = -1;
	
	if (volume_ > 1.0f) volume = 1.0f;
	else if (volume_ < 0.0f) volume = 0.0f;
	else volume = volume_;
	
	flags = SOUND_DIRTY;
	if (flags_ & SOUND_STATIC) {
		flags |= SOUND_STATIC;
		x_pos = x_pos_;
		y_pos = y_pos_;
	} else if (flags_ & SOUND_OBJECT) {
		flags |= SOUND_OBJECT;
		objectid = objectid_;
	} else
		flags |= SOUND_GLOBAL;
	
	int loops = 0;
	if (flags_ & SOUND_LOOPING) {
		flags |= SOUND_LOOPING;
		loops = -1;
	}

	Sound *sound = NULL;
	if (soundsystem->is_enabled())
		sound = soundsystem->get_sound(name,1);
	else
		sound = soundsystem->get_sound(name,0);
	if (!sound) {
		logmsg(lt_error,"Couldn't open sound \"%s\".",name);
		flags |= SOUND_DONE;
		return;
	}

	/*struct timeval now, len;
	gettimeofday(&now,NULL);
	len.tv_sec = (int)sound->get_len();
	len.tv_usec = (int)((sound->get_len()-(float)len.tv_sec)*1000000.0f);
	timeradd(&now,&len,&expire);
			logmsg(lt_debug," sound inited %d ",len.tv_sec);*/
	expire = time_ms() + (unsigned int)(sound->get_len()*1000.0);

	if (soundsystem->is_enabled()) { //Play the sound
		channel = Mix_PlayChannel(-1, sound->get_chunk(), loops);
		if (channel == -1)
			logmsg(lt_error,"Unable to play sound %s: %s",name,Mix_GetError());
	} else
		channel = -1;
}

//-------------------------------------------------------------------
Sound_event::~Sound_event()
{
	delete [] name;
}

//-------------------------------------------------------------------
void Sound_event::update(Game_client *game_client)
{
	if (channel < 0) return;

	// Multiple local players? How are we gonna do that?
	Viewport *viewport = game_client->get_player()->viewport;

	if (flags & SOUND_OBJECT) {
		Obj_base *object = game_client->clientworld.get_object(objectid, ot_error);
		if (object)
			object->getpos(x_pos, y_pos);
		else {
			flags &= ~SOUND_OBJECT;
			if (x_pos < 0 && y_pos < 0)
				flags |= SOUND_GLOBAL;
			else
				flags |= SOUND_STATIC;
		}
	}

	if ((flags & SOUND_STATIC) || (flags & SOUND_OBJECT)) {
		// Viewport's center coordinates
		float xcenter = viewport->xpos+(viewport->get_xsize()/2);
		float ycenter = viewport->ypos+(viewport->get_ysize()/2);
		
		// Get the distance to the object form the viewport
		double dist = hypot(x_pos-xcenter, y_pos-ycenter)-50.0f;
		if (dist <= 0.0f)
			dist = 0.0f;
		else
			dist /= 2.0f;
		if (dist > 220.0f) // Maximum distance, you can always hear sound, no matter how far away
			dist = 220.0f;

		double angle = 0.0f;
		// Caculate what direction the sound is coming from
		if (soundsystem->get_channels() > 1 && dist > 0.0f) {
			if (x_pos == xcenter && y_pos >= ycenter) // 0
				angle = 0.0f;
			else if (x_pos >= xcenter && y_pos == ycenter) // 90
				angle = 90.0f;
			else if (x_pos == xcenter && y_pos < ycenter) // 180
				angle = 180.0f;
			else if (x_pos < xcenter && y_pos == ycenter) // 270
				angle = 270.0f;
			else if (x_pos > xcenter && y_pos < ycenter) // 1 - 89
				angle = atan((double)(x_pos-xcenter)/(double)(ycenter-y_pos))*(180.0f/M_PI);
			else if (x_pos > xcenter && y_pos > ycenter) // 91 - 179
				angle = 90.0f+atan((double)(y_pos-ycenter)/(double)(x_pos-xcenter))*(180.0f/M_PI);
			else if (x_pos < xcenter && y_pos > ycenter) // 181 - 269
				angle = 180.0f+atan((double)(xcenter-x_pos)/(double)(y_pos-ycenter))*(180.0f/M_PI);
			else if (x_pos < xcenter && y_pos < ycenter) // 271 - 359
				angle = 270.0f+atan((double)(ycenter-y_pos)/(double)(xcenter-x_pos))*(180.0f/M_PI);
		}

		// Also use distance for the requested sound volume
		dist += (255.0f-dist)*(1.0f-volume);

		// Have SDL_mixer take care of the job
		Mix_SetPosition(channel, (Sint16)angle, (Uint8)dist);
// with older SDL replace Mix_SetPosition by Mix_Volume
//		Mix_Volume(channel, (int)(volume*(float)MIX_MAX_VOLUME));
	} else {	// Global sounds only care about the volume
		Mix_Volume(channel, (int)(volume*(float)MIX_MAX_VOLUME));
	}
}

//-------------------------------------------------------------------
void Sound_event::bedone()
{
	if (flags & SOUND_DONE) return;

	Mix_HaltChannel(channel);
	flags |= SOUND_DONE | SOUND_DIRTY;
}

//-------------------------------------------------------------------
void Sound_event::send_event(Serializer_reader *serializer_reader)
{
	serializer_reader->writer_int(strlen(name));
	serializer_reader->rwstr(name, strlen(name)+1 );
	serializer_reader->rw(volume);
	serializer_reader->rw(flags);
	if (flags & SOUND_OBJECT) serializer_reader->rw(objectid);
	else if (flags & SOUND_STATIC) {
		serializer_reader->rw(x_pos);
		serializer_reader->rw(y_pos);
	}
}

//-------------------------------------------------------------------
//void Sound_event::check_timer(struct timeval &now)
void Sound_event::check_timer(unsigned int now)
{
	if (!(flags & SOUND_LOOPING)) {
		//if (timercmp(&now,&expire,>)) {
		if(now>expire) {
			flags |= SOUND_DONE;
			return;
		}
	}
}
