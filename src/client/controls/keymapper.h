/***************************************************************************
  keymapper.h  -  Header for NEEDDESCRIPTION
 ---------------------------------------------------------------------------
  begin                : 2000-02-11
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Include SDL headers via systemheaders.h

  last changed         : 2004-09-19
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Don't bother with the whole file without SDL.

  copyright            : (C) 2000 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef _NIL_EVTMAPPER_H_
#define _NIL_EVTMAPPER_H_

#ifndef WITHOUT_SDL

#include "common/configuration/configuration.h"
#include "common/systemheaders.h"
#include "client/event/event.h"

class Evtmapper_bindings;

/*! \author Flemming Frandsen, Christoph Brill
    \brief This class is the event mapper which uses the design pattern "observer" */
class Evtmapper : protected Configuration_observer {
public: 
	//! Constructor
	Evtmapper();
	//! Destructor
	virtual ~Evtmapper();
	/*! find the event
	    \param inputevent The event we are searching for
	    \return Returns true if the event was found */
	bool lookup(Nil_inputevent &inputevent);
	/*! initializer
	    \param config_ Handle to the configuration*/
	void init(Configuration *config_);
	/*! Get the current bindings
	    \return Returns the binding */
	Evtmapper_bindings *get_bindings();
	//! Call to say that the config has changed
	virtual void config_changed();
	/*! bind a commandfunction to a symbolic SDL keyname
	    \param function The commandfunction
	    \param sdlsym The symbolic SDL keyname */
	void bindkey(CMDFUNCTION::Function function, SDLKey sdlsym);
protected:
	//! ???
	void addplayerkeys(CMDFUNCTION::Function player);
	//! Event bindings
	Evtmapper_bindings *bindings;
};

#endif
#endif
