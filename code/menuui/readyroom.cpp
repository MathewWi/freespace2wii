/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/MenuUI/ReadyRoom.cpp $
 * $Revision: 2.23.2.6 $
 * $Date: 2007-11-20 01:11:10 $
 * $Author: Goober5000 $
 *
 * Ready Room code, which is the UI screen for selecting Campaign/mission to play next mainly.
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.23.2.5  2007/02/11 09:09:55  taylor
 * fix simroom crash bug from Goober's rencent change
 * add NULL handling to make sure we don't crap out on sorting
 *
 * Revision 2.23.2.4  2006/10/01 19:27:28  taylor
 * fix for the "branch" bug (still a minor usability issue however, see Mantis bug for details)
 * add a popup to the loopbrief screen when you press ESC, so that we can either accept or decline the loop offer
 *
 * Revision 2.23.2.3  2006/08/28 17:14:52  taylor
 * stupid, stupid, stupid...
 *  - fix AVI/MPG movie playback
 *  - fix missing campaign craziness
 *
 * Revision 2.23.2.2  2006/08/27 18:11:37  taylor
 * quite a few fixes to handle missing campaigns better
 * change load order for campaign loading to a full check: Player-specified -> BUILTIN_CAMPAIGN -> First Avaiable.
 * clean up the getting of a list of available campaigns
 * fix simroom issue where single missions, with the [V] icon, would display wrong (this was a retail bug, but it doesn't show normally)
 * fix bug where, if a campaign failed to load, it would still appear available for savefile useage
 * fix bug where, when resetting the campaign info, the num_missions var wasn't 0'd and it could cause a sexp Assert() during reset
 *
 * Revision 2.23.2.1  2006/07/08 18:09:41  taylor
 * remove -allslev
 * make CTRL-SHIFT-S hotkey work in mission simulator (it's a toggle, so you can turn it on or off while on the screen)
 * fix a bug where blank lines would show up in the campaign list for branch/alternate type missions that are otherwise marked as not completed
 *
 * Revision 2.23  2006/04/14 18:44:16  taylor
 * remove all of the *_ex() parsing functions added for use by EFFs
 * add a pause/unpause for parsing so that we can safely start parsing something new then continue parsing something old
 * make Mission_text and Mission_text_raw only use the memory needed, and free it when it doesn't need to parse anymore
 *   (should work ok with FRED2, but I wasn't able to test it)
 *
 * Revision 2.22  2006/01/13 08:27:43  taylor
 * swap order of pilot read and reset-to-defaults so that the various entries in the pilot file don't get clobbered needlessly
 *
 * Revision 2.21  2005/10/30 19:22:31  wmcoolmon
 * Display a special dialog if the player tries to continue a campaign with no missions
 *
 * Revision 2.20  2005/09/16 02:51:54  taylor
 * when changing a campaign in the campaign room make sure that the mainhall is properly set without having to play a mission first
 *
 * Revision 2.19  2005/07/18 03:45:07  taylor
 * more non-standard res fixing
 *  - I think everything should default to resize now (much easier than having to figure that crap out)
 *  - new mouse_get_pos_unscaled() function to return 1024x768/640x480 relative values so we don't have to do it later
 *  - lots of little cleanups which fix several strange offset/size problems
 *  - fix gr_resize/unsize_screen_pos() so that it won't wrap on int (took too long to track this down)
 *
 * Revision 2.18  2005/07/02 19:43:54  taylor
 * ton of non-standard resolution fixes
 *
 * Revision 2.17  2005/05/12 17:49:13  taylor
 * use vm_malloc(), vm_free(), vm_realloc(), vm_strdup() rather than system named macros
 *   fixes various problems and is past time to make the switch
 *
 * Revision 2.16  2005/04/25 06:36:01  taylor
 * use full size of Campaign_missions[] for simroom filtering, fixes the problem of campaign
 *   missions still showing up in the single list even after filtering
 *
 * Revision 2.15  2005/03/27 06:12:38  taylor
 * some pilot file fixing when going between multi and single (partial fix)
 *
 * Revision 2.14  2005/02/23 04:51:56  taylor
 * some bm_unload() -> bm_release() changes to save bmpman slots
 *
 * Revision 2.13  2005/02/18 20:50:40  wmcoolmon
 * Fixed line misalignment at nonstandard res in the campaign room
 *
 * Revision 2.12  2004/10/31 21:53:24  taylor
 * new pilot code support, no-multiplayer and compiler warning fixes, center mouse cursor for redalert missions
 *
 * Revision 2.11  2004/07/26 20:47:37  Kazan
 * remove MCD complete
 *
 * Revision 2.10  2004/07/12 16:32:53  Kazan
 * MCD - define _MCD_CHECK to use memory tracking
 *
 * Revision 2.9  2004/04/10 16:55:45  Goober5000
 * quashed the slider bug!
 * --Goober5000
 *
 * Revision 2.8  2004/03/05 09:01:53  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.7  2003/09/05 04:25:28  Goober5000
 * well, let's see here...
 *
 * * persistent variables
 * * rotating gun barrels
 * * positive/negative numbers fixed
 * * sexps to trigger whether the player is controlled by AI
 * * sexp for force a subspace jump
 *
 * I think that's it :)
 * --Goober5000
 *
 * Revision 2.6  2003/04/06 03:56:54  Goober5000
 * fixed some scrolling issues; added scroll bar to mission select
 * --Goober5000
 *
 * Revision 2.5  2003/04/05 11:09:22  Goober5000
 * fixed some fiddly bits with scrolling and ui stuff
 * --Goober5000
 *
 * Revision 2.4  2003/03/18 10:07:03  unknownplayer
 * The big DX/main line merge. This has been uploaded to the main CVS since I can't manage to get it to upload to the DX branch. Apologies to all who may be affected adversely, but I'll work to debug it as fast as I can.
 *
 * Revision 2.3  2003/03/03 04:28:37  Goober5000
 * fixed the tech room bug!  yay!
 * --Goober5000
 *
 * Revision 2.2  2002/10/19 03:50:29  randomtiger
 * Added special pause mode for easier action screenshots.
 * Added new command line parameter for accessing all single missions in tech room. - RT
 *
 * Revision 2.1.2.2  2002/11/04 21:25:00  randomtiger
 *
 * When running in D3D all ani's are memory mapped for speed, this takes up more memory but stops gametime locking of textures which D3D8 hates.
 * Added new command line tag Cmdline_d3dlowmem for people who dont want to make use of this because they have no memory.
 * Cleaned up some more texture stuff enabled console debug for D3D.
 *
 * Revision 2.1.2.1  2002/09/24 18:56:43  randomtiger
 * DX8 branch commit
 *
 * This is the scub of UP's previous code with the more up to date RT code.
 * For full details check previous dev e-mails
 *
 * Revision 2.1  2002/08/01 01:41:06  penguin
 * The big include file move
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
 * 20    9/30/99 5:59p Jefff
 * fixed compile error in OEM ver
 * 
 * 19    9/08/99 12:28p Jefff
 * 
 * 18    9/06/99 6:38p Dave
 * Improved CD detection code.
 * 
 * 17    9/02/99 2:33p Jefff
 * column heading coordinate changes
 * 
 * 16    8/22/99 4:16p Jefff
 * scroll button coord fixes
 * 
 * 15    7/20/99 1:49p Dave
 * Peter Drake build. Fixed some release build warnings.
 * 
 * 14    7/19/99 2:13p Dave
 * Added some new strings for Heiko.
 * 
 * 13    7/15/99 9:20a Andsager
 * FS2_DEMO initial checkin
 * 
 * 12    7/09/99 5:54p Dave
 * Seperated cruiser types into individual types. Added tons of new
 * briefing icons. Campaign screen.
 * 
 * 11    2/01/99 5:55p Dave
 * Removed the idea of explicit bitmaps for buttons. Fixed text
 * highlighting for disabled gadgets.
 * 
 * 10    1/30/99 9:01p Dave
 * Coord fixes.
 * 
 * 9     1/30/99 5:08p Dave
 * More new hi-res stuff.Support for nice D3D textures.
 * 
 * 8     1/29/99 12:47a Dave
 * Put in sounds for beam weapon. A bunch of interface screens (tech
 * database stuff).
 * 
 * 7     12/11/98 4:35p Andsager
 * Fix sim room bug when no standalone missions
 * 
 * 6     12/07/98 5:02p Dan
 * Removed improper use of bitmap filename with extension,
 * 
 * 5     10/13/98 9:28a Dave
 * Started neatening up freespace.h. Many variables renamed and
 * reorganized. Added AlphaColors.[h,cpp]
 *    
 *
 * $NoKeywords: $
 */

#include <ctype.h>

#include "menuui/readyroom.h"
#include "graphics/font.h"
#include "ui/ui.h"
#include "ui/uidefs.h"
#include "io/key.h"
#include "gamesequence/gamesequence.h"
#include "mission/missioncampaign.h"
#include "gamesnd/gamesnd.h"
#include "missionui/missionscreencommon.h"
#include "freespace2/freespace.h"
#include "playerman/player.h"
#include "playerman/managepilot.h"
#include "popup/popup.h"
#include "gamehelp/contexthelp.h"
#include "globalincs/alphacolors.h"
#include "menuui/techmenu.h"	// for tech menu reset stuff
#include "cfile/cfile.h"
#include "parse/parselo.h"
#include "menuui/mainhallmenu.h"



#define MAX_MISSIONS	1024

int Mission_list_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		33, 108, 402, 279
	},
	{ // GR_1024
		43, 175, 402, 459
	}
};

int Campaign_list_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		491, 108, 115, 279
	},
	{ // GR_1024
		491, 175, 115, 459
	}
};


#define C_SUBTEXT_X	19
// x coordinate offsets for data when campaign tab active
#define C_TEXT_X		0

// x coordinate offsets for data when mission tab active
#define M_TEXT_X		C_SUBTEXT_X //0

#define MODE_CAMPAIGNS	0
#define MODE_MISSIONS	1

#define MAX_LINES					200
#define MAX_DESC_LINES			200
#define NUM_BUTTONS				11
#define LIST_BUTTONS_MAX		42

#define SCROLL_UP_BUTTON		0
#define SCROLL_DOWN_BUTTON		1
#define MISSION_TAB				2
#define CAMPAIGN_TAB				3
#define HELP_BUTTON				4
#define COMMIT_BUTTON			5
#define OPTIONS_BUTTON			6
#define TECH_DATABASE_BUTTON	7
#define SIMULATOR_BUTTON		8
#define CUTSCENES_BUTTON		9
#define CREDITS_BUTTON			10

#define X_COORD		0
#define Y_COORD		1
#define W_COORD		2
#define H_COORD		3

#define CAMPAIGN_MISSION_HASH_SIZE 307

struct sim_room_buttons {
	char *filename;
	int x, y, xt, yt;
	int hotspot;
	UI_BUTTON button;  // because we have a class inside this struct, we need the constructor below..

	sim_room_buttons(char *name, int x1, int y1, int xt1, int yt1, int h) : filename(name), x(x1), y(y1), xt(xt1), yt(yt1), hotspot(h) {}
};

static sim_room_buttons Buttons[GR_NUM_RESOLUTIONS][NUM_BUTTONS] = {
//XSTR:OFF
	{		// GR_640
		sim_room_buttons("LMB_04",		1,		99,	-1,	-1,	4),
		sim_room_buttons("LMB_05",		1,		381,	-1,	-1,	5),
		sim_room_buttons("LMB_06",		6,		438,	40,	445,	6),
		sim_room_buttons("LMB_07",		6,		457,	40,	462,	7),
		sim_room_buttons("LMB_08",		534,	426,	500,	440,	8),
		sim_room_buttons("LMB_09",		571,	426,	572,	413,	9),
		sim_room_buttons("LMB_10",		534,	455,	480,	462,	10),

		sim_room_buttons("TDB_00",		7,		3,		37,	7,		0),
		sim_room_buttons("TDB_01",		7,		18,	37,	23,	1),
		sim_room_buttons("TDB_02",		7,		34,	37,	38,	2),
		sim_room_buttons("TDB_03",		7,		49,	37,	54,	3),
	},
	{		// GR_1024
		sim_room_buttons("2_LMB_04",	2,		159,	-1,	-1,	4),
		sim_room_buttons("2_LMB_05",	2,		609,	-1,	-1,	5),
		sim_room_buttons("2_LMB_06",	10,	701,	64,	712,	6),
		sim_room_buttons("2_LMB_07",	10,	732,	64,	739,	7),
		sim_room_buttons("2_LMB_08",	854,	681,	800, 704,	8),
		sim_room_buttons("2_LMB_09",	914,	681,	915, 660,	9),
		sim_room_buttons("2_LMB_10",	854,	728,	800, 728,	10),

		sim_room_buttons("2_TDB_00",	12,	5,		59,	12,	0),
		sim_room_buttons("2_TDB_01",	12,	31,	59,	37,	1),
		sim_room_buttons("2_TDB_02",	12,	56,	59,	62,	2),
		sim_room_buttons("2_TDB_03",	12,	81,	59,	88,	3),
	}
//XSTR:ON
};

char *Sim_filename[GR_NUM_RESOLUTIONS] = {
	"LoadMission",
	"2_LoadMission"
};
char *Sim_mask_filename[GR_NUM_RESOLUTIONS] = {
	"LoadMission-m",
	"2_LoadMission-m"
};

char *Campaign_filename[GR_NUM_RESOLUTIONS] = {
	"Campaign",
	"2_Campaign"
};
char *Campaign_mask_filename[GR_NUM_RESOLUTIONS] = {
	"Campaign-m",
	"2_Campaign-m"
};

char *Sim_room_slider_filename[GR_NUM_RESOLUTIONS] = {
	"slider",
	"2_slider"
};

// misc text. ("Mission" and "Filename"
#define NUM_SIM_MISC_TEXT				2
#define SIM_MISC_TEXT_MISSION			0
#define SIM_MISC_TEXT_FILENAME		1
int Sim_misc_text_coords[GR_NUM_RESOLUTIONS][NUM_SIM_MISC_TEXT][2] = {
	{ // GR_640
		{33, 95},
		{491, 95}
	}, 
	{ // GR_1024
		{43, 155},
		{491, 155}
	}
};

int Sim_room_slider_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		4, 131, 20, 245
	},
	{ // GR_1024
		5, 209, 32, 392
	}
};

// readyroom text line stuff
#define READYROOM_LINE_CAMPAIGN	1
#define READYROOM_LINE_CMISSION	2
#define READYROOM_LINE_MISSION	3

#define READYROOM_FLAG_FROM_VOLITION			(1<<0)			// volition made
static struct {	
	int type;					// see READYROOM_LINE_* defines above
	char *name;
	char *filename;
	int x;						// X coordinate of line
	int y;						// Y coordinate of line
	int flags;					// special flags, see READYROOM_FLAG_* defines above
} sim_room_lines[MAX_LINES];

static char Cur_campaign[MAX_FILENAME_LEN];
static char *Mission_filenames[MAX_MISSIONS] = { NULL };
static char *Standalone_mission_names[MAX_MISSIONS] = { NULL };
static int  Standalone_mission_flags[MAX_MISSIONS];
static char *Campaign_missions[MAX_MISSIONS] = { NULL };
static char *Campaign_mission_names[MAX_CAMPAIGN_MISSIONS] = { NULL };
static int Campaign_mission_flags[MAX_MISSIONS];
static int Simroom_show_all = 0;
static int Standalone_mission_names_inited = 0;
static int Campaign_mission_names_inited = 0;
static int Num_standalone_missions;
static int Num_campaign_missions;
//static int Num_player_missions;
static int Scroll_offset;
static int Selected_line;
static int Num_lines;
static int Num_campaign_missions_with_info = 0;
static int Num_standalone_missions_with_info = 0;
static int list_x1;
static int list_x2;
static int list_y;
static int list_w1;
static int list_w2;
static int list_h;
static int Background_bitmap;
static int Old_main_hall = 0;
static UI_WINDOW Ui_window;
static UI_BUTTON List_buttons[LIST_BUTTONS_MAX];  // buttons for each line of text in list

// globals
bool Campaign_room_no_campaigns = false;

// slider stuff
static UI_SLIDER2 Sim_room_slider;

typedef struct hash_node {
	hash_node *next;
	char *filename;
} hash_node;

static hash_node *Campaign_mission_hash_table[CAMPAIGN_MISSION_HASH_SIZE];
static int Hash_table_inited;

// special icons (1.04 + stuff)
#define NUM_MISSION_ICONS			1
#define MISSION_ICON_VOLITION		0				// mini volition death's head :)

// icon offsets (see LIST_ defines above
//#define MISSION_ICON_VOLITION_X				(46)
#define MISSION_ICON_VOLITION_Y_OFFSET		(-1)

// icon offsets
static int Sim_volition_icon_x[GR_NUM_RESOLUTIONS] = {
	38,
	49
};

// special icons themselves
int Mission_icon_bitmaps[NUM_MISSION_ICONS];
//XSTR:OFF
char *Mission_icon_bitmap_filenames[NUM_MISSION_ICONS] = {
	"icon-volition"	
};
//XSTR:ON
void sim_room_load_mission_icons();
void sim_room_unload_mission_icons();
void sim_room_blit_icons(int line_index, int y_start, fs_builtin_mission *fb = NULL, int is_md = 0);

// Finds a hash value for mission filename
//
// returns hash value
int hash_filename(char *filename) {
	ulonglong hash_val = 0;
	char *ptr = filename;
	
	// Dont hash .fsm extension, convert all to upper case
	for (int i=0; i < ((signed int)(strlen(filename)) - 4); i++) {
		hash_val = (hash_val << 4) + toupper(*ptr++);
	}

	return int(hash_val % CAMPAIGN_MISSION_HASH_SIZE);
}

// insert filename into Campaign_mission_hash_table
//
// returns 1 if successful, 0 if could not allocate memory
int hash_insert(char *filename) {
	int hash_val = hash_filename(filename);
	hash_node *cur_node;

	// Check if table empty
	if (Campaign_mission_hash_table[hash_val] == NULL) {
		Campaign_mission_hash_table[hash_val] = new (vm_malloc(sizeof(hash_node))) hash_node;

		cur_node = Campaign_mission_hash_table[hash_val];

		if (cur_node == NULL) {
			// Unable to allocate memory
			return 0;
		}
	} else {
		// Walk down list to first empty node
		cur_node = Campaign_mission_hash_table[hash_val];
		while (cur_node->next != NULL) {
			cur_node = cur_node->next;
		}

		// Create new node
		cur_node->next = new (vm_malloc(sizeof(hash_node))) hash_node;

		if (cur_node->next == NULL) {
			// unable to allocate memory
			return 0;
		} else {
			cur_node = cur_node->next;
		}
	}

	// Initialize data
	cur_node->next = NULL;
	cur_node->filename = filename;

	// Return successs
	return 1;
}

// Checks if a filename already exitst in the hash table
//
// returns 1 if found (collision), 0 if no collision
int campaign_mission_hash_collision(char *filename)
{
	int hash_val = hash_filename(filename);
	hash_node *cur_node = Campaign_mission_hash_table[hash_val];

	if (cur_node == NULL) {
		return 0;
	}

	do {
		if (!stricmp(filename, cur_node->filename)) {
			return 1;
		}

		cur_node = cur_node->next;
	} while (cur_node != NULL);

	// Ran out of stuff to check
	return 0;
}

// builds hash table of campaign mission filenames
//
// returns 1 if successful, 0 if not successful
int build_campaign_mission_filename_hash_table()
{
	int rval;
	// Go through all campaign missions
	for (int i=0; i<Num_campaign_missions; i++) {
		rval = hash_insert(Campaign_missions[i]);
		if (rval == 0) {
			return 0;
		}
	}

	// successful
	return 1;
}

// deletes hash table nodes
//
void campaign_mission_hash_table_delete()
{
	hash_node *cur_node;

	for (int i=0; i<CAMPAIGN_MISSION_HASH_SIZE; i++) {
		// Look for entries into array
		if (Campaign_mission_hash_table[i] != NULL) {
			cur_node = Campaign_mission_hash_table[i];

			// Walk down the list deleting self
			while (cur_node->next != NULL) {
				hash_node *temp = cur_node->next;
				cur_node->~hash_node();
				vm_free(cur_node);
				cur_node = temp;
			}

			// Delete last node
			cur_node->~hash_node();
			vm_free(cur_node);
			Campaign_mission_hash_table[i] = NULL;
		}
	}
}


// add a line of sim_room smuck to end of list
int sim_room_line_add(int type, char *name, char *filename, int x, int y, int flags)
{
	if (Num_lines >= MAX_LINES)
		return 0;

	sim_room_lines[Num_lines].type = type;
	sim_room_lines[Num_lines].name = name;
	sim_room_lines[Num_lines].filename = filename;
	sim_room_lines[Num_lines].x = x;
	sim_room_lines[Num_lines].y = y;
	sim_room_lines[Num_lines].flags = flags;
	return Num_lines++;
}

// filter out all multiplayer campaigns
int campaign_room_campaign_filter(char *filename)
{
	Int3();

	return 0;

	// obsolete, moved all of this to global mission_campaign_build_list() - taylor

/*	int type, max_players;
	char name[NAME_LENGTH], *desc = NULL;

	#ifdef OEM_BUILD
	// also need to check if this is the builtin campaign
	if ( game_find_builtin_mission(filename) && mission_campaign_get_info(filename, name, &type, &max_players, &desc) ) {
	#else
	if ( mission_campaign_get_info(filename, name, &type, &max_players, &desc) ) {
	#endif
		if ( type == CAMPAIGN_TYPE_SINGLE ) {			
			Campaign_file_names_temp[Num_campaigns] = vm_strdup(filename);
			Campaign_descs_temp[Num_campaigns++] = desc;
			return 1;			
		}
	}

	if (desc){
		vm_free(desc);
	}

	return 0;*/
}

// build up a list of all missions in all campaigns.
int sim_room_campaign_mission_filter(char *filename)
{
	int num;

	num = mission_campaign_get_mission_list(filename, &Campaign_missions[Num_campaign_missions], MAX_MISSIONS - Num_campaign_missions);
	if (num < 0)
		return 0;

	Num_campaigns++;
	Num_campaign_missions += num;
	return 1;
}

// filter out all missions already used in existing campaigns
int sim_room_standalone_mission_filter(char *filename)
{
	int type;
	char mission_name[255];

	// Check if a campaign mission (single and multi)
	if (campaign_mission_hash_collision(filename)) {
		return 0;
	} 

	// Check if a standalone multi mission OR Mdisk mission with data
	type = mission_parse_is_multi(filename, mission_name);
	if (type && !(type & MISSION_TYPE_SINGLE))
		return 0;

	return 1;
}

// builds up list of standalone missions and adds them to missions simulator
// processes one mission per frame
//
// returns 1 if finished with all missions, 0 otherwise
//
int build_standalone_mission_list_do_frame()
{
	int font_height = gr_get_font_height();
	char filename[MAX_FILENAME_LEN];
	char str[256];
	
	// When no standalone missions in data directory
	if (Num_standalone_missions == 0) {
		Standalone_mission_names_inited = 1;
		return 1;
	}

	// Set global variable so we we'll have list available next time
	Standalone_mission_names[Num_standalone_missions_with_info] = NULL;
	Standalone_mission_flags[Num_standalone_missions_with_info] = 0;

	if (Num_standalone_missions > 0) {  // sanity check
		if (strlen(Mission_filenames[Num_standalone_missions_with_info]) < MAX_FILENAME_LEN - 4) { // sanity check?
			strcpy(filename, Mission_filenames[Num_standalone_missions_with_info]);

			// update popup		
			memset(str, 0, 256);
			sprintf(str, XSTR("Single Mission\n\n%s",989), filename);
			popup_change_text(str);

			// tack on an extension
			strcat(filename, FS_MISSION_FILE_EXT);
			if (!get_mission_info(filename)) {			
				Standalone_mission_names[Num_standalone_missions_with_info] = vm_strdup(The_mission.name);
				Standalone_mission_flags[Num_standalone_missions_with_info] = The_mission.game_type;
				int y = Num_lines * (font_height + 2);

				// determine some extra information
				int flags = 0;
				fs_builtin_mission *fb = game_find_builtin_mission(filename);				
				if((fb != NULL) && (fb->flags & FSB_FROM_VOLITION)){
					flags |= READYROOM_FLAG_FROM_VOLITION;
				}				

				// add the line
				sim_room_line_add(READYROOM_LINE_MISSION, Standalone_mission_names[Num_standalone_missions_with_info], Mission_filenames[Num_standalone_missions_with_info], list_x1 + M_TEXT_X, y, flags);			
			}
		}

		Num_standalone_missions_with_info++;
	}

	if (Num_standalone_missions_with_info == Num_standalone_missions) {
		Standalone_mission_names_inited = 1;
		return 1;
	} else {
		return 0;
	}
}

// builds up list of already played missions in a campaign and adds them to missions simulator
// processes one mission per frame
//
// returns 1 if finished with all missions, 0 otherwise
//
int build_campaign_mission_list_do_frame()
{
	int font_height = gr_get_font_height();
	char str[256];
	static int valid_missions_with_info = 0; // we use this to avoid blank entries in the mission list

	// When no campaign files in data directory
	if (Campaign.num_missions == 0) {
		Campaign_mission_names_inited = 1;
		return 1;
	}

	// change popup
	memset(str, 0, 256);
	sprintf(str, XSTR("Campaign Mission\n\n%s",990), Campaign.missions[Num_campaign_missions_with_info].name);
	popup_change_text(str);

	// Set global variable so we we'll have list available next time
	Campaign_mission_names[Num_campaign_missions_with_info] = NULL;
	Campaign_mission_flags[Num_campaign_missions_with_info] = 0;

	// Only allow missions already completed
	if (Campaign.missions[Num_campaign_missions_with_info].completed || Simroom_show_all) 
	{
		if (!get_mission_info(Campaign.missions[Num_campaign_missions_with_info].name)) 
		{
			// add to list
			Campaign_mission_names[Num_campaign_missions_with_info] = vm_strdup(The_mission.name);
			Campaign_mission_flags[Num_campaign_missions_with_info] = The_mission.game_type;
			int y = valid_missions_with_info * (font_height + 2);

			// determine some extra information
			int flags = 0;
			fs_builtin_mission *fb = game_find_builtin_mission(Campaign.missions[Num_campaign_missions_with_info].name);				
			if((fb != NULL) && (fb->flags & FSB_FROM_VOLITION))
			{
				flags |= READYROOM_FLAG_FROM_VOLITION;
			}				
	
			sim_room_line_add(READYROOM_LINE_CMISSION, Campaign_mission_names[Num_campaign_missions_with_info], Campaign.missions[Num_campaign_missions_with_info].name, list_x1 + C_SUBTEXT_X, y, flags);
			valid_missions_with_info++;
		}
	}

	Num_campaign_missions_with_info++;

	if (Num_campaign_missions_with_info == Campaign.num_missions) {
		valid_missions_with_info = 0;
		Campaign_mission_names_inited = 1;
		return 1;
	} else {
		return 0;
	}
}

// Builds list of either (1) standalone missions or (2) already played missions in current campaign
// First time through, it builds list, next time it uses precompiled list
void sim_room_build_listing()
{
	int i, y, max_num_entries_viewable;
	int font_height = gr_get_font_height();
	char full_filename[256];

	Num_lines = y = 0;
	list_y = Mission_list_coords[gr_screen.res][1];
	list_h = Mission_list_coords[gr_screen.res][3];

	// Stand alone single player missions.
	if (Player->readyroom_listing_mode == MODE_MISSIONS) {
		if (Hash_table_inited) {
			if (!Standalone_mission_names_inited) {  // Is this the first time through
				// build_list_do_frame builds list and adds sim room line and sets Standalone_mission_names_inited
				popup_till_condition(build_standalone_mission_list_do_frame, POPUP_CANCEL, XSTR("Loading missions", 991) );
			} else {
				for (i=0; i<Num_standalone_missions_with_info; i++) {
					if (Standalone_mission_names[i]) {
						// determine some extra information
						int flags = 0;
						memset(full_filename, 0, 256);
						strcpy(full_filename, cf_add_ext(Mission_filenames[i], FS_MISSION_FILE_EXT));
						fs_builtin_mission *fb = game_find_builtin_mission(full_filename);						
						if((fb != NULL) && (fb->flags & FSB_FROM_VOLITION)){
							flags |= READYROOM_FLAG_FROM_VOLITION;
						}
						
						sim_room_line_add(READYROOM_LINE_MISSION, Standalone_mission_names[i], Mission_filenames[i], list_x1 + M_TEXT_X, y, flags);
						y += font_height + 2;
					}
				}
			}
		}
	} else {
		// Campaign missions
		list_y += font_height + 2;
		list_h -= font_height - 2;

		if (!Campaign_mission_names_inited) {  // Is this the first time through
			// builds list, adds sim room line and sets Campaign_mission_names_inited
			popup_till_condition(build_campaign_mission_list_do_frame, POPUP_CANCEL, XSTR("Loading campaign missions",992));
		} else {
			for (i=0; i<Num_campaign_missions_with_info; i++) {
				if (Campaign_mission_names[i]) {
					// determine some extra information
					int flags = 0;
					memset(full_filename, 0, 256);
					strcpy(full_filename, cf_add_ext(Campaign.missions[i].name, FS_MISSION_FILE_EXT));
					fs_builtin_mission *fb = game_find_builtin_mission(full_filename);
					if((fb != NULL) && (fb->flags & FSB_FROM_VOLITION)){
						flags |= READYROOM_FLAG_FROM_VOLITION;
					}					

					sim_room_line_add(READYROOM_LINE_CMISSION, Campaign_mission_names[i], Campaign.missions[i].name, list_x1 + C_SUBTEXT_X, y, flags);
					y += font_height + 2;	// Goober5000 - added +2 to conform with above
				}
			}
		}
	}

	// free up memory from parsing all of those missions
	stop_parse();

	// set appropriate slider size
	max_num_entries_viewable = list_h / (font_height + 2);
	Sim_room_slider.set_numberItems((Num_lines > max_num_entries_viewable) ? (Num_lines - max_num_entries_viewable) : 0);
}

void sim_room_reset_campaign_listing()
{
	// only reset if we got fully inited in the first place
	if (!Campaign_mission_names_inited)
		return;


	for (int i=0; i<Campaign.num_missions; i++) {
		if (Campaign_mission_names[i]) {
			vm_free(Campaign_mission_names[i]);
			Campaign_mission_names[i] = NULL;
		}
	}

	Campaign_mission_names_inited = 0;
	Num_campaign_missions_with_info = 0;

	sim_room_build_listing();
}

int sim_room_line_query_visible(int n)
{
	int y;

	if ((n < 0) || (n >= Num_lines))
		return 0;
	
	y = sim_room_lines[n].y - sim_room_lines[Scroll_offset].y;
	if ((y < 0) || (y + gr_get_font_height() > list_h))
		return 0;

	return 1;
}

void sim_room_scroll_screen_up()
{
	if (Scroll_offset > 0) {
		Scroll_offset--;

		if (Player->readyroom_listing_mode == MODE_MISSIONS)
		{
			Assert(Selected_line > Scroll_offset);
			while (!sim_room_line_query_visible(Selected_line))
			{
				Selected_line--;
			}
		}

		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

void sim_room_scroll_line_up()
{
	if (Selected_line) {
		Selected_line--;
		if (Selected_line < Scroll_offset)
		{
			Scroll_offset = Selected_line;
			Sim_room_slider.forceUp();
		}

		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

void sim_room_scroll_screen_down()
{
	if (sim_room_lines[Num_lines - 1].y + gr_get_font_height() > sim_room_lines[Scroll_offset].y + list_h) {
		Scroll_offset++;

		if (Player->readyroom_listing_mode == MODE_MISSIONS)
		{			
			while (!sim_room_line_query_visible(Selected_line))
			{
				Selected_line++;
				Assert(Selected_line < Num_lines);
			}
		}

		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

void sim_room_scroll_line_down()
{
	if (Selected_line < Num_lines - 1) {
		Selected_line++;

		Assert(Selected_line > Scroll_offset);
		while (!sim_room_line_query_visible(Selected_line))
		{
			Scroll_offset++;
			Sim_room_slider.forceDown();
		}

		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

/*  Goober5000 - why are there two nearly identical functions?
	(campaign_room_reset_campaign and sim_room_reset_campaign)
	Looks like this function should be deprecated...

// returns: 0 = success, !0 = aborted or failed
int sim_room_reset_campaign()
{
	int z, rval = 1;

	z = popup(PF_TITLE_BIG | PF_TITLE_RED, 2, POPUP_CANCEL, POPUP_OK, XSTR( "Warning\nThis will cause all progress in your\ncurrent campaign to be lost", 110) );

	if (z) {
		mission_campaign_savefile_delete(Campaign.filename);
		mission_campaign_load(Campaign.filename);

		rval = 0;
	}

	return rval;
}
*/

// Decide if we should offer choice to resume this savegame
int sim_room_can_resume_savegame(char *savegame_filename)
{
	#ifdef FREESPACE_SAVERESTORE_SYSTEM
	char savegame_mission[MAX_FILENAME_LEN];

	if (state_read_description(savegame_filename, NULL, savegame_mission)) {
		return 0;
	}

	if (stricmp(Game_current_mission_filename, savegame_mission)) {
		return 0;
	}

	return 1;
	#else
	return 0;
	#endif
}

// Decide wether to resume a save game or not
// exit:	1	=>	savegame has been restored
//			0	=>	no restore, proceed to briefing
//			-1	=>	don't start mission at all
int sim_room_maybe_resume_savegame()
{
	// MWA -- 3/26/98 -- removed all savegame references in game
	return 0;

	/*
	char savegame_filename[_MAX_FNAME];
	int popup_rval = -1, resume_savegame = 0;

	// Generate the save-game filename for this campaign
	memset(savegame_filename, 0, _MAX_FNAME);
	mission_campaign_savefile_generate_root(savegame_filename);
	strcat(savegame_filename, NOX("svg"));

	// Decide if we should offer choice to resume this savegame
	if ( sim_room_can_resume_savegame(savegame_filename) ) {
		popup_rval = popup(0, 3, XSTR("&Cancel",-1), XSTR("&Overwrite",-1), XSTR("&Resume",-1), XSTR("A save game for this mission exists.", -1));
		switch ( popup_rval ) {
		case 0:
		case -1:
			resume_savegame = -1;
			break;
		case 1:
			resume_savegame = 0;
			break;
		case 2:
			resume_savegame = 1;
			break;
		default:
			Int3();
			resume_savegame = -1;
			break;
		}
	} else {
		resume_savegame = 0;
	}

	if (resume_savegame == 1) {
		if ( state_restore_all(savegame_filename) == -1 ) {
			popup_rval = popup(PF_TITLE_BIG | PF_TITLE_RED, 2, POPUP_NO, POPUP_YES, XSTR("Error\nSaved misison could not be loaded.\nDo you wish to start this mission from the beginning?", -1));
			if (popup_rval == 1) {
				resume_savegame = 0;
			} else {
				resume_savegame = -1;
			}

		} else {
			resume_savegame = 1;
		}
	}

	// If we are resuming this savegame, then delete the file
	if (resume_savegame == 1) {
		cf_delete(savegame_filename);
	}

	return resume_savegame;
	*/
}

int readyroom_continue_campaign()
{
	// check for possible mission loop, need to do this before calling mission_campaign_next_mission()!
	if ( !(Game_mode & GM_MULTIPLAYER) && !Campaign.loop_enabled && (Campaign.current_mission == -1) &&
		(Campaign.prev_mission != -1) && (Campaign.next_mission != -1) )
	{
		if (Campaign.missions[Campaign.prev_mission].has_mission_loop) {
			// NOTE: the order of these calls is *very* important

			// we must manually set the current mission to the *previous* mission and the mission name.
			// this isn't the same thing that is done in mission_campaign_next_mission(), but it's
			// cleaner to do this here than it is to hack that function to do the same thing
			Campaign.current_mission = Campaign.prev_mission;
			strncpy( Game_current_mission_filename, Campaign.missions[Campaign.current_mission].name, MAX_FILENAME_LEN );

			// set the bit for campaign mode
			Game_mode |= GM_CAMPAIGN_MODE;

			// eval the mission (may be needed to setup loop mission stuff)
			if (Campaign.loop_mission == CAMPAIGN_LOOP_MISSION_UNINITIALIZED)
				mission_campaign_eval_next_mission();
	
			// only proceed to loop if we have a loop
			if (Campaign.loop_mission > 0) {
				gameseq_post_event(GS_EVENT_LOOP_BRIEF);
				return 0;
			}
		}
	}

	int mc_rval = mission_campaign_next_mission();
	if (mc_rval == -1)
	{  // is campaign and next mission valid?
#ifdef FS2_DEMO
		int reset_campaign = 0;
		reset_campaign = popup(PF_BODY_BIG, 2, POPUP_NO, POPUP_YES, XSTR( "Demo Campaign Is Over.  Would you like to play the campaign again?", 111) );
		if ( reset_campaign == 1 ) {
			mission_campaign_savefile_delete(Campaign.filename);
			mission_campaign_load(Campaign.filename);
			mission_campaign_next_mission();
		} else {
			return -1;
		}
#else
		gamesnd_play_iface(SND_GENERAL_FAIL);
		popup(0, 1, POPUP_OK, XSTR( "The campaign is over.  To replay the campaign, either create a new pilot or restart the campaign in the campaign room.", 112) );
		return -1;
#endif
	}
	else if(mc_rval == -2)
	{
		gamesnd_play_iface(SND_GENERAL_FAIL);
		popup(0, 1, POPUP_OK, NOX("The current campaign has no missions") );
		return -1;
	}

	// CD CHECK
	if(!game_do_cd_mission_check(Game_current_mission_filename)){		
		return -1;
	}

	// set the bit for campaign mode
	Game_mode |= GM_CAMPAIGN_MODE;
	gameseq_post_event( GS_EVENT_START_GAME );	

	return 0;
}

void sim_room_commit()
{
	if ((Selected_line >= Num_lines) || !sim_room_lines[Selected_line].filename) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	strncpy(Game_current_mission_filename, sim_room_lines[Selected_line].filename, MAX_FILENAME_LEN);

	Game_mode &= ~(GM_CAMPAIGN_MODE);						// be sure this bit is clear

	// CD CHECK
	if(game_do_cd_mission_check(Game_current_mission_filename)){		
		// don't resume savegame, proceed to briefing
		gameseq_post_event(GS_EVENT_START_GAME);
		gamesnd_play_iface(SND_COMMIT_PRESSED);
	}
}

int sim_room_button_pressed(int n)
{
	switch (n) {
		case SCROLL_UP_BUTTON:
			sim_room_scroll_screen_up();
			Sim_room_slider.forceUp();
			break;

		case SCROLL_DOWN_BUTTON:
			sim_room_scroll_screen_down();
			Sim_room_slider.forceDown();
			break;

		case MISSION_TAB:
			Simroom_show_all = 0;
#ifdef OEM_BUILD
			game_feature_not_in_demo_popup();
//			gamesnd_play_iface(SND_GENERAL_FAIL);
			break;
#else
			Player->readyroom_listing_mode = MODE_MISSIONS;
			Selected_line = Scroll_offset = 0;
			gamesnd_play_iface(SND_USER_SELECT);
			sim_room_build_listing();
			break;
#endif

		case CAMPAIGN_TAB:
			if ( !strlen(Campaign.filename) ) {
				popup( PF_NO_NETWORKING, 1, POPUP_OK, XSTR( "The currently active campaign cannot be found, unable to switch to campaign mode!", -1));
				break;
			}

			if (Player->readyroom_listing_mode != MODE_CAMPAIGNS)
				Simroom_show_all = 0;

			Player->readyroom_listing_mode = MODE_CAMPAIGNS;
			Scroll_offset = 0;
			gamesnd_play_iface(SND_USER_SELECT);
			sim_room_build_listing();
			break;

		case COMMIT_BUTTON:
			sim_room_commit();
			break;

		case HELP_BUTTON:
			launch_context_help();
			gamesnd_play_iface(SND_HELP_PRESSED);
			break;

		case OPTIONS_BUTTON:
			gamesnd_play_iface(SND_SWITCH_SCREENS);
			gameseq_post_event(GS_EVENT_OPTIONS_MENU);
			return 1;

		case TECH_DATABASE_BUTTON:
			gamesnd_play_iface(SND_SWITCH_SCREENS);
			gameseq_post_event(GS_EVENT_TECH_MENU);
			return 1;

		case CUTSCENES_BUTTON:
			gamesnd_play_iface(SND_SWITCH_SCREENS);
			gameseq_post_event(GS_EVENT_GOTO_VIEW_CUTSCENES_SCREEN);
			return 1;

		case CREDITS_BUTTON:
			gamesnd_play_iface(SND_SWITCH_SCREENS);
			gameseq_post_event(GS_EVENT_CREDITS);
			return 1;
	}

	return 0;
}

// do nothing
void sim_room_scroll_capture()
{
}

// ---------------------------------------------------------------------
// mission_sim_room_init()
//
// Initialize the sim_room assignment screen system.  Called when GS_STATE_sim_room_SCREEN
// is entered.
//

void sim_room_init()
{
	int i;
	sim_room_buttons *b;
	char wild_card[256];

	list_x1 = Mission_list_coords[gr_screen.res][0];
	list_x2 = Campaign_list_coords[gr_screen.res][0];
	list_y = Mission_list_coords[gr_screen.res][1];
	list_w1 = Mission_list_coords[gr_screen.res][2];
	list_w2 = Campaign_list_coords[gr_screen.res][2];
	list_h = Mission_list_coords[gr_screen.res][3];

	// force mode to valid range.  I once had a bogus value here.  Don't know how that happened, though.
	if (Player->readyroom_listing_mode != MODE_MISSIONS)
		Player->readyroom_listing_mode = MODE_CAMPAIGNS;

	*Game_current_mission_filename = 0;
	common_set_interface_palette("InterfacePalette");  // set the interface palette
	Ui_window.create(0, 0, gr_screen.max_w_unscaled, gr_screen.max_h_unscaled, 0);
	Ui_window.set_mask_bmap(Sim_mask_filename[gr_screen.res]);

	for (i=0; i<NUM_BUTTONS; i++) {
		b = &Buttons[gr_screen.res][i];

		b->button.create(&Ui_window, "", b->x, b->y, 60, 30, (i < 2), 1);
		// set up callback for when a mouse first goes over a button
		b->button.set_highlight_action(common_play_highlight_sound);
		b->button.set_bmaps(b->filename);
		b->button.link_hotspot(b->hotspot);
	}

	// screen/button specific text
	Ui_window.add_XSTR("Single Missions", 1060, Buttons[gr_screen.res][MISSION_TAB].xt, Buttons[gr_screen.res][MISSION_TAB].yt, &Buttons[gr_screen.res][MISSION_TAB].button, UI_XSTR_COLOR_GREEN);
	Ui_window.add_XSTR("Campaign Missions", 1061, Buttons[gr_screen.res][CAMPAIGN_TAB].xt, Buttons[gr_screen.res][CAMPAIGN_TAB].yt, &Buttons[gr_screen.res][CAMPAIGN_TAB].button, UI_XSTR_COLOR_GREEN);
	Ui_window.add_XSTR("Help", 928, Buttons[gr_screen.res][HELP_BUTTON].xt, Buttons[gr_screen.res][HELP_BUTTON].yt, &Buttons[gr_screen.res][HELP_BUTTON].button, UI_XSTR_COLOR_GREEN);
	Ui_window.add_XSTR("Commit", 1062, Buttons[gr_screen.res][COMMIT_BUTTON].xt, Buttons[gr_screen.res][COMMIT_BUTTON].yt, &Buttons[gr_screen.res][COMMIT_BUTTON].button, UI_XSTR_COLOR_PINK);
	Ui_window.add_XSTR("Options", 1036, Buttons[gr_screen.res][OPTIONS_BUTTON].xt, Buttons[gr_screen.res][OPTIONS_BUTTON].yt, &Buttons[gr_screen.res][OPTIONS_BUTTON].button, UI_XSTR_COLOR_GREEN);
	
	// common tab button text
	Ui_window.add_XSTR("Technical Database", 1055, Buttons[gr_screen.res][TECH_DATABASE_BUTTON].xt,  Buttons[gr_screen.res][TECH_DATABASE_BUTTON].yt, &Buttons[gr_screen.res][TECH_DATABASE_BUTTON].button, UI_XSTR_COLOR_GREEN);
	Ui_window.add_XSTR("Mission Simulator", 1056, Buttons[gr_screen.res][SIMULATOR_BUTTON].xt,  Buttons[gr_screen.res][SIMULATOR_BUTTON].yt, &Buttons[gr_screen.res][SIMULATOR_BUTTON].button, UI_XSTR_COLOR_GREEN);
	Ui_window.add_XSTR("Cutscenes", 1057, Buttons[gr_screen.res][CUTSCENES_BUTTON].xt,  Buttons[gr_screen.res][CUTSCENES_BUTTON].yt, &Buttons[gr_screen.res][CUTSCENES_BUTTON].button, UI_XSTR_COLOR_GREEN);
	Ui_window.add_XSTR("Credits", 1058, Buttons[gr_screen.res][CREDITS_BUTTON].xt,  Buttons[gr_screen.res][CREDITS_BUTTON].yt, &Buttons[gr_screen.res][CREDITS_BUTTON].button, UI_XSTR_COLOR_GREEN);

	// misc text - not associated with any buttons
	Ui_window.add_XSTR("Mission", 1063, Sim_misc_text_coords[gr_screen.res][SIM_MISC_TEXT_MISSION][0], Sim_misc_text_coords[gr_screen.res][SIM_MISC_TEXT_MISSION][1], NULL, UI_XSTR_COLOR_GREEN);
	Ui_window.add_XSTR("Filename", 1064, Sim_misc_text_coords[gr_screen.res][SIM_MISC_TEXT_FILENAME][0], Sim_misc_text_coords[gr_screen.res][SIM_MISC_TEXT_FILENAME][1], NULL, UI_XSTR_COLOR_GREEN);

	for (i=0; i<LIST_BUTTONS_MAX; i++) {
		List_buttons[i].create(&Ui_window, "", 0, 0, 60, 30, 0, 1);
		List_buttons[i].hide();
		List_buttons[i].disable();
	}

	// set up sim_rooms for buttons so we draw the correct animation frame when a key is pressed
	Buttons[gr_screen.res][SCROLL_UP_BUTTON].button.set_hotkey(KEY_PAGEUP);
	Buttons[gr_screen.res][SCROLL_DOWN_BUTTON].button.set_hotkey(KEY_PAGEDOWN);
	Buttons[gr_screen.res][COMMIT_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_ENTER);	

	Background_bitmap = bm_load(Sim_filename[gr_screen.res]);

	// load in help overlay bitmap	
	help_overlay_load(SIM_ROOM_OVERLAY);
	help_overlay_set_state(SIM_ROOM_OVERLAY,0);

	Scroll_offset = Selected_line = 0;

	strcpy(Cur_campaign, Player->current_campaign);
	if ( !mission_load_up_campaign() ) {
		mission_campaign_next_mission();
	} else {
		Campaign.filename[0] = 0;
		Campaign.num_missions = 0;
	}

	Num_campaign_missions = 0;
	Get_file_list_filter = sim_room_campaign_mission_filter;

	mission_campaign_build_list(false, false);	// no descs, no sorting

	Hash_table_inited = 0;
	if (build_campaign_mission_filename_hash_table()) {
		Hash_table_inited = 1;
	}

	// if there is no campaign available then force the use of mode_missions only
	if ( !strlen(Campaign.filename) )
		Player->readyroom_listing_mode = MODE_MISSIONS;

	// HACK
	GR_MAYBE_CLEAR_RES(Background_bitmap);
	if(Background_bitmap != -1){
		gr_set_bitmap(Background_bitmap);
		gr_bitmap(0, 0);
	}
	Ui_window.draw();
	gr_flip();		

	Get_file_list_filter = sim_room_standalone_mission_filter;
	memset(wild_card, 0, 256);
	strcpy(wild_card, NOX("*"));
	strcat(wild_card, FS_MISSION_FILE_EXT);
	Num_standalone_missions = cf_get_file_list(MAX_MISSIONS, Mission_filenames, CF_TYPE_MISSIONS, wild_card, CF_SORT_NAME);

	// set up slider with 0 items to start
	Sim_room_slider.create(&Ui_window, Sim_room_slider_coords[gr_screen.res][X_COORD], Sim_room_slider_coords[gr_screen.res][Y_COORD], Sim_room_slider_coords[gr_screen.res][W_COORD], Sim_room_slider_coords[gr_screen.res][H_COORD], 0, Sim_room_slider_filename[gr_screen.res], &sim_room_scroll_screen_up, &sim_room_scroll_screen_down, &sim_room_scroll_capture);

	Num_campaign_missions_with_info = Num_standalone_missions_with_info = Standalone_mission_names_inited = Campaign_mission_names_inited = 0;
	Simroom_show_all = 0;
	sim_room_build_listing();

	// load special mission icons
	sim_room_load_mission_icons();
}

// ---------------------------------------------------------------------
// sim_room_close()
//
// Cleanup the sim_room assignment screen system.  Called when GS_STATE_sim_room_SCREEN
// is left.
//
void sim_room_close()
{
	int i;

	for (i=0; i<Num_campaign_missions; i++) {
		if (Campaign_missions[i]) {
			vm_free(Campaign_missions[i]);
			Campaign_missions[i] = NULL;
		}
	}

	if (Background_bitmap >= 0)
		bm_release(Background_bitmap);

	if (Standalone_mission_names_inited){
		for (i=0; i<Num_standalone_missions; i++){
			if (Standalone_mission_names[i] != NULL){
				vm_free(Standalone_mission_names[i]);
				Standalone_mission_names[i] = NULL;
			}
			Standalone_mission_flags[i] = 0;
		}
	}

	if (Campaign_mission_names_inited) {
		for (i=0; i<Campaign.num_missions; i++) {
			if (Campaign_mission_names[i]) {
				vm_free(Campaign_mission_names[i]);
				Campaign_mission_names[i] = NULL;
			}
		}
	}

	for (i=0; i<Num_standalone_missions; i++) {
		vm_free(Mission_filenames[i]);
		Mission_filenames[i] = NULL;
	}

	// free global Campaign_* list stuff
	mission_campaign_free_list();

	// unload the overlay bitmap
	help_overlay_unload(SIM_ROOM_OVERLAY);

	campaign_mission_hash_table_delete();

	Ui_window.destroy();
	common_free_interface_palette();		// restore game palette
	write_pilot_file();
	mission_campaign_savefile_save();

	// unload special mission icons
	sim_room_unload_mission_icons();
}

// ---------------------------------------------------------------------
// sim_room_do_frame()
//
// Called once per frame to process user input for the sim_room Assignment Screen
//
void sim_room_do_frame(float frametime)
{
	char buf[256];
	int i, k, y, z, line;
	int font_height = gr_get_font_height();
	int select_tease_line = -1;  // line mouse is down on, but won't be selected until button released	

	z = -1;
	for (i=0; i<Num_campaigns; i++)
		if (!stricmp(Campaign_file_names[i], Campaign.filename)) {
			z = i;
			break;
		}

	if ( help_overlay_active(SIM_ROOM_OVERLAY) ) {
		Buttons[gr_screen.res][HELP_BUTTON].button.reset_status();
		Ui_window.set_ignore_gadgets(1);
	}

	k = Ui_window.process() & ~KEY_DEBUGGED;

	if ( (k > 0) || B1_JUST_RELEASED ) {
		if ( help_overlay_active(SIM_ROOM_OVERLAY) ) {
			help_overlay_set_state(SIM_ROOM_OVERLAY, 0);
			Ui_window.set_ignore_gadgets(0);
			k = 0;
		}
	}

	if ( !help_overlay_active(SIM_ROOM_OVERLAY) ) {
		Ui_window.set_ignore_gadgets(0);
	}

	switch (k) {
		case KEY_DOWN:  // scroll list down
			sim_room_scroll_line_down();
			break;

		case KEY_UP:  // scroll list up
			sim_room_scroll_line_up();
			break;

		case KEY_ESC:
			gameseq_post_event(GS_EVENT_MAIN_MENU);
			break;

		case KEY_CTRLED | KEY_UP:
			sim_room_button_pressed(TECH_DATABASE_BUTTON);
			break;

		case KEY_CTRLED | KEY_DOWN:
			sim_room_button_pressed(CUTSCENES_BUTTON);
			break;

		case KEY_TAB:
			if (Player->readyroom_listing_mode == MODE_CAMPAIGNS)
				Player->readyroom_listing_mode = MODE_MISSIONS;
			else if ( strlen(Campaign.filename) )
				Player->readyroom_listing_mode = MODE_CAMPAIGNS;
			else
				Player->readyroom_listing_mode = MODE_MISSIONS;

			Selected_line = Scroll_offset = Simroom_show_all = 0;
			gamesnd_play_iface(SND_USER_SELECT);
			sim_room_build_listing();
			break;

		case KEY_F2:
			gamesnd_play_iface(SND_SWITCH_SCREENS);
			gameseq_post_event(GS_EVENT_OPTIONS_MENU);
			break;

		case KEY_CTRLED | KEY_SHIFTED | KEY_S:
			if (Player->readyroom_listing_mode == MODE_CAMPAIGNS) {
				Simroom_show_all = !Simroom_show_all;
				sim_room_reset_campaign_listing();
			}
			break;
	}	// end switch

	for (i=0; i<NUM_BUTTONS; i++){
		if (Buttons[gr_screen.res][i].button.pressed()){
			if (sim_room_button_pressed(i)){
				return;
			}
		}
	}

	for (i=0; i<LIST_BUTTONS_MAX; i++) {
		if (List_buttons[i].is_mouse_on())
			select_tease_line = i + Scroll_offset;
	
		if (List_buttons[i].pressed()) {
			Selected_line = i + Scroll_offset;
			gamesnd_play_iface(SND_USER_SELECT);
		}
	}

	GR_MAYBE_CLEAR_RES(Background_bitmap);
	if (Background_bitmap >= 0) {
		gr_set_bitmap(Background_bitmap);
		gr_bitmap(0, 0);
	}

	Ui_window.draw();

	for (i=TECH_DATABASE_BUTTON; i<=CREDITS_BUTTON; i++){
		if (Buttons[gr_screen.res][i].button.button_down()){
			break;
		}
	}

	if (i > CREDITS_BUTTON){
		Buttons[gr_screen.res][SIMULATOR_BUTTON].button.draw_forced(2);
	}

	if (!Buttons[gr_screen.res][CAMPAIGN_TAB].button.button_down() && !Buttons[gr_screen.res][MISSION_TAB].button.button_down()) {
		if (Player->readyroom_listing_mode == MODE_CAMPAIGNS){
			Buttons[gr_screen.res][CAMPAIGN_TAB].button.draw_forced(2);
		} else if (Player->readyroom_listing_mode == MODE_MISSIONS){
			Buttons[gr_screen.res][MISSION_TAB].button.draw_forced(2);
		}
	}

	gr_set_font(FONT1);
	if (Player->readyroom_listing_mode == MODE_CAMPAIGNS) {
		gr_set_color_fast(&Color_text_heading);
		strcpy(buf, Campaign.name);
		gr_force_fit_string(buf, 255, list_w1);
		gr_printf(list_x1, Mission_list_coords[gr_screen.res][1], buf);

		if ( strlen(Campaign.filename) > 0 ) {			
			sprintf(buf, NOX("%s%s"), Campaign.filename, FS_CAMPAIGN_FILE_EXT);
			gr_force_fit_string(buf, 255, list_w2);
			gr_printf(list_x2, Mission_list_coords[gr_screen.res][1], buf);		

			// blit the proper icons if necessary
			char full_name[256];
			memset(full_name, 0, 256);
			strcpy(full_name, cf_add_ext(Campaign.filename,FS_CAMPAIGN_FILE_EXT));
			fs_builtin_mission *fb = game_find_builtin_mission(full_name);
			if(fb != NULL){
				// sim_room_blit_icons(0, Mission_list_coords[gr_screen.res][1], fb, 0);
			}
		}
	}

	line = Scroll_offset;
	while (sim_room_line_query_visible(line)) {
		y = list_y + sim_room_lines[line].y - sim_room_lines[Scroll_offset].y;

		if (sim_room_lines[line].type != READYROOM_LINE_CAMPAIGN) {
			List_buttons[line - Scroll_offset].update_dimensions(list_x1, y, list_x2 + list_w2 - list_x1, font_height);
			List_buttons[line - Scroll_offset].enable();

		} else
			List_buttons[line - Scroll_offset].disable();

		if (line == Selected_line)
			gr_set_color_fast(&Color_text_selected);
		else if (line == select_tease_line)
			gr_set_color_fast(&Color_text_subselected);
		else
			gr_set_color_fast(&Color_text_normal);

		strcpy(buf, sim_room_lines[line].name);
		gr_force_fit_string(buf, 255, list_x1 + list_w1 - sim_room_lines[line].x);
		gr_printf(sim_room_lines[line].x, y, buf);

		if (sim_room_lines[line].filename) {
			strcpy(buf, sim_room_lines[line].filename);
			gr_force_fit_string(buf, 255, list_w2);
			gr_printf(list_x2, y, buf);
		}

		// blit additional icon information
		sim_room_blit_icons(line, y);

		line++;
	}

	i = line - Scroll_offset;
	while (i < LIST_BUTTONS_MAX)
		List_buttons[i++].disable();

	// blit help overlay if active
	help_overlay_maybe_blit(SIM_ROOM_OVERLAY);

	gr_flip();
}

void sim_room_blit_icons(int line_index, int y_start, fs_builtin_mission *fb, int is_md)
{
	int is_from_volition = 0;	

	// determine icon status
	if(fb == NULL){
		is_from_volition = (sim_room_lines[line_index].flags & READYROOM_FLAG_FROM_VOLITION) ? 1 : 0;		
	} else {
		is_from_volition = (fb->flags & FSB_FROM_VOLITION) ? 1 : 0;		
	}

	// if the line is flagged as a volition file
	if(is_from_volition && (Mission_icon_bitmaps[MISSION_ICON_VOLITION] >= 0)){		
		gr_set_bitmap(Mission_icon_bitmaps[MISSION_ICON_VOLITION]);
		gr_bitmap(Sim_volition_icon_x[gr_screen.res], y_start + MISSION_ICON_VOLITION_Y_OFFSET);
	}	
}

///  Campaign room stuff below
int Cr_list_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		47, 21, 565, 233
	},
	{ // GR_1024
		64, 34, 916, 459
	}
};

int Cr_info_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		28, 267, 476, 103
	},
	{ // GR_1024
		45, 427, 761, 165
	},
};

#define CR_NUM_BUTTONS					6

#define CR_SCROLL_UP_BUTTON			0
#define CR_SCROLL_DOWN_BUTTON			1
#define CR_SCROLL_INFO_UP_BUTTON		2
#define CR_SCROLL_INFO_DOWN_BUTTON	3
#define CR_RESET_BUTTON					4
#define CR_COMMIT_BUTTON				5

#define MAX_INFO_LINES		20

#define MAX_INFO_LINE_LEN	256

ui_button_info Cr_buttons[GR_NUM_RESOLUTIONS][CR_NUM_BUTTONS] = {
	{ // GR_640
		ui_button_info("CAB_00",	2,		42,	-1,	-1,	0),
		ui_button_info("CAB_01",	2,		89,	-1,	-1,	1),
		ui_button_info("CAB_02",	2,		279,	-1,	-1,	2),
		ui_button_info("CAB_03",	2,		325,	-1,	-1,	3),
		ui_button_info("CAB_04",	579,	353,	-1,	-1,	4),
		ui_button_info("CAB_05",	575,	434,	-1,	-1,	5),
	},
	{ // GR_1024
		ui_button_info("2_CAB_00",	3,		68,	-1,	-1,	0),
		ui_button_info("2_CAB_01",	3,		142,	-1,	-1,	1),
		ui_button_info("2_CAB_02",	3,		446,	-1,	-1,	2),
		ui_button_info("2_CAB_03",	3,		520,	-1,	-1,	3),
		ui_button_info("2_CAB_04",	927,	565,	-1,	-1,	4),
		ui_button_info("2_CAB_05",	920,	694,	-1,	-1,	5),
	}
};

// text
#define CR_NUM_TEXT			3
UI_XSTR Cr_text[GR_NUM_RESOLUTIONS][CR_NUM_TEXT] = {
	{ // GR_640
		{ "Restart",		1403,		569,	326, UI_XSTR_COLOR_GREEN,	-1, &Cr_buttons[0][CR_RESET_BUTTON].button },
		{ "Campaign",		1404,		569,	337, UI_XSTR_COLOR_GREEN,	-1, &Cr_buttons[0][CR_RESET_BUTTON].button },
		{ "Select",			1409,		568,	413, UI_XSTR_COLOR_PINK,	-1, &Cr_buttons[0][CR_COMMIT_BUTTON].button },
	},
	{ // GR_1024
		{ "Restart",		1403,		922,	523, UI_XSTR_COLOR_GREEN,	-1, &Cr_buttons[1][CR_RESET_BUTTON].button },
		{ "Campaign",		1404,		922,	538, UI_XSTR_COLOR_GREEN,	-1, &Cr_buttons[1][CR_RESET_BUTTON].button },
		{ "Select",			1409,		921,	665, UI_XSTR_COLOR_PINK,	-1, &Cr_buttons[1][CR_COMMIT_BUTTON].button },
	}
};

/*
static struct {
	char *text;
	int len;
} campaign_desc_lines[MAX_DESC_LINES];
*/

static int Num_desc_lines;
static int Desc_scroll_offset;
static int Selected_campaign_index;
static int Active_campaign_index;

char *Info_text_ptrs[MAX_INFO_LINES];
int Num_info_lines, Info_text_line_size[MAX_INFO_LINES];

void campaign_room_build_listing()
{
	int font_height = gr_get_font_height();
	int i, y;

	Num_lines = y = 0;

	for (i = 0; i < Num_campaigns; i++) {
		if (Campaign_names[i] != NULL) {
			// determine some extra information
			int flags = 0;
			fs_builtin_mission *fb = game_find_builtin_mission(Campaign_file_names[i]);
	
			if (fb != NULL) {
				if (fb->flags & FSB_FROM_VOLITION) {
					flags |= READYROOM_FLAG_FROM_VOLITION;
				}
			}

			sim_room_line_add(READYROOM_LINE_CAMPAIGN, Campaign_names[i], Campaign_file_names[i], Cr_list_coords[gr_screen.res][0], y, flags);
			y += font_height + 2;
		}
	}
}

void set_new_campaign_line(int n)
{
	char *str;

	Selected_campaign_index = n;
	str = Campaign_descs[Selected_campaign_index];
	Num_info_lines = 0;
	if (str) {
		Num_info_lines = split_str(str, Cr_info_coords[gr_screen.res][2], Info_text_line_size, Info_text_ptrs, MAX_INFO_LINES);
		Assert(Num_info_lines >= 0);
	}

	Desc_scroll_offset = 0;
}

void campaign_room_scroll_info_up()
{
	if (Desc_scroll_offset) {
		Desc_scroll_offset--;
		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

void campaign_room_scroll_info_down()
{
	if ( (Num_info_lines - Desc_scroll_offset) * gr_get_font_height() > Cr_info_coords[gr_screen.res][3]) {
		Desc_scroll_offset++;
		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

// returns: 0 = success, !0 = aborted or failed
int campaign_room_reset_campaign(int n)
{
	char *filename;
	int z;

	// z = popup(PF_TITLE_BIG | PF_TITLE_RED, 2, POPUP_CANCEL, POPUP_OK, XSTR( "Warning\nThis will cause all progress in your\ncurrent campaign to be lost", 110), Campaign_names[n]);
	z = popup(PF_TITLE_BIG | PF_TITLE_RED, 2, POPUP_CANCEL, POPUP_OK, XSTR( "Warning\nThis will cause all progress in your\ncurrent campaign to be lost", 110));
	if (z == 1) {
		filename = (char *) vm_malloc(strlen(Campaign_file_names[n]) + 5);
		strcpy(filename, Campaign_file_names[n]);
		strcat(filename, FS_CAMPAIGN_FILE_EXT);

		mission_campaign_savefile_delete(filename);
		mission_campaign_load(filename);
		mission_campaign_next_mission();

		return 0;
	}

	return 1;
}

void campaign_room_commit()
{
	if (Selected_campaign_index < 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	if (stricmp(Campaign_file_names[Selected_campaign_index], Campaign.filename)) {  // new campaign selected
		/* This is all that's stopping us from switching campaigns without restarting - taylor
		if ((Active_campaign_index >= 0) && campaign_room_reset_campaign(Active_campaign_index)) {
			gamesnd_play_iface(SND_GENERAL_FAIL);
			return;
		}

		mission_campaign_savefile_delete(Campaign_file_names[Selected_campaign_index]);
		*/

		// Goober5000 - reinitialize tech database if needed
		if ( (Campaign.flags & CF_CUSTOM_TECH_DATABASE) || !stricmp(Campaign.filename, "freespace2") )
		{
			// reset tech database to what's in the tables
			tech_reset_to_default();
		}

		mission_campaign_load(Campaign_file_names[Selected_campaign_index]);
		strcpy(Player->current_campaign, Campaign.filename);  // track new campaign for player

		// Goober5000 - reset player-persistent variables
		Player->num_variables = 0;
	}

	if (mission_campaign_next_mission()) {  // is campaign and next mission valid?
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	gameseq_post_event(GS_EVENT_MAIN_MENU);
	gamesnd_play_iface(SND_COMMIT_PRESSED);
}

int campaign_room_button_pressed(int n)
{
	switch (n) {
		case CR_SCROLL_UP_BUTTON:
			sim_room_scroll_screen_up();
			break;

		case CR_SCROLL_DOWN_BUTTON:
			sim_room_scroll_screen_down();
			break;

		case CR_SCROLL_INFO_UP_BUTTON:
			campaign_room_scroll_info_up();
			break;

		case CR_SCROLL_INFO_DOWN_BUTTON:
			campaign_room_scroll_info_down();
			break;

		case CR_COMMIT_BUTTON:
			campaign_room_commit();
			break;

		/*
		case CR_HELP_BUTTON:
			launch_context_help();
			gamesnd_play_iface(SND_HELP_PRESSED);
			break;

		case CR_OPTIONS_BUTTON:
			gamesnd_play_iface(SND_SWITCH_SCREENS);
			gameseq_post_event(GS_EVENT_OPTIONS_MENU);
			return 1;
		*/

		case CR_RESET_BUTTON:
			if ( (Active_campaign_index < 0) || (Active_campaign_index >= Num_campaigns) )
				gamesnd_play_iface(SND_GENERAL_FAIL);
			else if (campaign_room_reset_campaign(Active_campaign_index))
				gamesnd_play_iface(SND_GENERAL_FAIL);
			else
			{
				gamesnd_play_iface(SND_USER_SELECT);

				// Goober5000 - reinitialize tech database if needed
				if ( (Campaign.flags & CF_CUSTOM_TECH_DATABASE) || !stricmp(Campaign.filename, "freespace2") )
				{
					// reset tech database to what's in the tables
					tech_reset_to_default();
				}

				// Goober5000 - reset player-persistent variables
				Player->num_variables = 0;
			}

			break;
	}

	return 0;
}

void campaign_room_init()
{
	int i, load_failed;
	ui_button_info *b;

	list_h = Mission_list_coords[gr_screen.res][3];

	// common_set_interface_palette("InterfacePalette");  // set the interface palette
	Ui_window.create(0, 0, gr_screen.max_w_unscaled, gr_screen.max_h_unscaled, 0);
	Ui_window.set_mask_bmap(Campaign_mask_filename[gr_screen.res]);

	for (i=0; i<CR_NUM_BUTTONS; i++) {
		b = &Cr_buttons[gr_screen.res][i];

		b->button.create(&Ui_window, "", b->x, b->y, 60, 30, (i < 2), 1);
		// set up callback for when a mouse first goes over a button
		b->button.set_highlight_action(common_play_highlight_sound);
		b->button.set_bmaps(b->filename);
		b->button.link_hotspot(b->hotspot);
	}

	for (i=0; i<LIST_BUTTONS_MAX; i++) {
		List_buttons[i].create(&Ui_window, "", 0, 0, 60, 30, 0, 1);
		List_buttons[i].hide();
		List_buttons[i].disable();
	}

	// add xstrs
	for(i=0; i<CR_NUM_TEXT; i++){
		Ui_window.add_XSTR(&Cr_text[gr_screen.res][i]);
	}

	// set up sim_rooms for buttons so we draw the correct animation frame when a key is pressed
	Cr_buttons[gr_screen.res][CR_SCROLL_UP_BUTTON].button.set_hotkey(KEY_PAGEUP);
	Cr_buttons[gr_screen.res][CR_SCROLL_DOWN_BUTTON].button.set_hotkey(KEY_PAGEDOWN);
	Cr_buttons[gr_screen.res][CR_RESET_BUTTON].button.set_hotkey(KEY_DELETE);
	Cr_buttons[gr_screen.res][CR_COMMIT_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_ENTER);
	// Cr_buttons[gr_screen.res][CR_HELP_BUTTON].button.set_hotkey(KEY_F2);

	Background_bitmap = bm_load(Campaign_filename[gr_screen.res]);

	// load in help overlay bitmap	
	help_overlay_load(CAMPAIGN_ROOM_OVERLAY);
	help_overlay_set_state(CAMPAIGN_ROOM_OVERLAY,0);

	Num_desc_lines = 0;
	Desc_scroll_offset = Scroll_offset = 0;

	// Goober5000 - dumb hack, since main hall can be set in multiple places
	Old_main_hall = Player->main_hall;

	// this stuff needs to happen before the mission_campaign_build_list() call
	load_failed = mission_load_up_campaign();
	if (!load_failed) {
		mission_campaign_next_mission();
	} else {
		Campaign.filename[0] = 0;
		Campaign.num_missions = 0;
	}

	// we need descriptions too, so "true" it
	mission_campaign_build_list(true);

	campaign_room_build_listing();

	Selected_campaign_index = Active_campaign_index = -1;
	if (!load_failed) {
		for (i=0; i<Num_campaigns; i++)
			if (!stricmp(Campaign_file_names[i], Campaign.filename)) {
				set_new_campaign_line(i);
				Active_campaign_index = i;
				break;
			}
	}

	Campaign_room_no_campaigns = false;
}

void campaign_room_close()
{
	if (Background_bitmap >= 0)
		bm_release(Background_bitmap);

	// free the global Campaign_* list stuff
	mission_campaign_free_list();

	// unload the overlay bitmap
	help_overlay_unload(CAMPAIGN_ROOM_OVERLAY);

	// be sure that we are going to use the correct mainhall
	if ( (Player != NULL) && (Campaign.current_mission >= 0) ) {
		Player->main_hall = Campaign.missions[Campaign.current_mission].main_hall;

		// we might need to switch the music
		if (main_hall_get_music_index(Player->main_hall) != main_hall_get_music_index(Old_main_hall))
			main_hall_stop_music();
	}

	Ui_window.destroy();
	common_free_interface_palette();		// restore game palette
	write_pilot_file();
	mission_campaign_savefile_save();
}

void campaign_room_do_frame(float frametime)
{
	char buf[256];
	char line_text[MAX_INFO_LINE_LEN];
	int i, k, y, line;
	int font_height = gr_get_font_height();
	int select_tease_line = -1;  // line mouse is down on, but won't be selected until button released

	if ( help_overlay_active(CAMPAIGN_ROOM_OVERLAY) ) {
		// Cr_buttons[gr_screen.res][CR_HELP_BUTTON].button.reset_status();
		Ui_window.set_ignore_gadgets(1);
	}

	k = Ui_window.process() & ~KEY_DEBUGGED;

	if ( (k > 0) || B1_JUST_RELEASED ) {
		if ( help_overlay_active(CAMPAIGN_ROOM_OVERLAY) ) {
			help_overlay_set_state(CAMPAIGN_ROOM_OVERLAY, 0);
			Ui_window.set_ignore_gadgets(0);
			k = 0;
		}
	}

	if ( !help_overlay_active(CAMPAIGN_ROOM_OVERLAY) ) {
		Ui_window.set_ignore_gadgets(0);
	}

	switch (k) {
		case KEY_DOWN:  // scroll list down
			if (Selected_campaign_index < Num_campaigns - 1) {
				set_new_campaign_line(Selected_campaign_index + 1);
				gamesnd_play_iface(SND_SCROLL);

			} else
				gamesnd_play_iface(SND_GENERAL_FAIL);

			break;

		case KEY_UP:  // scroll list up
			if (Selected_campaign_index < 0)
				Selected_campaign_index = 1;

			if (Selected_campaign_index) {
				set_new_campaign_line(Selected_campaign_index - 1);
				gamesnd_play_iface(SND_SCROLL);

			} else
				gamesnd_play_iface(SND_GENERAL_FAIL);

			break;

		case KEY_ESC:
			gameseq_post_event(GS_EVENT_MAIN_MENU);
			break;
	}	// end switch

	for (i=0; i<CR_NUM_BUTTONS; i++){
		if (Cr_buttons[gr_screen.res][i].button.pressed()){
			if (campaign_room_button_pressed(i)){
				return;
			}
		}
	}

	for (i=0; i<LIST_BUTTONS_MAX; i++) {
		if (List_buttons[i].is_mouse_on()){
			select_tease_line = i + Scroll_offset;
		}
	
		if (List_buttons[i].pressed()) {
			set_new_campaign_line(i + Scroll_offset);
			gamesnd_play_iface(SND_USER_SELECT);
		}
	}

	GR_MAYBE_CLEAR_RES(Background_bitmap);
	if (Background_bitmap >= 0) {
		gr_set_bitmap(Background_bitmap);
		gr_bitmap(0, 0);
	}

	Ui_window.draw();

	gr_set_font(FONT1);
	line = Scroll_offset;
	while (sim_room_line_query_visible(line)) {
		y = Cr_list_coords[gr_screen.res][1] + sim_room_lines[line].y - sim_room_lines[Scroll_offset].y;

		List_buttons[line - Scroll_offset].update_dimensions(Cr_list_coords[gr_screen.res][0], y, Cr_list_coords[gr_screen.res][2], font_height);
		List_buttons[line - Scroll_offset].enable();

		if (!stricmp(sim_room_lines[line].filename, Campaign.filename)) {
			gr_set_color_fast(&Color_white);
			i = y + font_height / 2 - 1;
			gr_circle(Cr_list_coords[gr_screen.res][0] - 6, i, 5);

			gr_set_color_fast(&Color_bright_white);
			gr_line(Cr_list_coords[gr_screen.res][0] - 10, i, Cr_list_coords[gr_screen.res][0] - 8, i);
			gr_line(Cr_list_coords[gr_screen.res][0] - 6, i - 4, Cr_list_coords[gr_screen.res][0] - 6, i - 2);
			gr_line(Cr_list_coords[gr_screen.res][0] - 4, i, Cr_list_coords[gr_screen.res][0] - 2, i);
			gr_line(Cr_list_coords[gr_screen.res][0] - 6, i + 2, Cr_list_coords[gr_screen.res][0] - 6, i + 4);
		}

		if (line == Selected_campaign_index)
			gr_set_color_fast(&Color_text_selected);
		else if (line == select_tease_line)
			gr_set_color_fast(&Color_text_subselected);
		else
			gr_set_color_fast(&Color_text_normal);

		strcpy(buf, sim_room_lines[line].name);
		gr_force_fit_string(buf, 255, Cr_list_coords[gr_screen.res][0] + Cr_list_coords[gr_screen.res][2] - sim_room_lines[line].x);
		gr_printf(sim_room_lines[line].x, y, buf);
		line++;
	}

	i = line - Scroll_offset;
	while (i < LIST_BUTTONS_MAX)
		List_buttons[i++].disable();

	y = 0;
	i = Desc_scroll_offset;
	gr_set_color_fast(&Color_text_normal);

	while (y + font_height <= Cr_info_coords[gr_screen.res][3]) {
		if (i >= Num_info_lines)
			break;

		Assert(Info_text_line_size[i] < MAX_INFO_LINE_LEN);
		strncpy(line_text, Info_text_ptrs[i], Info_text_line_size[i]);
		line_text[Info_text_line_size[i]] = 0;
		drop_white_space(line_text);
		gr_string(Cr_info_coords[gr_screen.res][0], Cr_info_coords[gr_screen.res][1] + y, line_text);
		y += font_height;
		i++;
	}

	if (Num_campaigns < 1) {
		popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR( "No campaigns are available!", -1));
		Campaign_room_no_campaigns = true;
		gameseq_post_event(GS_EVENT_MAIN_MENU);
	}

	// blit help overlay if active
	help_overlay_maybe_blit(CAMPAIGN_ROOM_OVERLAY);

	gr_flip();
}

void sim_room_load_mission_icons()
{
	int idx;
	
	// load all bitmaps
	for(idx=0; idx<NUM_MISSION_ICONS; idx++){
		Mission_icon_bitmaps[idx] = -1;
		Mission_icon_bitmaps[idx] = bm_load(Mission_icon_bitmap_filenames[idx]);
	}
}

void sim_room_unload_mission_icons()
{
	int idx;

	// unload all bitmaps
	for(idx=0; idx<NUM_MISSION_ICONS; idx++){
		if(Mission_icon_bitmaps[idx] >= 0){
			// don't bm_release() here, used in multiple places - taylor
			bm_unload(Mission_icon_bitmaps[idx]);
			Mission_icon_bitmaps[idx] = -1;
		}
	}
}
