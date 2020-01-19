/***************************************************************************
  nil_server.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-11-14
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/configuration/configuration.h"
#include "common/systemheaders.h"
#include "common/misc.h"
#include "server/serverworld.h"
#include "common/vfs/loader.h"
#include "common/net/tcp_server.h"
#include "common/console/logmsg.h"
#include "common/sound/soundsystem.h"
#include "common/console/console.h"
#include "common/objects/obj_settings.h"
#include "common/objects/obj_chatmessage.h"
#include <signal.h>

//! Time to wait after notification when shutting server down
#define TIME_QUITDELAY 1500

//! Time for the clients to exit "peacfully" after the TT_END_GAME message, when
//  TIME_QUITDELAY+TIME_FORCESHUTDOWN are over, the server will shut down anyway
#define TIME_FORCESHUTDOWN 10000

/*! main server program, starts game loop
    \param config A configuration object for the server */
void run_server(Configuration &config);

#ifndef _WIN32
//! create an own sig hup handler, to make a clean shutdown when running under linux
void mySigHup(int sig);
#endif
