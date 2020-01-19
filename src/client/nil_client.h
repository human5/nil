/***************************************************************************
  nil_client.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-11-15
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-05-16
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef NILCLIENT_H
#define NILCLIENT_H

class Configuration;
class Config_xy;
class Loader;

//! the main client program
void run_client(Configuration &config);

//! do some benchmarking of the drawing routines
//void perform_benchmark(SDL_Surface *screen , Config_xy *screen_size, Loader *loader);

#endif
