/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/Fireball/FireBalls.cpp $
 * $Revision: 2.33.2.4 $
 * $Date: 2007-09-02 02:07:39 $
 * $Author: Goober5000 $
 *
 * Code to move, render and otherwise deal with fireballs.
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.33.2.3  2007/07/28 21:31:04  Goober5000
 * this should really be capitalized
 *
 * Revision 2.33.2.2  2007/02/11 09:53:33  taylor
 * support for tbl specified fireball explosion color ("$Light color:")
 *
 * Revision 2.33.2.1  2006/09/11 01:12:50  taylor
 * fixes for stuff_string() bounds checking
 * stict compiler build fixes
 *
 * Revision 2.33  2006/02/25 21:46:59  Goober5000
 * spelling
 *
 * Revision 2.32  2006/02/13 00:20:45  Goober5000
 * more tweaks, plus clarification of checks for the existence of files
 * --Goober5000
 *
 * Revision 2.31  2006/01/20 07:10:33  Goober5000
 * reordered #include files to quash Microsoft warnings
 * --Goober5000
 *
 * Revision 2.30  2005/12/29 08:08:33  wmcoolmon
 * Codebase commit, most notably including objecttypes.tbl
 *
 * Revision 2.29  2005/12/28 22:17:01  taylor
 * deal with cf_find_file_location() changes
 * add a central parse_modular_table() function which anything can use
 * fix up weapon_expl so that it can properly handle modular tables and LOD count changes
 * add support for for a fireball TBM (handled a little different than a normal TBM is since it only changes rather than adds)
 *
 * Revision 2.28  2005/12/01 07:45:21  Goober5000
 * bypass annoying warnings when loading optional models
 * --Goober5000
 *
 * Revision 2.27  2005/10/20 17:50:00  taylor
 * fix player warpout
 * basic code cleanup (that previous braces change did nothing for readability)
 * spell "plyr" correctly
 * tweak warp shrink time to better match WMC's other changes and avoid the skipping during shrink
 *
 * Revision 2.26  2005/04/05 05:53:15  taylor
 * s/vector/vec3d/g, better support for different compilers (Jens Granseuer)
 *
 * Revision 2.25  2005/03/19 18:02:33  bobboau
 * added new graphic functions for state blocks
 * also added a class formanageing a new effect
 *
 * Revision 2.24  2005/03/14 03:24:25  taylor
 * speedup grow/shrink time for the warp to avoid the big size skips
 *
 * Revision 2.23  2005/03/10 08:00:02  taylor
 * change min/max to MIN/MAX to fix GCC problems
 * add lab stuff to Makefile
 * build unbreakage for everything that's not MSVC++ 6
 * lots of warning fixes
 * fix OpenGL rendering problem with ship insignias
 * no Warnings() in non-debug mode for Linux (like Windows)
 * some campaign savefile fixage to stop reverting everyones data
 *
 * Revision 2.22  2005/03/01 06:55:40  bobboau
 * oh, hey look I've commited something :D
 * animation system, weapon models detail box alt-tab bug, probly other stuff
 *
 * Revision 2.21  2005/02/23 05:05:39  taylor
 * compiler warning fixes (for MSVC++ 6)
 * have the warp effect only load as many LODs as will get used
 * head off strange bug in release when corrupt soundtrack number gets used
 *    (will still Assert in debug)
 * don't ever try and save a campaign savefile in multi or standalone modes
 * first try at 32bit->16bit color conversion for TGA code (for TGA only ship textures)
 *
 * Revision 2.20  2005/01/30 09:27:39  Goober5000
 * nitpicked some boolean tests, and fixed two small bugs
 * --Goober5000
 *
 * Revision 2.19  2004/11/01 20:57:03  taylor
 * make use of Knossos_warp_ani_used flag - thanks Goober5000
 *
 * Revision 2.18  2004/10/31 02:26:15  Goober5000
 * whoops
 * --Goober5000
 *
 * Revision 2.17  2004/10/31 02:04:33  Goober5000
 * added Knossos_warp_ani_used flag for taylor
 * --Goober5000
 *
 * Revision 2.16  2004/09/17 07:12:22  Goober5000
 * changed around the logic for the 3D warp effect
 * --Goober5000
 *
 * Revision 2.15  2004/08/23 04:32:39  Goober5000
 * warp effect is back to FS2 default
 * --Goober5000
 *
 * Revision 2.14  2004/07/26 20:47:28  Kazan
 * remove MCD complete
 *
 * Revision 2.13  2004/07/12 16:32:45  Kazan
 * MCD - define _MCD_CHECK to use memory tracking
 *
 * Revision 2.12  2004/05/12 22:49:14  phreak
 * renamed the warp model variable from 'wm' to 'Warp_model'
 *
 * Revision 2.11  2004/03/17 04:07:29  bobboau
 * new fighter beam code
 * fixed old after burner trails
 * had to bump a few limits, working on some dynamic solutions
 * a few fixed to background POF rendering
 * fixing asorted bugs
 *
 * Revision 2.10  2004/03/05 09:02:00  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.9  2004/02/14 00:18:30  randomtiger
 * Please note that from now on OGL will only run with a registry set by Launcher v4. See forum for details.
 * OK, these changes effect a lot of file, I suggest everyone updates ASAP:
 * Removal of many files from project.
 * Removal of meanless Gr_bitmap_poly variable.
 * Removal of glide, directdraw, software modules all links to them, and all code specific to those paths.
 * Removal of redundant Fred paths that arent needed for Fred OGL.
 * Have seriously tidied the graphics initialisation code and added generic non standard mode functionality.
 * Fixed many D3D non standard mode bugs and brought OGL up to the same level.
 * Removed texture section support for D3D8, voodoo 2 and 3 cards will no longer run under fs2_open in D3D, same goes for any card with a maximum texture size less than 1024.
 *
 * Revision 2.8  2003/11/16 04:09:19  Goober5000
 * language
 *
 * Revision 2.7  2003/11/11 03:56:10  bobboau
 * lots of bug fixing, much of it in nebula and bitmap drawing
 *
 * Revision 2.6  2003/10/23 18:03:23  randomtiger
 * Bobs changes (take 2)
 *
 * Revision 2.5  2003/03/19 06:23:27  Goober5000
 * added warp-effect sexp
 * --Goober5000
 *
 * Revision 2.4  2003/03/18 01:44:30  Goober5000
 * fixed some misspellings
 * --Goober5000
 *
 * Revision 2.3  2002/12/07 01:37:41  bobboau
 * initial decals code, if you are worried a bug is being caused by the decals code it's only references are in,
 * collideshipweapon.cpp line 262, beam.cpp line 2771, and modelinterp.cpp line 2949.
 * it needs a better renderer, but is in prety good shape for now,
 * I also (think) I squashed a bug in the warpmodel code
 *
 * Revision 2.2  2002/11/14 04:18:16  bobboau
 * added warp model and type 1 glow points
 * and well as made the new glow file type,
 * some general improvement to fighter beams,
 *
 * Revision 2.1  2002/08/01 01:41:04  penguin
 * The big include file move
 *
 * Revision 2.0  2002/06/03 04:02:22  penguin
 * Warpcore CVS sync
 *
 * Revision 1.2  2002/05/03 22:07:08  mharris
 * got some stuff to compile
 *
 * Revision 1.1  2002/05/02 18:03:07  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 19    9/14/99 1:27a Andsager
 * Better LOD for fireballs when behind.  Move point ahead to get vertex
 * ahead and then find size.
 * 
 * 18    9/12/99 11:42p Dave
 * 
 * 17    9/13/99 10:09a Andsager
 * Add debug console commands to lower model render detail and fireball
 * LOD for big ship explosiosns.
 * 
 * 16    9/09/99 8:53p Dave
 * Fixed multiplayer degenerate orientation case problem. Make sure warp
 * effect never goes lower than LOD 1. 
 * 
 * 15    9/07/99 12:20a Andsager
 * LOD less agressive at lower hardware detail level
 * 
 * 14    9/06/99 6:47p Jamesa
 * Fixed build error.
 * 
 * 13    9/06/99 3:23p Andsager
 * Make fireball and weapon expl ani LOD choice look at resolution of the
 * bitmap
 * 
 * 12    9/01/99 10:09a Dave
 * Pirate bob.
 * 
 * 11    8/31/99 10:13p Andsager
 * Add Knossos warp effect fireball
 * 
 * 10    8/30/99 9:59a Dave
 * Removed explosion LOD stuff.
 * 
 * 9     8/28/99 4:54p Dave
 * Fixed directives display for multiplayer clients for wings with
 * multiple waves. Fixed hud threat indicator rendering color.
 * 
 * 8     8/27/99 9:07p Dave
 * LOD explosions. Improved beam weapon accuracy.
 * 
 * 7     7/09/99 5:54p Dave
 * Seperated cruiser types into individual types. Added tons of new
 * briefing icons. Campaign screen.
 * 
 * 6     4/28/99 11:13p Dave
 * Temporary checkin of artillery code.
 * 
 * 5     4/23/99 12:01p Johnson
 * Added SIF_HUGE_SHIP
 * 
 * 4     11/05/98 5:55p Dave
 * Big pass at reducing #includes
 * 
 * 3     10/23/98 3:51p Dave
 * Full support for tstrings.tbl and foreign languages. All that remains
 * is to make it active in Fred.
 * 
 * 2     10/07/98 10:52a Dave
 * Initial checkin.
 * 
 * 1     10/07/98 10:48a Dave
 * 
 * 75    5/18/98 2:49p Lawrance
 * Allow up to double instance of sound for warp holes
 * 
 * 74    5/15/98 3:54p John
 * Added code so that only "perishable" fireballs get removed.
 * 
 * 73    4/15/98 9:42a Adam
 * added 2 more explosion types (1, actually, but placeholder for 2)
 * 
 * 72    4/12/98 9:56a John
 * Made lighting detail flags work.   Made explosions cast light on
 * highest.
 * 
 * 71    4/09/98 7:58p John
 * Cleaned up tmapper code a bit.   Put NDEBUG around some ndebug stuff.
 * Took out XPARENT flag settings in all the alpha-blended texture stuff.
 * 
 * 70    3/31/98 5:11p John
 * Removed demo/save/restore.  Made NDEBUG defined compile.  Removed a
 * bunch of debug stuff out of player file.  Made model code be able to
 * unload models and malloc out only however many models are needed.
 * 
 * 69    3/30/98 4:02p John
 * Made machines with < 32 MB of RAM use every other frame of certain
 * bitmaps.   Put in code to keep track of how much RAM we've malloc'd.
 * 
 * 68    3/29/98 12:39p John
 * Made warp in glow page in
 * 
 * 67    3/27/98 2:17p Lawrance
 * Don't play warphole closing for non-capital sized warp effects
 * 
 * 66    3/26/98 5:44p Lawrance
 * take out debug comments for warphole sounds playing
 * 
 * 65    3/26/98 5:21p John
 * Added new code to preload all bitmaps at the start of a level.
 * Commented it out, though.
 * 
 * 64    3/25/98 5:30p Lawrance
 * Fix bug that was not playing wormhole open sound effect at correct
 * volume
 * 
 * 63    3/25/98 10:56a Lawrance
 * Make a sound for warphole close as well as warphole open, move to
 * fireball lib
 * 
 * 62    3/18/98 12:36p John
 * Made hardware have nicer looking warp effect
 * 
 * 61    3/18/98 12:01p John
 * Made warp grid grow faster
 * 
 * 60    2/26/98 10:07p Hoffoss
 * Rewrote state saving and restoring to fix bugs and simplify the code.
 * 
 * 59    2/22/98 12:19p John
 * Externalized some strings
 * 
 * 58    2/19/98 4:32p Lawrance
 * Add asteroid explosion
 * 
 * 57    2/12/98 11:53p Lawrance
 * comment out use of current_frame since not used anywhere
 * 
 * 56    2/05/98 9:21p John
 * Some new Direct3D code.   Added code to monitor a ton of stuff in the
 * game.
 * 
 * 55    1/23/98 5:06p John
 * Took L out of vertex structure used B (blue) instead.   Took all small
 * fireballs out of fireball types and used particles instead.  Fixed some
 * debris explosion things.  Restructured fireball code.   Restructured
 * some lighting code.   Made dynamic lighting on by default. Made groups
 * of lasers only cast one light.  Made fireballs not cast light.
 * 
 * 54    1/19/98 9:37p Allender
 * Great Compiler Warning Purge of Jan, 1998.  Used pragma's in a couple
 * of places since I was unsure of what to do with code.
 * 
 * 53    1/15/98 4:58p John
 * Made warp effect use a looping ani.  Made the scaling up & down be in
 * software.
 * 
 * 52    1/15/98 2:32p John
 * Added code to optionally take a velocity for a fireball.
 * 
 * 51    1/11/98 2:15p John
 * Changed a lot of stuff that had to do with bitmap loading.   Made cfile
 * not do callbacks, I put that in global code.   Made only bitmaps that
 * need to load for a level load.
 * 
 * 50    1/02/98 5:04p John
 * Several explosion related changes.  Made fireballs not be used as
 * ani's.  Made ship spark system expell particles.  Took away impact
 * explosion for weapon hitting ship... this needs to get added to weapon
 * info and makes shield hit more obvious.  Only make sparks when hit
 * hull, not shields.
 * 
 * 49    12/31/97 4:48p John
 * added some debug info
 * 
 * 48    12/30/97 6:44p John
 * Made g3_Draw_bitmap functions account for aspect of bitmap.
 * 
 * 47    12/08/97 11:15a John
 * added parameter to warpout for life.
 * 
 * 46    12/02/97 3:59p John
 * Added first rev of thruster glow, along with variable levels of
 * translucency, which retquired some restructing of palman.
 * 
 * 45    11/30/97 2:03p John
 * made warp effect not use an ani for height map.
 * 
 * 44    11/16/97 12:20p John
 * added code to make ships instantly warp in/out if fireball effect
 * fails.   But, made it so the fireball effect will kill off old sparks
 * so it can go, so this should never happen.
 * 
 * 43    11/16/97 10:48a John
 * added code to remove the oldest fireball if there weren't enough slots
 * for a new one.
 * 
 * 42    11/01/97 1:49p John
 * added code to page fireballs in during level load.  Made player warpout
 * use Adam's new camera movement pattern.  Make ships docked to warping
 * out ships render correctly.
 * 
 * 41    10/29/97 5:05p John
 * Changed dynamic lighting to only rotate and calculate lighting for
 * point lights that are close to an object.  Changed lower framerate cap
 * from 4 to .5.
 * 
 * 40    10/24/97 6:24p John
 * added code to return the life left of a fireball
 * 
 * 39    9/14/97 4:49p Lawrance
 * add some demo debugging code
 * 
 * 38    9/12/97 4:02p John
 * put in ship warp out effect.
 * put in dynamic lighting for warp in/out
 * 
 * 37    9/09/97 4:49p John
 * Almost done ship warp in code
 * 
 * 36    9/08/97 8:39a John
 * added in code structure for grid
 * 
 * 35    9/03/97 5:04p Lawrance
 * add error checking when restoring structs
 * 
 * 34    9/03/97 4:32p John
 * changed bmpman to only accept ani and pcx's.  made passing .pcx or .ani
 * to bm_load functions not needed.   Made bmpman keep track of palettes
 * for bitmaps not mapped into game palettes.
 * 
 * 33    8/29/97 2:26p John
 * first rev of ship warp in effect.  Nothing more than a fireball inside
 * of freespace, but textest.cpp contains the correct effect code that
 * needs to be transferred into the game next.
 * 
 * 32    8/21/97 5:11p Lawrance
 * frame numbering for ANI's now is from 0 -> total_frames-1.
 * 
 * 31    8/13/97 9:50p Allender
 * split *_move into *_process_pre and *_process_post functions.
 * process_pre functions called before object is moved.  _process_post
 * functions called after object is moved.  Reordered code in ship_post
 * and weapon_post for multiplayer
 * 
 * 30    8/13/97 12:06p Lawrance
 * supporting multiple types of fireball explosions
 * 
 * 29    8/05/97 10:18a Lawrance
 * my_rand() being used temporarily instead of rand()
 * 
 * 28    7/31/97 5:55p John
 * made so you pass flags to obj_create.
 * Added new collision code that ignores any pairs that will never
 * collide.
 * 
 * 27    7/24/97 9:54a Lawrance
 * don't free fireball system if not inited
 * 
 * 26    7/23/97 11:35a Lawrance
 * ensure fireball animations are always freed properly
 * 
 * 25    7/21/97 11:41a Lawrance
 * clean up fireballs at the end of each level
 * 
 * 24    7/20/97 6:58p Lawrance
 * call anim_get_frame() to take advantage of key-framed animation
 * 
 * 23    7/11/97 3:57p John
 * made fireballs translucent.  alpha=1.3x intesity. only for d3d.
 * 
 * 22    7/11/97 12:08p John
 * made fireballs randomly be rotated.
 * 
 * 21    7/11/97 11:54a John
 * added rotated 3d bitmaps.
 * 
 * 20    7/11/97 11:19a Lawrance
 * fix mem leaks, move save code from State.cpp here
 * 
 * 19    7/10/97 1:51p Lawrance
 * sorting anim fireballs
 * 
 * 18    6/24/97 6:21p John
 * added detail flags.
 * sped up motion debris system a bit.
 * 
 * 17    5/14/97 4:08p Lawrance
 * removing my_index from game arrays
 * 
 * 16    5/05/97 10:40p Mike
 * Make missile exhaust trails look a tad nicer.
 * 
 * 15    3/11/97 10:47p Mike
 * Add a slew of secondary weapons.
 * Support exhaust blobs.
 * Add weapons that spawn weapons.
 * Add remotely detonatable weapons.
 * Add heat-seeking missiles.
 * 
 * 14    2/17/97 5:18p John
 * Added a bunch of RCS headers to a bunch of old files that don't have
 * them.
 *
 * $NoKeywords: $
 */

#include "fireball/fireballs.h"
#include "graphics/tmapper.h"
#include "render/3d.h"
#include "model/model.h"
#include "object/object.h"
#include "ship/ship.h"
#include "gamesnd/gamesnd.h"
#include "localization/localize.h"
#include "cmdline/cmdline.h"
#include "parse/parselo.h"
#include "globalincs/pstypes.h"

#include <stdlib.h>
#include <vector>


// make use of the LOD checker for tbl/tbm parsing (from weapons.cpp)
extern std::vector<lod_checker> LOD_checker;

static std::vector<color> LOD_color;

int Warp_model;
int Knossos_warp_ani_used;

#define WARPHOLE_GROW_TIME		(2.35f)	// time for warphole to reach max size (also time to shrink to nothing once it begins to shrink)

#define MAX_FIREBALL_LOD						4

#define MAX_FIREBALLS	200

#define MAX_WARP_LOD	0

fireball Fireballs[MAX_FIREBALLS];

fireball_info Fireball_info[MAX_FIREBALL_TYPES];

int fireball_used[MAX_FIREBALL_TYPES];

int Num_fireballs = 0;

int Num_fireball_types = 0;

int fireballs_inited = 0;

int Warp_glow_bitmap = -1;
int Warp_ball_bitmap = -1;

#define FB_INDEX(fb)	(fb-Fireballs)

// play warp in sound for warp effect
void fireball_play_warphole_open_sound(int ship_class, fireball *fb)
{
	int		sound_index;
	float		range_multiplier = 1.0f;
	object	*fireball_objp;	
		
	Assert((fb != NULL) && (fb->objnum >= 0));
	if((fb == NULL) || (fb->objnum < 0)){
		return;
	}
	fireball_objp = &Objects[fb->objnum];

	sound_index = SND_WARP_IN;

	if(fb->warp_open_sound_index > -1) {
		sound_index = fb->warp_open_sound_index;
	} else if((ship_class >= 0) && (ship_class < Num_ship_classes)){
		if ( Ship_info[ship_class].flags & SIF_HUGE_SHIP ) {
			sound_index = SND_CAPITAL_WARP_IN;
			fb->flags |= FBF_WARP_CAPITAL_SIZE;
		} else if ( Ship_info[ship_class].flags & SIF_BIG_SHIP ) {
			range_multiplier = 6.0f;
			fb->flags |= FBF_WARP_CRUISER_SIZE;
		}
	}

	snd_play_3d(&Snds[sound_index], &fireball_objp->pos, &Eye_position, fireball_objp->radius, NULL, 0, 1.0f, SND_PRIORITY_DOUBLE_INSTANCE, NULL, range_multiplier); // play warp sound effect
}

// play warp out sound for warp effect
void fireball_play_warphole_close_sound(fireball *fb)
{
	int	sound_index;	

	object *fireball_objp;

	fireball_objp = &Objects[fb->objnum];

	sound_index = SND_WARP_OUT;

	if ( fb->warp_close_sound_index > -1 ) {
		sound_index = fb->warp_close_sound_index;
	} else if ( fb->flags & FBF_WARP_CAPITAL_SIZE ) {
		sound_index = SND_CAPITAL_WARP_OUT;
	} else {
		// AL 27-3-98: Decided that warphole closing is only required for capital ship sized warp effects.
		return;
	}

	snd_play_3d(&Snds[sound_index], &fireball_objp->pos, &Eye_position, fireball_objp->radius); // play warp sound effect
}

// set default colors for each explosion type (original values from object.cpp)
static void fireball_set_default_color(int idx)
{
	Assert( (idx >= 0) && (idx < MAX_FIREBALL_TYPES) );

	switch (idx)
	{
		case FIREBALL_EXPLOSION_LARGE1:
		case FIREBALL_EXPLOSION_LARGE2:
		case FIREBALL_EXPLOSION_MEDIUM:
		case FIREBALL_ASTEROID:
			Fireball_info[idx].exp_color[0] = 1.0f;
			Fireball_info[idx].exp_color[1] = 0.5f;
			Fireball_info[idx].exp_color[2] = 0.125f;
			break;

		case FIREBALL_WARP:
			Fireball_info[idx].exp_color[0] = 0.75f;
			Fireball_info[idx].exp_color[1] = 0.75f;
			Fireball_info[idx].exp_color[2] = 1.0f;
			break;


		case FIREBALL_KNOSSOS:
			Fireball_info[idx].exp_color[0] = 0.75f;
			Fireball_info[idx].exp_color[1] = 1.0f;
			Fireball_info[idx].exp_color[2] = 0.75f;
			break;

		default:
			Fireball_info[idx].exp_color[0] = 1.0f;
			Fireball_info[idx].exp_color[1] = 1.0f;
			Fireball_info[idx].exp_color[2] = 1.0f;
			break;
	}
}

// NOTE: we can't be too trusting here so a tbm will only modify the LOD count, not add an entry
void parse_fireball_tbl(char *filename)
{
	int rval;
	lod_checker lod_check;
	color fb_color;

	// open localization
	lcl_ext_open();

	if ((rval = setjmp(parse_abort)) != 0) {
		mprintf(("TABLES: Unable to parse '%s'!  Error code = %i.\n", filename, rval));
		lcl_ext_close();
		return;
	}

	read_file_text(filename, CF_TYPE_TABLES);
	reset_parse();

	required_string("#Start");

	while (required_string_either("#End", "$Name:")) {
		memset( &lod_check, 0, sizeof(lod_checker) );

		// base filename
		required_string("$Name:");
		stuff_string(lod_check.filename, F_NAME, MAX_FILENAME_LEN);

		lod_check.override = -1;

		// these entries should only be in TBMs, and it has to include at least one
		if ( Parsing_modular_table ) {
			if (optional_string("+Explosion_Medium")) {
				lod_check.override = FIREBALL_EXPLOSION_MEDIUM;
			} else if (optional_string("+Warp_Effect")) {
				lod_check.override = FIREBALL_WARP;
			} else if (optional_string("+Knossos_Effect")) {
				lod_check.override = FIREBALL_KNOSSOS;
			} else if (optional_string("+Asteroid")) {
				lod_check.override = FIREBALL_ASTEROID;
			} else if (optional_string("+Explosion_Large1")) {
				lod_check.override = FIREBALL_EXPLOSION_LARGE1;
			} else if (optional_string("+Explosion_Large2")){
				lod_check.override = FIREBALL_EXPLOSION_LARGE2;
			} else {
				required_string("+Custom_Fireball");
				stuff_int(&lod_check.override);
			}
		}

		lod_check.num_lods = 1;

		// Do we have an LOD num
		if (optional_string("$LOD:")) {
			stuff_int(&lod_check.num_lods);
		}

		if (lod_check.num_lods > MAX_FIREBALL_LOD) {
			lod_check.num_lods = MAX_FIREBALL_LOD;
		}

		// check for particular lighting color
		if ( optional_string("$Light color:") ) {
			int r, g, b;

			stuff_int(&r);
			stuff_int(&g);
			stuff_int(&b);

			CLAMP(r, 0, 255);
			CLAMP(g, 0, 255);
			CLAMP(b, 0, 255);

			gr_init_color(&fb_color, r, g, b);
		} else {
			// to keep things simple, we just use 0 alpha to indicate that a default value should be used
			memset( &fb_color, 0, sizeof(color) );
		}

		// we may use one filename for multiple entries so we'll have to handle dupes post parse
		LOD_checker.push_back(lod_check);
		LOD_color.push_back(fb_color);
	}

	required_string("#End");

	// close localization
	lcl_ext_close();
}

void fireball_parse_tbl()
{
	int i, j;

	memset( &Fireball_info, 0, sizeof(fireball_info) * MAX_FIREBALL_TYPES );


	parse_fireball_tbl("fireball.tbl");

	// look for any modular tables
	parse_modular_table(NOX("*-fbl.tbm"), parse_fireball_tbl);

	// we've got our list so pass it off for final checking and loading.
	// we assume that entries in fireball.tbl are in the correct order
	for (i = 0; i < (int)LOD_checker.size(); i++) {
		if ( (i < MAX_FIREBALL_TYPES) && (LOD_checker[i].override < 0) ) {
			strcpy( Fireball_info[i].lod[0].filename, LOD_checker[i].filename );
			Fireball_info[i].lod_count = LOD_checker[i].num_lods;
			Num_fireball_types++;

			if (LOD_color[i].alpha == 255) {
				Fireball_info[i].exp_color[0] = (LOD_color[i].red / 255.0f);
				Fireball_info[i].exp_color[1] = (LOD_color[i].green / 255.0f);
				Fireball_info[i].exp_color[2] = (LOD_color[i].blue / 255.0f);
			} else {
				fireball_set_default_color(i);
			}
		}
	}

	// having to do this twice is less than optimal, but less error prone too.
	// this handles (and should only have to handle) TBM related entries
	for (i = 0; i < (int)LOD_checker.size(); i++) {
		// try entry replacement
		if ( (LOD_checker[i].override >= 0) && (LOD_checker[i].override < Num_fireball_types) ) {
			strcpy( Fireball_info[LOD_checker[i].override].lod[0].filename, LOD_checker[i].filename );
			Fireball_info[LOD_checker[i].override].lod_count = LOD_checker[i].num_lods;

			if (LOD_color[i].alpha == 255) {
				Fireball_info[LOD_checker[i].override].exp_color[0] = (LOD_color[i].red / 255.0f);
				Fireball_info[LOD_checker[i].override].exp_color[1] = (LOD_color[i].green / 255.0f);
				Fireball_info[LOD_checker[i].override].exp_color[2] = (LOD_color[i].blue / 255.0f);
			} else {
				fireball_set_default_color(LOD_checker[i].override);
			}
		}
	}

	// fill in extra LOD filenames
	for (i = 0; i < Num_fireball_types; i++) {
		for (j = 1; j < Fireball_info[i].lod_count; j++) {
			snprintf( Fireball_info[i].lod[j].filename, sizeof(Fireball_info[i].lod[j].filename), "%s_%d", Fireball_info[i].lod[0].filename, j);
		}
	}

	// done
	LOD_checker.clear();
}


void fireball_load_data()
{
	int				i, idx;
	fireball_info	*fd;

	for ( i = 0; i < Num_fireball_types; i++ ) {
		fd = &Fireball_info[i];

		for(idx=0; idx<fd->lod_count; idx++){
			// we won't use a warp effect lod greater than MAX_WARP_LOD so don't load it either
			if ( (i == FIREBALL_WARP) && (idx > MAX_WARP_LOD) )
				continue;

			fd->lod[idx].bitmap_id	= bm_load_animation( fd->lod[idx].filename, &fd->lod[idx].num_frames, &fd->lod[idx].fps, 1 );
			if ( fd->lod[idx].bitmap_id < 0 ) {
				Error(LOCATION, "Could not load %s anim file\n", fd->lod[idx].filename);
			}
		}
	} 

	if ( Warp_glow_bitmap == -1 )	{
		Warp_glow_bitmap = bm_load( NOX("warpglow01") );
	}
	if ( Warp_ball_bitmap == -1 )	{
		Warp_ball_bitmap = bm_load( NOX("warpball01") );
	}
	
//	polymodel Warp_pm;

}

void fireball_preload()
{
	// Do nothing.  Called before level init, this used to page in warp effect.
	// Not needed with new BmpMan system.
}

// This will get called at the start of each level.
void fireball_init()
{
	int i;

	if ( !fireballs_inited ) {
		fireballs_inited = 1;

		// Do all the processing that happens only once
		fireball_parse_tbl();
		fireball_load_data();
	}
	
	// Reset everything between levels
	Num_fireballs = 0;
	for (i=0; i<MAX_FIREBALLS; i++ )	{
		Fireballs[i].objnum	= -1;
	}

	// Goober5000 - reset Knossos warp flag
	Knossos_warp_ani_used = 0;

	mprintf(("Loading warp model\n"));
	Warp_model = -1;

	// Goober5000 - check for existence of file before trying to load it
	if (cf_exists_full("warp.pof", CF_TYPE_MODELS))
	{
		Warp_model = model_load("warp.pof", 0, NULL, 0);
	}

	mprintf((" %d\n", Warp_model));
}

MONITOR( NumFireballsRend )

void fireball_render(object * obj)
{
//	if(!Cmdline_nohtl)gr_set_lighting(false,false);

	int		num;
	vertex	p;
	fireball	*fb;
	fireball_info *fd;

	MONITOR_INC( NumFireballsRend, 1 );	
	
	num = obj->instance;
	fb = &Fireballs[num];
	fd = &Fireball_info[fb->fireball_info_index];

	if ( Fireballs[num].current_bitmap < 0 )
		return;

//	gr_set_color( 0, 100, 0 );
//	g3_draw_sphere_ez( &obj->pos, obj->radius );
//	return;

	// turn off fogging
	if(The_mission.flags & MISSION_FLAG_FULLNEB){
		gr_fog_set(GR_FOGMODE_NONE, 0, 0, 0);
	}

	if(Cmdline_nohtl) {
		g3_rotate_vertex(&p, &obj->pos );
	}else{
		g3_transfer_vertex(&p, &obj->pos);
	}

	switch( fb->fireball_render_type )	{

		case FIREBALL_MEDIUM_EXPLOSION:
			gr_set_bitmap(Fireballs[num].current_bitmap, GR_ALPHABLEND_FILTER, GR_BITBLT_MODE_NORMAL, 1.3f );
			g3_draw_bitmap(&p, fb->orient, obj->radius, TMAP_FLAG_TEXTURED | TMAP_HTL_3D_UNLIT );
			break;

		case FIREBALL_LARGE_EXPLOSION:
			// Make the big explosions rotate with the viewer.
			gr_set_bitmap(Fireballs[num].current_bitmap, GR_ALPHABLEND_FILTER, GR_BITBLT_MODE_NORMAL, 1.3f );
			g3_draw_rotated_bitmap(&p, (i2fl(fb->orient)*PI)/180.0f, obj->radius, TMAP_FLAG_TEXTURED | TMAP_HTL_3D_UNLIT);
			break;

		case FIREBALL_WARP_EFFECT: {
			
				float percent_life = fb->time_elapsed / fb->total_time;

				float rad;

				// Code to make effect grow/shrink. 
				float t = fb->time_elapsed;
			
				if ( t < WARPHOLE_GROW_TIME )	{
					rad = (float)pow(t/WARPHOLE_GROW_TIME,0.4f)*obj->radius;
					//rad = t*obj->radius/WARPHOLE_GROW_TIME;
					//mprintf(( "T=%.2f, Rad = %.2f\n", t, rad ));
				} else if ( t < fb->total_time - WARPHOLE_GROW_TIME )	{
					rad = obj->radius;
				} else {
					rad = (float)pow((fb->total_time - t)/WARPHOLE_GROW_TIME,0.4f)*obj->radius;
					//rad = (fb->total_time - t )*obj->radius/WARPHOLE_GROW_TIME;
				}
				//rad = obj->radius;
//				obj->alt_rad = rad;


				warpin_render(obj, &obj->orient, &obj->pos, Fireballs[num].current_bitmap, rad, percent_life, obj->radius, (Fireballs[num].flags & FBF_WARP_3D) );
			}
			break;

			
		default:
			Int3();
	}
}

// -----------------------------------------------------------------
//	fireball_delete()
//
//	Delete a fireball.  Called by object_delete() code... do not call
// directly.
//
void fireball_delete( object * obj )
{
	int	num;
	fireball	*fb;

	num = obj->instance;
	fb = &Fireballs[num];

	Assert( fb->objnum == OBJ_INDEX(obj));

	Fireballs[num].objnum = -1;
	Num_fireballs--;
	Assert( Num_fireballs >= 0 );
}

// -----------------------------------------------------------------
//	fireball_delete_all()
//
//	Delete all active fireballs, by calling obj_delete directly.
//
void fireball_delete_all()
{
	fireball	*fb;
	int		i;

	for ( i = 0; i < MAX_FIREBALLS; i++ ) {
		fb = &Fireballs[i];
		if ( fb->objnum != -1 ) {
			obj_delete(fb->objnum);
		}
	}
}

void fireball_set_framenum(int num)
{
	int				framenum;
	fireball			*fb;
	fireball_info	*fd;
	fireball_lod	*fl;

	fb = &Fireballs[num];
	fd = &Fireball_info[Fireballs[num].fireball_info_index];

	// valid lod?
	fl = NULL;
	if((fb->lod >= 0) && (fb->lod < fd->lod_count)){
		fl = &Fireball_info[Fireballs[num].fireball_info_index].lod[(int)fb->lod];
	}
	if(fl == NULL){
		// argh
		return;
	}

	if ( fb->fireball_render_type == FIREBALL_WARP_EFFECT )	{
		float total_time = i2fl(fl->num_frames) / fl->fps;	// in seconds

		framenum = fl2i(fb->time_elapsed * fl->num_frames / total_time + 0.5);

		if ( framenum < 0 ) framenum = 0;

		framenum = framenum % fl->num_frames;

		if ( fb->orient )	{
			// warp out effect plays backwards
			framenum = fl->num_frames-framenum-1;
			fb->current_bitmap = fl->bitmap_id + framenum;
		} else {
			fb->current_bitmap = fl->bitmap_id + framenum;
		}
	} else {

		framenum = fl2i(fb->time_elapsed / fb->total_time * fl->num_frames + 0.5);

		// ensure we don't go past the number of frames of animation
		if ( framenum > (fl->num_frames-1) ) {
			framenum = (fl->num_frames-1);
			Objects[fb->objnum].flags |= OF_SHOULD_BE_DEAD;
		}

		if ( framenum < 0 ) framenum = 0;
		fb->current_bitmap = fl->bitmap_id + framenum;
	}
}

int fireball_is_perishable(object * obj)
{
	//	return 1;
	int			num, objnum;
	fireball		*fb;

	num = obj->instance;
	objnum = OBJ_INDEX(obj);
	Assert( Fireballs[num].objnum == objnum );

	fb = &Fireballs[num];

	if ( fb->fireball_render_type == FIREBALL_MEDIUM_EXPLOSION )	
		return 1;

	if ( !(fb->fireball_render_type == FIREBALL_WARP_EFFECT) )	{
		if ( !(obj->flags & OF_WAS_RENDERED))	{
			return 1;
		}
	}

	return 0;
}


// -----------------------------------------------------------------
//	fireball_free_one()
//
//	There are too many fireballs, so delete the oldest small one
// to free up a slot.  Returns the fireball slot freed.
//
int fireball_free_one()
{
	fireball	*fb;
	int		i;

	int		oldest_objnum = -1, oldest_slotnum = -1;
	float		lifeleft, oldest_lifeleft = 0.0f;

	for ( i = 0; i < MAX_FIREBALLS; i++ ) {
		fb = &Fireballs[i];

		// only remove the ones that aren't warp effects
		if ( (fb->objnum >= 0) && fireball_is_perishable(&Objects[fb->objnum]) )	{

			lifeleft = fb->total_time - fb->time_elapsed;
			if ( (oldest_objnum < 0) || (lifeleft < oldest_lifeleft) )	{
				oldest_slotnum = i;
				oldest_lifeleft = lifeleft;
				oldest_objnum = fb->objnum;
			}
			break;
		}
	}

	if ( oldest_objnum > -1 )	{
		obj_delete(oldest_objnum);
	}
	return oldest_slotnum;
}

// broke fireball_move into fireball_process_pre and fireball_process_post as was done
// with all *_move functions on 8/13 by Mike K. and Mark A.
void fireball_process_pre( object *objp, float frame_time)
{
}

int fireball_is_warp(object * obj)
{
	int			num, objnum;
	fireball		*fb;

	num = obj->instance;
	objnum = OBJ_INDEX(obj);
	Assert( Fireballs[num].objnum == objnum );

	fb = &Fireballs[num];

	if ( fb->fireball_render_type == FIREBALL_WARP_EFFECT)	
		return 1;

	return 0;
}

// mabye play sound effect for warp hole closing
void fireball_maybe_play_warp_close_sound(fireball *fb)
{
	float life_left;

	// If not a warphole fireball, do a quick out
	if ( !(fb->fireball_render_type == FIREBALL_WARP_EFFECT)) {
		return;
	}

	// If the warhole close sound has been played, don't play it again!
	if ( fb->flags & FBF_WARP_CLOSE_SOUND_PLAYED ) {
		return;
	}

	life_left = fb->total_time - fb->time_elapsed;

	if ( life_left < WARPHOLE_GROW_TIME ) {
		fireball_play_warphole_close_sound(fb);
		fb->flags |= FBF_WARP_CLOSE_SOUND_PLAYED;
	}
}

MONITOR( NumFireballs )

void fireball_process_post(object * obj, float frame_time)
{
	int			num, objnum;
	fireball		*fb;

	MONITOR_INC( NumFireballs, 1 );	

	num = obj->instance;
	objnum = OBJ_INDEX(obj);
	Assert( Fireballs[num].objnum == objnum );

	fb = &Fireballs[num];

	fb->time_elapsed += frame_time;
	if ( fb->time_elapsed > fb->total_time ) {
		obj->flags |= OF_SHOULD_BE_DEAD;
	}

	fireball_maybe_play_warp_close_sound(fb);

	fireball_set_framenum(num);
}

// Returns life left of a fireball in seconds
float fireball_lifeleft( object *obj )
{
	int			num, objnum;
	fireball		*fb;

	num = obj->instance;
	objnum = OBJ_INDEX(obj);
	Assert( Fireballs[num].objnum == objnum );

	fb = &Fireballs[num];

	return fb->total_time - fb->time_elapsed;
}

// Returns life left of a fireball in percent
float fireball_lifeleft_percent( object *obj )
{
	int			num, objnum;
	fireball		*fb;

	num = obj->instance;
	objnum = OBJ_INDEX(obj);
	Assert( Fireballs[num].objnum == objnum );

	fb = &Fireballs[num];

//	Assert((fb->total_time - fb->time_elapsed) / fb->total_time >=0);

	float p = (fb->total_time - fb->time_elapsed) / fb->total_time;
	if (p < 0)p=0.0f;
	return p;
}

// determine LOD to use
int fireball_get_lod(vec3d *pos, fireball_info *fd, float size)
{
	vertex v;
	int x, y, w, h, bm_size;
	int must_stop = 0;
	int ret_lod = 1;
	int behind = 0;

	// bogus
	if(fd == NULL){
		return 1;
	}

	// start the frame
	extern int G3_count;

	if(!G3_count){
		g3_start_frame(1);
		must_stop = 1;
	}
	g3_set_view_matrix(&Eye_position, &Eye_matrix, Eye_fov);

	// get extents of the rotated bitmap
	g3_rotate_vertex(&v, pos);

	// if vertex is behind, find size if in front, then drop down 1 LOD
	if (v.codes & CC_BEHIND) {
		float dist = vm_vec_dist_quick(&Eye_position, pos);
		vec3d temp;

		behind = 1;
		vm_vec_scale_add(&temp, &Eye_position, &Eye_matrix.vec.fvec, dist);
		g3_rotate_vertex(&v, &temp);

		// if still behind, bail and go with default
		if (v.codes & CC_BEHIND) {
			behind = 0;
		}
	}

	if(!g3_get_bitmap_dims(fd->lod[0].bitmap_id, &v, size, &x, &y, &w, &h, &bm_size)) {
		if (Detail.hardware_textures == 4) {
			// straight LOD
			if(w <= bm_size/8){
				ret_lod = 3;
			} else if(w <= bm_size/2){
				ret_lod = 2;
			} else if(w <= (1.56*bm_size)){
				ret_lod = 1;
			} else {
				ret_lod = 0;
			}		
		} else {
			// less aggressive LOD for lower detail settings
			if(w <= bm_size/8){
				ret_lod = 3;
			} else if(w <= bm_size/3){
				ret_lod = 2;
			} else if(w <= (1.2*bm_size)){
				ret_lod = 1;
			} else {
				ret_lod = 0;
			}		
		}
	}

	// if it's behind, bump up LOD by 1
	if (behind) {
		ret_lod++;
	}

	// end the frame
	if(must_stop){
		g3_end_frame();
	}

	// return the best lod
	return MIN(ret_lod, fd->lod_count - 1);
}

//	Create a fireball, return object index.
int fireball_create( vec3d * pos, int fireball_type, int render_type, int parent_obj, float size, int reverse, vec3d *velocity, float warp_lifetime, int ship_class, matrix *orient_override, int low_res, int extra_flags, int warp_open_sound, int warp_close_sound)
{
	int				n, objnum, fb_lod;
	object			*obj;
	fireball			*fb;
	fireball_info	*fd;
	fireball_lod	*fl;

	Assert( fireball_type > -1 );
	Assert( fireball_type < Num_fireball_types );

	fd = &Fireball_info[fireball_type];

	// check to make sure this fireball type exists
	if (!fd->lod_count)
		return -1;

	if ( !(Game_detail_flags & DETAIL_FLAG_FIREBALLS) )	{
		if ( !((fireball_type == FIREBALL_WARP) || (fireball_type == FIREBALL_KNOSSOS)) )	{
			return -1;
		}
	}

	if ( (Num_fireballs >= MAX_FIREBALLS) || (Num_objects >= MAX_OBJECTS) )	{
		// who cares if we don't create a spark.
		// JAS - Should this code be in?  Is it better to remove an old spark
		// and start a new one, or just not start the new one?
		//if ( fd->type == FIREBALL_TYPE_SMALL )	{
		//	return -1;
		//}

		//mprintf(( "Out of fireball slots, trying to free one up!\n" ));
		// out of slots, so free one up.
		n = fireball_free_one();
		if ( n < 0 )	{
			// If there's still no free slots, then exit
			//mprintf(( "ERROR: Couldn't free one up!!\n" ));
			return -1;
		} else {
			//mprintf(( "Freed one up just fine!!\n" ));
		}
	} else {
		for ( n = 0; n < MAX_FIREBALLS; n++ )	{
			if ( Fireballs[n].objnum < 0  )	{
				break;
			}
		}
		Assert( n != MAX_FIREBALLS );
	}

	fb = &Fireballs[n];

	// get an lod to use	
	fb_lod = fireball_get_lod(pos, fd, size);

	// change lod if low res is desired
	if (low_res) {
		fb_lod++;
		fb_lod = MIN(fb_lod, fd->lod_count - 1);
	}

	// if this is a warpout fireball, never go higher than LOD 1
	if(fireball_type == FIREBALL_WARP){
		/*
		if(fb_lod > 1){
			fb_lod = 1;
		}
		*/
		fb_lod = MAX_WARP_LOD;
	}
	fl = &fd->lod[fb_lod];

	fb->lod = (char)fb_lod;

	fb->flags = extra_flags;
	fb->warp_open_sound_index = warp_open_sound;
	fb->warp_close_sound_index = warp_close_sound;

	matrix orient;
	if(orient_override != NULL){
		orient = *orient_override;
	} else {
		if ( parent_obj < 0 )	{
			orient = vmd_identity_matrix;
		} else {
			orient = Objects[parent_obj].orient;
		}
	}
	
	objnum = obj_create(OBJ_FIREBALL, parent_obj, n, &orient, pos, size, OF_RENDERS);

	if (objnum < 0) {
		Int3();				// Get John, we ran out of objects for fireballs
		return objnum;
	}

	obj = &Objects[objnum];

	fb->fireball_info_index = fireball_type;
	fb->fireball_render_type = render_type;
	fb->time_elapsed = 0.0f;
	fb->objnum = objnum;
	fb->current_bitmap = -1;
	
	switch( fb->fireball_render_type )	{

		case FIREBALL_MEDIUM_EXPLOSION:	
			fb->orient = (myrand()>>8) & 7;							// 0 - 7
			break;

		case FIREBALL_LARGE_EXPLOSION:
			fb->orient = (myrand()>>8) % 360;						// 0 - 359
			break;

		case FIREBALL_WARP_EFFECT:
			// Play sound effect for warp hole opening up
			fireball_play_warphole_open_sound(ship_class, fb);

			// warp in type
			if (reverse)	{
				fb->orient = 1;
				// if warp out, then reverse the orientation
				vm_vec_scale( &obj->orient.vec.fvec, -1.0f );	// Reverse the forward vector
				vm_vec_scale( &obj->orient.vec.rvec, -1.0f );	// Reverse the right vector
			} else {
				fb->orient = 0;
			}
			break;

		default:
			Int3();
			break;
	}

	if ( fb->fireball_render_type == FIREBALL_WARP_EFFECT )	{
		Assert( warp_lifetime > 4.0f );		// Warp lifetime must be at least 4 seconds!
		fb->total_time = warp_lifetime;	// in seconds
	} else {
		fb->total_time = i2fl(fl->num_frames) / fl->fps;	// in seconds
	}
	
	fireball_set_framenum(n);

	if ( velocity )	{
		// Make the explosion move at a constant velocity.
		obj->flags |= OF_PHYSICS;
		obj->phys_info.mass = 1.0f;
		obj->phys_info.side_slip_time_const = 0.0f;
		obj->phys_info.rotdamp = 0.0f;
		obj->phys_info.vel = *velocity;
		obj->phys_info.max_vel = *velocity;
		obj->phys_info.desired_vel = *velocity;
		obj->phys_info.speed = vm_vec_mag(velocity);
		vm_vec_zero(&obj->phys_info.max_rotvel);
	}
	
	Num_fireballs++;
	return objnum;
}

// -----------------------------------------------------------------
//	fireball_close()
//
//	Called at game shutdown to clean up the fireball system
//
void fireball_close()
{
	if ( !fireballs_inited )
		return;

	fireball_delete_all();
}

// -----------------------------------------------------------------
//	fireball_level_close()
//
//	Called when a mission ends... frees up any animations that might
// be partially played
//
void fireball_level_close()
{
	if ( !fireballs_inited )
		return;

	fireball_delete_all();
}

void fireballs_page_in()
{
	int				i, idx;
	fireball_info	*fd;

	for ( i = 0; i < Num_fireball_types; i++ ) {
		if((i < NUM_DEFAULT_FIREBALLS) || fireball_used[i]){
			fd = &Fireball_info[i];

			// if this is a Knossos ani, only load if Knossos_warp_ani_used is true
			if ( (i == FIREBALL_KNOSSOS) && !Knossos_warp_ani_used)
				continue;

			for(idx=0; idx<fd->lod_count; idx++) {
				// we won't use a warp effect lod greater than MAX_WARP_LOD so don't load it either
				if ( (i == FIREBALL_WARP) && (idx > MAX_WARP_LOD) )
					continue;

				bm_page_in_texture( fd->lod[idx].bitmap_id, fd->lod[idx].num_frames );
			}
		}
	}

	bm_page_in_texture( Warp_glow_bitmap );
	bm_page_in_texture( Warp_ball_bitmap );
}

void fireball_get_color(int idx, float *red, float *green, float *blue)
{
	Assert( red && blue && green );

	if ( (idx < 0) || (idx >= Num_fireball_types) ) {
		Int3();
		
		*red = 1.0f;
		*green = 1.0f;
		*blue = 1.0f;

		return;
	}

	fireball_info *fbi = &Fireball_info[idx];

	*red = fbi->exp_color[0];
	*green = fbi->exp_color[1];
	*blue = fbi->exp_color[2];
}

int fireball_ship_explosion_type(ship_info *sip)
{
	Assert( sip != NULL );

	int index = -1;
	int ship_fireballs = (int)sip->explosion_bitmap_anims.size();
	int objecttype_fireballs = -1;

	if (sip->class_type >= 0) {
		objecttype_fireballs = (int)Ship_types[sip->class_type].explosion_bitmap_anims.size();
	}

	if(ship_fireballs > 0){
		index = sip->explosion_bitmap_anims[rand()%ship_fireballs];
	} else if(objecttype_fireballs > 0){
		index = Ship_types[sip->class_type].explosion_bitmap_anims[rand()%objecttype_fireballs];
	}

	return index;
}
