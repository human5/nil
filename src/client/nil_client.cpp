/***************************************************************************
  nil_client.cpp  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-20
  by                   : Flemming Frandsen (dion@swamp.dk)

  expansion            : 2003-06-17
  by                   : Nils Thuerey (n_t@gmx.de)

  last changed         : 2004-05-16 
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-08
  by                   : Phil Howlett (phowlett@bigpond.net.au)
  description          : Changed SDL_UpdateRect to SDL_Flip so that
                         double buffering works correctly.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-09-22
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Use load_files_standard().

  last changed         : 2004-10-28
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Use Console::set_source instead of setmsgsource.
  
  last changed         : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : add joystick support, move out event handling

  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : cleanup some includes

  last changed         : 2005-01-17
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : move out SDL_mixer initialization

  copyright            : (C) 2000 by Flemming Frandsen (dion@swamp.dk)
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#include "client/nil_client.h"

#include "client/fonts/font_instance.h"
#include "client/graphics/icon_graph.h"
#include "client/console/nil_console.h"
#include "client/menu/menu.h"
#include "client/chat.h"
#include "client/game_client.h"
#include "common/configuration/configuration.h"

//! colored blood
bool colored_blood;

void perform_benchmark(SDL_Surface *screen , Config_xy *screen_size, Loader *loader) {
	Font_instance *smallfont;
    // XXX Unused Variable
	//Font_instance *bigfont;
	smallfont = loader->fonts.getfont("arial", 10);
	//bigfont = loader->fonts.getfont("arial", 24);
	logmsg(lt_debug, "bench: ---- start...");
	Mutable_raw_surface tmp; 

	if (SDL_MUSTLOCK(screen)) {
		if ( SDL_LockSurface(screen) < 0 )
			logmsg(lt_error, "Unable to lock SDL surface");		
	}
	tmp.attach(screen_size->x, screen_size->y, (int32 *)screen->pixels);

	// now perform drawing
	Graphic *logo=loader->graphics.getgraphic("gfx/logos/nil_small.rgbf");
	unsigned int startTime, stopTime, counter;

	// draw alpha test
	startTime = time_ms();
	counter = 10000;
	for(unsigned int l=0; l<counter; l++) {
		logo->draw_alpha(&tmp, tmp.get_xsize()-logo->get_xsize()-2, 15);
	}
	stopTime = time_ms();
	logmsg(lt_debug, "bench: draw_alpha duration %d, per call %f", (stopTime-startTime), ((float)(stopTime-startTime)/(float)counter) );

	// draw copy
	startTime = time_ms();
	counter = 50000;
	for(unsigned int l=0;l<counter;l++) {
		logo->draw_copy(&tmp,tmp.get_xsize()-logo->get_xsize()-2, 15);
	}
	stopTime = time_ms();
	logmsg(lt_debug, "bench: draw_copy duration %d, per call %f", (stopTime-startTime), ((float)(stopTime-startTime)/(float)counter) );		

	// text rendering
	char str[512];
	sprintf(str, "test text bla long text");

	startTime = time_ms();
	counter = 20000;
	for(unsigned int l=0; l<counter; l++) {
		smallfont->put_text(10, 10, str, &tmp, true, 255, 255, 255);
	}
	stopTime = time_ms();
	logmsg(lt_debug, "bench: put_text duration %d, per call %f", (stopTime-startTime), ((float)(stopTime-startTime)/(float)counter) );		

	sprintf(str, "short");
	startTime = time_ms();
	counter = 20000;
	for(unsigned int l=0; l<counter; l++) {
		smallfont->put_text(10,10,str,&tmp,true,255,255,255);
	}
	stopTime = time_ms();
	logmsg(lt_debug,"bench: put_text duration %d, per call %f", (stopTime-startTime), ((float)(stopTime-startTime)/(float)counter) );		

	if ( SDL_MUSTLOCK(screen) ) {
		SDL_UnlockSurface(screen);
	}

	logmsg(lt_debug, "bench: ---- stop...");
	// benchmarking done ...
}

void run_client(Configuration &config) {
	//! no. of joysticks found
	int njoys;
	//! the sdl joysticks
	SDL_Joystick **joys = NULL;

	colored_blood = config.get_option_int("coloredblood");

	// init client
	master_console.set_source("C");
	logmsg(lt_message, "Running client");

	logmsg(lt_debug, "Going to add: %s as sources",config.get_option("data"));
	Loader loader;
	load_files_standard(loader, config.get_option("data"));

	// init SDL
	int have_sound = config.get_option_int("sound");
	int sdl_flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
	if (have_sound != 0){ 
		sdl_flags |= SDL_INIT_AUDIO;
		logmsg(lt_debug, "Sound is enabled (sound = %d)", have_sound);
	} else {
		logmsg(lt_debug, "Sound is disabled (sound = %d)", have_sound);
	}
	sdl_flags |= SDL_INIT_NOPARACHUTE;

	if (SDL_Init(sdl_flags) < 0) {
		logmsg(lt_fatal, "Unable to initialize SDL: %i", SDL_GetError());
		return;
	} else {
		const SDL_version *sdl_version = SDL_Linked_Version();
		logmsg(lt_message, "SDL %d.%d.%d initialized", sdl_version->major, sdl_version->minor, sdl_version->patch);
		atexit(SDL_Quit);
	}

	if (TTF_Init() < 0) {
		logmsg(lt_fatal, "Couldn't initialize SDL_ttf: %s", SDL_GetError());
		return;
	} else {
		const SDL_version *sdl_version = TTF_Linked_Version();
		logmsg(lt_message, "SDL_ttf %d.%d.%d initialized", sdl_version->major, sdl_version->minor, sdl_version->patch);
		atexit(TTF_Quit);
	}

	Config_xy screen_size = config.get_option_xy("screen_size");

	// Adding in the SDL_DOUBLEBUF flag so that we can use the doublebuffer :)
	// - Phil Howlett 8th Sept 2004
	int sdl_video_flags = SDL_HWSURFACE | SDL_HWACCEL | SDL_DOUBLEBUF;
	if (config.get_option_int("fullscreen"))
		sdl_video_flags |= SDL_FULLSCREEN;
	SDL_Surface *screen = SDL_SetVideoMode(screen_size.x, screen_size.y, 32, sdl_video_flags);
	if (!screen) {
		logmsg(lt_fatal, "Couldn't set %ix%ix32 video mode: %s", screen_size.x, screen_size.y, SDL_GetError());
		return;
	} else {
		logmsg(lt_debug, "Using video mode: %ix%ix32", screen_size.x, screen_size.y);
	}

	SDL_ShowCursor(SDL_DISABLE);

	// display welcome graphic, and enable console
	Graphic *logo_big = loader.graphics.getgraphic("gfx/logos/nil.rgbf");
	//FIXME: replace loader.fonts.getfont("arial", 10) by smallfont?
	Nil_console console(screen, screen_size, logo_big, loader.fonts.getfont("arial", 10));
	console.show_console(true);
	console.draw(screen);
	logmsg(lt_message, "Console initialized");

	njoys = SDL_NumJoysticks();
	if (njoys > 0) {
		logmsg(lt_debug, "Found %i joysticks, opening them:", njoys);
		joys = new SDL_Joystick *[njoys];
		for(int i=0; i<njoys; ++i) {
			logmsg(lt_debug, "  opening %s", SDL_JoystickName(i));
			joys[i] = SDL_JoystickOpen(i);
		}
	} else {
		logmsg(lt_debug, "No joystick found.");
	}

	Evtmapper evtmapper;
	evtmapper.init(&config);

	if (config.get_option_int("benchmark")) {
		logmsg(lt_debug, "Performing benchmark");
		perform_benchmark( screen, &screen_size, &loader );
		logmsg(lt_debug, "Done performing benchmark");
	}
	
	// initialize the single player client:
	// strangely, the mac compiler doesnt like the creation of the Game_client object
	// as an instance (!?!), so it has be created as a pointer...
	Game_client *game_client = new Game_client();
	
	Soundsystem soundsystem(&loader, game_client, &config);
	game_client->init(&loader, &soundsystem, &config);

	int join_error = game_client->join_game(config.get_option("server"),
	                                        config.get_option_int("server_port"),
	                                        config.get_option_int("local_players"),
	                                        config.get_option("player_name"),
	                                        config.get_option_int("player_color"),
	                                        config.get_option_int("team"));

	if (join_error) {
		logmsg(lt_fatal, "Unable to join the game (%i), giving up and going home", join_error);
		exit(-1);
	}

	int view_quality = config.get_option_int("view_quality");
	game_client->setgeom(0, 0, screen_size.x, screen_size.y, view_quality);
	//NT set gorelevel from config file
	game_client->clientworld.set_gorelevel( config.get_option_int("gorelevel") );
	//for stats:
	game_client->smallfont = loader.fonts.getfont("arial", 10);
	game_client->bigfont = loader.fonts.getfont("arial", 24);
	//Obj_player_data *player_data = NULL;

	Icon_graph traffic_graph;
	traffic_graph.init(10, 20, 50, 20, PIXELA(128, 128, 0, 6), PIXELA(255, 0, 0, 8));
	float traffic_trend = 0;
	float last_traffic_sample = 0;
	float traffic_sum = 0;
	Graphic *logo = loader.graphics.getgraphic("gfx/logos/nil_small.rgbf");
	int screen_shot_number = 0;
	int anon_anounce = 0;
	Menu* my_menu = new Menu(game_client->bigfont, game_client);
	Chat* my_chat = new Chat(game_client, my_menu);
	console.show_console(false);
	
	// everything set, start main game loop
	game_client->set_event_handler(game_client);
	while (!game_client->done) {
		if (request_quit)
			game_client->done = true;

		if (anon_anounce-- <= 0) {
			anon_anounce = 30;
			logmsg(lt_debug, "Anon objects: %i, time:%.2f", game_client->clientworld.get_anon_objects_inuse(), game_client->clientworld.gettime());
		}

		//handle the SDL events:
		SDL_PumpEvents();
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			Nil_inputevent inputevent;
			inputevent.sdl_inputevent = &event;
			if (evtmapper.lookup(inputevent) || game_client->chat_visible) {
				if (!game_client->event_handler->handle(&inputevent))
					logmsg(lt_warning, "Event hot handled");
			} else {
				logmsg(lt_debug, "No binding found");
			}
		} //while (SDL_PollEvent(&event)) 

		game_client->think();

		// get surfcae for new frame
		Mutable_raw_surface tmp; 
		
		if (SDL_MUSTLOCK(screen)) {
			if ( SDL_LockSurface(screen) < 0 )
				logmsg(lt_error,"Unable to lock SDL surface");
		}
		tmp.attach(screen_size.x, screen_size.y, (int32 *)screen->pixels);

		console.show_console(game_client->console_visible);
		if (!game_client->console_visible) {
			game_client->draw(&tmp);
		
			if (game_client->logo_visible) {
				logo->draw_alpha(&tmp,tmp.get_xsize()-logo->get_xsize()-2,15);
			}

			if (game_client->fps_visible) {
				char str[140];
				float tick_time = game_client->get_trend_tick_duration();
				if (tick_time == 0) tick_time = 1;
				float fps = 1/tick_time;
				sprintf(str,"fps: %.2f, objects:%d, inuse:%d", fps, game_client->clientworld.getobjcount(),  game_client->clientworld.get_anon_objects_inuse());
				if (game_client->smallfont)
					game_client->smallfont->put_text(10, 0, str, &tmp, true, 255, 255, 255);
			}

			if (game_client->traffic_visible) {
				traffic_sum += game_client->last_update_size;
				float traffic_time = game_client->clientworld.gettime()- last_traffic_sample;
				if (traffic_time > 1) {
					traffic_trend += ((traffic_sum/traffic_time)-traffic_trend)/5;
					traffic_sum = 0;
					last_traffic_sample = game_client->clientworld.gettime();
					traffic_graph.addsample(traffic_trend/(128*1024/8));
				}
				traffic_graph.draw(&tmp);
			}

			//NT better display chat messages this way...
			game_client->draw_messages( &tmp );

			if(game_client->scoreboard_visible) {
				game_client->draw_scoreboard( &tmp );
			} else if (game_client->chat_visible) {
				game_client->set_event_handler(my_chat);
			} else if (game_client->menu_visible) {
				game_client->draw_menu(&tmp, my_menu);
				game_client->set_event_handler(my_menu);
			}
			// make death pic
			if (game_client->death_cam_active) {
				if (game_client->clientworld.time_of_death >= 0 && (game_client->clientworld.gettime() - game_client->clientworld.time_of_death) == 0)
					game_client->death_cam_armed = true;
		
				if (game_client->death_cam_armed && (game_client->clientworld.gettime() - game_client->clientworld.time_of_death) > 0.2) {
					game_client->screen_shoot = true;
					game_client->death_cam_armed = false;
				}
			} else
				game_client->death_cam_armed = false;
		
			if (game_client->screen_shoot) {
				game_client->screen_shoot = false;
				screen_shot_number++;
		
				char filename[100];
				sprintf(filename,"/tmp/nil_screenshot_%i.bmp",screen_shot_number);
	
				if ( SDL_SaveBMP(screen,filename) == 0 ) {
					logmsg(lt_message, "Dumping screenshot to: %s", filename);
				} else {
					logmsg(lt_error, "Error dumping screenshot to: %s", filename);
				}			
				
				/*
				//Make sure that all the pixels are visible:
				int32 *pixels = tmp.get_pixels_write();
				for (int y=0;y<tmp.get_ysize();y++)
					for (int x=0;x<tmp.get_xsize();x++) {
						int32 *pixel = pixels + x+y*tmp.get_xsize();
						*pixel |= 0xff000000;
					}
				tmp.save_to_file(filename);*/
			}
		}

		if (SDL_MUSTLOCK(screen) ) {
			SDL_UnlockSurface(screen);
		}

		// Replacing SDL_UpdateRect with SDL_Flip so that double buffering works
		// - Phil Howlett 8th Sept 2004
		//
		//SDL_UpdateRect(screen, 0,0, screen_size.x, screen_size.y);
		SDL_Flip(screen);
		
	} // while not done

	// delete joysticks
	// when game_client is deleted, netstate quit is sent to server
	delete[] joys;

	return;
}
