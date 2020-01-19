/***************************************************************************
  serializer_writer.cpp  -  NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2000-01-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  last changed         : 2005-01-04
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : Cleanup

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/serializer/serializer_writer.h"
#include "common/systemheaders.h"
#include "common/console/logmsg.h"
#include "common/sexchanger.h"

Serializer_writer::Serializer_writer() {
	buffer_size = 1000;
	buffer_inuse = 0;
	buffer = (char *)malloc(buffer_size);
	rewind();
}

Serializer_writer::~Serializer_writer() {
	free(buffer);
}

bool Serializer_writer::is_reading() {
	return false;
}

char *Serializer_writer::setbuffer(int newlength) {
	if (newlength > buffer_size) {
		buffer_size = newlength;
		free(buffer);
		buffer = (char *)malloc(buffer_size);
	}

	buffer_inuse = newlength;
	rewind();
	return buffer;
}

int Serializer_writer::next_int() {
	if (is_empty() || curmsg+(signed)sizeof(int) > buffer_inuse)
		return -1;

	int ret = *((int*)(&buffer[curmsg]));
	return getChangedByteOrder32(ret);
}

bool Serializer_writer::eat_int(int expected) {
	int i = reader_int();
	if (i == expected)
		return true;
	else {
		logmsg(lt_error,"eat_int(%i) found %i instead", expected, i);
		return false;
	}
}

inline bool Serializer_writer::get_bytes(char *data, int length) {
	if (curmsg+length > buffer_inuse) {
		logmsg(lt_error,"tried to read %i bytes from Serializer_writer, that was too many",length);
		empty_read_error = true;
		return false;
	} else {
		memcpy(data, buffer+curmsg, length);
		curmsg += length;
		return true;
	}
}

void Serializer_writer::rw(Sint8 &value) {
	get_bytes((char *)&value,sizeof(value));
}

void Serializer_writer::rw(Sint16 &value) {
	get_bytes((char *)&value,sizeof(value));
	value = getChangedByteOrder16(value);
}

void Serializer_writer::rw(Sint32 &value) {
	get_bytes((char *)&value,sizeof(value));
	value = getChangedByteOrder32(value);
}

void Serializer_writer::rw(int32 &value) {
	get_bytes((char *)&value,sizeof(value));
	value = getChangedByteOrder32(value);
}

void Serializer_writer::rw(bool &value) {
	Sint8 recval;
	get_bytes((char *)&recval,sizeof(recval));
	value = recval;
}

void Serializer_writer::rw(float &value) {
	get_bytes((char *)&value,sizeof(value));
}

void Serializer_writer::rw(double &value) {
	get_bytes((char *)&value,sizeof(value));
}

void Serializer_writer::rw(Vector &value) {
	get_bytes((char *)&value,sizeof(value));
}

void Serializer_writer::rwstr(char *value, int maxsize) {
	Sint16 slen; rw(slen);
	int len = Math::min(slen,maxsize-1);
	//int len=min(reader_int(),maxsize-1);
	get_bytes(value,len);
	value[len]=0; // always terminate...
	//logmsg(lt_debug, "!!!writer rwstr len %d, pos %d, string %s",slen,get_pos(), value); //D8
}

void Serializer_writer::rwblob(char *blob, int size) {
	get_bytes(blob,size);
}

int Serializer_writer::reader_int() {
	int i;
	rw(i);
	return i;
}

float Serializer_writer::reader_float() {
	float f;
	rw(f);
	return f;
}

void Serializer_writer::rewind() {
	empty_read_error = false;
	curmsg = 0;
}

bool Serializer_writer::is_empty() {
	return curmsg >= buffer_inuse;
}

bool Serializer_writer::empty_read() {
	return empty_read_error;
}

int Serializer_writer::get_pos() {
	return curmsg;
}
