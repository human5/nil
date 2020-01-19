/***************************************************************************
  clientworld.cpp  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-20
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  expansion            : 2000-01-30
  by                   : Eero Tamminen
  email                : t150315@cc.tut.fi

  last changed         : 2004-03-07
  by                   : Nils Thuerey
  email                : n_t@gmx.de

    copyright            : (C) 1999 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

#ifndef CLIENTWORLD_H
#define CLIENTWORLD_H

#include "common/world.h"
#include "common/objects/obj_chatmessage.h"
#include "common/net/netpackets.h"
#include "common/serializer/serializer_writer.h"

//! @author Flemming Frandsen
//! @brief class for the soundsystem
class Soundsystem;

//! @author Flemming Frandsen
//! @brief class for the client sided part of world
class Clientworld : public World  {
public: 
	//! Constructor with options
	Clientworld(Loader *loader_, Soundsystem *soundsystem_);
	//! Constructor
	Clientworld();
	//! Destructor
	~Clientworld();
	//! initialize
	void init(Loader *loader_, Soundsystem *soundsystem_);
	//! serialize
	Serializer_writer serializer_writer;
	/*! Called by the client to parse the thoughts stored in the thoughts. Make sure to set the thoughts
	    before calling client_think()! */
	void client_think();
	//! returns that this is a client
	virtual bool isclient() { return true; }
	/*! Creates an anonymous object (one without id, that only exists on the client). These
	    objects are NOT allowed to interact with real objects and they cannot change the map */
	Obj_base *newanonobject(Object_type objecttype);
	//! renders the world (map and objects) into the viewport
	void draw(Viewport *viewport);
	//! 0=no gore, 5=normal, 10=insane
	int gorelevel;
	//! modify gorelevel by config file
	void set_gorelevel(int set) { gorelevel = set; }
	//! puts a hole to the map and spawns a couple of debris particles
	virtual void makehole(int x, int y, int diameter);
	//! creates a map
	bool createmap(Net_joinreply &joinreply, char *map_data);
	//! returns number of anonymous objects
	int get_anon_objects_inuse() { return anon_objects_inuse; }
	//! add new message to arrays
	void add_message( char *txt, int32 col );
	//! add new suicide message to arrays
	void add_suicidemessage( int pkid );
	//! add new frag message to arrays
	void add_fragmessage( int plid, int pvid );
	//! message access for game client
	char *get_message(int i){ return gameMsgText[i]; }
	//! message color for displaying in game client
	int32 get_msgcolor(int i){ return gameMsgColor[i]; }
	//! still display message?
	bool get_messagedisplay(int i) { return( gettime()-gameMsgTime[i]<Obj_chatmessage::DISPLAYTIME ); }
	//! display all anonymous objects in the world
	void dumpAnonObjects();
	//! set scoreboard id from join init
	void setScoreboardId(int set){ scoreboard_id = set; }
	//! ???
	inline Sint32 getxsize() { return mSizeX; }
	//! ???
        inline Sint32 getysize() { return mSizeY; }
protected:
	//! array for anonymous objects like particles that are not serialized
	Obj_base *anon_objects[MAX_ANON_OBJECTS];
	//! no of anonymous objects (particles, blood etc.) currently used
	int anon_objects_inuse;
	//! color of each msg 
	int32 gameMsgColor[Obj_chatmessage::MSGNUM];       
	//! text of all messages
	char  gameMsgText[Obj_chatmessage::MSGNUM][Obj_chatmessage::MSGLENGTH];
	//! creation time of each msg
	float gameMsgTime[Obj_chatmessage::MSGNUM];
	
};

#endif
