/***************************************************************************
  world_objects.h  -  Header for NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2000-02-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Add obj_base.h header to make this selfstanding.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef WORLDOBJECTS_H
#define WORLDOBJECTS_H

#include "common/objects/obj_base.h"
#include <map>

//! A list of objects
class World_objects :public std::map<int,Obj_base*>{};

#endif
