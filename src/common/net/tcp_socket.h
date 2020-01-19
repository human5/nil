/***************************************************************************
  tcp_socket.h  -  Header for TCP socket handling
  --------------------------------------------------------------------------
  begin                : 1999-11-17
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-07
  by                   : Nils Thuerey
  email                : n_t@gmx.de

  last changed         : 2005-01-03
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : API cleanup

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "common/serializer/serializable.h"
#include "common/serializer/serializer_reader.h"
#include "common/serializer/serializer_writer.h"

#define IP_TIMEOUT 5000

//! @author Flemming Frandsen
//! @brief This class is used for TCP/IP communication
class Tcp_socket {
protected:
	//! set this to the socket you connected.
	int socket;
	//! this is set to true if something fails
	bool bedone;
public:
	//! write a serializer
	int send_ser(Serializer_reader *serializer_reader);
	//! read a serializer
	int recv_ser(Serializer_writer *serializer_writer, int max_sane_size=0xffff);
	//! write a serializable object 
	int send_obj(Serializable *object);
	//! read a serializable object
	int recv_obj(Serializable *object);
	//! write a single int
	int send_int(int value);
	//! read a single int
	int recv_int(int &value);
	//! write a raw buffer
	int send_buf(void *buf, int len, unsigned int timeout_ = IP_TIMEOUT);
	//! read a raw buffer
	int recv_buf(void *buf, int len, unsigned int timeout_ = IP_TIMEOUT);

private:
	//! This is a member, because it's expensive to construct
	Serializer_writer writer;
	//! This is a member, because it's expensive to construct
	Serializer_reader reader;
};

#endif
