/***************************************************************************
  obj_particle.cpp  -  a particle
 ---------------------------------------------------------------------------
  begin                : 1999-11-04
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

    copyright            : (C) 1999 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_particle.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "client/clientworld.h"

//-----------------------------------------------------------------------------
Obj_particle::Obj_particle(World *world_, int id) : Obj_base(world_,id)
{
	air_resistance = DEFAULT_DRAG*2;
}

//-----------------------------------------------------------------------------
Obj_particle::~Obj_particle()
{
}

//-----------------------------------------------------------------------------
void Obj_particle::init(float x, float y, float xspeed_, float yspeed_)
{
	xpos = x;
	ypos = y;
	xspeed = xspeed_;
	yspeed = yspeed_;
}

//-----------------------------------------------------------------------------
void Obj_particle::client_think()
{
	old_xpos=xpos;
	old_ypos=ypos;
	old_xspeed=xspeed;
	old_yspeed=yspeed;
	
	newton_update(world->gettime());

	//check to see if we have hit a pixel
	int xc=0;
	int yc=0;
	bool collided = false;
	collided = world->collision(ROUND(old_xpos),ROUND(old_ypos), ROUND(xpos), ROUND(ypos), xc, yc, 1);

 	if (collided) {
		Vector v;
		v.x = xspeed;
		v.y = yspeed;
		v = Math::normalize(v);
		int x = ROUND(xc + v.x);
		int y = ROUND(yc + v.y);

		if ((x >= 0) && (y >= 0) && (x < world->getSizeX()) && (y < world->getSizeY()) ) {
			xpos = xc;
			ypos = yc;
			hit_something(x,y, xc,yc);
		} else {
			bedone(); //if we have hit some something outside the map, just die.
		}
	};

	// make particles think
	if(!isdone()) {
		particle_think();
	}
}

//-----------------------------------------------------------------------------
// make particle think, e.g. check age and deletion
void Obj_particle::particle_think()
{
	// do nothing by default
}

//-----------------------------------------------------------------------------
void Obj_particle::draw(Viewport *viewport)
{
	int x = ROUND(xpos)-viewport->xpos;
	int y = ROUND(ypos)-viewport->ypos;

	if (x >= 0 && y >= 0 && x < viewport->get_xsize() && y < viewport->get_ysize()) {
		view_draw(viewport->get_pixels_write()+x+y*viewport->get_xsize());
	} else {
		// make sure the particles dont get deleted only by the view_draw function!
	}
}


//=============================================================================
//= Bloodloss in a bathroom stall, scartissue that I wish you saw...
//=============================================================================
void Obj_particle_blood::view_draw(int32 *target)
{
	*target = color | PF_TYPE_EARTH;
}

void Obj_particle_blood::hit_something(int x,int y, int xc, int yc) //(int offset_after_hit, int offset_before_hit)
{
	//only stain non passable pixels.
	if (!world->isPassable(x,y)) {
		world->getMapBuffer()->setRawColor(x,y, color);
	}

	bedone();
}


//=============================================================================
//= Debris
//=============================================================================
void Obj_particle_debris::view_draw(int32 *target)
{
	*target = color | PF_TYPE_EARTH;
}

void Obj_particle_debris::hit_something(int x,int y, int xc, int yc) //(int offset_after_hit, int offset_before_hit)
{
	bogo_bounce();	

	if (age() > 1)
		done = true;
}

//=============================================================================
//= Railtrail
//=============================================================================
void Obj_particle_railtrail::view_draw(int32 *target)
{
	int alpha = ROUND(15-(15*age()/2));
	if (alpha > 15) alpha = 15;
	if (alpha <= 0) {
  	done = true;
	} else {
		int32 aa = PIXELA(0,0,0,alpha);

		// dont worry about this here if (!IS_PASSABLE(*target)) return;

  	int32 type = *target & PF_TYPE_BITS;
	
   	if (IS_OPAQUE(aa))
			*target = (*target & PF_RGB_BITS) | type;

		else if (IS_INVISIBLE(aa)) {
			//Don't modify the pixel at all
   	
		}	else {
    	//ARGH! ARGH! it's not opaque and it's not invisible, so I gotta do alpha now.
 	  	
			int a = alpha;
			int a1 = 16-a;
  		
			int r = (GETRED(color)*a   + GETRED(*target)*a1)   >> 4;
			int g = (GETGREEN(color)*a + GETGREEN(*target)*a1) >> 4;
			int b = (GETBLUE(color)*a  + GETBLUE(*target)*a1)  >> 4;
  		
			*target = PIXEL(r,g,b) | type;
		}
	}
}

//-----------------------------------------------------------------------------
void Obj_particle_railtrail::hit_something(int x,int y, int xc, int yc) //(int offset_after_hit, int offset_before_hit)
{
	done = true;
}

//-----------------------------------------------------------------------------
void Obj_particle_railtrail::particle_think()
{
	// check age
	if(age() > 2) {
		done = true; return;
	}

	// ?????????????????????
	/*
	// check map hit
	if (xpos >= 0 && ypos >= 0 && xpos < world->map_buffer.get_xsize() && ypos < world->map_buffer.get_ysize()) {
		const int32 *map_pixel = world->map_buffer.get_pixels_read()+ROUND(xpos)+ROUND(ypos)*world->map_buffer.get_xsize();
		if (!IS_PASSABLE(*map_pixel)) {
			done = true; return;
		}
	} else {
		done = true;
	}*/
}


//=============================================================================
//= Spark
//=============================================================================
//-----------------------------------------------------------------------------
void Obj_particle_spark::view_draw(int32 *target)
{
	float new_age = age();
	if (new_age != old_age) {
		float newTemp = mTemperature;
		newTemp /= 1+(new_age-old_age)/2;
		newTemp -= 1;
		if (newTemp < 0) newTemp = 0;
		// store, make sure new temp is smaller
		old_age = new_age;
		if(newTemp<mTemperature){ 
			mTemperature = newTemp;
		} else {
			if(mTemperature>0) mTemperature--;
		}
	}

	float red   = mTemperature;
	float green = red / 3;
	float blue  = red / 10;
	if (red > 255) {
		green += (red-255)/2;
		blue  += (red-255)/2;
  	red = 255;
	}

	if (green > 255) {
		blue  += (green-255)/2;
  	green = 255;
	}

	if (blue > 255) {
  	blue = 255;
	}

	color = PIXELA(ROUND(red),ROUND(green),ROUND(blue),15);	
	*target = color;
}

//-----------------------------------------------------------------------------
void Obj_particle_spark::hit_something(int x,int y, int xc, int yc) //(int offset_after_hit, int offset_before_hit)
{
	// let hot sparks split upon hit
	if((curtime > createtime )&&(mTemperature>0)) {
		int split = ROUND(((mTemperature/100)+(sqrt(SQR(xspeed)+SQR(yspeed))/1000))/100);
		// mTemperature ranges from 0-1000, max of 10 sparks
		split = rnd( 0, ROUND((mTemperature/1000.0)*10.0) );
		if (split) {
			//logmsg(lt_debug,"Split became %i in spark",split);
			for (int i=0;i<split;i++) {
				Clientworld *clientworld = (Clientworld *)world;

				Vector speed;
				speed.x = -xspeed;
				speed.y = -yspeed;
				speed.scale(0.7);
				speed.rotate(PI/2*rnd(-1000,1000)/1000.0);

				bool ok; // all checks for new spark ok?
				int newx, newy;
				newx = xc;
				newy = yc;
				if (! world->isPassable(newx,newy)) ok = false;
				else ok = true;
				
				if(!ok) {
					//logmsg(lt_debug,"Split deleted...");
					i=split; // stop creating splits
				} else {
					// place ok

					Obj_particle_spark *part = (Obj_particle_spark *)clientworld->newanonobject(ot_particle_spark);
					if (part) {
						part->init(newx, newy, speed.x, speed.y);
						part->setTemperature( mTemperature/2.0 );
						//logmsg(lt_debug,"New Split at %f,%f t %f", (newx),(newy),(mTemperature/2.0) );
					} else {
						i=split; // stop creating splits
					}	
				}
			}
			done = true;
			return;
		}
	} // spark splitting

	if (mTemperature <= 0) {
		// stain map
		if (!world->isPassable(x,y)) {
			world->getMapBuffer()->setRawColor(x,y, color);
		}

		done = true;
	} else {
		// just bounce along
		bogo_bounce();
	}
}

//-----------------------------------------------------------------------------
void Obj_particle_spark::particle_think()
{
	// check age
	if(age() > 15) {
		done = true; return;
	}

	// check map hit
	//if (xpos >= 0 && ypos >= 0 && xpos < world->map_buffer.get_xsize() && ypos < world->map_buffer.get_ysize()) {
		//const int32 *map_pixel = world->map_buffer.get_pixels_read()+ROUND(xpos)+ROUND(ypos)*world->map_buffer.get_xsize();
		//if (!IS_PASSABLE(*map_pixel)) {
			//done = true; return;
		//}
	//} else {
		//done = true;
	//}
}


//=============================================================================
//= Smoke
//=============================================================================
void Obj_particle_smoke::view_draw(int32 *target)
{
	// how to draw it?
	int alpha = ROUND(15-(15*age()/2));
	if (alpha > 15) alpha = 15;
	
	if (alpha <= 0) {
		// too old...
		done = true;
	} else {
		int32 aa = PIXELA(0,0,0,alpha);
		int32 type = *target & PF_TYPE_BITS;
	
		if (IS_OPAQUE(aa))
			*target = (*target & PF_RGB_BITS) | type;

		else if (IS_INVISIBLE(aa)) {
			//Don't modify the pixel at all
   	
		}	else {
    	//ARGH! ARGH! it's not opaque and it's not invisible, so I gotta do alpha now.
 	  	
			int a = alpha;
			int a1 = 16-alpha;
  		
			int r = (GETRED(color)*a   + GETRED(*target)*a1)   >> 4;
			int g = (GETGREEN(color)*a + GETGREEN(*target)*a1) >> 4;
			int b = (GETBLUE(color)*a  + GETBLUE(*target)*a1)  >> 4;
  		
			*target = PIXEL(r,g,b) | type;
		}
	}
}

//-----------------------------------------------------------------------------
void Obj_particle_smoke::hit_something(int x,int y, int xc, int yc) //(int offset_after_hit, int offset_before_hit)
{
	done = true;
}

//-----------------------------------------------------------------------------
void Obj_particle_smoke::particle_think()
{
	// check age
	if(age() > 2) {
		done = true; return;
	}

	// check map hit
	//if (xpos >= 0 && ypos >= 0 && xpos < world->map_buffer.get_xsize() && ypos < world->map_buffer.get_ysize()) {
		//const int32 *map_pixel = world->map_buffer.get_pixels_read()+ROUND(xpos)+ROUND(ypos)*world->map_buffer.get_xsize();
		//if (!IS_PASSABLE(*map_pixel)) {
			//done = true; return;
		//}
	//} else {
		//done = true;
	//}
}


