/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/Gamesnd/EventMusic.h $
 * $Revision: 2.16.2.4 $
 * $Date: 2007-11-21 07:27:47 $
 * $Author: Goober5000 $
 *
 * Header file for high-level control of event driven music 
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.16.2.3  2007/09/02 02:07:41  Goober5000
 * added fixes for #1415 and #1483, made sure every read_file_text had a corresponding setjmp, and sync'd the parse error messages between HEAD and stable
 *
 * Revision 2.16.2.2  2007/02/10 00:17:39  taylor
 * remove NO_SOUND
 *
 * Revision 2.16.2.1  2006/08/06 19:27:12  Goober5000
 * deprecate change-ship-model
 *
 * Revision 2.16  2006/05/21 22:57:30  Goober5000
 * fix for Mantis #750
 * --Goober5000
 *
 * Revision 2.15  2006/02/12 05:23:16  Goober5000
 * additional fixes and enhancements for substitute music
 * --Goober5000
 *
 * Revision 2.14  2006/02/12 01:27:47  Goober5000
 * more cool work on importing, music handling, etc.
 * --Goober5000
 *
 * Revision 2.13  2006/02/11 22:08:56  Goober5000
 * removed the FS1-specific music hack
 * --Goober5000
 *
 * Revision 2.12  2005/12/28 22:17:01  taylor
 * deal with cf_find_file_location() changes
 * add a central parse_modular_table() function which anything can use
 * fix up weapon_expl so that it can properly handle modular tables and LOD count changes
 * add support for for a fireball TBM (handled a little different than a normal TBM is since it only changes rather than adds)
 *
 * Revision 2.11  2005/12/13 21:48:39  wmcoolmon
 * Music TBL to proper XMT file (-mus)
 *
 * Revision 2.10  2005/07/13 02:50:51  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.9  2005/01/31 23:27:52  taylor
 * merge with Linux/OSX tree - p0131-2
 *
 * Revision 2.8  2004/08/11 05:06:23  Kazan
 * added preprocdefines.h to prevent what happened with fred -- make sure to make all fred2 headers include this file as the _first_ include -- i have already modified fs2 files to do this
 *
 * Revision 2.7  2004/05/28 23:37:30  wmcoolmon
 * Modular music tables
 *
 * Revision 2.6  2004/03/05 09:02:00  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.5  2003/08/25 04:46:53  Goober5000
 * added extra code to enable playing of FS1 music tracks
 * --Goober5000
 *
 * Revision 2.4  2003/03/29 05:18:17  Goober5000
 * yay for limit bumping!
 * bumped number of battle tracks and briefing/mainhall/credits tracks, both to 30
 * --Goober5000
 *
 * Revision 2.3  2003/03/22 06:06:22  Goober5000
 * changed event_sexp_change_music to event_change_sexp_soundtrack
 * --Goober5000
 *
 * Revision 2.2  2003/01/25 04:17:39  Goober5000
 * added change-music sexp and bumped MAX_SOUNDTRACKS from 10 to 25
 * --Goober5000
 *
 * Revision 2.1  2002/08/01 01:41:04  penguin
 * The big include file move
 *
 * Revision 2.0  2002/06/03 04:02:22  penguin
 * Warpcore CVS sync
 *
 * Revision 1.2  2002/05/10 06:08:08  mharris
 * Porting... added ifndef NO_SOUND
 *
 * Revision 1.1  2002/05/02 18:03:07  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 5     8/11/99 5:33p Jefff
 * added 3rd debrief music track
 * 
 * 4     6/20/99 12:06a Alanl
 * new event music changes
 * 
 * 3     11/20/98 4:08p Dave
 * Fixed flak effect in multiplayer.
 * 
 * 2     10/07/98 10:52a Dave
 * Initial checkin.
 * 
 * 1     10/07/98 10:48a Dave
 * 
 * 27    9/18/98 1:22p Dave
 * Added new event music stuff defines.
 * 
 * 26    5/18/98 5:22p Lawrance
 * Support new briefing music
 * 
 * 25    5/03/98 1:54a Lawrance
 * Fix event music problems related to respawning
 * 
 * 24    12/30/97 11:46a Lawrance
 * Support a success debriefing music.
 * 
 * 23    12/28/97 5:52p Lawrance
 * Add support for debriefing success/fail music.
 * 
 * 22    12/26/97 10:01p Lawrance
 * Add goal failure music... remove unused arrival music patterns
 * 
 * 21    9/19/97 5:13p Lawrance
 * add support for specifying music in the mission file
 * 
 * 20    9/09/97 5:24p Lawrance
 * added ability to switch soundtracks, gave soundtracks real names
 * 
 * 19    9/06/97 2:13p Mike
 * Replace support for TEAM_NEUTRAL
 * 
 * 18    5/14/97 9:54a Lawrance
 * supporting mission-specific briefing music
 * 
 * 17    4/21/97 5:36p Lawrance
 * add in hooks to play death music when pilot dies
 * 
 * 16    4/17/97 3:28p Lawrance
 * documented functions
 * 
 * 15    4/14/97 1:52p Lawrance
 * making transitions happen on measure boundries
 * 
 * 14    4/07/97 1:39p Lawrance
 * added event_music_first_pattern();
 * 
 * 13    4/03/97 4:26p Lawrance
 * getting digital event music working
 * 
 * 12    3/07/97 8:53a Lawrance
 * Read event at time 0 when music first starts
 * 
 * 11    2/25/97 11:10a Lawrance
 * using text of the mission name to match up which midi file gets played
 * for which mission
 * 
 * 10    2/18/97 9:43a Lawrance
 * make BTTL_2 play after 2 enemy arrivals in battle mode.  Then switch
 * back to default BTTL_1 until 2 more.
 * 
 * 9     2/14/97 11:50a Lawrance
 * added BTTL_2 track 
 * 
 * 8     2/11/97 4:22p Lawrance
 * adding song switching request to the event music level
 * 
 * 7     2/11/97 9:15a Lawrance
 * taking out BTTL_2 and BTTL_3
 * 
 * 6     2/10/97 9:26a Lawrance
 * 
 * 5     2/05/97 3:12p Lawrance
 * supporting changes in MIDI system that remove any high-level
 * dependencies
 * 
 * 4     2/04/97 11:58p Lawrance
 * volume change code, and fixing some bugs
 * 
 * 3     2/04/97 12:02p Lawrance
 * fixed temp bug, integrating music.tbl
 * 
 * 2     2/03/97 6:49p Lawrance
 * Event Music interface working
 *
 * $NoKeywords: $
 */


#ifndef __EVENT_MUSIC_H__
#define __EVENT_MUSIC_H__

#include "globalincs/globals.h"
#include "globalincs/pstypes.h"

// Identifies songs in the Soundtrack_filenames[] structure.  The order matches up with
// what is in Pattern_info and music.tbl.  Do not modify without properly inputting to
//Pattern_info and New_pattern_order
#define SONG_NRML_1				0		// Normal Song 1
#define SONG_NRML_2				1		// Normal Song 2 - for FS1
#define SONG_NRML_3				2		// Normal Song 3 - for FS1
#define SONG_AARV_1				3		// Allied Arrival 1
#define SONG_AARV_2				4		// Allied Arrival 2
#define SONG_EARV_1				5		// Enemy Arrival 1
#define SONG_EARV_2				6		// Enemy Arrival 2
#define SONG_BTTL_1				7		// Battle Song 1
#define SONG_BTTL_2				8		// Battle Song 2
#define SONG_BTTL_3				9		// Battle Song 3
#define SONG_FAIL_1				10		// Goal Failed
#define SONG_VICT_1				11		// Victory Song 1
#define SONG_VICT_2				12		// Victory Song 2
#define SONG_DEAD_1				13		// Death Song 1

#define MAX_PATTERNS	14

// if player targets a hostile ship at less than this range, switch to battle track 
#define BATTLE_START_MIN_TARGET_DIST	500	

extern int Event_Music_battle_started;	// flag that will tell us if we've started a battle in the current mission
extern int Event_music_enabled;
extern float Master_event_music_volume;			// range is 0->1


/////////////////////////////////////////////////////////////////////////////
// Used to track what briefing and debriefing music is played for the mission
/////////////////////////////////////////////////////////////////////////////
#define NUM_SCORES						5
#define SCORE_BRIEFING					0
#define SCORE_DEBRIEF_SUCCESS			1
#define SCORE_DEBRIEF_AVERAGE			2
#define SCORE_DEBRIEF_FAIL				3
#define SCORE_FICTION_VIEWER			4
extern int Mission_music[NUM_SCORES];		// indicies into Spooled_music[]
/////////////////////////////////////////////////////////////////////////////

extern int Current_soundtrack_num;		// index into Soundtracks[]


// menu music storage
typedef struct menu_music {
	int flags;
	char name[NAME_LENGTH];				// name music is known by
	char filename[MAX_FILENAME_LEN];	// name music is stored on disk as
} menu_music;

#define MAX_SPOOLED_MUSIC	30			// max number of briefing/mainhall/credits tracks

// Goober5000 - spooled music flags
#define SMF_VALID						(1 << 0)

extern menu_music Spooled_music[MAX_SPOOLED_MUSIC];
extern int Num_music_files;


// event music soundtrack storage
typedef struct tagSOUNDTRACK_INFO {
	int flags;
	int	num_patterns;
	char	name[NAME_LENGTH];
	char	pattern_fnames[MAX_PATTERNS][MAX_FILENAME_LEN];
} SOUNDTRACK_INFO;

#define MAX_SOUNDTRACKS		30			// max number of battle tracks

// Goober5000 - event music flags
#define EMF_VALID						(1 << 0)
#define EMF_ALLIED_ARRIVAL_OVERLAY		(1 << 1)
#define EMF_CYCLE_FS1					(1 << 2)

extern SOUNDTRACK_INFO Soundtracks[MAX_SOUNDTRACKS];
extern int Num_soundtracks;


void	event_music_init();
void	event_music_close();
void	event_music_level_init(int force_soundtrack = -1);
void	event_music_level_close();
void	event_music_do_frame();
void	event_music_disable();
void	event_music_enable();
void	event_music_pause();
void	event_music_unpause();
void	event_music_set_volume_all(float volume);
void	event_music_parse_musictbl(char *filename);
void	event_music_change_pattern(int new_pattern);
int	event_music_return_current_pattern();
void	event_music_first_pattern();
int	event_music_battle_start();
int	event_music_enemy_arrival();
int	event_music_friendly_arrival();
void	event_music_arrival(int team);
int	event_music_primary_goals_met();
int	event_music_primary_goal_failed();
int	event_music_player_death();
void	event_music_start_default();
void	event_music_get_info(char *outbuf, size_t max_buf);
void	event_music_get_soundtrack_name(char *outbuf, size_t max_buf);
int	event_music_next_soundtrack(int delta);
void event_sexp_change_soundtrack(char *name);
void	event_music_set_soundtrack(char *name);
void	event_music_set_score(int score_index, char *name);
int event_music_get_soundtrack_index(char *name);
int	event_music_get_spooled_music_index(char *name);
void	event_music_reset_choices();
int	event_music_player_respawn();
int	event_music_player_respawn_as_observer();
void event_music_hostile_ship_destroyed();

#endif /* __EVENT_MUSIC_H__  */
