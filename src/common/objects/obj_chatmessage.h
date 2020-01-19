/***************************************************************************
  obj_chatmessage.h  -  Header for sending chat messages to other players
 ---------------------------------------------------------------------------
  begin                : 2003-05-11
  by                   : Nils Thuerey
  email                : n_t@gmx.de

  last changed         : 2004-03-28
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2003 by Nils Thuerey
  email                : n_t@gmx.de

 ***************************************************************************/

#ifndef OBJ_CHATMESSAGE_H
#define OBJ_CHATMESSAGE_H

#include "obj_base.h"

//! @author Nils Thuerey
//! @brief a simple object for sending messages
class Obj_chatmessage : public Obj_base  {
public:
	//! Constructor (empty)
	Obj_chatmessage(World *world_, int id);
	//! Destructor (empty)
	virtual ~Obj_chatmessage();
	//! init message text with color, owner and lentgh
	void init(int setid);
	//! init a chat message for sending etc.
	void initmessage(char *txt, int32 setcol);
	//! return message
	char *get_message( void ) { return message; }
	//! return msg color
	int32 get_color( void ) { return color; }
	//! return ot_chatmessage as type
	virtual Object_type gettype(){ return ot_chatmessage; }
	//! don't do anything, it is displayed via game_client message array
	virtual void draw(Viewport *viewport);
	//! serialize color and the message (with length)
	virtual void serialize(Serializer *serializer);
	//! don't do anything, because there is not much to think
	virtual void server_think();
	//! constant: number of messages
	static const int MSGNUM = 10;
	//! constant: length of messages
	static const int MSGLENGTH = 70;
	//! constant: standard color for system messages
	static const int32 SYSMSGCOL = PIXEL(255,255,255);
	//! constant: duration of display
	static const int DISPLAYTIME = 10;

protected:
	//! Message color
	int32 color;
	//! Message text
	char message[100];
};

#endif
