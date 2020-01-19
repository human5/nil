/***************************************************************************
  vfs_inplace_alterable.h  - Strings alterable in place
  --------------------------------------------------------------------------
  begin                : 2005-02-05
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2005-03-07
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add possiblity to do temporary alterables
                         and add const to some methods.

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _vfs_inplace_alterable_h_
#define _vfs_inplace_alterable_h_

#include "common/smartpointer/pointer.h"

namespace VFS
{
/**
 * \brief Helper struct that enables using smart pointers.
 *
 * This is helper struct that enables using smart pointers for tracking 
 * ownership of inplace strings.
 *
 */
struct Inplace_alterable_helper
{
	//! Helper constructor.
	Inplace_alterable_helper(const char* newstring) throw();
	//! Our string.
	const char* string;
	//! Finalization method, does nothing.
	void finalize() throw();
};

/**
 * \brief Inplace alterable strings.
 *
 * This class implements zero-copy strings that can be altered in place.
 *
 */
class Inplace_alterable
{
public:
/**
 * \brief Comparision operator '<'.
 *
 * This operator orders inplace alterables in lexical (ASCIIbetic) order.
 *
 * \param another The other inplace string to compare with.
 *
 * \return 1 if this string becomes before another in lexical order.
 *
 */
	bool operator< (const Inplace_alterable& another) const throw();

/**
 * \brief Comparision operator '=='.
 *
 * This operator compares two inplace alterable strings.
 *
 * \param another The other inplace string to compare with.
 *
 * \return 1 if this string is same as the another.
 *
 */
	bool operator== (const Inplace_alterable& another) const throw();

/**
 * \brief Construct new inplace alterable string.
 *
 * This constructor constructs new inplace alterable string.
 * 
 * \param newstring The string to use.
 * \param temporary If set, temporary is created. These temporaries only may
 *                  be compared against and destroyed. The construction can't
 *                  fail through.
 *
 * \throws std::bad_alloc Not enough memory to construct string.
 *
 */
	Inplace_alterable(const char* newstring, bool temporary = false)
		throw(std::bad_alloc);

/**
 * \brief Alter the inplace alterable.
 *
 * This method alters the inplace alterable.
 *
 * \param newstring The new string.
 *
 * \note Unless the new string is the same as the old one, the results can
 *	be unpredictable. 
 *
 */
	void alter(const char* newstring) throw();

/**
 * \brief Are the strings literally the same?
 *
 * This method compares two strings for identity (not just equality), as in
 * == vs strcmp.
 *
 * \param another The other inplace string to compare with.
 *
 * \return 1 if this string is same as the another.
 *
 */
	bool identity(const Inplace_alterable& another) const throw();

/**
 * \brief Get the actual string.
 *
 * This method returns the actual string used.
 *
 */
	const char* get_string() const throw();
private:
	//! Our string. 
	pointer::Strong<Inplace_alterable_helper> string;

	//! Our string (temporary)
	const char* string_temp;
};

}

#endif
