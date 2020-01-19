/***************************************************************************
  vfs_inplace_alterable.cpp  - Strings alterable in place
  --------------------------------------------------------------------------
  begin                : 2005-02-05
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  last changed         : 2005-03-07
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : Add possiblity to do temporary alterables.

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include <cstring>

#include "common/vfs/vfs_inplace_alterable.h"
#include "common/console/console.h"

namespace VFS
{

void Inplace_alterable_helper::finalize() throw()
{
}

bool Inplace_alterable::operator< (const Inplace_alterable& another) const 
	throw()
{
	const char* s1 = this->string_temp;
	const char* s2 = another.string_temp;
	if(!s1)
		s1 = this->string->string;
	if(!s2)
		s2 = another.string->string;

	return (strcmp(s1, s2) < 0);
}

bool Inplace_alterable::operator== (const Inplace_alterable& another) const 
	throw()
{
	const char* s1 = this->string_temp;
	const char* s2 = another.string_temp;
	if(!s1)
		s1 = this->string->string;
	if(!s2)
		s2 = another.string->string;

	return (strcmp(s1, s2) == 0);
}

void Inplace_alterable::alter(const char* newstring) throw()
{
	this->string->string = newstring;
}

Inplace_alterable::Inplace_alterable(const char* newstring, bool temporary)
	throw()
{
	if(!temporary) {
		this->string = pointer::Strong<Inplace_alterable_helper>
			(new Inplace_alterable_helper(newstring));
		this->string_temp = NULL;
	} else
		this->string_temp = newstring;
}

Inplace_alterable_helper::Inplace_alterable_helper(const char* newstring) 
	throw()
{
	this->string = newstring;
}

bool Inplace_alterable::identity(const Inplace_alterable& another) const
	throw()
{
	return (this->string->string == another.string->string);
}

const char* Inplace_alterable::get_string() const throw()
{
	return this->string->string;
}

}
