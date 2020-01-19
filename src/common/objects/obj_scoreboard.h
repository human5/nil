/***************************************************************************
  obj_rocket.h  -  Header for the scoreboard
 ---------------------------------------------------------------------------
  begin                : 1999-12-26
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-30
  by                   : Christoph Brill
  email                : egore@gmx.de

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#ifndef OBJ_SCOREBOARD_H
#define OBJ_SCOREBOARD_H

#include "common/objects/obj_base.h"
#include "client/fonts/font_instance.h"

#define KEEP_FRAGS 10
#define KEEP_FRAG_TIME 8

//! @author Flemming Frandsen
//! @brief Class for the scoreboard
class Obj_scoreboard : public Obj_base  {
public:
	//! ???
	Obj_scoreboard(World *world_, int id);
	//! ???
	virtual ~Obj_scoreboard();
	//! ???
	virtual Object_type gettype() {return ot_scoreboard;}
	//! ???
	virtual void serialize(Serializer *serializer);
	//! ???
 	virtual void server_think();
	//! ???
	virtual void draw(Viewport *viewport){}
	//! ???
	virtual void client_think(){}
	//! add a scores struct for a new player
	void initFrag(int player_id);
	//! remove scores entries for leaving players
	void removeScores(int player_id);
	//! add a new frag to array (on server)
	void addfrag(int killer_id, int victim_id);
	//! clients - display next frag message (and remove from frags_inuse arrays), returns true if there is one
	bool get_fragmessage(char *msg);
	//! display (sorted) scores of all players
	void drawTable( Mutable_raw_surface *target, Font_instance *font );
	//! Getting the scores out again
	typedef struct Score {
		//! ???
		int player_id;
		//! ???
		Sint16 kills;
		//! ???
		Sint16 deaths;
		//! ???
		Sint16 frags;
		//! ???
		Sint8  team_id;
		//! ???
		float lastFragTime;
	} TD_score;
	//! return a score object from the array for given player
	Score getscore(int player_id);
	//! get rank of the given player (returns team rank in teamplay game mode)
	Sint8 getRank( Sint32 playerId );
	//! how many scores are used?
	int getscore_count() { return scores_inuse; }
	//! display scoreboard info as text
	void dump();
protected:
	//! get index of score for a given player id
	int get_index(int player_id);
	//! scores for the players (256 should be enough for a while...)
	Score scores[256];
	//! how much scores are used (=no of players)
	Sint16 scores_inuse;	
	//! remember last frag message (pseudo randomized with player id)
	int mFragCounter;
	//! remember last suicide message (pseudo randomized with player id)
	int mSuicideCounter;
	//! it's nice to keep in mind last few frags
	Sint16 frags_inuse;
	//! ???
	int last_killers [ KEEP_FRAGS ];
	//! ???
	int last_victims [ KEEP_FRAGS ];
	//float keep_frag_time [ KEEP_FRAGS ];
};

#endif
