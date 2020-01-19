/***************************************************************************
  sound.cpp  -  NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2002-08-15
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-03-27
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

#include "common/sexchanger.h"
#include "common/console/logmsg.h"
#include "common/misc.h"
#include "common/sound/sound.h"
#include "common/vfs/loader.h"
#include "common/nil_math.h"

//FIXME: no longer, right???
/* Parts commented out because SDL_mixer doesn't support MSB wave files yet.
 * But the extra endian code should make it work on MSB machines. */

Sound::Sound() {
	chunk = NULL;
}

Sound::~Sound() {
	if (chunk) Mix_FreeChunk(chunk);
}

//! Container for the RIFF-Format
struct RIFF_Chunk {
	//! the id
	char id[4];
	//! the size
	Uint32 size;
};
#define idcmp(id,c1,c2,c3,c4) ((id)[0]==(c1) && (id)[1]==(c2) && (id)[2]==(c3) && (id)[3]==(c4))

//! Container for the WAV-Format
struct WAVE_fmt {
	//! encoding type (1=PCM/uncompressed)
	Uint16 encoding;
	//! output channels
	Uint16 channels;
	//! a.k.a. frequency, samples/sec
	Uint32 samplerate;
	//! bytes/sec
	Uint32 byterate;
	//! bytes/sample*channels
	Uint16 blockalign;
	//! bits/sample
	Uint16 samplebits;
	// Extra data follows in compressed/non-PCM formats
	// We don't care though.
};

//FIXME: manual fixing seems no longer necessary since SDL_mixer can handle big vs. little endian
bool Sound::load(Loader *loader, char *name, bool load_chunk) {
	char *file = (char *)loader->getfile(name);
	if (!file) {
		logmsg(lt_error,"Unable to load sound %s!",name);
		return false;
	}

	int size = loader->filesize(name);
	if (size < 44) {
		logmsg(lt_error,"%s is too small to be a wave file.",name);
		return false;
	}

	char *f = file;
	struct RIFF_Chunk header;
//	bool msb; // big-endian?
	memcpy(&header,f,sizeof(struct RIFF_Chunk));
//	if (idcmp(header.id,'R','I','F','F')) // little-endian
//		msb = 0;
//	else if (idcmp(header.id,'R','I','F','X')) // big-endian
//		msb = 1;
//	else {
	if (!idcmp(header.id,'R','I','F','F')) {
		logmsg(lt_error,"%s has no RIFF header, not a wave file.",name);
		return false;
	}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	header.size = bytesex_change(header.size);
#endif

	if (header.size+8 != (unsigned int)size)
		logmsg(lt_warning,"File size and size in %s's header do not match.",name);

	f += 8;
	if (!idcmp((char *)f,'W','A','V','E')) {
		logmsg(lt_error,"%s is not a wave file.",name);
		return false;
	}

	int offset = 12;
	f += 4;
	int datasize = 0;
	struct WAVE_fmt format;
	char found = 0;
	while (offset < size) {
		memcpy(&header,f,sizeof(struct RIFF_Chunk));

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		header.size = bytesex_change(header.size);
#endif

		if (idcmp(header.id,'f','m','t',' ')) {
			found |= 0x01;
			memcpy(&format,f+8,sizeof(struct WAVE_fmt));
		} else if (idcmp(header.id,'d','a','t','a')) {
			found |= 0x02;
			datasize += header.size;
		}

		offset += 8+header.size;
		f = file+offset;
	}

	if (!(found & 0x01)) {
		logmsg(lt_error,"Wave file %s is corrupt: no format chunk.",name);
		return false;
	}
	if (!(found & 0x02))  {
		logmsg(lt_error,"Wave file %s is corrupt: no data chunk.",name);
		return false;
	}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
//	if (!msb) format.byterate = bytesex_change(format.byterate);
	format.byterate = bytesex_change(format.byterate);
//#else
//	if (msb) format.byterate = bytesex_change(format.byterate);
#endif

	length = (float)datasize/(float)format.byterate;

	if (!load_chunk) return true;

	chunk = Mix_LoadWAV_RW(SDL_RWFromMem(file,size),1);
	if (!chunk) {
		logmsg(lt_error,"Unable to load wave file %s: %s",name,Mix_GetError());
		return false;
	}

	return true;
}


Mix_Chunk* Sound::get_chunk() {
	return chunk;
}

float Sound::get_len() {
	return length;
}
