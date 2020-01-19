/***************************************************************************
  obj_chatmessage.cpp  -  send chat messages to other players
 ---------------------------------------------------------------------------
  begin                : 2003-05-11
  by                   : Nils Thuerey
  email                : n_t@gmx.de

  last changed         : 2004-03-29
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2003 by Nils Thuerey 
  email                : n_t@gmx.de

 ***************************************************************************/

#include "obj_chatmessage.h"

Obj_chatmessage::Obj_chatmessage(World *world_, int id) : Obj_base(world_,id) {}

Obj_chatmessage::~Obj_chatmessage() {}
       
void Obj_chatmessage::init(int setid) {
	color = PIXEL(0,0,0);
	ownerid = setid;
	message[0] = 0;
}

void Obj_chatmessage::initmessage(char *txt, int32 setcol) {
	strcpy(message, txt);
	color = setcol;
}

void Obj_chatmessage::draw(Viewport *viewport) {}

void Obj_chatmessage::serialize(Serializer *serializer) {
	//Obj_base::serialize(serializer);
	serializer->rw(color);
	serializer->rwstr(message,sizeof(message));
}

void Obj_chatmessage::server_think() {}
