/***************************************************************************
  netpackets.h  -  Header for structs used for client/server communication
 ---------------------------------------------------------------------------
  begin                : 2000-01-16
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef _NETPACKETS_H_
#define _NETPACKETS_H_

#include "common/serializer/serializable.h"

//! FIXME: replace with "static const char[20] PROTOCOL_SIGNATURE = "NiL - The game"; ??
#define PROTOCOL_SIGNATURE "NiL - The game"
//! FIXME: replace with "static const char[20] PROTOCOL_VERSION = "000215+multiplex"; ??
#define PROTOCOL_VERSION "000215+multiplex"

//! name of a player, max length 40
typedef char Playername[40];

//! @author Flemming Frandsen
//! @brief signature sent by server right after a connection is accepted
class Net_server_signature : public Serializable {
public:
	/*! Destructor */
	virtual ~Net_server_signature() {};
	//! The signature
	char signature[20];
	//! The protocol version
	char version[20];
	//! Load the signature and the version
	Net_server_signature();
	/*! check if the server is running the same game
	    @return Returns true, if the signature is like the server signature */
	bool server_ok();
	/*! check if the server is running the same protocol version
	    @return Returns true, if the protocol is like the server signature */
	bool version_ok();
	/*! serialize signature and version
	    @param serializer A reference to the serializer */
	virtual void serialize(Serializer *serializer);
};

// The player info could be sent as a separate packet, which would provide
// more flexibility (players could, for instance, join and quit dynamically),
// but there seem to be race conditions that make that complicated.  Possible
// future extension.

//! @author Flemming Frandsen
//! @brief client info sent to server after the signature Hardcodes 4 as the maximum players/client!
class Net_joinrequest : public Serializable {
public:
	//! information about the player
	struct player_info {
		//! player color
		Uint32 color;
		//! player name
		Playername name;
		//! team-id
		Sint8 team;
	};
	//! number of local players
	Sint8 nplayers;
	//! ???
	player_info players[4];
	//! ???
	virtual void serialize(Serializer *serializer);
};

//! @author Flemming Frandsen
//! @brief server info for client about map, contains player data id's
class Net_joinreply : public Serializable {
public:
	/*! Destructor */
	virtual ~Net_joinreply() {};
	//! x-size of the map
	Sint16 map_xsize;
	//! x-size of the map
	Sint16 map_ysize;
	//! A random seed
	Uint32 randomseed;
	//! ???
	Sint8 zcompressed;
	//! number of bytes in the compressed image
	Uint32 compressed_size;
	//! ???
	Uint32 player_ids[4];
	//! ???
	virtual void serialize(Serializer *serializer);
};

//! This is used to tell the other end of socket what kind of data comes next
namespace NETSTATE {
	//! The next lump of data will be...
	enum {
		//! a serialized Controls object
		CONTROLS,
		//! the combined state of a world
		WORLDUPDATE,
		//! a raw int telling us what updateid is interesting in the context it was sent
		UPDATEID,
		//! client wants to send a chat message
		CHATMSG,
		//! Nothing, this set of lumps is done
		DONE,
		//! Nothing, terminate the connection _now_
		QUIT,
		//! *Bugger*
		NETERROR
	};
};

#endif
