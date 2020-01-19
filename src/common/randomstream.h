/***************************************************************************
  randomstream.h  -  NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2003-05-11
  by                   : Nils Thuerey (n_t@gmx.de)

  last changed         : 2004-03-07
  by                   : Nils Thuerey (n_t@gmx.de)

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Make this selfstanding.

  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : API cleanup

  copyright            : (C) 2003 by Nils Thuerey (n_t@gmx.de)

 ***************************************************************************/

#ifndef _NIL_RANDOMSTREAM_H_
#define _NIL_RANDOMSTREAM_H_

#include "common/systemheaders.h"
#include "client/graphics/graphics.h"
#include "common/misc.h"

//! some big number
#define MBIG 1000000000
//! modify initial seed
#define MSEED 161803398
//! minimum value - no idea why this is a define?
#define MZ 0
//! for normalization to 0..1
#define FAC (1.0/MBIG)

/*! \author Nils Thuerey
    \brief a stream of random numbers using Knuth's portable method */
class Random_stream {
public:
	//! init random stream tables
	inline Random_stream(long seed);
	//! Destructor
	~Random_stream() {}
	/*! Get one random double value from the stream
	    \return The random double */
	inline double get_double();
	/*! Get one random integer value less than max
	    \param max The maximum number of integer
	    \return The random integer */
	inline int get_integer(int max);
	/*! return one random integer value between min and max
	    \param min The minumum number of the integer
	    \param max The maximum number of the integer
	    \return The random integer */
	inline int get_integer(int min, int max);
	/*! create a random color between the two given ones
	    \param color_min The minimum color
	    \param color_max The maximum color
	    \return Returns the random color*/
	inline Uint32 get_color(Uint32 color_min, Uint32 color_max);
private:
	//! random number state
	long idnum;
	//! pointers into number table
	int inext, inextp;
	//! store seed and number for subtraction
	unsigned long ma[56];
};


inline Random_stream::Random_stream(long seed) {
	idnum = seed;

	unsigned long mj = MSEED - (idnum < 0 ? -idnum : idnum);
	mj %= MBIG;
	ma[55] = mj;
	unsigned long mk = 1;

	// init table once, otherwise strange results...
	for(int i=0;i<=55;i++) ma[i] = (i*i+seed); 

	// init table in random order
	for(int i=1;i<=54;i++) {
		int ii = (21*i) % 56;
		ma[ii] = mk;
		mk = mj - mk;
		if(mk < MZ) mk += MBIG;
		mj = ma[ii];
	}

	// "warm up" generator
	for(int k=1;k<=4;k++) 
		for(int i=1;i<=55;i++) {
			ma[i] -= ma[1+ (i+30) % 55];
			if(ma[i] < MZ) ma[i] += MBIG;
		}

	inext = 0;
	inextp = 31; // the special "31"
	idnum = 1;
}


inline double Random_stream::get_double( void ) {
	if( ++inext == 56) inext = 1;
	if( ++inextp == 56) inextp = 1;

	// generate by subtraction
	unsigned long mj = ma[inext] - ma[inextp];

	// check range
	if(mj < MZ) mj += MBIG;
	ma[ inext ] = mj;
	return (double)(mj * FAC);
}

inline int Random_stream::get_integer( int max ) {
	if( ++inext == 56) inext = 1;
	if( ++inextp == 56) inextp = 1;

	// generate by subtraction
	unsigned long mj = ma[inext] - ma[inextp];

	// check range
	if(mj < MZ) mj += MBIG;
	ma[ inext ] = mj;
	return (mj % max);
}

inline int Random_stream::get_integer( int min, int max ) {
	int randmax;
	if (min > max) {
		randmax = (1+min-max);
		min = max;
	} else {
		randmax = (1+max-min);
	}

	if( ++inext == 56) inext = 1;
	if( ++inextp == 56) inextp = 1;

	// generate by subtraction
	unsigned long mj = ma[inext] - ma[inextp];

	// check range
	if(mj < MZ) mj += MBIG;
	ma[ inext ] = mj;
	return (min + (mj % randmax));
}

inline Uint32 Random_stream::get_color(Uint32 color_min, Uint32 color_max) {
	int r = rnd(GETRED(color_min), GETRED(color_max));
	int g = rnd(GETGREEN(color_min), GETGREEN(color_max));
	int b = rnd(GETBLUE(color_min), GETBLUE(color_max));

	return ((r & 0xff)<<16) + ((g & 0xff)<<8) + (b & 0xff);
}

#endif
