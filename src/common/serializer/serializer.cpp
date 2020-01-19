/***************************************************************************
  serializer.cpp  -  NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2000-01-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  last changed         : 2005-01-04
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : code and API cleanup

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/serializer/serializer.h"
#include "common/systemheaders.h"
#include "common/misc.h"
#include "common/console/logmsg.h"

Serializer::Serializer(){}

Serializer::~Serializer(){}

void Serializer::dump() {
	char tmp[500];
	char *t=tmp;
	
	t += sprintf(t, "%i bytes: ", buffer_inuse);

	for (int i=0; i<buffer_inuse; i++) {
		int b = buffer[i];
		t += sprintf(t, "%x ", b);
	}

	logmsg(lt_debug, tmp);
}

int Serializer::get_length() {
	return buffer_inuse;
}
