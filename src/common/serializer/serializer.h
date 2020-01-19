/***************************************************************************
  serializer.h  -  Header for NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2000-01-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-27
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2005-01-04
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : code and API cleanup

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "common/misc.h"
#include "common/nil_math.h"
#include "client/graphics/raw_surface.h"

//! headers identifying the next lump of data in a serializer transferred vai network
typedef enum Thought_type {
	//! The package is the state of an object, object_type, object_id, object data
	TT_OBJECT_STATE,
	//! delete an object
	TT_OBJECT_DELETE,
	//! The world state follows.
	TT_WORLD_STATE,
	//! Dead pixel to follow.
	TT_DEAD_PIXEL,
	//! new sound event
	TT_SOUND_START,
	//! delete a sound event
	TT_SOUND_STOP,
	//! used when a function returns error
	TT_ERROR,
	//! signal clients to quit the game
	TT_END_GAME
} TD_tought_type;

//! @author Flemming Frandsen
//! @brief The serializer
class Serializer {
public: 
	//! Empty constructor
	Serializer();
	//! Empty destructor (may be overridden)
	virtual ~Serializer();
	/*! return if we are reading or not (must be overridden)
	    @return Returns true if we are reading and false if we are writing */
	virtual bool is_reading() = 0;
	/*! Simple datatype: Sint8
	    @param value A Sint8 value that will be serialized */
	virtual void rw(Sint8 &value)=0;
	/*! Simple datatype: Sint16
	    @param value A Sint16 value that will be serialized */
	virtual void rw(Sint16 &value)=0;
	/*! Simple datatype: Sint32
	    @param value A Sint32 value that will be serialized */
	virtual void rw(Sint32 &value)=0;
	/*! Simple datatype: int32
	    @param value An int32 value that will be serialized */
	virtual void rw(int32 &value)=0;
	/*! Simple datatype: bool
	    @param value A bool value that will be serialized */
	virtual void rw(bool &value)=0;
	/*! Simple datatype: float
	    @param value A float value that will be serialized */
	virtual void rw(float &value)=0;
	/*! Simple datatype: double
	    @param value A double value that will be serialized */
	virtual void rw(double &value)=0;
	/*! Simple datatype: Vector
	    @param value A vector that will be serialized */
	virtual void rw(Vector &value)=0;
	/*! Complex datatype: str with max size
	    @param value A string that will be serialized
	    @param maxsize The maximum size of the string */
	virtual void rwstr(char *value, int maxsize)=0;
	/*! Complex datatype: blob with size
	    @param blob A blob that will be serialized
	    @param size The size of the blob */
	virtual void rwblob(char *blob, int size)=0;
	//! Print out a serialized object
	void dump();
	/*! Get the length of the buffer
	    @return Returns the length of the buffer */
	int get_length();
	//! ???
	virtual int get_pos()=0;
protected:
	//! The buffer used for serialization
	char *buffer;
	//! The size of the buffer used for serialization
	int buffer_size;
	//! The filled size of the buffer used for serialization
	int buffer_inuse;
};

#endif
