/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/MenuUI/Barracks.cpp $
 * $Revision: 2.29.2.1 $
 * $Date: 2006-08-27 18:11:37 $
 * $Author: taylor $
 *
 * C file for implementing barracks section
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.29  2005/12/29 08:08:36  wmcoolmon
 * Codebase commit, most notably including objecttypes.tbl
 *
 * Revision 2.28  2005/10/10 17:21:05  taylor
 * remove NO_NETWORK
 *
 * Revision 2.27  2005/09/22 19:16:48  taylor
 * fix screen update of pilot list when you delete one of them
 *
 * Revision 2.26  2005/08/18 01:15:41  taylor
 * Address a couple of pilot code issues
 *  - make the same MAX_PILOTS in the barracks that the playerselect screen has since it's a problem here too
 *  - add a -1 check for ship/weapon name lookups to avoid bounds issues.  NOTE: this can empty the techroom of entries if ithits this check, but it won't crash :)
 *
 * Revision 2.25  2005/07/22 10:18:36  Goober5000
 * CVS header tweaks
 * --Goober5000
 *
 * Revision 2.24  2005/07/18 03:45:07  taylor
 * more non-standard res fixing
 *  - I think everything should default to resize now (much easier than having to figure that crap out)
 *  - new mouse_get_pos_unscaled() function to return 1024x768/640x480 relative values so we don't have to do it later
 *  - lots of little cleanups which fix several strange offset/size problems
 *  - fix gr_resize/unsize_screen_pos() so that it won't wrap on int (took too long to track this down)
 *
 * Revision 2.23  2005/07/13 03:15:50  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.22  2005/07/02 19:43:53  taylor
 * ton of non-standard resolution fixes
 *
 * Revision 2.21  2005/06/03 06:41:37  taylor
 * eek... fix wrong description of changes
 *
 * Revision 2.20  2005/06/03 06:39:26  taylor
 * fix issue where, when in multi mode, loading a single player would overwrite the campaign savefile
 *
 * Revision 2.19  2005/05/26 04:28:45  taylor
 * if we switch to a pilot who has a campaign that isn't available then abort the read and
 *   deselect all pilots, should prevent data loss or crashes due to the missing safety check
 *
 * Revision 2.18  2005/03/27 06:12:38  taylor
 * some pilot file fixing when going between multi and single (partial fix)
 *
 * Revision 2.17  2005/03/02 21:24:44  taylor
 * more network/inferno goodness for Windows, takes care of a few warnings too
 *
 * Revision 2.16  2005/02/23 04:51:56  taylor
 * some bm_unload() -> bm_release() changes to save bmpman slots
 *
 * Revision 2.15  2005/02/04 23:29:32  taylor
 * merge with Linux/OSX tree - p0204-3
 *
 * Revision 2.14  2005/01/29 08:08:24  wmcoolmon
 * Various updates; shader, assert, and clipping
 *
 * Revision 2.13  2005/01/10 04:44:03  wmcoolmon
 * Small fix, so we don't try to delete the same thing twice
 *
 * Revision 2.12  2005/01/09 22:27:38  wmcoolmon
 * Techroom and Barracks now allocate memory only when open, and only for the number of ships currently loaded
 *
 * Revision 2.11  2004/12/22 21:49:05  taylor
 * add a popup to make sure people know about pilot upgrade
 *
 * Revision 2.10  2004/10/31 21:53:23  taylor
 * new pilot code support, no-multiplayer and compiler warning fixes, center mouse cursor for redalert missions
 *
 * Revision 2.9  2004/07/26 20:47:36  Kazan
 * remove MCD complete
 *
 * Revision 2.8  2004/07/12 16:32:53  Kazan
 * MCD - define _MCD_CHECK to use memory tracking
 *
 * Revision 2.7  2004/03/05 09:01:53  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.6  2004/02/14 00:18:33  randomtiger
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
 * Revision 2.5  2004/02/10 21:47:47  Goober5000
 * fixed the barracks crash bug
 * --Goober5000
 *
 * Revision 2.4  2003/08/20 08:11:00  wmcoolmon
 * Added error screens to the barracks and start screens when a pilot file can't be deleted
 *
 * Revision 2.3  2003/03/18 10:07:03  unknownplayer
 * The big DX/main line merge. This has been uploaded to the main CVS since I can't manage to get it to upload to the DX branch. Apologies to all who may be affected adversely, but I'll work to debug it as fast as I can.
 *
 * Revision 2.2.2.1  2002/09/24 18:56:43  randomtiger
 * DX8 branch commit
 *
 * This is the scub of UP's previous code with the more up to date RT code.
 * For full details check previous dev e-mails
 *
 * Revision 2.2  2002/08/01 01:41:06  penguin
 * The big include file move
 *
 * Revision 2.1  2002/07/07 19:55:59  penguin
 * Back-port to MSVC
 *
 * Revision 2.0  2002/06/03 04:02:24  penguin
 * Warpcore CVS sync
 *
 * Revision 1.2  2002/05/04 04:52:22  mharris
 * 1st draft at porting
 *
 * Revision 1.1  2002/05/02 18:03:09  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 42    10/25/99 5:47p Jefff
 * reassigned some xstr ids
 * 
 * 41    9/14/99 11:01p Jefff
 * stats area coord fix
 * 
 * 40    9/09/99 11:24a Jefff
 * 
 * 39    9/02/99 11:26a Jefff
 * fixed incorrect pilot list dimensions
 * 
 * 38    8/17/99 2:24p Dave
 * Fixed wacky squad color stuff.
 * 
 * 37    8/16/99 4:27p Jefff
 * fix button position on squad change error popup
 * 
 * 36    8/11/99 3:58p Jefff
 * fixed player list box coord bug
 * 
 * 35    8/05/99 2:46p Jefff
 * added popup to the disabled squad selection buttons in single player
 * 
 * 34    8/02/99 11:02p Dave
 * Fixed up squad filename and squad name copying when converting a pilot
 * between single and multiplayer.
 * 
 * 33    7/27/99 7:17p Jefff
 * Replaced some art text with XSTR() text.
 * 
 * 32    7/15/99 9:20a Andsager
 * FS2_DEMO initial checkin
 * 
 * 31    6/29/99 7:39p Dave
 * Lots of small bug fixes.
 * 
 * 30    6/16/99 4:06p Dave
 * New pilot info popup. Added new draw-bitmap-as-poly function.
 * 
 * 29    6/11/99 11:13a Dave
 * last minute changes before press tour build.
 * 
 * 28    4/25/99 7:43p Dave
 * Misc small bug fixes. Made sun draw properly.
 * 
 * 27    4/25/99 3:02p Dave
 * Build defines for the E3 build.
 * 
 * 26    3/24/99 4:05p Dave
 * Put in support for assigning the player to a specific squadron with a
 * specific logo. Preliminary work for doing pos/orient checksumming in
 * multiplayer to reduce bandwidth.
 * 
 * 25    2/25/99 4:19p Dave
 * Added multiplayer_beta defines. Added cd_check define. Fixed a few
 * release build warnings. Added more data to the squad war request and
 * response packets.
 * 
 * 24    2/02/99 12:09p Neilk
 * Fixed text coords, centered pilot and squad pics in 1024x768, added vss
 * comment template
 *
 * $NoKeywords: $
 */

#include <ctype.h>


#include "menuui/barracks.h"
#include "playerman/managepilot.h"
#include "ui/ui.h"
#include "graphics/font.h"
#include "io/key.h"
#include "gamesnd/gamesnd.h"
#include "popup/popup.h"
#include "menuui/playermenu.h"
#include "gamesequence/gamesequence.h"
#include "gamehelp/contexthelp.h"
#include "freespace2/freespace.h"
#include "io/mouse.h"
#include "osapi/osregistry.h"
#include "globalincs/alphacolors.h"
#include "playerman/player.h"
#include "ship/ship.h"
#include "cfile/cfile.h"
#include "parse/parselo.h"
#include "mission/missioncampaign.h"




//Returns 0 on failure, 1 on success
int delete_pilot_file( char *pilot_name, int single );		// manage_pilot.cpp

// stats defines
//#define NUM_STAT_LINES (21 + MAX_SHIP_CLASSES)	// Goober5000
#define STAT_COLUMN1_W 40
#define STAT_COLUMN2_W 10

static int Stat_column1_w[GR_NUM_RESOLUTIONS] =
{
	40,		// GR_640
	40			// GR_1024
};

static int Stat_column2_w[GR_NUM_RESOLUTIONS] =
{
	10,		// GR_640
	10			// GR_1024
};

// constants for coordinate lookup
#define BARRACKS_X_COORD 0
#define BARRACKS_Y_COORD 1
#define BARRACKS_W_COORD 2
#define BARRACKS_H_COORD 3

// area ints

// pilot selection field
static int Barracks_list_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		42, 34, 400, 90
	},
	{ // GR_1024
		45, 51, 646, 144
	}
};

// pilot stats field
static int Barracks_stats_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		32, 212, 240, 250
	},
	{ // GR_1024
		42, 351, 240, 400
	}
};

static int Barracks_stats2_coords[GR_NUM_RESOLUTIONS][3] = {
	{ // GR_640
		276, 212, 81		// X2, , W2
	},
	{ // GR_1024
		286, 351, 81		// X2, , W2
	}
};

// pilot picture field
static int Barracks_image_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		461, 23, 160, 120
	},
	{ // GR_1024
		782, 58, 160, 120
	}
};

// pilot picture # of # location
static int Barracks_image_number_coords[GR_NUM_RESOLUTIONS][2] = {
	{ // GR_640
		461, 145
	},
	{ // GR_1024
		732, 239
	}
};

// pilot squad logo field
int Barracks_squad_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		495, 177, 128, 128
	},
	{ // GR_1024
		829, 323, 128, 128
	}
};

// pilot squad # of # location
int Barracks_squad_number_coords[GR_NUM_RESOLUTIONS][2] = {
	{ // GR_640
		492, 307
	},
	{ // GR_1024
		794, 500
	}
};

// button defines
#define BARRACKS_NUM_BUTTONS		19

// pilot selection buttons
#define B_PILOT_CREATE_BOTTON			0	// B_PILOT_CREATE_BOTTON
#define B_PILOT_SCROLL_UP_BUTTON		1	// B_PILOT_SCROLL_UP_BUTTON
#define B_PILOT_SCROLL_DOWN_BUTTON	2	// B_PILOT_SCROLL_DOWN_BUTTON
#define B_PILOT_DELETE_BUTTON			11	// B_PILOT_B_PILOT_DELETE_BUTTON
#define B_PILOT_SET_ACTIVE_BUTTON	12	// B_PILOT_B_PILOT_SET_ACTIVE_BUTTON
#define B_PILOT_CLONE_BUTTON			13	// B_PILOT_B_PILOT_CLONE_BUTTON
#define B_PILOT_SINGLE_MODE_BUTTON	14	// B_PILOT_SINGLE_MODE_BUTTON
#define B_PILOT_MULTI_MODE_BUTTON	15	// B_PILOT_MULTI_MODE_BUTTON
#define B_PILOT_CONVERT_BUTTON		16	// B_PILOT_B_PILOT_CONVERT_BUTTON

// squad logo picture buttons
#define B_SQUAD_PREV_BUTTON			17
#define B_SQUAD_NEXT_BUTTON			18

// pilot picture buttons
#define B_PIC_PREV_PILOT_BUTTON		3	// B_PILOT_B_PIC_PREV_PILOT_BUTTON
#define B_PIC_NEXT_PILOT_BUTTON		4	// B_PILOT_B_PIC_NEXT_PILOT_BUTTON

// pilot stat buttons
#define B_STATS_MEDAL_BUTTON			8	// B_STATS_MEDAL_BUTTON
#define B_STATS_SCROLL_DOWN_BUTTON	9	// B_STATS_SCROLL_DOWN_BUTTON
#define B_STATS_SCROLL_UP_BUTTON		10	// B_STATS_SCROLL_UP_BUTTON

// general buttons
#define B_ACCEPT_BUTTON					5	// B_B_ACCEPT_BUTTON
#define B_HELP_BUTTON					6	// B_B_HELP_BUTTON
#define B_OPTION_BUTTON					7	// B_OPTION_BUTTON

//XSTR:OFF
// bitmaps defs
static char *Barracks_bitmap_fname[GR_NUM_RESOLUTIONS] = {
	"Barracks",		// GR_640
	"2_Barracks"		// GR_1024
};

static char *Barracks_bitmap_mask_fname[GR_NUM_RESOLUTIONS] = {
	"Barracks-M",		// GR_640
	"2_Barracks-M"		// GR_1024
};

//XSTR:ON

#define BARRACKS_IMAGE_NOT_LOADED	-2

struct barracks_bitmaps {
	char *filename;
	int x, y;
	int b;
};

struct barracks_buttons {
	char *filename;
	int x, y;
	int text_x, text_y;	// this is where the text label is
	int hotspot;
	int repeat;
	UI_BUTTON button;  // because we have a class inside this struct, we need the constructor below..

	barracks_buttons(char *name, int x1, int y1, int x2, int y2, int h, int r = 0) : filename(name), x(x1), y(y1), text_x(x2), text_y(y2), hotspot(h), repeat(r) {}
};

static int Background_bitmap = -1;
static UI_WINDOW Ui_window;
static UI_BUTTON List_region;
static UI_INPUTBOX Inputbox;

static barracks_buttons Buttons[GR_NUM_RESOLUTIONS][BARRACKS_NUM_BUTTONS] = {
//XSTR:OFF
	{		// GR_640
			barracks_buttons("BAB_00", 8,		122,	11,	157,	0),		
			barracks_buttons("BAB_01", 323,	130,	0,		0,		1, 1),
			barracks_buttons("BAB_02", 350,	130,	0,		0,		2,	1),
			barracks_buttons("BAB_03", 559,	143,	0,		0,		3,	1),			
			barracks_buttons("BAB_04", 598,	143,	0,		0,		4,	1),
			barracks_buttons("BAB_05", 571,	425,	578,	413,	5),
			barracks_buttons("BAB_06", 533,	425,	500,	438,	6),
			barracks_buttons("BAB_07", 533,	453,	481,	465,	7),
			barracks_buttons("BAB_08", 361,	425,	401,	465,	8),
			barracks_buttons("BAB_09", 0,		267,	0,		0,		9,	1),
			barracks_buttons("BAB_10", 0,		224,	0,		0,		10,1),
			barracks_buttons("BAB_11", 120,	122,	123,	157,	11),	
			barracks_buttons("BAB_12", 376,	124,	378,	163,	12),
			barracks_buttons("BAB_13", 66,	122,	69,	157,	13),
			barracks_buttons("BAB_14", 323,	0,		324,	25,	14),
			barracks_buttons("BAB_15", 372,	0,		374,	25,	15),
			barracks_buttons("BAB_16", 180,	122,	182,	157,	16),
			barracks_buttons("BAB_17", 559,	306,	0,		0,		17),
			barracks_buttons("BAB_18", 598,	306,	0,		0,		18)
	},
	{		// GR_1024
			barracks_buttons("2_BAB_00", 14,		196,	35,	252,	0),
			barracks_buttons("2_BAB_01", 518,	209,	0,		0,		1,	1),
			barracks_buttons("2_BAB_02", 561,	209,	0,		0,		2,	1),
			barracks_buttons("2_BAB_03", 896,	229,	0,		0,		3,	1),
			barracks_buttons("2_BAB_04", 958,	229,	0,		0,		4,	1),
			barracks_buttons("2_BAB_05", 914,	681,	932,	665,	5),
			barracks_buttons("2_BAB_06", 854,	681,	800,	704,	6),
			barracks_buttons("2_BAB_07", 854,	724,	778,	743,	7),
			barracks_buttons("2_BAB_08", 579,	681,	641,	743,	8),
			barracks_buttons("2_BAB_09", 0,		428,	0,		0,		9,	1),
			barracks_buttons("2_BAB_10", 0,		360,	0,		0,		10,1),
			barracks_buttons("2_BAB_11", 193,	196,	214,	252,	11),
			barracks_buttons("2_BAB_12", 602,	200,	617,	262,	12),
			barracks_buttons("2_BAB_13", 107,	196,	128,	252,	13),
			barracks_buttons("2_BAB_14", 517,	0,		532,	40,	14),
			barracks_buttons("2_BAB_15", 596,	0,		614,	40,	15),
			barracks_buttons("2_BAB_16", 289,	196,	309,	252,	16),
			barracks_buttons("2_BAB_17", 896,	491,	0,		0,		17),
			barracks_buttons("2_BAB_18", 958,	491,	0,		0,		18)
	}
//XSTR:ON
};


// FIXME add to strings.tbl, set correct coords
#define BARRACKS_NUM_TEXT			2
UI_XSTR Barracks_text[GR_NUM_RESOLUTIONS][BARRACKS_NUM_TEXT] = {
	{ 
		// GR_640
		{ "Barracks",			1434,		17, 7,		UI_XSTR_COLOR_GREEN, -1, NULL },
		{ "Pilot Stats",		1435,		17, 180,		UI_XSTR_COLOR_GREEN, -1, NULL }
	}, 
	{ 
		// GR_1024
		{ "Barracks",			1434,		27, 11,		UI_XSTR_COLOR_GREEN, -1, NULL },
		{ "Pilot Stats",		1435,		27, 288,		UI_XSTR_COLOR_GREEN, -1, NULL }
	}
};


static int Num_stat_lines;
static char (*Stat_labels)[STAT_COLUMN1_W];
static char (*Stats)[STAT_COLUMN2_W];

extern int Player_sel_mode;

static player *Cur_pilot;
static int Num_pilots;
static int Selected_line;
static char Pilots_arr[MAX_PILOTS][MAX_FILENAME_LEN];
static char *Pilots[MAX_PILOTS];
static int Pic_number;
static int Pic_squad_number;
static bool Barracks_callsign_enter_mode;
static int Pilot_ranks[MAX_PILOTS];
static int List_scroll_offset;
static int Stats_scroll_offset;
static int Clone_flag;
static int Pilot_images[MAX_PILOT_IMAGES];
static int Pilot_squad_images[MAX_PILOT_IMAGES];
static int Rank_pips_bitmaps;
static int Rank_pips_count;

void barracks_squad_change_popup();


// -------------------------------------------------------------------------------------------------
//
// BARRACKS screen
//

#define STRCPY1(a, b) do {	\
	Assert(strlen(b) < STAT_COLUMN1_W); \
	strcpy(a, b); \
} while (0)

void barracks_init_stats(scoring_struct *stats)
{
	int Max_stat_lines = Num_ship_classes + 23;
	int i;
	float f;

	//Set up variables
	if(Stat_labels != NULL)
	{
		vm_free(Stat_labels);
	}
	if(Stats != NULL)
	{
		vm_free(Stats);
	}

	Stat_labels = (char (*)[STAT_COLUMN1_W]) vm_malloc(sizeof(char)*(Max_stat_lines * STAT_COLUMN1_W));
	Stats = (char (*)[STAT_COLUMN2_W]) vm_malloc(sizeof(char)*(Max_stat_lines * STAT_COLUMN2_W));

	//Now start throwing stuff in
	Num_stat_lines = 0;

	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "*All Time Stats", 50));
	Stats[Num_stat_lines][0] = 0;
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	Stat_labels[Num_stat_lines][0] = 0;
	Stats[Num_stat_lines][0] = 0;
	Num_stat_lines++;	

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Primary weapon shots:", 51));
	sprintf(Stats[Num_stat_lines], "%d", stats->p_shots_fired);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Primary weapon hits:", 52));
	sprintf(Stats[Num_stat_lines], "%d", stats->p_shots_hit);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Primary friendly hits:", 53));
	sprintf(Stats[Num_stat_lines], "%d", stats->p_bonehead_hits);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Primary hit %:", 54));
	if (stats->p_shots_fired > 0) {
		f = (float) stats->p_shots_hit * 100.0f / (float) stats->p_shots_fired;
	} else {
		f = 0.0f;
	}
	sprintf(Stats[Num_stat_lines], XSTR( "%.1f%%", 55), f);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Primary friendly hit %:", 56));
	if (stats->p_bonehead_hits > 0) {
		f = (float) stats->p_bonehead_hits * 100.0f / (float) stats->p_shots_fired;
	} else {
		f = 0.0f;
	}
	sprintf(Stats[Num_stat_lines], XSTR( "%.1f%%", 55), f);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	Stat_labels[Num_stat_lines][0] = 0;
	Stats[Num_stat_lines][0] = 0;
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Secondary weapon shots:", 57));
	sprintf(Stats[Num_stat_lines], "%d", stats->s_shots_fired);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Secondary weapon hits:", 58));
	sprintf(Stats[Num_stat_lines], "%d", stats->s_shots_hit);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Secondary friendly hits:", 59));
	sprintf(Stats[Num_stat_lines], "%d", stats->s_bonehead_hits);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Secondary hit %:", 60));
	if (stats->s_shots_fired > 0) {
		f = (float) stats->s_shots_hit * 100.0f / (float) stats->s_shots_fired;
	} else {
		f = 0.0f;
	}
	sprintf(Stats[Num_stat_lines], XSTR( "%.1f%%", 55), f);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Secondary friendly hit %:", 61));
	if (stats->s_bonehead_hits > 0) {
		f = (float) stats->s_bonehead_hits * 100.0f / (float) stats->s_shots_fired;
	} else {
		f = 0.0f;
	}
	sprintf(Stats[Num_stat_lines], XSTR( "%.1f%%", 55), f);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	Stat_labels[Num_stat_lines][0] = 0;
	Stats[Num_stat_lines][0] = 0;
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Total kills:", 62));
	sprintf(Stats[Num_stat_lines], "%d", stats->kill_count_ok);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "Assists:", 63));
	sprintf(Stats[Num_stat_lines], "%d", stats->assists);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	Stat_labels[Num_stat_lines][0] = 0;
	Stats[Num_stat_lines][0] = 0;
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	STRCPY1(Stat_labels[Num_stat_lines], "Current Score:");
	sprintf(Stats[Num_stat_lines], "%d", stats->score);
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	Stat_labels[Num_stat_lines][0] = 0;
	Stats[Num_stat_lines][0] = 0;
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	Stat_labels[Num_stat_lines][0] = 0;
	Stats[Num_stat_lines][0] = 0;
	Num_stat_lines++;

	STRCPY1(Stat_labels[Num_stat_lines], XSTR( "*Kills by Ship Type", 64));
	Stats[Num_stat_lines][0] = 0;
	Num_stat_lines++;

	Assert(Num_stat_lines < Max_stat_lines);
	Stat_labels[Num_stat_lines][0] = 0;
	Stats[Num_stat_lines][0] = 0;
	Num_stat_lines++;

	// Goober5000 - make sure we have room for all ships
	Assert((Num_stat_lines + Num_ship_classes) < Max_stat_lines);

	for (i=0; i<Num_ship_classes; i++) {
		if (stats->kills[i]) {
			Assert(Num_stat_lines < Max_stat_lines);

			// Goober5000 - in case above Assert isn't triggered (such as in non-debug builds)
			if (Num_stat_lines >= Max_stat_lines)
			{
				break;
			}

			Assert(strlen(Ship_info[i].name) + 1 < STAT_COLUMN1_W);
			sprintf(Stat_labels[Num_stat_lines], NOX("%s:"), Ship_info[i].name);
			sprintf(Stats[Num_stat_lines], "%d", stats->kills[i]);
			Num_stat_lines++;
		}
	}

	for (i=0; i<Num_stat_lines; i++) {
		gr_force_fit_string(Stat_labels[i], Stat_column1_w[gr_screen.res], Barracks_stats_coords[gr_screen.res][BARRACKS_W_COORD]);
		gr_force_fit_string(Stats[i], Stat_column2_w[gr_screen.res], Barracks_stats2_coords[gr_screen.res][BARRACKS_W_COORD]);
	}
}

// sets or clears hotkeys for pilot selection.
void barracks_set_hotkeys(bool pilot_text_enter_mode)
{
	Buttons[gr_screen.res][B_PILOT_SCROLL_UP_BUTTON].button.set_hotkey(pilot_text_enter_mode ? KEY_UP : -1);
	Buttons[gr_screen.res][B_PILOT_SCROLL_DOWN_BUTTON].button.set_hotkey(pilot_text_enter_mode ? KEY_DOWN : -1);

	Buttons[gr_screen.res][B_PILOT_CREATE_BOTTON].button.set_hotkey(pilot_text_enter_mode ? KEY_C : -1);
	Buttons[gr_screen.res][B_PILOT_SET_ACTIVE_BUTTON].button.set_hotkey(pilot_text_enter_mode ? KEY_ENTER : -1);
	Buttons[gr_screen.res][B_PILOT_DELETE_BUTTON].button.set_hotkey(pilot_text_enter_mode ? KEY_DELETE : -1);

	Buttons[gr_screen.res][B_PIC_PREV_PILOT_BUTTON].button.set_hotkey(pilot_text_enter_mode ? KEY_LEFT : -1);
	Buttons[gr_screen.res][B_PIC_NEXT_PILOT_BUTTON].button.set_hotkey(pilot_text_enter_mode ? KEY_RIGHT : -1);

	Buttons[gr_screen.res][B_ACCEPT_BUTTON].button.set_hotkey(pilot_text_enter_mode ? KEY_CTRLED | KEY_ENTER : -1);
}

// strip the possible .pcx extension off a filename
void barracks_strip_pcx(char *str)
{
	int flen = strlen(str);
	int elen = 4;		
	if ((flen > 4) && !stricmp(str + flen - elen, ".pcx")) {
		str[flen - elen] = '\0';
	}
}

// new pilot name has focus, so update stats/pic to that pilot
int barracks_new_pilot_selected()
{
	char stripped[MAX_FILENAME_LEN+1] = "";

	// save the previous pilot first, so changes to it are kept
	if (strlen(Cur_pilot->callsign)) {
		write_pilot_file();
		mission_campaign_savefile_save();
	}

	// check if we have a valid pilot hilighted.  If so, attempt to active it
	if ((Num_pilots < 1) || (Selected_line < 0) || (Selected_line >= Num_pilots)) {
		Cur_pilot->callsign[0] = 0;  // this indicates no pilot active
		return -1;
	}

	// check to see if we are going to try and upgrade or not
	if ( pilot_file_upgrade_check(Pilots[Selected_line], !Player_sel_mode) )
		return -1;

	if (read_pilot_file(Pilots[Selected_line], !Player_sel_mode, Cur_pilot)) {
		Cur_pilot->callsign[0] = 0;  // this indicates no pilot active
		return -1;
	} else {
		// we need to load up the current campaign file now so we can get at the stats - taylor
		// had to change this to import the values to a specific player rather than global
		if (Player_sel_mode == PLAYER_SELECT_MODE_SINGLE) {
			mission_load_up_campaign(Cur_pilot);

		/*	if (Campaign_file_missing) {
				// error popup for a missing campaign file, abort loading of pilot in this case
				// TODO: need to handle reading of info without the risk of saving improper data later
				popup( PF_NO_NETWORKING, 1, POPUP_OK, XSTR( "The currently active campaign for this pilot cannot be found.  Unable to safely open pilot.", -1));
				Cur_pilot->callsign[0] = 0; // this indicates no pilot active
				return -1;
			} */
		}
	}

	// init stuff to reflect new pilot
	int i;
	barracks_init_stats(&Cur_pilot->stats);
	for (i=0; i<Num_pilot_images; i++) {
		strcpy(stripped, Cur_pilot->image_filename);
		barracks_strip_pcx(stripped);
		if (!stricmp(stripped, Pilot_image_names[i])) {
			break;
		}
	}
	Pic_number = i;
	for ( i=0; i<Num_pilot_squad_images; i++) {
		strcpy(stripped, Cur_pilot->squad_filename);
		barracks_strip_pcx(stripped);
		if (!stricmp(stripped, Pilot_squad_image_names[i])) {
			break;
		}
	}
	Pic_squad_number = i;	
	
	return 0;
}

void barracks_set_callsign_enter_mode(bool set_callsign_enter_mode)
{
	// set global mode variable
	Barracks_callsign_enter_mode = set_callsign_enter_mode;

	// disable/enable all buttons
	for (int idx=0; idx<BARRACKS_NUM_BUTTONS; idx++) {
		// don't ever mess with the prev and next squad logo buttons
		if((idx != B_SQUAD_PREV_BUTTON) && (idx != B_SQUAD_NEXT_BUTTON)){
			Buttons[gr_screen.res][idx].button.enable(!set_callsign_enter_mode);
		}
	}

	// enable/disable hotkeys
	barracks_set_hotkeys(!set_callsign_enter_mode);
	// disable/enable inputbox
	Inputbox.enable(set_callsign_enter_mode);
	// hide/unhide inputbox
	Inputbox.hide(!set_callsign_enter_mode);
}

// creates a new pilot file
void barracks_create_new_pilot()
{
	// check if too many pilots
	if (Num_pilots >= MAX_PILOTS) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	// play sound for pilot creation
	gamesnd_play_iface(SND_SCROLL);
	
	// only write pilot file if there is an active pilot
	if (strlen(Player->callsign)) {
		write_pilot_file();
		mission_campaign_savefile_save();
	}

	// move other pilot names and ranks down to make room for the new one
	int idx = Num_pilots;
	Assert(Num_pilots >= 0);
	while (idx--) {
		strcpy(Pilots[idx + 1], Pilots[idx]);
		Pilot_ranks[idx + 1] = Pilot_ranks[idx];
	}

	Selected_line = 0;
	Num_pilots++;
	Pilots[Selected_line][0] = 0;
	Pilot_ranks[Selected_line] = 0;
	List_scroll_offset = 0;

	// set mode to accept pilot name text
	barracks_set_callsign_enter_mode(true);
	// set focus to input box
	Inputbox.set_focus();
	// set initial pilot name to ""
	Inputbox.set_text("");
	// reset size of input box to only 1 line
	Inputbox.update_dimensions(Barracks_list_coords[gr_screen.res][BARRACKS_X_COORD], Barracks_list_coords[gr_screen.res][BARRACKS_Y_COORD], Barracks_list_coords[gr_screen.res][BARRACKS_W_COORD], gr_get_font_height());
}

// exiting screen without canceling, so load in the new pilot selected here
int barracks_pilot_accepted()
{
	char str[CALLSIGN_LEN + 1];

	// check if pilot active.  If not, don't allow accept.
	if (!Cur_pilot->callsign[0]){
		return -1;
	}

	// set his image 
	player_set_squad_bitmap(Cur_pilot, Cur_pilot->squad_filename);

//	Skill_level = get_default_skill_level();

	// MWA -- I think that we should be writing Cur_pilot here.
	//write_pilot_file(!is_pilot_multi(Cur_pilot));
	write_pilot_file( Cur_pilot );
	
	// when we store the LastPlayer key, we have to mark it as being single or multiplayer, so we know where to look for him
	// (since we could have a single and a multiplayer pilot with the same callsign)
	// we'll distinguish them by putting an M and the end of the multiplayer callsign and a P at the end of a single player
	strcpy(str, Cur_pilot->callsign);
	strcat(str, is_pilot_multi(Cur_pilot) ? NOX("M") : NOX("S"));
	os_config_write_string( NULL, "LastPlayer", str );
	return 0;
}

// scroll up barracks pilot list one line
void barracks_scroll_callsign_up()
{
	if (Selected_line > 0) {
		Selected_line--;
		gamesnd_play_iface(SND_SCROLL);
	} else {
		gamesnd_play_iface(SND_GENERAL_FAIL);
	}
	
	if ((Selected_line >= 0) && (Selected_line < List_scroll_offset)) {
		List_scroll_offset = Selected_line;
	}
}

// scroll down barracks pilot list one line
void barracks_scroll_callsign_down()
{
	if (Selected_line < Num_pilots - 1) {
		Selected_line++;
		gamesnd_play_iface(SND_SCROLL);
	} else {
		gamesnd_play_iface(SND_GENERAL_FAIL);
	}
	
	// num_pilots_to_fill_height is the number of pilots that can fit in given height
	int num_pilots_to_fill_height = Barracks_list_coords[gr_screen.res][BARRACKS_H_COORD] / gr_get_font_height();
	if (Selected_line >= List_scroll_offset + num_pilots_to_fill_height) {
		List_scroll_offset++;
	}
}

// scroll up barracks stats list one line
void barracks_scroll_stats_up()
{
	if (Stats_scroll_offset > 0) {
		Stats_scroll_offset--;
		gamesnd_play_iface(SND_SCROLL);
	} else {
		gamesnd_play_iface(SND_GENERAL_FAIL);
	}
}

// scroll down barracks stats list one line
void barracks_scroll_stats_down()
{
	int font_height = gr_get_font_height();

	if (Stats_scroll_offset + Barracks_stats_coords[gr_screen.res][BARRACKS_H_COORD] / font_height < Num_stat_lines) {
		Stats_scroll_offset++;
		gamesnd_play_iface(SND_SCROLL);
	} else {
		gamesnd_play_iface(SND_GENERAL_FAIL);
	}
}

// show previous pilot pic
void barracks_prev_pic()
{
	// check if no pilot images or no pilot selected
	if ((Num_pilot_images == 0) || (Cur_pilot->callsign[0] == '\0')) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	// reset pilot pic number
	Pic_number--;
	if (Pic_number < 0) {
		Pic_number = Num_pilot_images - 1;
	}

	// copy pilot pic filename into pilot struct
	if ((Pic_number >= 0) && (Pic_number < Num_pilot_images)) {
		strcpy(Cur_pilot->image_filename, Pilot_image_names[Pic_number]);
	}

	// play scroll sound
	gamesnd_play_iface(SND_SCROLL);
}

// show next pilot pic
void barracks_next_pic()
{
	// check if no pilot images or no pilot selected
	if ((Num_pilot_images == 0) || (Cur_pilot->callsign[0] == '\0')) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	// reset pilot pic number
	Pic_number++;
	if (Pic_number >= Num_pilot_images){
		Pic_number = 0;
	}

	// copy pilot pic filename into pilot struct
	if ((Pic_number >= 0) && (Pic_number < Num_pilot_images)){
		strcpy(Cur_pilot->image_filename, Pilot_image_names[Pic_number]);
	}

	// play scroll sound
	gamesnd_play_iface(SND_SCROLL);
}

// show previous squad pic
void barracks_prev_squad_pic()
{
	// check if no pilot images or no pilot selected
	if ((Num_pilot_squad_images == 0) || (Cur_pilot->callsign[0] == '\0')) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	// reset pilot pic number
	Pic_squad_number--;
	if (Pic_squad_number < 0) {
		Pic_squad_number = Num_pilot_squad_images - 1;
	}

	// copy pilot pic filename into pilot struct
	if ((Pic_squad_number >= 0) && (Pic_squad_number < Num_pilot_squad_images)) {
		strcpy(Cur_pilot->squad_filename, Pilot_squad_image_names[Pic_squad_number]);
	}

	// play scroll sound
	gamesnd_play_iface(SND_SCROLL);
}

// show next pilot pic
void barracks_next_squad_pic()
{
	// check if no pilot images or no pilot selected
	if ((Num_pilot_squad_images == 0) || (Cur_pilot->callsign[0] == '\0')) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	// reset pilot pic number
	Pic_squad_number++;
	if (Pic_squad_number >= Num_pilot_squad_images){
		Pic_squad_number = 0;
	}

	// copy pilot pic filename into pilot struct
	if ((Pic_squad_number >= 0) && (Pic_squad_number < Num_pilot_squad_images)){
		strcpy(Cur_pilot->squad_filename, Pilot_squad_image_names[Pic_squad_number]);
	}

	// play scroll sound
	gamesnd_play_iface(SND_SCROLL);
}

void barracks_delete_pilot()
{
	char buf[MAX_FILENAME_LEN];
	int active = 0;
	int del_rval;

	if (!Num_pilots) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	int popup_rval = popup(PF_TITLE_BIG | PF_TITLE_RED, 2, POPUP_NO, POPUP_YES, XSTR( "Warning!\n\nAre you sure you wish to delete this pilot?", 65));
	if (popup_rval != 1) {
		return;
	}

	if (!stricmp(Pilots[Selected_line], Cur_pilot->callsign)) {
		active = 1;
	}

	strcpy(buf, Pilots[Selected_line]);

	del_rval = delete_pilot_file(buf, (Player_sel_mode == PLAYER_SELECT_MODE_SINGLE) ? 1 : 0);

	if ( !del_rval ) {
		popup(PF_USE_AFFIRMATIVE_ICON | PF_TITLE_BIG | PF_TITLE_RED, 1, POPUP_OK, XSTR("Error\nFailed to delete pilot file. File may be read-only.", -1));
		return;
	} else {
		if (active) {
			Cur_pilot->callsign[0] = 0;
		}
	}

	for (int i=Selected_line; i<Num_pilots-1; i++) {
		strcpy(Pilots[i], Pilots[i + 1]);
		Pilot_ranks[i] = Pilot_ranks[i + 1];
	}

	Num_pilots--;
	if (Selected_line >= Num_pilots) {
		Selected_line = Num_pilots - 1;
	}

	if (active) {
		if (Selected_line >= 0) {
			barracks_new_pilot_selected();
		} else {
			Cur_pilot->callsign[0] = 0;
		}
	}

	gamesnd_play_iface(SND_USER_SELECT);
}

// Filter out pilots of wrong type (which shouldn't be in the directory we are checking, but just to be safe..)
int barracks_pilot_filter(char *filename)
{
	int r, rank;

	r = verify_pilot_file(filename, Player_sel_mode == PLAYER_SELECT_MODE_SINGLE, &rank);
	if (rank >= Rank_pips_count)
		rank = Rank_pips_count - 1;

	if (!r) {
		Pilot_ranks[Num_pilots++] = rank;
	}

	return !r;
}

// callback handler for the squadon selection buttons when they are disabled (in single player)
void barracks_squad_change_popup()
{
	// show a popup
	popup( PF_USE_AFFIRMATIVE_ICON | PF_NO_NETWORKING, 1, POPUP_OK, XSTR("You cannot change your squadron in Single Player mode.", 1445));
}


void barracks_init_player_stuff(int mode)
{
	int i, j;

	// determine if we should be looking for single or multiplayers at the outset
	Player_sel_mode = mode;
	
	// get the list of pilots based upon whether we're in single or multiplayer mode
	Num_pilots = 0;
	Get_file_list_filter = barracks_pilot_filter;

	// single player specific stuff
	if (mode == PLAYER_SELECT_MODE_SINGLE) {
		int old_pilot_num = 0;
		int new_pilot_num = 0;
		char old_pilots_arr[MAX_PILOTS][MAX_FILENAME_LEN];
		char *old_pilots[MAX_PILOTS];

		Game_mode &= ~GM_MULTIPLAYER;
		Game_mode |= GM_NORMAL;

		Num_pilots = cf_get_file_list_preallocated(MAX_PILOTS, Pilots_arr, Pilots, CF_TYPE_SINGLE_PLAYERS, NOX("*.pl2"), CF_SORT_TIME);
		old_pilot_num = cf_get_file_list_preallocated(MAX_PILOTS, old_pilots_arr, old_pilots, CF_TYPE_SINGLE_PLAYERS, NOX("*.plr"), CF_SORT_TIME);

		new_pilot_num = MIN((Num_pilots + old_pilot_num), MAX_PILOTS);

		for (i = Num_pilots; i<new_pilot_num;) {
			for (j = 0; j<old_pilot_num; j++) {
				if ( i >= MAX_PILOTS )
					break;

				strcpy( Pilots[i], old_pilots[j] );
				Num_pilots++;
				i++;
			}
		}

		// disable squad logo switching		
		Buttons[gr_screen.res][B_SQUAD_PREV_BUTTON].button.hide();
		Buttons[gr_screen.res][B_SQUAD_PREV_BUTTON].button.disable();
		Buttons[gr_screen.res][B_SQUAD_PREV_BUTTON].button.set_disabled_action(barracks_squad_change_popup);
		Buttons[gr_screen.res][B_SQUAD_NEXT_BUTTON].button.hide();
		Buttons[gr_screen.res][B_SQUAD_NEXT_BUTTON].button.disable();			
		Buttons[gr_screen.res][B_SQUAD_NEXT_BUTTON].button.set_disabled_action(barracks_squad_change_popup);
	}
	// multiplayer specific stuff
	else {
		Game_mode &= ~GM_NORMAL;
		Game_mode |= GM_MULTIPLAYER;

		Num_pilots = cf_get_file_list_preallocated(MAX_PILOTS, Pilots_arr, Pilots, CF_TYPE_MULTI_PLAYERS, NOX("*.plr"), CF_SORT_TIME);

		// enable squad logo switching
		Buttons[gr_screen.res][B_SQUAD_PREV_BUTTON].button.enable();
		Buttons[gr_screen.res][B_SQUAD_PREV_BUTTON].button.unhide();
		Buttons[gr_screen.res][B_SQUAD_NEXT_BUTTON].button.enable();
		Buttons[gr_screen.res][B_SQUAD_NEXT_BUTTON].button.unhide();			
	}

	int ranks[MAX_PILOTS];

	for (i=0; i<Num_pilots; i++) {
		for (j=0; j<Num_pilots; j++) {
			if (!strcmp(Pilots[i], Pilots_arr[j])) {
				ranks[i] = Pilot_ranks[j];
				break;
			}
		}
		Assert(j < Num_pilots);  // Pilot not found?  How is that possible?
	}

	for (i=0; i<Num_pilots; i++) {
		Pilot_ranks[i] = ranks[i];
	}

	Selected_line = List_scroll_offset = 0;
	barracks_new_pilot_selected();

}

void barracks_button_pressed(int n)
{
	switch (n) {
		case B_PILOT_SCROLL_UP_BUTTON:
			barracks_scroll_callsign_up();
			break;

		case B_PILOT_SCROLL_DOWN_BUTTON:
			barracks_scroll_callsign_down();
			break;

		case B_STATS_SCROLL_UP_BUTTON:
			barracks_scroll_stats_up();
			break;

		case B_STATS_SCROLL_DOWN_BUTTON:
			barracks_scroll_stats_down();
			break;

		case B_PIC_PREV_PILOT_BUTTON:
			barracks_prev_pic();
			break;

		case B_PIC_NEXT_PILOT_BUTTON:
			barracks_next_pic();
			break;

		case B_SQUAD_PREV_BUTTON:
			barracks_prev_squad_pic();
			break;

		case B_SQUAD_NEXT_BUTTON:
			barracks_next_squad_pic();
			break;

		case B_PILOT_SET_ACTIVE_BUTTON:
			if (barracks_new_pilot_selected()){
				gamesnd_play_iface(SND_GENERAL_FAIL);

				// if it's just the missing campaign file that failed for us then don't give the second popup
				if (Campaign_file_missing)
					break;

				// throw up a popup telling the player that he should create a pilot first
				if(Player_sel_mode == PLAYER_SELECT_MODE_SINGLE){
					popup(PF_USE_AFFIRMATIVE_ICON,1,POPUP_OK,XSTR( "You must create a single player pilot.", 66));
				} else {
					popup(PF_USE_AFFIRMATIVE_ICON,1,POPUP_OK,XSTR( "You must create a multi player pilot.", 67));
				}
			} else {
				gamesnd_play_iface(SND_SCROLL);

				if (Campaign_file_missing) {
					popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR( "The currently active campaign cannot be found.  Please select another...", -1));
					gameseq_post_event(GS_EVENT_CAMPAIGN_ROOM);
				}
			}
			break;

		case B_ACCEPT_BUTTON:			
			if (Num_pilots && !barracks_pilot_accepted()) {
				gamesnd_play_iface(SND_COMMIT_PRESSED);

				if (Campaign_file_missing) {
					popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR( "The currently active campaign cannot be found.  Please select another...", -1));
					gameseq_post_event(GS_EVENT_CAMPAIGN_ROOM);
				} else {
					gameseq_post_event(GS_EVENT_MAIN_MENU);
				}
			} else {
				gamesnd_play_iface(SND_GENERAL_FAIL);

				// if it's just the missing campaign file that failed for us then don't give the second popup
				if (Campaign_file_missing)
					break;

				// throw up a popup telling the player that he should create a pilot first
				if(Player_sel_mode == PLAYER_SELECT_MODE_SINGLE){
					popup(PF_USE_AFFIRMATIVE_ICON,1,POPUP_OK,XSTR( "You must create a single player pilot.", 66));
				} else {
					popup(PF_USE_AFFIRMATIVE_ICON,1,POPUP_OK,XSTR( "You must create a multi player pilot.", 67));
				}
			}
			break;

		case B_PILOT_CLONE_BUTTON:
			if (Num_pilots < 1) {
				gamesnd_play_error_beep();
				break;
			}

			Clone_flag = 1;
			barracks_create_new_pilot();
			break;

		case B_PILOT_CONVERT_BUTTON: {
#if defined(DEMO) || defined(OEM_BUILD)
			game_feature_not_in_demo_popup();
#else
			char temp[256], *str;
			char old_pic[256] = "";
			char old_squad_pic[256] = "";
			char old_squad[256] = "";
			int z;

			if (!barracks_new_pilot_selected()) {
				if (Player_sel_mode == PLAYER_SELECT_MODE_SINGLE)
					str = XSTR( "multiplayer", 68);
				else
					str = XSTR( "single player", 69);

				sprintf(temp, XSTR( "This will overwrite your %s pilot.  Proceed?", 70), str);
				if (!verify_pilot_file(Cur_pilot->callsign, Player_sel_mode == PLAYER_SELECT_MODE_MULTI)) {
					z = popup(0, 2, POPUP_CANCEL, POPUP_OK, temp);
					if (z != 1)
						break;
				}

				strcpy(old_pic, Cur_pilot->image_filename);
				strcpy(old_squad_pic, Cur_pilot->squad_filename);
				strcpy(old_squad, Cur_pilot->squad_name);
				init_new_pilot(Cur_pilot, 0);
				strcpy(Cur_pilot->image_filename, old_pic);
				strcpy(Cur_pilot->squad_filename, old_squad_pic);
				strcpy(Cur_pilot->squad_name, old_squad);
				if (Player_sel_mode == PLAYER_SELECT_MODE_SINGLE) {
					Cur_pilot->flags |= PLAYER_FLAGS_IS_MULTI;
					write_pilot_file();
					barracks_init_player_stuff(PLAYER_SELECT_MODE_MULTI);

				} else {
					// make sure we don't carry over the multi flag
					if (Cur_pilot->flags & PLAYER_FLAGS_IS_MULTI)
						Cur_pilot->flags &= ~PLAYER_FLAGS_IS_MULTI;

					write_pilot_file();
					mission_campaign_savefile_save();
					barracks_init_player_stuff(PLAYER_SELECT_MODE_SINGLE);
				}

				gamesnd_play_iface(SND_USER_SELECT);

			} else {
				gamesnd_play_iface(SND_GENERAL_FAIL);
			}
#endif
			break;
		}

		case B_PILOT_CREATE_BOTTON:
			Clone_flag = 0;
			barracks_create_new_pilot();
			break;

		case B_HELP_BUTTON:
			launch_context_help();
			gamesnd_play_iface(SND_HELP_PRESSED);
			break;

		case B_OPTION_BUTTON:
			gamesnd_play_iface(SND_SWITCH_SCREENS);
			gameseq_post_event(GS_EVENT_OPTIONS_MENU);
			break;

		case B_STATS_MEDAL_BUTTON:
#ifdef FS2_DEMO
			game_feature_not_in_demo_popup();
#else
			gamesnd_play_iface(SND_SWITCH_SCREENS);
			gameseq_post_event(GS_EVENT_VIEW_MEDALS);
#endif
			break;

		case B_PILOT_DELETE_BUTTON:
			barracks_delete_pilot();
			break;

		case B_PILOT_SINGLE_MODE_BUTTON:
			if (Player_sel_mode != PLAYER_SELECT_MODE_SINGLE) {
				gamesnd_play_iface(SND_USER_SELECT);
				barracks_init_player_stuff(PLAYER_SELECT_MODE_SINGLE);
			}						
			break;

		case B_PILOT_MULTI_MODE_BUTTON:
#if defined(DEMO) || defined(OEM_BUILD) // not for FS2_DEMO
			game_feature_not_in_demo_popup();
#else
			if ( Networking_disabled ) {
				game_feature_disabled_popup();
				break;
			}

			if (Player_sel_mode != PLAYER_SELECT_MODE_MULTI) {
				gamesnd_play_iface(SND_USER_SELECT);
				barracks_init_player_stuff(PLAYER_SELECT_MODE_MULTI);
			}
#endif
			break;
	}
}


void barracks_display_pilot_callsigns(int prospective_pilot)
{
	int y = 0;
	int cur_pilot_idx = List_scroll_offset;

	int multi = 0;
	if (Player_sel_mode == PLAYER_SELECT_MODE_MULTI) {
		multi = 1;
	}

	int font_height = gr_get_font_height();
	while (y + font_height <= Barracks_list_coords[gr_screen.res][BARRACKS_H_COORD]) {
		if (cur_pilot_idx >= Num_pilots)
			break;

		if (!stricmp(Cur_pilot->callsign, Pilots[cur_pilot_idx]) && (is_pilot_multi(Cur_pilot) == multi)) {
			if ((cur_pilot_idx == Selected_line) || (cur_pilot_idx == prospective_pilot)) {
				gr_set_color_fast(&Color_text_active_hi);
			} else {
				gr_set_color_fast(&Color_text_active);
			}
		} else {
			if (cur_pilot_idx == Selected_line) {
				gr_set_color_fast(&Color_text_selected);
			} else if (cur_pilot_idx == prospective_pilot) {
				gr_set_color_fast(&Color_text_subselected);
			} else {
				gr_set_color_fast(&Color_text_normal);
			}
		}

		gr_printf(Barracks_list_coords[gr_screen.res][BARRACKS_X_COORD], Barracks_list_coords[gr_screen.res][BARRACKS_Y_COORD] + y, Pilots[cur_pilot_idx]);
		gr_set_bitmap(Rank_pips_bitmaps + Pilot_ranks[cur_pilot_idx]);
		gr_bitmap(Barracks_list_coords[gr_screen.res][BARRACKS_X_COORD] - 34, Barracks_list_coords[gr_screen.res][BARRACKS_Y_COORD] + y);
 
		y += font_height;
		cur_pilot_idx++;
	}
}



void barracks_display_pilot_stats()
{
	int y = 0;
	int z = Stats_scroll_offset;
	int font_height = gr_get_font_height();
	char *str;
	int i, w, h;
	while (y + font_height <= Barracks_stats_coords[gr_screen.res][BARRACKS_H_COORD]) {
		if (z >= Num_stat_lines) {
			break;
		}

		str = Stat_labels[z];
		if (*str == '*') {
			gr_set_color_fast(&Color_text_heading);
			str++;

			gr_get_string_size(&w, &h, str);
			i = Barracks_stats_coords[gr_screen.res][BARRACKS_Y_COORD] + y + h / 2 - 1;			
			gr_line(Barracks_stats_coords[gr_screen.res][BARRACKS_X_COORD], i, Barracks_stats_coords[gr_screen.res][BARRACKS_X_COORD] + Barracks_stats_coords[gr_screen.res][BARRACKS_W_COORD] - w - 2, i);
			gr_line(Barracks_stats_coords[gr_screen.res][BARRACKS_X_COORD] + Barracks_stats_coords[gr_screen.res][BARRACKS_W_COORD] + 1, i, Barracks_stats2_coords[gr_screen.res][BARRACKS_X_COORD] + Barracks_stats2_coords[gr_screen.res][BARRACKS_W_COORD], i);

		} else {
			gr_set_color_fast(&Color_text_normal);
		}

		gr_get_string_size(&w, NULL, str);
		gr_printf(Barracks_stats_coords[gr_screen.res][BARRACKS_X_COORD] + Barracks_stats_coords[gr_screen.res][BARRACKS_W_COORD] - w, Barracks_stats_coords[gr_screen.res][BARRACKS_Y_COORD] + y, "%s", str);
		str = Stats[z];
		if (*str) {
			gr_printf(Barracks_stats2_coords[gr_screen.res][BARRACKS_X_COORD], Barracks_stats_coords[gr_screen.res][BARRACKS_Y_COORD] + y, "%s", str);
		}

		y += font_height;
		z++;
	}
}


// process pilot callsign
void barracks_accept_new_pilot_callsign()
{
	char buf[CALLSIGN_LEN + 1];
	char name[MAX_FILENAME_LEN];
	int i;

	int z = 0;
	Inputbox.get_text(buf);
	drop_white_space(buf);

	if (!isalpha(*buf)) {
		z = 1;
	} else {
		for (i=1; buf[i]; i++) {
			if (!isalpha(buf[i]) && !isdigit(buf[i]) && !strchr(VALID_PILOT_CHARS, buf[i])) {
				z = 1;
				return;
			}
		}
	}

	for (i=1; i<Num_pilots; i++) {
		if (!stricmp(buf, Pilots[i])) {
			z = 1;
			if (pilot_verify_overwrite() == 1) {
				strcpy(name, Pilots[Selected_line]);
				for (z=i; z<Num_pilots-1; z++) {
					strcpy(Pilots[z], Pilots[z + 1]);
					Pilot_ranks[z] = Pilot_ranks[z + 1];
				}

				Num_pilots--;
				delete_pilot_file(name, Player_sel_mode == PLAYER_SELECT_MODE_SINGLE ? 1 : 0);
				z = 0;
			}
			return;
		}
	}

	if (!*buf || (i < Num_pilots)) { // duplicate name, alert user
		z = 1;
	}

	if (z) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	strcpy(Pilots[0], buf);
	strcpy(Cur_pilot->callsign, buf);
	init_new_pilot(Cur_pilot, !Clone_flag);
	
	// again, make sure we set his flags correctly to ensure that he gets saved to the proper directory and gets
	// displayed correctly
	if (Player_sel_mode == PLAYER_SELECT_MODE_SINGLE) {
		Cur_pilot->flags &= ~(PLAYER_FLAGS_IS_MULTI);
	} else {
		Cur_pilot->flags |= PLAYER_FLAGS_IS_MULTI;
		Cur_pilot->stats.flags |= STATS_FLAG_MULTIPLAYER;
	}

	if ( !(Game_mode & GM_STANDALONE_SERVER) ) {
		write_pilot_file(Cur_pilot);
	}

	Selected_line = 0;
	barracks_new_pilot_selected();
	barracks_set_callsign_enter_mode(false);
}


// draw pilot image and clean up afterwards
void barracks_draw_pilot_pic()
{
	// draw pilot pic
	if (Cur_pilot->callsign[0] && (Pic_number >= 0) && (Pic_number < Num_pilot_images)) {
		if (Pilot_images[Pic_number] >= 0) {
			// JAS: This code is hacked to allow the animation to use all 256 colors
			extern int Palman_allow_any_color;
			Palman_allow_any_color = 1;
			gr_set_bitmap(Pilot_images[Pic_number]);
			gr_bitmap(Barracks_image_coords[gr_screen.res][BARRACKS_X_COORD], Barracks_image_coords[gr_screen.res][BARRACKS_Y_COORD]);
			Palman_allow_any_color = 0;

			// print number of the current pic
			char buf[40];			
			sprintf(buf, XSTR( "%d of %d", 71), Pic_number + 1, Num_pilot_images);
			gr_printf(Barracks_image_number_coords[gr_screen.res][BARRACKS_X_COORD], Barracks_image_number_coords[gr_screen.res][BARRACKS_Y_COORD], buf);				
		}
	} else {
		Pic_number = -1;
	}
}

// draw squad image and clean up afterwards
void barracks_draw_squad_pic()
{
	char buf[40];

	// draw pilot pic
	if (Cur_pilot->callsign[0] && (Pic_squad_number >= 0) && (Pic_squad_number < Num_pilot_squad_images)) {
		if (Pilot_squad_images[Pic_squad_number] >= 0) {
			// JAS: This code is hacked to allow the animation to use all 256 colors
			extern int Palman_allow_any_color;
			Palman_allow_any_color = 1;
			gr_set_bitmap(Pilot_squad_images[Pic_squad_number]);
			gr_bitmap(Barracks_squad_coords[gr_screen.res][BARRACKS_X_COORD], Barracks_squad_coords[gr_screen.res][BARRACKS_Y_COORD]);
			Palman_allow_any_color = 0;

			// print number of current squad pic
			if(Player_sel_mode != PLAYER_SELECT_MODE_SINGLE){
				sprintf(buf,XSTR( "%d of %d", 71), Pic_squad_number+1, Num_pilot_squad_images);
				gr_printf(Barracks_squad_number_coords[gr_screen.res][BARRACKS_X_COORD], Barracks_squad_number_coords[gr_screen.res][BARRACKS_Y_COORD], buf);
			}
		}
	} else {
		Pic_squad_number = -1;
	}	
}

// -----------------------------------------------------------------------------
void barracks_init()
{
	//Set these to null, 'cause they aren't allocated yet.
	Stat_labels = NULL;
	Stats = NULL;

	UI_WINDOW *w = &Ui_window;

	// save current pilot file, so we don't possibly loose it.
	write_pilot_file();
	mission_campaign_savefile_save();

	// create interface
	Ui_window.create(0, 0, gr_screen.max_w_unscaled, gr_screen.max_h_unscaled, 0);
	Ui_window.set_mask_bmap(Barracks_bitmap_mask_fname[gr_screen.res]);

	// load background bitmap
	Background_bitmap = bm_load(Barracks_bitmap_fname[gr_screen.res]);
	if(Background_bitmap < 0){
		// we failed to load the bitmap - this is very bad
		Int3();
	}

	// create buttons
	int i;
	for (i=0; i<BARRACKS_NUM_BUTTONS; i++) {
		// create the object
		Buttons[gr_screen.res][i].button.create(&Ui_window, "", Buttons[gr_screen.res][i].x, Buttons[gr_screen.res][i].y, 60, 30, Buttons[gr_screen.res][i].repeat, 1);

		// set the sound to play when highlighted
		Buttons[gr_screen.res][i].button.set_highlight_action(common_play_highlight_sound);

		// set the ani for the button
		Buttons[gr_screen.res][i].button.set_bmaps(Buttons[gr_screen.res][i].filename);

		// set the hotspot
		Buttons[gr_screen.res][i].button.link_hotspot(Buttons[gr_screen.res][i].hotspot);
	}

	// add all strings	
	w->add_XSTR("Create", 1034, Buttons[gr_screen.res][0].text_x,  Buttons[gr_screen.res][0].text_y, &Buttons[gr_screen.res][0].button, UI_XSTR_COLOR_GREEN);
	w->add_XSTR("Accept", 1035, Buttons[gr_screen.res][5].text_x,  Buttons[gr_screen.res][5].text_y, &Buttons[gr_screen.res][5].button, UI_XSTR_COLOR_PINK);
	w->add_XSTR("Help",   928, Buttons[gr_screen.res][6].text_x,  Buttons[gr_screen.res][6].text_y, &Buttons[gr_screen.res][6].button, UI_XSTR_COLOR_GREEN);
	w->add_XSTR("Options",1036, Buttons[gr_screen.res][7].text_x,  Buttons[gr_screen.res][7].text_y, &Buttons[gr_screen.res][7].button, UI_XSTR_COLOR_GREEN);
	w->add_XSTR("Medals", 1037, Buttons[gr_screen.res][8].text_x,  Buttons[gr_screen.res][8].text_y, &Buttons[gr_screen.res][8].button, UI_XSTR_COLOR_GREEN);
	w->add_XSTR("Remove", 1038, Buttons[gr_screen.res][11].text_x,  Buttons[gr_screen.res][11].text_y, &Buttons[gr_screen.res][11].button, UI_XSTR_COLOR_GREEN);
	w->add_XSTR("Select", 1552, Buttons[gr_screen.res][12].text_x,  Buttons[gr_screen.res][12].text_y, &Buttons[gr_screen.res][12].button, UI_XSTR_COLOR_GREEN);
	w->add_XSTR("Clone",  1040, Buttons[gr_screen.res][13].text_x,  Buttons[gr_screen.res][13].text_y, &Buttons[gr_screen.res][13].button, UI_XSTR_COLOR_GREEN);
	w->add_XSTR("Single", 1041, Buttons[gr_screen.res][14].text_x,  Buttons[gr_screen.res][14].text_y,  &Buttons[gr_screen.res][14].button, UI_XSTR_COLOR_GREEN);
	w->add_XSTR("Multi",  1042, Buttons[gr_screen.res][15].text_x,  Buttons[gr_screen.res][15].text_y,  &Buttons[gr_screen.res][15].button, UI_XSTR_COLOR_GREEN);
	w->add_XSTR("Convert",1043, Buttons[gr_screen.res][16].text_x,  Buttons[gr_screen.res][16].text_y, &Buttons[gr_screen.res][16].button, UI_XSTR_COLOR_GREEN);	
	for(i=0; i<BARRACKS_NUM_TEXT; i++) {
		w->add_XSTR(&Barracks_text[gr_screen.res][i]);
	}

	// button for selecting pilot
	List_region.create(&Ui_window, "", Barracks_list_coords[gr_screen.res][BARRACKS_X_COORD], Barracks_list_coords[gr_screen.res][BARRACKS_Y_COORD], Barracks_list_coords[gr_screen.res][BARRACKS_W_COORD], Barracks_list_coords[gr_screen.res][BARRACKS_H_COORD], 0, 1);
	List_region.hide();

	// create input box (for new pilot)
	Inputbox.create(&Ui_window, Barracks_list_coords[gr_screen.res][BARRACKS_X_COORD], Barracks_list_coords[gr_screen.res][BARRACKS_Y_COORD], Barracks_list_coords[gr_screen.res][BARRACKS_W_COORD], CALLSIGN_LEN - 1, "", UI_INPUTBOX_FLAG_INVIS | UI_INPUTBOX_FLAG_KEYTHRU | UI_INPUTBOX_FLAG_LETTER_FIRST);
	Inputbox.set_valid_chars(VALID_PILOT_CHARS);
	Inputbox.disable();
	Inputbox.hide();

	// load in help overlay bitmap	
	help_overlay_load(BARRACKS_OVERLAY);
	help_overlay_set_state(BARRACKS_OVERLAY,0);	

	// other init stuff
	Barracks_callsign_enter_mode = 0;	
	List_scroll_offset = Stats_scroll_offset = Pic_number = Pic_squad_number = Selected_line = 0;
	Cur_pilot = &Players[Player_num];

	// disable squad logo selection buttons in single player
	if(!(Cur_pilot->flags & PLAYER_FLAGS_IS_MULTI)){
		// squad logo picture buttons		
		Buttons[gr_screen.res][B_SQUAD_PREV_BUTTON].button.hide();
		Buttons[gr_screen.res][B_SQUAD_PREV_BUTTON].button.disable();
		Buttons[gr_screen.res][B_SQUAD_NEXT_BUTTON].button.hide();
		Buttons[gr_screen.res][B_SQUAD_NEXT_BUTTON].button.disable();
	} else {
		// squad logo picture buttons		
		Buttons[gr_screen.res][B_SQUAD_PREV_BUTTON].button.enable();
		Buttons[gr_screen.res][B_SQUAD_PREV_BUTTON].button.unhide();
		Buttons[gr_screen.res][B_SQUAD_NEXT_BUTTON].button.enable();
		Buttons[gr_screen.res][B_SQUAD_NEXT_BUTTON].button.unhide();		
	}

	// set up hotkeys for buttons so we draw the correct animation frame when a key is pressed
	barracks_set_hotkeys(1);

	// load ramp pips
	Rank_pips_bitmaps = bm_load_animation("IconRankMini.ani", &Rank_pips_count);	

	// load up the pilot pic list
	pilot_load_pic_list();	
	pilot_load_squad_pic_list();
	
	// don't load pilot images yet
	for (i=0; i<MAX_PILOT_IMAGES; i++) {
		Pilot_images[i] = BARRACKS_IMAGE_NOT_LOADED;  // while -1 is can't load
		Pilot_squad_images[i] = BARRACKS_IMAGE_NOT_LOADED;
	}
	
	// init stats
	barracks_init_stats(&Cur_pilot->stats);

	// disable some buttons for the multiplayer beta and e3 build
#if defined(MULTIPLAYER_BETA_BUILD) || defined(E3_BUILD) || defined(PRESS_TOUR_BUILD)
	Buttons[gr_screen.res][B_PILOT_CLONE_BUTTON].button.hide();
	Buttons[gr_screen.res][B_PILOT_CONVERT_BUTTON].button.hide();	
	Buttons[gr_screen.res][B_PILOT_CLONE_BUTTON].button.disable();	
	Buttons[gr_screen.res][B_PILOT_CONVERT_BUTTON].button.disable();	
#endif

	// multiplayer beta build
#ifdef MULTIPLAYER_BETA_BUILD
	Buttons[gr_screen.res][B_PILOT_SINGLE_MODE_BUTTON].button.hide();
	Buttons[gr_screen.res][B_PILOT_SINGLE_MODE_BUTTON].button.disable();
#endif

	// e3 build
#if defined(E3_BUILD) || defined(PRESS_TOUR_BUILD)
	Buttons[gr_screen.res][B_PILOT_MULTI_MODE_BUTTON].button.hide();
	Buttons[gr_screen.res][B_PILOT_MULTI_MODE_BUTTON].button.disable();
#endif
	
	// base the mode we're in (single or multi) on the status of the currently selected pilot
#ifdef MULTIPLAYER_BETA_BUILD
	barracks_init_player_stuff(1);
#elif defined(E3_BUILD) || defined(PRESS_TOUR_BUILD)
	barracks_init_player_stuff(0);
#else
	barracks_init_player_stuff(is_pilot_multi(Player));	
#endif
}

// -----------------------------------------------------------------------------
void barracks_do_frame(float frametime)
{
	int k = Ui_window.process();

	if ( k > 0 ) {
		if ( help_overlay_active(BARRACKS_OVERLAY) ) {
			help_overlay_set_state(BARRACKS_OVERLAY,0);
			k = 0;
		}
	}

	// pilot that mouse is over
	int prospective_pilot = -1;
	int i;

	// Entering pilot callsign
	if (Barracks_callsign_enter_mode) {
		// set focus to inputbox
		Inputbox.set_focus();

		switch (k) {
			case KEY_ESC:
				// cancel create pilot
				Num_pilots--;
				for (i=0; i<Num_pilots; i++) {
					strcpy(Pilots[i], Pilots[i + 1]);
					Pilot_ranks[i] = Pilot_ranks[i + 1];
				}

				barracks_set_callsign_enter_mode(false);
				break;

			case KEY_ENTER: 
				barracks_accept_new_pilot_callsign();
				break;
		} 
	} else {
		// not entering pilot callsign
		switch (k) {
			case KEY_ENTER:
				if (barracks_new_pilot_selected()) {
					gamesnd_play_iface(SND_GENERAL_FAIL);
				} else {
					gamesnd_play_iface(SND_USER_SELECT);
				}
				break;

			case KEY_ESC:  // cancel
				if (!help_overlay_active(BARRACKS_OVERLAY)) {
					if (Num_pilots && !barracks_pilot_accepted()) {
						gameseq_post_event(GS_EVENT_MAIN_MENU);
					} else {
						gamesnd_play_iface(SND_GENERAL_FAIL);
					}
				} else {
					// kill the overlay
					help_overlay_set_state(BARRACKS_OVERLAY,0);
				}
				break;

			case KEY_TAB:  // switch mode (simgle/multi)
#if defined(DEMO) || defined(OEM_BUILD) // not for FS2_DEMO
				game_feature_not_in_demo_popup();
#else
				if ( Networking_disabled ) {
					game_feature_disabled_popup();
					break;
				}

				if (Player_sel_mode == PLAYER_SELECT_MODE_SINGLE) {
					barracks_init_player_stuff(PLAYER_SELECT_MODE_MULTI);
				} else {
					barracks_init_player_stuff(PLAYER_SELECT_MODE_SINGLE);
				}

				gamesnd_play_iface(SND_USER_SELECT);
#endif
				break;

			case KEY_F1:  // show help overlay
				gamesnd_play_iface(SND_HELP_PRESSED);
				break;

			case KEY_F2:  // goto options screen
				gamesnd_play_iface(SND_SWITCH_SCREENS);
				gameseq_post_event(GS_EVENT_OPTIONS_MENU);
				break;
		}	// end switch

		// process buttons
		for (i=0; i<BARRACKS_NUM_BUTTONS; i++) {
			if (Buttons[gr_screen.res][i].button.pressed()) {
				barracks_button_pressed(i);
			}
		}

		// if mouse is over a pilot, find index into Pilots array
		if (List_region.is_mouse_on()) {
			int y;
			List_region.get_mouse_pos(NULL, &y);
			int pilot_index = List_scroll_offset + (y / gr_get_font_height());
			if ((pilot_index >= 0) && (pilot_index < Num_pilots)) {
				prospective_pilot = pilot_index;
			}
		}
		
		// if mouse clicked in list region, find index into Pilots array 
		if (List_region.pressed()) {
			if (prospective_pilot != -1) {
				Selected_line = prospective_pilot;
				gamesnd_play_iface(SND_USER_SELECT);
			}
		}
	}

	// check mouse over help
	if (mouse_down(MOUSE_LEFT_BUTTON)) {
		help_overlay_set_state(BARRACKS_OVERLAY, 0);
	}

	// do pilot pic stuff
	if ((Pic_number >= 0) && (Pic_number < Num_pilot_images)) {
		if (Pilot_images[Pic_number] == BARRACKS_IMAGE_NOT_LOADED) {  // haven't tried loading it yet
			Pilot_images[Pic_number] = bm_load(Pilot_image_names[Pic_number]);
			if (Pilot_images[Pic_number] >= 0) {
				int w, h;

				bm_get_info(Pilot_images[Pic_number], &w, &h, NULL);
				// check for invalid pilot pic file
				if ((w != PLAYER_PILOT_PIC_W) || (h != PLAYER_PILOT_PIC_H)) {
					bm_release(Pilot_images[Pic_number]);
					Pilot_images[Pic_number] = -1;
				}
			}
		}		
	} else {
		Pic_number = -1;
	}

	// do squad pic stuff
	if ((Pic_squad_number >= 0) && (Pic_squad_number < Num_pilot_squad_images)) {
		if (Pilot_squad_images[Pic_squad_number] == BARRACKS_IMAGE_NOT_LOADED) {  // haven't tried loading it yet
			Pilot_squad_images[Pic_squad_number] = bm_load_duplicate(Pilot_squad_image_names[Pic_squad_number]);
			if (Pilot_squad_images[Pic_squad_number] >= 0) {
				int w, h;

				bm_get_info(Pilot_squad_images[Pic_squad_number], &w, &h, NULL);
				// check for invalid pilot pic file
				if ((w != PLAYER_SQUAD_PIC_W) || (h != PLAYER_SQUAD_PIC_H)) {
					bm_release(Pilot_squad_images[Pic_squad_number]);
					Pilot_squad_images[Pic_squad_number] = -1;
				}
			}
		}
	} else {
		Pic_squad_number = -1;
	}

	// draw the background, etc
	gr_reset_clip();	
	GR_MAYBE_CLEAR_RES(Background_bitmap);
	if (Background_bitmap >= 0) {
		gr_set_bitmap(Background_bitmap);
		gr_bitmap(0, 0);	
	}		

	// draw pilot image and clean up afterwards
	barracks_draw_pilot_pic();
	barracks_draw_squad_pic();

	// draw the window	
	Ui_window.draw();	

	// light up the correct mode button (single or multi)	
	if (Player_sel_mode == PLAYER_SELECT_MODE_SINGLE) {
		Buttons[gr_screen.res][B_PILOT_SINGLE_MODE_BUTTON].button.draw_forced(2);
	} else {
		Buttons[gr_screen.res][B_PILOT_MULTI_MODE_BUTTON].button.draw_forced(2);
	}	

	// write out pilot call signs
	barracks_display_pilot_callsigns(prospective_pilot);

	// write out current pilot stats
	barracks_display_pilot_stats();

	// blit help overlay if active
	help_overlay_maybe_blit(BARRACKS_OVERLAY);	
	
	// flip the page
	gr_flip();
}

// -----------------------------------------------------------------------------
void barracks_close()
{
	// destroy window
	Ui_window.destroy();

	// release background bitmap
	if (Background_bitmap >= 0) {
		bm_release(Background_bitmap);
	}	

	// release rank pip bitmaps
	bm_release(Rank_pips_bitmaps);	

	// release pilot pic bitmaps
	for (int i=0; i<Num_pilot_images; i++) {
		if (Pilot_images[i] >= 0) {
			bm_release(Pilot_images[i]);
		}
	}

	// unload the overlay bitmap
	help_overlay_unload(BARRACKS_OVERLAY);

	if(Stat_labels != NULL)
	{
		vm_free(Stat_labels);
		Stat_labels = NULL;
	}
	if(Stats != NULL)
	{
		vm_free(Stats);
		Stats = NULL;
	}

	game_flush();
}
