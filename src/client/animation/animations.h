/***************************************************************************
  animation.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-09
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "client/animation/animator.h"
#include "client/animation/animation.h"

class Loader;
class Loaded_animations;

//! @author Flemming Frandsen
//! @brief Class for animations
class Animations {
public:
	//! Constructor
	Animations();
	//! Destructor that deletes the animation
	~Animations();
	/*! This is called by the loader to initialize the cache
	    @param loader_ Reference to a loader */
	void init(Loader *loader_);
	/*! This will either load an animation from disk or find it in the cache and then it will create a
	    new animator object from it. YOU MUST: Delete the Animator once you are done with it.
	    @param name The name
	    @param color The color */
	Animator *new_animator(char *name, int32 color = NOCOLOR);
protected:
	//! ???
	Loaded_animations *animations;
	//! ???
	Loader *loader;
};

#endif
