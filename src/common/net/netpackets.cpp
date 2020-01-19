/***************************************************************************
  netpackets.cpp  -  Structs used for client/server communication
 ---------------------------------------------------------------------------
  begin                : 2004-01-04
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 2004 by Christoph Brill
  email                : egore@gmx.de

 ***************************************************************************/

#include "common/net/netpackets.h"

Net_server_signature::Net_server_signature() {
	strcpy(signature, PROTOCOL_SIGNATURE);
	strcpy(version, PROTOCOL_VERSION);
}

bool Net_server_signature::server_ok() {
	return strcmp(signature, PROTOCOL_SIGNATURE) == 0;
}

bool Net_server_signature::version_ok() {
	return strcmp(version, PROTOCOL_VERSION) == 0;
}

void Net_server_signature::serialize(Serializer *serializer) {
	serializer->rwstr(signature, sizeof(signature));
	serializer->rwstr(version, sizeof(version));
}


void Net_joinrequest::serialize(Serializer *serializer) {
	if(nplayers < 1) nplayers = 1;
	if(nplayers > 4) nplayers = 4;
	serializer->rw(nplayers);
	for(int i=0; i<nplayers; ++i) {
		serializer->rw(players[i].color);
		serializer->rwstr(players[i].name, sizeof(players[i].name));
		serializer->rw(players[i].team);
	}
}


void Net_joinreply::serialize(Serializer *serializer) {
	serializer->rw(map_xsize);
	serializer->rw(map_ysize);
	serializer->rw(randomseed);
	serializer->rw(zcompressed);
	serializer->rw(compressed_size);
	for(int i=0; i<4; ++i)
		serializer->rw(player_ids[i]);
}
