/***************************************************************************
  tcp_client.h  -  Header for TCP handling for client
  --------------------------------------------------------------------------
  begin                : 1999-11-17
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2005-01-03
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : move code out of the header

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "tcp_socket.h"

//! @author Flemming Frandsen
//! @brief Class for TCP handling for client
class Tcp_client : public Tcp_socket {
public:
	//! Take care that socket is that to a reliable value
	Tcp_client();
	//! Kill the socket
	~Tcp_client();
	//! Check if we got an open socket
	bool isconnected();
	//! Create a stream socket to the given server on the given port
	int connect(const char *server, unsigned short port);
};

#endif
