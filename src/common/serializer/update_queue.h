/***************************************************************************
  update_queue.h  -  Header for code that encapsulates the queue of updates
                     going to the clients
  --------------------------------------------------------------------------
  begin                : 2000-02-13
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

/***************************************************************************
  Note: The reason for recycling the Serializers is that it is vastly faster
  (because the serializers have quite a large internal buffer that grows to
  fit the kinds of traffic that the serializer sees)
 ***************************************************************************/

#ifndef UPDATE_QUEUE_H
#define UPDATE_QUEUE_H

#include "common/systemheaders.h"

class Update_queue_recycler;
class Update_queue_serializers;
class Serializer_reader;

//! @author Flemming Frandsen
//! @brief Used to update a serial queue
class Update_queue {
public:
	//! Constructor
	Update_queue();
	//! Destructor (delete the serializers in the recycler pool, the udate queue, the containers and the lock)
	~Update_queue();
	//! Returns a used serializer or a new one, if there is no serializer in the recycle bin
	Serializer_reader *new_reader();
	//! This hands a serializer full of data to the queue
	void submit_update(Serializer_reader *newupdate);
	/*! recycle all the serializers older than this id to the recycler
	    @param update_id The ID */
	void recycle(int update_id);
	/*! Get the ID of the last update (accessed from the client connection threads)
	    @return The ID of the latest update */
	int get_latest_update_id() { return latest_update_id; }
	/*! Get the update with the given id
	    @param update_id The ID of the update you want to have 
	    @return Returns the update with the given id */
	Serializer_reader *get_update(int update_id);

protected:
	//! the latest id that was updated
	int latest_update_id;
	//! our recyler
	Update_queue_recycler *recycler;
	//! our serializer
	Update_queue_serializers *serializers;
	//! the lock of our queue
	pthread_mutex_t queue_lock;
};

#endif
