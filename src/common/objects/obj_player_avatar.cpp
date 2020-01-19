/***************************************************************************
  obj_player.cpp  -  the player
 ---------------------------------------------------------------------------
  begin                : 1999-10-30
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  last changed         : 2004-09-22
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Implemented falldamage.

    copyright            : (C) 1999 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_player_avatar.h"
#include "common/objects/obj_player_data.h"
#include "common/objects/obj_particle.h"
#include "common/objects/obj_scoreboard.h"
#include "common/objects/obj_gib.h"
#include "common/objects/obj_hook.h"
#include "common/objects/obj_settings.h"
#include "common/console/logmsg.h"
#include "common/vfs/loader.h"
#include "client/graphics/graphics.h"
#include "client/clientworld.h"
#include "common/sound/soundsystem.h"

#define HOOK_SPEED 800
#define FALL_DAMAGE_THRESHOLD 5000
#define FALL_DAMAGE_MULTIPLIER 0.05

Obj_player_avatar::Obj_player_avatar(World *world_, int id) : Obj_base( world_, id ), invisible_time(0) {
	cur_walk_frame = 0;
	damage = 0;
	damage_speed.x = 0;
	damage_speed.y = 0;
	hook_id = -1;
	player_data = NULL;
	deathtime = 0.0;
}

Obj_player_avatar::~Obj_player_avatar() {
	logmsg(lt_debug, "*** avatar deleted ***");
}

bool Obj_player_avatar::hurtable() {
	if(state == LIVING)
		return true;
	else
		return false;
}

void Obj_player_avatar::init(int owner, int x, int y) {
	ownerid = owner;
	xpos = x;
	ypos = y;
	radius = 0; //use the objects own collision test.
	health = MAX_HEALTH;
	damage = 0;
	damage_speed.x = 0;
	damage_speed.y = 0;
	state = PRESPAWNING;
	air_resistance = 0;
	invisible_time = 0;
	world->startSound( "sound/pop.wav", 0, 1, objectid );
}

void Obj_player_avatar::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);

	Controls tmp_controls;
	if (serializer->is_reading()) {
		setup_constants();
		if (controls) {
			tmp_controls = *controls;
		}
	}

	tmp_controls.serialize(serializer);

	//copy the controls if it was retrieved:
	if (!serializer->is_reading()) {
		setup_constants();
		if (controls) {
			*controls = tmp_controls;
		}
	}

	serializer->rw(cur_walk_frame);
	serializer->rw(dig_now);
	serializer->rw(health);
	serializer->rw(invisible_time);
	serializer->rw(state);
	serializer->rw(animation_sequence);

	serializer->rw(damage);
	serializer->rw(damage_speed.x);
	serializer->rw(damage_speed.y);
	serializer->rw(deathtime);
}

bool Obj_player_avatar::is_passable(int x, int y) {
	// for world boundaries, to prevent segfaults
	if (x >= world->getSizeX()) return false;
	if (y >= world->getSizeY()) return false;
	if (x < 0) return false;
	if ( (y-5) < 0) return false;

	//int offset = x+y*world->map_buffer.get_xsize();
	// check from y-5 to y if all pixels are passable
	//for (int yc=-5*world->map_buffer.get_xsize(); yc<=0; yc += world->map_buffer.get_xsize()) {
	for (int yc=0; yc<=5; yc++) {
		if (! world->isPassable(x,y-yc) )
			return false;
	}
	return true;
}

void Obj_player_avatar::draw( Viewport *viewport ) {
	//viewport->drawline(ROUND(xpos),ROUND(ypos),100,100,0x0ff0080,0);
	// Dont draw anything if the avatar is not alive.
	if( state != LIVING )  return;
	// Also do not draw this avatar, if it is not ours and
	// and it is invisible
	if( invisible_time > 0 && viewport->getowner() != ownerid )  return;

	setup_constants();
	if (!player_data) return;

	Graphic *sprite = player_data->get_current_frame(animation_sequence);
	if (!sprite) {
		logmsg(lt_error,"Unable to get graphic for skin");
		return;
	}

	int x = ROUND( xpos-viewport->xpos );
	int y = ROUND( ypos-viewport->ypos );

	if( x >= 0 && x < viewport->get_xsize() &&
	    y >= 0 && y < viewport->get_ysize()    )
	{
		// because of the decition above we know that, if the
		// avatar is invisible, it is our own
		if( invisible_time > 0 )  sprite->draw_shadow_only( viewport, x, y );
		else                      sprite->draw_view( viewport, x, y );

		int world_owner = viewport->getowner();
		if( ownerid == world_owner )
		{
			//draw crosshairs
			Vector v;
			if (!(elevation >= -PI/2 && elevation <= PI/2)) {
				v = Math::rad2vector(Math::elev2rad(elevation, direction));
			} else {
				logmsg(lt_warning, "Elevation was NAN!");
				v = Math::rad2vector(PI/2);
			}
			int aimx = ROUND(x+v.x*10);
			int aimy = ROUND(y-4+v.y*10);
			//viewport->draw_pixel_opaque(aimx,aimy,PIXEL(255,255,255));
			viewport->setRawPixelCheck(aimx,aimy,PIXEL(255,255,255));
		}
	}
}

void Obj_player_avatar::server_think() {
	// This should never happen.
	if (world->isclient()) {
		logmsg(lt_warning, "Player server think called on client???");
		return;
	}

	setup_constants();
	if (!controls) {
		logmsg(lt_error, "bitch and moan!");
		return;
	}
	if (!player_data) return;
	if (state == DYING) controls->clear_all();

	float dt = world->gettime() - curtime;

	if (controls->get_flag(Controls::cf_walk))
		animation_sequence = SEQUENCE::WALK;
	else
		animation_sequence = SEQUENCE::STOP;

	//react to the state of the controls.

	//The actual digging is handled by the client code.
	dig_now = controls->get_flag(Controls::cf_dig);

	//jumping
	if (controls->get_flag(Controls::cf_jump)) {
		// only jump if we are on the ground.
		// modify y speed, maybe amplyfy xspeed.
		if (ABS(yspeed) < 10 && canmove() && onground()) {
			yspeed = -65;
			// jump while walking?
			if (controls->get_flag(Controls::cf_walk)) {
				// jump left/right??
				xspeed += direction* (DEFAULT_WALKSPEED/6.0);
			}
		}

	//walking
	} else if (controls->get_flag(Controls::cf_walk)) {
		// animate
		cur_walk_frame++;
		cur_walk_frame &= 3;

		if (onground() && canmove()) {
			// do normal walking
			bool step_taken = false;

			// determine up/down movement while walking
			for (int step=3; step >= -4;step--) {
				//NT why not use normal is_passable here, was Obj_base::is_passable before, 
				//   but that gets players stuck in walls...
				if(  is_passable(direction+ROUND(xpos),ROUND(ypos)+step)   &&
						!is_passable(direction+ROUND(xpos),ROUND(ypos)+step+1) ) {
					xpos = ROUND(xpos)+ direction;
					ypos = ROUND(ypos)+ step;
					yspeed = 0;	xspeed = 0;
					step_taken = true;
					break;
				}
			}

			// there's no obstacle so just walk... (e.g. fall down somewhere)
			if (!step_taken && is_passable(direction+ROUND(xpos), ROUND(ypos)+4)) {
				xpos = ROUND(xpos)+direction;
				xspeed += direction* (DEFAULT_WALKSPEED/6.0);
			}

			yspeed = 0;
		} else {
			// in the air - still allow some movement
			xspeed += direction;
		}

	}

	if (controls->get_flag(Controls::cf_shoot)) { 
		Obj_weapon *weapon = player_data->get_weapon(controls->get_weapon());
		if (weapon) {
			if (weapon->canFire()) {
				weapon->fire(xpos, ypos, Math::elev2rad(elevation,direction));
			} else {
				//FIXME: play click-sound here?
			}
		}
	}

	if (controls->get_flag(Controls::cf_hook_fire))
		hook_fire();

	if (controls->get_flag(Controls::cf_hook_release))
		hook_release();	

	if (controls->get_flag(Controls::cf_hook_in))
		hook_in();	

	if (controls->get_flag(Controls::cf_hook_out))
		hook_out();
	
	if (controls->get_flag(Controls::cf_jetpack)) {
		// try to lift off 
		if( onground() ){
			if(is_passable( ROUND(xpos), ROUND(ypos-0.1) )) ypos -= 0.1; // get off ground
		}
		jetpack();
	}

	if (state == SPAWNING) {
		xspeed = 0;
		yspeed = 0;
	  state = LIVING;
	}

	if (state == PRESPAWNING) {
		//NT whats this for???
		xspeed = 0;
		yspeed = 0;
		state = SPAWNING;
	}

	if (state == DYING) {
		//NT delay respawn
		if(deathtime==0.0) {
			// remove hook, if necessary
			hook_release(); 
			// play dying sound
			/*char wav[] = "sound/dieXY.wav\0";
			int dieSound = rnd(1, 13);
			wav[9]  = '0' + (dieSound / 10);
			wav[10] = '0' + (dieSound % 10);*/
			//NT wav not yet checked in, world->startSound( wav, 0, 1, objectid );

			xspeed = yspeed = yaccel = 0.0;
			logmsg(lt_debug,"avatar has died.");
			//Report our death to the scoreboard:
			Obj_scoreboard *sb= world->getscoreboard();
			if(sb) {
				sb->addfrag(hurter_id,ownerid);
			}	else {
				logmsg(lt_error,"Unable to find the scoreboard for reporting the death of the avatar (this cannot happen)");
			}
			deathtime = world->gettime();
			setdirty();
		}
		if(deathtime>0.0) {
			if(world->gettime()-deathtime > RESPAWN_TIME) {
				done = true;
				setdirty();
			}
		}
		//logmsg(lt_debug,"deathtime %f - %f",deathtime, world->gettime() ); 
		return;
	}

	// update object, make newton moves, check for collisions
	// calls client_think and newton_update, no object collisions 
	Map_hit hit = update_simple(); 
	if (hit.hittype != ht_none) {
		float speedsquared = xspeed * xspeed + yspeed * yspeed;

		if(speedsquared > FALL_DAMAGE_THRESHOLD) {
			float fulldamage = FALL_DAMAGE_MULTIPLIER * speedsquared;
			fulldamage -= FALL_DAMAGE_MULTIPLIER * FALL_DAMAGE_THRESHOLD;
		
			Vector nullVec; nullVec.zero();
			hurt(fulldamage, nullVec, -1);
			hurter_id = ownerid;	//Self-damage...

			cdebug << "Ouch: Hit ground at " << speedsquared << ". Taking " << fulldamage << " damage." 
				<< std::flush;
		}
		bogo_bounce();
	}

	//Handle the _PAIN_
	if (damage != 0) {
		//logmsg(lt_debug,"avatar %d hit: health: %.1f, damage: %.1f",getid(), health,damage);

		if (health == 0) {
			health = 1;
			logmsg(lt_assert,"This cannot happen! health was 0");
		}

		health -= damage;

		//see if we died.
		if (health <= 0) {
			state = DYING;
		} else {
			// play pain sound
			/*char wav[] = "sound/injuredx.wav\0";
			wav[13] = '0' + rnd(1, 7);*/
			//NT wav not yet checked in, world->startSound( wav, 0, 1, objectid );

			xspeed += KNOCK_BACK*(damage*damage_speed.x);
			yspeed += KNOCK_BACK*(damage*damage_speed.y);	
			damage = 0;
			damage_speed.x = 0;
			damage_speed.y = 0;
		}
	}	

	// TODO: armageddon handling, not so nice yet
	if(world->getServerSettings()->getArmageddonTime() > 0) {
		Vector nullVec; nullVec.zero();
		if(ypos > (world->getSizeY()-10.0) ) hurt(20.0, nullVec, -1);
	}

	// eventually decrement invisibility counter (uwe)
	if( invisible_time > 0 ) {
		if( 0 > (invisible_time -= dt) )  invisible_time = 0;
	}

	// update...
	setdirty();
}

void Obj_player_avatar::client_think() {
	//handle the different states:
	if (state == SPAWNING) {
		world->makehole(ROUND(xpos),ROUND(ypos),16);
		//init an animation too?
		return;
	}

	if (state == DYING) {
		if( (world->isclient()) && (deathtime==world->gettime()) ) {
			Clientworld *clientworld = (Clientworld *)world;

			// create blood on client
			int blood = 250 + ROUND(200*clientworld->gorelevel);

			logmsg(lt_debug,"avatar dying: %i (%.0f,%.0f)",blood,damage_speed.x,damage_speed.y);
			int hx = ROUND(damage_speed.x*damage);
			if (hx > 100) hx = 100;
			if (hx < -100) hx = -100;

			int hy = ROUND(damage_speed.y*damage);
			if (hy > 100) hy = 100;
			if (hy < -100) hy = -100;
			
	 		for (int c=0;c< blood ;c++) {
				Vector offset;
				Obj_particle_blood *drop = NULL;
				drop = (Obj_particle_blood *)clientworld->newanonobject(ot_particle_blood);
				if (drop) {
					offset = Math::rad2vector (rnd(0, 999)*(0.002*PI));
					offset.scale(rnd(50, 210));
					drop->init(xpos+rnd(-2,2),ypos-4+rnd(-4,4),
								xspeed+hx+offset.x,
								yspeed+hy+offset.y
						);
					if (colored_blood)
						drop->set_color( mk_bloodcolor(player_data->getcolor()) );
				}
			}

			// remember dying time
			world->time_of_death = world->gettime();

			// make gibs
			int gibs = clientworld->gorelevel*2;
			for (int c=0;c<gibs;c++) {
 				Vector offset;
				Obj_gib *gib = NULL;
				gib = (Obj_gib *)clientworld->newanonobject(ot_gib);
				if (gib)
                {
					offset = Math::rad2vector (rnd(0, 999)*(0.002*PI));
					offset.scale(rnd(10, 30));
					gib->init( xpos, ypos,
						xspeed+damage_speed.x+offset.x,
						yspeed+damage_speed.y+offset.y,
						player_data->getcolor()
					);
                }
			}

		}

 		return;
	} // dying

	//the digging code here is pretty ugly, but be careful touching it,
	//the constants are finely tuned to give smooth digging, altering them will probably upset it.
	if (dig_now) {

		//find the direction to dig in
		float digelev = controls->get_elevation()-(PI/8);

		//and limit the steepness to something that actually looks good 3/4 pi is 45 deg up.
		if (digelev > 3*PI/4) digelev = 3*PI/4;
	
		//create a vector in the direction that we are digging in.
		Vector v = Math::rad2vector(Math::elev2rad(digelev,direction));

		//create a vector to position the center of the hole with, this is needed so that the bottom
		//of the hole always hits the bottom of the bottom of the worm.
		Vector vr = Math::rot90ccw(v);

		//we need to rotate the vector 90 deg clockwise if we are digging east
		if (direction != -1) vr = Math::rot90ccw(Math::rot90ccw(vr));

		//calc the starting hole center
		float hcx = xpos+vr.x*5+1;
		float hcy = ypos+vr.y*5;
		
		//dig a number of holes in the right direction
		for (float c=0; c < 2; c++)
			world->makehole(ROUND(hcx+c*v.x),ROUND(hcy+c*v.y),12);   	
	}

	//do the strict client side stuff:
	if (world->isclient()) {
		Clientworld *clientworld = (Clientworld *)world;

		// create bleeding
		float blood = 1-(health/MAX_HEALTH);
		if (blood > 0.3) {
			float pressure = 1-(ROUND(100*age()) % 400)/400.0;
			pressure *= blood;

  		for (int c=0;c< ROUND(pressure*2*clientworld->gorelevel) ;c++) {
				Obj_particle_blood *drop = NULL;
				drop = (Obj_particle_blood *)clientworld->newanonobject(ot_particle_blood);
				if (drop) {
					Vector v;
					v.x = ROUND(rnd(-1-direction* 5,1-direction*20));
					v.y = ROUND(rnd(-20,-65));
					v.normalize();	
					v.scale(pressure*80);
		
					drop->init(
							xpos+rnd(-1,1),ypos-4+rnd(-1,1),
							ROUND(xspeed+v.x),ROUND(yspeed+v.y)
						);
					if (colored_blood)
						drop->set_color( mk_bloodcolor(player_data->getcolor()) );
				}
			}
		}

		// create blood for damages
		if(damage > 0.0) {
			//logmsg(lt_debug,"avatar damaged: %f (%f,%f)",damage,damage_speed.x,damage_speed.y);
			int hx = ROUND(damage_speed.x*damage*0.1);
			if (hx > 100) hx = 100;
			if (hx < -100) hx = -100;
			int hy = ROUND(damage_speed.y*damage*0.1);
			if (hy > 100) hy = 100;
			if (hy < -100) hy = -100;
			hx = hy = 0;

			for (int c=0;c< damage/20*clientworld->gorelevel ;c++) {
				Vector offset;
				Obj_particle_blood *drop = NULL;
				drop = (Obj_particle_blood *)clientworld->newanonobject(ot_particle_blood);
				if (drop) {
					offset = Math::rad2vector (rnd(0, 999)*(0.002*PI));
					offset.scale(rnd(30, 110));
					drop->init(xpos+rnd(-2,2),ypos-4+rnd(-4,4),
							xspeed+hx+offset.x,
							yspeed+hy+offset.y
							);
					// set player color as blood color
					if (colored_blood)
						drop->set_color( mk_bloodcolor(player_data->getcolor()) );
				}
			}

			// reset
			damage = 0.0;
			damage_speed.x = damage_speed.y = 0.0;
		} // damage blood


		// handle client effects
		setup_constants();
		if (!controls) {
			logmsg(lt_error,"player client think - cant get controls...!");
			return;
		}
		if (!player_data) return;
	
		if (controls->get_flag(Controls::cf_shoot)) { 
			Obj_weapon *weapon = player_data->get_weapon(controls->get_weapon());
			if (weapon) {
				if (weapon->canFire()) {
					weapon->clientFire(xpos, ypos, Math::elev2rad(elevation,direction));
				} else {
					// logmsg(lt_message,"You pull the trigger, but nothing happens...");
				}			
			}
		}

	}

	// move along
	newton_update(world->gettime());
}

void Obj_player_avatar::hook_fire() {
	setup_constants();

	//get rid of the old hook.
	hook_release();

	//Create a new hook if we didn't have one already
	Serverworld *serverworld = (Serverworld *)world;
	Obj_hook *hook = (Obj_hook *)serverworld->newobject(ot_hook);
	hook_id = hook->getid();

	//fire the hook:
	Vector dir = Math::rad2vector(Math::elev2rad(elevation,direction));	
	hook->init(ownerid,ROUND(xpos)+dir.x*5,ROUND(ypos-4)+dir.y*5,xspeed+dir.x*HOOK_SPEED,yspeed+dir.y*HOOK_SPEED);
}

void Obj_player_avatar::hook_release() {
	Obj_hook *hook = (Obj_hook *)world->get_object(hook_id,ot_hook);

	if (hook) {
		hook->bedone();
		hook_id = -1;
	}
}

void Obj_player_avatar::hook_in() {
	Obj_hook *hook = (Obj_hook *)world->get_object(hook_id, ot_hook);
	if (hook) {
		hook->reel_in();
	}
}

void Obj_player_avatar::hook_out() {
	Obj_hook *hook = (Obj_hook *)world->get_object(hook_id, ot_hook);
	if (hook) {
		hook->reel_out();
	}
}

void Obj_player_avatar::jetpack() {
	/* FIXME: Jetpack is always expected to be weapon 4, Tom */
	/* FIXME: No smoke, Tom */
	Obj_weapon *weapon = player_data->get_weapon(4);
	if (weapon) {
		if (weapon->canFire()) {
			weapon->fire(xpos, ypos, Math::elev2rad(elevation,direction));
		} else {
			// FIXME: play emtpy-jetpack-sound
		}
	}
}

void Obj_player_avatar::setup_constants() {
	if (!player_data)
		player_data = get_owner_data();
	
	if (!player_data) {
		logmsg(lt_fatal, "Unable to get_owner_data() in Obj_player_avatar::setup_constants(), going down!");
		world->dump_objects();
		logmsg(lt_warning, "Objects dumped...");
		controls = NULL;
		return;
	}
	controls = &player_data->controls;
	direction = controls->get_flag(Controls::cf_direction_east)?1:-1;
	elevation = controls->get_elevation();
}

void Obj_player_avatar::hurt(float hitpoints, Vector speed, int hurter) {
	if(hurter > 0) {
		world->assert_object_type(hurter, ot_player_data);
	}
	hurter_id = hurter;
	damage += hitpoints;
	damage_speed.add(speed);
}

void Obj_player_avatar::heal(float hitpoints) {
	health+=hitpoints;
	if(health>MAX_HEALTH)
		health=MAX_HEALTH;
	setdirty();
}
