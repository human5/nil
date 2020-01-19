/***************************************************************************
  obj_homingmissile.cpp  -  the homing missile
 ---------------------------------------------------------------------------
  begin                : 2003-07-17
  by                   : Uwe Fabricius
  email                : ???@???.???

  last changed         : 2004-05-16
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2003 by Uwe Fabricius
  email                : qute@klog.dk

 ***************************************************************************/

#include "common/objects/obj_player_avatar.h"
#include "common/objects/obj_homingmissile.h"
#include "common/objects/obj_projectile.h"
#include "common/objects/obj_explosion.h"
#include "common/objects/obj_smoke.h"
#include "common/objects/obj_player_data.h"
#include "common/console/logmsg.h"
#include "client/clientworld.h"

/***********************************************
 *
 * The fuels
 *
 ***********************************************/
#define HOMING_MISSILE_CHASE_FUEL     (3.5)
#define HOMING_MISSILE_SHOT_FUEL      (0.4)
#define HOMING_MISSILE_START_DELAY    (0.1)
#define HOMING_MISSILE_THRUST          400
#define HOMING_MISSILE_CUT_OF_RADIUS   200
#define HOMING_MISSILE_ATTRACTION      500
#define HOMING_MISSILE_DISTANT_PING     20
/***********************************************/
// derived constants
#define HOMING_MISSILE_IGNITION_FUEL (HOMING_MISSILE_CHASE_FUEL + HOMING_MISSILE_SHOT_FUEL)
#define HOMING_MISSILE_INITIAL_FUEL  (HOMING_MISSILE_IGNITION_FUEL + HOMING_MISSILE_START_DELAY)
/***********************************************/

Obj_homingmissile::Obj_homingmissile( World *world_, int id ) : Obj_base( world_,id ), smoke_color( 0xAAAAAA ) {
	for( int i = 0; i < 8; i++ )  sprite[i] = NULL;
}

Obj_homingmissile::~Obj_homingmissile() {}
       
void Obj_homingmissile::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float trail_, float power_, int   size_, Vector& aimVector_ ) {
	ownerid        = owner;
	xpos           = xpos_;
	ypos           = ypos_;
	xspeed         = xspeed_;
	yspeed         = yspeed_;
	yaccel         = DEFAULT_ACCEL;
	createtime     = world->gettime();
	trail          = trail_;
	power          = power_;
	size           = size_;
	last           = 0;
	air_resistance = 0.002;
	// uwe start
	signalcount = -1;
	fuel        = HOMING_MISSILE_INITIAL_FUEL;
	smoke_color = 0xAAAAAA;
	aimVector   = aimVector_;
	aimVector.scale( HOMING_MISSILE_THRUST );
	// uwe end

	// init object time, assume it was created at the begining of the last frame,
	// this is needed for the first call to newton_update
	curtime = world->gettime() - world->get_last_frame_delta();
	
	// init mLeaveOwerID to prevent the rocket from hitting its owner immediatly
	Obj_player_data *player_data = (Obj_player_data *)world->get_object(ownerid,ot_player_data);
	if(player_data) {
		mLeaveOwnerID = player_data->getavatarid();
	} else {
		mLeaveOwnerID = -1; // this should not happen
	}
}

//-----------------------------------------------------------------------------
//The frames are numbered like this:
//234
//1 5
//076
void Obj_homingmissile::draw(Viewport *viewport) {
	int x = ROUND(xpos)-viewport->xpos;
	int y = ROUND(ypos)-viewport->ypos;
	
	double rad=atan2(yspeed,xspeed); // Calculated the direction
	// rad between PI and -PI, 0 = due right, clockwise +
	
	rad *= 4.0 / PI;
	// now between -4 and 4

	int frame = (int)(rad +5.5);
	frame %= 8;	

	if (!sprite[frame]) {
		// *WARNING* Could this be static??  Same goes for grep STATIC_STRING. email qute@cutey.com if you know the answer
		char s[100];
		sprintf(s,"gfx/obj/missile/homing%i.png",frame);
		//logmsg(lt_fatal,"fil: %s rad=%f\n  dir %d",s,rad,dir);
		sprite[frame] = world->getLoader()->graphics.getgraphic(s);
		if (!sprite[frame]) return;
	}
	
	sprite[frame]->draw_view(viewport,x,y);
}

void Obj_homingmissile::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
	
	serializer->rw(trail);
	serializer->rw(power);
	serializer->rw(last);
	serializer->rw(size);
	serializer->rw(mLeaveOwnerID);

	serializer->rw(aimVector);
	serializer->rw(fuel);
	serializer->rw(smoke_color);
}

void Obj_homingmissile::client_think() {
	// create smoke on the clients
	if( world->isclient() && fuel > 0 && fuel != HOMING_MISSILE_CHASE_FUEL )
	{
		Clientworld *clientworld = (Clientworld *)world;
		Obj_smoke *smoke = (Obj_smoke *) clientworld->newanonobject(ot_smoke);
		if( smoke ) { 
			smoke->init( ownerid, xpos, ypos, 10, smoke_color );
		}
	}
	// move the rocket
	newton_update(world->gettime());
}

void Obj_homingmissile::server_think() {
	const float dt = world->gettime() - curtime;
	Serverworld *serverworld = (Serverworld *)world;

	//this calls client_think() for us and checks for collisions
	Map_hit hit = update_simple(true);
      
	setdirty();
	air_resistance = 0.001;
	xaccel         = 0;
	yaccel         = DEFAULT_ACCEL;
	smoke_color    = 0xAAAAAA;

	// take initial delay into account
	if( fuel > HOMING_MISSILE_IGNITION_FUEL ) {
		fuel -= dt;
	}
	// Only containing some fuel the homing missile has
	// the ability to accelerate
	else if( fuel > 0 ) {
		// search for the nearest worm in the neighbourhood
		int players = world->get_player_count();
		Obj_player_avatar *nearestPlayer     = NULL;
		int                nearestSquareDist = 100000;

		for ( int i = 0; i < players; i++ ) {
			Obj_player_avatar *victim = world->get_player_number( i );

			// homing missiles do not chase their owner or dead avatars
			// and the cannot detect invisible worms
			if(    victim
				&& victim != world->get_player_avatar( ownerid )
				&& victim->getstate() == Obj_player_avatar::LIVING
				&& victim->is_visible() )
			{
		    	int   px, py, dx, dy,
				SquareDist;
		    	float fx, fy;
		    	victim->getpos( fx, fy );
		    	px = ROUND(fx);
		    	py = ROUND(fy);

		    	dx = (int)(px - xpos);
		    	dy = (int)(py - ypos);
		    	if( (SquareDist = (dx*dx + dy*dy)) < nearestSquareDist ) {
					nearestSquareDist = SquareDist;
					nearestPlayer     = victim;
		    	}
			}
		}

		// a victim could be found
		if( nearestSquareDist < ( HOMING_MISSILE_CUT_OF_RADIUS *
				                  HOMING_MISSILE_CUT_OF_RADIUS   )
			&& nearestPlayer != NULL )
		{
			int   px, py, dx, dy;
			float fx, fy;
			float dist = sqrt(nearestSquareDist);
			nearestPlayer->getpos( fx, fy );
			px = ROUND(fx);
			py = ROUND(fy);
			dx = (int)(px - xpos);
			dy = (int)(py - ypos);

			float factor = HOMING_MISSILE_ATTRACTION / dist;

			air_resistance = 0.006;
			// accelerate towards the victim worm
			xaccel += (int)(factor * dx);
			yaccel += (int)(factor * dy);

			// smoke color
			smoke_color = nearestPlayer->get_owner_data()->getcolor();
			// reduce fuel only if chasing a worm
		    fuel -= dt;

			int new_signalcount = (int)( HOMING_MISSILE_DISTANT_PING * (dist/HOMING_MISSILE_CUT_OF_RADIUS)) + 3;
			if( signalcount-- == 0 ) {
		    	//NT wav not yet checked in, world->startSound( "sound/beep.wav" , 0, 1, objectid );
			}		
			if( signalcount < 0 ) {
		    	//NT wav not yet checked in, world->startSound( "sound/beep.wav" , 0, 1, objectid );
		    	signalcount = new_signalcount;
			}
			if( new_signalcount < signalcount ) {
		    	signalcount = new_signalcount;
			}
		// no victim, but initial acceleration
	    } else if( fuel > HOMING_MISSILE_CHASE_FUEL ) {
			xaccel += aimVector.x;
			yaccel += aimVector.y;
			fuel   -= dt;
		} else {
			fuel = HOMING_MISSILE_CHASE_FUEL;
		}
		
	} // if( fuel > 0 )

	bool blow=false;
	if(trail>0)
		if((age()>0.03) && (age()>last+trail)) {
			last+=trail;
			Obj_projectile *bullet = (Obj_projectile *)serverworld->newobject(ot_projectile);
			if (bullet) bullet->init(ownerid, xpos, ypos, xspeed/10+rnd(-15,15), 25+yspeed/5+rnd(-1,1), 100, 4, 0, 0);
		}
	if (hit.hittype != ht_none) {
		net_dirty = true;
		blow=true;
	}
	if(blow) {
		done=true;
		Obj_explosion *explosion = (Obj_explosion *)serverworld->newobject(ot_explosion);
		world->startSound("sound/bgun.wav", 0, 1, xpos, ypos);
		if (explosion) explosion->init(ownerid, ROUND(xpos), ROUND(ypos), size, ROUND(power)
		);
	}
}
