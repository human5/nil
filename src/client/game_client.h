/***************************************************************************
  game_client.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-03
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-29
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Don't bother with the whole file without SDL.

  last changed         : 2004-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : add joystick support, minor API cleanup

  last changed         : 2004-01-15
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : cleanup

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#ifndef GAME_CLIENT_H
#define GAME_CLIENT_H

#ifndef WITHOUT_SDL

#include "common/configuration/configuration.h"
#include "client/clientworld.h"
#include "common/vfs/loader.h"
#include "client/controls/controls.h"
#include "client/controls/keymapper.h"
#include "client/graphics/viewport.h"
#include "client/graphics/graphic_bar.h"
#include "client/graphics/graphic_msg.h"
#include "client/fonts/font_instance.h"
#include "common/objects/obj_chatmessage.h"
#include "client/menu/menu.h"
#include "common/net/tcp_client.h"
#include "client/event/event_target.h"

// The maximum number of *local* players.
#define MAX_PLAYERS 4

//! @author Flemming Frandsen
//! @brief the game client
class Game_client : public Configuration_observer, public Event_target {
public:
	//! Constructor
	Game_client();
	//! Destructor
	~Game_client();
	/*! Call to get things going
	    @param loader_ A reference to the loader
	    @param soundsystem_ A reference to the soundsystem
	    @param configuration_ A reference to the configuration
	    @return ??? */
	int init(Loader *loader_, Soundsystem *soundsystem_, Configuration *configuration_);
	/*! Set geometry
	    @param xpos_ ???
	    @param ypos_ ???
	    @param view_xsize ???
	    @param view_ysize ???
	    @param view_quality_ ???
	    @return ??? */
	int setgeom(int xpos_, int ypos_, int view_xsize, int view_ysize, int view_quality_);
	/*! Must be called before addplayer (join_game only requests the map from the server now;
	    The player addition is done elsewhere.)
	    @param server The name of the server
	    @param server_port The remote TCP port of the server
	    @param nplayers_ The number of local players that will join
	    @param name The name of the local player 1
	    @param color The color of the player
	    @param player_team The team of the player
	    @return ??? */
	int join_game(const char *server, int server_port, int nplayers_, const char *name, int color, Sint8 player_team);
	//FIXME: setup controls in the following function
	/*! Adds a local player to the game.
	    @param player_name The name of the player
	    @param player_color The color of the player
	    @return ??? */
	int addplayer(const char *player_name, int player_color);
	//void get_join_status(char *text, int textsize, int &progress_, int &max_progress_); remember to have a semaphor
	/*! poll once for each frame: it gets new data from the server
	    @return Returns 0 on success, -1 on recieving error and -2 on sending error*/
	int think();
	//! This does the routing of events (the default is to let the event happen to this object)
	virtual bool handle(Nil_inputevent *event);
	//! keyboard events that a target can handle
	virtual bool handle_keyboard_key(Event_keyboard_key *key_event);
	//! mouse button events that a target can handle
	virtual bool handle_mouse_key(Event_mouse_key *mouse_key_event);
	//! mouse movement events that a target can handle
	virtual bool handle_mouse_motion(Event_mouse_motion *mouse_motion_event);
	//! mouse button events that a target can handle
	virtual bool handle_joystick_key(Event_joystick_key *joystick_key_event);
	//! mouse movement events that a target can handle
	virtual bool handle_joystick_motion(Event_joystick_motion *joystick_motion_event);
	//! handle the client movement
	bool handle_avatar_actions(Nil_inputevent* event, int player);
	/*! call to have this game client draw it's view of the world
	    \param target ???
	    \return ???*/
	int draw(Mutable_raw_surface *target);
	//! ???
	float get_trend_tick_duration();
	//! This contains: the offsets from our position, name, color, team, owner, frags, suicides, messages, etc.
	struct player_info {
		//! the x position of the player
		int         xpos;
		//! the y position of the player
		int         ypos;
		//! the width of the player
		int         width;
		//! the height of the player
		int         height;
		//! the player name
		Playername  player_name;
		//! the players color
		Uint32      player_color;
		//! the team
		Sint8       team;
		//! the owner of a player
		int         owner;
		//! the number of the joystick
		int         joystick;
		//! the controlls interface
		Controls    controls;
		//! to store if we will elevate up
		bool        elevate_up;
		//! to store if we will elevate down
		bool        elevate_down;
		//! the health bar
		Graphic_bar health_bar;
		//! the weapons bar
		Graphic_bar weapon_bar[WEAPON_SLOTS];
		//! the frag info
		Graphic_msg frag_info;
		//! our viewport (what the players is seeing of our world)
		Viewport    *viewport;
	};
	/*! returns the current active weapon of the player
	    \param player The player we want the info from
	    \return Returns the current active weapon */
	int get_weapon(int player);
	/*! returns the address of the pointer of the specified player
	    \param player The player we want to query
	    \return Returns the player info */
	player_info *get_player_info(int player) { return &players[player]; }
	/*! returns the amount of players
	    \return Returns the number of players */
	int get_numplayers();
	/*! Get the avatar for a player
	    \param player The player we want to have the avatar from
	    \return Returns the player avatar*/
	Obj_player_avatar* get_player(int player);
	//! the owner id of a player
	int get_ownid(int player);
	//! the controls of a player
	Controls& get_controls(int player);
	//! the client sided part of the world
	Clientworld clientworld;
	//! retruns the x size of the clientworld
	inline int get_worldsizex() { return clientworld.getxsize(); }
	//! retruns the y size of the clientworld
	inline int get_worldsizey() { return clientworld.getysize(); }
	//! ???
	int last_update_size;
	//! gets the player id
	int get_player_id(int player) { return players[player].owner; }
	//! get the playerinfo
	player_info *get_player() { return &players[0]; }
	//! Just checks who's using what joysticks.
	void config_changed();
	//! font for stats
	Font_instance *smallfont;
	//! font for menu
	Font_instance *bigfont;
	//! draw all messages in small font 
	void draw_messages( Mutable_raw_surface *target ); 
	//! draw  the scoreboard (basically just calls draw routine of scoreboard object)
	void draw_scoreboard( Mutable_raw_surface *target );
	//! draw the Menu (calls draw routine of menu)
	void draw_menu(Mutable_raw_surface *target, Menu *menu);
	//! add chat message for first player
	void add_chatmessage(); 
	//! return chat string for nil client
	char *getChatString() { return mChatString; }
	//! say that it's time to quit
	void set_quit();
	//! change the active event handler
	void set_event_handler(Event_target* event_handler_);

	//! will we smooth the surface?
	bool smooth;
	//! will the menu be drawn?
	bool menu_visible;
	//! will the chat be drawn?
	bool chat_visible;
	//! will the console be drawn?
	bool console_visible;
	//! will we take a screenshot?
	bool screen_shoot;
	//! will the traffic be drawn?
	bool traffic_visible;
	//! will the fps counte be drawn?
	bool fps_visible;
	//! will the logo be drawn?
	bool logo_visible;
	//! will we drop shadows?
	bool shadow_visible;
	//! is the death cam used?
	bool death_cam_active;
	//! is the death cam active?
	bool death_cam_armed;
	//! will we draw the scoreboard?
	bool scoreboard_visible;
	//! will we quit?
	bool done;
	//! our event target (this is where the input is sent to)
	Event_target* event_handler;

protected:
	// FIXME: use arbitrary number (STL vector?)
	//! number of local players
	int nplayers;
	//! ???
	player_info players[MAX_PLAYERS];
	//! ???
	float last_tick_time;
	//! ???
	float last_tick_duration;
	//! ???
	float trend_tick_duration;
	//! if the controls were processed
	bool controls_ticked;
	//! process our controls
	void controls_tick();
	//! the x location of a ???
	int xloc;
	//! the x location of a ???
	int yloc;
	//! the width of a ???
	int width;
	//! the height of a ???
	int height;
	//! the x and y size of the map
	int map_xsize, map_ysize;
	//! ???
	int view_quality;
	//! decides if the bars are drawn on floor or top of the screen
	bool bars_at_top;
	//! The TCP/IP client
	Tcp_client tcp_client;
	//! Handle to the loader
	Loader *loader;
	//! Handle to the soundsystem
	Soundsystem *soundsystem;
	//void set_join_status(char *text, int &progress_, int &max_progress_);

private:
	//! sets up the viewport
	void setup_viewports();
	//! all messages to display in nil_client.cpp
	Graphic_msg gameMessages[Obj_chatmessage::MSGNUM];
	//! message ready to be sent?
	bool mSendMsg;
	//! contents of the chat message
	char mChatString[Obj_chatmessage::MSGLENGTH];
	//! to display chat message from nil_client
	Graphic_msg mChatMessage;
};

#endif
#endif
