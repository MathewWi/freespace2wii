/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/Hud/HUDtargetbox.cpp $
 * $Revision: 2.65.2.6 $
 * $Date: 2007-08-30 04:52:30 $
 * $Author: Backslash $
 *
 * C module for drawing the target monitor box on the HUD
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.65.2.5  2007/07/15 04:19:33  Goober5000
 * partial commit of aldo's eyepoint feature
 * it will need a keystroke to be complete
 *
 * Revision 2.65.2.4  2007/02/20 04:19:10  Goober5000
 * the great big duplicate model removal commit
 *
 * Revision 2.65.2.3  2006/12/26 05:27:15  taylor
 * make sure that we don't target a hidden jumpnode, and if we do be sure to only detarget it rather than turning off auto-targetting
 *
 * Revision 2.65.2.2  2006/08/03 01:33:25  Goober5000
 * add a second method for specifying ship copies, plus allow the parser to recognize ship class copy names that aren't consistent with the table
 * --Goober5000
 *
 * Revision 2.65.2.1  2006/07/17 01:09:03  taylor
 * fix some missile autocentering issues
 *  - use MR_AUTOCENTER and MR_IS_MISSILE flags to generate an autocenter for a missile if one doesn't already exist
 *  - don't try to autocenter loadout icons when rendered 3d
 *
 * Revision 2.65  2006/05/13 07:09:24  taylor
 * minor cleanup and a couple extra error checks
 * get rid of some wasteful math from the gr_set_proj_matrix() calls
 *
 * Revision 2.64  2006/04/12 22:23:41  taylor
 * compiler warning fixes to make GCC 4.1 shut the hell up
 *
 * Revision 2.63  2006/01/18 16:14:04  taylor
 * allow gr_render_buffer() to take TMAP flags
 * let gr_render_buffer() render untextured polys (OGL only until some D3D people fix it on their side)
 * add MR_SHOW_OUTLINE_HTL flag so we easily render using HTL mode for wireframe views
 * make Interp_verts/Interp_norms/etc. dynamic and get rid of the extra htl_* versions
 *
 * Revision 2.62  2006/01/13 03:30:59  Goober5000
 * übercommit of custom IFF stuff :)
 *
 * Revision 2.61  2005/12/12 21:32:14  taylor
 * allow use of a specific LOD for ship and weapon rendering in the hud targetbox
 *
 * Revision 2.60  2005/10/10 17:21:04  taylor
 * remove NO_NETWORK
 *
 * Revision 2.59  2005/09/25 08:25:15  Goober5000
 * Okay, everything should now work again. :p Still have to do a little more with the asteroids.
 * --Goober5000
 *
 * Revision 2.58  2005/09/25 05:26:13  Goober5000
 * bah
 * --Goober5000
 *
 * Revision 2.57  2005/09/25 05:13:06  Goober5000
 * hopefully complete species upgrade
 * --Goober5000
 *
 * Revision 2.56  2005/07/25 05:24:16  Goober5000
 * cleaned up some command line and mission flag stuff
 * --Goober5000
 *
 * Revision 2.55  2005/07/22 10:18:38  Goober5000
 * CVS header tweaks
 * --Goober5000
 *
 * Revision 2.54  2005/07/18 03:44:01  taylor
 * cleanup hudtargetbox rendering from that total hack job that had been done on it (fixes wireframe view as well)
 * more non-standard res fixing
 *  - I think everything should default to resize now (much easier than having to figure that crap out)
 *  - new mouse_get_pos_unscaled() function to return 1024x768/640x480 relative values so we don't have to do it later
 *  - lots of little cleanups which fix several strange offset/size problems
 *  - fix gr_resize/unsize_screen_pos() so that it won't wrap on int (took too long to track this down)
 *
 * Revision 2.53  2005/07/13 03:15:52  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.52  2005/07/13 00:44:22  Goober5000
 * improved species support and removed need for #define
 * --Goober5000
 *
 * Revision 2.51  2005/05/11 22:15:49  phreak
 * added mission flag that will not show enemy wing names, just the ship class.
 *
 * Revision 2.50  2005/04/28 05:29:29  wmcoolmon
 * Removed FS2_DEMO defines that looked like they wouldn't cause the universe to collapse
 *
 * Revision 2.49  2005/04/05 05:53:17  taylor
 * s/vector/vec3d/g, better support for different compilers (Jens Granseuer)
 *
 * Revision 2.48  2005/03/27 12:28:33  Goober5000
 * clarified max hull/shield strength names and added ship guardian thresholds
 * --Goober5000
 *
 * Revision 2.47  2005/03/25 06:57:34  wmcoolmon
 * Big, massive, codebase commit. I have not removed the old ai files as the ones I uploaded aren't up-to-date (But should work with the rest of the codebase)
 *
 * Revision 2.46  2005/03/06 11:23:45  wmcoolmon
 * RE-fixed stuff. Ogg support. Briefings.
 *
 * Revision 2.45  2005/03/03 06:05:28  wmcoolmon
 * Merge of WMC's codebase. "Features and bugs, making Goober say "Grr!", as release would be stalled now for two months for sure"
 *
 * Revision 2.44  2005/03/02 21:24:44  taylor
 * more network/inferno goodness for Windows, takes care of a few warnings too
 *
 * Revision 2.43  2005/01/30 03:26:11  wmcoolmon
 * HUD updates
 *
 * Revision 2.42  2005/01/29 09:19:45  argv
 * Fixed compile errors due to several source files not having been updated to
 * reference "Min/Max_draw_distance" instead of "MIN/MAX_DRAW_DISTANCE".
 *
 * -- _argv[-1]
 *
 * Revision 2.41  2005/01/11 21:38:49  Goober5000
 * multiple ship docking :)
 * don't tell anyone yet... check the SCP internal
 * --Goober500
 *
 * Revision 2.40  2005/01/01 19:47:26  taylor
 * make use of MR_NO_FOGGING to render models without fog
 *
 * Revision 2.39  2004/08/01 02:30:52  phreak
 * -phreak command line changed to -dualscanlines
 *
 * Revision 2.38  2004/07/26 20:47:33  Kazan
 * remove MCD complete
 *
 * Revision 2.37  2004/07/12 16:32:49  Kazan
 * MCD - define _MCD_CHECK to use memory tracking
 *
 * Revision 2.36  2004/04/07 17:20:24  phreak
 * changed around the order of some function calls to get the target model
 * properly rendering in the hud targetbox
 *
 * Revision 2.35  2004/04/06 03:09:53  phreak
 * added a control config option for the wireframe hud targetbox i enabled ages ago
 *
 * Revision 2.34  2004/03/17 04:07:30  bobboau
 * new fighter beam code
 * fixed old after burner trails
 * had to bump a few limits, working on some dynamic solutions
 * a few fixed to background POF rendering
 * fixing asorted bugs
 *
 * Revision 2.33  2004/03/05 09:02:04  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.32  2004/02/15 06:02:32  bobboau
 * fixed sevral asorted matrix errors,
 * OGL people make sure I didn't break anything,
 * most of what I did was replaceing falses with (if graphicts_mode == D3D)
 *
 * Revision 2.31  2004/02/10 05:07:38  matt
 * Fixed issue with ships being rendered off center
 * in the HUD target box. Much easier than I
 * thought
 * -- Sticks
 *
 * Revision 2.30  2004/02/07 05:11:08  Goober5000
 * fix for undestroyable subsystems
 * --Goober5000
 *
 * Revision 2.29  2004/02/06 23:17:43  Goober5000
 * tweakage
 * --Goober5000
 *
 * Revision 2.28  2004/02/04 09:02:45  Goober5000
 * got rid of unnecessary double semicolons
 * --Goober5000
 *
 * Revision 2.27  2004/02/04 08:41:04  Goober5000
 * made code more uniform and simplified some things,
 * specifically shield percentage and quadrant stuff
 * --Goober5000
 *
 * Revision 2.26  2004/01/30 07:39:07  Goober5000
 * whew - I just went through all the code I ever added (or at least, that I could
 * find that I commented with a Goober5000 tag) and added a bunch of Asserts
 * and error-checking
 * --Goober5000
 *
 * Revision 2.25  2003/12/16 20:46:37  phreak
 * made gr_set_proj_matrix use the MIN/MAX_DRAW_DISTANCE constants
 *
 * Revision 2.24  2003/12/04 20:39:09  randomtiger
 * Added DDS image support for D3D
 * Added new command flag '-ship_choice_3d' to activate 3D models instead of ani's in ship choice, feature now off by default
 * Hopefully have fixed D3D text batching bug that caused old values to appear
 * Added Hud_target_object_factor variable to control 3D object sizes of zoom in HUD target
 * Fixed jump nodes not showing
 *
 * Revision 2.23  2003/11/16 04:09:23  Goober5000
 * language
 *
 * Revision 2.22  2003/11/11 03:56:11  bobboau
 * lots of bug fixing, much of it in nebula and bitmap drawing
 *
 * Revision 2.21  2003/11/09 06:31:39  Kazan
 * a couple of htl functions being called in nonhtl (ie NULL functions) problems fixed
 * conflicts in cmdline and timerbar.h log entries
 * cvs stopped acting like it was on crack obviously
 *
 * Revision 2.20  2003/11/06 22:47:02  phreak
 * added gr_start_**_matrix() and gr_end_**_matrix() around where ships are rendered
 *
 * Revision 2.19  2003/09/13 08:27:29  Goober5000
 * added some minor things, such as code cleanup and the following:
 * --turrets will not fire at cargo
 * --MAX_SHIELD_SECTIONS substituted for the number 4 in many places
 * --supercaps have their own default message bitfields (distinguished from capships)
 * --turrets are allowed on fighters
 * --jump speed capped at 65m/s, to avoid ship travelling too far
 * --non-huge weapons now scale their damage, instead of arbitrarily cutting off
 * ----Goober5000
 *
 * Revision 2.18  2003/09/13 06:02:05  Goober5000
 * clean rollback of all of argv's stuff
 * --Goober5000
 *
 * Revision 2.15  2003/08/21 08:31:24  Goober5000
 * fixed turret text display for non-laser weapons
 * --Goober5000
 *
 * Revision 2.14  2003/04/29 01:03:23  Goober5000
 * implemented the custom hitpoints mod
 * --Goober5000
 *
 * Revision 2.13  2003/01/27 07:46:33  Goober5000
 * finished up my fighterbay code - ships will not be able to arrive from or depart into
 * fighterbays that have been destroyed (but you have to explicitly say in the tables
 * that the fighterbay takes damage in order to enable this)
 * --Goober5000
 *
 * Revision 2.12  2003/01/17 07:59:09  Goober5000
 * fixed some really strange behavior with strings not being truncated at the
 * # symbol
 * --Goober5000
 *
 * Revision 2.11  2003/01/17 01:48:50  Goober5000
 * added capability to the $Texture replace code to substitute the textures
 * without needing and extra model, however, this way you can't substitute
 * transparent or animated textures
 * --Goober5000
 *
 * Revision 2.10  2003/01/15 20:49:10  Goober5000
 * did the XSTR for the turret thing
 * --Goober5000
 *
 * Revision 2.9  2003/01/15 16:52:27  Goober5000
 * oops, that'll introduce a bug - naming it just plain "Turret" instead
 * --Goober5000
 *
 * Revision 2.8  2003/01/15 16:48:59  Goober5000
 * capship ballistic weapon displays "Cannon" on HUD instead of "Laser Turret"
 * --Goober5000
 *
 * Revision 2.7  2003/01/15 07:09:09  Goober5000
 * changed most references to modelnum to use ship instead of ship_info --
 * this will help with the change-model sexp and any other instances of model
 * changing
 * --Goober5000
 *
 * Revision 2.6  2002/12/14 17:09:28  Goober5000
 * removed mission flag for fighterbay damage; instead made damage display contingent on whether the fighterbay subsystem is assigned a damage percentage in ships.tbl
 * --Goober5000
 *
 * Revision 2.5  2002/12/14 01:55:04  Goober5000
 * added mission flag to show subsystem damage for fighterbays
 * ~Goober5000~
 *
 * Revision 2.4  2002/10/22 23:02:40  randomtiger
 * Made Phreaks alternative scanning style optional under the command line tag "-phreak"
 * Fixed bug that changes HUD colour when targetting debris in a full nebula. - RT
 *
 * Revision 2.3  2002/09/20 20:01:30  phreak
 * extra effects during cargo scan
 *
 * Revision 2.2  2002/08/06 16:50:44  phreak
 * added wireframe targetbox feature
 *
 * Revision 2.2 2002/08/04 23:17:40  phreak
 * Did the toggling code for wireframes -> CTRL+SHIFT+Q
 *
 * Revision 2.2  2002/08/04 21:53:13  phreak
 * Changed hud_render_target_asteroid, ...weapon, ...ship to render in wireframe
 *
 * Revision 2.1  2002/08/01 01:41:06  penguin
 * The big include file move
 *
 * Revision 2.0  2002/06/03 04:02:23  penguin
 * Warpcore CVS sync
 *
 * Revision 1.3  2002/05/13 21:09:28  mharris
 * I think the last of the networking code has ifndef NO_NETWORK...
 *
 * Revision 1.2  2002/05/03 22:07:08  mharris
 * got some stuff to compile
 *
 * Revision 1.1  2002/05/02 18:03:08  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 31    11/02/99 3:22p Jefff
 * translation of targetbox text
 * 
 * 30    10/29/99 10:41p Jefff
 * more subsystem fixes
 * 
 * 29    10/28/99 11:17p Jefff
 * used escape seqs for some special German chars
 * 
 * 28    10/28/99 2:02a Jefff
 * revised the subsystem localization
 * 
 * 27    9/14/99 11:03p Jefff
 * dont draw target names from # on (weapons case)
 * 
 * 26    9/04/99 5:17p Andsager
 * Make event log record name of destroyed subsytem, and use this name for
 * different types of turrets
 * 
 * 25    8/25/99 11:35a Andsager
 * Move hud render ship subsystem target box for ships with Autocenter
 * 
 * 24    8/17/99 7:32p Jefff
 * models use autocenter in target view
 * 
 * 23    8/01/99 12:39p Dave
 * Added HUD contrast control key (for nebula).
 * 
 * 22    7/31/99 4:15p Dave
 * Fixed supernova particle velocities. Handle OBJ_NONE in target
 * monitoring view. Properly use objectives notify gauge colors.
 * 
 * 21    7/28/99 2:49p Andsager
 * Make hud target speed use vm_vec_mag (not vm_vec_mag_quick)
 * 
 * 20    7/15/99 9:20a Andsager
 * FS2_DEMO initial checkin
 * 
 * 19    7/02/99 10:56a Andsager
 * Put in big ship - big ship attack mode.  Modify stealth sweep ai.
 * 
 * 18    6/29/99 3:16p Andsager
 * Debug stuff.
 * 
 * 17    6/10/99 3:43p Dave
 * Do a better job of syncing text colors to HUD gauges.
 * 
 * 16    6/09/99 2:55p Andsager
 * Allow multiple asteroid subtypes (of large, medium, small) and follow
 * family.
 * 
 * 15    6/07/99 4:20p Andsager
 * Add HUD color for tagged object.  Apply to target and radar.
 * 
 * 14    6/03/99 11:43a Dave
 * Added the ability to use a different model when rendering to the HUD
 * target box.
 * 
 * 13    5/24/99 9:02a Andsager
 * Remove Int3() in turret subsys name code when turret has no weapon.
 * 
 * 12    5/21/99 1:42p Andsager
 * Added error checking for HUD turret name
 * 
 * 11    5/20/99 7:00p Dave
 * Added alternate type names for ships. Changed swarm missile table
 * entries.
 * 
 * 10    5/19/99 3:50p Andsager
 * Show type of debris is debris field (species debris or asteroid).  Show
 * type of subsystem turret targeted (laser, missile, flak, beam).
 * 
 * 9     5/14/99 4:22p Andsager
 * Modify hud_render_target_ship to show damaged subsystems in their
 * actual state.  Now also shows rotation of subsystems.
 * 
 * 8     4/16/99 5:54p Dave
 * Support for on/off style "stream" weapons. Real early support for
 * target-painting lasers.
 * 
 * 7     1/07/99 9:08a Jasen
 * HUD coords
 * 
 * 6     12/28/98 3:17p Dave
 * Support for multiple hud bitmap filenames for hi-res mode.
 * 
 * 5     12/21/98 5:03p Dave
 * Modified all hud elements to be multi-resolution friendly.
 * 
 * 4     11/05/98 4:18p Dave
 * First run nebula support. Beefed up localization a bit. Removed all
 * conditional compiles for foreign versions. Modified mission file
 * format.
 * 
 * 3     10/13/98 9:28a Dave
 * Started neatening up freespace.h. Many variables renamed and
 * reorganized. Added AlphaColors.[h,cpp]
 * 
 * 2     10/07/98 10:53a Dave
 * Initial checkin.
 * 
 * 1     10/07/98 10:49a Dave
 * 
 * 108   8/28/98 3:28p Dave
 * EMP effect done. AI effects may need some tweaking as required.
 * 
 * 107   8/25/98 1:48p Dave
 * First rev of EMP effect. Player side stuff basically done. Next comes
 * AI code.
 * 
 * 106   6/19/98 3:49p Lawrance
 * localization tweaks
 * 
 * 105   6/17/98 11:04a Lawrance
 * localize subsystem names that appear on the HUD
 * 
 * 104   6/09/98 5:18p Lawrance
 * French/German localization
 * 
 * 103   6/09/98 10:31a Hoffoss
 * Created index numbers for all xstr() references.  Any new xstr() stuff
 * added from here on out should be added to the end if the list.  The
 * current list count can be found in FreeSpace.cpp (search for
 * XSTR_SIZE).
 * 
 * 102   6/01/98 11:43a John
 * JAS & MK:  Classified all strings for localization.
 * 
 * 101   5/20/98 3:52p Allender
 * fixed compiler warnings
 * 
 * 100   5/20/98 12:59p John
 * Turned optimizations on for debug builds.   Also turning on automatic
 * function inlining.  Turned off the unreachable code warning.
 * 
 * 99    5/15/98 8:36p Lawrance
 * Add 'target ship that last sent transmission' target key
 * 
 * 98    5/14/98 11:26a Lawrance
 * ensure fighter bays are drawn with correct bracket color
 * 
 * 97    5/08/98 5:32p Lawrance
 * Allow cargo scanning even if target gauge is disabled
 * 
 * 96    5/04/98 10:51p Lawrance
 * remove unused local 
 * 
 * 95    5/04/98 9:17p Lawrance
 * Truncate ship class names at # char when displaying debris on target
 * moniter
 * 
 * 94    5/04/98 6:12p Lawrance
 * Write generic function hud_end_string_at_first_hash_symbol(), to use in
 * various spots on the HUD
 * 
 * 93    4/15/98 12:55a Lawrance
 * Show time to impact for bombs
 * 
 * 92    4/02/98 6:31p Lawrance
 * remove asteroid references if DEMO defined
 * 
 * 91    3/31/98 5:18p John
 * Removed demo/save/restore.  Made NDEBUG defined compile.  Removed a
 * bunch of debug stuff out of player file.  Made model code be able to
 * unload models and malloc out only however many models are needed.
 *  
 * 
 * 90    3/30/98 1:08a Lawrance
 * Implement "blast" icon.  Blink HUD icon when player ship is hit by a
 * blast.
 *
 * $NoKeywords: $
 */


#include "hud/hudtargetbox.h"
#include "object/object.h"
#include "hud/hud.h"
#include "hud/hudbrackets.h"
#include "model/model.h"
#include "mission/missionparse.h"
#include "debris/debris.h"
#include "playerman/player.h"
#include "gamesnd/gamesnd.h"
#include "freespace2/freespace.h"
#include "io/timer.h"
#include "ship/subsysdamage.h"
#include "graphics/font.h"
#include "asteroid/asteroid.h"
#include "jumpnode/jumpnode.h"
#include "weapon/emp.h"
#include "localization/localize.h"
#include "cmdline/cmdline.h"
#include "ship/ship.h"
#include "weapon/weapon.h"
#include "parse/parselo.h"
#include "object/objectdock.h"
#include "species_defs/species_defs.h"
#include "iff_defs/iff_defs.h"
#include "network/multi.h"

#ifndef NDEBUG
#include "hud/hudets.h"
#endif


extern float View_zoom;

int Target_window_coords[GR_NUM_RESOLUTIONS][4] =
{
	{ // GR_640
		8, 362, 131, 112
	},
	{ // GR_1024
		8, 629, 131, 112
	}
};

object *Enemy_attacker = NULL;

static int Target_static_next;
static int Target_static_playing;
int Target_static_looping;

// used to print out + or - after target distance and speed
char* modifiers[] = {
//XSTR:OFF
"+",
"-",
""
//XSTR:ON
};

char Target_view_fname[GR_NUM_RESOLUTIONS][MAX_FILENAME_LEN] = {
	"targetview1",
	"targetview1"
};
char Target_integ_fname[GR_NUM_RESOLUTIONS][MAX_FILENAME_LEN] = {
	"targetview2",
	"targetview2"
};
char Target_extra_fname[GR_NUM_RESOLUTIONS][MAX_FILENAME_LEN] = {
	"targetview3",
	"targetview3"
};

// animation frames for the target view monitor
// frames:	0	=>		background of target monitor
//				1	=>		foreground of target monitor
hud_frames Target_view_gauge;
int Target_view_gauge_loaded = 0;

// animation frames for the extended target information
// frames:	0	=>		normal gague
hud_frames Target_view_extra;
int Target_view_extra_loaded = 0;

// animation frames for the target view monitor integrity bar
// frames:	0	=>		dark bar
//				1	=>		bright bar
hud_frames Target_view_integrity_gauge;
int Target_view_integrity_gauge_loaded = 0;

#define NUM_TBOX_COORDS			11	// keep up to date
#define TBOX_BACKGROUND			0
#define TBOX_NAME					1
#define TBOX_CLASS				2
#define TBOX_DIST					3
#define TBOX_SPEED				4
#define TBOX_CARGO				5
#define TBOX_HULL					6
#define TBOX_EXTRA				7
#define TBOX_EXTRA_ORDERS		8
#define TBOX_EXTRA_TIME			9
#define TBOX_EXTRA_DOCK			10

int Targetbox_coords[GR_NUM_RESOLUTIONS][NUM_TBOX_COORDS][2] = 
{
	{ // GR_640
		{5,319},
		{13,316},
		{13,325},
		{13,337},
		{90,337},
		{13,349},
		{139,361},
		{5,283},
		{13,280},
		{13,290},
		{13,299}
	}, 
	{ // GR_1024
		{5,590},
		{13,587},
		{13,597},
		{13,608},
		{90,608},
		{13,620},
		{139,632},
		{5,555},
		{13,552},
		{13,561},
		{13,570}
	}
};

int Integrity_bar_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		138, 371, 4, 88
	},
	{ // GR_1024
		138, 642, 4, 88
	}
};
int Integrity_string_coords[GR_NUM_RESOLUTIONS][2] = {
	{ // GR_640
		112, 372
	},
	{ // GR_1024
		112, 643
	}
};

// cargo scanning extents
int Cargo_scan_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		7, 364, 130, 109
	},
	{ // GR_1024
		7, 635, 130, 109
	}
};

// first element is time flashing expires, second element is time of next flash
int Targetbox_flash_timers[NUM_TBOX_FLASH_TIMERS][2];
int Targetbox_flash_flags;

// flag to indicate whether to show the extra information about a target 
// The HUD_config controls whether this can be shown... but the player can still toggle it on/off
// during the game.
int Targetbox_show_extra_info = 1;

int Targetbox_wire=0;

// Different target states.  This drives the text display right below the hull integrity on the targetbox.
#define TS_DIS		0
#define TS_OK		1
#define TS_DMG		2
#define TS_CRT		3

static int Last_ts;	// holds last target status.

void hud_blit_target_integrity(int disabled,int force_obj_num = -1);

// cut down long subsystem names to a more manageable length
char *hud_targetbox_truncate_subsys_name(char *outstr)
{	
	if(Lcl_gr){
		if ( strstr(outstr, "communication") )	{
			strcpy(outstr, "Komm");
		} else if ( !stricmp(outstr, "weapons") ) {
			strcpy(outstr, "Waffen");
		} else if ( strstr(outstr, "engine") || strstr(outstr, "Engine")) {
			strcpy(outstr, "Antrieb");
		} else if ( !stricmp(outstr, "sensors") ) {
			strcpy(outstr, "Sensoren");
		} else if ( strstr(outstr, "navigat") ) {
			strcpy(outstr, "Nav");
		} else if ( strstr(outstr, "fighterbay") || strstr(outstr, "Fighterbay") ) {
			strcpy(outstr, "J\x84gerhangar");
		} else if ( strstr(outstr, "missile") ) {
			strcpy(outstr, "Raketenwerfer");
		} else if ( strstr(outstr, "laser") || strstr(outstr, "turret") ) {
			strcpy(outstr, "Gesch\x81tzturm");
		} else if ( strstr(outstr, "Command Tower") || strstr(outstr, "Bridge") ) {
			strcpy(outstr, "Br\x81""cke");
		} else if ( strstr(outstr, "Barracks") ) {
			strcpy(outstr, "Quartiere");
		} else if ( strstr(outstr, "Reactor") ) {
			strcpy(outstr, "Reaktor");
		} else if ( strstr(outstr, "RadarDish") ) {
			strcpy(outstr, "Radarantenne");
		} else if (!stricmp(outstr, "Gas Collector")) {
			strcpy(outstr, "Sammler");
		} 
	} else if(Lcl_fr){	
		if ( strstr(outstr, "communication") )	{
			strcpy(outstr, "comm");
		} else if ( !stricmp(outstr, "weapons") ) {
			strcpy(outstr, "armes");
		} else if ( strstr(outstr, "engine") ) {
			strcpy(outstr, "moteur");
		} else if ( !stricmp(outstr, "sensors") ) {
			strcpy(outstr, "detecteurs");
		} else if ( strstr(outstr, "navi") ) {
			strcpy(outstr, "nav");
		} else if ( strstr(outstr, "missile") ) {
			strcpy(outstr, "lanceur de missiles");
		} else if ( strstr(outstr, "fighter") ) {
			strcpy(outstr, "baie de chasse");
		} else if ( strstr(outstr, "laser") || strstr(outstr, "turret") || strstr(outstr, "missile") ) {
			strcpy(outstr, "tourelle");
		} 
	} else {	
		if ( strstr(outstr, XSTR( "communication", 333)))	{
			strcpy( outstr, XSTR( "comm", 334) );
		} else if ( strstr(outstr, XSTR( "navigation", 335)))	{
			strcpy( outstr, XSTR( "nav", 336) );
		} else if (!stricmp(outstr, "Gas Collector")) {
			strcpy(outstr, "collector");
		}
	}

	return outstr;
}

//swich through the valid targetbox modes
void hud_targetbox_switch_wireframe_mode()
{
	//0==standard
	//1==wireframe only
	//2==wireframe with textures
	Targetbox_wire++;
		if (Targetbox_wire==3)
			Targetbox_wire=0;
}

// init a specific targetbox timer
void hud_targetbox_init_flash_timer(int index)
{
	Targetbox_flash_timers[index][0] = 1;
	Targetbox_flash_timers[index][1] = 1;
	Targetbox_flash_flags &= ~(1<<index);
}

// init the timers used to flash different parts of the targetbox.  This needs to get called whenever
// the current target changes.
void hud_targetbox_init_flash()
{
	hud_targetbox_init_flash_timer(TBOX_FLASH_NAME);
	hud_targetbox_init_flash_timer(TBOX_FLASH_CARGO);
	hud_targetbox_init_flash_timer(TBOX_FLASH_HULL);
	hud_targetbox_init_flash_timer(TBOX_FLASH_STATUS);
	hud_targetbox_init_flash_timer(TBOX_FLASH_SUBSYS);
	hud_targetbox_init_flash_timer(TBOX_FLASH_DOCKED);

	Last_ts = -1;
}

// set the color for flashing text
// input:	index	=>	item to flash
//				flash_fast	=>	optional param (default value 0), flash twice as fast
// exit:	1 =>	set bright color
//			0 =>	set default color
int hud_targetbox_maybe_flash(int index, int flash_fast)
{
	int draw_bright=0;

	// hud_set_default_color();
	hud_set_gauge_color(HUD_TARGET_MONITOR);
	if ( !timestamp_elapsed(Targetbox_flash_timers[index][0]) ) {
		if ( timestamp_elapsed(Targetbox_flash_timers[index][1]) ) {
			if ( flash_fast ) {
				Targetbox_flash_timers[index][1] = timestamp(fl2i(TBOX_FLASH_INTERVAL/2.0f));
			} else {
				Targetbox_flash_timers[index][1] = timestamp(TBOX_FLASH_INTERVAL);
			}
			Targetbox_flash_flags ^= (1<<index);	// toggle between default and bright frames
		}

		if ( Targetbox_flash_flags & (1<<index) ) {
			// hud_set_bright_color();
			hud_set_gauge_color(HUD_TARGET_MONITOR, HUD_C_BRIGHT);
			draw_bright=1;
		} else {			
			// hud_set_dim_color();
			hud_set_gauge_color(HUD_TARGET_MONITOR, HUD_C_DIM);
		}
	}

	return draw_bright;
}

// init all targetbox flash timers
void hud_targetbox_init_all_timers()
{
	int i;
	for ( i = 0; i < NUM_TBOX_FLASH_TIMERS; i++ ) {
		hud_targetbox_init_flash_timer(i);
	}

	Last_ts = -1;
}

// Initialize the data needed for the target view.  This is called from HUD_init() once per mission
void hud_targetbox_init()
{
	if (!Target_view_gauge_loaded) {
		Target_view_gauge.first_frame = bm_load_animation(Target_view_fname[gr_screen.res], &Target_view_gauge.num_frames);
		if ( Target_view_gauge.first_frame < 0 ) {
			Warning(LOCATION,"Cannot load hud ani: %s\n", Target_view_fname[gr_screen.res]);
		}
		Target_view_gauge_loaded = 1;
	}

	if (!Target_view_integrity_gauge_loaded) {
		Target_view_integrity_gauge.first_frame = bm_load_animation(Target_integ_fname[gr_screen.res], &Target_view_integrity_gauge.num_frames);
		if ( Target_view_integrity_gauge.first_frame < 0 ) {
			Warning(LOCATION,"Cannot load hud ani: %s\n", Target_integ_fname[gr_screen.res]);
		}
		Target_view_integrity_gauge_loaded = 1;
	}

	if (!Target_view_extra_loaded) {
		Target_view_extra.first_frame = bm_load_animation(Target_extra_fname[gr_screen.res], &Target_view_extra.num_frames);
		if ( Target_view_extra.first_frame < 0 ) {
			Warning(LOCATION,"Cannot load hud ani: %s\n", Target_extra_fname[gr_screen.res]);
		}
		Target_view_extra_loaded = 1;
	}

	hud_targetbox_init_all_timers();
}

// -------------------------------------------------------------------------------------
// hud_render_target_background()
//
// Common set up for drawing the background of the target monitor, for ships/debris/missiles
//
void hud_render_target_background()
{
	// blit the background frame
	hud_set_gauge_color(HUD_TARGET_MONITOR);

	GR_AABITMAP(Target_view_gauge.first_frame, Targetbox_coords[gr_screen.res][TBOX_BACKGROUND][0],Targetbox_coords[gr_screen.res][TBOX_BACKGROUND][1]);

	// blit the extra targeting info frame
	hud_set_gauge_color(HUD_TARGET_MONITOR_EXTRA_DATA);
}


// -------------------------------------------------------------------------------------
// hud_render_target_setup()
//
// Common set up for the 3d code for drawing the target monitor, for ships/debris/missiles
//
void hud_render_target_setup(vec3d *camera_eye, matrix *camera_orient, float zoom)
{
	// JAS: g3_start_frame uses clip_width and clip_height to determine the
	// size to render to.  Normally, you would set this by using gr_set_clip,
	// but because of the hacked in hud jittering, I couldn't.  So come talk
	// to me before modifying or reusing the following code. Thanks.

	int clip_width = Target_window_coords[gr_screen.res][2];
	int clip_height = Target_window_coords[gr_screen.res][3];

	gr_resize_screen_pos( &clip_width, &clip_height );

	gr_screen.clip_width = clip_width;
	gr_screen.clip_height = clip_height;
	g3_start_frame(1);		// Turn on zbuffering
	hud_save_restore_camera_data(1);
	g3_set_view_matrix( camera_eye, camera_orient, zoom);	
	model_set_detail_level(1);		// use medium detail level

	HUD_set_clip(Target_window_coords[gr_screen.res][0],Target_window_coords[gr_screen.res][1],Target_window_coords[gr_screen.res][2],Target_window_coords[gr_screen.res][3]);

	if (!Cmdline_nohtl) {
		gr_set_proj_matrix(Proj_fov, gr_screen.clip_aspect, Min_draw_distance, Max_draw_distance);
		gr_set_view_matrix(&Eye_position, &Eye_matrix);
	}

}

// -------------------------------------------------------------------------------------
// hud_render_target_close()
//
// Common clean-up after drawing the target monitor, for ships/debris/missiles
//
void hud_render_target_close()
{
	if (!Cmdline_nohtl) {
		gr_end_view_matrix();
		gr_end_proj_matrix();
	}

	g3_end_frame();
	hud_save_restore_camera_data(0);
}

// -------------------------------------------------------------------------------------
// hud_blit_target_foreground()
//
void hud_blit_target_foreground()
{
	hud_set_gauge_color(HUD_TARGET_MONITOR);

	GR_AABITMAP(Target_view_gauge.first_frame+1, Targetbox_coords[gr_screen.res][TBOX_BACKGROUND][0],Targetbox_coords[gr_screen.res][TBOX_BACKGROUND][1]);	
}

// -------------------------------------------------------------------------------------
// hud_get_target_strength()
//
// Get the shield and hull percentages for a given ship object
//
// input:	*objp		=>		pointer to ship object that you want strength values for
//				shields	=>		OUTPUT parameter:	percentage value of shields (0->1.0)
//				integrity =>	OUTPUT parameter: percentage value of integrity (0->1.0)
//
// Goober5000 - simplified
void hud_get_target_strength(object *objp, float *shields, float *integrity)
{
	*shields = get_shield_pct(objp);
	*integrity = get_hull_pct(objp);
}

// maybe draw the extra targeted ship information above the target monitor
void hud_targetbox_show_extra_ship_info(ship *target_shipp, object *target_objp)
{
	char outstr[256], tmpbuf[256];
	int has_orders = 0;
	int not_training;
	int extra_data_shown=0;

	hud_set_gauge_color(HUD_TARGET_MONITOR_EXTRA_DATA);

	not_training = !(The_mission.game_type & MISSION_TYPE_TRAINING);
	if ( not_training && (hud_gauge_active(HUD_TARGET_MONITOR_EXTRA_DATA)) && (Targetbox_show_extra_info) ) {
		// Print out current orders if the targeted ship is friendly
		// AL 12-26-97: only show orders and time to target for friendly ships
		// Backslash: actually let's consult the IFF table.  Maybe we want to show orders for certain teams, or hide orders for friendlies
		if ( ((Player_ship->team == target_shipp->team) || ((Iff_info[target_shipp->team].flags & IFFF_ORDERS_SHOWN) && !(Iff_info[target_shipp->team].flags & IFFF_ORDERS_HIDDEN)) ) && !(ship_get_SIF(target_shipp) & SIF_NOT_FLYABLE) ) {
			extra_data_shown=1;
			if ( ship_return_orders(outstr, sizeof(outstr), target_shipp) ) {
				gr_force_fit_string(outstr, 255, 162);
				has_orders = 1;
			} else {
				strcpy(outstr, XSTR( "no orders", 337));
			}
			
			emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_EXTRA_ORDERS][0], Targetbox_coords[gr_screen.res][TBOX_EXTRA_ORDERS][1], EG_TBOX_EXTRA1, outstr);			
		}

		if ( has_orders ) {
			snprintf(outstr, sizeof(outstr), XSTR( "time to: ", 338));
			if ( ship_return_time_to_goal(tmpbuf, sizeof(tmpbuf), target_shipp) ) {
				strcat(outstr, tmpbuf);
				
				emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_EXTRA_TIME][0], Targetbox_coords[gr_screen.res][TBOX_EXTRA_TIME][1], EG_TBOX_EXTRA2, outstr);				
			}
		}
	}

	// Print out dock status
	if ( object_is_docked(target_objp) )
	{
		// count the objects directly docked to me
		int dock_count = dock_count_direct_docked_objects(target_objp);

		// docked to only one object
		if (dock_count == 1)
		{
			snprintf(outstr, sizeof(outstr), XSTR("Docked: %s", 339), Ships[dock_get_first_docked_object(target_objp)->instance].ship_name);
			end_string_at_first_hash_symbol(outstr);
		}
		// docked to multiple objects
		else
		{
			snprintf(outstr, sizeof(outstr), XSTR("Docked: %d objects", -1), dock_count);
		}

		gr_force_fit_string(outstr, 255, 173);
		hud_targetbox_maybe_flash(TBOX_FLASH_DOCKED);
			
		emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_EXTRA_DOCK][0], Targetbox_coords[gr_screen.res][TBOX_EXTRA_DOCK][1], EG_TBOX_EXTRA3, outstr);			
		extra_data_shown=1;
	}

	if ( extra_data_shown ) {
		// hud_set_default_color();		

		GR_AABITMAP(Target_view_extra.first_frame, Targetbox_coords[gr_screen.res][TBOX_EXTRA][0],Targetbox_coords[gr_screen.res][TBOX_EXTRA][1]);		
	}
}

// Render a jump node on the target monitor
void hud_render_target_jump_node(object *target_objp)
{
	char			outstr[256];
	vec3d		obj_pos = ZERO_VECTOR;
	vec3d		camera_eye = ZERO_VECTOR;
	matrix		camera_orient = IDENTITY_MATRIX;
	vec3d		orient_vec, up_vector;
	float			factor, dist;
	int			hx, hy, w, h;

	if ( target_objp->jnp->is_hidden() ) {
		set_target_objnum( Player_ai, -1 );
		return;
	}

	if ( Detail.targetview_model )	{
		// take the forward orientation to be the vector from the player to the current target
		vm_vec_sub(&orient_vec, &target_objp->pos, &Player_obj->pos);
		vm_vec_normalize(&orient_vec);

		factor = target_objp->radius*4.0f;

		// use the player's up vector, and construct the viewers orientation matrix
		up_vector = Player_obj->orient.vec.uvec;
		vm_vector_2_matrix(&camera_orient,&orient_vec,&up_vector,NULL);

		// normalize the vector from the player to the current target, and scale by a factor to calculate
		// the objects position
		vm_vec_copy_scale(&obj_pos,&orient_vec,factor);

		hud_render_target_setup(&camera_eye, &camera_orient, 0.5f);
		target_objp->jnp->render( &obj_pos );
		hud_render_target_close();
	}
	
	HUD_reset_clip();
	hud_blit_target_foreground();
	hud_blit_target_integrity(1);
	// hud_set_default_color();
	hud_set_gauge_color(HUD_TARGET_MONITOR);

	emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_NAME][0], Targetbox_coords[gr_screen.res][TBOX_NAME][1], EG_TBOX_NAME, target_objp->jnp->get_name_ptr());	

	dist = vm_vec_dist_quick(&target_objp->pos, &Player_obj->pos);
	if ( Hud_unit_multiplier > 0.0f ) {	// use a different displayed distance scale
		dist = dist * Hud_unit_multiplier;
	}

	// account for hud shaking
	hx = fl2i(HUD_offset_x);
	hy = fl2i(HUD_offset_y);

	snprintf(outstr,sizeof(outstr), XSTR( "d: %.0f", 340), dist);
	hud_num_make_mono(outstr);
	gr_get_string_size(&w,&h,outstr);
	
	emp_hud_printf(Targetbox_coords[gr_screen.res][TBOX_DIST][0]+hx, Targetbox_coords[gr_screen.res][TBOX_DIST][1]+hy, EG_TBOX_DIST, outstr);	
}

// -------------------------------------------------------------------------------------
// hud_render_target_asteroid()
//
// Render a piece of asteroid on the target monitor
//
void hud_render_target_asteroid(object *target_objp)
{
	vec3d		obj_pos = ZERO_VECTOR;
	vec3d		camera_eye = ZERO_VECTOR;
	matrix		camera_orient = IDENTITY_MATRIX;
	asteroid		*asteroidp;
	vec3d		orient_vec, up_vector;
	int			target_team;
	float			time_to_impact, factor;	
	int			pof;

	int flags=0;									//draw flags for wireframe
	asteroidp = &Asteroids[target_objp->instance];

	target_team = obj_team(target_objp);

	pof = asteroidp->asteroid_subtype;
	
	time_to_impact = asteroid_time_to_impact(target_objp);

	if ( Detail.targetview_model )	{
		// take the forward orientation to be the vector from the player to the current target
		vm_vec_sub(&orient_vec, &target_objp->pos, &Player_obj->pos);
		vm_vec_normalize(&orient_vec);

		factor = 2*target_objp->radius;

		// use the player's up vector, and construct the viewers orientation matrix
		up_vector = Player_obj->orient.vec.uvec;
		vm_vector_2_matrix(&camera_orient,&orient_vec,&up_vector,NULL);

		// normalize the vector from the player to the current target, and scale by a factor to calculate
		// the objects position
		vm_vec_copy_scale(&obj_pos,&orient_vec,factor);

		hud_render_target_setup(&camera_eye, &camera_orient, 0.5f);
		model_clear_instance(Asteroid_info[asteroidp->asteroid_type].model_num[pof]);
		
		if (Targetbox_wire!=0)
		{
			if (time_to_impact>=0)
				model_set_outline_color(255,255,255);
			else
				model_set_outline_color(64,64,0);

			flags = (Cmdline_nohtl) ? MR_SHOW_OUTLINE : MR_SHOW_OUTLINE_HTL;

			if (Targetbox_wire==1)
				flags |=MR_NO_POLYS;
		}

		model_render(Asteroid_info[asteroidp->asteroid_type].model_num[pof], &target_objp->orient, &obj_pos, flags |MR_NO_LIGHTING | MR_LOCK_DETAIL | MR_NO_FOGGING );
		hud_render_target_close();
	}

	HUD_reset_clip();
	hud_blit_target_foreground();
	hud_blit_target_integrity(1);
	// hud_set_default_color();
	hud_set_gauge_color(HUD_TARGET_MONITOR);

	// hud print type of Asteroid (debris)
	char hud_name[64];
	switch (asteroidp->asteroid_type) {
		case ASTEROID_TYPE_SMALL:
		case ASTEROID_TYPE_MEDIUM:
		case ASTEROID_TYPE_LARGE:
			strcpy(hud_name, NOX("asteroid"));
			break;

		default:
			snprintf(hud_name, sizeof(hud_name), NOX("%s debris"), Species_info[(asteroidp->asteroid_type / NUM_DEBRIS_SIZES) - 1].species_name);
			break;
	}

	emp_hud_printf(Targetbox_coords[gr_screen.res][TBOX_NAME][0], Targetbox_coords[gr_screen.res][TBOX_NAME][1], EG_TBOX_NAME, hud_name);	
	

	if ( time_to_impact >= 0.0f ) {
		emp_hud_printf(Targetbox_coords[gr_screen.res][TBOX_CLASS][0], Targetbox_coords[gr_screen.res][TBOX_CLASS][1], EG_TBOX_CLASS, NOX("impact: %.1f sec"), time_to_impact);	
	}
}

//from aicode.cpp. Less include...problems...this way.
extern bool turret_weapon_has_flags(ship_weapon *swp, int flags);
extern bool turret_weapon_has_flags2(ship_weapon *swp, int flags);
extern bool turret_weapon_has_subtype(ship_weapon *swp, int subtype);
void get_turret_subsys_name(ship_weapon *swp, char *outstr, size_t outstr_size)
{
	Assert(swp != NULL);	// Goober5000 //WMC
	//Assert(system_info->type == SUBSYSTEM_TURRET);

	//WMC - find the first weapon, if there is one
	if (swp->num_primary_banks || swp->num_secondary_banks) {
		// check if beam or flak using weapon flags
		if (turret_weapon_has_flags(swp, WIF_BEAM)) {
			snprintf(outstr, outstr_size, "%s", XSTR("Beam turret", 1567));
		}else if (turret_weapon_has_flags(swp, WIF_FLAK)) {
			snprintf(outstr, outstr_size, "%s", XSTR("Flak turret", 1566));
		} else {

			if (!turret_weapon_has_subtype(swp, WP_MISSILE) && turret_weapon_has_subtype(swp, WP_LASER)) {
				// ballistic too! - Goober5000
				if (turret_weapon_has_flags2(swp, WIF2_BALLISTIC))
				{
					snprintf(outstr, outstr_size, "%s", XSTR("Turret", 1487));
				}
				// the TVWP has some primaries flagged as bombs
				else if (turret_weapon_has_flags(swp, WIF_BOMB))
				{
					snprintf(outstr, outstr_size, "%s", XSTR("Missile lnchr", 1569));
				}
				else
				{
					snprintf(outstr, outstr_size, "%s", XSTR("Laser turret", 1568));
				}
			} else if (turret_weapon_has_subtype(swp, WP_MISSILE)) {
				snprintf(outstr, outstr_size, "%s", XSTR("Missile lnchr", 1569));
			} else {
				// Illegal subtype
				Int3();
				snprintf(outstr, outstr_size, "%s", XSTR("Turret", 1487));
			}
		}
	} else if(swp->num_tertiary_banks) {
		//TODO: add tertiary turret code stuff here
		snprintf(outstr, outstr_size, "%s", NOX("Unknown"));
	} else {
		// This should not happen
		snprintf(outstr, outstr_size, "%s", NOX("Unused"));
	}
}

// -------------------------------------------------------------------------------------
// hud_render_target_ship_info()
//
// Render the data for a ship on the target monitor.  Called by hud_render_target_ship().
//
void hud_render_target_ship_info(object *target_objp)
{
	ship			*target_shipp;
	ship_info	*target_sip;
	int			w, h, screen_integrity = 1;
	char			outstr[NAME_LENGTH+1];
	float			ship_integrity, shield_strength;

	Assert(target_objp);	// Goober5000
	Assert(target_objp->type == OBJ_SHIP);
	target_shipp = &Ships[target_objp->instance];
	target_sip = &Ship_info[target_shipp->ship_info_index];

	// set up colors
	if ( hud_gauge_maybe_flash(HUD_TARGET_MONITOR) == 1 ) {
		hud_set_iff_color(target_objp, 1);
	} else {
		// Print out ship name, with wing name if it exists
		if ( hud_targetbox_maybe_flash(TBOX_FLASH_NAME) ) {
			hud_set_iff_color(target_objp, 1);
		} else {
			hud_set_iff_color(target_objp);
		}
	}

	// print lines
	hud_stuff_ship_name(outstr, target_shipp);
	emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_NAME][0], Targetbox_coords[gr_screen.res][TBOX_NAME][1], EG_TBOX_NAME, outstr);	
	hud_stuff_ship_class(outstr, target_shipp);
	emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_CLASS][0], Targetbox_coords[gr_screen.res][TBOX_CLASS][1], EG_TBOX_CLASS, outstr);
	hud_stuff_ship_callsign(outstr, target_shipp);
	emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_CARGO][0], Targetbox_coords[gr_screen.res][TBOX_CARGO][1], EG_TBOX_CARGO, outstr);				

	// ----------

	ship_integrity = 1.0f;
	shield_strength = 1.0f;
	hud_get_target_strength(target_objp, &shield_strength, &ship_integrity);

	// convert to values of 0->100
	shield_strength *= 100.0f;
	ship_integrity *= 100.0f;

	screen_integrity = fl2i(ship_integrity+0.5f);
	if ( screen_integrity == 0 ) {
		if ( ship_integrity > 0 ) {
			screen_integrity = 1;
		}
	}
	// Print out right-justified integrity
	snprintf(outstr, sizeof(outstr), XSTR( "%d%%", 341), screen_integrity);
	gr_get_string_size(&w,&h,outstr);

	if ( hud_gauge_maybe_flash(HUD_TARGET_MONITOR) == 1 ) {
		// hud_set_bright_color();
		hud_set_gauge_color(HUD_TARGET_MONITOR, HUD_C_BRIGHT);
	} else {
		hud_targetbox_maybe_flash(TBOX_FLASH_HULL);
	}

	emp_hud_printf(Targetbox_coords[gr_screen.res][TBOX_HULL][0]-w, Targetbox_coords[gr_screen.res][TBOX_HULL][1], EG_TBOX_HULL, "%s", outstr);	
	hud_set_gauge_color(HUD_TARGET_MONITOR);

	// print out the targeted sub-system and % integrity
	if (Player_ai->targeted_subsys != NULL) {
		shield_strength = Player_ai->targeted_subsys->current_hits/Player_ai->targeted_subsys->max_hits * 100.0f;
		screen_integrity = fl2i(shield_strength+0.5f);

		if ( screen_integrity < 0 ) {
			screen_integrity = 0;
		}

		if ( screen_integrity == 0 ) {
			if ( shield_strength > 0 ) {
				screen_integrity = 1;
			}
		}

		// Goober5000 - don't flash if this subsystem can't be destroyed
		if ( ship_subsys_takes_damage(Player_ai->targeted_subsys) )
		{
			if ( screen_integrity <= 0 ){
				hud_targetbox_start_flash(TBOX_FLASH_SUBSYS);	// need to flash 0% continuously
				hud_targetbox_maybe_flash(TBOX_FLASH_SUBSYS);
			}
		}

		// PRINT SUBSYS NAME
		// hud_set_default_color();
		// get turret subsys name
		if (Player_ai->targeted_subsys->system_info->type == SUBSYSTEM_TURRET && !ship_subsys_has_instance_name(Player_ai->targeted_subsys)) {
			get_turret_subsys_name(&Player_ai->targeted_subsys->weapons, outstr, sizeof(outstr));
		} else {
			snprintf(outstr, sizeof(outstr), "%s", ship_subsys_get_name(Player_ai->targeted_subsys));
		}
		hud_targetbox_truncate_subsys_name(outstr);
		gr_printf(Target_window_coords[gr_screen.res][0]+2, Target_window_coords[gr_screen.res][1]+Target_window_coords[gr_screen.res][3]-h, outstr);

		// AL 23-3-98: Fighter bays are a special case.  Player cannot destroy them, so don't
		//					show the subsystem strength
		// Goober5000: don't display any strength if we can't destroy this subsystem - but sometimes
		// fighterbays can be destroyed
		if ( ship_subsys_takes_damage(Player_ai->targeted_subsys) )
		{
			snprintf(outstr, sizeof(outstr), XSTR( "%d%%", 341),screen_integrity);
			gr_get_string_size(&w,&h,outstr);
			gr_printf(Target_window_coords[gr_screen.res][0]+Target_window_coords[gr_screen.res][2]-w-1, Target_window_coords[gr_screen.res][1]+Target_window_coords[gr_screen.res][3] - h, "%s", outstr);
		}

		hud_set_gauge_color(HUD_TARGET_MONITOR);
	}

	// print out 'disabled' on the monitor if the target is disabled
	if ( (target_shipp->flags & SF_DISABLED) || (ship_subsys_disrupted(target_shipp, SUBSYSTEM_ENGINE)) ) {
		if ( target_shipp->flags & SF_DISABLED ) {
			snprintf(outstr, sizeof(outstr), XSTR( "DISABLED", 342));
		} else {
			snprintf(outstr, sizeof(outstr), XSTR( "DISRUPTED", 343));
		}
		gr_get_string_size(&w,&h,outstr);
		gr_printf(Target_window_coords[gr_screen.res][0]+Target_window_coords[gr_screen.res][2]/2 - w/2 - 1, Target_window_coords[gr_screen.res][1]+Target_window_coords[gr_screen.res][3] - 2*h, "%s", outstr);
	}

	hud_targetbox_show_extra_ship_info(target_shipp, target_objp);
}

// call to draw the integrity bar that is on the right of the target monitor
void hud_blit_target_integrity(int disabled,int force_obj_num)
{
	object	*objp;
	int		clip_h,w,h;
	char		buf[16];
	int		current_ts;

	if ( Target_view_integrity_gauge.first_frame == -1 ) 
		return;

	if ( disabled ) {
		GR_AABITMAP(Target_view_integrity_gauge.first_frame, Integrity_bar_coords[gr_screen.res][0], Integrity_bar_coords[gr_screen.res][1]);
		return;
	}

	if(force_obj_num == -1){
		Assert(Player_ai->target_objnum >= 0 );
		objp = &Objects[Player_ai->target_objnum];
	} else {
		// Goober5000: what the... this probably should be changed
		//objp = &Objects[Player_ai->target_objnum];
		objp = &Objects[force_obj_num];
	}

	clip_h = fl2i( (1 - Pl_target_integrity) * Integrity_bar_coords[gr_screen.res][3] );

	// print out status of ship
	if ( (Ships[objp->instance].flags & SF_DISABLED) || (ship_subsys_disrupted(&Ships[objp->instance], SUBSYSTEM_ENGINE)) ) {
		snprintf(buf,sizeof(buf), XSTR( "dis", 344));
		current_ts = TS_DIS;
	} else {
		if ( Pl_target_integrity > 0.9 ) {
			snprintf(buf, sizeof(buf), XSTR( "ok", 345));
			current_ts = TS_OK;
		} else if ( Pl_target_integrity > 0.2 ) {
			snprintf(buf, sizeof(buf), XSTR( "dmg", 346));
			current_ts = TS_DMG;
		} else {
			snprintf(buf, sizeof(buf), XSTR( "crt", 347));
			current_ts = TS_CRT;
		}
	}

	if ( Last_ts != -1 && current_ts != Last_ts ) {
		hud_targetbox_start_flash(TBOX_FLASH_STATUS);
	}
	Last_ts = current_ts;

	hud_targetbox_maybe_flash(TBOX_FLASH_STATUS);
	
	emp_hud_string(Integrity_string_coords[gr_screen.res][0], Integrity_string_coords[gr_screen.res][1], EG_TBOX_INTEG, buf);	

	hud_set_gauge_color(HUD_TARGET_MONITOR);

	bm_get_info(Target_view_integrity_gauge.first_frame,&w,&h);
	
	if ( clip_h > 0 ) {
		// draw the dark portion
		GR_AABITMAP_EX(Target_view_integrity_gauge.first_frame, Integrity_bar_coords[gr_screen.res][0], Integrity_bar_coords[gr_screen.res][1], w, clip_h,0,0);		
	}

	if ( clip_h <= Integrity_bar_coords[gr_screen.res][3] ) {
		// draw the bright portion
		GR_AABITMAP_EX(Target_view_integrity_gauge.first_frame+1, Integrity_bar_coords[gr_screen.res][0], Integrity_bar_coords[gr_screen.res][1]+clip_h,w,h-clip_h,0,clip_h);		
	}
}

// determine if the subsystem is in line-of sight, without taking into account whether the player ship is
// facing the subsystem
int hud_targetbox_subsystem_in_view(object *target_objp, int *sx, int *sy)
{
	ship_subsys	*subsys;
	vec3d		subobj_pos;
	vertex		subobj_vertex;
	int			rval = -1;
	polymodel	*pm;

	subsys = Player_ai->targeted_subsys;
	if (subsys != NULL ) {
		vm_vec_unrotate(&subobj_pos, &subsys->system_info->pnt, &target_objp->orient);
		vm_vec_add2(&subobj_pos, &target_objp->pos);

		// is it subsystem in view
		if ( Player->subsys_in_view == -1 ) {
			rval = ship_subsystem_in_sight(target_objp, subsys, &View_position, &subobj_pos, 0);
		} else {
			rval =  Player->subsys_in_view;
		}

		// get screen coords, adjusting for autocenter
		Assert(target_objp->type == OBJ_SHIP);
		if (target_objp->type == OBJ_SHIP) {
			pm = model_get(Ship_info[Ships[target_objp->instance].ship_info_index].model_num);
			if (pm->flags & PM_FLAG_AUTOCEN) {
				vec3d temp, delta;
				vm_vec_copy_scale(&temp, &pm->autocenter, -1.0f);
				vm_vec_unrotate(&delta, &temp, &target_objp->orient);
				vm_vec_add2(&subobj_pos, &delta);
			}
		}

		g3_rotate_vertex(&subobj_vertex, &subobj_pos);
		g3_project_vertex(&subobj_vertex);
		*sx = (int) subobj_vertex.sx;
		*sy = (int) subobj_vertex.sy;
		gr_unsize_screen_pos( sx, sy );
	}

	return rval;
}

void hud_update_cargo_scan_sound()
{
	if ( Player->cargo_inspect_time <= 0  ) {
		player_stop_cargo_scan_sound();
		return;
	}
	player_maybe_start_cargo_scan_sound();

}

// If the player is scanning for cargo, draw some cool scanning lines on the target monitor
void hud_maybe_render_cargo_scan(ship_info *target_sip)
{
	int x1, y1, x2, y2;
	int scan_time;				// time required to scan ship

	if ( Player->cargo_inspect_time <= 0  ) {
		return;
	}

	scan_time = target_sip->scan_time;
	// hud_set_default_color();
	hud_set_gauge_color(HUD_TARGET_MONITOR, HUD_C_BRIGHT);

	// draw horizontal scan line
	x1 = Cargo_scan_coords[gr_screen.res][0];
	y1 = fl2i(0.5f + Cargo_scan_coords[gr_screen.res][1] + ( (i2fl(Player->cargo_inspect_time) / scan_time) * Cargo_scan_coords[gr_screen.res][3] ));
	x2 = x1 + Cargo_scan_coords[gr_screen.res][2];

	gr_line(x1, y1, x2, y1);

	// RT Changed this to be optional
	if(Cmdline_dualscanlines) {
		// added 2nd horizontal scan line - phreak
		y1 = fl2i(Cargo_scan_coords[gr_screen.res][1] + Cargo_scan_coords[gr_screen.res][3] - ( (i2fl(Player->cargo_inspect_time) / scan_time) * Cargo_scan_coords[gr_screen.res][3] ));
		gr_line(x1, y1, x2, y1);
	}

	// draw vertical scan line
	x1 = fl2i(0.5f + Cargo_scan_coords[gr_screen.res][0] + ( (i2fl(Player->cargo_inspect_time) / scan_time) * Cargo_scan_coords[gr_screen.res][2] ));
	y1 = Cargo_scan_coords[gr_screen.res][1];
	y2 = y1 + Cargo_scan_coords[gr_screen.res][3];

	gr_line(x1, y1-3, x1, y2-1);

	// RT Changed this to be optional
	if(Cmdline_dualscanlines) {
		// added 2nd vertical scan line - phreak
		x1 = fl2i(0.5f + Cargo_scan_coords[gr_screen.res][2] + Cargo_scan_coords[gr_screen.res][0] - ( (i2fl(Player->cargo_inspect_time) / scan_time) * Cargo_scan_coords[gr_screen.res][2] ));
		gr_line(x1, y1-3, x1, y2-1);
	}
}

// Get the eye position for an object at the origin, called from hud_render_target_ship()
// input:	eye_pos		=>	Global pos for eye (output parameter)
//			orient		=>	Orientation of object at the origin
void hud_targetbox_get_eye(vec3d *eye_pos, matrix *orient, int ship_num)
{
	polymodel	*pm;
	eye			*ep;
	vec3d		origin = ZERO_VECTOR;

	pm = model_get(Ship_info[Ships[ship_num].ship_info_index].model_num);

	// If there is no eye, don't do anything
	if ( pm->n_view_positions == 0 ) {
		return;
	}

	ep = &(pm->view_positions[Ships[ship_num].current_viewpoint]);

	model_find_world_point( eye_pos, &ep->pnt, pm->id, ep->parent, orient, &origin );
}

// -------------------------------------------------------------------------------------
// hud_render_target_ship()
//
// Render a ship to the target monitor
//
void hud_render_target_ship(object *target_objp)
{
	vec3d		obj_pos = ZERO_VECTOR;
	vec3d		camera_eye = ZERO_VECTOR;
	matrix		camera_orient = IDENTITY_MATRIX;
	ship		*target_shipp;
	ship_info	*target_sip;
	vec3d		orient_vec, up_vector;
	int			sx, sy;
	int			subsys_in_view;
	float		factor;
	
	target_shipp	= &Ships[target_objp->instance];
	target_sip		= &Ship_info[target_shipp->ship_info_index];

	int flags=0;
	if ( Detail.targetview_model )	{
		// take the forward orientation to be the vector from the player to the current target
		vm_vec_sub(&orient_vec, &target_objp->pos, &Player_obj->pos);
		vm_vec_normalize(&orient_vec);

		factor = -target_sip->closeup_pos.xyz.z;

		// use the player's up vector, and construct the viewers orientation matrix
		up_vector = Player_obj->orient.vec.uvec;
		vm_vector_2_matrix(&camera_orient,&orient_vec,&up_vector,NULL);

		// normalize the vector from the player to the current target, and scale by a factor to calculate
		// the objects position
		vm_vec_copy_scale(&obj_pos,&orient_vec,factor);

		// set camera eye to eye of ship relative to origin
	//	hud_targetbox_get_eye(&camera_eye, &camera_orient, Player_obj->instance);

		// RT, changed scaling here
		hud_render_target_setup(&camera_eye, &camera_orient, target_sip->closeup_zoom);
		// model_clear_instance(target_shipp->modelnum);
		ship_model_start( target_objp );

		if (Targetbox_wire!=0)
		{
			int is_bright = 1;

			if (ship_is_tagged(target_objp))
				model_set_outline_color_fast(iff_get_color(IFF_COLOR_TAGGED, is_bright));
			else
			{
				model_set_outline_color_fast(iff_get_color_by_team(target_shipp->team, Player_ship->team, is_bright));
			}

			flags = (Cmdline_nohtl) ? MR_SHOW_OUTLINE : MR_SHOW_OUTLINE_HTL;

			if (Targetbox_wire==1)
				flags |=MR_NO_POLYS;
		}

		if (target_sip->hud_target_lod >= 0) {
			model_set_detail_level(target_sip->hud_target_lod);
		}

		// maybe render a special hud-target-only model
		if(target_sip->model_num_hud >= 0){
			model_render( target_sip->model_num_hud, &target_objp->orient, &obj_pos, flags | MR_NO_LIGHTING | MR_LOCK_DETAIL | MR_AUTOCENTER | MR_NO_FOGGING);
		} else {
			model_render( target_sip->model_num, &target_objp->orient, &obj_pos, flags | MR_NO_LIGHTING | MR_LOCK_DETAIL | MR_AUTOCENTER | MR_NO_FOGGING, -1, -1, target_shipp->ship_replacement_textures);
		}
		ship_model_stop( target_objp );

		sx = 0;
		sy = 0;
		// check if subsystem target has changed
		if ( Player_ai->targeted_subsys == Player_ai->last_subsys_target ) {
			vec3d save_pos;
			save_pos = target_objp->pos;
			target_objp->pos = obj_pos;
			subsys_in_view = hud_targetbox_subsystem_in_view(target_objp, &sx, &sy);
			target_objp->pos = save_pos;

			if ( subsys_in_view != -1 ) {

				// AL 29-3-98: If subsystem is destroyed, draw gray brackets
				// Goober5000 - hm, caught a tricky bug for destroyable fighterbays
				if ( (Player_ai->targeted_subsys->current_hits <= 0) && ship_subsys_takes_damage(Player_ai->targeted_subsys) ) {
					gr_set_color_fast(iff_get_color(IFF_COLOR_MESSAGE, 1));
				} else {
					hud_set_iff_color( target_objp, 1 );
				}

				if ( subsys_in_view ) {
					draw_brackets_square_quick(sx - 10, sy - 10, sx + 10, sy + 10);
				} else {
					draw_brackets_diamond_quick(sx - 10, sy - 10, sx + 10, sy + 10);
				}
			}
		}
		hud_render_target_close();
	}
	HUD_reset_clip();
	hud_blit_target_foreground();
	hud_blit_target_integrity(0,OBJ_INDEX(target_objp));

	hud_set_gauge_color(HUD_TARGET_MONITOR);

	hud_render_target_ship_info(target_objp);
	hud_maybe_render_cargo_scan(target_sip);
}

// -------------------------------------------------------------------------------------
// hud_render_target_debris()
//
// Render a piece of debris on the target monitor
//
void hud_render_target_debris(object *target_objp)
{
	vec3d	obj_pos = ZERO_VECTOR;
	vec3d	camera_eye = ZERO_VECTOR;
	matrix	camera_orient = IDENTITY_MATRIX;
	debris	*debrisp;
	vec3d	orient_vec, up_vector;
	int		target_team;
	float		factor;	
	int flags=0;

	debrisp = &Debris[target_objp->instance];

	//target_sip = &Ship_info[debrisp->ship_info_index];
	target_team = obj_team(target_objp);

	if ( Detail.targetview_model )	{
		// take the forward orientation to be the vector from the player to the current target
		vm_vec_sub(&orient_vec, &target_objp->pos, &Player_obj->pos);
		vm_vec_normalize(&orient_vec);

		factor = 2*target_objp->radius;

		// use the player's up vector, and construct the viewers orientation matrix
		up_vector = Player_obj->orient.vec.uvec;
		vm_vector_2_matrix(&camera_orient,&orient_vec,&up_vector,NULL);

		// normalize the vector from the player to the current target, and scale by a factor to calculate
		// the objects position
		vm_vec_copy_scale(&obj_pos,&orient_vec,factor);

		if (Targetbox_wire!=0)
		{
			model_set_outline_color(255,255,255);
			flags = (Cmdline_nohtl) ? MR_SHOW_OUTLINE : MR_SHOW_OUTLINE_HTL;

			if (Targetbox_wire==1)
				flags |=MR_NO_POLYS;
		}
		hud_render_target_setup(&camera_eye, &camera_orient, 0.5f);
		model_clear_instance(debrisp->model_num);

		// This calls the colour that doesnt get reset
		submodel_render( debrisp->model_num, debrisp->submodel_num, &target_objp->orient, &obj_pos, flags | MR_NO_LIGHTING | MR_LOCK_DETAIL | MR_NO_FOGGING );
		hud_render_target_close();
	}

	HUD_reset_clip();
	hud_blit_target_foreground();
	hud_blit_target_integrity(1);

	// print out ship class that debris came from
	char printable_ship_class[NAME_LENGTH];
	if (debrisp->parent_alt_name >= 0)
		mission_parse_lookup_alt_index(debrisp->parent_alt_name, printable_ship_class);
	else
		strcpy(printable_ship_class, Ship_info[debrisp->ship_info_index].name);

	end_string_at_first_hash_symbol(printable_ship_class);

	emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_CLASS][0], Targetbox_coords[gr_screen.res][TBOX_CLASS][1], EG_TBOX_CLASS, printable_ship_class);	
	emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_NAME][0], Targetbox_coords[gr_screen.res][TBOX_NAME][1], EG_TBOX_NAME, XSTR( "debris", 348));	
}

// -------------------------------------------------------------------------------------
// hud_render_target_weapon()
//
// Render a missile or a missile view to the target monitor
//
void hud_render_target_weapon(object *target_objp)
{
	vec3d		obj_pos = ZERO_VECTOR;
	vec3d		camera_eye = ZERO_VECTOR;
	matrix		camera_orient = IDENTITY_MATRIX;
	vec3d		orient_vec, up_vector;
	weapon_info	*target_wip = NULL;
	weapon		*wp = NULL;
	object		*viewer_obj, *viewed_obj;
	int *replacement_textures = NULL;
	int			target_team, is_homing, is_player_missile, missile_view, viewed_model_num, hud_target_lod, w, h;
	float			factor;
	char			outstr[100];				// temp buffer
	int flags=0;

	target_team = obj_team(target_objp);

	wp = &Weapons[target_objp->instance];
	target_wip = &Weapon_info[wp->weapon_info_index];

	is_homing = FALSE;
	if ( target_wip->wi_flags & WIF_HOMING && wp->homing_object != &obj_used_list )
		is_homing = TRUE;

	is_player_missile = FALSE;
	if ( target_objp->parent_sig == Player_obj->signature ) {
		is_player_missile = TRUE;
	}

	if ( Detail.targetview_model )	{

		viewer_obj			= Player_obj;
		viewed_obj			= target_objp;
		missile_view		= FALSE;
		viewed_model_num	= target_wip->model_num;
		hud_target_lod		= target_wip->hud_target_lod;
		if ( is_homing && is_player_missile ) {
			ship *homing_shipp = &Ships[wp->homing_object->instance];
			ship_info *homing_sip = &Ship_info[homing_shipp->ship_info_index];

			viewer_obj			= target_objp;
			viewed_obj			= wp->homing_object;
			missile_view		= TRUE;
			viewed_model_num	= homing_sip->model_num;
			replacement_textures = homing_shipp->ship_replacement_textures;
			hud_target_lod		= homing_sip->hud_target_lod;
		}

		if (Targetbox_wire!=0)
		{
			int is_bright = 0;

			model_set_outline_color_fast(iff_get_color_by_team(target_team, Player_ship->team, is_bright));

			flags = (Cmdline_nohtl) ? MR_SHOW_OUTLINE : MR_SHOW_OUTLINE_HTL;

			if (Targetbox_wire==1)
				flags |=MR_NO_POLYS;
		}
			

		// take the forward orientation to be the vector from the player to the current target
		vm_vec_sub(&orient_vec, &viewed_obj->pos, &viewer_obj->pos);
		vm_vec_normalize(&orient_vec);

		if ( missile_view == FALSE )
			factor = 2*target_objp->radius;
		else
			factor = vm_vec_dist_quick(&viewer_obj->pos, &viewed_obj->pos);

		// use the viewer's up vector, and construct the viewers orientation matrix
		up_vector = viewer_obj->orient.vec.uvec;
		vm_vector_2_matrix(&camera_orient,&orient_vec,&up_vector,NULL);

		// normalize the vector from the viewer to the viwed target, and scale by a factor to calculate
		// the objects position
		vm_vec_copy_scale(&obj_pos,&orient_vec,factor);

		hud_render_target_setup(&camera_eye, &camera_orient, View_zoom/3);
		model_clear_instance(viewed_model_num);

		if (hud_target_lod >= 0) {
			model_set_detail_level(hud_target_lod);
		}

		model_render( viewed_model_num, &viewed_obj->orient, &obj_pos, flags | MR_NO_LIGHTING | MR_LOCK_DETAIL | MR_AUTOCENTER | MR_IS_MISSILE | MR_NO_FOGGING, -1, -1, replacement_textures);
		hud_render_target_close();
	}

	HUD_reset_clip();
	if ( is_homing == TRUE ) {
		hud_blit_target_foreground();
	} else {
		hud_blit_target_foreground();
	}

	hud_blit_target_integrity(1);
	// hud_set_default_color();
	hud_set_gauge_color(HUD_TARGET_MONITOR);

	// print out the weapon class name
	snprintf( outstr, sizeof(outstr), "%s", target_wip->name );
	gr_get_string_size(&w,&h,outstr);

	// drop name past the # sign
	end_string_at_first_hash_symbol(outstr);			

	emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_NAME][0], Targetbox_coords[gr_screen.res][TBOX_NAME][1], EG_TBOX_NAME, outstr);	

	// If a homing weapon, show time to impact
	if ( is_homing ) {
		float dist, speed;

		dist = vm_vec_dist(&target_objp->pos, &wp->homing_object->pos);
		speed = vm_vec_mag(&target_objp->phys_info.vel);
		if ( speed > 0 ) {
			snprintf(outstr, sizeof(outstr), NOX("impact: %.1f sec"), dist/speed);
		} else {
			snprintf(outstr, sizeof(outstr), XSTR( "unknown", 349));
		}

		emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_CLASS][0], Targetbox_coords[gr_screen.res][TBOX_CLASS][1], EG_TBOX_CLASS, outstr);		
	}
}

// -------------------------------------------------------------------------------------
// hud_render_target_model() will render the target in the small targetting box.  The box
// is then shaded to give a monochrome effect
//
void hud_render_target_model()
{
	object	*target_objp;

	if ( !hud_gauge_active(HUD_TARGET_MONITOR) )
		return;

	if ( Player_ai->target_objnum == -1)
		return;

	if ( Target_static_playing ) 
		return;

	target_objp = &Objects[Player_ai->target_objnum];

	// Draw the background frame
	hud_render_target_background();

	switch ( target_objp->type ) {
		case OBJ_SHIP:
			hud_render_target_ship(target_objp);
			break;
	
		case OBJ_DEBRIS:
			hud_render_target_debris(target_objp);
			break;

		case OBJ_WEAPON:
			hud_render_target_weapon(target_objp);
			break;

		case OBJ_ASTEROID:
			hud_render_target_asteroid(target_objp);
			break;

		case OBJ_JUMP_NODE:
			hud_render_target_jump_node(target_objp);
			break;

		default:
			// Error(LOCATION, "Trying to show object type %d on target monitor\n", target_objp->type);
			hud_cease_targeting();
			break;
	} // end switch
}

void hud_cargo_scan_update(object *targetp, float frametime)
{
	char outstr[256];						// temp buffer for sprintf'ing hud output
	int hx, hy;

	// Account for HUD shaking
	hx = fl2i(HUD_offset_x);
	hy = fl2i(HUD_offset_y);

	// display cargo inspection status
	if ( targetp->type == OBJ_SHIP ) {
		if ( player_inspect_cargo(frametime, outstr) ) {
			if ( hud_gauge_active(HUD_TARGET_MONITOR) ) {
				if ( Player->cargo_inspect_time > 0 ) {
					hud_targetbox_start_flash(TBOX_FLASH_CARGO);
				}

				// Print out what the cargo is
				if ( hud_gauge_maybe_flash(HUD_TARGET_MONITOR) == 1 ) {
					// hud_set_bright_color();
					hud_set_gauge_color(HUD_TARGET_MONITOR, HUD_C_BRIGHT);
				} else {
					hud_targetbox_maybe_flash(TBOX_FLASH_CARGO);
				}

				emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_CARGO][0]+hx, Targetbox_coords[gr_screen.res][TBOX_CARGO][1]+hy, EG_TBOX_CARGO, outstr);				
				hud_set_gauge_color(HUD_TARGET_MONITOR);
			}
		}
	}	// end if (is_ship)

}

// -----------------------------------------------------------------------------------
// hud_show_target_data() will display the data about the target in and
// around the targetting window
//
void hud_show_target_data(float frametime)
{
	char outstr[256];						// temp buffer for sprintf'ing hud output
	int w,h;									// width and height of string about to print
	object		*target_objp;
	ship			*shipp = NULL;
	debris		*debrisp = NULL;
	ship_info	*sip = NULL;
	int is_ship = 0;
	float		displayed_target_distance, displayed_target_speed, current_target_distance, current_target_speed;

	hud_set_gauge_color(HUD_TARGET_MONITOR);

	target_objp = &Objects[Player_ai->target_objnum];

	current_target_distance = Player_ai->current_target_distance;

	if ( Hud_unit_multiplier > 0.0f ) {	// use a different displayed distance scale
		displayed_target_distance = current_target_distance * Hud_unit_multiplier;
	} else {
		displayed_target_distance = current_target_distance;
	}

	switch( Objects[Player_ai->target_objnum].type ) {
		case OBJ_SHIP:
			shipp = &Ships[target_objp->instance];
			sip = &Ship_info[shipp->ship_info_index];
			is_ship = 1;
			break;

		case OBJ_DEBRIS:
			debrisp = &Debris[target_objp->instance]; 
			sip = &Ship_info[debrisp->ship_info_index];
			break;

		case OBJ_WEAPON:
			sip = NULL;
			break;

		case OBJ_ASTEROID:
			sip = NULL;
			break;

		case OBJ_JUMP_NODE:
			return;

		default:
			Int3();	// can't happen
			break;
	}

	int hx, hy;

	// Account for HUD shaking
	hx = fl2i(HUD_offset_x);
	hy = fl2i(HUD_offset_y);

	// print out the target distance and speed
	snprintf(outstr, sizeof(outstr),XSTR( "d: %.0f%s", 350), displayed_target_distance, modifiers[Player_ai->current_target_dist_trend]);

	hud_num_make_mono(outstr);
	gr_get_string_size(&w,&h,outstr);

	emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_DIST][0]+hx, Targetbox_coords[gr_screen.res][TBOX_DIST][1]+hy, EG_TBOX_DIST, outstr);	

#if 0
	current_target_speed = vm_vec_dist(&target_objp->pos, &target_objp->last_pos) / frametime;
#endif
	// 7/28/99 DKA: Do not use vec_mag_quick -- the error is too big
	current_target_speed = vm_vec_mag(&target_objp->phys_info.vel);
//	current_target_speed = target_objp->phys_info.fspeed;
	if ( current_target_speed < 0.1f ) {
		current_target_speed = 0.0f;
	}
	// if the speed is 0, determine if we are docked with something -- if so, get the docked velocity
	if ( (current_target_speed == 0.0f) && is_ship ) {
		current_target_speed = dock_calc_docked_fspeed(&Objects[shipp->objnum]);

		if ( current_target_speed < 0.1f ) {
			current_target_speed = 0.0f;
		}
	}

	if ( Hud_unit_multiplier > 0.0f ) {	// use a different displayed speed scale
		displayed_target_speed = current_target_speed * Hud_unit_multiplier;
	} else {
		displayed_target_speed = current_target_speed;
	}

	snprintf(outstr, sizeof(outstr), XSTR( "s: %.0f%s", 351), displayed_target_speed, (displayed_target_speed>1)?modifiers[Player_ai->current_target_speed_trend]:"");
	hud_num_make_mono(outstr);

	emp_hud_string(Targetbox_coords[gr_screen.res][TBOX_SPEED][0]+hx, Targetbox_coords[gr_screen.res][TBOX_SPEED][1]+hy, EG_TBOX_SPEED, outstr);

	//
	// output target info for debug purposes only, this will be removed later
	//

#ifndef NDEBUG
	//XSTR:OFF
	char outstr2[256];	
	if ( Show_target_debug_info && (is_ship == 1) ) {
		int sx, sy, dy;
		sx = 5;
		dy = gr_get_font_height() + 1;
		sy = 300 - 7*dy;

		gr_set_color_fast(&HUD_color_debug);

		if ( shipp->ai_index >= 0 ) {
			ai_info	*aip = &Ai_info[shipp->ai_index];

			snprintf(outstr, sizeof(outstr),"AI: %s",Ai_behavior_names[aip->mode]);

			switch (aip->mode) {
			case AIM_CHASE:
				Assert(aip->submode <= SM_BIG_PARALLEL);	//	Must be <= largest chase submode value.
//				sprintf(outstr,"AI: %s",Submode_text[aip->submode]);
				snprintf(outstr2, sizeof(outstr2), " / %s",Submode_text[aip->submode]);
				strcat(outstr,outstr2);
				break;
			case AIM_STRAFE:
				Assert(aip->submode <= AIS_STRAFE_POSITION);	//	Must be <= largest chase submode value.
//				sprintf(outstr,"AI: %s",Strafe_submode_text[aip->submode-AIS_STRAFE_ATTACK]);
				snprintf(outstr2, sizeof(outstr2), " / %s",Strafe_submode_text[aip->submode-AIS_STRAFE_ATTACK]);
				strcat(outstr,outstr2);
				break;
			case AIM_WAYPOINTS:
//				gr_printf(sx, sy, "Wpnum: %i",aip->wp_index);
				snprintf(outstr2, sizeof(outstr2)," / Wpnum: %i",aip->wp_index);
				strcat(outstr,outstr2);
				break;
			default:
				break;
			}

			gr_printf(sx, sy, outstr);
			sy += dy;

			gr_printf(sx, sy, "Max speed = %d, (%d%%)", (int) shipp->current_max_speed, (int) (100.0f * vm_vec_mag(&target_objp->phys_info.vel)/shipp->current_max_speed));
			sy += dy;
			
			// data can be found in target montior
			// gr_printf(TARGET_WINDOW_X1+TARGET_WINDOW_WIDTH+3, TARGET_WINDOW_Y1+5*h, "Shields: %d", (int) Players[Player_num].current_target->ship_max_shield_strength);
			if (aip->target_objnum != -1) {
				char	target_str[32];
				float	dot, dist;
				vec3d	v2t;

				if (aip->target_objnum == Player_obj-Objects)
					strcpy(target_str, "Player!");
				else
					snprintf(target_str, sizeof(target_str), "%s", Ships[Objects[aip->target_objnum].instance].ship_name);

//		gr_printf(TARGET_WINDOW_X1+TARGET_WINDOW_WIDTH+2, TARGET_WINDOW_Y1+4*h, "Target: %s", target_str);
				gr_printf(sx, sy, "Targ: %s", target_str);
				sy += dy;

				dist = vm_vec_dist_quick(&Objects[Player_ai->target_objnum].pos, &Objects[aip->target_objnum].pos);
				vm_vec_normalized_dir(&v2t,&Objects[aip->target_objnum].pos, &Objects[Player_ai->target_objnum].pos);

				dot = vm_vec_dot(&v2t, &Objects[Player_ai->target_objnum].orient.vec.fvec);

				// data can be found in target montior
				// gr_printf(TARGET_WINDOW_X1+TARGET_WINDOW_WIDTH+3, TARGET_WINDOW_Y1+6*h, "Targ dist: %5.1f", dist);
//		gr_printf(TARGET_WINDOW_X1+TARGET_WINDOW_WIDTH+2, TARGET_WINDOW_Y1+5*h, "Targ dot: %3.2f", dot);
				gr_printf(sx, sy, "Targ dot: %3.2f", dot);
				sy += dy;
//		gr_printf(TARGET_WINDOW_X1+TARGET_WINDOW_WIDTH+2, TARGET_WINDOW_Y1+6*h, "Targ dst: %3.2f", dist);
				gr_printf(sx, sy, "Targ dst: %3.2f", dist);
				sy += dy;

				if ( aip->targeted_subsys != NULL ) {
					snprintf(outstr, sizeof(outstr), "Subsys: %s", aip->targeted_subsys->system_info->name);
					gr_printf(sx, sy, outstr);
				}
				sy += dy;
			}

			// print out energy transfer information on the ship
			sy = 70;

			snprintf(outstr, sizeof(outstr),"MAX G/E: %.0f/%.0f",shipp->weapon_energy,shipp->current_max_speed);
			gr_printf(sx, sy, outstr);
			sy += dy;
			 
			snprintf(outstr, sizeof(outstr), "G/S/E: %.2f/%.2f/%.2f",Energy_levels[shipp->weapon_recharge_index],Energy_levels[shipp->shield_recharge_index],Energy_levels[shipp->engine_recharge_index]);
			gr_printf(sx, sy, outstr);
			sy += dy;

			//	Show information about attacker.
			{
				int	found = 0;

				if (Enemy_attacker != NULL)
					if (Enemy_attacker->type == OBJ_SHIP) {
						ship		*eshipp;
						ai_info	*eaip;
						float		dot, dist;
						vec3d	v2t;

						eshipp = &Ships[Enemy_attacker->instance];
						eaip = &Ai_info[eshipp->ai_index];

						if (eaip->target_objnum == Player_obj-Objects) {
							found = 1;
							dist = vm_vec_dist_quick(&Enemy_attacker->pos, &Player_obj->pos);
							vm_vec_normalized_dir(&v2t,&Objects[eaip->target_objnum].pos, &Enemy_attacker->pos);

							dot = vm_vec_dot(&v2t, &Enemy_attacker->orient.vec.fvec);

							gr_printf(sx, sy, "#%i: %s", Enemy_attacker-Objects, Ships[Enemy_attacker->instance].ship_name);
							sy += dy;
							gr_printf(sx, sy, "Targ dist: %5.1f", dist);
							sy += dy;
							gr_printf(sx, sy, "Targ dot: %3.2f", dot);
							sy += dy;
						}
					}

				if (Player_ai->target_objnum == Enemy_attacker - Objects)
					found = 0;

				if (!found) {
					int	i;

					Enemy_attacker = NULL;
					for (i=0; i<MAX_OBJECTS; i++)
						if (Objects[i].type == OBJ_SHIP) {
							int	enemy;

							if (i != Player_ai->target_objnum) {
								enemy = Ai_info[Ships[Objects[i].instance].ai_index].target_objnum;

								if (enemy == Player_obj-Objects) {
									Enemy_attacker = &Objects[i];
									break;
								}
							}
						}
				}
			}

			// Show target size
			// hud_target_w
			gr_printf(sx, sy, "Targ size: %dx%d", Hud_target_w, Hud_target_h );
			sy += dy;

			polymodel *pm = model_get(sip->model_num);
			gr_printf(sx, sy, "POF:%s", pm->filename );
			sy += dy;

			gr_printf(sx, sy, "Mass: %.2f\n", pm->mass);
			sy += dy;
		}
	}

	// display the weapons for the target on the HUD.  Include ammo counts.
	if ( Show_target_weapons && (is_ship == 1) ) {
		int sx, sy, dy, i;
		ship_weapon *swp;

		swp = &shipp->weapons;
		sx = 400;
		sy = 100;
		dy = gr_get_font_height();

		snprintf(outstr, sizeof(outstr), "Num primaries: %d", swp->num_primary_banks);
		gr_printf(sx,sy,outstr);
		sy += dy;
		for ( i = 0; i < swp->num_primary_banks; i++ ) {
			snprintf(outstr, sizeof(outstr), "%d. %s", i+1, Weapon_info[swp->primary_bank_weapons[i]].name);
			gr_printf(sx,sy,outstr);
			sy += dy;
		}

		sy += dy;
		snprintf(outstr, sizeof(outstr),"Num secondaries: %d", swp->num_secondary_banks);
		gr_printf(sx,sy,outstr);
		sy += dy;
		for ( i = 0; i < swp->num_secondary_banks; i++ ) {
			snprintf(outstr, sizeof(outstr), "%d. %s", i+1, Weapon_info[swp->secondary_bank_weapons[i]].name);
			gr_printf(sx,sy,outstr);
			sy += dy;
		}
	}
	//XSTR:ON

#endif
}

// called at the start of each level
void hud_targetbox_static_init()
{
	Target_static_next = 0;
	Target_static_playing = 0;
}

// determine if we should draw static on top of the target box
int hud_targetbox_static_maybe_blit(float frametime)
{
	float	sensors_str;

	// on lowest skill level, don't show static on target monitor
	if ( Game_skill_level == 0 )
		return 0;

	// if multiplayer observer, don't show static
	if((Game_mode & GM_MULTIPLAYER) && (Net_player->flags & NETINFO_FLAG_OBSERVER)){
		return 0;
	}

	sensors_str = ship_get_subsystem_strength( Player_ship, SUBSYSTEM_SENSORS );

	if ( ship_subsys_disrupted(Player_ship, SUBSYSTEM_SENSORS) ) {
		sensors_str = SENSOR_STR_TARGET_NO_EFFECTS-1;
	}

	if ( sensors_str > SENSOR_STR_TARGET_NO_EFFECTS ) {
		Target_static_playing = 0;
		Target_static_next = 0;
	} else {
		if ( Target_static_next == 0 )
			Target_static_next = 1;
	}

	if ( timestamp_elapsed(Target_static_next) ) {
		Target_static_playing ^= 1;
		Target_static_next = timestamp_rand(50, 750);
	}

	if ( Target_static_playing ) {
		// hud_set_default_color();
		hud_set_gauge_color(HUD_TARGET_MONITOR);
		hud_anim_render(&Target_static, frametime, 1);
		if ( Target_static_looping == -1 ) {
			Target_static_looping = snd_play_looping(&Snds[SND_STATIC]);
		}
	} else {
		if ( Target_static_looping != -1 ) {
			snd_stop(Target_static_looping);
			Target_static_looping = -1;
		}
	}

	return Target_static_playing;
}

// start the targetbox item flashing for duration ms
// input:	index	=>	TBOX_FLASH_ #define
//				duration	=>	optional param (default value TBOX_FLASH_DURATION), how long to flash in ms
void hud_targetbox_start_flash(int index, int duration)
{
	Targetbox_flash_timers[index][0] = timestamp(duration);
}

// stop flashing a specific targetbox item 
void hud_targetbox_end_flash(int index)
{
	Targetbox_flash_timers[index][0] = 1;
}

// determine if a given flashing index is bright or not
int hud_targetbox_is_bright(int index)
{
	return (Targetbox_flash_flags & (1<<index));
}

// determine if the flashing has expired
int hud_targetbox_flash_expired(int index)
{
	if ( timestamp_elapsed(Targetbox_flash_timers[index][0]) ) {
		return 1;
	}
																		
	return 0;
} 


void hudtargetbox_page_in()
{
	bm_page_in_aabitmap( Target_view_gauge.first_frame, Target_view_gauge.num_frames);

	bm_page_in_aabitmap( Target_view_integrity_gauge.first_frame, Target_view_integrity_gauge.num_frames );

	bm_page_in_aabitmap( Target_view_extra.first_frame, Target_view_extra.num_frames );
}
