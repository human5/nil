/***************************************************************************
  pointer.h  -  ???
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

  last change          : 2005-03-06
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Add missing returns.

  last change          : 2005-03-07
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Fix some differences to CVS version.

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _NIL_POINTER_H_
#define _NIL_POINTER_H_

#include "object.h"
#include <stdexcept>

namespace pointer
{

/**
 * \brief Pointer points to nowhere. 
 *
 * This exception is thrown if dereference of NULL
 * pointer is attempted.
 *
 * \author H. Ilari Liusvaara
 *
 */
class Null_dereference : public std::runtime_error
{
public:
/**
 * \internal 
 *
 * \brief Construct new exception object.
 *
 * This constructor constructs new
 * exception object with preset string.
 * 
 * \param e The error message.
 *
 * \author H. Ilari Liusvaara
 *
 */
	Null_dereference(const std::string& e) throw() : runtime_error(e) {};
};

//Predeclare this as we'll need it.
template<typename T>
class Weak;

/**
 * \brief Strong pointer class.
 *
 * This class provodes strong pointers that actually 
 * prevent object from being finalized. When all strong pointers to object
 * are gone finalizer() is run for that object. For object to be destroyed and
 * freed, there may be no strong or weak references to it. Once all strong 
 * references to some object have been dropped, no more can be established in
 * order to ensure well-defined state. 
 *
 * Strong pointer objects are not thread safe and access to them must be
 * mutexed. However, as long as operations involve diffrent smart pointer
 * objects, they are properly internally serialized. Thus, it is recommended
 * that diffrent threads do not share smart pointer objects.  
 *
 * Strong pointers are not signal-safe. Do not invoke strong pointer operations
 * with objects that may have smart pointer operations interrupted by executing
 * signal handler. Doing so may lead to to deadlock.
 *
 * Invoking smart pointer operations from callbacks (including to the object
 * callback is for) is safe thing to do.
 *
 * \author H. Ilari Liusvaara
 *
 */
template<typename T>
class Strong
{
///Weak and Strong pointer classes are friends.
friend class Weak<T>;
public:
/**
 * \brief Create new strong pointer to specified object.
 *
 * This constructor creates new
 * strong pointer making it point to specified object. Note that this function
 * should be only called for heap-allocated objects gotten from non-array new
 * and only once per object (special case is NULL for which unlimited amount
 * of pointer can be created).
 *
 * \param object The object to point it to, if NULL, the created pointer points
 *	nowhere (default: NULL).
 * \param free_on_error If wrapping object fails, should the object be freed?
 *	(default: yes).
 *
 * \throws std::bad_alloc Can't wrap object because insufficent memory.
 *
 * \author H. Ilari Liusvaara
 *
 */
	explicit Strong(T* object = NULL, bool free_on_error = true) 
		throw(std::bad_alloc)
	{
		this->tracker = NULL;
		this->object = NULL;

		if(object == NULL)
			return;

		try
		{
			this->tracker = new Object<T>(object);
		}
		catch(std::bad_alloc)
		{
			if(free_on_error)
				delete object;
			throw;
		}

		this->object = object;
	}

/**
 * \brief Destructor.
 *
 * Destroy a strong pointer and release reference to object it 
 * points to.
 *
 * \author H. Ilari Liusvaara
 *
 */
	~Strong() throw()
	{
		if(this->tracker)
			this->tracker->release_reference_strong();
	}

/**
 * \brief Check wheither object points somewhere.
 *
 * This function checks if strong 
 * pointer points to somewhere (as opposed to pointing nowhere, i.e. being
 * NULL).
 *
 * \return TRUE if pointer points to some valid object, FALSE if it is NULL.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	operator bool() const throw()
	{
		return (this->object != NULL);
	}

/**
 * \brief Check if two pointers point to same object. 
 *
 * This function checks if two
 * pointers point to same object. This function is not thread-safe as
 * the pointers can change underneath so mutex access to them if necressary.
 * 
 * \param pointer The another pointer (another being object for which this is
 *	called.
 *
 * \return TRUE if pointers point to same object, FALSE if they point to 
 *	diffrent objects.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	bool operator== (const Strong<T>& pointer) const throw()
	{
		return (this->object == pointer.object);
	}

/**
 * \brief Partial ordering of pointers.
 *
 * This function does total ordering on
 * pointers according to where they point. It checks if this pointer becomes
 * before another in lexical ordering. This function is not thread-safe as
 * the pointers can change underneath so mutex access to them if necressary.
 * Null pointers are considered to be lexically before any pointer pointing
 * somewhere.
 *
 * \param pointer Another pointer
 *
 * \return TRUE if this pointer becomes before another in lexical ordering of
 *	where they point. If operator== for pointers returns TRUE, this
 *	function will return FALSE.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	bool operator< (const Strong<T>& pointer) const throw()
	{
		return (this->object < pointer.object);
	}

/**
 * \brief Check if object pointed by pointer is castable to specified type. 
 *
 * This
 * function checks if object pointed by pointer is viewable as having type S,
 * by either directly having that type, or having some other type that is S.
 * Pointers to nowhere are regarded as being of any type.
 * 
 * \param dummy Ignored.
 *
 * \return TRUE if pointer points to nowhere or to object of specified type,
 *	FALSE otherwise.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	template<typename S>
	bool is_castable (S* dummy) const throw()
	{
		if(this->object == NULL)
			return true;

		try
		{
			return (dynamic_cast<S*>(this->object) != NULL);
		}
		catch(...)
		{
			return false;
		}
	}

/**
 * \brief Indirect dereference operator (constant version).
 *
 * \return constant reference to object pointed.
 *
 * \throws Null_dereference The pointer points to nowhere.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	const T& operator* () const throw(Null_dereference)
	{
		if(this->object == NULL)
			throw Null_dereference("Dereferencing pointer to "
				"nowhere");
	
		return *(this->object);
	}

/**
 * \brief Indirect dereference operator (nonconstant version).
 *
 * \return reference to object pointed.
 *
 * \throws Null_dereference The pointer points to nowhere.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	T& operator* () throw(Null_dereference)
	{
		if(this->object == NULL)
			throw Null_dereference("Dereferencing pointer to "
				"nowhere");
	
		return *(this->object);
	}

/**
 * \brief Indirect field-projection operator (constant version).
 *
 * \return constant pointer to object pointed.
 *
 * \throws Null_dereference The pointer points to nowhere.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	const T* operator-> () const throw(Null_dereference)
	{
		if(this->object == NULL)
			throw Null_dereference("Dereferencing pointer to "
				"nowhere");
	
		return this->object;
	}

/**
 * \brief Indirect field-projection operator (nonconstant version).
 *
 * \return pointer to object pointed.
 *
 * \throws Null_dereference The pointer points to nowhere.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	T* operator-> () throw(Null_dereference)
	{
		if(this->object == NULL)
			throw Null_dereference("Dereferencing pointer to "
				"nowhere");
	
		return this->object;
	}

/**
 * \brief Copy-constructor.
 *
 * This constructor copies the pointer, managing refererence 
 * counts apporiately. Make sure that poiter to be copied does not get fiddled
 * with during the copy.
 * 
 * \param pointer The pointer to copy.
 *
 * \throws Already_finalized Can't happen.
 *
 * \author H. Ilari Liusvara
 *
 */
	Strong(const Strong<T>& pointer) throw(Already_finalized)
	{
		if(pointer.object == NULL)
		{
			this->object = NULL;
			this->tracker = NULL;
		}
		else
		{
			this->object = pointer.tracker->
				grab_reference_strong();
			this->tracker = pointer.tracker;
		}
	}

/**
 * \brief Assingment.
 *
 * This method assigns the pointer, managing refererence 
 * counts apporiately. Don't fiddle with either side of assignment while it is
 * in progress.
 * 
 * \param pointer The pointer to assign.
 *
 * \return Reference to this.
 *
 * \throws Already_finalized Can't happen.
 *
 * \author H. Ilari Liusvara
 *
 */
	Strong<T>& operator=(const Strong<T>& pointer)
		throw(Already_finalized)
	{
		//Check for NOP.
		if(pointer.object == this->object)
			return *this;

		if(pointer.object == NULL)
		{
			if(this->tracker)
				this->tracker->release_reference_strong();
			this->object = NULL;
			this->tracker = NULL;
		}
		else
		{
			this->object = pointer.tracker->
				grab_reference_strong();
			if(this->tracker)
				this->tracker->release_reference_strong();
			this->tracker = pointer.tracker;
		}
		return *this;
	}

/**
 * \brief Nullfy a pointer.
 *
 * This method nullfies (i.e. makes NULL) the pointer it is
 * appiled to. It does nothing if pointer is already NULL.
 *
 * \author H. Ilari Liusvaara
 *
 */
	void nullify() throw()
	{
		if(this->object != NULL)
		{
			this->tracker->release_reference_strong();
			this->object = NULL;
			this->tracker = NULL;
		}
	}

	//
	//
	//Following are various cross-operators with weak pointers. We can't
	//define them yet as we don't know details of Weak. Declare
	//them inline so we can inline them. Their defintions come later in
	//this file.
	//
	//

/**
 * \brief Check if two pointers point to same object.
 *
 * This function checks if two
 * pointers point to same object. This function is not thread-safe as
 * the pointers can change underneath so mutex access to them if necressary.
 * 
 * \param pointer The another pointer (another being object for which this is
 *	called.
 *
 * \return TRUE if pointers point to same object, FALSE if they point to 
 *	diffrent objects.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	inline bool operator== (const Weak<T>& pointer) const throw();

/**
 * \brief Total ordering of pointers.
 *
 * This function does total ordering on
 * pointers according to where they point. It checks if this pointer becomes
 * before another in lexical ordering. This function is not thread-safe as
 * the pointers can change underneath so mutex access to them if necressary.
 * Null pointers are considered to be lexically before any pointer pointing
 * somewhere.
 *
 * \param pointer Another pointer
 *
 * \return TRUE if this pointer becomes before another in lexical ordering of
 *	where they point. If operator== for pointers returns TRUE, this
 *	function will return FALSE.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	inline bool operator< (const Weak<T>& pointer) const throw();

/**
 * \brief Copy-constructor.
 *
 * This constructor copies the pointer, managing refererence 
 * counts apporiately. Make sure that poiter to be copied does not get fiddled
 * with during the copy.
 * 
 * \param pointer The pointer to copy.
 *
 * \throws Already_finalized The object copied pointer points to has been 
 *	finalized.
 *
 * \author H. Ilari Liusvara
 *
 */
	inline Strong(const Weak<T>& pointer)
		throw(Already_finalized);

/**
 * \brief Assingment.
 *
 * This constructor assigns the pointer, managing refererence 
 * counts apporiately. Don't fiddle with either side of assignment while it is
 * in progress.
 * 
 * \param pointer The pointer to assign.
 *
 * \return Reference to this.
 *
 * \throws Already_finalized The object that is being assigned is finalized
 *	already.
 *
 * \author H. Ilari Liusvara
 *
 */
	inline Strong<T>& operator=(const Weak<T>& pointer) 
		throw(Already_finalized);
	

private:
	///\internal Our object.
	mutable T* object;
	///\internal Our tracker.
	mutable Object<T>* tracker;
};




/**
 * \brief Weak pointer class.
 *
 * This class provodes weak pointers that actually 
 * only prevent object from being destroyed. When all strong pointers to object
 * are gone finalizer() is run for that object. For object to be destroyed and
 * freed, there may be no strong or weak references to it. Also note that weak
 * pointer objects are not thread-safe and access to them must be mutexed. 
 * However, most interactions between strong and weak pointers are mutexed so 
 * they are thread safe unless otherwise noted.
 *
 * Weak pointer objects are not thread safe and access to them must be
 * mutexed. However, as long as operations involve diffrent smart pointer
 * objects, they are properly internally serialized. Thus, it is recommended
 * that diffrent threads do not share smart pointer objects.  
 *
 * Weak pointers are not signal-safe. Do not invoke strong pointer operations
 * with objects that may have smart pointer operations interrupted by executing
 * signal handler. Doing so may lead to to deadlock.
 *
 * Invoking smart pointer operations from callbacks (including to the object
 * callback is for) is safe thing to do.
 *
 * \author H. Ilari Liusvaara
 *
 */
template<typename T>
class Weak
{
///Weak and Strong pointer classes are friends.
friend class Strong<T>;
public:
/**
 * \brief Create new weak pointer to nowhere.
 *
 * This constructor creates new
 * weak pointer making it point nowhere. To get weak pointer to point 
 * somewhere, create strong pointer to that object and assign/copy-construct
 * from that.
 *
 * \author H. Ilari Liusvaara
 *
 */
	explicit Weak() throw()
	{
		this->tracker = NULL;
		this->object = NULL;
	}

/**
 * \brief Destructor.
 *
 * Destroy a weak pointer and release reference to object it 
 * points to.
 *
 * \author H. Ilari Liusvaara
 *
 */
	~Weak() throw()
	{
		if(this->tracker)
			this->tracker->release_reference_weak();
	}

/**
 * \brief Check wheither object points somewhere.
 *
 * This function checks if weak 
 * pointer points to somewhere (as opposed to pointing nowhere, i.e. being
 * NULL).
 *
 * \return TRUE if pointer points to some valid object, FALSE if it is NULL.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	operator bool() const throw()
	{
		return (this->object != NULL);
	}

/**
 * \brief Check if two pointers point to same object.
 *
 * This function checks if two
 * pointers point to same object. This function is not thread-safe as
 * the pointers can change underneath so mutex access to them if necressary.
 * 
 * \param pointer The another pointer (another being object for which this is
 *	called.
 *
 * \return TRUE if pointers point to same object, FALSE if they point to 
 *	diffrent objects.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	bool operator== (const Weak<T>& pointer) const throw()
	{
		return (this->object == pointer.object);
	}

/**
 * \brief Total ordering of pointers.
 *
 *  This function does total ordering on
 * pointers according to where they point. It checks if this pointer becomes
 * before another in lexical ordering. This function is not thread-safe as
 * the pointers can change underneath so mutex access to them if necressary.
 * Null pointers are considered to be lexically before any pointer pointing
 * somewhere.
 *
 * \param pointer Another pointer
 *
 * \return TRUE if this pointer becomes before another in lexical ordering of
 *	where they point. If operator== for pointers returns TRUE, this
 *	function will return FALSE.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	bool operator< (const Weak<T>& pointer) const throw()
	{
		return (this->object < pointer.object);
	}

/**
 * \brief Check if object pointed by pointer is castable to specified type. 
 *
 * This
 * function checks if object pointed by pointer is viewable as having type S,
 * by either directly having that type, or having some other type that is S.
 * Pointers to nowhere are regarded as being of any type.
 * 
 * \param dummy Ignored.
 *
 * \return TRUE if pointer points to nowhere or to object of specified type,
 *	FALSE otherwise.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	template<typename S>
	bool is_castable (S* dummy) const throw()
	{
		if(this->object == NULL)
			return true;

		try
		{
			return (dynamic_cast<S*>(this->object) != NULL);
		}
		catch(...)
		{
			return false;
		}
	}

/**
 * \brief Indirect dereference operator (constant version).
 *
 * \return constant reference to object pointed.
 *
 * \throws Null_dereference The pointer points to nowhere.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	const T& operator* () const throw(Null_dereference)
	{
		if(this->object == NULL)
			throw Null_dereference("Dereferencing pointer to "
				"nowhere");
	
		return *(this->object);
	}

/**
 * \brief Indirect dereference operator (nonconstant version).
 *
 * \return reference to object pointed.
 *
 * \throws Null_dereference The pointer points to nowhere.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	T& operator* () throw(Null_dereference)
	{
		if(this->object == NULL)
			throw Null_dereference("Dereferencing pointer to "
				"nowhere");
	
		return *(this->object);
	}

/**
 * \brief Indirect field-projection operator (constant version).
 *
 * \return constant pointer to object pointed.
 *
 * \throws Null_dereference The pointer points to nowhere.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	const T* operator-> () const throw(Null_dereference)
	{
		if(this->object == NULL)
			throw Null_dereference("Dereferencing pointer to "
				"nowhere");
	
		return this->object;
	}

/**
 * \brief Indirect field-projection operator (nonconstant version).
 *
 * \return pointer to object pointed.
 *
 * \throws Null_dereference The pointer points to nowhere.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	T* operator-> () throw(Null_dereference)
	{
		if(this->object == NULL)
			throw Null_dereference("Dereferencing pointer to "
				"nowhere");
	
		return this->object;
	}

/**
 * \brief Copy-constructor.
 *
 * This constructor copies the pointer, managing refererence 
 * counts apporiately. Make sure that poiter to be copied does not get fiddled
 * with during the copy.
 * 
 * \param pointer The pointer to copy.
 *
 * \author H. Ilari Liusvara
 *
 */
	Weak(const Weak<T>& pointer) throw()
	{
		if(pointer.object == NULL)
		{
			this->object = NULL;
			this->tracker = NULL;
		}
		else
		{
			this->object = pointer.tracker->
				grab_reference_weak();
			this->tracker = pointer.tracker;
		}
	}

/**
 * \brief Assingment.
 *
 * This constructor assigns the pointer, managing refererence 
 * counts apporiately. Don't fiddle with either side of assignment while it is
 * in progress.
 * 
 * \param pointer The pointer to assign.
 *
 * \return Reference to this.
 *
 * \author H. Ilari Liusvara
 *
 */
	Weak<T>& operator=(const Weak<T>& pointer) throw()
	{
		//Check for NOP.
		if(pointer.object == this->object)
			return *this;

		if(pointer.object == NULL)
		{
			if(this->tracker)
				this->tracker->release_reference_weak();
			this->object = NULL;
			this->tracker = NULL;
		}
		else
		{
			this->object = pointer.tracker->
				grab_reference_weak();
			if(this->tracker)
				this->tracker->release_reference_weak();
			this->tracker = pointer.tracker;
		}
		return *this;
	}

/**
 * \brief Check if two pointers point to same object.
 *
 * This function checks if two
 * pointers point to same object. This function is not thread-safe as
 * the pointers can change underneath so mutex access to them if necressary.
 * 
 * \param pointer The another pointer (another being object for which this is
 *	called.
 *
 * \return TRUE if pointers point to same object, FALSE if they point to 
 *	diffrent objects.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	bool operator== (const Strong<T>& pointer) const throw()
	{
		return (this->object == pointer.object);
	}

/**
 * \brief Total ordering of pointers.
 *
 * This function does total ordering on
 * pointers according to where they point. It checks if this pointer becomes
 * before another in lexical ordering. This function is not thread-safe as
 * the pointers can change underneath so mutex access to them if necressary.
 * Null pointers are considered to be lexically before any pointer pointing
 * somewhere.
 *
 * \param pointer Another pointer
 *
 * \return TRUE if this pointer becomes before another in lexical ordering of
 *	where they point. If operator== for pointers returns TRUE, this
 *	function will return FALSE.
 * 
 * \author H. Ilari Liusvaara
 *
 */
	bool operator< (const Strong<T>& pointer) const throw()
	{
		return (this->object < pointer.object);
	}

/**
 * \brief Copy-constructor.
 *
 * This constructor copies the pointer, managing refererence 
 * counts apporiately. Make sure that poiter to be copied does not get fiddled
 * with during the copy.
 * 
 * \param pointer The pointer to copy.
 *
 * \author H. Ilari Liusvara
 *
 */
	Weak(const Strong<T>& pointer) throw()
	{
		if(pointer.object == NULL)
		{
			this->object = NULL;
			this->tracker = NULL;
		}
		else
		{
			this->object = pointer.tracker->
				grab_reference_weak();
			this->tracker = pointer.tracker;
		}
	}

/**
 * \brief Assingment.
 *
 * This constructor assigns the pointer, managing refererence 
 * counts apporiately. Don't fiddle with either side of assignment while it is
 * in progress.
 * 
 * \param pointer The pointer to assign.
 *
 * \return Reference to this.
 *
 * \author H. Ilari Liusvara
 *
 */
	Weak<T>& operator=(const Strong<T>& pointer) throw()
	{
		//Check for NOP.
		if(pointer.object == this->object)
		{
			return *this;
		}

		if(pointer.object == NULL)
		{
			if(this->tracker)
				this->tracker->release_reference_weak();
			this->object = NULL;
			this->tracker = NULL;
		}
		else
		{
			this->object = pointer.tracker->
				grab_reference_weak();
			if(this->tracker)
				this->tracker->release_reference_weak();
			this->tracker = pointer.tracker;
		}	
		return *this;
	}

/**
 * \brief Nullfy a pointer.
 *
 * This method nullfies (i.e. makes NULL) the pointer it is
 * appiled to. It does nothing if pointer is already NULL.
 *
 * \author H. Ilari Liusvaara
 *
 */
	void nullify() throw()
	{
		if(this->object != NULL)
		{
			this->tracker->release_reference_weak();
			this->object = NULL;
			this->tracker = NULL;
		}
	}
	
private:
	///\internal \brief Our object.
	mutable T* object;
	///\internal \brief Our tracker.
	mutable Object<T>* tracker;
};

///////////////////////////////////////////////////////////////////////////////
template<typename T>
Strong<T>& Strong<T>::operator=(const Weak<T>& pointer) 
	throw(Already_finalized)
{
	//Check for NOP.
	if(pointer.object == this->object)
		return *this;

	if(pointer.object == NULL)
	{
		if(this->tracker)
			this->tracker->release_reference_strong();
		this->object = NULL;
		this->tracker = NULL;
	}
	else
	{
		this->object = pointer.tracker->
			grab_reference_strong();
		if(this->tracker)
			this->tracker->release_reference_strong();
		this->tracker = pointer.tracker;
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
template<typename T>
Strong<T>::Strong(const Weak<T>& pointer) throw(Already_finalized)
{
	if(pointer.object == NULL)
	{
		this->object = NULL;
		this->tracker = NULL;
	}
	else
	{
		this->object = pointer.tracker->
			grab_reference_strong();
		this->tracker = pointer.tracker;
	}
}

///////////////////////////////////////////////////////////////////////////////
template<typename T>
bool Strong<T>::operator== (const Weak<T>& pointer) const throw()
{
	return (this->object == pointer.object);
}

///////////////////////////////////////////////////////////////////////////////
template<typename T>
bool Strong<T>::operator< (const Weak<T>& pointer) const throw()
{
	return (this->object < pointer.object);
}

/**
 * \brief Downcast datatype within pointer.
 *
 * This function downcasts a pointer.
 *
 * \param pointer The pointer to cast.
 *
 * \return The casted pointer.
 *
 * \exception std::bad_cast Cast is bad.
 *
 * \author H. Ilari Liusvaara
 *
 */
template<typename S, typename T>
Strong<S> pointer_cast(Strong<T> pointer) throw(std::bad_cast)
{
	Strong<T> ptr = pointer;
	if(!ptr.is_castable((S*)NULL))
		throw std::bad_cast();
	return *(Strong<S>*)(&ptr);
}

/**
 * \brief Downcast datatype within weak pointer.
 *
 * This function downcasts a weak 
 * pointer.
 *
 * \param pointer The pointer to cast.
 *
 * \return The casted pointer.
 *
 * \exception std::bad_cast Cast is bad.
 *
 * \author H. Ilari Liusvaara
 *
 */
template<typename S, typename T>
Weak<S> pointer_cast(Weak<T> pointer) throw(std::bad_cast)
{
	Weak<T> ptr = pointer;
	if(!ptr.is_castable((S*)NULL))
		throw std::bad_cast();
	return *(Weak<S>*)(&ptr);
}

}

#endif
