/***************************************************************************
  timer.cpp  -  Class to calculate a delta time
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

#include "common/console/logmsg.h"
#ifdef _WIN32
#include <windows.h>	// QueryPerformanceTimer()
#else
#include <time.h>	// gettimeofday()
#include <sys/time.h>
#endif

#include <stdio.h>
#include "timer.h"

float dt = 0.0f;
float fps = 0.0f;

void initFPS() {
#ifdef _WIN32
	QueryPerformanceCounter((LARGE_INTEGER*)&lastFrame);
	// get frequency of timer
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
#else
	gettimeofday(&t2, NULL);
#endif
}

void calcFPS() {
	static float acc_dt = 0.0f;
#ifdef _WIN32
	LONGLONG CurrentFrame;
	QueryPerformanceCounter((LARGE_INTEGER*)&CurrentFrame);
	dt = (float)(CurrentFrame - lastFrame) / (float)freq;
	lastFrame = CurrentFrame;
#else
	struct timeval t1;
	gettimeofday(&t1, NULL);
	dt = (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) / 1000000.0; // 1000000 microseconds in a second...
	gettimeofday(&t2, NULL);
#endif
	fps = 1 / dt;
	acc_dt += dt;
	// print every second the fps
	if (acc_dt >= 1.0f) {
		logmsg(lt_debug, "FPS: %.0f", fps);
	}
}

#ifdef _WIN32
LARGE_INTEGER time_freq() {
	LARGE_INTEGER liTimerFrequency;
	QueryPerformanceFrequency( &liTimerFrequency );
	return liTimerFrequency;
}
#endif

unsigned int time_ms() {
#ifdef _WIN32
	static LARGE_INTEGER liTimerFrequency = time_freq();
	LARGE_INTEGER liLastTime;
	QueryPerformanceCounter(&liLastTime);
	return (INT)( ((double)liLastTime.QuadPart / liTimerFrequency.QuadPart)*1000 );
#else
	struct timeval tv;
	struct timezone tz;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
	gettimeofday(&tv,&tz);
 	return (tv.tv_sec*1000)+(tv.tv_usec/1000);
#endif
}

unsigned int time_elapsed(unsigned int t_old, unsigned int t_new) {
	if (t_old > t_new) //we crossed midnight
		return (24*60*60*1000+t_new)-t_old;
	else
		return t_new-t_old;
}

