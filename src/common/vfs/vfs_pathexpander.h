/***************************************************************************
  vfs_pathexpander.h  -  VFS path expander.
  --------------------------------------------------------------------------
  begin                : 2004-08-03
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last change          : 2005-01-12
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Rename to vfs_pathexpander.h, add namespace,
                         better function descriptions, use exceptions, remove
                         duplicate version of expand_path.

  copyright            : (C) 2004-2005 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/

#ifndef _VFS_PATHEXPANDER_H_
#define _VFS_PATHEXPANDER_H_

#include <cstdlib>
#include <stdexcept>

namespace VFS
{

/**
 * \brief Exception thrown when expansion would overflow buffer.
 *
 * This exception class is thrown when expanding something would overflow
 * the target buffer.
 *
 */
class Expansion_overflows : public std::overflow_error
{
public:
/**
 * \brief Constructor taking message string.
 *
 * This constructor constructs exception, taking exception description string
 * directly as parameter.
 *
 * \param description The exception description.
 *
 * \exception std::bad_alloc Not enough memory to copy the exception 
 *	description.
 * 
 */
	Expansion_overflows(const std::string& description) 
		throw();
};

/**
 * \brief Exception thrown when path expansion hits unknown user.
 *
 * This exception class is thrown when expanding path that refers to home
 * directory of nonexistent user.
 *
 */
class Nonexistent_user : public std::range_error
{
public:
/**
 * \brief Constructor taking message string.
 *
 * This constructor constructs exception, taking exception description string
 * directly as parameter.
 *
 * \param description The exception description.
 *
 * \exception std::bad_alloc Not enough memory to copy the exception 
 *	description.
 * 
 */
	Nonexistent_user(const std::string& description) 
		throw();
};

/**
 * \brief Exception thrown when path expansion hits user with no home 
 *	directory.
 *
 * This exception class is thrown when expanding path that refers to home
 * directory of existent user, but that home directory can't be determined.
 *
 */
class No_home_directory : public std::range_error
{
public:
/**
 * \brief Constructor taking message string.
 *
 * This constructor constructs exception, taking exception description string
 * directly as parameter.
 *
 * \param description The exception description.
 *
 * \exception std::bad_alloc Not enough memory to copy the exception 
 *	description.
 * 
 */
	No_home_directory(const std::string& description) 
		throw();
};



/**
 * \brief Expand home-directory notations from path into full paths.
 *
 * This function takes paths possibly containing shell home-directory notation
 * ('~' and '~user') and expands it to full absolute path of file.
 *
 * \param expbuf Buffer to store the expansion to. The buffer should consist
 *	of at least expbufsize writable consequtive elements starting from
 *	offset 0.
 * \param expbufsize Size of the target buffer. Including space for terminating
 *	NUL.
 * \param orig The orignal path to expand.
 *
 * \exception Expansion_overflows Insufficent space in target buffer.
 * \exception Nonexistent_user No such user.
 * \exception No_home_directory User exists, but seems not to have home 
 *	directory.
 *
 */
void expand_path(char* expbuf, size_t expbufsize, const char* orig)
	throw();


}
#endif
