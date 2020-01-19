/***************************************************************************
  serializer_reader.h  -  Header for NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2000-01-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2005-01-04
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : Cleanup

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/


#ifndef SERIALIZER_READER_H
#define SERIALIZER_READER_H

#include "serializer.h"

//! @author Flemming Frandsen
//! @brief The serializer Reader
class Serializer_reader : public Serializer  {
public: 
	//! Constructor that initializes an empty buffer
	Serializer_reader();
	//! Destructor that free's the buffer
	virtual ~Serializer_reader();
	//! Reset buffer_inuse
	void clear();
	/*! Tell that this is a reading serializer
	    @return Returns true */
	virtual inline bool is_reading();
	//! getting thoughts out in one lump when done collecting them.
	char *get_buffer();
	/*! This is just syntactical sugar to avoid declaring an int variable for writing
	    @return Returns the rw'ed int */
	void writer_int(int value);
	/*! This is just syntactical sugar to avoid declaring a float variable for writing
	    @return Returns the rw'ed float */
	void writer_float(float value);
	//! Get the current offset
	virtual inline int get_pos();
	//! Simple datatype: Sint8
	virtual void rw(Sint8  &value);
	//! Simple datatype: Sint16
	virtual void rw(Sint16 &value);
	//! Simple datatype: Sint32
	virtual void rw(Sint32 &value);
	//! Simple datatype: int32
	virtual void rw(int32 &value);
	//! Simple datatype: bool
	virtual void rw(bool &value);
	//! Simple datatype: float
	virtual void rw(float &value);
	//! Simple datatype: double
	virtual void rw(double &value);
	//! Simple datatype: Vector
	virtual void rw(Vector &value);
	//! Complex datatype: str with maxsize
	virtual void rwstr(char *value, int maxsize);
	//! Complex datatype: blob with size
	virtual void rwblob(char *blob, int size);
protected:
	/*! Add something to the buffer
	    @param data The data added to the buffer
	    @param length The length of the data */
	inline void add_bytes(char *data, int length);
};

#endif
