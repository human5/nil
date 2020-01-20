/***************************************************************************
  game_client.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-03
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-01-29
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : add joystick support, minor API cleanup,
                         add mouse support

  last changed         : 2004-01-15
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : cleanup and dropshadow fixes

  last changed         : 2004-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : random colors for local players

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "client/game_client.h"
#include "common/objects/obj_player_avatar.h"
#include "common/console/logmsg.h"
#include "common/objects/obj_player_data.h"
#include "common/objects/obj_scoreboard.h"

Game_client::Game_client() {
	// This must happen before setgeom()!
	nplayers=0;
	for(int i=0; i < MAX_PLAYERS; ++i) {
		players[i].player_name[0] = '\0';
		players[i].viewport = NULL;
		players[i].team = -1;
		players[i].owner = -1;
		players[i].joystick = i;
		players[i].controls.set_owner(i);
		players[i].elevate_up = false;
		players[i].elevate_down = false;
	}

	last_tick_time = 0;
	last_tick_duration = 1;
	loader = NULL;
	smooth = false;
	bars_at_top = false;
	smallfont = NULL;
	bigfont = NULL;
	//setgeom(0,0,320,240,2);

	//NT init
	mSendMsg = false;
	mChatString[0] = 0;
	//CB init
	trend_tick_duration = 1;
	
	menu_visible = false;
	chat_visible = false;
	console_visible = false;
	screen_shoot = false;
	traffic_visible = false;
	fps_visible = true;
	logo_visible = true;
	shadow_visible = true;
	death_cam_active = false;
	death_cam_armed = false;
	scoreboard_visible = false;
	done = false;
	
}

Game_client::~Game_client() {
	logmsg(lt_debug, "Sending quit signal...");
	if (tcp_client.send_int(NETSTATE::QUIT) < 0) {
		logmsg(lt_error, "Unable to tell the server we want to quit.");
	} else {
		logmsg(lt_debug, "Sent quit netstate to server");
	}

	for (int i=0; i<nplayers; ++i) {
		logmsg(lt_debug, "Freeing viewport for player %i.", i);
		delete players[i].viewport;
	}
}

int Game_client::init(Loader *loader_, Soundsystem *soundsystem_, Configuration *configuration_) {
	loader = loader_;
	soundsystem = soundsystem_;
	config_attach(configuration_);
	config_changed();
	return 0;
}

void Game_client::config_changed() {
	players[0].joystick = configuration->get_option_int("Player1_joystick");
	players[1].joystick = configuration->get_option_int("Player2_joystick");
	players[2].joystick = configuration->get_option_int("Player3_joystick");
	players[3].joystick = configuration->get_option_int("Player4_joystick");

	for (int i=0; i<MAX_PLAYERS; ++i)
		logmsg(lt_debug, "Player %i is using joystick %i", i, players[i].joystick);
}

void Game_client::setup_viewports() {
	if (nplayers == 1) {
		players[0].xpos = 0;
		players[0].ypos = 0;
		players[0].width  = width;
		players[0].height = height;
		delete players[0].viewport;
		players[0].viewport = new Viewport(width/view_quality,
		                                   height/view_quality,
		                                   players[0].owner,
		                                   map_xsize, map_ysize);
	} else if (nplayers == 2) {
		int w = width / 2 - 2;
		players[0].xpos   = 0;
		players[0].ypos   = 0;
		players[0].width  = w;
		players[0].height = height;
		delete players[0].viewport;
		players[0].viewport = new Viewport(w/view_quality,
		                                   height/view_quality,
		                                   players[0].owner,
		                                   map_xsize, map_ysize);
		players[1].xpos   = w + 4;
		players[1].ypos   = 0;
		players[1].width  = w;
		players[1].height = height;
		delete players[1].viewport;
		players[1].viewport=new Viewport(w/view_quality,
		                                 height/view_quality,
		                                 players[1].owner,
		                                 map_xsize, map_ysize);
	} else if (nplayers >= 3) {
		int w = width / 2 - 2, h = height / 2 - 2;
		players[0].xpos = 0;
		players[1].xpos = w+4;
		players[2].xpos = 0;
		if (nplayers == 4)
			players[3].xpos = w + 4;
		players[0].ypos = 0;
		players[1].ypos = 0;
		players[2].ypos = h + 4;
		if (nplayers == 4)
			players[3].ypos = h + 4;
		for (int i=0; i<nplayers; ++i) {
			players[i].width  = w;
			players[i].height = h;
			delete players[i].viewport;
			players[i].viewport = new Viewport(w/view_quality,
			                                   h/view_quality,
			                                   players[i].owner,
			                                   map_xsize, map_ysize);
		}
	}

	// set the healthbars for every player
	for(int i=0; i<nplayers; ++i) {
		players[i].health_bar.setgeom(5,players[i].height-(7+5),
		                              100*players[i].width/640,7);
		players[i].health_bar.bar_color = PIXELA(128,0,255,8);
		players[i].health_bar.render();
	}
}

int Game_client::setgeom(int xpos_, int ypos_, int view_xsize, int view_ysize, int view_quality_) {
	xloc = xpos_;
	yloc = ypos_;
	width=view_xsize;
	height=view_ysize;
	view_quality = view_quality_;

	setup_viewports();
	return 0;
}

int Game_client::join_game(const char *server, int server_port, int nplayers_, const char *name, int color, Sint8 player_team) {
	nplayers=nplayers_;
	for(int i=0; i<nplayers; ++i) {
		if (i == 0) {
			// use given color and name for player 1
			strcpy(players[i].player_name, name);
			players[i].player_color = color;
		} else {
			// and generate a random color and different name for other local players
			sprintf(players[i].player_name, "%s_%i", name, i+1);
			players[i].player_color = mkrndcolor(0x00000, 0xffffff);
		}
		players[i].viewport = NULL;
		players[i].team = player_team;
		//players[i].controls.set_elevation((MINELEVATION+MAXELEVATION)/2);
	}

	{
		unsigned int firsttry = time_ms();
		int error = -1;

		logmsg(lt_debug,"Trying to connect to server %s:%i",server,server_port);
		while (error < 0 && time_elapsed(firsttry,time_ms()) < 10000) {
			error = tcp_client.connect(server,server_port);
			if (error < 0) {
				sleep(1);
				logmsg(lt_debug,"Couln't connect, but trying again...");
			}
			//FIXME: NT try to catch ESC here...
		}

		if (error < 0) {
			logmsg(lt_error,"Unable to get connected to the server, quitting. %i",error);
			return -1;
		} else {
			logmsg(lt_message,"Yes!, we are connected to the server!");
		}
	}

	// check server signature and protocol id
	{
		Net_server_signature server_signature;
		if (tcp_client.recv_obj(&server_signature) < 0) {
			logmsg(lt_error,"Unable to get server signature");	
			return -2;
		}

		if (!server_signature.server_ok()) {
			logmsg(lt_fatal,"Server is not a NiL server, it said: %s",server_signature.signature);
			return -3;
		};

		if (!server_signature.version_ok()) {
			logmsg(lt_fatal,"Server is a NiL server, but it has the wrong version: %s you are using %s",server_signature.version,PROTOCOL_VERSION);
			return -4;
		};
	}

	// setup join request (information about local players)
	Net_joinrequest joinrequest;
	joinrequest.nplayers=nplayers;

	for(int i=0; i<nplayers; ++i) {
		strcpy(joinrequest.players[i].name, players[i].player_name);
		joinrequest.players[i].color=players[i].player_color;
		joinrequest.players[i].team=players[i].team;
	}

	if (tcp_client.send_obj(&joinrequest) < 0) {
		logmsg(lt_error,"tcp_client.send(&joindata,sizeof(joindata) failed.");
		return -5;
	}
	logmsg(lt_debug,"join request sent");

	//get join reply
	Net_joinreply joinreply;
	if (tcp_client.recv_obj(&joinreply) < 0) {
		logmsg(lt_error,"tcp_client.recv(&joinreply,sizeof(joinreply)) failed.");
		return -6;
	}
	logmsg(lt_debug,"got join reply from server.");

	for(int i=0; i<nplayers; ++i) {
		logmsg(lt_debug, "Player %i has id %i", i, joinreply.player_ids[i]);
		players[i].owner=joinreply.player_ids[i];
	}

	//create client world
	clientworld.init(loader, soundsystem);
	logmsg(lt_debug,"client world created, about to initmap.");

	{
		//get the compressed map:
		char *map_data = (char*)malloc(joinreply.compressed_size);
		if (tcp_client.recv_buf(map_data,joinreply.compressed_size) < 0) {
			logmsg(lt_error,"Couldn't read compressed map diff.");
			return -7;
		}
		logmsg(lt_debug,"got map.");

		//have the clientworld unpack the packed gob:
		clientworld.createmap(joinreply,map_data);
		free(map_data);
		logmsg(lt_debug,"initmap done.");
	}

	map_xsize=joinreply.map_xsize;
	map_ysize=joinreply.map_ysize;


	//NT receive all objects before starting...	
	logmsg(lt_debug,"Joining: receiving objects...");

	// fill our writer
	Serializer_writer serializer_writer;
	tcp_client.recv_ser(&serializer_writer);

	int countObjects=0;
	while (serializer_writer.next_int() == TT_OBJECT_STATE) {
		serializer_writer.eat_int(TT_OBJECT_STATE);

		int winners_id = serializer_writer.reader_int();
		Object_type winners_type = (Object_type)serializer_writer.reader_int();

		//Now create the object and hand it it's data:
		Obj_base *lucky_winner = clientworld.newobject(winners_type, winners_id);
		//logmsg(lt_debug,"Joining: got new object id:%d, type:%d = %s",winners_id, winners_type, OBJECT_NAMES[winners_type] );
		lucky_winner->serialize(&serializer_writer);
		countObjects++;

		// remember special objects
		if (winners_type == ot_scoreboard) clientworld.setScoreboardId( winners_id );
		if (winners_type == ot_settings) clientworld.setServerSettings( (Obj_settings *)lucky_winner );
	}
	logmsg(lt_debug,"Joining: received %d objects...",countObjects);
	if(serializer_writer.get_length() > serializer_writer.get_pos() ) {
		logmsg(lt_error,"Joining failed - serializer not finished!");
		return -9;
	}

	//send handshake:
	int handshake = 42;
	if (tcp_client.send_int(handshake) < 0) {
		logmsg(lt_error,"Couldn't send handshake.");
		return -8;
	}

	//setup_viewports();
	// update player info with stuff assigned by the server
	for(int i=0; i<nplayers; ++i) {
		Obj_player_data *playerData = (Obj_player_data *)clientworld.get_object(players[i].owner,ot_player_data);
		if (!playerData) {
			logmsg(lt_warning,"Join: unable to find player data for player %d: id %d",i,players[i].owner );
			return -1;
		}
		players[i].player_color=playerData->getcolor();
		players[i].team=playerData->getTeam();
		//logmsg(lt_warning,"GOT %d, %d ", players[i].player_color, players[i].team );
	}


	logmsg(lt_debug,"ready to go.");
	return 0;
}

//-----------------------------------------------------------------------------
// void Game_client::get_join_status(char *text, int textsize, int &progress_, int &max_progress_); remember to have a semaphor
//-----------------------------------------------------------------------------
// poll once for each frame:
int Game_client::think() {
	int netstate = -1;
	int this_update_id = -1;

	//Get the update id:
	tcp_client.recv_int(netstate);
	if (netstate != NETSTATE::UPDATEID) {
		logmsg(lt_error,"protocol error 1: got netstate %i.", netstate);
		return -1;
	}
	if (tcp_client.recv_int(this_update_id) < 0) {
		logmsg(lt_error,"unable to get the update_id from the server.");
	}

	//Get the world update itself
	tcp_client.recv_int(netstate);
	if (netstate != NETSTATE::WORLDUPDATE) {
		logmsg(lt_error,"protocol error 2.");
		return -1;
	}
	if (tcp_client.recv_ser(&clientworld.serializer_writer) < 0) {
		logmsg(lt_error,"unable to get the worldupdate from the server.");
		return -1;
	}

	//Get the done flag:
	tcp_client.recv_int(netstate);
	if (netstate != NETSTATE::DONE) {
		logmsg(lt_error,"protocol error 3.");
		return -1;
	}

	//Send ack for this update:
	if (tcp_client.send_int(NETSTATE::UPDATEID) < 0) {
		logmsg(lt_error,"unable to send state to the server.");
		return -2;
	}
	if (tcp_client.send_int(this_update_id) < 0) {
		logmsg(lt_error,"unable to send this_update_id to the server.");
		return -2;
	}

	for(int i=0; i<nplayers; ++i) {
		//send the current controls
		if (tcp_client.send_int(NETSTATE::CONTROLS) < 0) {
			logmsg(lt_error,"unable to send state to the server.");
			return -2;
		}
		if (tcp_client.send_obj(&players[i].controls) < 0) {
			logmsg(lt_error,"unable to send controls to the server.");
			return -2;
		}
	}

	// send chat message?
	if(mSendMsg) {
		//send the message
		if (tcp_client.send_int(NETSTATE::CHATMSG) < 0) {
			logmsg(lt_error,"unable to send chatmsg-state to the server.");
			return -2;
		}
		Obj_chatmessage tmp(dynamic_cast<World *>(&clientworld), -1);
		tmp.initmessage( mChatString, players[0].player_color );
		if (tcp_client.send_obj(dynamic_cast<Obj_base *>(&tmp)) < 0) {
			logmsg(lt_error,"unable to send chatmsg to the server.");
			return -2;
		}
		mSendMsg = false;
	}
	
	//Tell the server that there is no more lumps for now
	if (tcp_client.send_int(NETSTATE::DONE) < 0) {
		logmsg(lt_error,"unable to send state to the server.");
		return -2;
	}


	//update some statistics:
	last_update_size = clientworld.serializer_writer.get_length();
	last_tick_duration = clientworld.gettime()-last_tick_time;
	last_tick_time = clientworld.gettime();
	if (last_tick_duration > 10) {
		last_tick_duration = 1;
		trend_tick_duration = 1;
	}
	trend_tick_duration += (last_tick_duration-trend_tick_duration)/20;

	clientworld.client_think();

	controls_tick();

	return 0;
}

float Game_client::get_trend_tick_duration() {
	return trend_tick_duration;
}

void Game_client::draw_messages( Mutable_raw_surface *target ) {
	Sint16 yStart = 10+5;

	//smallfont->put_text(10,30,mChatString,target,true,255,255,255);
	for(int i=0; i<Obj_chatmessage::MSGNUM; i++) {
		// display only if font is ok, msg contains text, and time is not over
		if( ( gameMessages[i].is_font() ) && 
				(strlen(clientworld.get_message(i))>0) &&
				(clientworld.get_messagedisplay(i))	) {

			// draw background
			int32 msgbackcol = makeBackgroundColor(clientworld.get_msgcolor(i));
			gameMessages[i].setgeom ( 10+1 , height-yStart-i*13+1 , clientworld.get_message(i) ,
			                          GETRED(msgbackcol),
			                          GETGREEN(msgbackcol),
			                          GETBLUE(msgbackcol) );
			gameMessages[i].render( );
			gameMessages[i].draw( target );

			gameMessages[i].setgeom ( 10 , height-yStart-i*13 , clientworld.get_message(i) , 
			                          GETRED(clientworld.get_msgcolor(i)),
			                          GETGREEN(clientworld.get_msgcolor(i)),
			                          GETBLUE(clientworld.get_msgcolor(i)) );
			gameMessages[i].render( );
			gameMessages[i].draw( target );

		} // font there?
	}

}

void Game_client::draw_scoreboard( Mutable_raw_surface *target ) {
	Obj_scoreboard *sb = clientworld.getscoreboard ();
	if(sb) {
		// we have it -> draw
		sb->drawTable( target, smallfont );
	}
}

void Game_client::draw_menu(Mutable_raw_surface *target, Menu *menu) {
	menu->draw_menu(target);
}

void Game_client::add_chatmessage() {
	// add player name
	char chatOut[Obj_chatmessage::MSGLENGTH];
	if( -1 == snprintf(chatOut,Obj_chatmessage::MSGLENGTH, "%s :%s", players[0].player_name, mChatString) )
    {
        // Prevent GCC truncation warning
    }
	strcpy(mChatString, chatOut);
	// activate send flag
	mSendMsg = true;
}

int Game_client::draw(Mutable_raw_surface *target) {
	// render player viewports
	for(int i=0; i<nplayers; ++i) {
		Obj_player_data *player_data	= (Obj_player_data *)clientworld.get_object(players[i].owner,ot_player_data);
		if (!player_data) {
			logmsg(lt_warning,"Unable to find player_data");
			return -1;
		}

	if ( smallfont ) {
		if ( !(players [ 0 ].frag_info.is_font()) ) {
			for ( int i = 0; i < nplayers; ++i ) {
				players[i].frag_info.set_font ( smallfont );
			}
		}
		
		if(! mChatMessage.is_font() ) {
			mChatMessage.set_font( smallfont );
		}
		for(int i=0; i<Obj_chatmessage::MSGNUM; i++) {
			if(! gameMessages[i].is_font() ) {
				gameMessages[i].set_font(smallfont);
			}
		}
	}

	// figure out if we are to put the bars at the top or the bottom:
	Obj_player_avatar *avatar = player_data->get_player_avatar();
	if (avatar) {
		float x,y;
		avatar->getpos(x,y);
		//int switch_margin = players[i].viewport->get_ysize()/2;
		int switch_margin = 50;

		// default to top
		bars_at_top = true;
		if (y < switch_margin)
			bars_at_top = false;
	}

	// Figure out the pixel position of the bars
	int bars_ypos = 5;
	if(! bars_at_top ) bars_ypos = players[i].viewport->get_ysize()-(7+5);

	//Update the health bar:
	if (avatar) {
		float healthDisp = (avatar->gethealth()*1.0)/MAX_HEALTH;
		if( healthDisp < 0.0 ) healthDisp = 0.0; //NT when the players dead this can happen
		players[i].health_bar.setpos(healthDisp);
	}
	players[i].health_bar.ypos = bars_ypos;

	//Update the weapon bars:
	if (player_data) {
		// bar_width is calculated as follows: each bar has 5 pixels of
		// padding on each side, and the bars are packed into the right-hand
		// 3/4 of the screen.
		float spacing   = 10.0 * players[i].width/640;
		float bar_start = 120.0 * players[i].width/640;
		float bar_width = 5*(10-WEAPON_SLOTS)*players[i].width/640;
		for (int s=0;s<WEAPON_SLOTS;s++) {
			// FIXME: use separate controls for each player.
			float this_bar_width=players[i].controls.get_weapon()==s?bar_width*2:bar_width;
			players[i].weapon_bar[s].setgeom((int)bar_start,bars_ypos,ROUND(this_bar_width),7);

			Obj_weapon *weapon = player_data->get_weapon(s);
			if (weapon) {

				//Figure out the color of the bar
				int32 newcolor = weapon->canFire()?PIXELA(0,255,0,6):PIXELA(255,0,0,6);
				if (players[i].weapon_bar[s].bar_color != newcolor) {
					players[i].weapon_bar[s].bar_color = newcolor;
					players[i].weapon_bar[s].render();
				}
				
				float readyness = weapon->getReadyness();
				players[i].weapon_bar[s].setpos(readyness);
			} else {
				players[i].weapon_bar[s].setpos(1);
				logmsg(lt_warning,"Unable to find the weapon for slot %i",s);
			}

			bar_start += this_bar_width+spacing;
		}
	} // player info
		

	//figure out what to view
	int vx = 0;
	int vy = 0;
	clientworld.get_player_pos(players[i].viewport->getowner(),vx,vy);
	players[i].viewport->addsample(vx,vy,last_tick_duration);

	//logmsg(lt_debug, " bla %d  %d %d",i, players[i].viewport->xpos,players[i].viewport->ypos );
	clientworld.draw(players[i].viewport);

	// create world shadows (if active)
	if (shadow_visible)
		players[i].viewport->drop_shadow();

	//draw the player specific UI bits:
	if (player_data) {
		// Now render texts
		Obj_scoreboard *sb = clientworld.getscoreboard ();
		if(sb) {
			int id = get_player_id(i);
			Obj_scoreboard::Score sc = sb -> getscore ( id );
			if ( players[i].frag_info.is_font() ) {
				char str [ 100 ];
				int textYPos = 5 + 8;
				if ( !bars_at_top ) textYPos = players[i].viewport->get_ysize() - ( 7 + 5 + 3 + 10);
				if ( id != -1 ) {
					//sprintf ( str , "Frags:%d, Kills:%d, Deaths:%d" , sc.frags , sc.kills , sc.deaths );
					sprintf ( str , "Frags: %d  -  Rank: %d", sc.frags, sb->getRank( id ) );
					players[i].frag_info.setgeom ( 5+1 , textYPos+1 , str , 0,0,0 );
					players[i].frag_info.render( );
					players[i].frag_info.draw(players[i].viewport); 
					players[i].frag_info.setgeom ( 5 , textYPos , str , 255 , 255 , 255 );
					players[i].frag_info.render( );
					players[i].frag_info.draw(players[i].viewport); 
				}
			}
		} // valid scoreboard? 

		//NT display chatting, only for first player
		if( ( mChatMessage.is_font() ) && (chat_visible) && (i==0) ) {
			int textYPos = 5 + 8 +12;
			if(! bars_at_top ) textYPos = 5;

			char chatOut[300];
			sprintf(chatOut, "%s :%s", players[i].player_name, mChatString);
			players[i].controls.clear_flag( 0xFFFFFFFF );
			mChatMessage.setgeom ( 5+1 , textYPos+1 , chatOut , 0,0,0 );
			mChatMessage.render( );
			mChatMessage.draw(players[i].viewport); //NT draw chat 
			mChatMessage.setgeom ( 5 , textYPos , chatOut , 
			                       GETRED(players[i].player_color),
			                       GETGREEN(players[i].player_color),
			                       GETBLUE(players[i].player_color) );
			mChatMessage.render( );
			mChatMessage.draw(players[i].viewport); //NT draw chat 
		} // font there?
	}

	// draw status info (health, weapons)
	players[i].health_bar.draw(players[i].viewport);
	for (int s=0;s<WEAPON_SLOTS;s++) {
		players[i].weapon_bar[s].draw(players[i].viewport);
	}

	if (smooth) {
		players[i].viewport->draw_copy_stretch( target,
		                                        0, 0,
		                                        players[i].viewport->get_xsize(), players[i].viewport->get_ysize(),
		                                        players[i].xpos+xloc, players[i].ypos+yloc,
		                                        players[i].width, players[i].height,
		                                        Raw_surface::NOBLEND );
	} else {
		players[i].viewport->draw_copy_stretch( target,
		                                        0,0,
		                                        players[i].viewport->get_xsize(), players[i].viewport->get_ysize(),
		                                        players[i].xpos+xloc, players[i].ypos+yloc,
		                                        view_quality );
		}
	} // nplayers
	return 0;
}

// FIXME: this belongs on the server, doesn't it?  (think hacked client)
void Game_client::controls_tick() {
	for(int i=0; i<nplayers; ++i) {
		if (players[i].elevate_down)
			players[i].controls.set_elevation(players[i].controls.get_elevation()-0.1);
		if (players[i].elevate_up)
			players[i].controls.set_elevation(players[i].controls.get_elevation()+0.1);
	}
	controls_ticked = true;
}

void Game_client::set_quit() {
	done = true;
}

void Game_client::set_event_handler(Event_target* event_handler_) {
	event_handler = event_handler_;
}

Controls& Game_client::get_controls(int player) {
	return players[player].controls;
}

int Game_client::get_numplayers() {
	return clientworld.get_player_count();
}

Obj_player_avatar* Game_client::get_player(int player) {
	Obj_player_avatar *avatar = clientworld.get_player_number( player );
	return avatar;
}

int Game_client::get_ownid(int player) {
	Obj_player_data *player_data = (Obj_player_data *)clientworld.get_object(players[player].owner,ot_player_data);
	return player_data->getavatarid();
}

int Game_client::get_weapon(int player) {
	Controls &controls=players[player].controls;
	return controls.get_weapon();
}

bool Game_client::handle_avatar_actions(Nil_inputevent* event, int player) {
	int function = event->cmdfunction & CMDFUNCTION::FUNCTION_MASK;
	bool pressed = event->pressed;
	Controls &controls = players[player].controls;
	switch (function) {
		case (CMDFUNCTION::UP) :{
			if (pressed) {
				players[player].elevate_up = true;
				controls_ticked = false;
			} else {
				if (!controls_ticked) controls_tick();
				players[player].elevate_up = false;
			}
			return true;
		}
		case (CMDFUNCTION::DOWN) :{
			if (pressed) {
				players[player].elevate_down = true;
				controls_ticked = false;
			} else {
				if (!controls_ticked) controls_tick();
				players[player].elevate_down = false;
			}
			return true;
		}
		case (CMDFUNCTION::LEFT) :{
			controls.clear_flag(Controls::cf_direction_east);
			controls.set_flag(Controls::cf_walk, pressed);
			return true;
		}
		case (CMDFUNCTION::RIGHT) :{
			controls.set_flag(Controls::cf_direction_east);
			controls.set_flag(Controls::cf_walk, pressed);
			return true;
		}
		case (CMDFUNCTION::JUMP) :{
			controls.set_flag(Controls::cf_jump, pressed);
			return true;
		}
		case (CMDFUNCTION::HOOK_FIRE) :{
			controls.set_flag(Controls::cf_hook_fire, pressed);
			return true;
		}
		case (CMDFUNCTION::HOOK_RELEASE) :{
			controls.set_flag(Controls::cf_hook_release, pressed);
			return true;
		}
		case (CMDFUNCTION::HOOK_IN) :{
			controls.set_flag(Controls::cf_hook_in, pressed);
			return true;
		}
		case (CMDFUNCTION::HOOK_OUT) :{
			controls.set_flag(Controls::cf_hook_out, pressed);
			return true;
		}
		case (CMDFUNCTION::FIRE) :{
			controls.set_flag(Controls::cf_shoot, pressed);
			return true;
		}
		case (CMDFUNCTION::DIG) :{
			controls.set_flag(Controls::cf_dig, pressed);
			return true;
		}
		case (CMDFUNCTION::JETPACK) :{
			controls.set_flag(Controls::cf_jetpack, pressed);
			return true;
		}
		case (CMDFUNCTION::WEAPON1) :{
			if (pressed)
				controls.set_weapon(0);
			return true;
		}
		case (CMDFUNCTION::WEAPON2) :{
			if (pressed)
				controls.set_weapon(1);
			return true;
		}
		case (CMDFUNCTION::WEAPON3) :{
			if (pressed)
				controls.set_weapon(2);
			return true;
		}
		case (CMDFUNCTION::WEAPON4) :{
			if (pressed)
				controls.set_weapon(3);
			return true;
		}
		case (CMDFUNCTION::WEAPON5) :{
			if (pressed)
				controls.set_weapon(4);
			return true;
		}
		case (CMDFUNCTION::NEXTWEAPON) :{
			if(pressed)
				controls.set_weapon((controls.get_weapon()+1)%5);
			return true;
		}
		case (CMDFUNCTION::PREVWEAPON) :{
			if(pressed) { // shouldn't (-1)%5==4?
				if(controls.get_weapon()<=0)
					controls.set_weapon(4);
				else
					controls.set_weapon(controls.get_weapon()-1);
			}
			return true;
		}
		default: {
			if(event->sdl_inputevent->type == SDL_KEYUP || event->sdl_inputevent->type == SDL_KEYDOWN) {
				SDLKey sdlkey = ((SDL_KeyboardEvent *) event->sdl_inputevent)->keysym.sym;
				logmsg(lt_error, "Unhandled key: 0x%x (%s) -> 0x%x",sdlkey,SDL_GetKeyName(sdlkey),function);
			} else {
				logmsg(lt_warning, "Mouse or Joystick button not handled");
			}
			return false;
		}
	}
}

bool Game_client::handle(Nil_inputevent *event) {
	if (!event) {
		logmsg(lt_error, "No event given, cannot handle it");
		return false;
	}
	switch (event->sdl_inputevent->type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			// we do not access any member of Event_keyboard_key, only from Nil_inputevent, so casting is suitable
			return handle_keyboard_key((Event_keyboard_key *)event);

		case SDL_MOUSEMOTION:
			return handle_mouse_motion(new Event_mouse_motion(&event->sdl_inputevent->motion));

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			return handle_mouse_key((Event_mouse_key *)event);
		
		case SDL_JOYAXISMOTION:
			return handle_joystick_motion(new Event_joystick_motion(&event->sdl_inputevent->jaxis));

		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			return handle_joystick_key((Event_joystick_key *)event);

		default : return false;
	}
	return false;
}

bool Game_client::handle_keyboard_key(Event_keyboard_key *key_event) {
	int target = key_event->cmdfunction & CMDFUNCTION::TARGET_MASK;
	switch (target) {
		case (CMDFUNCTION::PLAYER1):
			handle_avatar_actions(key_event, 0);
			break;
		case (CMDFUNCTION::PLAYER2): 
			handle_avatar_actions(key_event, 1);
			break;
		case (CMDFUNCTION::HOTKEY): {
			// up/down events
			int function = key_event->cmdfunction & CMDFUNCTION::FUNCTION_MASK;
			switch (function) {
				case (CMDFUNCTION::FRAGTABLE): {
					if (key_event->pressed)
						scoreboard_visible = true;
					else
						scoreboard_visible = false;
					break;
				}
			}
			// only pressed ones after here...
			if (!key_event->pressed)
				break;
			switch (function) {
				case (CMDFUNCTION::MENU): {
					menu_visible = true;
					break;
				}
				case (CMDFUNCTION::CHAT):
				{
					chat_visible = true;
					sprintf( getChatString() , " " );
					break;
				}
				case CMDFUNCTION::TOGGLE_CONSOLE: {
					console_visible = !console_visible;
					if (console_visible)
						logmsg(lt_debug, "console on");
					else
						logmsg(lt_debug, "console off");
					break;
				}
				case CMDFUNCTION::SCREEN_SHOOT: {
					screen_shoot = true;
					break;
				}
				case CMDFUNCTION::TOGGLE_FRAMERATE: {
					fps_visible = !fps_visible;
					if (fps_visible)
						logmsg(lt_debug,"fps on");
					else
						logmsg(lt_debug,"fps off");
					break;
				}
				case CMDFUNCTION::TOGGLE_TRAFFIC: {
					traffic_visible = !traffic_visible;
					if (traffic_visible)
						logmsg(lt_debug,"plotting of traffic on (100%% is 128kb/s)");
					else
						logmsg(lt_debug,"plotting of traffic off");
					break;
				}
				case CMDFUNCTION::TOGGLE_SHADOW: {
					shadow_visible = !shadow_visible;
					if (shadow_visible)
						logmsg(lt_message,"Dropshadow has been turned on");
					else
						logmsg(lt_message,"Dropshadow has been turned off");
					break;
				}
				case CMDFUNCTION::TOGGLE_SMOOTH: {
					smooth = !smooth;
					if (smooth)
						logmsg(lt_message,"Smoothing has been turned on");
					else
						logmsg(lt_message,"Smoothing has been turned off");
					break;
				}
				case CMDFUNCTION::TOGGLE_LOGO: {
					logo_visible = !logo_visible;
					if (logo_visible)
						logmsg(lt_debug,"The nifty logo is back.");
					else
						logmsg(lt_debug,"The nifty logo now gone, why oh why?");
					break;
				}
				case CMDFUNCTION::TOGGLE_DEATHCAM: {
					death_cam_active = !death_cam_active;
					if (death_cam_active)
						logmsg(lt_debug,"The DeathCam(tm) is now active.");
					else
						logmsg(lt_debug,"The DeathCam(tm) is now off.");
					break;
				}
			} //switch (function) 
			break;
		} //case (CMDFUNCTION::HOTKEY): */
		default:
		{
			logmsg(lt_warning, "Key with invalid target: %x", target);
			return false;
		}
	} //switch (target) 
	return true;
}

bool Game_client::handle_mouse_key(Event_mouse_key *mouse_key_event) {
	int target = mouse_key_event->cmdfunction & CMDFUNCTION::TARGET_MASK;
	switch (target) {
		case (CMDFUNCTION::PLAYER1):
			return handle_avatar_actions(mouse_key_event, 0);
		case (CMDFUNCTION::PLAYER2): 
			return handle_avatar_actions(mouse_key_event, 1);
		default:
		{
			logmsg(lt_warning, "Button with invalid target: %x", target);
			return false;
		}
	}
}

bool Game_client::handle_mouse_motion(Event_mouse_motion *mouse_motion_event) {
	//FIXME: it's not really usable
	//Uint16 xpos = mouse_motion_event->mouse_motion->x;
	//Uint16 ypos = mouse_motion_event->mouse_motion->y;
	Sint16 xrel = mouse_motion_event->mouse_motion->xrel;
	Sint16 yrel = mouse_motion_event->mouse_motion->yrel;
	//logmsg(lt_debug, "Mouse at (%i,%i) (%i,%i)", xpos, ypos, xrel, yrel);
	Controls &controls=players[0].controls;

	if (xrel <=1 && xrel >= -1)
		controls.clear_flag(Controls::cf_walk);
	else
		controls.set_flag(Controls::cf_walk);
	if (xrel > 1)
		controls.set_flag(Controls::cf_direction_east);
	else if (xrel < -1)
		controls.clear_flag(Controls::cf_direction_east);

	if (yrel <=1 && yrel >= -1) {
		players[0].elevate_up   = false;
		players[0].elevate_down = false;
		if(!controls_ticked) controls_tick();
	} else if (yrel > 1) {
		players[0].elevate_down = true;
		players[0].elevate_up   = false;
		controls_ticked         = false;
	} else if (yrel < 1) {
		players[0].elevate_down = false;
		players[0].elevate_up   = true;
		controls_ticked = false;
	}
    delete mouse_motion_event;
	return true;
}

bool Game_client::handle_joystick_key(Event_joystick_key *joystick_key_event) {
	int target = joystick_key_event->cmdfunction & CMDFUNCTION::TARGET_MASK;
	//Want to know which button was pressed? Uncomment the following
	//Event_joystick_key * button_logger = new Event_joystick_key(&joystick_key_event->sdl_inputevent->jbutton);
	//logmsg(lt_debug, "Button %i was pressed/released", button_logger->joystick_key->button);
	switch (target) {
		case (CMDFUNCTION::PLAYER1):
			return handle_avatar_actions(joystick_key_event, 0);
		case (CMDFUNCTION::PLAYER2): 
			return handle_avatar_actions(joystick_key_event, 1);
		default:
		{
			logmsg(lt_warning, "Button with invalid target: %x", target);
			return false;
		}
	}
	/*Uint8 joystick = joystick_key_event->joystick_key->which;
	Uint8 button = joystick_key_event->joystick_key->button;
	if (joystick_key_event->joystick_key->type == SDL_JOYBUTTONDOWN) {
		//SDL_PRESSED
		logmsg(lt_debug, "Joystick(%i) button(%i) pressed", joystick, button);
		handle_avatar_actions(joystick_key_event, 0);
	} else if (joystick_key_event->joystick_key->type == SDL_JOYBUTTONUP) {
		//SDL_RELEASED
		logmsg(lt_debug, "Joystick button released");
	} else {
		logmsg(lt_error, "You joystick button is wheter raised nor lowered ????");
	}
	return false;*/
}

bool Game_client::handle_joystick_motion(Event_joystick_motion *joystick_motion_event) {
	Uint8 joystick = joystick_motion_event->joystick_motion->which;
	Uint8 axis = joystick_motion_event->joystick_motion->axis;
	Sint16 value = joystick_motion_event->joystick_motion->value;
	// For now, just treat it like a key depending on <,=,>0. (with some tolerance)
	value /= 10; // Hopefully should drown out jitter for analogs.
	for(int player=0; player<nplayers; ++player) {
		if(players[player].joystick == joystick) {
			Controls &controls=players[player].controls;
			if (axis == 0 || axis == 2 || axis == 4) {
				// X axis: axis 0
				if (value == 0)
					controls.clear_flag(Controls::cf_walk);
				else
					controls.set_flag(Controls::cf_walk);
				if (value > 0)
					controls.set_flag(Controls::cf_direction_east);
				else if (value < 0)
					controls.clear_flag(Controls::cf_direction_east);
			} else if (axis == 1 || axis == 3 || axis == 5) {
				// Y axis
				//FIXME: When do I need to do a controls_tick()?
				if (value == 0) {
					players[player].elevate_up   = false;
					players[player].elevate_down = false;
					if(!controls_ticked) controls_tick();
				}
				else if (value > 0) {
					players[player].elevate_down = true;
					players[player].elevate_up   = false;
					controls_ticked              = false;
				}
				else if (value < 0) {
					players[player].elevate_down = false;
					players[player].elevate_up   = true;
					controls_ticked = false;
				}
			} else {
				logmsg(lt_error, "Wheter X nor Y axis(%i) has been moved", axis);
				return false;
			}
		}
	}
	return true;
}
