/***************************************************************************
  icon_base.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-01-27
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef ICON_BASE_H
#define ICON_BASE_H

#include "raw_surface.h"

//! @author Flemming Frandsen
//! @brief The icon class
class Icon_base : public Mutable_raw_surface {
public:
	//! Constructor
	Icon_base();
	//! Destructor
	virtual ~Icon_base();
	/*! draws the icon on a mutable raw surface
	    @param target The surface that will be drawn to */
	virtual void draw(Mutable_raw_surface *target) = 0;
};

#endif
