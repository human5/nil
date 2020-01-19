/***************************************************************************
  tcp_server.h  -  Header TCP handling for server
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
  changedescription    : minor cleanup
  
  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "common/net/tcp_server_connection.h"
#include "common/systemheaders.h"
#include "server/serverworld.h"
#include "common/serializer/update_queue.h"

#define MAX_CONNECTIONS 128

//! @author Flemming Frandsen
//! @brief This class is used for the server side of TCP/IP communication
class Tcp_server {
public:
	/*! Opens a port and begins to listen to it (initializes the server)
	    @param port the port we are listening to
	    @param multithreaded_ Used to determine if we run multithreaded
	    @return Returns the different errors or 0 if sucessful */
	int bind_and_listen(int port, bool multithreaded_);
	//! ???
	void poll();
	//! ???
	Tcp_server();
	//! ???
	~Tcp_server();
	//! ???
	bool is_multithreaded();
	/*! This should be called from time to time to find out what the oldest relevant update is and then
	    have the updates_queue recycle all serializers older than this. */
	void recycle_serializers();
	//! set timeouts etc.
	void set_fudge(int normal_backlog, int abnormal_backlog, int max_wait_time);
	//! returns no. of connections that are still in use
	int get_connections_inuse();
	//! Connections to the clients
	Tcp_server_connection *connections[MAX_CONNECTIONS];
	//! No. of connections in use
	int connections_inuse;
	//! The server world object
	Serverworld *serverworld;
	//! Queued updates to send to clients
	Update_queue update_queue;
protected:
	//! Everything is ok as long as you don't get further behind than this 
	int net_normal_backlog;
	//! Kick clients that are this far behind. 
	int net_abnormal_backlog; 
	//! the number of deca ms we wait for all clients to catch up. 
	int net_max_wait_time;
	//! Socket for the server to listen for client connections
	int server_socket;
	//! Is the server multithreaded?
	bool multithreaded;
};

#endif
