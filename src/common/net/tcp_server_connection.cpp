/***************************************************************************
  tcp_server_connection.cpp  -  TCP connection handling for server
  --------------------------------------------------------------------------
  begin                : 1999-11-13
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

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
  changedescription    : Ignore client address.
  
  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/net/tcp_server_connection.h"
#include "common/net/tcp_server.h"
#include "common/systemheaders.h"
#include "common/console/logmsg.h"
#include "common/world_objects.h"
#include "common/objects/obj_scoreboard.h"
#include "common/objects/obj_chatmessage.h"

//! This routine is called in the new thread:
void *start_server_connection(void *arg) {
	Tcp_server_connection * conn = ((Tcp_server_connection *)arg);
	conn->execute();
	if (conn->socket > 0) {
		shutdown(conn->socket,2);
		conn->socket = -1;
	}
	//mark the connection object for deletion 
	conn->done = true;
	return NULL;
}

Tcp_server_connection::Tcp_server_connection() {
	tcp_server = NULL;
	bedone = true;
	done = true;
	ack_update_id = -1;
	current_update_id = -1;
	nplayers=0;
	mSendQuitMsg = false;
}

Tcp_server_connection::~Tcp_server_connection() {
	// remove players
	for(int i=0; i<nplayers; ++i) {
		// remove players and scores
		// TODO: if this thread terminates after main() thread, the world is already gone -> SEGFAULT, main should wait
		players[i]->bedone();
		tcp_server->serverworld->getscoreboard()->removeScores( players[i]->getid() );
	}
	nplayers=0;

	//signal the thread that it ought to finish up now
	bedone = true;
	//wait for it to do so:
	while (!done) usleep(1000);

	if(!mSendQuitMsg) {
		//NT send quit message to clients, when not shutting down
		if(!request_quit) {
			Obj_chatmessage *sendmsg = dynamic_cast<Obj_chatmessage *>(tcp_server->serverworld->newobject(ot_chatmessage));
			char quittxt[Obj_chatmessage::MSGLENGTH];
			snprintf(quittxt, Obj_chatmessage::MSGLENGTH, "System: Player %s was kicked out of the game...", players[0]->get_player_name() );
			sendmsg->initmessage( quittxt, Obj_chatmessage::SYSMSGCOL );
			sendmsg->bedone();
			sendmsg->setdirty();
		}
		mSendQuitMsg = true;
	}
	logmsg(lt_debug, "Connection terminated.");
}

int Tcp_server_connection::accept(Tcp_server *tcp_server_, int server_socket) {
	//we cannot start a new thread on a connection that is not done.
	if (!done)
		return -1; 
	bedone = false;
	done = false;

	tcp_server = tcp_server_;
#ifndef _WIN32
	unsigned int client_addr_size;
#else
	// under windows this seems necessary
	int client_addr_size;
#endif
	client_addr_size = sizeof(client_addr);
	socket = ::accept(server_socket, NULL, 0);
	if(socket >= 0)
		cdebug << "accept(" << server_socket << ", NULL, 0) = "
			<< socket << "." << std::flush;
	else
		cerror << "accept(" << server_socket << ", NULL, 0) = "
			<< "<Error: " << strerror(errno) << ">." << std::flush;
		
	if (socket < 0) {
		done = true;
		return -2;
	}

	//let the client join in this thread
	//this could be moved into the connection new thread to get rid of the blocking
	if (!joinclient()) {
		done = true;
		return -4;
	}

	//If the server is multithreaded then create a thread, if not the server will poll us
	if (tcp_server->is_multithreaded()) {
		int therad_error = pthread_create(&threaddata, NULL, start_server_connection, this);
		if (therad_error) {
			done = true;
			return -3;
		}
	}

	return 0;
}

bool Tcp_server_connection::joinclient() {
	//start by configuring the socket:
	{
		int flag = 1;
		int opt_error = setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
		if (opt_error) {
			logmsg(lt_error,"Could not disable nagle on connection_socket");
			bedone = true;
			return false;
		}
	}

	// send the server signature
	Net_server_signature server_signature;
	if (send_obj(&server_signature) < 0) {
		logmsg(lt_error,"Couldn't send server signature");
		bedone = true;
		return false;
	}
	logmsg(lt_debug,"signature sent");

	// get client info
	Net_joinrequest joinrequest;
	if (recv_obj(&joinrequest) < 0) {
		logmsg(lt_error,"Couldn't recv joindata");
		bedone = true;
		return false;
	}
	logmsg(lt_debug,"Got joindata");
	
	// send data about map
	Net_joinreply joinreply;
	char *map_data=NULL;
	tcp_server->serverworld->handle_join_request(
			joinrequest,// input, comes from the client
			nplayers,   // output
			players,    // also output
			joinreply,  // output
			map_data    // output, must free()
	);

	if (send_obj(&joinreply) < 0) {
		logmsg(lt_error,"Couldn't send joinreply");
		bedone = true;
		free(map_data);
		return false;
	}
	logmsg(lt_debug,"sent joinreply");

	if (send_buf(map_data, joinreply.compressed_size) < 0) {
		logmsg(lt_error,"Couldn't send map_data");
		bedone = true;
		free(map_data);
		return false;
	}
	free(map_data);
	logmsg(lt_debug,"sent map %d", joinreply.compressed_size);

	//NT send all objects once to client before proceeding
	Serializer_reader serializer_reader;
	int countObjects=0;
	World_objects::iterator i = tcp_server->serverworld->getObjects()->begin();
	while (i != tcp_server->serverworld->getObjects()->end()) {
		serializer_reader.writer_int(TT_OBJECT_STATE);
		serializer_reader.writer_int(i->second->getid());
		serializer_reader.writer_int(i->second->gettype());
		i->second->serialize(&serializer_reader);

		//logmsg(lt_debug, "Playerjoin: sent id %d, type %d (%s) ",i->second->getid(), i->second->gettype(), OBJECT_NAMES[i->second->gettype()] );
		i++;
		countObjects++;
	}
	logmsg(lt_debug,"Playerjoin: sent %d objects... %d", countObjects, serializer_reader.get_length() );
	
	// and send them
	send_ser(&serializer_reader);

	// finish join
	int handshake=0;
	if (recv_int(handshake) < 0) {
		logmsg(lt_error,"Couldn't recv post-map handshake");
		bedone = true;
		return false;
	}

	logmsg(lt_debug,"got post-map handshake");

	//Make sure that the serverworld sends out all objects in it's next run.
	//? not necessary anymore?? logmsg(lt_debug, " Player joined, making all dirty...");
	//? not necessary anymore?? tcp_server->serverworld->make_all_dirty();

	ack_update_id = current_update_id = tcp_server->update_queue.get_latest_update_id();

	return true;
}

void Tcp_server_connection::poll() {
	// we should shut down...
	if (bedone) {
		if (!tcp_server->is_multithreaded()) {
			// if not multithreaded delete connection, (else wait for execute to end)
			done = true;
		}
		return;
	}

	fd_set readable;
	FD_ZERO(&readable);
	FD_SET(socket, &readable);

	fd_set writeable;
	FD_ZERO(&writeable);
	FD_SET(socket, &writeable);

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = tcp_server->is_multithreaded()?10000:2000;

	int retval = select(socket+1, &readable, &writeable, NULL, &tv);
	if (retval < 0) {
		logmsg(lt_error,"select() failed in Tcp_server_connection::poll() : %i",retval);
		kick();
		return;
	}

	//a set of controls are available, get it.
	if (FD_ISSET(socket,&readable)) {
		bool quit = false;
		int netstate = -1;
		recv_int(netstate);
		while (netstate != NETSTATE::DONE) {
			switch (netstate) {
				case NETSTATE::CONTROLS: {
					Controls tmp;
					if(recv_obj(&tmp)<0) {
						logmsg(lt_error, "Couldn't recv(&tmp) to get controls, dropping client");
						quit=true;
					}
					if(tmp.get_owner()<0 || tmp.get_owner()>=nplayers) {
						logmsg(lt_error, "Invalid player number in control data: %i", tmp.get_owner());
						quit=true;
					}
					players[(int)tmp.get_owner()]->controls=tmp;
					break;
				}
				case NETSTATE::CHATMSG: {
					Obj_chatmessage tmp(tcp_server->serverworld, -1);
					if(recv_obj(&tmp)<0) {
						logmsg(lt_error, "Couldn't recv(&tmp) to get chat message, dropping client");
						quit=true;
					}
					Obj_chatmessage *sendmsg = dynamic_cast<Obj_chatmessage *>(tcp_server->serverworld->newobject(ot_chatmessage));
					sendmsg->initmessage( tmp.get_message(), tmp.get_color() );
					sendmsg->bedone();
					sendmsg->setdirty();
					logmsg(lt_debug, "We received a message from a client: %s ",tmp.get_message() );
					break;
				}
				case NETSTATE::UPDATEID:
					recv_int(ack_update_id);
					break;
				case NETSTATE::DONE: 
					// update finished correctly
					break;
				case NETSTATE::QUIT:
					logmsg(lt_message,"Client politely requested to quit, going to let it.");
					quit = true;
					//NT send quit message to clients
					if(!request_quit) {
						Obj_chatmessage *sendmsg = dynamic_cast<Obj_chatmessage *>(tcp_server->serverworld->newobject(ot_chatmessage));
						char quittxt[Obj_chatmessage::MSGLENGTH];
						snprintf(quittxt, Obj_chatmessage::MSGLENGTH, "System: Looser %s was bored and left...", players[0]->get_player_name() );
						sendmsg->initmessage( quittxt, Obj_chatmessage::SYSMSGCOL );
						sendmsg->bedone();
						sendmsg->setdirty();
					}
					mSendQuitMsg = true;
					break;
				default:
					logmsg(lt_error,"Invalid netstate while recv()'in data from the client: %i",netstate);
					quit = true;
					return;
			} //switch (netstate) 
			netstate = -1;
			if(!quit)
				recv_int(netstate);
			if((quit) || (netstate == -1) ) {
				logmsg(lt_debug,"Player has been told to remove itself from the world.");
				kick();
				return;
			}
		} //while (netstate != NETSTATE::DONE) 
	}

	//the socket is ready for more data, let it have an update:
	if (FD_ISSET(socket,&writeable)) {
		int latest_update_id = tcp_server->update_queue.get_latest_update_id();
		while (current_update_id < latest_update_id) {
			send_int(NETSTATE::UPDATEID);
			send_int(current_update_id);
			send_int(NETSTATE::WORLDUPDATE);
			Serializer_reader *reader = tcp_server->update_queue.get_update(current_update_id);
			if (!reader) {
				logmsg(lt_error,"Asked for a reader (%i) and I didn't get one! latest was: %i",current_update_id,latest_update_id);
				kick();
				return;
			}
			if (send_ser(reader) < 0) {
				logmsg(lt_error,"Couldn't send(&reader)");
				kick();
				return;
			}
			current_update_id++;
			send_int(NETSTATE::DONE);
			} //while (current_update_id < latest_update_id) {
	} //if (FD_ISSET(socket,&writeable)) {
}

void Tcp_server_connection::execute() {
	logmsg(lt_debug,"server connection thread started");
	// FIXME: This should not be done this way!
	// A much better way would be to create an object that that we can block while waiting for
	// (I just don't know how yet)
	while (!bedone) {
		poll();
		usleep(1000);
	}
	logmsg(lt_debug,"Server connection thread ended");
	done = true;
}

void Tcp_server_connection::kick() {
	// just set our bedone to true
	terminate();
}

void Tcp_server_connection::terminate() {
	bedone = true;
}

bool Tcp_server_connection::is_done() {
	return done;
}

bool Tcp_server_connection::is_bedone() {
	return done;
}

int Tcp_server_connection::get_current_update_id() {
	return current_update_id;
}

int Tcp_server_connection::get_ack_update_id() {
	return ack_update_id;
}
