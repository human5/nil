/***************************************************************************
  update_queue.cpp  -  encapsulates the queue of updates going to the clients
  --------------------------------------------------------------------------
  begin                : 2000-02-13
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

/***************************************************************************
  Note: The reason for recycling the Serializers is that it is vastly faster
  (because the serializers have quite a large internal buffer that grows to
  fit the kinds of traffic that the serializer sees)
 ***************************************************************************/

#include "common/serializer/update_queue.h"
#include "common/serializer/serializer_reader.h"
#include "common/console/logmsg.h"
#include <list>
#include <map>

//! @author Flemming Frandsen
//! @brief An array of Serializer_reader
class Update_queue_recycler : public std::list<Serializer_reader*>{};

//! @author: Flemming Frandsen
//! @brief An array of Serializer_reader and int
class Update_queue_serializers : public std::map<int,Serializer_reader*>{};

Update_queue::Update_queue() {
	pthread_mutex_init(&queue_lock, NULL);
	latest_update_id = 0;
	recycler = new Update_queue_recycler;
	serializers = new Update_queue_serializers;
}

Update_queue::~Update_queue() {
	// delete the serializers in the recycler pool as well as the udate queue
	Update_queue_serializers::iterator s = serializers->begin();
	while (s != serializers->end()) {
		delete s->second;
		s++;
	}
	Update_queue_recycler::iterator r = recycler->begin();
	while (r != recycler->end()) {
		delete *r;
		r++;
	}
	// delete the containers and the lock
	delete recycler;
	delete serializers;
	pthread_mutex_destroy(&queue_lock);
}

//! Returns a used serializer or a new one, if there is no serializer in the recycle bin
Serializer_reader *Update_queue::new_reader() {
	if (recycler->empty()) {
		return new Serializer_reader;
	} else {
		Serializer_reader *recycled = recycler->front();
		recycler->pop_front();
		return recycled;
	}
}

//! This gives the queue a serializer to play with
void Update_queue::submit_update(Serializer_reader *newupdate) {
	pthread_mutex_lock(&queue_lock);
	serializers->insert(std::pair<int, Serializer_reader*>(++latest_update_id,newupdate));
	pthread_mutex_unlock(&queue_lock);
}

//! recycle all the serializers older than this id to the recycler
void Update_queue::recycle(int update_id) {
	pthread_mutex_lock(&queue_lock);
	Update_queue_serializers::iterator s = serializers->begin();
	while (s != serializers->end()) {
		if (s->first < update_id) {
			Update_queue_serializers::iterator doomed = s++;
			//make sure the recyled serializer is ready to go.
			doomed->second->clear();
			recycler->push_front(doomed->second);
			serializers->erase(doomed);
		} else {
			s++;
		}
	}
	pthread_mutex_unlock(&queue_lock);
}

//! Returns the update with the given id
Serializer_reader *Update_queue::get_update(int update_id) {
	Serializer_reader *update=NULL;

	pthread_mutex_lock(&queue_lock);
	Update_queue_serializers::iterator s = serializers->find(update_id);
	if (s != serializers->end())
		update = s->second;
	pthread_mutex_unlock(&queue_lock);

	if (!update)
		logmsg(lt_error, "A server_connection requested an update that didn't exist in the queue:%i (%i)", update_id, latest_update_id);

	return update;
}

