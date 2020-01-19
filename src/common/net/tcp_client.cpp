/***************************************************************************
  tcp_client.cpp  -  TCP handling for client
  --------------------------------------------------------------------------
  begin                : 1999-11-17
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

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/systemheaders.h"
#include "common/net/tcp_client.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include <stdio.h>

Tcp_client::Tcp_client() {
	socket = -1;
}

Tcp_client::~Tcp_client() {
	shutdown(socket, 2);
}

bool Tcp_client::isconnected() {
	return socket > 0;
}

int Tcp_client::connect(const char *server, unsigned short port) {
	//create the socket
	socket = ::socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
	if (socket <= 0) {
		return -1;
	}

	struct hostent *server_hostent = gethostbyname(server);
	if (!server_hostent) {
		logmsg(lt_error,"Unable to find server '%s'",server);
		return -2;
	}

	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = *(unsigned int *)server_hostent->h_addr_list[0];
	int connect_error = ::connect(socket, (sockaddr *) &serveraddr, sizeof(serveraddr));
	if (connect_error) {
		return -3;
	}
	return 0;
}
