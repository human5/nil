/***************************************************************************
  timer.h  -  Class to calculate a delta time
  --------------------------------------------------------------------------
  begin                : 2005-01-12
  by                   : Daniel Schneidereit ()

  last changed         : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : import into NiL

  last changed         : 2005-01-16
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : move timer functions from misc.cpp here

  copyright            : (C) 2005 by Daniel Schneidereit ()
                             2005 by Christoph Brill (egore@gmx.de)

 ***************************************************************************/

#ifndef _NIL_TIMER_H_
#define _NIL_TIMER_H_

void initFPS();
//! Calculates frames per second and changes fps and dt
void calcFPS();
unsigned int time_ms();
//! calcs the elapsed time between two time_ms()'es
unsigned int time_elapsed(unsigned int t_old, unsigned int t_new);
#ifdef _WIN32
LARGE_INTEGER time_freq();
#endif
extern float fps;
extern float dt;
#ifdef _WIN32
static LONGLONG lastFrame, freq;
#else
static struct timeval t2;
#endif

#endif
