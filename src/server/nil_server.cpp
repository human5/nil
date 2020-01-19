/***************************************************************************
  nil_server.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-11-14
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-05-17
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-09-22
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Use load_files_standard().

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Use Console::set_source instead of setmsgsource.

  last changed         : 2005-01-23
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : fix memleak reported by valgrind

  copyright            : (C) 1999 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#include "nil_server.h"

#ifndef _WIN32
void mySigHup(int sig) {
	if(!request_quit) {
		logmsg(lt_message,"Caught signal %d - shutting server down...", sig);
		request_quit = true;
	}
	return;
}
#endif

void run_server(Configuration &config) {
	Weapon_style weapon_style;
	int gameweapons[WEAPON_SLOTS];
	master_console.set_source("S");
	logmsg(lt_message,"Running server");

	Tcp_server tcp_server;
	
#ifndef _WIN32
	// unix, ignore sig pipe signal
	signal(SIGPIPE, SIG_IGN);

	// handle exit signals
	signal(SIGHUP, mySigHup);
	signal(SIGINT, mySigHup);
	signal(SIGTERM, mySigHup);
#endif

	tcp_server.set_fudge(
		config.get_option_int("net_normal_backlog"),
		config.get_option_int("net_abnormal_backlog"),
		config.get_option_int("net_max_wait_time")
	);
	
	tcp_server.bind_and_listen(config.get_option_int("server_port"),config.get_option_int("net_multithread"));

	Loader loader;
	load_files_standard(loader, config.get_option("data"));

	Soundsystem soundsystem(&loader, NULL, &config);

	// Parse the weapon options (partly)
	if(!strcasecmp(config.get_option("weapon_style"), "randidentical")) {
		weapon_style=ws_randidentical;
		logmsg(lt_debug, "Using identically random weapon slots");
		for(int i=0; i<WEAPON_SLOTS; ++i) {
			//FIXME: prevent weapon duplicates
			gameweapons[i]=rnd(ot_weapon_first, ot_weapon_last);
		}
	} else if(!strcasecmp(config.get_option("weapon_style"), "rand")) {
		weapon_style=ws_rand;
		logmsg(lt_debug, "Using random weapon slots");
	} else if(!strcasecmp(config.get_option("weapon_style"), "oldnil")) {
		weapon_style=ws_old_nil;
		logmsg(lt_debug, "Using old nil weapon slot setup");
		for(int i=0; i<WEAPON_SLOTS; ++i)
			gameweapons[i]=weaponinit_oldnil[i];
	} else {
		weapon_style=ws_fixed;
		if(strcasecmp(config.get_option("weapon_style"), "fixed"))
			logmsg(lt_warning, "Unknown weapon style %s, reverting to fixed", config.get_option("weapon_style"));
		else
			logmsg(lt_debug, "Using fixed weapon slots");
		for(int i=0; i<WEAPON_SLOTS; ++i)
			gameweapons[i]=weaponinit_default[i];
		for(int i=0; i<WEAPON_SLOTS; ++i) {
			char buf[64]; // way bigger than we need.
			snprintf(buf, 64, "weapon%i", i+1);
			buf[63]='\0'; // better safe than sorry.
			gameweapons[i]=ot_from_name(config.get_option(buf));
			if(gameweapons[i]==ot_error || gameweapons[i]<ot_weapon_first || gameweapons[i]>ot_weapon_last) {
				logmsg(lt_warning, "Bad weapon name %s for slot %i, reverting to %s (uwe debug -> weapon[%i]=%i, error=%i, first=%i, last=%i)",
				config.get_option(buf), i, OBJECT_NAMES[weaponinit_default[i]], gameweapons[i], ot_error, ot_weapon_first, ot_weapon_last);
				gameweapons[i]=weaponinit_default[i];
			}
		}
	}

	//set up the server:
	Serverworld serverworld(&loader, &soundsystem, config.get_option_int("always_reload"), weapon_style, gameweapons);
	tcp_server.serverworld = &serverworld;

	//! create settings
	Obj_settings *gameSettings = (Obj_settings *)serverworld.newobject(ot_settings);
	serverworld.setServerSettings( gameSettings );
	if(!gameSettings) {
		logmsg(lt_error, "Fatal: unable to create settings object");
		exit(1);
	}
	gameSettings->setGameMode( config.get_option("game_mode") );
	gameSettings->setArmageddonTime( config.get_option_int("armageddon") );

	// init world with given size
	Config_xy world_size = config.get_option_xy("world_size");
	char *map_name = strdup ( config.get_option ( "map" ) );
	if (strcmp(map_name, "-=random=-") == 0) {
		// create random map
		serverworld.createmap(world_size.x,world_size.y,config.get_option_int("randomseed"),
		config.get_option_int("map_stuff") ); //NT
	} else {
		// load LIERO map from file
		serverworld.createmap (map_name);
	}
	free(map_name);
	serverworld.save_original_map();
	logmsg(lt_debug, "map created");
	
	int traffic = 0;
	int frames = 0;
	bool done = false;

	unsigned int cur_time = time_ms();
	float gametime = 0.0;
	float stat_start = 0.0;
	unsigned int last_time = cur_time;
	int recycle_counter = 0;

	// time of quit signal, shutdown proceeds in the following way: request_quit
	// is somehow set to true, notification is sent to clients, delay TIME_QUITDELAY ms, quit message
	// is sent to clients, these leave the game, when all are gone (or timeout occurs), 
	// the server exits
	unsigned int quitStartTime = 0;

	// when leaving, was the quit msg sent to client?
	// if it was sent, wait for all clients to exit (or a timeout TODO!)
	bool quitMessageSent = false;

	// send a textual message to the clients when leaving
	bool quitNotificationSent = false;

	//int debugCount = 0; // DEBUG D4

	unsigned int time_to_sleep;

	while (!done) {
		//debugCount++; // DEBUG D4
		//if(debugCount==300) request_quit = true; // DEBUG D4

		{ //find the new time
			last_time = cur_time;
			cur_time = time_ms();
			unsigned int delta_time = time_elapsed(last_time,cur_time);
		
			gametime += delta_time/1000.0;
		}

		if ((request_quit) && (!quitNotificationSent)) {
			// send quit notification message
			char quitmsg[Obj_chatmessage::MSGLENGTH];
			snprintf(quitmsg, Obj_chatmessage::MSGLENGTH, "System: Server is shutting down... Goodbye!");
			Obj_chatmessage *sendmsg = dynamic_cast<Obj_chatmessage *>(serverworld.newobject(ot_chatmessage));
			sendmsg->initmessage( quitmsg, Obj_chatmessage::SYSMSGCOL );
			sendmsg->bedone();
			sendmsg->setdirty();
			quitNotificationSent = true;
			quitStartTime = time_ms();
			logmsg(lt_debug,"Shutdown notification send to players");
		}

		if ((request_quit) && (quitMessageSent) && (tcp_server.get_connections_inuse() == 0) ){
			logmsg(lt_message, "All players left, we're done");
			done = true;
		}
	
		if((request_quit)&&(quitMessageSent)&&(tcp_server.get_connections_inuse() > 0) && ((time_ms() - quitStartTime) > (TIME_QUITDELAY+TIME_FORCESHUTDOWN)) ){
			logmsg(lt_message,"Still %d player(s) left but time's up, leaving anyway", tcp_server.get_connections_inuse() );
			done = true;
		}

		//logmsg(lt_debug,"%d   reqq %d, qum %d, no rem %d ",debugCount, request_quit, quitMessageSent, tcp_server.getConnectionsInuse() ); //DEBUG D4

		//see if it is time to recycle serializers
		if (recycle_counter-- <= 0) {
			tcp_server.recycle_serializers();
			//do a recycle run every 5 frames (maybe this should be done every frame, experiment)
			recycle_counter = 5;
		}

		// update game state
		serverworld.settime(gametime);
		if( (request_quit) && (!quitMessageSent)&& (quitNotificationSent) && ((time_ms() - quitStartTime)>TIME_QUITDELAY) ) {
			// send quit message to all left clients, when exit was requested and the delay for notification is over
			serverworld.server_think(&tcp_server.update_queue, true);
			quitMessageSent = true;
		} else {
			// normal update
			serverworld.server_think(&tcp_server.update_queue, false);
		}

		//update clients, update their controls and let new clients join.
		tcp_server.poll();

		//do some stats:
		traffic += serverworld.last_reader_length;
		frames++;
		//print stats:
		if (frames >= 150) {
			logmsg(lt_message,"Bytes/sec: %.1f, fps: %.2f, dt: %.2f, time:%.2f",
				traffic/(gametime-stat_start),
				frames/(gametime-stat_start),
				gametime-stat_start,
				gametime
			);
			traffic = 0;
			frames = 0;
			stat_start = gametime;
		}

		//delay one tick
		time_to_sleep = 40;
		if (time_elapsed(cur_time,time_ms()) > time_to_sleep) {
			logmsg(lt_warning, "Frame to longer than %d ms", time_to_sleep);
		}
		while (time_elapsed(cur_time,time_ms()) < time_to_sleep)
			usleep(1000);
	}

	// TODO: wait for all other threads (else these will segfault when trying to access the world)
	logmsg(lt_debug, "Main thread complete...");
 	return;
}
