/***************************************************************************
  obj_smoke.cpp  -  a smoke object
 ---------------------------------------------------------------------------
  begin                : 2000-04-05
  by                   : Maciek Fijalkowski
    email                : fijaljr@linuxfreemail.com 

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2000 by Maciek Fijalkowski
  email                : fijaljr@linuxfreemail.com

 ***************************************************************************/

#include "common/objects/obj_smoke.h"
#include "common/console/logmsg.h"

#define LIFE_TIMER 1

Obj_smoke::Obj_smoke( World *world_, int id, int32 color ) : Obj_base(world_,id), m_color(color) {
	smoke_map_ready = false;
}

Obj_smoke::~Obj_smoke() {}
       
void Obj_smoke::init(int owner, float xpos_, float ypos_, int size_, int32 color ) {
	ownerid = owner;
	xpos = xpos_;
	ypos = ypos_;
	xspeed = 0;
	yspeed = 0;
	yaccel = -DEFAULT_ACCEL/10;
	createtime=world->gettime();
	size=size_;
	air_resistance=0.007;
	m_color = color;
}

void Obj_smoke::draw(Viewport *viewport) {
	/*	int x0 = ROUND(xpos)-viewport->xpos;
	int y0 = ROUND(ypos)-viewport->ypos;
	int32 *target;

	int size = (int)(age () * SMOKE_SIZE);
	for ( int x = -size + x0; x < size + x0; ++x )
		for ( int y = -size + y0; y < size + y0; ++y ) {
			if ( (x - x0)*(x - x0) + (y - y0)*(y - y0) < size*size ) {
				target = viewport->get_pixels_write()+x+y*viewport->get_xsize();
				if ( rand () % 10 < 4 )
					*target = 0xaaaaaa | PF_TYPE_HOLE;
			}
			}*/

	if ( !smoke_map_ready )
	{
		int size = SMOKE_SIZE;
		for ( int x = 0; x < 2 * size + 1; ++x )
			for ( int y = 0; y < 2 * size + 1; ++y ) {
				if (( (x - size)*(x - size) + (y - size)*(y - size) < size*size ) && ( rand () % 100 < 12 ))
					smoke_map [ x ] [ y ] = 1;
				else
					smoke_map [ x ] [ y ] = 0;
			}
		smoke_map_ready = true;
	}
	// ok, smoke has been made

	int x0 = ROUND(xpos)-viewport->xpos;
	int y0 = ROUND(ypos)-viewport->ypos;
	int32 *target;

	for ( int x = -SMOKE_SIZE; x < SMOKE_SIZE; ++x )
		for ( int y = -SMOKE_SIZE; y < SMOKE_SIZE; ++y ) {
			//			logmsg ( lt_debug , "sx = %i sy = %i map = %i" , x , y , smoke_map [ x + SMOKE_SIZE ] [ y + SMOKE_SIZE ] );
			if ( smoke_map [ x + SMOKE_SIZE ] [ y + SMOKE_SIZE ] ) {
				int sx = (int)(age () * x) + x0;
				int sy = (int)(age () * y) + y0;
				//				logmsg ( lt_debug , "sx = %i sy = %i" , sx , sy );
				if (sx >= 0 && sy >= 0 && sx < viewport->get_xsize() && sy < viewport->get_ysize()) {
					target = viewport->get_pixels_write()+sx+sy*viewport->get_xsize();
					*target = m_color | PF_TYPE_HOLE;
				}
			}
		}
}

void Obj_smoke::serialize(Serializer *serializer)
{
	Obj_base::serialize(serializer);
       
	serializer->rw(size);
	serializer->rw(m_color);
}

void Obj_smoke::client_think() {
	//Map_hit hit = update_simple(false); //this calls client_think() for us and checks for collisions
	update_simple(false); //this calls client_think() for us and checks for collisions
	//newton_update(world->gettime());

	if ( age() > LIFE_TIMER ) {
		done = true;
	}
}

void Obj_smoke::server_think() {
	//Serverworld *serverworld = (Serverworld *)world;
       
	//Map_hit hit = update_simple(false); //this calls client_think() for us and checks for collisions

	//if ( age() > LIFE_TIMER )
		//done = true;
}




