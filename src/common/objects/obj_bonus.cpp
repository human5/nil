/***************************************************************************
  obj_bonus.cpp  -  boni falling from the sky
 ---------------------------------------------------------------------------
  begin                : 2000-02-20
  by                   : Daniel Burrows
  email                : dburrows@debian.org

  last changed         : 2004-03-27
  by                   : Nils Thuerey
  email                : n_t@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2000 by Daniel Burrows
  email                : dburrows@debian.org

 ***************************************************************************/

#include "common/systemheaders.h"
#include "common/objects/obj_bonus.h"
#include "common/console/logmsg.h"

Obj_bonus::Obj_bonus(World *world_, int id) : Obj_base(world_, id) {}

// FIXME: Try at most 20 times to place a bonus before giving up.
// FIXME: catalog the "open" areas with a floodsearch and be smarter?  How much computation will that take?
void Obj_bonus::init(int xpos_, int ypos_, float timeout_)
{
	xpos=xpos_;
	ypos=ypos_;
	if (timeout_ == NOTIMEOUT)
		timeout = NOTIMEOUT;
	else
		timeout = world->gettime()+timeout_;
}

// FIXME: need to check onground() and decide whether to call simple_update()
// based on that! -- simple_update() doesn't avoid bouncing.  (bug in
// newton_update?)
void Obj_bonus::server_think() {
	Map_hit hit=update_simple(true);

	int playerid=dynamic_cast<Serverworld *>(world)->find_radius_object(xpos, ypos, getradius(), ot_player_avatar);

	if(playerid!=-1) {
		Obj_player_avatar *hitter=(Obj_player_avatar *) world->get_object(playerid, ot_player_avatar);
		if( pickup(hitter) ) {
			logmsg(lt_debug, "Player picked up bonus");
		}
	}

	switch(hit.hittype) {
	case ht_object:
		break;
	case ht_left:
	case ht_right:
	case ht_top:
	case ht_map:
		bogo_bounce();
		break;
	case ht_none:
	default:
		break;
	}

	if (timeout != NOTIMEOUT && curtime >= timeout)
		do_timeout();

	setdirty();
}

void Obj_bonus::do_timeout() {
	bedone();
}

void Obj_bonus::serialize(Serializer *serializer) {
	Obj_base::serialize(serializer);
	serializer->rw(timeout);
}
