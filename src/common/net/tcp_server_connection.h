/***************************************************************************
  tcp_server_connection.h  -  Header for TCP connection handling for server
  --------------------------------------------------------------------------
  begin                : 1999-11-13
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2005-01-03
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : cleanup
  
  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef TCP_SERVER_CONNECTION_H
#define TCP_SERVER_CONNECTION_H

#include "common/misc.h"
#include "common/systemheaders.h"
#include "common/net/tcp_socket.h"
#include "common/objects/obj_player_data.h"

#include <list>

class Tcp_server;

//! @author Flemming Frandsen
//! @brief a tcp connection to one of the clients
class Tcp_server_connection : public Tcp_socket {
public:
	//! ???
	Tcp_server_connection();
	//! ???
	~Tcp_server_connection();
	/*! do basic receive operations (used by the thread and it should be
	    polled if we are running in single threaded mode) */
	void poll(); 
	//! accept connection and start new thread if necessary
	int accept(Tcp_server *tcp_server_, int server_socket);
	//! ???
	friend void *start_server_connection(void *arg);
	//! shutdown (be deleted by tcp_server or end execute)
	void terminate();
	//! is the end flag set?
	bool is_done();
	//! are we already trying to shut down?
	bool is_bedone();
	//! The update that this client is currently working with
	int get_current_update_id();
	//! ???
	int get_ack_update_id();
	//! Deletes all player objects and terminates the connection.
	void kick();

protected:
	//! this is here to be preserved through several calls to poll()
	// FIXME: ew. hardcoded to 4 players/connection.
	int nplayers;
	//! Array to store the player data for up to four players
	// FIXME: ew. hardcoded to 4 players/connection.
	Obj_player_data *players[4];
	//! ???
	int ack_update_id;
	//! ???
	int current_update_id;
	//! This handles the joining of a client
	bool joinclient();
	//! ???
	bool done;
	//! This is the routine that is run in the sepperate thread
	void execute();
	//! ???
	Tcp_server *tcp_server;
	//! ???
	pthread_t threaddata;
	//! ???
	sockaddr_in client_addr;
	//! text quit message for other clients already sent?
	bool mSendQuitMsg;
};

#endif
