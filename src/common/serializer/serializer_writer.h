/***************************************************************************
  serializer_writer.h  -  Header for NEEDDESCRIPTION
  --------------------------------------------------------------------------
  begin                : 2000-01-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  last changed         : 2005-01-04
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : Cleanup

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef SERIALIZER_WRITER_H
#define SERIALIZER_WRITER_H

#include "common/serializer/serializer.h"
#include "common/nil_math.h"
#include "common/misc.h"

//! @author Flemming Frandsen
//! @brief The serializer writer
class Serializer_writer : public Serializer  {
public: 
	//! Constructor that initializes the buffer
	Serializer_writer();
	//! Desctructor that free's the buffer
	~Serializer_writer();
	/*! Get the type of this serializer
	    @return Returns false, since we are a writer */
	virtual bool is_reading();
	/*! Enlarge the buffer
	    @param newlength The new length of this buffer (must be bigger than the old size)
	    @return The new buffer */
	char *setbuffer(int newlength);
	/*! Used for looking ahead when interpreting a stream
	    @return Return the offset of the buffer of the next int or -1 on error */
	int next_int();
	/*! Compare an rint()'ed int with the expected value
	    @param expected The integer value that we expect
	    @return Returns true, if the expected value and the real value are equal */
	bool eat_int(int expected);
	//! Simple datatype: Sint8
	virtual void rw(Sint8 &value);
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
	/*! This is just syntactical sugar to avoid declaring an int variable for writing
	    @return Returns the rw'ed int */
	int reader_int();
	/*! This is just syntactical sugar to avoid declaring a float variable for writing
	    @return Returns the rw'ed float */
	float reader_float();
	//! Reset the pointer used to work with the buffer
	void rewind();
	/*! Check if the buffer is empty
	    @return Returns true, if the curmsg >= buffer_inuse */
	bool is_empty();
	/*! Get the status of the flag to determine read errors
	    @return Returns empty_read_error */
	bool empty_read();
	/*! Get the current offset of the pointer used to work with the buffer
	    @return Returns the offset */
	virtual int get_pos();

protected:
	//! ???
	inline bool get_bytes(char *data, int length);
	//! ???
	bool empty_read_error;
	//! offset to begining of current thought.
	int curmsg;
};

#endif
