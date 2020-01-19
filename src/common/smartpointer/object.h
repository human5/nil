/***************************************************************************
  object.h  -  ???
  --------------------------------------------------------------------------
  begin                : 2005-01-12
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2005-01-13
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : Add NiL header

  last change          : 2005-01-14
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Improve documentation.

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _NIL_OBJECT_H_
#define _NIL_OBJECT_H_

#include <pthread.h>
#include <iostream>
#include <stdexcept>
#include <cassert>

/**
 * \def POINTER_LOCK_OBJECT
 *
 * \internal
 *
 * \brief Lock an object. 
 *
 * This macro grabs lock field 'lock' of some object. The
 * lock must be unlocked before grabbing (if NDEBUG is not set, this is 
 * actually checked). If _REENTRANT is not defined, this compiles to nothing
 * as locks are not needed in that case.
 *
 * \param X the object to lock (field 'lock').
 *
 * \author H. Ilari Liusvaara
 *
 */

/**
 * \def POINTER_UNLOCK_OBJECT
 *
 * \internal
 *
 * \brief Unlock an object. 
 *
 * This macro releases lock field 'lock' of some object. The
 * lock must be locked before grabbing (if NDEBUG is not set, this is 
 * actually checked). If _REENTRANT is not defined, this compiles to nothing
 * as locks are not needed in that case.
 *
 * \param X the object to unlock (field 'lock').
 *
 * \author H. Ilari Liusvaara
 *
 */

/**
 * \def POINTER_LOCK_INIT
 *
 * \internal
 *
 * \brief Initialize a lock.  
 *
 * This macro initializes field 'lock' of object to be a
 * valid lock.
 *
 * \param X the object to initialize lock for (field 'lock').
 *
 * \author H. Ilari Liusvaara
 *
 */

/**
 * \def POINTER_LOCK_DEINIT
 *
 * \internal
 *
 * \brief Deinitialize a lock. 
 *
 * This macro deinitializes field 'lock' of object. The
 * lock must not be taken by any thread on calling this macro.
 *
 * \param X the object to deinitialize lock for (field 'lock').
 *
 * \author H. Ilari Liusvaara
 *
 */

#ifdef _REENTRANT
#ifdef NDEBUG
#define POINTER_LOCK_OBJECT(X) do { pthread_mutex_lock(&(( X )->lock)); } \
	while(false)
#define POINTER_UNLOCK_OBJECT(X) do { pthread_mutex_unlock(&(( X )->lock)); \
	} while(false)
#define POINTER_LOCK_INIT(X) do { \
	pthread_mutex_init(&(( X )->lock), NULL); } while(false)
#define POINTER_LOCK_DEINIT(X) do { pthread_mutex_destroy(&(( X )->lock)); \
	} while(false)
#else
#define POINTER_LOCK_OBJECT(X) do { int x = \
	pthread_mutex_lock(&(( X )->lock)); assert(!x); } while(false)
#define POINTER_UNLOCK_OBJECT(X) do { int x = \
	pthread_mutex_unlock(&(( X )->lock)); assert(!x); } while(false)
#define POINTER_LOCK_INIT(X) do { pthread_mutexattr_t a; \
	pthread_mutexattr_init(&a); assert(!pthread_mutexattr_settype(&a, \
	PTHREAD_MUTEX_ERRORCHECK)); pthread_mutex_init(&(( X )->lock), &a); \
	pthread_mutexattr_destroy(&a); } while(false)
#define POINTER_LOCK_DEINIT(X) do { int x = \
	pthread_mutex_destroy(&(( X )->lock)); assert(!x); } while(false)
#endif
#else
#define POINTER_LOCK_OBJECT(X) do { } while(false)
#define POINTER_UNLOCK_OBJECT(X) do { } while(false)
#define POINTER_LOCK_INIT(X) do { } while(false)
#define POINTER_LOCK_DEINIT(X) do { } while(false)
#endif

/**
 * \brief Smart pointers.
 *
 * This namespace provodes classes relating to smart pointers.
 *
 */
namespace pointer
{
/**
 * \brief Object is already finalized. 
 *
 * This exception is thown when trying to grab
 * strong refrence to object that already has been finalized (that is, 
 * already has its strong reference count dropped to zero.)
 *
 * \author H. Ilari Liusvaara
 *
 */
class Already_finalized : public std::logic_error 
{
public:
/**
 * \internal 
 *
 * \brief Construct new exception object. 
 *
 * This constructor constructs new exception object with preset string.
 * 
 * \param e The error message.
 *
 * \author H. Ilari Liusvaara
 *
 */
	Already_finalized(const std::string& e) throw() : logic_error(e) {};
};

/**
 *
 * \internal
 *
 * \brief Internal helper class of object system.
 *
 * This class tracks the references
 * to practicular object, and frees/finalizes it when apporiate.
 *
 * \note 
 *	Strong references pin the object strongly. When they run out, 
 *	finalize() from tracked object is invoked which can then get rid of
 *	most (but not necressarily all) of weak references to it. Some weak
 *	references can remain pointing to finalized object. When all 
 *	references are gone, the object will be freed. Note that both 
 *	finalize() and destructor are always called, even when last strong
 *	reference drops in absence of weak refrences.
 *
 * \author H. Ilari Liusvaara
 *
 */
template<typename T>
class Object
{
public:
/**
 * \brief Grab a strong reference.
 *
 * This function grabs strong reference to object
 * tracked.
 *
 * \return Pointer to object tracked.
 *
 * \throws Already_finalized Tracked object has already been finalized or is
 *	being finalized.
 *
 * \author H. Ilari Liusvaara
 *
 */
	T* grab_reference_strong() throw(Already_finalized)
	{
		POINTER_LOCK_OBJECT(this);
		if(this->finalized || this->finalizing)
		{
			POINTER_UNLOCK_OBJECT(this);
			throw Already_finalized("Object already finalized.");
		}
	
		++this->strong_count;

		POINTER_UNLOCK_OBJECT(this);
		return this->object;		
	}

/**
 * \brief Release a strong reference. 
 *
 * This function releases a strong reference to 
 * object tracked. If strong reference count reaches zero, finalize()-method of
 * object tracked is called. If after that no weak refences are left, 
 * destructor of tracked object is called and both tracked object and tracker 
 * are freed.
 *
 * \author H. Ilari Liusvaara
 *
 */
	void release_reference_strong() throw()
	{
		POINTER_LOCK_OBJECT(this);
		--this->strong_count;


		if(this->strong_count == 0)
		{
			//Finalize. Mark finalizing while locked, then unlock 
			//for duration of finalizer (to avoid races and 
			//deadlocks).
			this->finalizing = true;
			POINTER_UNLOCK_OBJECT(this);
			this->object->finalize();
			POINTER_LOCK_OBJECT(this);
			//Finalization complete.
			this->finalizing = false;
			this->finalized = true;
	
			if(this->weak_count == 0)
			{
				//No problems with resurrection as no pointers 
				//are left.
				delete this;
				return ;
			}
		}
		POINTER_UNLOCK_OBJECT(this);
	}

/**
 * \brief Grab a weak reference. 
 *
 * This function grabs weak reference to object
 * tracked.
 *
 * \return Pointer to object tracked.
 *
 * \author H. Ilari Liusvaara
 *
 */
	T* grab_reference_weak() throw()
	{
		POINTER_LOCK_OBJECT(this);

		++this->weak_count;

		POINTER_UNLOCK_OBJECT(this);
		return this->object;
	}

/**
 * \brief Release a weak reference.
 *
 * Release a weak reference. This function releases a weak reference to 
 * object tracked. If reference count reaches zero, finalize()-method of object
 * tracked is called. If after that no weak refences are left, destructor of
 * tracked object is called and both tracked object and tracker are freed.
 *
 * \author H. Ilari Liusvaara
 *
 */
	void release_reference_weak() throw()
	{
		POINTER_LOCK_OBJECT(this);

		--this->weak_count;
		if(this->weak_count == 0 && this->finalized)
		{
			//Get rid of this object.
			delete this;
			return;
		}
		  
		POINTER_UNLOCK_OBJECT(this);
	}	

/**
 * \brief Create tracker for object.
 *
 * This function creates tracker for given object.
 * Note that there should only be trackers for heap-allocated objects
 * (allocated with non-array new) and there should only be one tracker for
 * given object. The strong reference count of object is initialzed to 1 and
 * weak reference count to 0.
 *
 * \param object The object to track.
 *
 * \author H. Ilari Liusvaara
 *
 */
	Object(T* object)
	{
		this->object = object;
		POINTER_LOCK_INIT(this);
		this->strong_count = 1;
		this->weak_count = 0;
		this->finalizing = false;
		this->finalized = false;
	}

/**
 * \brief Destructor.
 *
 * This function frees tracker and object being tracked, invoking
 * destructor of tracked object. It must be called with object lock held.
 *
 * \author H. Ilari Liusvaara
 *
 */
	~Object()
	{
		POINTER_UNLOCK_OBJECT(this);
		POINTER_LOCK_DEINIT(this);
		delete this->object;
	}

private:
	///\brief Nonsense operation.
	Object(const Object<T>& object);

	///\brief Nonsense operation.
	Object<T>& operator= (const Object<T>& object);

	///\brief The object we are tracking.
	T* object;
#ifdef _REENTRANT
	///\brief Lock protecting reference counts and finalizer flags.
	pthread_mutex_t lock;
#endif
	///\brief Strong references existing to this object.
	unsigned strong_count;
	///\brief Weak references existing to this object.
	unsigned weak_count;
	///\brief Set when tracked object has been finalized.
	bool finalized;
	///\brief Set when tracked object is finalizing.
	bool finalizing;
};
}

#endif
