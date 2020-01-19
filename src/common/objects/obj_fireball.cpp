/***************************************************************************
  obj_fireball.cpp  -  a fireball
 ---------------------------------------------------------------------------
  begin                : 1999-12-19
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_fireball.h"
#include "common/objects/obj_explosion.h"
#include "common/objects/obj_player_avatar.h"
#include "common/console/logmsg.h"

Graphic *Obj_fireball::frame = 0;

Obj_fireball::Obj_fireball(World *world_, int id) : Obj_base(world_,id) {
	if ( frame == 0 && world->isclient() ) {
		frame = world->getLoader()->graphics.getgraphic("gfx/obj/fire/fire.rgbf");
		logmsg(lt_debug,"loaded graphic");
	}
}

Obj_fireball::~Obj_fireball() {}

void Obj_fireball::init(int owner, float xpos_, float ypos_, float xspeed_, float yspeed_, float explosion_damage_, int explosion_size_, int penetration_, int max_age_) {
	ownerid = owner;
	xpos = xpos_;
	ypos = ypos_;
	xspeed = xspeed_;
	yspeed = yspeed_;
	explosion_damage = explosion_damage_;
	explosion_size = explosion_size_;
	// FIXME: where is this used??
	//penetration = penetration_;
	air_resistance = 0;
	yaccel = 0;
	curtime = world->gettime();
	max_age = max_age_;
}

void Obj_fireball::draw(Viewport *viewport) {
	int x = ROUND(xpos)-viewport->xpos;
	int y = ROUND(ypos)-viewport->ypos;
	frame->draw_view(viewport,x,y);

/*obsolete
	if (x >= 0 && y >= 0 && x < viewport->get_xsize() && y < viewport->get_ysize()) {
		int32 *target = viewport->get_pixels()+x+y*viewport->get_xsize();
		*target = 0xffffff | PF_TYPE_EARTH;
	}
*/
}

void Obj_fireball::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
//	thinker->RWfloat(explosion_damage);
//	thinker->RWint(explosion_size);
}

void Obj_fireball::server_think() {
	if (age()>max_age) {
		done = true;
		return;
	}
	
	//! this calls client_think() for us and checks for collisions
	Map_hit hit = update_simple(false);
	
	if (hit.hittype != ht_none) {
		done = true;
		return;
	}
	
	int players = world->get_player_count();
	Vector speed;
	
	for (int i=0; i<players; i++) {
		Obj_player_avatar *victim = world->get_player_number(i);
		
		if ( victim && victim != world->get_player_avatar(ownerid) ) {
			// is a player, but isn't us
			int px, py;
			float fx, fy;
			victim->getpos(fx, fy);
			px = ROUND(fx);
			py = ROUND(fy);
			
			if ( SQR( px - xpos ) + SQR( py - ypos ) <= 256 ) {
				// in range - so hurt
				victim->hurt(explosion_damage, speed, ownerid);
			}
		}
	}
}
