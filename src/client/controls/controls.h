/***************************************************************************
  controls.h  -  Headerfile for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 1999-10-30
  by                   : Flemming Frandsen (dion@swamp.dk)

  last changed         : 2004-05-18
  by                   : Christoph Brill (egore@gmx.de)

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)
  changedescription    : added logmsg.h header.

  last changed         : 2005-01-03
  by                   : Christoph Brill (egore@gmx.de)
  changedescription    : directory move
                         remove code from header

  copyright            : (C) 1999 by Flemming Frandsen (dion@swamp.dk)

 ***************************************************************************/

#ifndef _NIL_CONTROLS_H_
#define _NIL_CONTROLS_H_

#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/systemheaders.h"
#include "common/nil_math.h"
#include "common/serializer/serializable.h"

/*! \author Flemming Frandsen
    \brief A class that is used to collect all the control information that must be sent to the server. */
class Controls : public Serializable {
public:
	/* This is all there is control wise, it's pretty effective, but it also means that
	   there can only be 8 static and 8 impulse controls */
	typedef enum {
		//!static flags:
		cf_walk           = 0x00000001,
		cf_shoot          = 0x00000002,
		cf_hook_in        = 0x00000004,
		cf_hook_out       = 0x00000008,
		cf_direction_east = 0x00000010,
		cf_jetpack        = 0x00000020,
		//!impulse flags
		cf_jump           = 0x00000100,
		cf_dig            = 0x00000200,
		cf_hook_fire      = 0x00000400,
		cf_hook_release   = 0x00000800,
		cf_die_horribly   = 0x00001000,
		//!special:
		cf_not_impulse    = 0x000000FF
	} Controlflags;

	/*! control flags
	    This is all there is control wise, it's pretty effective, but it also means that
	    there can only be 16 static and 16 impulse controls
	typedef enum {
		//!static flags:
		cf_walk           = 0x00000001,
		cf_shoot          = 0x00000002,
		cf_hook_in        = 0x00000004,
		cf_hook_out       = 0x00000008,
		cf_direction_east = 0x00000010,
		cf_stat_aux1      = 0x00000020,
		cf_stat_aux2      = 0x00000040,
		cf_stat_aux3      = 0x00000080,
		cf_stat_aux4      = 0x00000100,
		cf_stat_aux5      = 0x00000200,
		cf_stat_aux6      = 0x00000400,
		cf_stat_aux7      = 0x00000800,
		cf_stat_aux8      = 0x00001000,
		cf_stat_aux9      = 0x00002000,
		cf_stat_aux10     = 0x00004000,
		cf_stat_aux11     = 0x00008000,
		//impulse flags
		cf_jump           = 0x00010000,
		cf_dig            = 0x00020000,
		cf_hook_fire      = 0x00040000,
		cf_hook_release   = 0x00080000,
		cf_die_horribly   = 0x00100000,
		cf_imp_aux1       = 0x00200000,
		cf_imp_aux2       = 0x00400000,
		cf_imp_aux3       = 0x00800000,
		cf_imp_aux4       = 0x01000000,
		cf_imp_aux5       = 0x02000000,
		cf_imp_aux6       = 0x04000000,
		cf_imp_aux7       = 0x08000000,
		cf_imp_aux8       = 0x10000000,
		cf_imp_aux9       = 0x20000000,
		cf_imp_aux10      = 0x40000000,
		cf_imp_aux11      = 0x80000000,
		//special:
		cf_not_impulse    = 0x0000FFFF
	} Controlflags_old;
	 */
	//! constructor, def init
	Controls();
	/*! constructor, def init */
	virtual ~Controls() {};
	//! bit diddeling
	void set_flag(int flag, bool state=true);
	//! ???
	void clear_flag(int flag);
	//! ???
	bool get_flag(int flag);
	//! get the current elevation
	float get_elevation();
	//! set an elevation between MINELEVATION and MAXELEVATION
	void set_elevation(float elev);
	//! get the currently used weapons
	char get_weapon();
	//! set the given weapon
	void set_weapon(char weapon_);
	//! clear impulses
	void clear_impulses();
	//! clear all inputs (e.g. when dying)
	void clear_all();
	//! the client/server interface
	void serialize(Serializer *serializer);
	//! print the message to debug output
	void print(char *msg);
	//! set control owner
	void set_owner(char new_owner);
	//! get control owner
	char get_owner();
protected:
	// FIXME (should it be an object ID?)
	//! Who owns us?  This is a *player number*, not an object id.
	Sint8 owner;
	//! in which direction are we pointing?
	float elevation;
	//! selected weapon
	int weapon;
	//! player controls
	int controlflags;
};

#endif
