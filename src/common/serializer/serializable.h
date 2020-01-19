/***************************************************************************
  serializable.h  -  Header for NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2000-01-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2005-01-04
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : Fix comments

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "serializer.h"

//! @author Flemming Frandsen
//! @brief Empty interface used for serializable objects
class Serializable {
public:
	/*! A virtual function that has to be implemented by every serializable object
	    @param serializer The serializer used to serialize an object */
	virtual void serialize(Serializer *serializer) = 0;
	/*! Destructor */
	virtual ~Serializable() {};
};

#endif
