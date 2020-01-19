/***************************************************************************
  nil_bot.cpp  -  NiL bot mode
 ---------------------------------------------------------------------------
  begin                : 2003-10-01
  by                   : Harald Koestler (???)

  last changed         : 2004-03-13
  by                   : Nils Thuerey (n_t@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2004-09-22
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Use load_files_standard().

  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : move to src/client and include cleanup

  copyright            : (C) 2003 by Harald Koestler (???)
                             2004 by Thomas Pohl (???)
                             2004 Nils Thuerey (n_t@gmx.de)
                             2005 Christoph Brill (egore@gmx.de)

 ***************************************************************************/


/*************************************
 * BOT ToDo List:
 * - dont explicitly set angles for aiming
 * - use real screen field of view
 * - check weapons configuration for available ones...
 * - calculate rocket/weapon trajectories more exactly
 *
 */

#include "client/bot.h"
#include "client/controls/keymapper.h"
#include "client/game_client.h"
#include "common/configuration/configuration.h"
#include "common/console/logmsg.h"
#include "common/objects/obj_settings.h"
#include "common/objects/obj_player_avatar.h"
#include "common/objects/obj_player_data.h"
#include "common/randomstream.h"

// local player id for bot
#define BOT 0

#define PI (float)3.14159265
#define MINELEV (PI/8.0)
#define HALFELEV (PI/2.0)
#define MAXELEV PI


// weapon defines
// TODO check if these are really the weapons on the server
#define W_SHOTGUN 0
#define W_MINIGUN 1
#define W_ROCKETLAUNCHER 2
#define W_HOMINGMISS 3

// distance at which bot trys to get away from target
#define MIN_TARGET_DISTANCE 50

// only rate each n'th frame
#define RATE_INTERVAL 10

//! the main client program
void run_bot(Configuration &config) {
	//! flag for handeling if we want to exit the game
	bool done = false;
	logmsg(lt_message, "Starting NiL bot...");
	Loader loader;
	load_files_standard(loader, config.get_option("data"));

	Config_xy screen_size = config.get_option_xy("screen_size");
	int sdl_video_flags = SDL_HWSURFACE;
	SDL_Surface *screen = SDL_SetVideoMode(1, 1, 32, sdl_video_flags);
	if (!screen) {
		logmsg(lt_fatal,"Couldn't set %ix%ix32 video mode: %s\n",screen_size.x, screen_size.y,SDL_GetError());
		return;
	}


	Evtmapper evtmapper;
	evtmapper.init(&config);

	Game_client *game_client = new Game_client();

	Soundsystem soundsystem(&loader, game_client, &config);
	game_client->init(&loader, &soundsystem, &config);

	//FIXME: generate a random color for the bot
	
	int join_error=game_client->join_game(config.get_option("server"),
	                                      config.get_option_int("server_port"),
	                                      config.get_option_int("local_players"),
	                                      config.get_option("player_name"),
	                                      config.get_option_int("player_color"),
	                                      config.get_option_int("team"));

	if (join_error) {
		logmsg(lt_fatal,"Unable to join the game (%i), giving up and going home",join_error);
		exit(-1);
	}

	int view_quality = config.get_option_int("view_quality");
	game_client->setgeom(0,0,screen_size.x,screen_size.y,view_quality);
	game_client->clientworld.set_gorelevel(0);

	//for stats:
	game_client->smallfont = loader.fonts.getfont("arial",10);

	//DEBUG D7
	/*logmsg(lt_message,"test ...");
	int xc=0,yc=0;
	//C: debug:  COLL??? s 59,239 - e 50,61760 , ret 59,240  - rad 1
	game_client->clientworld.collision(59,239, 50,61760, xc,yc, 1);
	logmsg(lt_message,"test %d,%d...",xc,yc);
	exit(1);*/
	//DEBUG D7
	Random_stream botrand(time_ms());
	int jumpCounter = 0;
	int my_id = -1;
	int worldx,worldy;
	int my_team = -1; // initied later on from player data

	//controls.set_flag(Controls::cf_jump,1);

	worldx = game_client->get_worldsizex();
	worldy = game_client->get_worldsizey();
	logmsg(lt_debug,"sizex %i, sizey %i",worldx,worldy);

	// variables and structures for selecting a target
	struct Target {
		// targeting flags
		bool isValid;
		bool isFoe;
		bool isHidden;

		float angle;
		float dist2;
		float vdx, vdy;
		int rating;			// target rating
		int id; 				// index in array = player number

		Target() : 
			isValid(false), isFoe(false), isHidden(false),
			angle(0.0), dist2(0.0), vdx(0.0), vdy(0.0),
			rating(0) { };
	};
	Target* target = new (Target[20]); // FIXME

	float avPosx, avPosy;         // read position from player avatar to check 
	float my_x=10.0, my_y=10.0;   // bot position x,y
	int num_players;							// no of players on server
	int curr_pl = 0;              // player to check
	int rateCount = 0;						// only rate each few frames
	Obj_settings::GameMode gameMode = game_client->clientworld.getServerSettings()->getGameMode();
	float my_lastx, my_lasty;
	int lastNotChangedCounter = 0;

	bool walkAway = false;
	int walkAwayCounter = 0;


	typedef enum {
		stNone = 1,       //! none, search new
		stPlayer = 2,     //! targeting player
		stTeammate = 3,   //! walking with team mate
		stMappoint = 4    //! walk towards random map spot
	} botTargetState;

	typedef enum {
		swStanding = 1,   //! nothing?
		swWalking = 2,    //! walking towards goal
		swBacking = 3     //! backing from target (while not shooting)
		                  //FIXME: add state for being stuck?
	} botWalkState;
	
	//FIXME: add state of the bot
	//botWalkState walkState = swStanding; // walking state
	botTargetState stateTarget = stNone; // targeting state
	//FIXME: add IQ
	// double botIQ = 10.0; // "intelligence" from 0-10

	float angle =0.0, my_vdx, my_vdy;
	my_vdx = my_vdy = 0.0;
	Target *currTarget = NULL;
	
	while (!done) {

		Controls &controls = game_client->get_controls(BOT);
		// always walk and dig...
		controls.set_flag(Controls::cf_dig,true);
		controls.set_flag(Controls::cf_walk,true);

		my_id = game_client->get_ownid(BOT);
		num_players = game_client->get_numplayers(); // TODO reset flags when player leaves?
		Obj_player_avatar* my_avatar = NULL;

		// find own position and store it in (my_x, my_y)
		for(int p = 0; p < num_players; p++) {
			Obj_player_avatar *curr_avatar = game_client->get_player(p);
			if(!curr_avatar) continue;
			if (curr_avatar->getid() == my_id) {
				my_avatar = curr_avatar;
				my_avatar->getpos(avPosx,avPosy);
				my_lastx = my_x;
				my_lasty = my_y;
				my_x = avPosx;
				my_y = avPosy;
				if((my_lastx == my_x )&&(my_lasty==my_y)) lastNotChangedCounter++;
				else lastNotChangedCounter=0;
				break;
			}
		}

		// if we're stuck, try changing direction for some time
		// and switch off jetpack
		if(lastNotChangedCounter>5) walkAwayCounter = botrand.get_integer(10,100);

		if(my_avatar==NULL) {
			logmsg(lt_fatal,"Couldn't find bot avatar..." );
			controls.clear_all();
			currTarget = NULL;
			stateTarget = stNone;
			my_vdx = my_vdy = 0.0;
			rateCount = jumpCounter = lastNotChangedCounter = walkAwayCounter = 0;
			my_x=10.0, my_y=10.0; 
		} else {

		// look for targets... haehaehae!
		Obj_player_data *my_player_data = my_avatar->get_player_data();
		my_team = my_player_data->getTeam();
		int nearestSquareDist = 1000000;

		// try to find a weapon to shoot with
		int my_weapon = 0;
		for(int w=0; w<3; w++) {
			Obj_weapon *objweapon = my_player_data->get_weapon( w );
			if(objweapon->canFire()) { my_weapon = w; w=10; continue; }
		}


		// rate targets
		// only do this each n frames
		if((stateTarget ==stNone) || (rateCount == 0)) {

			for(curr_pl=0; curr_pl<num_players; curr_pl++) {
				// find targets and check distance
				target[curr_pl].isValid = false;
				Obj_player_avatar* avatar = game_client->get_player(curr_pl);
				if (avatar &&
						avatar->getid() != my_id &&                           // is he not me?
						avatar->is_visible() &&                            // is he visible?
						avatar->getstate() == Obj_player_avatar::LIVING) { // is he living?
					avatar->getpos(avPosx,avPosy);
					const float dx = avPosx - my_x,
					dy = avPosy - my_y,
					dist2 = dx*dx + dy*dy,
					dt = sqrt (dist2) / ((my_weapon == W_SHOTGUN) ? 300 : 200);
					float myXSpeed, myYSpeed, otherXSpeed, otherYSpeed;
					avatar->getspeed (otherXSpeed, otherYSpeed);
					my_avatar->getspeed (myXSpeed, myYSpeed);
					target[curr_pl].vdx = (avPosx+otherXSpeed*dt) - (my_x+myXSpeed*dt),
					target[curr_pl].vdy = (avPosy+otherYSpeed*dt) - (my_y+myYSpeed*dt);

					angle = atan2 (target[curr_pl].vdy, target[curr_pl].vdx) + M_PI;
					Obj_player_data* otherData = avatar->get_player_data();
					if(gameMode == Obj_settings::gm_teamplay) {
						target[curr_pl].isFoe = (my_team != otherData->getTeam());
					} else {
						target[curr_pl].isFoe = true;
					}

					target[curr_pl].id = curr_pl;
					target[curr_pl].isValid = true;
					target[curr_pl].isHidden = false;
					target[curr_pl].rating = ROUND (-sqrt (dist2));
					if (target[curr_pl].isFoe) {
						target[curr_pl].rating += 10000;
						if (target[curr_pl].rating < 1000) target[curr_pl].rating = 1000;
					}
					else target[curr_pl].rating += 5000;
					target[curr_pl].angle = angle;
					target[curr_pl].dist2 = dist2;
				}
				else target[curr_pl].isValid = false;

			} // curr pl

			for (int p1 = 0; p1 < num_players; p1++) {
				if ( ! target[p1].isFoe || ! target[p1].isValid) continue;
				for (int p2 = 0; p2 < num_players; p2++) {
					if (p1 == p2 || ! target[p2].isValid) continue;

					// p1 and p2 in same direction?
					float angleDiff = target[p1].angle - target[p2].angle;
					const float SECTOR_SIZE = 10 * (M_PI/180.0);
					if (fabs (angleDiff) < SECTOR_SIZE ||
							fabs (angleDiff) > 2*M_PI-SECTOR_SIZE) {

						// p2 is foe?
						if (target[p2].isFoe) {
							// even better
							target[p1].rating += 50;
						}
						// else p2 is friend
						else {
							// p2 might get hurt
							target[p1].rating -= 10;
							// friend is closer? -> p2 is isHidden
							target[p1].isHidden = (target[p1].dist2 > target[p2].dist2);
						}
					}

				}
			}

			// look for highest targetRating and set variables accordingly
			int highestRating = -1;
			for (int p = 0; p < num_players; p++) {
				// debugging
				if (false && jumpCounter == 1) {
					printf ("Player %i:\n\tisValid: %i\n\tisFoe: %i\n\tisHidden: %i\n\trating: %i\n",
							p, target[p].isValid, target[p].isFoe, target[p].isHidden, target[p].rating);
				}
				if (target[p].isValid &&
						target[p].rating > highestRating) {
					highestRating = target[p].rating;
					currTarget = &target[p];
					if(target[p].isFoe) stateTarget = stPlayer;
					else stateTarget = stTeammate;
				}
			}

		} // ratecount
		rateCount ++;
		rateCount = rateCount % RATE_INTERVAL;

		// shoot at target?
		bool shootNow = false;

		if(currTarget) {
			Sint8 state = Obj_player_avatar::SPAWNING; // some default
			if(game_client->get_player( currTarget->id )) state = game_client->get_player( currTarget->id )->getstate();
			if(state != Obj_player_avatar::LIVING) {
				currTarget = NULL;
				stateTarget = stNone; }
		}

		bool tooFarAbove = false;
		if(currTarget) {
			shootNow = currTarget->isFoe && (! currTarget->isHidden);
			float target_x=0.0, target_y=0.0;
			game_client->get_player( currTarget->id )->getpos(target_x,target_y);

			if (currTarget->vdx < 0)
				if (currTarget->vdy < 0) angle = currTarget->angle + 0.5*M_PI;
				else         angle = currTarget->angle - 1.5*M_PI;
			else           angle = 1.5*M_PI - currTarget->angle;
			if (angle < MINELEVATION) tooFarAbove = true;

			nearestSquareDist = ROUND (currTarget->dist2);

			my_vdx = currTarget->vdx;
			my_vdy = currTarget->vdy;
		} else {
			//if(num_players>0) logmsg(lt_debug," NO TARGET ??? %d", num_players);
			shootNow = false;
		}

		//logmsg(lt_debug,"Player %i, x %.2f, y %.2f,me: x %.2f, y %.2f dist %i,angle %.2f",playid,target_x,target_y,my_x, my_y,nearestSquareDist,angle);
		if(nearestSquareDist > 22000) shootNow = false;
		//FIXME: unused, someone should use it
		//int direction = controls.get_flag(Controls::cf_direction_east)?1:-1;


		if( tooFarAbove ) shootNow = false;
		if((shootNow) && (currTarget)) {

			int xc=0, yc=0;
			float target_x=0.0, target_y=0.0;
			game_client->get_player( currTarget->id )->getpos(target_x,target_y);
			if(my_weapon == W_ROCKETLAUNCHER) {
				if (game_client->clientworld.collision(ROUND(my_x),ROUND(my_y-4.0), ROUND(target_x),ROUND(target_y-4.0), xc,yc,5)) {
					shootNow = false;
				}
			//if (nearestSquareDist < 1000) {
				Obj_weapon *objweapon = my_player_data->get_weapon( W_HOMINGMISS );
				if(objweapon->canFire()) my_weapon = W_HOMINGMISS; 
			//}
			} else {
				if (game_client->clientworld.stoneCollision(ROUND(my_x),ROUND(my_y-4.0), ROUND(target_x),ROUND(target_y-4.0), xc,yc,5)) {
					shootNow = false;
				}
			}

			if(shootNow) {
				controls.set_flag(Controls::cf_shoot,true);
				controls.clear_flag(Controls::cf_jetpack);
			}
		} else {
			shootNow = false; // shoudlnt happen
		}

		controls.set_weapon(my_weapon);
		if(!shootNow) {
			controls.clear_flag(Controls::cf_shoot);
			if (my_vdy < -5.0)  {
				controls.set_flag(Controls::cf_jetpack, true);
			}
		}

		//logmsg(lt_debug,"Botdebug vx %.2f, y %.2f,me: x %.2f, y %.2f dist %i,angle %.2f ",my_vdx,my_vdy,my_x, my_y,nearestSquareDist,angle );
		walkAway = false;
		if(!shootNow) {
			if(ABS(my_vdx) < MIN_TARGET_DISTANCE) {
				walkAway = true;
			}
			else if(walkAwayCounter > 0) {
				walkAway = true;
				walkAwayCounter--;
			}
		}

		if (my_x < 10) {
			controls.set_flag(Controls::cf_direction_east);
		}
		else if (my_x > (worldx - 10)) {
			controls.clear_flag(Controls::cf_direction_east);
		}
		else if (walkAway) {
			// too close
			if (my_vdx < 0) {
				controls.set_flag(Controls::cf_direction_east);
			} else if (my_vdx > 0) {
				controls.clear_flag(Controls::cf_direction_east);
			}
		} else {
			// too far away
			if (my_vdx < 0) {
				controls.clear_flag(Controls::cf_direction_east);
			} else if (my_vdx > 0) {
				controls.set_flag(Controls::cf_direction_east);
			} else {
				//logmsg(lt_debug," VDX ??? ");
			}
		}

		controls.set_elevation(angle);
		if((jumpCounter == 100) && (my_vdy<0.0)) {
			controls.set_flag(Controls::cf_jetpack, false);
			controls.set_flag(Controls::cf_jump, true);
			jumpCounter = 0;
		}
		else controls.set_flag(Controls::cf_jump, false);

		if(tooFarAbove) {
			jumpCounter = 0;
			controls.clear_flag(Controls::cf_jetpack);
			controls.clear_flag(Controls::cf_jump);
		}

		// dont jetpack if bot is stuck?
		if(walkAwayCounter>30) {
			controls.clear_flag(Controls::cf_jetpack);
		}

		//logmsg(lt_debug,"Botdebug %d - w%i, s %i  t%i f%d myvdy:%f", my_id, my_weapon, shootNow, my_team, tooFarAbove, my_vdy);
		} // my avatar

		if (jumpCounter == 70) {
			//handle the SDL events:
			SDL_PumpEvents();
			SDL_Event event;
			while (SDL_PollEvent(&event)) {

				//handle the keyboard/joystick button events:
				if (event.type==SDL_KEYUP || event.type==SDL_KEYDOWN) {
					Nil_inputevent inputevent;
					inputevent.sdl_inputevent = &event;
					evtmapper.lookup(inputevent);
					int target = inputevent.cmdfunction & CMDFUNCTION::TARGET_MASK;

					switch (target) {
						case (CMDFUNCTION::HOTKEY):
							//handle gen purpose hotkeys here...
							int function = inputevent.cmdfunction & CMDFUNCTION::FUNCTION_MASK;
							// only pressed ones after here...
							if (!inputevent.pressed) break;
							switch (function) {
								case (CMDFUNCTION::MENU):
									done = true;
									break;
							} //switch (function)
							break;
					} //switch (target)
				} //if (event.type==SDL_KEYUP || event.type==SDL_KEYDOWN) 
			} //while (SDL_PollEvent(&event))
		}

		if (request_quit) done = true;
		game_client->think();
		jumpCounter++;
  } // while not done

	// clean up
  delete [] target;
}
