/***************************************************************************
    obj_chatmessage.h    - global settings of the server (game mode etc.)
                             -------------------
    begin                : 2003-05-11
    copyright            : (C) 2003 by Nils Thuerey 
    email                : n_t@gmx.de
 ***************************************************************************/


#ifndef OBJ_SETTINGS_H
#define OBJ_SETTINGS_H

#include "obj_base.h"

//! @author Nils Thuerey
//! @brief global settings of the server (game mode etc.)
class Obj_settings : public Obj_base  {
public: 
	//! Contructor thats inits default settings
	Obj_settings(World *world_, int id);
	//! Destructor (empty)
	virtual ~Obj_settings();
	//! game mode settings
	typedef enum {
		//! normal deathmatch
		gm_deathmatch = 0,
		//! team deathmatch
		gm_teamplay
	} GameMode;
	//! return ot_settings as type
	virtual Object_type gettype(){ return ot_settings; }
	//! unused, it is displayed via game_client message array
	virtual void draw(Viewport *viewport);
	//! transfer server settings to clients
	virtual void serialize(Serializer *serializer);
	//! unused
	virtual void server_think();
	//! set game mode
	void setGameMode(GameMode set) { mGameMode = (Sint8)set; }
	//! set game mode from string
	void setGameMode(const char *set);
	//! get game mode
	GameMode getGameMode( void ){ return (GameMode)mGameMode; }

	//! set world size
	void setWorldSize(int setx, int sety){ mWorldSizeX = setx; mWorldSizeY = sety; }
	//! get world size x
	Sint16 getWorldSizeX( void ){ return mWorldSizeX; }
	//! get world size y
	Sint16 getWorldSizeY( void ){ return mWorldSizeY; }
	
	//! set armageddon time
	void setArmageddonTime(Sint8 set) { mArmageddonTime = set; }
	//! returns armageddon time
	Sint8 getArmageddonTime(void) { return mArmageddonTime; }
	
protected:
	//! game mode settings
	Sint8 mGameMode;
	//! size of the game world
	Sint16 mWorldSizeX, mWorldSizeY;
	//! activate armageddon if >0 (makes world shift down a bit in some interval)
	Sint8 mArmageddonTime;
};

#endif
