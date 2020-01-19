/***************************************************************************
  tcp_server.cpp  -  TCP handling for server
  --------------------------------------------------------------------------
  begin                : 1999-11-13
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-02-05
  by                   : H.Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : If tcp_server_connection::accept() returns -2
                         (accept failed), force us out of accept loop in
                         order to cut runaway accept loops.
                         Release tcp_server_connection objects on failed
                         accept.

  last changed         : 2004-07-15
  by                   : H.Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Cache tcp_server_connection objects that have failed
                         to accept the connection. Previously it tried to 
                         free the object, sometimes with bad results...
                         Also default the protocol to be 0 if protocols 
                         database doesn't have entry for TCP.

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.
  
  last changed         : 2005-01-03
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : cleanup

  last changed         : 2005-03-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Add debug print if accept fails.
  
  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/net/tcp_server.h"
#include "common/console/logmsg.h"
#include "common/systemheaders.h"

Tcp_server::Tcp_server() {
	server_socket        = 0;
	connections_inuse    = 0;
	net_normal_backlog   = 4;    // Everything is ok as long as you don't get further behind than this
	net_abnormal_backlog = 20;   // Kick clients that are this far behind.
	net_max_wait_time    = 200;  // the number of deca ms we wait for all clients to catch up.
}

void Tcp_server::set_fudge(int normal_backlog, int abnormal_backlog, int max_wait_time) {
	net_normal_backlog   = normal_backlog;
	net_abnormal_backlog = abnormal_backlog;
	net_max_wait_time    = max_wait_time;
}

Tcp_server::~Tcp_server() {
	for (int c=0;c<connections_inuse;c++)
		delete connections[c];
	if (server_socket)
		shutdown(server_socket,0);
}

int Tcp_server::bind_and_listen(int port, bool multithreaded_) {
	multithreaded = multithreaded_;

	//Fetch the protocol number. If we can't find it, default to 0, hoping that
	//kernel knows only 1 valid stream protocol on IPv4.
	int proto = 0;
	struct protoent* tcp = getprotobyname("tcp");
	if(tcp)
		proto = tcp->p_proto;

	//create the socket
	server_socket = socket(AF_INET, SOCK_STREAM, proto);
	if (server_socket <= 0)
		return -1;

	//bind to an addy
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	int bind_error = bind(server_socket, (sockaddr *) &serveraddr, sizeof(serveraddr));
	if (bind_error)
		return -2;

	//start listening for messages:
	int listen_error = listen(server_socket, 10);
	if (listen_error)
		return -3;

	//disable nagle:
	int flag = 1;
	int opt_error = setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	if (opt_error)
		return -4;

	return 0;
}

void Tcp_server::poll() {
	//clean up after the connections that have died
	int put = 0;
	for (int c=0;c<connections_inuse;c++)
		if (connections[c]->is_done()) {
			delete connections[c];
		} else {
			connections[put] = connections[c];
			put++;
		}

	connections_inuse = put;

	//keep accepting connections until there are no more in the queue
	bool connection_pending = false;
	do {
		static Tcp_server_connection* cached_connection = NULL;
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(server_socket, &rfds);

		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		int retval = select(server_socket+1, &rfds, NULL, NULL, &tv);
		connection_pending = (retval > 0) && FD_ISSET(server_socket,&rfds);

		if (connection_pending) {
			Tcp_server_connection *newconnection;

			logmsg(lt_debug,"going to accept connection");
			
			if(cached_connection)
			{
				newconnection = cached_connection;
				cached_connection = NULL;
			}
			else
				newconnection = new Tcp_server_connection;

			int new_error = newconnection->accept(this,server_socket);
			//If accept returns -2, force us out of loop to avoid runaway
			//accept loops. Also cache object on failure.
			if(new_error < 0)
				cached_connection = newconnection;
			if(new_error == -2) {
				cerror << "Error accepting connection: "
					<< strerror(errno) 
					<< " On FD=" << server_socket << "."
					<< std::flush;
				break;
			} if(new_error == 0)
				connections[connections_inuse++] = newconnection;
		}

	} while (connection_pending);

	// attempt to slow down the server until it holds a sensible framerate.
	int wait_time = 0;
	int waitcount = 0;
	bool all_ready;
	do {
		all_ready = true;
		for (int c=0; c<connections_inuse; c++) {

			// poll manually if not multithreaded (which is usually enabled)
			if (!multithreaded)
				connections[c]->poll();
			int latest_id = update_queue.get_latest_update_id();
			int ack_id = connections[c]->get_ack_update_id();
			int client_backlog =  latest_id-ack_id;

			// check to see if the client is down with the groove
			if (client_backlog > net_normal_backlog)
				all_ready = false;

			// check to see if the client is totally out of whack
			if( (!connections[c]->is_bedone()) && (client_backlog > net_abnormal_backlog) ) {
				logmsg(lt_message,"A player (con. %d) fell too far behind (%i) and was kicked as a result. %i %i",c,client_backlog,ack_id,latest_id);
				connections[c]->kick();
				logmsg(lt_debug,"DEBUG test %d ", connections[c]->is_done() ); // DEBUG D7
			}
		} //for (int c=0;c<connections_inuse;c++) 

		if (!all_ready) {
			int ms = 5+waitcount++;
			usleep(ms*1000);
			wait_time += ms;
		}
	} while (!all_ready && wait_time < net_max_wait_time);
}

void Tcp_server::recycle_serializers() {
	int oldest_update_id = 1<<30;
	for (int i=0;i<connections_inuse;i++) {
		int current_update_id = connections[i]->get_current_update_id();
		if (current_update_id < oldest_update_id)
			oldest_update_id = current_update_id;
	}
	update_queue.recycle(oldest_update_id);
}

bool Tcp_server::is_multithreaded() {
	return multithreaded;
}


int Tcp_server::get_connections_inuse() {
	return connections_inuse;
}
