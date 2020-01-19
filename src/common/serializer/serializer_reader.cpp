/***************************************************************************
  serializer_reader.cpp  -  NEEDDESCRIPTION
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

#include "common/serializer/serializer_reader.h"
#include "common/systemheaders.h"
#include "common/console/logmsg.h"
#include "common/sexchanger.h"

Serializer_reader::Serializer_reader() {
	buffer_size = 1000;
	buffer = (char *)malloc(buffer_size);
	clear();
}

Serializer_reader::~Serializer_reader() {
	free(buffer);
}

void Serializer_reader::clear() {
	buffer_inuse = 0;
}

bool Serializer_reader::is_reading() {
	return true;
}

char *Serializer_reader::get_buffer() {
	return buffer;
}

void Serializer_reader::writer_int(int value) {
	rw(value);
}

void Serializer_reader::writer_float(float value) {
	rw(value);
}

int Serializer_reader::get_pos() {
	return buffer_inuse;
}

void Serializer_reader::add_bytes(char *data, int length) {
	if (length + buffer_inuse > buffer_size) {
		buffer_size *= 2;
		while (length + buffer_inuse > buffer_size) buffer_size *= 2;
		buffer = (char *)realloc(buffer,buffer_size);
	}
	memcpy(&buffer[buffer_inuse], data, length);
	buffer_inuse += length;
}

void Serializer_reader::rw(Sint8 &value) {
	add_bytes((char *)&value,sizeof(value));
}

void Serializer_reader::rw(Sint16 &value) {
	Sint16 sendval = getChangedByteOrder16(value);
	add_bytes((char *)&sendval,sizeof(value));
}

void Serializer_reader::rw(Sint32 &value) {
	Sint32 sendval = getChangedByteOrder32(value);
	add_bytes((char *)&sendval,sizeof(value));
}

void Serializer_reader::rw(int32 &value) {
	Sint32 sendval = getChangedByteOrder32(value);
	add_bytes((char *)&sendval,sizeof(value));
}

void Serializer_reader::rw(bool &value) {
	Sint8 sendval = value;
	add_bytes((char *)&sendval,sizeof(value));
}

void Serializer_reader::rw(float &value) {
	add_bytes((char *)&value,sizeof(value));
}

void Serializer_reader::rw(double &value) {
	add_bytes((char *)&value,sizeof(value));
}

void Serializer_reader::rw(Vector &value) {
	add_bytes((char *)&value,sizeof(Vector));
}

void Serializer_reader::rwstr(char *value, int maxsize) {
	Sint16 slen = strlen(value);
	Sint16 len = Math::min(slen,maxsize-1);
	rw(len);
	add_bytes(value,len);
	//logmsg(lt_debug, "!!!writer rwstr len %d, pos %d, string %s",len,get_pos(), value); //D8
}

void Serializer_reader::rwblob(char *blob, int size) {
	add_bytes(blob,size);
}
