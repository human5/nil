/***************************************************************************
  obj_particle.h  -  Header for the particle
 ---------------------------------------------------------------------------
  begin                : 1999-11-04
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_PARTICLE_H
#define OBJ_PARTICLE_H

#include "obj_base.h"

//! @author Flemming Frandsen
//! @brief A particle
//! These are a few of eveyones favorite particle
class Obj_particle : public Obj_base  {
public:
	//! ???
	Obj_particle(World *world_, int id);
	//! ???
	virtual ~Obj_particle();
	//! This is an anon object so it doesn't make sense to server_think() it
	virtual void server_think(){}
	//! ???
	void init(float x, float y, float xspeed_, float yspeed_);
	//! ???
	virtual void draw(Viewport *viewport);
	//! ???
	virtual void client_think();
	/*! collision has detected a hit at the collided pos (x,y), the last good
	    pos from collison xc,yc is also passed this is different for each
	    particle type virtual void hit_something(int x,int y, int xc, int yc) = 0; */
	virtual void view_draw(int32 *target) = 0;
	/*! Set the color
	    @param set The color that will be saved */
	void set_color(int32 set) { color = set; }
	//! make particle think, e.g. check age and deletion
	virtual void particle_think();
	//! ???
	virtual void hit_something(int x,int y, int xc, int yc) = 0;
protected:
	//! The color of this particle
	int32 color;
};

//! @author Flemming Frandsen
//! @brief A blood particle
class Obj_particle_blood : public Obj_particle  {
public:
	/*! Constructor setting blood color
	    @param world_ ???
	    @param id ??? */
	Obj_particle_blood(World *world_, int id) : Obj_particle(world_,id) {
		color = mkrndcolor(PIXEL(255, 40, 40),PIXEL(100, 0, 0));
	};
	//! ???
	virtual Object_type gettype() {return ot_particle_blood;}
	//! ???
	virtual void view_draw(int32 *target);
	//! ???
	virtual void hit_something(int x,int y, int xc, int yc);
};

//! @author Flemming Frandsen
//! @brief A debris particle
class Obj_particle_debris : public Obj_particle  {
public:
	//! Constructor setting debris color
	Obj_particle_debris(World *world_, int id) : Obj_particle(world_,id) {color = 0;}
	//! ???
	void setcolor(int32 newcolor) {color = newcolor & PF_RGB_BITS;}
	//! ???
	virtual Object_type gettype() {return ot_particle_debris;}
	//! ???
	virtual void view_draw(int32 *target);
	//! ???
	virtual void hit_something(int x,int y, int xc, int yc);
};

//! @author Flemming Frandsen
//! @brief A debris particle
class Obj_particle_railtrail : public Obj_particle  {
public:
	//! Constructor setting railtrail color
	Obj_particle_railtrail(World *world_, int id) : Obj_particle(world_,id) {
		yaccel = 0; //float
		color = PIXEL(0,0,255);
	}
	//! ???
	void setcolor(int32 newcolor) {color = newcolor & PF_RGB_BITS;}
	//! ???
	virtual Object_type gettype() {return ot_particle_railtrail;}
	//! ???
	virtual void view_draw(int32 *target);
	//! ???
	virtual void hit_something(int x,int y, int xc, int yc);
	//! ???
	virtual void particle_think();
};

//! @author Flemming Frandsen
//! @brief A spark particle
class Obj_particle_spark : public Obj_particle  {
public:
	//! ???
	Obj_particle_spark(World *world_, int id) : Obj_particle(world_,id) {
		old_age = 0;
		mTemperature = rnd(500,1000);
	}
	//! ???
	virtual Object_type gettype() {return ot_particle_spark;}
	//! ???
	virtual void view_draw(int32 *target);
	//! ???
	virtual void hit_something(int x,int y, int xc, int yc);
	//! ???
	virtual void particle_think();
	//! set temperature
	void setTemperature(float set){ mTemperature = set; }
private:
	//! ???
	float mTemperature;
	//! ???
	float old_age;
};

//! @author Flemming Frandsen
//! @brief A smoke particle
class Obj_particle_smoke : public Obj_particle  {
public:
	//! ???
	Obj_particle_smoke(World *world_, int id) : Obj_particle(world_,id) {
		yaccel = -0.5; //float
		color = PIXEL(100,100,100);
		air_resistance = DEFAULT_DRAG*5;
		has_split = false;
	}
	//! ???
	void setcolor(int32 newcolor) {color = newcolor & PF_RGB_BITS;}
	//! ???
	virtual Object_type gettype() {return ot_particle_smoke;}
	//! ???
	virtual void view_draw(int32 *target);
	//! ???
	virtual void hit_something(int x,int y, int xc, int yc);
	//! ???
	virtual void particle_think();
protected:
	//! ???
	bool has_split;
};

#endif
