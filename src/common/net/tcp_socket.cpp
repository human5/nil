/***************************************************************************
  tcp_socket.cpp  -  TCP socket handling
  --------------------------------------------------------------------------
  begin                : 1999-11-17
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-07
  by                   : Nils Thuerey
  email                : n_t@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  last changed         : 2005-01-03
  by                   : Christoph Brill
  email                : egore@gmx.de
  changedescription    : API cleanup

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/net/tcp_socket.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/systemheaders.h"
#include "common/sexchanger.h"

#define COMPRESSED_SEND_FLAG (1<<31)
#define COMPRESSED_MINLENGTH 200
#define ERROR_NONE     0
#define ERROR_TIMEOUT -1
#define ERROR_SOCKET  -2


int recv_timeout(int socket, void *buf, int len, unsigned int timeout) {
	int bytes_left = len;
	int bytes_done = 0;
	unsigned int start_time = time_ms();

	// "endless" loop to recieve from the given socket into the given buffer (until we recieved everything)
	while (1) {
		int recv_result = recv(socket, ((char*)buf)+bytes_done, bytes_left, 0);

		if (recv_result < 0) {
			logmsg(lt_error,"recv(%i,((char*)%p)+%i,%i,0); returned %i (%s)", socket,buf,bytes_done,bytes_left,errno,strerror(errno));
			// This is in most cases a critical error, so stop the loop
			return ERROR_SOCKET;
		} else if (recv_result > 0) {
			bytes_done += recv_result;
			bytes_left -= recv_result;
			//If this was the last bit of data, return immediatly.
			if (bytes_left == 0)
				return ERROR_NONE;
			//reset the timer once we get a bit of data.
			start_time = time_ms();
		}
		//see if we have timed out
		if (time_elapsed(start_time, time_ms()) > timeout)
			return ERROR_TIMEOUT;
	}
}

int send_timeout(int socket, void *buf, int len, unsigned int timeout) {
	int bytes_left = len;
	int bytes_done = 0;
	unsigned int start_time = time_ms();

	// "endless" loop to send on the given socket into the given buffer (until we sent everything)
	while (1) {
		int send_result = send(socket, ((char*)buf)+bytes_done, bytes_left, 0);

		if (send_result < 0) {
			logmsg(lt_error,"send(%i,((char*)%p)+%i,%i,0); returned %i (%s)", socket,buf,bytes_done,bytes_left,errno,strerror(errno));
			// This is in most cases a critical error, so stop the loop
			return ERROR_SOCKET;
		} else if (send_result > 0) {
			bytes_done += send_result;
			bytes_left -= send_result;
			//If this was the last bit of data, return immediatly.
			if (bytes_left == 0)
				return ERROR_NONE;
			//reset the timer once we get a bit of data.
			start_time = time_ms();
		}
		//see if we have timed out
		if (time_elapsed(start_time,time_ms()) > timeout)
			return ERROR_TIMEOUT;
	}
}

int Tcp_socket::send_buf(void *buf, int len, unsigned int timeout) {
	int result = send_timeout(socket, buf, len, timeout);
	if (result == ERROR_TIMEOUT)
		logmsg(lt_error, "Timed out while writing to the socket.");
	else if (result == ERROR_SOCKET)
		logmsg(lt_error, "Error while writing to the socket: %i",result);
	else if (result < 0)
		logmsg(lt_error, "Unkown error: %i", result);
	// An error occured, let's stop
	if (result < 0)
		bedone = true;
	return result;
}

int Tcp_socket::recv_buf(void *buf, int len, unsigned int timeout) {
	int result = recv_timeout(socket, buf, len, timeout);
	if (result == ERROR_TIMEOUT)
		logmsg(lt_error,"Time out while reading from the socket.");
	else if (result == ERROR_SOCKET)
		logmsg(lt_error,"Error while reading from the socket: %i",result);
	else if (result < 0)
		logmsg(lt_error, "Unkown error: %i", result);
	// An error occured, let's stop
	if (result < 0)
		bedone = true;
	return result;
}

int Tcp_socket::send_ser(Serializer_reader *serializer_reader) {
	// switch serializer compression on/off
	bool compressSend = true;
	if(serializer_reader->get_length() < COMPRESSED_MINLENGTH)
		compressSend = false;
	if(compressSend) {
		//compress the data:
		unsigned int bufferSize = ROUND(serializer_reader->get_length()*1.01)+12; //read zlib for the extra space.
		char *serBuffer = (char *)malloc(bufferSize);
		// TODO check malloc
		unsigned long compressedSize = bufferSize;
		int zerror = compress(
				(unsigned char *)serBuffer,
				&compressedSize,
				(const unsigned char*)serializer_reader->get_buffer(),
				serializer_reader->get_length()
				);

		if (zerror == Z_OK) {
			//logmsg(lt_debug,"Sending %i instead of %i bytes",(compressedSize+8), (serializer_reader->get_length()+4) );
		} else {
			logmsg(lt_error,"Unable to compress the serializer, the error was %i",zerror);
			exit(1);
		}

		// set compressed on flag 
		// FIXME: COMPRESSED_SEND_FLAG: integer conversion resulted in a change of sign
		int net_length = (compressedSize| COMPRESSED_SEND_FLAG); 

		// send size of following buffer
		int error;
		error = send_int(net_length);
		if (error){ free(serBuffer); return error; }

		int uncompressedSize = serializer_reader->get_length();
		error = send_int(uncompressedSize);
		if (error){ free(serBuffer); return error; }

		//send the state data itself.
		error = send_buf(serBuffer,compressedSize);
		if (error){ free(serBuffer); return error; }
		free(serBuffer);
	} else {
		int net_length = serializer_reader->get_length();

		// send size of following buffer
		int error = send_int(net_length);
		if (error) return error;

		//send the state data itself.
		error = send_buf(serializer_reader->get_buffer(),serializer_reader->get_length());
		if (error) return error;
	}
	//logmsg(lt_debug,"Sent ser %i bytes", (serializer_reader->get_length()) ); //D8
	return 0;
}

int Tcp_socket::recv_ser(Serializer_writer *serializer_writer, int max_sane_size) {
	// is the received data compressed? (check highest bit)
	bool compressSend = false;

	int error, length;
	error = recv_int(length);
	if (error) return error;

	if(length & COMPRESSED_SEND_FLAG) {
		length &= (~COMPRESSED_SEND_FLAG);
		compressSend = true;
	}

	//check the length of the data for bogosity
	if (length < 0 || length > max_sane_size) {
		logmsg(lt_error, "In int Tcp_socket::recv(Serializer_writer) we got a length of %i, this is wrong, expect the world to end.", length);
		return -50;
	}

	if(compressSend) {
		int recUncompressedSize;
		error = recv_int(recUncompressedSize);
		if (error) return error;
		unsigned long uncompressedSize = recUncompressedSize;

		// allocate the memory buffer
		char *serBuffer = (char *)malloc(length);
		// TODO check malloc
		error = recv_buf(serBuffer, length);
		if (error){ free(serBuffer); return error; }
		
		//get the data itself
		char *destBuf = serializer_writer->setbuffer(uncompressedSize);

		int zerror = uncompress(
				(unsigned char *)destBuf,
				&uncompressedSize,
				(const unsigned char*)serBuffer,
				length
				);

		if (zerror == Z_OK) {
			//logmsg(lt_debug,"uncompressed the diff alright");
		}else{
			logmsg(lt_error,"Unable to uncompress the diff! the error was:%i",zerror);
			free(serBuffer);
			exit(1);
		}

		free(serBuffer);
	} else {
		//get the data itself
		error = recv_buf(serializer_writer->setbuffer(length),length);
		if (error) return error;
	}
	//logmsg(lt_debug,"Recv ser %i bytes", (serializer_writer->get_length()) ); //D8
	return 0;
}

int Tcp_socket::send_int(int value) {
	int net_value = getChangedByteOrder32(value);
	int error = send_buf(&net_value,sizeof(net_value));
	if (error)
		return error;
	return 0;
}

int Tcp_socket::recv_int(int &value) {
	int net_value = -1;
	int error = recv_buf(&net_value,sizeof(net_value));
	if (error)
		return error;
	value = getChangedByteOrder32(net_value);
	return 0;
}

int Tcp_socket::send_obj(Serializable *object) {
	//get the objects state
	reader.clear();
	object->serialize(&reader);
	return send_ser(&reader);
}

int Tcp_socket::recv_obj(Serializable *object) {
	//get the object state
	int error = recv_ser(&writer);
	if (error) return error;
	object->serialize(&writer);
	//check the serializer for error conditions.
	if (!writer.is_empty()) {
		logmsg(lt_error,"In Tcp_socket we got too many bytes for the obejct to eat, this is wrong, expect the world to end.");
		return -51;
	}
	if (writer.empty_read()) {
		logmsg(lt_error,"In Tcp_socket we got too few bytes for the obejct to eat, this is wrong, expect the world to end.");
		return -52;
	}
	return 0;
}

/*bool Tcp_socket::set_timeout(int value) {
	if (value <= 5000 || value > 0) {
		timeout = value;
		return true;
	} else {
		logmsg(lt_error, "Your timeout (%i) was less than 0 or more than 5000", value);
		return false;
	}
}*/
