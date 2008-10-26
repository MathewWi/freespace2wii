/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/

/*
 * $Logfile: /Freespace2/code/FREESPACE2/FreeSpace.h $
 * $Revision: 2.12.2.1 $
 * $Date: 2006-07-13 22:11:36 $
 * $Author: taylor $
 *
 * FreeSpace, the game, not the project, header information.
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.12  2006/04/20 06:32:01  Goober5000
 * proper capitalization according to Volition
 *
 * Revision 2.11  2006/01/16 11:02:23  wmcoolmon
 * Various warning fixes, scripting globals fix; added "plr" and "slf" global variables for in-game hooks; various lua functions; GCC fixes for scripting.
 *
 * Revision 2.10  2006/01/14 19:54:55  wmcoolmon
 * Special shockwave and moving capship bugfix, (even more) scripting stuff, slight rearrangement of level management functions to facilitate scripting access.
 *
 * Revision 2.9  2005/12/06 03:13:49  taylor
 * fix quite a few CFILE issues:
 *   use #define's for path lengths when possible so it's easier to move between functions
 *   fix huge Cfile_stack[] issue (how the hell did that get through :v: QA?)
 *   add Int3() check on cfopen() so it's easier to know if it get's called before cfile is ready to use
 *   move path separators to pstypes.h
 *   fix possible string overruns when setting up CFILE roots
 *   make sure we don't try to init current directory again thinking it's a CD-ROM
 *   add the list of VP roots to debug log, this will undoubtedly be useful
 * when -nosound is use go ahead and set -nomusic too to both checks are correct
 * add list of cmdline options to debug log
 * fix possible overwrite issues with get_version_string() and remove '(fs2_open)' from string plus change OGL->OpenGL, D3D->Direct3D
 *
 * Revision 2.8  2005/10/10 17:16:22  taylor
 * remove NO_NETWORK
 * whether multi is disabled or not is now determined at runtime
 * clean out some crap and old debug messages that were littered about
 *
 * Revision 2.7  2005/07/13 02:50:52  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.6  2005/06/03 06:39:26  taylor
 * better audio pause/unpause support when game window loses focus or is minimized
 *
 * Revision 2.5  2005/03/03 06:05:27  wmcoolmon
 * Merge of WMC's codebase. "Features and bugs, making Goober say "Grr!", as release would be stalled now for two months for sure"
 *
 * Revision 2.4  2004/10/31 21:31:34  taylor
 * bump COUNT_ESTIMATE, reset time compression at the start of a mission, new pilot file support, add feature_disabled popup
 *
 * Revision 2.3  2004/08/11 05:06:22  Kazan
 * added preprocdefines.h to prevent what happened with fred -- make sure to make all fred2 headers include this file as the _first_ include -- i have already modified fs2 files to do this
 *
 * Revision 2.2  2004/04/07 03:31:53  righteous1
 * Updated to add alt_tab_pause() function to draw pause screen and discontinue sounds when the game is minimized. -R1
 *
 * Revision 2.1  2002/08/01 01:41:04  penguin
 * The big include file move
 *
 * Revision 2.0  2002/06/03 04:02:22  penguin
 * Warpcore CVS sync
 *
 * Revision 1.1  2002/05/02 18:03:07  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 24    10/06/99 10:32a Jefff
 * oem updates
 * 
 * 23    9/30/99 6:04p Jefff
 * OEM changes
 * 
 * 22    9/08/99 3:22p Dave
 * Updated builtin mission list.
 * 
 * 21    9/06/99 6:38p Dave
 * Improved CD detection code.
 * 
 * 20    9/06/99 1:30a Dave
 * Intermediate checkin. Started on enforcing CD-in-drive to play the
 * game.
 * 
 * 19    9/06/99 1:16a Dave
 * Make sure the user sees the intro movie.
 * 
 * 18    9/05/99 11:19p Dave
 * Made d3d texture cache much more safe. Fixed training scoring bug where
 * it would backout scores without ever having applied them in the first
 * place.
 * 
 * 17    9/03/99 1:32a Dave
 * CD checking by act. Added support to play 2 cutscenes in a row
 * seamlessly. Fixed super low level cfile bug related to files in the
 * root directory of a CD. Added cheat code to set campaign mission # in
 * main hall.
 * 
 * 16    8/19/99 10:12a Alanl
 * preload mission-specific messages on machines greater than 48MB
 * 
 * 15    5/19/99 4:07p Dave
 * Moved versioning code into a nice isolated common place. Fixed up
 * updating code on the pxo screen. Fixed several stub problems.
 * 
 * 14    5/09/99 8:57p Dave
 * Final E3 build preparations.
 * 
 * 13    5/05/99 10:06a Dave
 * Upped beta version to 0.04
 * 
 * 12    4/29/99 3:02p Dave
 * New beta version.
 * 
 * 11    4/25/99 3:02p Dave
 * Build defines for the E3 build.
 * 
 * 10    4/09/99 2:27p Dave
 * Upped version #
 * 
 * 9     4/09/99 2:21p Dave
 * Multiplayer beta stuff. CD checking.
 * 
 * 8     4/08/99 2:10a Dave
 * Numerous bug fixes for the beta. Added builtin mission info for the
 * beta.
 * 
 * 7     3/19/99 9:52a Dave
 * Checkin to repair massive source safe crash. Also added support for
 * pof-style nebulae, and some new weapons code.
 * 
 * 6     12/03/98 5:22p Dave
 * Ported over FreeSpace 1 multiplayer ships.tbl and weapons.tbl
 * checksumming.
 * 
 * 5     10/13/98 9:26a Dave
 * Began neatening up freespace.h.
 * 
 * 4     10/09/98 1:35p Dave
 * Split off registry stuff into seperate file.
 * 
 * 3     10/07/98 6:27p Dave
 * Globalized mission and campaign file extensions. Removed Silent Threat
 * special code. Moved \cache \players and \multidata into the \data
 * directory.
 * 
 * 2     10/07/98 10:54a Dave
 * Initial checkin. 
 * 
 * 79    9/13/98 10:51p Dave
 * Put in newfangled icons for mission simulator room. New mdisk.vp
 * checksum and file length.
 * 
 * 78    9/11/98 6:59p Dave
 * First beta/rev thingie to interplay.
 * 
 * 77    9/11/98 4:14p Dave
 * Fixed file checksumming of < file_size. Put in more verbose kicking and
 * PXO stats store reporting.
 * 
 * 76    9/04/98 3:51p Dave
 * Put in validated mission updating and application during stats
 * updating.
 * 
 * 75    5/23/98 2:41p Mike
 * Make Easy the default skill level and prevent old pilot's skill level
 * from carrying into new pilot.
 * 
 * 74    5/19/98 1:19p Allender
 * new low level reliable socket reading code.  Make all missions/campaign
 * load/save to data missions folder (i.e. we are rid of the player
 * missions folder)
 * 
 * 73    5/13/98 11:34p Mike
 * Model caching system.
 * 
 * 72    5/10/98 10:05p Allender
 * only show cutscenes which have been seen before.  Made Fred able to
 * write missions anywhere, defaulting to player misison folder, not data
 * mission folder.  Fix FreeSpace code to properly read missions from
 * correct locations
 * 
 * 71    5/08/98 7:08p Dave
 * Lots of UI tweaking.
 * 
 * 70    5/08/98 5:31p Lawrance
 * extern cd checking routines 
 *
 * $NoKeywords: $
 */

#ifndef _FREESPACE_H
#define _FREESPACE_H
#ifndef STAMPER_PROGRAM							// because of all the dependancies, I have to do this...yuck!!!  MWA 7/21/97

#include "camera/camera.h"
#include "globalincs/pstypes.h"
#include "globalincs/systemvars.h"
#include "graphics/2d.h"
#include "ship/ship.h"

// --------------------------------------------------------------------------------------------------------
// FREESPACE DEFINES/VARS
//
														
// filename extensions
#define FS_CAMPAIGN_FILE_EXT				NOX(".fc2")

// CDROM volume names
#ifdef MULTIPLAYER_BETA_BUILD
	#define FS_CDROM_VOLUME_1					NOX("FS2_BETA")
	#define FS_CDROM_VOLUME_2					NOX("FS2_BETA")
#elif defined(E3_BUILD)
	#define FS_CDROM_VOLUME_1					NOX("FS2_E3DEMO")
	#define FS_CDROM_VOLUME_2					NOX("FS2_E3DEMO")
#elif defined(OEM_BUILD)
	#define FS_CDROM_VOLUME_1					NOX("FS2_OEM")
	#define FS_CDROM_VOLUME_2					NOX("FS2_OEM")
	#define FS_CDROM_VOLUME_3					NOX("FS2_OEM")
#else
	#define FS_CDROM_VOLUME_1					NOX("FREESPACE2_1")
	#define FS_CDROM_VOLUME_2					NOX("FREESPACE2_2")
	#define FS_CDROM_VOLUME_3					NOX("FREESPACE2_3")

	// old volume names
	// #define FS_CDROM_VOLUME_1					NOX("FREESPACE_1")
	// #define FS_CDROM_VOLUME_2					NOX("FREESPACE_2")
	// #define FS_CDROM_VOLUME_3					NOX("FREESPACE_3")
#endif

// this is a mission actually designed at Volition
#define MAX_BUILTIN_MISSIONS					100
#define FSB_FROM_VOLITION						(1<<0)			// we made it in-house
#define FSB_MULTI									(1<<1)			// is a multiplayer mission
#define FSB_TRAINING								(1<<2)			// is a training mission
#define FSB_CAMPAIGN								(1<<3)			// is a campaign mission
#define FSB_CAMPAIGN_FILE						(1<<4)			// is actually a campaign file

typedef struct fs_builtin_mission {
	char filename[MAX_FILENAME_LEN];
	int flags;															// see FSB_* defines above
	char cd_volume[MAX_FILENAME_LEN];							// cd volume which this needs
} fs_builtin_mission;


// function to read keyboard stuff
extern void game_process_keys();

#define ctx Freespace_base::_this

struct Freespace_base
{
	static Freespace_base * _this;
	
// frametime/missiontime variables
 fix Frametime;
#ifndef BUILD_CONTEXT
#define Frametime ctx->Frametime
#endif
 float flRealframetime;
#ifndef BUILD_CONTEXT
#define flRealframetime ctx->flRealframetime
#endif
 float flFrametime;
#ifndef BUILD_CONTEXT
#define flFrametime ctx->flFrametime
#endif
 fix Missiontime;
#ifndef BUILD_CONTEXT
#define Missiontime ctx->Missiontime
#endif

// 0 - 4
 int Game_skill_level;
#ifndef BUILD_CONTEXT
#define Game_skill_level ctx->Game_skill_level
#endif

// see GM_* defines in systemvars.h
 int Game_mode;
#ifndef BUILD_CONTEXT
#define Game_mode ctx->Game_mode
#endif
 

// if this value is set anywhere within the game_do_state_common() function, the normal do_state() will not be called
// for this frame. Useful for getting out of sticky sequencing situations.
 int Game_do_state_should_skip;
#ifndef BUILD_CONTEXT
#define Game_do_state_should_skip ctx->Game_do_state_should_skip
#endif
 

// time compression
 bool Time_compression_locked;
#ifndef BUILD_CONTEXT
#define Time_compression_locked ctx->Time_compression_locked
#endif
 
 
 fix Game_time_compression;
#ifndef BUILD_CONTEXT
#define Game_time_compression ctx->Game_time_compression
#endif


// Set if subspace is active this level
 int Game_subspace_effect;		
#ifndef BUILD_CONTEXT
#define Game_subspace_effect ctx->Game_subspace_effect
#endif


// The current mission being played.
 char Game_current_mission_filename[MAX_FILENAME_LEN];
#ifndef BUILD_CONTEXT
#define Game_current_mission_filename ctx->Game_current_mission_filename
#endif

// game's CDROM directory
 char Game_CDROM_dir[MAX_PATH_LEN];
#ifndef BUILD_CONTEXT
#define Game_CDROM_dir ctx->Game_CDROM_dir
#endif

// if the ships.tbl the player has is valid
 int Game_ships_tbl_valid;
#ifndef BUILD_CONTEXT
#define Game_ships_tbl_valid ctx->Game_ships_tbl_valid
#endif


// if the weapons.tbl the player has is valid
 int Game_weapons_tbl_valid;
#ifndef BUILD_CONTEXT
#define Game_weapons_tbl_valid ctx->Game_weapons_tbl_valid
#endif


// to disable networking at runtime
 int Networking_disabled;
#ifndef BUILD_CONTEXT
#define Networking_disabled ctx->Networking_disabled
#endif

int	Fred_running ;// 0;
#ifndef BUILD_CONTEXT
#define Fred_running ctx->Fred_running
#endif


// --------------------------------------------------------------------------------------------------------
// FREESPACE FUNCTIONS
//

// mission management -------------------------------------------------

// loads in the currently selected mission
virtual int game_start_mission();
#ifndef BUILD_CONTEXT
#define game_start_mission ctx->game_start_mission
#endif

// shutdown a mission
virtual void game_level_close();
#ifndef BUILD_CONTEXT
#define game_level_close ctx->game_level_close
#endif


// gameplay stuff -----------------------------------------------------

// stop the game (mission) timer
virtual void game_stop_time();
#ifndef BUILD_CONTEXT
#define game_stop_time ctx->game_stop_time
#endif

// start the game (mission) timer
virtual void game_start_time();
#ifndef BUILD_CONTEXT
#define game_start_time ctx->game_start_time
#endif


// call whenever in a loop or if you need to get a keypress
virtual int game_check_key();
#ifndef BUILD_CONTEXT
#define game_check_key ctx->game_check_key
#endif

// poll for keypresses
virtual int game_poll();
#ifndef BUILD_CONTEXT
#define game_poll ctx->game_poll
#endif

// call this when you don't want the user changing time compression
virtual void lock_time_compression(bool is_locked);
#ifndef BUILD_CONTEXT
#define lock_time_compression ctx->lock_time_compression
#endif


// call this to set time compression properly
virtual void set_time_compression(float multiplier, float change_time = 0);
#ifndef BUILD_CONTEXT
#define set_time_compression ctx->set_time_compression
#endif

//call this to change the relative time compression (ie double it)
virtual void change_time_compression(float multiplier);
#ifndef BUILD_CONTEXT
#define change_time_compression ctx->change_time_compression
#endif

// call this to set frametime properly (once per frame)
virtual void game_set_frametime(int state);
#ifndef BUILD_CONTEXT
#define game_set_frametime ctx->game_set_frametime
#endif

// overall frametime of game, indepedent of mission timer
virtual fix game_get_overall_frametime();
#ifndef BUILD_CONTEXT
#define game_get_overall_frametime ctx->game_get_overall_frametime
#endif

// Used to halt all looping game sounds
virtual void game_stop_looped_sounds();
#ifndef BUILD_CONTEXT
#define game_stop_looped_sounds ctx->game_stop_looped_sounds
#endif

// do stuff that may need to be done regardless of state
virtual void game_do_state_common(int state,int no_networking = 0);
#ifndef BUILD_CONTEXT
#define game_do_state_common ctx->game_do_state_common
#endif


// skill level --------------------------------------------------------

// increase the skill level (will wrap around to min skill level)
virtual void game_increase_skill_level();
#ifndef BUILD_CONTEXT
#define game_increase_skill_level ctx->game_increase_skill_level
#endif

// get the default game skill level
virtual int game_get_default_skill_level();
#ifndef BUILD_CONTEXT
#define game_get_default_skill_level ctx->game_get_default_skill_level
#endif

// a keypress.  See CPP file for more info.
virtual void game_flush();
#ifndef BUILD_CONTEXT
#define game_flush ctx->game_flush
#endif

// running with low-memory (less than 48MB)
virtual bool game_using_low_mem();
#ifndef BUILD_CONTEXT
#define game_using_low_mem ctx->game_using_low_mem
#endif

// misc ---------------------------------------------------------------

// lookup the specified filename. return an fs_builtin_mission* if found, NULL otherwise
virtual fs_builtin_mission *game_find_builtin_mission(char *filename);
#ifndef BUILD_CONTEXT
#define game_find_builtin_mission ctx->game_find_builtin_mission
#endif



//================================================================
// GAME FLASH STUFF  - code in FreeSpace.cpp

// Resets the flash
virtual void game_flash_reset();
#ifndef BUILD_CONTEXT
#define game_flash_reset ctx->game_flash_reset
#endif

// Adds a flash effect.  These can be positive or negative.
// The range will get capped at around -1 to 1, so stick 
// with a range like that.
virtual void game_flash( float r, float g, float b );
#ifndef BUILD_CONTEXT
#define game_flash ctx->game_flash
#endif

// Adds a flash for Big Ship explosions
// cap range from 0 to 1
virtual void big_explosion_flash(float flash);
#ifndef BUILD_CONTEXT
#define big_explosion_flash ctx->big_explosion_flash
#endif

// Call once a frame to diminish the
// flash effect to 0.
virtual void game_flash_diminish();
#ifndef BUILD_CONTEXT
#define game_flash_diminish ctx->game_flash_diminish
#endif

// Loads the best palette for this level, based
// on nebula color and hud color.  You could just call palette_load_table with
// the appropriate filename, but who wants to do that.
virtual void game_load_palette();
#ifndef BUILD_CONTEXT
#define game_load_palette ctx->game_load_palette
#endif

//================================================================

// Call at the beginning of each frame
virtual void game_whack_reset();
#ifndef BUILD_CONTEXT
#define game_whack_reset ctx->game_whack_reset
#endif

// Call to apply a whack to a the ship. Used for force feedback
virtual void game_whack_apply( float x, float y );
#ifndef BUILD_CONTEXT
#define game_whack_apply ctx->game_whack_apply
#endif

// call to apply a "shudder"
virtual void game_shudder_apply(int time, float intensity);
#ifndef BUILD_CONTEXT
#define game_shudder_apply ctx->game_shudder_apply
#endif

//===================================================================

// make sure a CD is in the drive before continuing (returns 1 to continue, otherwise 0).
virtual int game_do_cd_check(char *volume_name=NULL);
#ifndef BUILD_CONTEXT
#define game_do_cd_check ctx->game_do_cd_check
#endif
virtual int game_do_cd_check_specific(char *volume_name, int cdnum);
#ifndef BUILD_CONTEXT
#define game_do_cd_check_specific ctx->game_do_cd_check_specific
#endif
virtual int find_freespace_cd(char *volume_name=NULL);
#ifndef BUILD_CONTEXT
#define find_freespace_cd ctx->find_freespace_cd
#endif
virtual int set_cdrom_path(int drive_num);
#ifndef BUILD_CONTEXT
#define set_cdrom_path ctx->set_cdrom_path
#endif
virtual int game_do_cd_mission_check(char *filename);
#ifndef BUILD_CONTEXT
#define game_do_cd_mission_check ctx->game_do_cd_mission_check
#endif

// Used to tell the player that a feature isn't available in the demo version of FreeSpace
virtual void game_feature_not_in_demo_popup();
#ifndef BUILD_CONTEXT
#define game_feature_not_in_demo_popup ctx->game_feature_not_in_demo_popup
#endif

// Used to tell the player that a feature is disabled by build settings
virtual void game_feature_disabled_popup();
#ifndef BUILD_CONTEXT
#define game_feature_disabled_popup ctx->game_feature_disabled_popup
#endif

//	Return version string for demo or full version, depending on build.
virtual void get_version_string(char *str, int max_size);
#ifndef BUILD_CONTEXT
#define get_version_string ctx->get_version_string
#endif

// format the specified time (fixed point) into a nice string
virtual void game_format_time(fix m_time,char *time_str);
#ifndef BUILD_CONTEXT
#define game_format_time ctx->game_format_time
#endif

// if the game is running using hacked data
virtual int game_hacked_data();
#ifndef BUILD_CONTEXT
#define game_hacked_data ctx->game_hacked_data
#endif

// show the oem upsell screens (end of campaign, or close of game
virtual void oem_upsell_show_screens();
#ifndef BUILD_CONTEXT
#define oem_upsell_show_screens ctx->oem_upsell_show_screens
#endif

// calls to be executed when the game is put in or restored from minimized or inactive state
virtual void game_pause();
#ifndef BUILD_CONTEXT
#define game_pause ctx->game_pause
#endif
virtual void game_unpause();
#ifndef BUILD_CONTEXT
#define game_unpause ctx->game_unpause
#endif

//WMC - Stuff for scripting, these make the game go
camid Main_camera;
#ifndef BUILD_CONTEXT
#define Main_camera ctx->Main_camera
#endif

 virtual void game_level_init(int seed = -1);
#ifndef BUILD_CONTEXT
#define game_level_init ctx->game_level_init
#endif
virtual  void game_post_level_init();
#ifndef BUILD_CONTEXT
#define game_post_level_init ctx->game_post_level_init
#endif
 virtual camid game_render_frame_setup();
#ifndef BUILD_CONTEXT
#define game_render_frame_setup ctx->game_render_frame_setup
#endif
 virtual void game_render_frame(camid cid);
#ifndef BUILD_CONTEXT
#define game_render_frame ctx->game_render_frame
#endif
 virtual void game_simulation_frame();
#ifndef BUILD_CONTEXT
#define game_simulation_frame ctx->game_simulation_frame
#endif
 virtual void game_update_missiontime();
#ifndef BUILD_CONTEXT
#define game_update_missiontime ctx->game_update_missiontime
#endif
 virtual void game_render_post_frame();
#ifndef BUILD_CONTEXT
#define game_render_post_frame ctx->game_render_post_frame
#endif


 virtual void game_leave_state( int old_state, int new_state );
#ifndef BUILD_CONTEXT
#define game_leave_state ctx->game_leave_state
#endif
 virtual void game_enter_state( int old_state, int new_state );
#ifndef BUILD_CONTEXT
#define game_enter_state ctx->game_enter_state
#endif

 virtual void game_do_state(int state);
#ifndef BUILD_CONTEXT
#define game_do_state ctx->game_do_state
#endif

 virtual void game_process_event( int current_state, int event );
#ifndef BUILD_CONTEXT
#define game_process_event ctx->game_process_event
#endif

int game_single_step ;// 0;
#ifndef BUILD_CONTEXT
#define game_single_step ctx->game_single_step
#endif
int last_single_step;//0;
#ifndef BUILD_CONTEXT
#define last_single_step ctx->last_single_step
#endif

float Warpout_time ;// 0.0f;
#ifndef BUILD_CONTEXT
#define Warpout_time ctx->Warpout_time
#endif
int Warpout_forced ;// 0;		// Set if this is a forced warpout that cannot be cancelled.
#ifndef BUILD_CONTEXT
#define Warpout_forced ctx->Warpout_forced
#endif

int Debug_octant ;// -1;
#ifndef BUILD_CONTEXT
#define Debug_octant ctx->Debug_octant
#endif

int Test_begin ;// 0;
#ifndef BUILD_CONTEXT
#define Test_begin ctx->Test_begin
#endif

float frametotal ;// 0.0f;
#ifndef BUILD_CONTEXT
#define frametotal ctx->frametotal
#endif

int	Framerate_delay ;// 0;
#ifndef BUILD_CONTEXT
#define Framerate_delay ctx->Framerate_delay
#endif

float FreeSpace_gamma ;// 1.0f;
#ifndef BUILD_CONTEXT
#define FreeSpace_gamma ctx->FreeSpace_gamma
#endif

float Framerate ;// 0.0f;
#ifndef BUILD_CONTEXT
#define Framerate ctx->Framerate
#endif

virtual void game_get_framerate();
#ifndef BUILD_CONTEXT
#define game_get_framerate ctx->game_get_framerate
#endif

int Multi_ping_timestamp ;// -1;
#ifndef BUILD_CONTEXT
#define Multi_ping_timestamp ctx->Multi_ping_timestamp
#endif

int tst ;// 0;
#ifndef BUILD_CONTEXT
#define tst ctx->tst
#endif

int	Show_cpu ;// 0;
#ifndef BUILD_CONTEXT
#define Show_cpu ctx->Show_cpu
#endif

//	Player's velocity just before he blew up.  Used to keep camera target moving.
vec3d	Dead_player_last_vel ;// { { { 1.0f, 1.0f, 1.0f } } };
#ifndef BUILD_CONTEXT
#define Dead_player_last_vel ctx->Dead_player_last_vel
#endif

	int	Show_framerate ;// 1;
#ifndef BUILD_CONTEXT
#define Show_framerate ctx->Show_framerate
#endif

int	Show_target_debug_info ;// 0;
#ifndef BUILD_CONTEXT
#define Show_target_debug_info ctx->Show_target_debug_info
#endif

int	Show_target_weapons ;// 0;
#ifndef BUILD_CONTEXT
#define Show_target_weapons ctx->Show_target_weapons
#endif

virtual void game_tst_mark(object *objp, ship *shipp);
#ifndef BUILD_CONTEXT
#define game_tst_mark ctx->game_tst_mark
#endif

int Sun_drew ;// 0;
#ifndef BUILD_CONTEXT
#define Sun_drew ctx->Sun_drew
#endif

};
#endif			// endif of #ifndef STAMPER_PROGRAM
#endif 
