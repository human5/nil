/***************************************************************************
  obj_scoreboard.cpp  -  the scoreboard
 ---------------------------------------------------------------------------
  begin                : 1999-12-26
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-03-28
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

  copyright            : (C) 1999 by Flemming Frandsen
  email                : dion@swamp.dk

 ***************************************************************************/

#include "common/objects/obj_scoreboard.h"
#include "common/objects/obj_player_data.h"
#include "common/objects/obj_settings.h"
#include "common/console/logmsg.h"

Obj_scoreboard::Obj_scoreboard(World *world_, int id) : Obj_base(world_, id) {
	scores_inuse = 0;
	frags_inuse = 0;
	mFragCounter = 0;
	mSuicideCounter = 0;
}

Obj_scoreboard::~Obj_scoreboard() {}

void Obj_scoreboard::serialize(Serializer *serializer) {
	serializer->rw(scores_inuse);
	for(int i=0; i<scores_inuse; i++) {
		serializer->rw(scores[i].player_id);
		serializer->rw(scores[i].kills);
		serializer->rw(scores[i].deaths);
		serializer->rw(scores[i].frags);
		serializer->rw(scores[i].lastFragTime);
	}
	
	serializer->rw(frags_inuse);
	for (int i=0; i<frags_inuse; ++i ) {
		serializer->rw(last_killers[i]);
		serializer->rw(last_victims[i]);
	}

	if(!world->isclient()) {
		frags_inuse = 0; // all sent...
	}
}

void Obj_scoreboard::server_think () {
	// NT - nothing to do here?
	return;
}

//! display (sorted) scores of all players
void Obj_scoreboard::drawTable( Mutable_raw_surface *target, Font_instance *font  ) {
	// center
	int width = 200; // approx. ?
	int fontheight = 14; // 10+4 
	int posx = target->get_xsize()/2 - width/2;
	int posy = target->get_ysize()/2 - scores_inuse*(fontheight/2);

	// team mode - first display team stats
	if(world->getServerSettings()->getGameMode() == Obj_settings::gm_teamplay) {
		// store team scores
		int teamFrags[TEAM_NUM];
		int teamKills[TEAM_NUM];
		int teamDeaths[TEAM_NUM];
		bool teamDisplayed[TEAM_NUM];
		bool teamActive[TEAM_NUM];
		int teamNum = 0;
		for(int k=0;k<TEAM_NUM;k++) {
			teamFrags[k] = teamKills[k] = teamDeaths[k] = 0;
			teamDisplayed[k] = teamActive[k] = false;
		}

		posy -= (teamNum+4)*(fontheight/2);
		font->put_text(posx+1, posy+1, "Team Stats:",target, true, 0,0,0 );
		font->put_text(posx  , posy  , "Team Stats:",target, true, 255,255,255 );
		posy += 14;

		// collect scores from players
		for(int k=0;k<scores_inuse;k++) {
			Obj_player_data *player = (Obj_player_data *)world->get_object(scores[k].player_id, ot_player_data);
			if(player) {
				int teamID = player->getTeam();
				if((teamID<0)||(teamID>=TEAM_NUM)) {
					logmsg(lt_warning,"Team score display - invalid team id %d for %d", teamID, scores[k].player_id);
				} else {
					// everything ok - add scores
					teamFrags[ teamID ] += scores[k].frags;
					teamKills[ teamID ] += scores[k].kills;
					teamDeaths[ teamID ] += scores[k].deaths;
					if(!teamActive[ teamID ]) {
						teamActive[ teamID ] = true;
						teamNum++;
					}
				}
			} else {
				logmsg(lt_warning,"Team score display - unable to get player data for %d", scores[k].player_id);
			}
		} 

		// display teams
		for(int k=0;k<teamNum;k++) {

			// get highest frag count
			int maxfrags = -1000000, index = -1;
			for(int i=0;i<4;i++) {
				if((teamFrags[i]>maxfrags)&&(!teamDisplayed[i])&&(teamActive[i])) {
					maxfrags = teamFrags[i];
					index = i;
				}
			}

			int i = index;
			if(i<0) { // this shouldnt happen
				logmsg(lt_warning,"drawTable - Team Negative  index %d ?!",i); 
				dump();
				return;
			} 
			char buf[256];
			int32 pcol = teamColorsMax[i];

			snprintf(buf, 256, "Frags: %3d    Kills: %3d   Deaths: %3d     %s", 
					teamFrags[i], teamKills[i], teamDeaths[i],
					teamNames[i] );

			int32 invpcol = makeBackgroundColor( pcol );
			int32 inv_color[3] = {GETRED(invpcol), GETGREEN(invpcol), GETBLUE(invpcol)};
			font->put_text(posx+1, posy+1, buf,target, true, inv_color[0],inv_color[1],inv_color[2] );

			int32 color[3] = {GETRED(pcol), GETGREEN(pcol), GETBLUE(pcol)};
			font->put_text(posx  , posy  , buf,target, true, color[0],color[1],color[2] );

			posy += fontheight;
			teamDisplayed[i] = true;
		}

		// space between team and player stats
		posy += 14;

	} // team mode

	// player stats header
	font->put_text(posx+1, posy+1, "Player Ranking:",target, true, 0,0,0 );
	font->put_text(posx  , posy  , "Player Ranking:",target, true, 255,255,255 );
	posy += 14;

	// display all scores using a primitive sorting
	bool displayed[scores_inuse];
	for(int i=0;i<scores_inuse;i++) displayed[i] = false;

	for(int k=0;k<scores_inuse;k++) {

		// get highest frag count
		int maxfrags = -1000000, index = -1;
		for(int i=0;i<scores_inuse;i++) {
			if((scores[i].frags>maxfrags)&&(!displayed[i])) {
				// FIXME consider time, ranks
				maxfrags = scores[i].frags;
				index = i;
			}
		}

		int i = index;
		if(i<0) { // this shouldnt happen
			i=0; 
			logmsg(lt_warning,"drawTable - Negative scorebaord index %d ?!",i); 
			dump(); 
			return;
		} 
		char buf[256];
		int32 pcol = PIXEL(255,255,255);

		Obj_player_data *player = (Obj_player_data *)world->get_object(scores[i].player_id,ot_player_data);
		if(player) {
			snprintf(buf, 256, "Frags: %3d    Kills: %3d   Deaths: %3d     %s", 
					scores[i].frags, scores[i].kills, scores[i].deaths,
					player->get_player_name() );
			pcol = player->getcolor();
		} else {
			snprintf(buf, 256, "Frags: %3d    Kills: %3d   Deaths: %3d     %s", 
					scores[i].frags, scores[i].kills, scores[i].deaths,
					"unknown" );
		}

		int32 invpcol = makeBackgroundColor( pcol );
		int32 inv_color[3] = {GETRED(invpcol), GETGREEN(invpcol), GETBLUE(invpcol)};
		font->put_text(posx+1, posy+1, buf,target, true, inv_color[0],inv_color[1],inv_color[2] );

		int32 color[3] = {GETRED(pcol), GETGREEN(pcol), GETBLUE(pcol)};
		font->put_text(posx  , posy  , buf,target, true, color[0],color[1],color[2] );

		posy += fontheight;
		displayed[i] = true;
	}

}

//! add a scores struct for a new player
void Obj_scoreboard::initFrag(int player_id) {
	logmsg(lt_debug,"scores init: %i", player_id);

	int i=get_index(player_id);
	if (i >= 0) {
		logmsg(lt_warning,"scores entry already existed, resetting...!?");
	} else {
		// create new entry
		scores[scores_inuse].player_id = player_id;
		scores[scores_inuse].kills = 0;
		scores[scores_inuse].deaths = 0;  	
		scores[scores_inuse].frags = 0;
		scores[scores_inuse].lastFragTime = 0;
		scores_inuse++;
		setdirty();
	}
}

//! delete scores entry
void Obj_scoreboard::removeScores(int player_id) {
	//logmsg(lt_debug,"removing scores for player %i", player_id);
	int i=get_index(player_id);
	if (i >= 0) {
		for(int l=i+1;l<scores_inuse;l++) {
			// move down other entries
			scores[l-1].player_id = scores[l].player_id;
			scores[l-1].kills     = scores[l].kills;
			scores[l-1].deaths    = scores[l].deaths;
			scores[l-1].frags     = scores[l].frags;
			scores[l-1].lastFragTime = scores[l].lastFragTime;
		}
		scores_inuse--;
		setdirty();
	} else {
		logmsg(lt_warning,"unable to find score entry!");
	}
}

//! add a new frag to array (on server)
void Obj_scoreboard::addfrag(int killer_id, int victim_id) {
	logmsg(lt_debug,"Added frag: %i %i",killer_id, victim_id);

	//First update the killer:
	//If this wasn't a guy killing himself, give the killer a frag
	{

		int frag = ((killer_id==victim_id)? -1:1);
		// team mode also has to handle team killings
		if((frag==1)&&(world->getServerSettings()->getGameMode() == Obj_settings::gm_teamplay)) {
			Obj_player_data *pk = (Obj_player_data *)world->get_object(killer_id, ot_player_data);
			Obj_player_data *pv = (Obj_player_data *)world->get_object(victim_id, ot_player_data);
			if((pk)&&(pv)) {
				if(pk->getTeam() == pv->getTeam()) {
					frag = -1; // team mate kill
			} }
		}

		int killerIndex = get_index(killer_id);
		if (killerIndex >= 0) {
			if ( killer_id != victim_id )
				scores[killerIndex].kills++;
			scores[killerIndex].frags += frag;
			scores[killerIndex].lastFragTime = world->gettime();
		} else {

			// add new entry
			logmsg(lt_warning,"new killer entry had to be created on the fly... shouldn't happen");
			scores[scores_inuse].player_id = killer_id;
			if ( killer_id != victim_id )
				scores[scores_inuse].kills = 1;
			else
				scores[scores_inuse].kills = 0;
			scores[scores_inuse].deaths = 0;
			scores[scores_inuse].frags = frag;
			scores[scores_inuse].lastFragTime = world->gettime();
			scores_inuse++;
		}
	}

	//Update the victim:
	{
		int i=get_index(victim_id);
		if (i >= 0) {
			scores[i].deaths++;
		} else {

			// add new entry
			logmsg(lt_warning,"new victim entry had to be created on the fly... shouldn't happen");
			scores[scores_inuse].player_id = victim_id;
			scores[scores_inuse].kills = 0;
			scores[scores_inuse].deaths = 1;  	
			scores[scores_inuse].frags = 0;
			scores[scores_inuse].lastFragTime = 0;
			scores_inuse++;
		}
	}

	// Update last few frags
	{
		if ( frags_inuse >= KEEP_FRAGS ) {
			for ( int i = 1; i < KEEP_FRAGS; ++i ) {
				last_killers[ i - 1 ] = last_killers[ i ];
				last_victims[ i - 1 ] = last_victims[ i ];
			}
    } else
			frags_inuse++;
    last_killers[ frags_inuse - 1 ] = killer_id;
    last_victims[ frags_inuse - 1 ] = victim_id;
  }
	setdirty();
}

//! return a score object from the array
Obj_scoreboard::Score Obj_scoreboard::getscore(int index) {
	int ind = get_index ( index );
	if (ind >= 0 && ind < scores_inuse) {
		return scores[ind];
	} else {
		Score s;
		s.player_id = -1;
		s.frags = 0;
		s.kills = 0;
		s.deaths = 0;
		return s;
	}
}

//! return score object for a player
int Obj_scoreboard::get_index(int player_id) {
	for(int i=0; i<scores_inuse; i++)
		if (scores[i].player_id == player_id)
			return i;
	return -1;
}

//! get rank of the given player (returns team rank in teamplay game mode)
Sint8 Obj_scoreboard::getRank(Sint32 playerId ) {
	if(world->getServerSettings()->getGameMode() == Obj_settings::gm_teamplay) {}

	int index = get_index( playerId );
	int rank = 1;
	if (index >= 0 && index < scores_inuse) {
		// scores ok
		for(int i=0;i<scores_inuse;i++)
			if((i!=index) && (scores[i].frags > scores[index].frags)) {
				// FIXME handle frag times
				rank++;
			}

		return rank;
	}
	
	// not found...
	return -1;
}

//! clients - display next frag message (and remove from frags_inuse arrays), returns true if there is one
bool Obj_scoreboard::get_fragmessage(char *msg) {
	// frag messages
	int fragPresetNum = 6;
	char *fragPreset[] = { 
		"%s fragged %s",
		"%s killed %s",
		"%s pulverized %s",
		"%s blew %s to smithereens",
		"%s shredded %s",
		"%s disposed %s"
	};

	// suicide messages
	int suicidePresetNum = 5;
	char *suicidePreset[] = { 
		"%s was tired of life",
		"%s commited suicide",
		"%s decided to start over",
		"%s made a stupid mistake",
		"%s went to hell"
	};

	// team play "suicide" messages
	int teamplayPresetNum = 4;
	char *teamplayPreset[] = { 
		"%s got rid of his teammate %s",
		"%s had coordination problems with %s",
		"%s friendly-fired %s",
		"%s did not get along with %s"
	};

	if ( frags_inuse > 0 ) {

		Obj_player_data *pk = (Obj_player_data *)world->get_object(last_killers[0], ot_player_data);
		Obj_player_data *pv = (Obj_player_data *)world->get_object(last_victims[0], ot_player_data);

		if(last_killers[0] != last_victims[0]) {

			// distuingish game modes
			if(world->getServerSettings()->getGameMode() == Obj_settings::gm_deathmatch) {
				// standar deathmatch frag
				mFragCounter = (mFragCounter+last_killers[0]+last_victims[0])%fragPresetNum;
				if((pk)&&(pv)) 
					snprintf(msg, 256, fragPreset[mFragCounter], pk->get_player_name() , pv->get_player_name() );
				else
					snprintf(msg, 256, fragPreset[mFragCounter], " unknown killer ", " unknown victim " );
			}

			// team mode also has to handle team killings
			if(world->getServerSettings()->getGameMode() == Obj_settings::gm_teamplay) {
				
				char *frmsg;
				mFragCounter = (mFragCounter+last_killers[0]+last_victims[0])%fragPresetNum;
				frmsg = fragPreset[mFragCounter];

				if((pk)&&(pv)) {
					if(pk->getTeam() == pv->getTeam()) {
						// own team
						mFragCounter = (mFragCounter+last_killers[0]+last_victims[0])%teamplayPresetNum;
						frmsg = teamplayPreset[mFragCounter];
				} }

				if((pk)&&(pv)) 
					snprintf(msg, 256, frmsg, pk->get_player_name() , pv->get_player_name() );
				else
					snprintf(msg, 256, frmsg, " unknown killer ", " unknown victim " );
			}
			
		} else {
			// suicide
			mSuicideCounter = (mSuicideCounter+last_killers[0])%suicidePresetNum;
			if(pk) 
				snprintf(msg, 256, suicidePreset[mSuicideCounter], pk->get_player_name() );
			else
				snprintf(msg, 256, suicidePreset[mSuicideCounter], " unknown player " );
		}
		for ( int i = 1; i < frags_inuse; ++i ) {
			last_killers[i-1] = last_killers[i];
			last_victims[i-1] = last_victims[i];
			//keep_frag_time[i-1] = keep_frag_time[i];
		}
		frags_inuse--;
		return true;
	}

	// no more frags to display...
	return false;
}

void Obj_scoreboard::dump() {
	logmsg(lt_message,"%i scores:",scores_inuse);
	for(int i=0;i<scores_inuse;i++){
		Obj_player_data *pd = (Obj_player_data *)world->get_object(scores[i].player_id,ot_player_data);
		if (pd)
			logmsg(lt_message,"%s: f:%2i k:%2i d:%2i",pd->get_player_name(),scores[i].frags,scores[i].kills,scores[i].deaths);
		else
			logmsg(lt_message,"%s: f:%2i k:%2i d:%2i", " unknown " ,scores[i].frags,scores[i].kills,scores[i].deaths);
	}
	logmsg ( lt_message , "frags_inuse: %d" , frags_inuse );
	if ( frags_inuse > 0 )
		for ( int i = 0; i < frags_inuse; ++i )
			//logmsg ( lt_message , "last_kills: %d - %d at %f" , last_killers [ i ] , last_victims [ i ] , keep_frag_time [ i ] );
			logmsg ( lt_message , "last_kills: %d - %d " , last_killers [ i ] , last_victims [ i ] );
}
