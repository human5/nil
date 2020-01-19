/***************************************************************************
  nil_files.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-07
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

    copyright            : (C) 2000 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

#include "nil_files.h"

void swap32(Sint32 &value) {
	value = htonl(value);
}

void swap16(Sint16 &value) {
	value = htons(value);
}
