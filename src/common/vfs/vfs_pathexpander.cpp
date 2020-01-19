/***************************************************************************
  loader_pathexpander.cpp  -  Loader path expander.
  --------------------------------------------------------------------------
  begin                : 2004-08-03
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  last change          : 2005-01-12
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Rename to vfs_pathexpander.cpp, add namespace,
                         use exceptions, remove duplicate version of 
                         expand_path. Use console framework directly.

  copyright            : (C) 2004-2005 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/

#include "common/vfs/vfs_pathexpander.h"
#include "common/misc.h"
#include "common/console/console.h"
#include <cstring>
#ifndef _WIN32
#include <pwd.h>
#include <sys/types.h>
#endif

namespace VFS
{

Expansion_overflows::Expansion_overflows(const std::string& description) 
		throw() : overflow_error(description) 
{
};

Nonexistent_user::Nonexistent_user(const std::string& description) 
		throw() : range_error(description) 
{
};

No_home_directory::No_home_directory(const std::string& description) 
		throw() : range_error(description) 
{
};


namespace 
{
/**
 *
 * \brief Do path expansion in case there is no user home directory 
 *	specification (i.e. just copy it if it fits).
 *
 * \param expbuf Same as in expand_path.
 * \param expbufsize Same as in expand_path.
 * \param orig Same as in expand_path.
 *
 * \exception Expansion_overflows The expansion overflows the target buffer.
 *
 */
void expand_path_nouser(char* expbuf, size_t expbufsize, const char* orig)
	throw()
{
	size_t origlen = strlen(orig);

	cdebug << "expand_path_nouser: Legth of original is " << origlen 
		<< " and expbufsize is " << expbufsize << ".\n";
	if(origlen + 1 > expbufsize) {
		//Does not fit.
		cdebug << "expand_path_nouser: Does not fit.\n";
		throw Expansion_overflows("Expanded path too large to fit to "
			"target buffer.");
	}
	cdebug << "expand_path_nouser: Copying path.\n";
	strcpy(expbuf, orig);
	return;
}

/**
 * \brief Return current user name.
 *
 * Return name of user the program runs for.
 *
 * \return Name of user, or NULL if it can't be determined.
 *
 */
char* current_user() throw()
{
	char* name = NULL;

	cdebug << "current_user: Obtaining name of current user.\n";

	//First try getlogin.
	cdebug << "current_user: Trying getlogin() to yield name of current "
		<< "user.\n";
	name = getlogin();
	if(name != NULL) {
		cdebug << "current_user: getlogin() yields \"" << name 
			<< "\".\n";
		return name;
	}

	cdebug << "current_user: No luck with getlogin() trying user "
		<< "database.\n";

	//Second, try users database lookup using current UID.
	struct passwd* user = getpwuid(getuid());
	if(user != NULL) {
		cdebug << "current_user: User database yields \"" 
			<< user->pw_name << "\".\n";
		return user->pw_name;
	}
	
	cdebug << "current_user: No luck.\n";

	//No luck.
	return NULL;
}

/**
 *
 * \brief Do path expansion in case home directory is known.
 *
 * This function expands the path in case needed home directory is known, and
 * user part of path ends with NUL or '/'.
 *
 * \param expbuf Same as in expand_path.
 * \param expbufsize Same as in expand_path.
 * \param orig Same as in expand_path.
 * \param homedir Home directory of user.
 *
 * \exception Expansion_overflows The expansion overflows the target buffer.
 *
 */
void expand_path_known_home(char* expbuf, size_t expbufsize, const char* orig,
	const char* homedir) throw()
{
	cdebug << "expand_path_known_home: Original path is \"" << orig 
		<< "\".\n";

	//Find beginning of actual path.
	while(*orig != '/' && *orig != '\0')
		orig++;

	cdebug << "expand_path_known_home: Trimmed path is \"" << orig 
		<< "\".\n";

	size_t origlen = strlen(orig);
	size_t homedirlen = strlen(homedir);

	cdebug << "expand_path_known_homedir: Legth of original is " 
		<< origlen << ", of homedir is " << homedirlen 
		<< " and expbufsize is " << expbufsize << ".\n";

	if(origlen + homedirlen + 1  > expbufsize) {
		//Does not fit.
		cdebug << "expand_path_known_home: Does not fit.\n";
		throw Expansion_overflows("Expanded path too large to fit to "
			"target buffer.");
	}

	cdebug << "expand_path_known_homedir: Making path.\n";
	strcpy(expbuf, homedir);
	strcat(expbuf, orig);
	cdebug << "expand_path_known_home: Path is \"" << expbuf << "\".\n";
	return;
}


}
void expand_path(char* expbuf, size_t expbufsize, const char* orig)
	throw()
{
#ifdef _WIN32
	//Right now, we can't do any more elaborate processing than this...
	cdebug << "expand_path: No home directory specification in \""
		<< orig << "\".\n";
	expand_path_nouser(expbuf, expbufsize, orig);
	cdebug << "Expand path: \"" << orig << "\" => \"" << expbuf
		<< "\".\n";
	return;

#else
	char* username = NULL;
	const char* username_ends;
	size_t username_length = 0;
	struct passwd* user;

	//Easy case. If first character is not '~', then just copy the path.
	if(*orig != '~') {
		cdebug << "expand_path: No home directory specification in \""
			<< orig << "\".\n";
		expand_path_nouser(expbuf, expbufsize, orig);
		cdebug << "Expand path: \"" << orig << "\" => \"" << expbuf
			<< "\".\n";
		return;
	}

	//Intermediate case. Path refers to current user.
	if(orig[1] == '/' || orig[1] == '\0') {
		//First, try to expand using $HOME.
		cdebug << "Expand_path: Expanding path refering to current "
			<< "user. Trying $HOME first.\n";
		const char* home = getenv("HOME");
		if(home == NULL) {
			//Need to do full-username case. Find user name of
			//current user.
			cdebug << "Expand_path: No $HOME set. Trying users "
				<< "database.\n";

			username = current_user();
			if(username != NULL) {
				cdebug << "Expand_path: current_user() "
					<< "yielded " << username << ".\n";
				goto use_user_name;
			}

			cdebug << "Expand_path: No $HOME and no known login "
				<< "name. Giving up.\n";

			//No home directory known.
			throw No_home_directory("Can't determine home "
				"directory of current user.");
		}
		cdebug << "Expand_path: $HOME is \"" << home << "\".\n";
		expand_path_known_home(expbuf, expbufsize, orig, home);
		cdebug << "Expand path: \"" << orig << "\" => \"" << expbuf
			<< "\".\n";
		return;
	}

	//Last case: User name specified in path.
	username_ends = strchr(orig + 1, '/');
	if(username_ends == NULL)
		username_ends = orig + strlen(orig);
	username_length = username_ends - orig;
	username = (char*)alloca(username_length);
	strlcpy(username, orig + 1, username_length);
	cdebug << "Expand_path: Username \"" << username << "\" extracted "
		<< "from specified path.\n";
use_user_name:
	cdebug << "Expand_path: Looking up \"" << username << "\" from users"
		<< "database.\n";
	user = getpwnam(username);
	if(user == NULL) {
		cdebug << "Expand_path: No such user \"" << username 
			<< "\" found from users database.\n";
		throw Nonexistent_user("No such user exists.");
	}
	if(user->pw_dir == NULL) {
		cdebug << "Expand_path: No homedirectory for user \"" 
			<< username << "\" found from users database.\n";
		throw No_home_directory("User homedirectory unknown.");
	}

	cdebug << "Expand_path: User home directory is \"" << user->pw_dir 
		<< "\".\n";
	expand_path_known_home(expbuf, expbufsize, orig, user->pw_dir);
	cdebug << "Expand path: \"" << orig << "\" => \"" << expbuf
		<< "\".\n";
	return;
#endif
}

}
