/***************************************************************************
    obj_chatmessage.cpp  - global settings of the server (game mode etc.)
                             -------------------
    begin                : 2003-05-11
    by                   : Nils Thuerey 
    email                : n_t@gmx.de
    
    last changed         : 2004-03-30
    copyright            : Christoph Brill
    email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.
    
    copyright            : (C) 2003 by Nils Thuerey 
    email                : n_t@gmx.de

 ***************************************************************************/

#include "common/objects/obj_settings.h"
#include "common/console/logmsg.h"

//! game mode names
const char *globalGameModeNames[] = {
	"deathmatch",
	"teamplay",
	""
};

Obj_settings::Obj_settings(World *world_, int id) : Obj_base(world_,id) {
	mGameMode = gm_deathmatch;
	mWorldSizeX = mWorldSizeY = -1;
}

Obj_settings::~Obj_settings() {}

void Obj_settings::setGameMode(const char *set) {
	int i = 0;
	while(strlen(globalGameModeNames[i]) > 0) {
		if(!strcmp(globalGameModeNames[i], set)) {
			mGameMode = i;
			return;
		}
		i++;
	}
	// no mode found??
	logmsg(lt_warning,"Invalid game mode setting %s", set);
	mGameMode = gm_deathmatch;
}

void Obj_settings::serialize(Serializer *serializer) {
	//Obj_base::serialize(serializer);
	serializer->rw( mGameMode );
	//serializer->rwstr(message,sizeof(message));
}

void Obj_settings::server_think() {}

void Obj_settings::draw(Viewport *viewport) {}
