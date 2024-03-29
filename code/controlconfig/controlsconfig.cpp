/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/ControlConfig/ControlsConfig.cpp $
 * $Revision: 2.18.2.1 $
 * $Date: 2007-12-28 02:10:35 $
 * $Author: Backslash $
 *
 * C module for keyboard, joystick and mouse configuration
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.18  2005/10/10 17:14:30  taylor
 * remove NO_NETWORK
 * fix controlconfig so that axes can be set to joystick/mouse using the mouse (required when no joystick attached)
 *
 * Revision 2.17  2005/07/22 10:18:37  Goober5000
 * CVS header tweaks
 * --Goober5000
 *
 * Revision 2.16  2005/07/13 02:50:50  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.15  2005/07/07 16:36:57  taylor
 * various compiler warning fixes (some of these from dizzy)
 *
 * Revision 2.14  2005/07/02 19:42:14  taylor
 * ton of non-standard resolution fixes
 *
 * Revision 2.13  2005/05/12 17:49:11  taylor
 * use vm_malloc(), vm_free(), vm_realloc(), vm_strdup() rather than system named macros
 *   fixes various problems and is past time to make the switch
 *
 * Revision 2.12  2005/04/28 05:29:28  wmcoolmon
 * Removed FS2_DEMO defines that looked like they wouldn't cause the universe to collapse
 *
 * Revision 2.11  2005/04/17 05:38:28  taylor
 * updated Linux joystick code that's a bit less insane speed wise
 * remove ability to build without joystick support, no reason to keep it around
 * fix unusable warning flag with libjpeg building
 *
 * Revision 2.10  2005/03/02 21:24:43  taylor
 * more network/inferno goodness for Windows, takes care of a few warnings too
 *
 * Revision 2.9  2004/07/29 19:37:50  Kazan
 * unbug the JS bug i caused --- kazan
 *
 * Revision 2.8  2004/07/26 20:47:26  Kazan
 * remove MCD complete
 *
 * Revision 2.7  2004/07/25 00:31:28  Kazan
 * i have absolutely nothing to say about that subject
 *
 * Revision 2.6  2004/07/12 16:32:43  Kazan
 * MCD - define _MCD_CHECK to use memory tracking
 *
 * Revision 2.5  2004/03/05 09:01:58  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.4  2003/11/11 02:15:43  Goober5000
 * ubercommit - basically spelling and language fixes with some additional
 * warnings disabled
 * --Goober5000
 *
 * Revision 2.3  2003/04/05 11:09:21  Goober5000
 * fixed some fiddly bits with scrolling and ui stuff
 * --Goober5000
 *
 * Revision 2.2  2003/03/18 10:07:00  unknownplayer
 * The big DX/main line merge. This has been uploaded to the main CVS since I can't manage to get it to upload to the DX branch. Apologies to all who may be affected adversely, but I'll work to debug it as fast as I can.
 *
 * Revision 2.1.2.1  2002/09/24 18:56:41  randomtiger
 * DX8 branch commit
 *
 * This is the scub of UP's previous code with the more up to date RT code.
 * For full details check previous dev e-mails
 *
 * Revision 2.1  2002/08/01 01:41:04  penguin
 * The big include file move
 *
 * Revision 2.0  2002/06/03 04:02:21  penguin
 * Warpcore CVS sync
 *
 * Revision 1.4  2002/05/16 06:06:23  mharris
 * ifndef NO_JOYSTICK
 *
 * Revision 1.3  2002/05/10 20:42:43  mharris
 * use "ifndef NO_NETWORK" all over the place
 *
 * Revision 1.2  2002/05/10 06:08:08  mharris
 * Porting... added ifndef NO_SOUND
 *
 * Revision 1.1  2002/05/02 18:03:04  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 13    10/14/99 2:50p Jefff
 * localization fixes
 * 
 * 12    8/16/99 9:50a Jefff
 * fixed loading of tab bitmaps
 * 
 * 11    8/11/99 3:21p Jefff
 * added tab highlights on conflict
 * 
 * 10    7/26/99 5:25p Jefff
 * removed invalidation of key binding for demo builds
 * 
 * 9     7/19/99 2:13p Dave
 * Added some new strings for Heiko.
 * 
 * 8     7/15/99 9:20a Andsager
 * FS2_DEMO initial checkin
 * 
 * 7     6/19/99 2:46p Dave
 * New control config screen.
 * 
 * 6     1/30/99 5:08p Dave
 * More new hi-res stuff.Support for nice D3D textures.
 * 
 * 5     1/15/99 11:29a Neilk
 * Fixed D3D screen/texture pixel formatting problem. 
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
 * 2     10/07/98 10:52a Dave
 * Initial checkin.
 * 
 * 1     10/07/98 10:48a Dave
 * 
 * 61    8/09/98 11:55a Lawrance
 * if GRAVIS_OEM is defined, map the throttle axis by default
 * 
 * 60    6/19/98 3:51p Lawrance
 * localize control text
 * 
 * 59    6/17/98 11:04a Lawrance
 * localize the control config strings
 * 
 * 58    6/13/98 5:19p Hoffoss
 * externalized control config texts.
 * 
 * 57    6/09/98 5:15p Lawrance
 * French/German localization
 * 
 * 56    6/09/98 10:31a Hoffoss
 * Created index numbers for all xstr() references.  Any new xstr() stuff
 * added from here on out should be added to the end if the list.  The
 * current list count can be found in FreeSpace.cpp (search for
 * XSTR_SIZE).
 * 
 * 55    6/01/98 11:43a John
 * JAS & MK:  Classified all strings for localization.
 * 
 * 54    5/26/98 11:10a Lawrance
 * Fix bug where window controls get disabled when F1 pressed twice
 * 
 * 53    5/20/98 10:35p Hoffoss
 * Fixed bug with mouse buttons not working when action isn't continuous.
 * 
 * 52    5/19/98 4:08p Allender
 * kill default binding for Z axis
 * 
 * 51    5/19/98 12:56p Hoffoss
 * Added some code to help prevent triple-clicking for binding the mouse
 * button to an action (why people triple click is beyond me,
 * though).
 * 
 * 50    5/19/98 11:11a Lawrance
 * Ensure X and Y axis have defaults!
 * 
 * 49    5/18/98 4:53p Hoffoss
 * Some force feedback tweaks and pilot initializations there should have
 * been happening, but weren't, and not are!
 * 
 * 48    5/18/98 10:15a Lawrance
 * Only do hud squad msg key check when necessary
 * 
 * 47    5/18/98 10:08a Lawrance
 * deal with overlap between hud squad msg number keys and CC_CONTINUOUS
 * keys
 * 
 * 46    5/17/98 5:44p Hoffoss
 * Made throttle never bound by default (ask Sandeep why if interested).
 * 
 * 45    5/14/98 5:32p Hoffoss
 * Improved axis binding code some more.
 * 
 * 44    5/13/98 7:15p Hoffoss
 * Fixed remaining bugs with axis binding.
 * 
 * 43    5/13/98 1:17a Hoffoss
 * Added joystick axes configurability.
 * 
 * 42    5/12/98 3:49p Hoffoss
 * Fixed bug where double mouse click would bind mouse button right away.
 * 
 * 41    5/11/98 5:43p Hoffoss
 * Made num lock not bindable.
 * 
 * 40    5/11/98 5:29p Hoffoss
 * Added mouse button mapped to joystick button support.
 * 
 * 39    5/07/98 6:25p Dave
 * Fix strange boundary conditions which arise when players die/respawn
 * while the game is being ended. Spiff up the chatbox doskey thing a bit.
 * 
 * 38    5/05/98 1:48a Lawrance
 * Add in missing help overlays
 * 
 * 37    4/27/98 10:11a Lawrance
 * Add in disabled beep for missing buttons
 * 
 * 36    4/25/98 2:59p Hoffoss
 * Fixed typo that was causing a bug.
 * 
 * 35    4/22/98 1:51a Lawrance
 * Take out multiplayer key from demo key config
 * 
 * 34    4/16/98 4:29p Hoffoss
 * Fixed bank_when_pressed functionality when using alt or shift for it.
 * 
 * 33    4/15/98 11:06a Lawrance
 * fix bug with a multi key showing up in demo, remove obsolete bindings
 * from demo and full version
 * 
 * 32    4/14/98 2:45p Hoffoss
 * Made hitting escape to exit screen not play failed sound.
 * 
 * 31    4/14/98 2:27p Hoffoss
 * Made certain actions be hidden in demo build.
 * 
 * 30    4/13/98 2:38p Hoffoss
 * Added a tooltip handler and make binding attempts with illegal keys
 * show a popup.
 * 
 * 29    4/11/98 7:59p Lawrance
 * Add support for help overlays
 * 
 * 28    4/09/98 4:12p Hoffoss
 * Changed check_control() to automatically register a control as used if
 * it detects it being used.
 * 
 * 27    4/08/98 11:11a Hoffoss
 * Fixed some bugs that showed up due to fixing other bugs the other day
 * with controls.
 * 
 * 26    4/07/98 3:47p Hoffoss
 * Fixed continuous controls checking with respect to modifiers.
 * 
 * 25    4/06/98 11:17a Hoffoss
 * Fixed num lock/pause interplay bug.
 * 
 * 24    4/03/98 3:51p Hoffoss
 * Fixed some bugs, and made changed Interplay requested regarding search
 * mode.
 * 
 * 23    3/31/98 4:12p Hoffoss
 * Made control used status clear at mission init time.
 * 
 * 22    3/23/98 11:28a Hoffoss
 * Fixed flashing question mark bug.
 * 
 * 21    3/21/98 11:30a John
 * Fixed bug where joymouse caused you to stay in binding mode when
 * binding joystick button 1 to something.
 * 
 * 20    3/20/98 3:37p Hoffoss
 * Tried to fix mitri's bug, failed miserably.
 * 
 * 19    3/19/98 5:04p Dave
 * Put in support for targeted multiplayer text and voice messaging (all,
 * friendly, hostile, individual).
 * 
 * 18    3/18/98 12:03p John
 * Marked all the new strings as externalized or not.
 * 
 * 17    3/18/98 10:16a Hoffoss
 * Fixed warning.
 * 
 * 16    3/17/98 11:15a Hoffoss
 * Made question mark that appears when you are in bind mode flash.
 * 
 * 15    3/17/98 10:48a Hoffoss
 * Allowed a special hack for "bank while pressed" action to use alt and
 * shift keys standalone.
 * 
 * 14    3/12/98 3:22p Hoffoss
 * Fixed 2 bugs with one solution!  Yay!  Failed sound on bind fixed and
 * pad enter now not translated to enter.
 * 
 * 13    3/11/98 5:28p Hoffoss
 * Added control config debug display info to possibly aid in tracking
 * down a bug.
 * 
 * 12    2/26/98 10:07p Hoffoss
 * Rewrote state saving and restoring to fix bugs and simplify the code.
 * 
 * 11    2/22/98 12:19p John
 * Externalized some strings
 * 
 * 10    2/20/98 3:39p Hoffoss
 * Updated code for new control config screen artwork.
 * 
 * 9     2/09/98 2:50p Hoffoss
 * Made 'none' show up as gray instead of normal color, to distinguish it
 * from actions with bound keys.
 * 
 * 8     2/07/98 10:04p Hoffoss
 * Changed color and placement of "more" indicator.
 * 
 * 7     2/05/98 10:42a Hoffoss
 * Fixed bug where while in bind mode, you could change the line selected
 * using the mouse, and binding would work on the new line instead.
 * 
 * 6     2/03/98 5:05p Hoffoss
 * Added "clear other" button to clear all conflicting bindings with
 * selected action.
 * 
 * 5     1/22/98 4:53p Hoffoss
 * Made training messages/directives display a joystick button in place of
 * a keypress if there is no keypress bound to the action.
 * 
 * 4     1/20/98 4:20p Hoffoss
 * Removed confusing behavior of clear button clearing out the other
 * binding in a conflict.
 * 
 * 3     1/08/98 12:11p Hoffoss
 * Changed Rudder axis to Roll axis, added new function we can use to
 * check what joystick axes are valid with.
 * 
 * 2     12/24/97 3:37p Hoffoss
 * Moved control config stuff to seperate library to Fred can access it as
 * well.
 * 
 * 1     12/24/97 11:58a Hoffoss
 * 
 * 98    12/22/97 2:15p Hoffoss
 * Fixed bug where joystick axis lines weren't being displayed.
 * 
 * 97    12/16/97 2:44p Hoffoss
 * Added clear button to control config screen.
 * 
 * 96    12/12/97 3:07p Hoffoss
 * Changed how deleting bindings work.  Each control of an action can be
 * deleted independently or both at once.
 * 
 * 95    12/07/97 2:36p John
 * Made warp out be Alt+J instead of J
 * 
 * 94    12/03/97 4:59p Hoffoss
 * Added reset sound and change control config sounds around.
 * 
 * 93    12/03/97 4:16p Hoffoss
 * Changed sound stuff used in interface screens for interface purposes.
 *
 * $NoKeywords: $
 *
*/


#include "freespace2/freespace.h"
#include "controlconfig/controlsconfig.h"
#include "gamesequence/gamesequence.h"
#include "hud/hudsquadmsg.h"
#include "io/key.h"
#include "io/timer.h"
#include "ui/ui.h"
#include "io/joy.h"
#include "gamesnd/gamesnd.h"
#include "missionui/missionscreencommon.h"
#include "graphics/font.h"
#include "playerman/managepilot.h"
#include "gamehelp/contexthelp.h"
#include "popup/popup.h"
#include "ui/uidefs.h"
#include "globalincs/alphacolors.h"
#include "network/multi_pmsg.h"
#include "network/multiutil.h"


#ifndef NDEBUG
#include "hud/hud.h"
#endif



#define NUM_SYSTEM_KEYS			14
#define NUM_BUTTONS				19
#define NUM_TABS					4

// coordinate indicies
#define CONTROL_X_COORD 0
#define CONTROL_Y_COORD 1
#define CONTROL_W_COORD 2
#define CONTROL_H_COORD 3

char* Conflict_background_bitmap_fname[GR_NUM_RESOLUTIONS] = {
	"ControlConfig",		// GR_640
	"2_ControlConfig"		// GR_1024
};

char* Conflict_background_bitmap_mask_fname[GR_NUM_RESOLUTIONS] = {
	"ControlConfig-m",		// GR_640
	"2_ControlConfig-m"		// GR_1024
};

// control list area
int Control_list_coords[GR_NUM_RESOLUTIONS][4] = {
	{
		32, 58, 198, 259			// GR_640
	},
	{
		32, 94, 904, 424			// GR_1024
	}
};

// width of the control name section of the list
int Control_list_ctrl_w[GR_NUM_RESOLUTIONS] = {
	350,		// GR_640
	600		// GR_1024
};

// x start position of the binding area section of the list
int Control_list_key_x[GR_NUM_RESOLUTIONS] = {
	397,		// GR_640
	712		// GR_1024
};

// width of the binding area section of the list
int Control_list_key_w[GR_NUM_RESOLUTIONS] = {
	198,		// GR_640
	230		// GR_1024
};

// display the "more..." text under the control list
int Control_more_coords[GR_NUM_RESOLUTIONS][2] = {
	{
		320, 326			// GR_640
	},
	{
		500, 542			// GR_1024
	}
};

// area to display "conflicts with..." text
int Conflict_wnd_coords[GR_NUM_RESOLUTIONS][4] = {
	{
		32, 313, 250, 32	// GR_640
	},
	{
		48, 508, 354, 46	// GR_1024
	}
};

// conflict warning anim coords
int Conflict_warning_coords[GR_NUM_RESOLUTIONS][2] = {
	{
		-1, 420			// GR_640
	},
	{
		-1, 669			// GR_1024
	}
};

// for flashing the conflict text
#define CONFLICT_FLASH_TIME	250
int Conflict_stamp = -1;
int Conflict_bright = 0;

#define LIST_BUTTONS_MAX	42
#define JOY_AXIS				0x80000

static int Num_cc_lines;
static struct {
	char *label;
	int cc_index;  // index into Control_config of item
	int y;  // Y coordinate of line
	int kx, kw, jx, jw;  // x start and width of keyboard and joystick bound text
} Cc_lines[CCFG_MAX];

// struct to hold backup config_item elements so we can undo them
struct config_item_undo {
	int size;
	int *index;  // array (size) of Control_config indices of replaced elements
	config_item *list;  // array (size) of original elements
	config_item_undo *next;
};

config_item Control_config_backup[CCFG_MAX];

int Axis_map_to[NUM_JOY_AXIS_ACTIONS] = { -1, -1, -1, -1, WII_STICK1_Y, WIIMOTE_YAW, WIIMOTE_PITCH, WIIMOTE_ROLL };
int Axis_map_to_defaults[NUM_JOY_AXIS_ACTIONS] = { -1, -1, -1, -1, WII_STICK1_Y, WIIMOTE_YAW, WIIMOTE_PITCH, WIIMOTE_ROLL };

// all this stuff is localized/externalized
#define NUM_MOUSE_TEXT			5
#define NUM_MOUSE_AXIS_TEXT	2
#define NUM_INVERT_TEXT			2	
char *Joy_axis_action_text[NUM_JOY_AXIS_ACTIONS];
char *Joy_axis_text[JOY_NUM_AXES];
char *Mouse_button_text[NUM_MOUSE_TEXT];
char *Mouse_axis_text[NUM_MOUSE_AXIS_TEXT];
char *Invert_text[NUM_INVERT_TEXT];

ubyte System_keys[NUM_SYSTEM_KEYS] = {
	KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
	KEY_F11, KEY_F12, KEY_PRINT_SCRN
};

int Control_check_count = 0;

static int Tab;  // which tab we are currently in
static int Binding_mode = 0;  // are we waiting for a key to bind it?
static int Bind_time = 0;
static int Search_mode = 0;  // are we waiting for a key to bind it?
static int Last_key = -1;
static int Selected_line = 0;  // line that is currently selected for binding
static int Selected_item = -1;  // -1 = none, 0 = key, 1 = button
static int Scroll_offset;
static int Axis_override = -1;
static int Background_bitmap;
static int Conflicts_tabs[NUM_TABS];
static UI_BUTTON List_buttons[LIST_BUTTONS_MAX];  // buttons for each line of text in list
static UI_WINDOW Ui_window;

static struct {
	int key;  // index of other control in conflict with this one
	int joy;  // index of other control in conflict with this one
} Conflicts[CCFG_MAX];

int Conflicts_axes[NUM_JOY_AXIS_ACTIONS];

#define TARGET_TAB				0
#define SHIP_TAB					1
#define WEAPON_TAB				2
#define COMPUTER_TAB				3
#define SCROLL_UP_BUTTON		4
#define SCROLL_DOWN_BUTTON		5
#define ALT_TOGGLE				6
#define SHIFT_TOGGLE				7
#define INVERT_AXIS				8
#define CANCEL_BUTTON			9
#define UNDO_BUTTON				10
#define RESET_BUTTON				11
#define SEARCH_MODE				12
#define BIND_BUTTON				13
#define HELP_BUTTON				14
#define ACCEPT_BUTTON			15
#define CLEAR_OTHER_BUTTON		16
#define CLEAR_ALL_BUTTON		17
#define CLEAR_BUTTON				18

ui_button_info CC_Buttons[GR_NUM_RESOLUTIONS][NUM_BUTTONS] = {
	{ // GR_640
		ui_button_info("CCB_00",	32,	348,	17,	384,	0),	// target tab
		ui_button_info("CCB_01",	101,	348,	103,	384,	1),	// ship tab
		ui_button_info("CCB_02",	173,	352,	154,	384,	2),	// weapon tab
		ui_button_info("CCB_03",	242,	347,	244,	384,	3),	// computer/misc tab
		ui_button_info("CCB_04",	614,	73,	-1,	-1,	4),	// scroll up
		ui_button_info("CCB_05",	614,	296,	-1,	-1,	5),	// scroll down
		ui_button_info("CCB_06",	17,	452,	12,	440,	6),	// alt toggle
		ui_button_info("CCB_07",	56,	452,	50,	440,	7),	// shift toggle
		ui_button_info("CCB_09",	162,	452,	155,	440,	9),	// invert
		ui_button_info("CCB_10",	404,	1,		397,	45,	10),	// cancel
		ui_button_info("CCB_11",	582,	347,	586,	386,	11),	// undo
		ui_button_info("CCB_12",	576,	1,		578,	45,	12),	// default
		ui_button_info("CCB_13",	457,	4,		453,	45,	13),	// search
		ui_button_info("CCB_14",	516,	4,		519,	45,	14),	// bind
		ui_button_info("CCB_15",	540,	428,	500,	440,	15),	// help
		ui_button_info("CCB_16",	574,	432,	571,	412,	16),	// accept
		ui_button_info("CCB_18",	420,	346,	417,	386,	18),	// clear other 
		ui_button_info("CCB_19",	476,	346,	474,	386,	19),	// clear all
		ui_button_info("CCB_20",	524,	346,	529,	386,	20),	// clear button
	},
	{ // GR_1024
		ui_button_info("2_CCB_00",	51,	557,	27,	615,	0),	// target tab
		ui_button_info("2_CCB_01",	162,	557,	166,	615,	1),	// ship tab
		ui_button_info("2_CCB_02",	277,	563,	246,	615,	2),	// weapon tab
		ui_button_info("2_CCB_03",	388,	555,	391,	615,	3),	// computer/misc tab
		ui_button_info("2_CCB_04",	982,	117,	-1,	-1,	4),	// scroll up
		ui_button_info("2_CCB_05",	982,	474,	-1,	-1,	5),	// scroll down
		ui_button_info("2_CCB_06",	28,	723,	24,	704,	6),	// alt toggle
		ui_button_info("2_CCB_07",	89,	723,	80,	704,	7),	// shift toggle
		ui_button_info("2_CCB_09",	260,	723,	249,	704,	9),	// invert
		ui_button_info("2_CCB_10",	646,	2,		635,	71,	10),	// cancel
		ui_button_info("2_CCB_11",	932,	555,	938,	619,	11),	// undo
		ui_button_info("2_CCB_12",	921,	1,		923,	71,	12),	// default
		ui_button_info("2_CCB_13",	732,	6,		726,	71,	13),	// search
		ui_button_info("2_CCB_14",	825,	6,		831,	71,	14),	// bind
		ui_button_info("2_CCB_15",	864,	685,	800,	704,	15),	// help
		ui_button_info("2_CCB_16",	919,	692,	914,	660,	16),	// accept
		ui_button_info("2_CCB_18",	672,	553,	668,	619,	18),	// clear other 
		ui_button_info("2_CCB_19",	761,	553,	749,	619,	19),	// clear all
		ui_button_info("2_CCB_20",	838,	553,	846,	619,	20),	// clear button
	}
};

// strings
#define CC_NUM_TEXT		20
UI_XSTR CC_text[GR_NUM_RESOLUTIONS][CC_NUM_TEXT] = {
	{ // GR_640
		{ "Targeting",		1340,		17,	384,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][TARGET_TAB].button },
		{ "Ship",			1341,		103,	384,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][SHIP_TAB].button },
		{ "Weapons",		1065,		154,	384,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][WEAPON_TAB].button },
		{ "Misc",			1411,		244,	384,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][COMPUTER_TAB].button },		
		{ "Alt",				1510,		12,	440,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][ALT_TOGGLE].button },
		{ "Shift",			1511,		50,	440,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][SHIFT_TOGGLE].button },
		{ "Invert",			1342,		155,	440,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][INVERT_AXIS].button },
		{ "Cancel",			641,		397,	45,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][CANCEL_BUTTON].button },
		{ "Undo",			1343,		586,	386,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][UNDO_BUTTON].button },
		{ "Defaults",		1344,		568,	45,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][RESET_BUTTON].button },
		{ "Search",			1345,		453,	45,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][SEARCH_MODE].button },
		{ "Bind",			1346,		519,	45,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][BIND_BUTTON].button },
		{ "Help",			928,		500,	440,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][HELP_BUTTON].button },
		{ "Accept",			1035,		571,	412,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][ACCEPT_BUTTON].button },
		{ "Clear",			1347,		417,	386,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][CLEAR_OTHER_BUTTON].button },
		{ "Conflict",		1348,		406,	396,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][CLEAR_OTHER_BUTTON].button },
		{ "Clear",			1413,		474,	386,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][CLEAR_ALL_BUTTON].button },
		{ "All",				1349,		483,	396,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][CLEAR_ALL_BUTTON].button },
		{ "Clear",			1414,		529,	388,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][CLEAR_BUTTON].button },
		{ "Selected",		1350,		517,	396,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][CLEAR_BUTTON].button },
	},
	{ // GR_1024
		{ "Targeting",		1340,		47,	615,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][TARGET_TAB].button },
		{ "Ship",			1341,		176,	615,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][SHIP_TAB].button },
		{ "Weapons",		1065,		266,	615,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][WEAPON_TAB].button },
		{ "Misc",			1411,		401,	615,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][COMPUTER_TAB].button },		
		{ "Alt",				1510,		29,	704,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][ALT_TOGGLE].button },
		{ "Shift",			1511,		85,	704,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][SHIFT_TOGGLE].button },
		{ "Invert",			1342,		254,	704,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][INVERT_AXIS].button },
		{ "Cancel",			641,		655,	71,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][CANCEL_BUTTON].button },
		{ "Undo",			1343,		938,	619,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][UNDO_BUTTON].button },
		{ "Defaults",		1344,		923,	71,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][RESET_BUTTON].button },
		{ "Search",			1345,		746,	71,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][SEARCH_MODE].button },
		{ "Bind",			1346,		846,	71,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][BIND_BUTTON].button },
		{ "Help",			928,		800,	704,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][HELP_BUTTON].button },
		{ "Accept",			1035,		914,	660,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][ACCEPT_BUTTON].button },
		{ "Clear",			1347,		683,	619,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][CLEAR_OTHER_BUTTON].button },
		{ "Conflict",		1348,		666,	634,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][CLEAR_OTHER_BUTTON].button },
		{ "Clear",			1413,		759,	619,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][CLEAR_ALL_BUTTON].button },
		{ "All",				1349,		772,	634,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][CLEAR_ALL_BUTTON].button },
		{ "Clear",			1414,		871,	619,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][CLEAR_BUTTON].button },
		{ "Selected",		1350,		852,	634,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][CLEAR_BUTTON].button },
	}
};

// linked list head of undo items
config_item_undo *Config_item_undo;

// same indices as Scan_code_text[].  Indicates if a scancode is allowed to be bound.
int Config_allowed[] = {
	0, 0, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,

	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 1, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 0, 1, 0, 1, 0, 1,
	1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
};


/*
// old invalid demo keys
#define INVALID_DEMO_KEYS_MAX	14
int Invalid_demo_keys[] = {
	INCREASE_SHIELD,
	DECREASE_SHIELD,
	SHIELD_EQUALIZE,
	SHIELD_XFER_TOP,
	SHIELD_XFER_BOTTOM,
	SHIELD_XFER_LEFT,
	SHIELD_XFER_RIGHT,
	XFER_SHIELD,
	XFER_LASER,
	MULTI_MESSAGE_ALL,
	MULTI_MESSAGE_FRIENDLY,
	MULTI_MESSAGE_HOSTILE,
	MULTI_MESSAGE_TARGET,
	MULTI_OBSERVER_ZOOM_TO
};
*/
#define INVALID_DEMO_KEYS_MAX	0
int Invalid_demo_keys[INVALID_DEMO_KEYS_MAX+1];		// +1 is only to prevent a 0-size array;


#ifndef NDEBUG
int Show_controls_info = 0;

DCF_BOOL(show_controls_info, Show_controls_info)
#endif

static int Axes_origin[JOY_NUM_AXES];

void control_config_detect_axis_reset()
{
	joystick_read_raw_axis(JOY_NUM_AXES, Axes_origin);
}

int control_config_detect_axis()
{
	int i, d, axis = -1, delta = 16384;
	int axes_values[JOY_NUM_AXES];
	int dx, dy, dz, fudge = 7;

	joystick_read_raw_axis(JOY_NUM_AXES, axes_values);
	for (i=0; i<JOY_NUM_AXES; i++) {
	
		switch(i)
		{
		// Always ignore g-force readings
		case WIIMOTE_GX:
		case WIIMOTE_GY:
		case WIIMOTE_GZ:
			continue;
		}
		if(Use_mouse_to_fly)
		{
			// Ignore wii-mote axis!
			switch(i)
			{
			case WIIMOTE_YAW:
			case WIIMOTE_PITCH:
				continue;
			}
		}
		d = abs(axes_values[i] - Axes_origin[i]);
		if (d > delta) {
			axis = i;
			delta = d;
		}
	}

	if ( (axis == -1) && Use_mouse_to_fly ) {
		mouse_get_delta( &dx, &dy, &dz );

		if ( (dx > fudge) || (dx < -fudge) ) {
			axis = WIIMOTE_YAW;
		} else if ( (dy > fudge) || (dy < -fudge) ) {
			axis = WIIMOTE_PITCH;
		} else if ( (dz > fudge) || (dz < -fudge) ) {
			axis = 2;
		}
	}
		
	return axis;
}

int control_config_valid_action(int n)
{
//WMC - Invalid_demo_keys is never even filled!!
/*
#ifdef FS2_DEMO
	int i;

	for (i=0; i<INVALID_DEMO_KEYS_MAX; i++)
		if (n == Invalid_demo_keys[i])
			return 0;
#endif
*/
	return 1;
}

void control_config_conflict_check()
{
	int i, j, a, b, c, shift = -1, alt = -1;

	for (i=0; i<CCFG_MAX; i++) {
		Conflicts[i].key = Conflicts[i].joy = -1;
		switch (Control_config[i].key_id) {
			case KEY_LSHIFT:
			case KEY_RSHIFT:
				shift = i;
				break;

			case KEY_LALT:
			case KEY_RALT:
				alt = i;
				break;
		}
	}

	for (i=0; i<NUM_TABS; i++)
		Conflicts_tabs[i] = 0;

	for (i=0; i<CCFG_MAX-1; i++) {
		if (control_config_valid_action(i)) {
			for (j=i+1; j<CCFG_MAX; j++) {
				if (control_config_valid_action(j)) {
					if (Control_config[i].key_id >= 0) {
						c = 0;
						a = Control_config[i].key_id;
						b = Control_config[j].key_id;
						if (a == b) {
							Conflicts[i].key = j;
							Conflicts[j].key = i;
							Conflicts_tabs[ Control_config[i].tab ] = 1;
							Conflicts_tabs[ Control_config[j].tab ] = 1;
						}

		/*				if ((a >= 0) && (a & KEY_SHIFTED) && (shift >= 0)) {
							Conflicts[i].key = shift;
							Conflicts[shift].key = i;
							Conflicts_tabs[ Control_config[i].tab ] = 1;
							Conflicts_tabs[ Control_config[shift].tab ] = 1;
						}

						if ((b >= 0) && (b & KEY_SHIFTED) && (shift >= 0)) {
							Conflicts[j].key = shift;
							Conflicts[shift].key = j;
							Conflicts_tabs[ Control_config[j].tab ] = 1;
							Conflicts_tabs[ Control_config[shift].tab ] = 1;
						}

						if ((a >= 0) && (a & KEY_ALTED) && (alt >= 0)) {
							Conflicts[i].key = alt;
							Conflicts[alt].key = i;
							Conflicts_tabs[ Control_config[i].tab ] = 1;
							Conflicts_tabs[ Control_config[alt].tab ] = 1;
						}

						if ((b >= 0) && (b & KEY_ALTED) && (alt >= 0)) {
							Conflicts[j].key = alt;
							Conflicts[alt].key = j;
							Conflicts_tabs[ Control_config[j].tab ] = 1;
							Conflicts_tabs[ Control_config[alt].tab ] = 1;
						}*/
					}

					if ((Control_config[i].joy_id >= 0) && (Control_config[i].joy_id == Control_config[j].joy_id)) {
						Conflicts[i].joy = j;
						Conflicts[j].joy = i;
						Conflicts_tabs[ Control_config[i].tab ] = 1;
						Conflicts_tabs[ Control_config[j].tab ] = 1;
					}
				}
			}
		}
	}

	for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++)
		Conflicts_axes[i] = -1;

	for (i=0; i<NUM_JOY_AXIS_ACTIONS-1; i++) {
		for (j=i+1; j<NUM_JOY_AXIS_ACTIONS; j++) {
			if ((Axis_map_to[i] >= 0) && (Axis_map_to[i] == Axis_map_to[j])) {
				Conflicts_axes[i] = j;
				Conflicts_axes[j] = i;
				Conflicts_tabs[SHIP_TAB] = 1;
 			}
		}
	}
}

// do list setup required prior to rendering and checking for the controls listing.  Called when list changes
void control_config_list_prepare()
{
	int j, k, y, z;
	int font_height = gr_get_font_height();

	Num_cc_lines = y = z = 0;
	while (z < CCFG_MAX) {
		if ((Control_config[z].tab == Tab) && control_config_valid_action(z)) {
			k = Control_config[z].key_id;
			j = Control_config[z].joy_id;

			if (Control_config[z].hasXSTR)
				Cc_lines[Num_cc_lines].label = XSTR(Control_config[z].text, CONTROL_CONFIG_XSTR + z);
			else
				Cc_lines[Num_cc_lines].label = Control_config[z].text;


			Cc_lines[Num_cc_lines].cc_index = z;
			Cc_lines[Num_cc_lines++].y = y;
			y += font_height + 2;
		}

		z++;
	}

	if (Tab == SHIP_TAB) {
		for (j=0; j<NUM_JOY_AXIS_ACTIONS; j++) {
			Cc_lines[Num_cc_lines].label = Joy_axis_action_text[j];
			Cc_lines[Num_cc_lines].cc_index = j | JOY_AXIS;
			Cc_lines[Num_cc_lines++].y = y;
			y += font_height + 2;
		}
	}
}

int cc_line_query_visible(int n)
{
	int y;

	if ((n < 0) || (n >= Num_cc_lines))
		return 0;
	
	y = Cc_lines[n].y - Cc_lines[Scroll_offset].y;
	if ((y < 0) || (y + gr_get_font_height() > Control_list_coords[gr_screen.res][CONTROL_H_COORD])){
		return 0;
	}

	return 1;
}

// allocates the required space for one undo block and put it in the beginning of the linked list (top of a stack).
// Returns a pointer to this newly allocated block
config_item_undo *get_undo_block(int size)
{
	config_item_undo *ptr;

	ptr = (config_item_undo *) vm_malloc( sizeof(config_item_undo) );
	Assert(ptr);
	ptr->next = Config_item_undo;
	Config_item_undo = ptr;

	ptr->size = size;
	if (size) {
		ptr->index = (int *) vm_malloc( sizeof(int) * size );
		Assert(ptr->index);
		ptr->list = (config_item *) vm_malloc( sizeof(config_item) * size );
		Assert(ptr->list);

	} else {
		ptr->index = NULL;
		ptr->list = NULL;
	}

	return ptr;
}

// frees one undo block.  The first one in the list (top of the stack) to be precise.
void free_undo_block()
{
	config_item_undo *ptr;

	ptr = Config_item_undo;
	if (!ptr)
		return;

	Config_item_undo = ptr->next;
	if (ptr->size) {
		vm_free(ptr->list);
		vm_free(ptr->index);
	}

	vm_free(ptr);
}

// undo the most recent binding changes
int control_config_undo_last()
{
	int i, z, tab;

	if (!Config_item_undo) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	if (Config_item_undo->index[0] & JOY_AXIS)
		tab = SHIP_TAB;
	else
		tab = Control_config[Config_item_undo->index[0]].tab;

	for (i=1; i<Config_item_undo->size; i++) {
		if (Config_item_undo->index[i] & JOY_AXIS) {
			if (tab != SHIP_TAB)
				tab = -1;

		} else {
			if (Control_config[Config_item_undo->index[i]].tab != tab)
				tab = -1;
		}
	}

	if (tab >= 0)
		Tab = tab;

	for (i=0; i<Config_item_undo->size; i++) {
		z = Config_item_undo->index[i];
		if (z & JOY_AXIS) {
			config_item *ptr;

			z &= ~JOY_AXIS;
			ptr = &Config_item_undo->list[i];
			Axis_map_to[z] = ptr->joy_id;
			Invert_axis[z] = ptr->used;

		} else {
			Control_config[z] = Config_item_undo->list[i];
		}
	}

	free_undo_block();
	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_USER_SELECT);
	return 0;
}

void control_config_save_axis_undo(int axis)
{
	config_item_undo *ptr;
	config_item item;

	memset( &item, 0, sizeof(config_item) );

	item.joy_id = (short) Axis_map_to[axis];
	item.used = Invert_axis[axis];

	ptr = get_undo_block(1);
	ptr->index[0] = axis | JOY_AXIS;
	ptr->list[0] = item;
}

void control_config_bind_key(int i, int key)
{
	config_item_undo *ptr;

	ptr = get_undo_block(1);
	ptr->index[0] = i;
	ptr->list[0] = Control_config[i];
	Control_config[i].key_id = (short) key;
}

void control_config_bind_joy(int i, int joy)
{
	config_item_undo *ptr;

	ptr = get_undo_block(1);
	ptr->index[0] = i;
	ptr->list[0] = Control_config[i];
	Control_config[i].joy_id = (short) joy;
}

void control_config_bind_axis(int i, int axis)
{
	control_config_save_axis_undo(i);
	Axis_map_to[i] = axis;
}

int control_config_remove_binding()
{
	int z, j, k;
	config_item_undo *ptr;

	if (Selected_line < 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	z = Cc_lines[Selected_line].cc_index;
	if (z & JOY_AXIS) {
		z &= ~JOY_AXIS;
		if (Axis_map_to[z] < 0) {
			gamesnd_play_iface(SND_GENERAL_FAIL);
			return -1;
		}

		control_config_save_axis_undo(z);
		Axis_map_to[z] = -1;
		control_config_conflict_check();
		control_config_list_prepare();
		gamesnd_play_iface(SND_USER_SELECT);
		Selected_item = -1;
		return 0;
	}

	if ((Control_config[z].joy_id < 0) && (Control_config[z].key_id < 0)) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	j = k = -1;
	ptr = get_undo_block(1);
	ptr->index[0] = z;
	ptr->list[0] = Control_config[z];

	if (Selected_item && (Control_config[z].joy_id >= 0))  // if not just key selected (which would be 0)
		Control_config[z].joy_id = (short) -1;

	if ((Selected_item != 1) && (Control_config[z].key_id >= 0))  // if not just joy button selected (1)
		Control_config[z].key_id = (short) -1;

	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_USER_SELECT);
	Selected_item = -1;
	return 0;
}

int control_config_clear_other()
{
	int z, i, j, total = 0;
	config_item_undo *ptr;

	if (Selected_line < 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	z = Cc_lines[Selected_line].cc_index;
	if (z & JOY_AXIS) {
		config_item item;

		z &= ~JOY_AXIS;
		if (Axis_map_to[z] < 0) {
			gamesnd_play_iface(SND_GENERAL_FAIL);
			return -1;
		}

		for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++)
			if ((Axis_map_to[i] == Axis_map_to[z]) && (i != z))
				total++;

		if (!total) {
			gamesnd_play_iface(SND_GENERAL_FAIL);
			return -1;
		}

		ptr = get_undo_block(total);
		for (i=j=0; i<NUM_JOY_AXIS_ACTIONS; i++)
			if ((Axis_map_to[i] == Axis_map_to[z]) && (i != z)) {
				memset( &item, 0, sizeof(config_item) );

				item.joy_id = (short) Axis_map_to[i];
				item.used = Invert_axis[i];

				ptr->index[j] = i | JOY_AXIS;
				ptr->list[j] = item;
				j++;

				Axis_map_to[i] = -1;
			}

		control_config_conflict_check();
		control_config_list_prepare();
		gamesnd_play_iface(SND_USER_SELECT);
		return 0;
	}

	for (i=0; i<CCFG_MAX; i++)
		if ( (Control_config[i].key_id == Control_config[z].key_id) || (Control_config[i].joy_id == Control_config[z].joy_id) )
			if (i != z)
				total++;

	if (!total) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	if ((Control_config[z].joy_id < 0) && (Control_config[z].key_id < 0)) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	// now, back up the old bindings so we can undo if we want to
	ptr = get_undo_block(total);
	for (i=j=0; i<CCFG_MAX; i++)
		if ( (Control_config[i].key_id == Control_config[z].key_id) || (Control_config[i].joy_id == Control_config[z].joy_id) )
			if (i != z) {
				ptr->index[j] = i;
				ptr->list[j] = Control_config[i];
				j++;

				if (Control_config[i].key_id == Control_config[z].key_id)
					Control_config[i].key_id = (short) -1;
				if (Control_config[i].joy_id == Control_config[z].joy_id)
					Control_config[i].joy_id = (short) -1;
			}

	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_USER_SELECT);
	return 0;
}

int control_config_clear_all()
{
	int i, j, total = 0;
	config_item_undo *ptr;

	// first, determine how many bindings need to be changed
	for (i=0; i<CCFG_MAX; i++)
		if ((Control_config[i].key_id >= 0) || (Control_config[i].joy_id >= 0))
			total++;

	if (!total) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	// now, back up the old bindings so we can undo if we want to
	ptr = get_undo_block(total);
	for (i=j=0; i<CCFG_MAX; i++) {
		if ((Control_config[i].key_id >= 0) || (Control_config[i].joy_id >= 0)) {
			ptr->index[j] = i;
			ptr->list[j] = Control_config[i];
			j++;
		}
	}

	Assert(j == total);
	for (i=0; i<CCFG_MAX; i++) {
		Control_config[i].key_id = Control_config[i].joy_id = -1;
	}

	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_RESET_PRESSED);
	return 0;
}

extern Joy_info joystick;

int control_config_axis_default(int axis)
{
	Assert(axis >= 0);

	if ( axis > 1 ) {
		if (Axis_map_to_defaults[axis] < 0)
			return -1;

		if (!joystick.axis_valid[Axis_map_to_defaults[axis]])
			return -1;
	}

	return Axis_map_to_defaults[axis];
}

int control_config_do_reset()
{
	int i, j, total = 0;
	config_item_undo *ptr;
	config_item item;

	// first, determine how many bindings need to be changed
	for (i=0; i<CCFG_MAX; i++)
		if ((Control_config[i].key_id != Control_config[i].key_default) || (Control_config[i].joy_id != Control_config[i].joy_default))
			total++;

	for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++)
		if ((Axis_map_to[i] != control_config_axis_default(i)) || (Invert_axis[i] != Invert_axis_defaults[i]))
			total++;

	if (!total) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	// now, back up the old bindings so we can undo if we want to
	ptr = get_undo_block(total);
	for (i=j=0; i<CCFG_MAX; i++) {
		if ((Control_config[i].key_id != Control_config[i].key_default) || (Control_config[i].joy_id != Control_config[i].joy_default)) {
			ptr->index[j] = i;
			ptr->list[j] = Control_config[i];
			j++;
		}
	}

	for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++)
		if ((Axis_map_to[i] != control_config_axis_default(i)) || (Invert_axis[i] != Invert_axis_defaults[i])) {
			memset( &item, 0, sizeof(config_item) );

			item.joy_id = (short) Axis_map_to[i];
			item.used = Invert_axis[i];

			ptr->index[j] = i | JOY_AXIS;
			ptr->list[j] = item;
			j++;
		}

	Assert(j == total);
	control_config_reset_defaults();
	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_RESET_PRESSED);
	return 0;
}

// This sets all the controls to their default values
void control_config_reset_defaults()
{
	int i;

	// Reset keyboard defaults
	for (i=0; i<CCFG_MAX; i++) {
		Control_config[i].key_id = Control_config[i].key_default;
		Control_config[i].joy_id = Control_config[i].joy_default;
	}

	for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++) {
		Axis_map_to[i] = control_config_axis_default(i);
		Invert_axis[i] = Invert_axis_defaults[i];
	}
}

void control_config_scroll_screen_up()
{
	if (Scroll_offset) {
		Scroll_offset--;
		Assert(Selected_line > Scroll_offset);
		while (!cc_line_query_visible(Selected_line))
			Selected_line--;

		Selected_item = -1;
		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

void control_config_scroll_line_up()
{
	if (Selected_line) {
		Selected_line--;
		if (Selected_line < Scroll_offset)
			Scroll_offset = Selected_line;

		Selected_item = -1;
		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

void control_config_scroll_screen_down()
{
	if (Cc_lines[Num_cc_lines - 1].y + gr_get_font_height() > Cc_lines[Scroll_offset].y + Control_list_coords[gr_screen.res][CONTROL_H_COORD]) {
		Scroll_offset++;
		while (!cc_line_query_visible(Selected_line)) {
			Selected_line++;
			Assert(Selected_line < Num_cc_lines);
		}

		Selected_item = -1;
		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

void control_config_scroll_line_down()
{
	if (Selected_line < Num_cc_lines - 1) {
		Selected_line++;
		Assert(Selected_line > Scroll_offset);
		while (!cc_line_query_visible(Selected_line))
			Scroll_offset++;

		Selected_item = -1;
		gamesnd_play_iface(SND_SCROLL);

	} else
		gamesnd_play_iface(SND_GENERAL_FAIL);
}

void control_config_toggle_modifier(int bit)
{
	int k, z;

	z = Cc_lines[Selected_line].cc_index;
	Assert(!(z & JOY_AXIS));
	k = Control_config[z].key_id;
	if (k < 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	control_config_bind_key(z, k ^ bit);
	control_config_conflict_check();
	gamesnd_play_iface(SND_USER_SELECT);
}

void control_config_toggle_invert()
{
	int z;

	z = Cc_lines[Selected_line].cc_index;
	Assert(z & JOY_AXIS);
	z &= ~JOY_AXIS;
	control_config_save_axis_undo(z);
	Invert_axis[z] = !Invert_axis[z];
}

void control_config_do_bind()
{
	int i;

	game_flush();
//	if ((Selected_line < 0) || (Cc_lines[Selected_line].cc_index & JOY_AXIS)) {
	if (Selected_line < 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	for (i=0; i<NUM_BUTTONS; i++)
		if (i != CANCEL_BUTTON) {
			CC_Buttons[gr_screen.res][i].button.reset_status();
			CC_Buttons[gr_screen.res][i].button.disable();
		}

	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.enable();
	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.set_hotkey(KEY_ESC);

	for (i=0; i<JOY_TOTAL_BUTTONS; i++){
		joy_down_count(i, 1);  // clear checking status of all joystick buttons
	}

	control_config_detect_axis_reset();

	Binding_mode = 1;
	Bind_time = timer_get_milliseconds();
	Search_mode = 0;
	Last_key = -1;
	Axis_override = -1;
	gamesnd_play_iface(SND_USER_SELECT);
}

void control_config_do_search()
{
	int i;

	for (i=0; i<NUM_BUTTONS; i++){
		if (i != CANCEL_BUTTON) {
			CC_Buttons[gr_screen.res][i].button.reset_status();
			CC_Buttons[gr_screen.res][i].button.disable();
		}
	}

	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.enable();
	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.set_hotkey(KEY_ESC);

	for (i=0; i<JOY_TOTAL_BUTTONS; i++){
		joy_down_count(i, 1);  // clear checking status of all joystick buttons
	}

	Binding_mode = 0;
	Search_mode = 1;
	Last_key = -1;
	gamesnd_play_iface(SND_USER_SELECT);
}

void control_config_do_cancel(int fail = 0)
{
	int i;

	game_flush();

	for (i=0; i<NUM_BUTTONS; i++){
		if ( (i != CANCEL_BUTTON) && (i != INVERT_AXIS) ){
			CC_Buttons[gr_screen.res][i].button.enable();
		}
	}

	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.reset_status();
	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.disable();
	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.set_hotkey(-1);
	CC_Buttons[gr_screen.res][BIND_BUTTON].button.reset_status();
	CC_Buttons[gr_screen.res][SEARCH_MODE].button.reset_status();

	Binding_mode = Search_mode = 0;
	if (fail){
		gamesnd_play_iface(SND_GENERAL_FAIL);
	} else {
		gamesnd_play_iface(SND_USER_SELECT);
	}
}

int control_config_accept()
{
	int i;

	for (i=0; i<NUM_TABS; i++)
		if (Conflicts_tabs[i])
			break;

	if (i < NUM_TABS) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	hud_squadmsg_save_keys();  // rebuild map for saving/restoring keys in squadmsg mode
	gameseq_post_event(GS_EVENT_PREVIOUS_STATE);
	gamesnd_play_iface(SND_COMMIT_PRESSED);
	return 0;
}

void control_config_cancel_exit()
{
	int i;

	for (i=0; i<CCFG_MAX; i++)
		Control_config[i] = Control_config_backup[i];

	gameseq_post_event(GS_EVENT_PREVIOUS_STATE);
}

void control_config_button_pressed(int n)
{
	switch (n) {
		case TARGET_TAB:
		case SHIP_TAB:
		case WEAPON_TAB:
		case COMPUTER_TAB:
			Tab = n;
			Scroll_offset = Selected_line = 0;
			control_config_list_prepare();
			gamesnd_play_iface(SND_SCREEN_MODE_PRESSED);
			break;

		case BIND_BUTTON:
			control_config_do_bind();
			break;

		case SEARCH_MODE:
			control_config_do_search();
			break;

		case SHIFT_TOGGLE:
			control_config_toggle_modifier(KEY_SHIFTED);
			gamesnd_play_iface(SND_USER_SELECT);
			break;

		case ALT_TOGGLE:
			control_config_toggle_modifier(KEY_ALTED);
			gamesnd_play_iface(SND_USER_SELECT);
			break;

		case INVERT_AXIS:
			control_config_toggle_invert();
			gamesnd_play_iface(SND_USER_SELECT);
			break;

		case SCROLL_UP_BUTTON:
			control_config_scroll_screen_up();
			break;

		case SCROLL_DOWN_BUTTON:
			control_config_scroll_screen_down();
			break;

		case ACCEPT_BUTTON:
			control_config_accept();
			break;

		case CLEAR_BUTTON:
			control_config_remove_binding();
			break;

		case HELP_BUTTON:
			launch_context_help();
			gamesnd_play_iface(SND_HELP_PRESSED);
			break;

		case RESET_BUTTON:
			control_config_do_reset();
			break;

		case UNDO_BUTTON:
			control_config_undo_last();
			break;

		case CANCEL_BUTTON:
			control_config_do_cancel();
			break;

		case CLEAR_OTHER_BUTTON:
			control_config_clear_other();
			break;

		case CLEAR_ALL_BUTTON:
			control_config_clear_all();
			break;		
	}
}

char *control_config_tooltip_handler(char *str)
{
	int i;

	if (!stricmp(str, NOX("@conflict"))) {
		for (i=0; i<NUM_TABS; i++) {
			if (Conflicts_tabs[i])
				return XSTR( "Conflict!", 205);
		}
	}

	return NULL;
}

void control_config_init()
{
	int i;
	ui_button_info *b;

	// make backup of all controls
	for (i=0; i<CCFG_MAX; i++)
		Control_config_backup[i] = Control_config[i];

	common_set_interface_palette(NOX("ControlConfigPalette"));  // set the interface palette
	Ui_window.create(0, 0, gr_screen.max_w_unscaled, gr_screen.max_h_unscaled, 0);
	Ui_window.set_mask_bmap(Conflict_background_bitmap_mask_fname[gr_screen.res]);
	Ui_window.tooltip_handler = control_config_tooltip_handler;

	// load in help overlay bitmap	
	help_overlay_load(CONTROL_CONFIG_OVERLAY);
	help_overlay_set_state(CONTROL_CONFIG_OVERLAY,0);

	// reset conflict flashing
	Conflict_stamp = -1;

	for (i=0; i<NUM_BUTTONS; i++) {
		b = &CC_Buttons[gr_screen.res][i];

		if (b->hotspot < 0) {  // temporary
			b->button.create(&Ui_window, NOX("Clear other"), b->x, b->y, 150, 30, 0, 1);  // temporary
			b->button.set_highlight_action(common_play_highlight_sound);
			continue;
		}

		b->button.create(&Ui_window, "", b->x, b->y, 60, 30, ((i == SCROLL_UP_BUTTON) || (i == SCROLL_DOWN_BUTTON)), 1);

		// set up callback for when a mouse first goes over a button
		b->button.set_highlight_action(common_play_highlight_sound);		
		if (i<4) {
			b->button.set_bmaps(b->filename, 5, 1);		// a bit of a hack here, but buttons 0-3 need 4 frames loaded
		} else {
			b->button.set_bmaps(b->filename);
		}
		b->button.link_hotspot(b->hotspot);
	}	

	// create all text
	for(i=0; i<CC_NUM_TEXT; i++){
		Ui_window.add_XSTR(&CC_text[gr_screen.res][i]);
	}

	for (i=0; i<LIST_BUTTONS_MAX; i++) {
		List_buttons[i].create(&Ui_window, "", 0, 0, 60, 30, 0, 1);
		List_buttons[i].hide();
		List_buttons[i].disable();
	}

	// set up hotkeys for buttons so we draw the correct animation frame when a key is pressed
	CC_Buttons[gr_screen.res][SCROLL_UP_BUTTON].button.set_hotkey(KEY_PAGEUP);
	CC_Buttons[gr_screen.res][SCROLL_DOWN_BUTTON].button.set_hotkey(KEY_PAGEDOWN);
	CC_Buttons[gr_screen.res][BIND_BUTTON].button.set_hotkey(KEY_ENTER);
	CC_Buttons[gr_screen.res][CLEAR_OTHER_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_DELETE);
	CC_Buttons[gr_screen.res][UNDO_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_Z);
	CC_Buttons[gr_screen.res][CLEAR_BUTTON].button.set_hotkey(KEY_DELETE);
	CC_Buttons[gr_screen.res][ACCEPT_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_ENTER);
	CC_Buttons[gr_screen.res][HELP_BUTTON].button.set_hotkey(KEY_F1);
	CC_Buttons[gr_screen.res][RESET_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_R);
	CC_Buttons[gr_screen.res][INVERT_AXIS].button.set_hotkey(KEY_I);

	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.disable();
	CC_Buttons[gr_screen.res][CLEAR_OTHER_BUTTON].button.disable();

	Background_bitmap = bm_load(Conflict_background_bitmap_fname[gr_screen.res]);	

	Scroll_offset = Selected_line = 0;
	Config_item_undo = NULL;
	control_config_conflict_check();

	// setup strings	
	Joy_axis_action_text[JOY_HEADING_AXIS] = vm_strdup(XSTR("Turn (Yaw) Axis", 1016));
	Joy_axis_action_text[JOY_PITCH_AXIS] = vm_strdup(XSTR("Pitch Axis", 1017));
	Joy_axis_action_text[JOY_BANK_AXIS] = vm_strdup(XSTR("Bank Axis", 1018));
	Joy_axis_action_text[JOY_ABS_THROTTLE_AXIS] = vm_strdup(XSTR("Absolute Throttle Axis", 1019));
	Joy_axis_action_text[JOY_REL_THROTTLE_AXIS] = vm_strdup(XSTR("Relative Throttle Axis", 1020));
	Joy_axis_action_text[JOY_REL_HEADING_AXIS] = vm_strdup("Relative Turn (Yaw) Axis");
	Joy_axis_action_text[JOY_REL_PITCH_AXIS] = vm_strdup("Relative Pitch Axis Axis");
	Joy_axis_action_text[JOY_REL_BANK_AXIS] = vm_strdup("Relative Bank Axis Axis");
	Joy_axis_text[WIIMOTE_YAW] = vm_strdup("Wiimote yaw");
	Joy_axis_text[WIIMOTE_PITCH] = vm_strdup("Wiimote pitch");
	Joy_axis_text[WIIMOTE_ROLL] = vm_strdup("Wiimote roll");
	Joy_axis_text[WIIMOTE_GX] = vm_strdup("Wiimote x-accel");
	Joy_axis_text[WIIMOTE_GY] = vm_strdup("Wiimote y-accel");
	Joy_axis_text[WIIMOTE_GZ] = vm_strdup("Wiimote z-accel");
	Joy_axis_text[WII_STICK1_X] = vm_strdup("Stick 1 x");
	Joy_axis_text[WII_STICK1_Y] = vm_strdup("Stick 1 y");
	Joy_axis_text[WII_STICK2_X] = vm_strdup("Stick 2 x");
	Joy_axis_text[WII_STICK2_Y] = vm_strdup("Stick 2 y");
	Joy_axis_text[WII_R_SHOUDLER] = vm_strdup("Right shoulder");
	Joy_axis_text[WII_L_SHOUDLER] = vm_strdup("Left shoulder");
	Mouse_button_text[0] = vm_strdup("");
	Mouse_button_text[1] = vm_strdup(XSTR("Left Button", 1027));
	Mouse_button_text[2] = vm_strdup(XSTR("Right Button", 1028));
	Mouse_button_text[3] = vm_strdup(XSTR("Mid Button", 1029));
	Mouse_button_text[4] = vm_strdup("");
	Mouse_axis_text[0] = vm_strdup(XSTR("L/R", 1030));
	Mouse_axis_text[1] = vm_strdup(XSTR("U/B", 1031));
	Invert_text[0] = vm_strdup(XSTR("N", 1032));
	Invert_text[1] = vm_strdup(XSTR("Y", 1033));

	control_config_list_prepare();
}

void control_config_close()
{
	int idx;
	
	while (Config_item_undo){
		free_undo_block();
	}

	// unload the overlay bitmap
	help_overlay_unload(CONTROL_CONFIG_OVERLAY);
	
	if (Background_bitmap){
		bm_release(Background_bitmap);
	}

	Ui_window.destroy();
	common_free_interface_palette();		// restore game palette
	hud_squadmsg_save_keys();				// rebuild map for saving/restoring keys in squadmsg mode
	game_flush();
	write_pilot_file();

	// free strings	
	for(idx=0; idx<NUM_JOY_AXIS_ACTIONS; idx++){
		if(Joy_axis_action_text[idx] != NULL){
			vm_free(Joy_axis_action_text[idx]);
			Joy_axis_action_text[idx] = NULL;
		}
	}
	for(idx=0; idx<JOY_NUM_AXES; idx++){
		if(Joy_axis_text[idx] != NULL){
			vm_free(Joy_axis_text[idx]);
			Joy_axis_text[idx] = NULL;
		}
	}
	for(idx=0; idx<NUM_MOUSE_TEXT; idx++){
		if(Mouse_button_text[idx] != NULL){
			vm_free(Mouse_button_text[idx]);
			Mouse_button_text[idx] = NULL;
		}
	}
	for(idx=0; idx<NUM_MOUSE_AXIS_TEXT; idx++){
		if(Mouse_axis_text[idx] != NULL){
			vm_free(Mouse_axis_text[idx]);
			Mouse_axis_text[idx] = NULL;
		}
	}
	for(idx=0; idx<NUM_INVERT_TEXT; idx++){
		if(Invert_text[idx] != NULL){
			vm_free(Invert_text[idx]);
			Invert_text[idx] = NULL;
		}
	}
}

void control_config_do_frame(float frametime)
{
	char buf[256], *str, *jptr;
	int i, j, k, w, x, y, z, len, line, conflict;
	int font_height = gr_get_font_height();
	int select_tease_line = -1;  // line mouse is down on, but won't be selected until button released
	static float timer = 0.0f;
	color *c;
	static int bound_timestamp = 0;
	static char bound_string[40];
	
	timer += frametime;

	if (Binding_mode) {
		if (Cc_lines[Selected_line].cc_index & JOY_AXIS) {
			int bind = 0;

			z = Cc_lines[Selected_line].cc_index & ~JOY_AXIS;
			i = control_config_detect_axis();
			if (i >= 0) {
				Axis_override = i;
				bind = 1;
			}

			k = game_poll();
			Ui_window.use_hack_to_get_around_stupid_problem_flag = 1;
			Ui_window.process(0);

			if (k == KEY_ESC) {
				strcpy(bound_string, XSTR( "Canceled", 206));
				bound_timestamp = timestamp(2500);
				control_config_do_cancel();

			} else {
				if (k == KEY_ENTER)
					bind = 1;

				for (i=0; i<JOY_TOTAL_BUTTONS; i++)
					if (joy_down_count(i, 1))
						bind = 1;

				if (bind) {
					if (Axis_override >= 0) {
						control_config_bind_axis(z, Axis_override);
						strcpy(bound_string, Joy_axis_text[Axis_override]);
						gr_force_fit_string(bound_string, 39, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
						bound_timestamp = timestamp(2500);
						control_config_conflict_check();
						control_config_list_prepare();
						control_config_do_cancel();

					} else {
						control_config_do_cancel(1);
					}
				}
			}

		} else {
			if (help_overlay_active(CONTROL_CONFIG_OVERLAY)) {
				CC_Buttons[gr_screen.res][HELP_BUTTON].button.reset_status();
				Ui_window.set_ignore_gadgets(1);
			}

			k = game_poll();
			Ui_window.use_hack_to_get_around_stupid_problem_flag = 1;
			Ui_window.process(0);

			if ( (k > 0) || B1_JUST_RELEASED ) {
				if (help_overlay_active(CONTROL_CONFIG_OVERLAY)) {
					help_overlay_set_state(CONTROL_CONFIG_OVERLAY, 0);
					Ui_window.set_ignore_gadgets(0);
					k = 0;
				}
			}

			if ( !help_overlay_active(CONTROL_CONFIG_OVERLAY) ) {
				Ui_window.set_ignore_gadgets(0);
			}

			if (k == KEY_ESC) {
				strcpy(bound_string, XSTR( "Canceled", 206));
				bound_timestamp = timestamp(2500);
				control_config_do_cancel();

			} else {
				switch (k & KEY_MASK) {
					case KEY_LSHIFT:
					case KEY_RSHIFT:
					case KEY_LALT:
					case KEY_RALT:
						Last_key = k & KEY_MASK;
						k = 0;
						break;
				}

				if (Cc_lines[Selected_line].cc_index == BANK_WHEN_PRESSED || /*Cc_lines[Selected_line].cc_index == SLIDE_WHEN_PRESSED ||*/ Cc_lines[Selected_line].cc_index == GLIDE_WHEN_PRESSED)  // a special hack just for Mike K.
					if ( (Last_key >= 0) && (k <= 0) && !keyd_pressed[Last_key] )	//Backslash - ok it's not just for Mike K. any more :-P  but seriously, this is so we can bind such controls to Shift or Alt.  Does this actually get used?
						k = Last_key;

				if ((k > 0) && !Config_allowed[k & KEY_MASK]) {
					popup(0, 1, POPUP_OK, XSTR( "That is a non-bindable key.  Please try again.", 207));
					k = 0;
				}

				k &= (KEY_MASK | KEY_SHIFTED | KEY_ALTED);
				if (k > 0) {
					z = Cc_lines[Selected_line].cc_index;
					Assert(!(z & JOY_AXIS));
					control_config_bind_key(z, k);

					strcpy(bound_string, textify_scancode(k));
					gr_force_fit_string(bound_string, 39, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
					bound_timestamp = timestamp(2500);
					control_config_conflict_check();
					control_config_list_prepare();
					control_config_do_cancel();
				}

				for (i=0; i<JOY_TOTAL_BUTTONS; i++)
					if (joy_down_count(i, 1)) {
						z = Cc_lines[Selected_line].cc_index;
						Assert(!(z & JOY_AXIS));
						control_config_bind_joy(z, i);

						strcpy(bound_string, Joy_button_text[i]);
						gr_force_fit_string(bound_string, 39, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
						bound_timestamp = timestamp(2500);
						control_config_conflict_check();
						control_config_list_prepare();
						control_config_do_cancel();
						break;
					}

				if (Bind_time + 375 < timer_get_milliseconds()) {
					for (i=0; i<NUM_BUTTONS; i++){
						if ( (CC_Buttons[gr_screen.res][i].button.is_mouse_on()) && (CC_Buttons[gr_screen.res][i].button.enabled()) ){
							break;
						}
					}

					if (i == NUM_BUTTONS) {  // no buttons pressed
						for (i=0; i<MOUSE_NUM_BUTTONS; i++)
							if (mouse_down(1 << i)) {
								z = Cc_lines[Selected_line].cc_index;
								Assert(!(z & JOY_AXIS));
								control_config_bind_joy(z, i);

								strcpy(bound_string, Joy_button_text[i]);
								gr_force_fit_string(bound_string, 39, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
								bound_timestamp = timestamp(2500);
								control_config_conflict_check();
								control_config_list_prepare();
								control_config_do_cancel();
								for (j=0; j<NUM_BUTTONS; j++){
									CC_Buttons[gr_screen.res][j].button.reset();
								}

								break;
							}
					}
				}
			}
		}

	} else if (Search_mode) {
		if (help_overlay_active(CONTROL_CONFIG_OVERLAY)) {
			CC_Buttons[gr_screen.res][HELP_BUTTON].button.reset_status();
			Ui_window.set_ignore_gadgets(1);
		}

		k = game_poll();
		Ui_window.use_hack_to_get_around_stupid_problem_flag = 1;
		Ui_window.process(0);

		if ( (k > 0) || B1_JUST_RELEASED ) {
			if ( help_overlay_active(CONTROL_CONFIG_OVERLAY) ) {
				help_overlay_set_state(CONTROL_CONFIG_OVERLAY, 0);
				Ui_window.set_ignore_gadgets(0);
				k = 0;
			}
		}

		if ( !help_overlay_active(CONTROL_CONFIG_OVERLAY) ) {
			Ui_window.set_ignore_gadgets(0);
		}

		if (k == KEY_ESC) {
			control_config_do_cancel();

		} else {
			if ((k > 0) && !Config_allowed[k & KEY_MASK])
				k = 0;

			k &= (KEY_MASK | KEY_SHIFTED | KEY_ALTED);
			z = -1;
			if (k > 0) {
				for (i=0; i<CCFG_MAX; i++)
					if (Control_config[i].key_id == k) {
						z = i;
						break;
					}
			}

			for (i=0; i<JOY_TOTAL_BUTTONS; i++)
				if (joy_down_count(i, 1)) {
					j = i;
					for (i=0; i<CCFG_MAX; i++)
						if (Control_config[i].joy_id == j) {
							z = i;
							break;
						}

					break;
				}

			// check if not on enabled button
			for (j=0; j<NUM_BUTTONS; j++){
				if ( (CC_Buttons[gr_screen.res][j].button.is_mouse_on()) && (CC_Buttons[gr_screen.res][j].button.enabled()) ){
					break;
				}
			}

			if (j == NUM_BUTTONS) {  // no buttons pressed
				for (j=0; j<MOUSE_NUM_BUTTONS; j++)
					if (mouse_down(1 << j)) {
						for (i=0; i<CCFG_MAX; i++)
							if (Control_config[i].joy_id == j) {
								z = i;
								for (j=0; j<NUM_BUTTONS; j++){
									CC_Buttons[gr_screen.res][j].button.reset();
								}
								break;
							}

						break;
					}
			}

			if (z >= 0) {
				Tab = Control_config[z].tab;
				control_config_list_prepare();
				Selected_line = Scroll_offset = 0;
				for (i=0; i<Num_cc_lines; i++)
					if (Cc_lines[i].cc_index == z) {
						Selected_line = i;
						break;
					}

				while (!cc_line_query_visible(Selected_line)) {
					Scroll_offset++;
					Assert(Scroll_offset < Num_cc_lines);
				}
			}
		}

	} else {
		z = Cc_lines[Selected_line].cc_index & JOY_AXIS;
		CC_Buttons[gr_screen.res][ALT_TOGGLE].button.enable(!z);
		CC_Buttons[gr_screen.res][SHIFT_TOGGLE].button.enable(!z);
		CC_Buttons[gr_screen.res][INVERT_AXIS].button.enable(z);

		if (!z) {
			z = Cc_lines[Selected_line].cc_index;
			k = Control_config[z].key_id;
			if ( (k == KEY_LALT) || (k == KEY_RALT) || (k == KEY_LSHIFT) || (k == KEY_RSHIFT) ) {
				CC_Buttons[gr_screen.res][ALT_TOGGLE].button.enable(0);
				CC_Buttons[gr_screen.res][SHIFT_TOGGLE].button.enable(0);
			}
		}

		CC_Buttons[gr_screen.res][UNDO_BUTTON].button.enable(Config_item_undo != NULL);

		if ( help_overlay_active(CONTROL_CONFIG_OVERLAY) ) {
			CC_Buttons[gr_screen.res][HELP_BUTTON].button.reset_status();
			Ui_window.set_ignore_gadgets(1);
		}

		k = Ui_window.process();

		if ( (k > 0) || B1_JUST_RELEASED ) {
			if ( help_overlay_active(CONTROL_CONFIG_OVERLAY) ) {
				help_overlay_set_state(CONTROL_CONFIG_OVERLAY, 0);
				Ui_window.set_ignore_gadgets(0);
				k = 0;
			}
		}

		if ( !help_overlay_active(CONTROL_CONFIG_OVERLAY) ) {
			Ui_window.set_ignore_gadgets(0);
		}

		switch (k) {
			case KEY_DOWN:  // select next line
				control_config_scroll_line_down();
				break;

			case KEY_UP:  // select previous line
				control_config_scroll_line_up();
				break;

			case KEY_SHIFTED | KEY_TAB:  // activate previous tab
				Tab--;
				if (Tab < 0)
					Tab = NUM_TABS - 1;

				Scroll_offset = Selected_line = 0;
				control_config_list_prepare();
				gamesnd_play_iface(SND_SCREEN_MODE_PRESSED);
				break;

			case KEY_TAB:  // activate next tab
				Tab++;
				if (Tab >= NUM_TABS)
					Tab = 0;

				Scroll_offset = Selected_line = 0;
				control_config_list_prepare();
				gamesnd_play_iface(SND_SCREEN_MODE_PRESSED);
				break;

			case KEY_LEFT:
				Selected_item--;
				if (Selected_item == -2) {
					Selected_item = 1;
					if (Cc_lines[Selected_line].jw < 1) {
						Selected_item = 0;
						if (Cc_lines[Selected_line].kw < 1)
							Selected_item = -1;
					}
				}

				gamesnd_play_iface(SND_SCROLL);
				break;

			case KEY_RIGHT:
				Selected_item++;
				if ((Selected_item == 1) && (Cc_lines[Selected_line].jw < 1))
					Selected_item = -1;
				else if (!Selected_item && (Cc_lines[Selected_line].kw < 1))
					Selected_item = -1;
				else if (Selected_item > 1)
					Selected_item = -1;

				gamesnd_play_iface(SND_SCROLL);
				break;

			case KEY_BACKSP:  // undo
				control_config_undo_last();
				break;

			case KEY_ESC:
				control_config_cancel_exit();
				break;
		}	// end switch
	}

	for (i=0; i<NUM_BUTTONS; i++){
		if (CC_Buttons[gr_screen.res][i].button.pressed()){
			control_config_button_pressed(i);
		}
	}

	for (i=0; i<LIST_BUTTONS_MAX; i++) {
		if (List_buttons[i].is_mouse_on())
			select_tease_line = i + Scroll_offset;
	
		if (List_buttons[i].pressed()) {
			Selected_line = i + Scroll_offset;
			Selected_item = -1;
			List_buttons[i].get_mouse_pos(&x, &y);
			if ((x >= Cc_lines[Selected_line].kx) && (x < Cc_lines[Selected_line].kx + Cc_lines[Selected_line].kw))
				Selected_item = 0;

			if ((x >= Cc_lines[Selected_line].jx) && (x < Cc_lines[Selected_line].jx + Cc_lines[Selected_line].jw))
				Selected_item = 1;

			gamesnd_play_iface(SND_USER_SELECT);
		}

		if (List_buttons[i].double_clicked())
			control_config_do_bind();
	}

	GR_MAYBE_CLEAR_RES(Background_bitmap);
	if (Background_bitmap >= 0) {
		gr_set_bitmap(Background_bitmap);
		gr_bitmap(0, 0);
	} 

	// highlight tab with conflict
	Ui_window.draw();
	for (i=z=0; i<NUM_TABS; i++) {
		if (Conflicts_tabs[i]) {
			CC_Buttons[gr_screen.res][i].button.draw_forced(4);
			z++;
		}
	}

	if (z) {
		// maybe switch from bright to normal
		if((Conflict_stamp == -1) || timestamp_elapsed(Conflict_stamp)){
			Conflict_bright = !Conflict_bright;

			Conflict_stamp = timestamp(CONFLICT_FLASH_TIME);
		}

		// set color and font
		gr_set_font(FONT2);
		if(Conflict_bright){
			gr_set_color_fast(&Color_bright_red);
		} else {
			gr_set_color_fast(&Color_red);
		}

		// setup the conflict string
		char conflict_str[512] = "";
		strncpy(conflict_str, XSTR("Conflict!", 205), 511);
		int sw, sh;
		gr_get_string_size(&sw, &sh, conflict_str);

		gr_string((gr_screen.max_w / 2) - (sw / 2), Conflict_warning_coords[gr_screen.res][1], conflict_str);

		gr_set_font(FONT1);
	} else {
		// might as well always reset the conflict stamp
		Conflict_stamp = -1;
	}

	for (i=0; i<NUM_TABS; i++) {
		if (CC_Buttons[gr_screen.res][i].button.button_down()) {
			break;
		}
	}

	if (i == NUM_TABS) {
		CC_Buttons[gr_screen.res][Tab].button.draw_forced(2);
	}

	if (Search_mode) {
		CC_Buttons[gr_screen.res][SEARCH_MODE].button.draw_forced(2);
	}

	if (Selected_line >= 0) {
		z = Cc_lines[Selected_line].cc_index;
		if (z & JOY_AXIS) {
			if (Invert_axis[z & ~JOY_AXIS]) {
				CC_Buttons[gr_screen.res][INVERT_AXIS].button.draw_forced(2);
			}

		} else {
			z = Control_config[z].key_id;
			if (z >= 0) {
				if (z & KEY_SHIFTED) {
					CC_Buttons[gr_screen.res][SHIFT_TOGGLE].button.draw_forced(2);
				}
				if (z & KEY_ALTED) {
					CC_Buttons[gr_screen.res][ALT_TOGGLE].button.draw_forced(2);
				}
			}
		}
	}

	if (Binding_mode) {
		CC_Buttons[gr_screen.res][BIND_BUTTON].button.draw_forced(2);
	}

	z = Cc_lines[Selected_line].cc_index;
	x = Conflict_wnd_coords[gr_screen.res][CONTROL_X_COORD] + Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD] / 2;
	y = Conflict_wnd_coords[gr_screen.res][CONTROL_Y_COORD] + Conflict_wnd_coords[gr_screen.res][CONTROL_H_COORD] / 2;
	if (Binding_mode) {
		int t;

		t = (int) (timer * 3);
		if (t % 2) {
			gr_set_color_fast(&Color_text_normal);
			gr_get_string_size(&w, NULL, XSTR( "?", 208));
			gr_printf(x - w / 2, y - font_height / 2, XSTR( "?", 208));
		}

	} else if (!(z & JOY_AXIS) && ((Conflicts[z].key >= 0) || (Conflicts[z].joy >= 0))) {
		i = Conflicts[z].key;
		if (i < 0)
			i = Conflicts[z].joy;

		gr_set_color_fast(&Color_text_normal);
		str = XSTR( "Control conflicts with:", 209);
		gr_get_string_size(&w, NULL, str);
		gr_printf(x - w / 2, y - font_height, str);

		if (Control_config[i].hasXSTR)
			strcpy(buf, XSTR(Control_config[i].text, CONTROL_CONFIG_XSTR + i));
		else
			strcpy(buf, Control_config[i].text);

		gr_force_fit_string(buf, 255, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
		gr_get_string_size(&w, NULL, buf);
		gr_printf(x - w / 2, y, buf);

	} else if (*bound_string) {
		gr_set_color_fast(&Color_text_normal);
		gr_get_string_size(&w, NULL, bound_string);
		gr_printf(x - w / 2, y - font_height / 2, bound_string);
		if (timestamp_elapsed(bound_timestamp))
			*bound_string = 0;
	}

//	gr_set_color_fast(&Color_text_heading);
//	gr_printf(LIST_X + 20, HEADING_Y, Heading[Tab]);

//	gr_get_string_size(&w, &h, Heading[Tab]);
//	y = HEADING_Y + h / 2 - 1;
//	gr_line(LIST_X, y, LIST_X + 18, y);
//	gr_line(LIST_X + w + 21, y, LIST_X + LIST_W, y);

	if (Cc_lines[Num_cc_lines - 1].y + font_height > Cc_lines[Scroll_offset].y + Control_list_coords[gr_screen.res][CONTROL_H_COORD]) {
		gr_set_color_fast(&Color_white);
		gr_printf(Control_more_coords[gr_screen.res][CONTROL_X_COORD], Control_more_coords[gr_screen.res][CONTROL_Y_COORD], XSTR( "More...", 210));
	}

	conflict = 0;
	line = Scroll_offset;
	while (cc_line_query_visible(line)) {
		z = Cc_lines[line].cc_index;
		y = Control_list_coords[gr_screen.res][CONTROL_Y_COORD] + Cc_lines[line].y - Cc_lines[Scroll_offset].y;

		List_buttons[line - Scroll_offset].update_dimensions(Control_list_coords[gr_screen.res][CONTROL_X_COORD], y, Control_list_coords[gr_screen.res][CONTROL_W_COORD], font_height);
		List_buttons[line - Scroll_offset].enable(!Binding_mode);

		Cc_lines[line].kw = Cc_lines[line].jw = 0;

		if (line == Selected_line){
			c = &Color_text_selected;
		} else if (line == select_tease_line) {
			c = &Color_text_subselected;
		} else {
			c = &Color_text_normal;
		}

		gr_set_color_fast(c);
		if (Cc_lines[line].label) {
			strcpy(buf, Cc_lines[line].label);
			gr_force_fit_string(buf, 255, Control_list_ctrl_w[gr_screen.res]);
			gr_printf(Control_list_coords[gr_screen.res][CONTROL_X_COORD], y, buf);
		}

		if (!(z & JOY_AXIS)) {
			k = Control_config[z].key_id;
			j = Control_config[z].joy_id;
			x = Control_list_key_x[gr_screen.res];
			jptr = NULL;
			*buf = 0;

			if ((k < 0) && (j < 0)) {
				gr_set_color_fast(&Color_grey);
				gr_printf(x, y, XSTR( "None", 211));

			} else {
				if (k >= 0) {
					strcpy(buf, textify_scancode(k));
					if (Conflicts[z].key >= 0) {
						if (c == &Color_text_normal)
							gr_set_color_fast(&Color_text_error);
						else {
							gr_set_color_fast(&Color_text_error_hi);
							conflict++;
						}

					} else if (Selected_item == 1) {
						gr_set_color_fast(&Color_text_normal);

					} else
						gr_set_color_fast(c);

					gr_printf(x, y, buf);

					len = strlen(buf);
					Cc_lines[line].kx = x - Control_list_coords[gr_screen.res][CONTROL_X_COORD];
					gr_get_string_size(&w, NULL, buf);
					Cc_lines[line].kw = w;
					x += w;

					if (j >= 0) {
						gr_set_color_fast(&Color_text_normal);
						gr_printf(x, y, XSTR( ", ", 212));
						gr_get_string_size(&w, NULL, XSTR( ", ", 212));
						x += w;
					}
				}

				if (j >= 0) {
					strcpy(buf, Joy_button_text[j]);
					if (Conflicts[z].joy >= 0) {
						if (c == &Color_text_normal)
							gr_set_color_fast(&Color_text_error);
						else {
							gr_set_color_fast(&Color_text_error_hi);
							conflict++;
						}

					} else if (!Selected_item) {
						gr_set_color_fast(&Color_text_normal);

					} else
						gr_set_color_fast(c);

					gr_force_fit_string(buf, 255, Control_list_key_w[gr_screen.res] + Control_list_key_x[gr_screen.res] - x);
					gr_printf(x, y, buf);

					Cc_lines[line].jx = x - Control_list_coords[gr_screen.res][CONTROL_X_COORD];
					gr_get_string_size(&Cc_lines[line].jw, NULL, buf);
				}
			}

		} else {
			x = Control_list_key_x[gr_screen.res];
			j = Axis_map_to[z & ~JOY_AXIS];
			if (Binding_mode && (line == Selected_line))
				j = Axis_override;

			if (j < 0) {
				gr_set_color_fast(&Color_grey);
				gr_printf(x, y, XSTR( "None", 211));

			} else {
				if (Conflicts_axes[z & ~JOY_AXIS] >= 0) {
					if (c == &Color_text_normal)
						gr_set_color_fast(&Color_text_error);

					else {
						gr_set_color_fast(&Color_text_error_hi);
						conflict++;
					}

				} else if (!Selected_item) {
					gr_set_color_fast(&Color_text_normal);

				} else
					gr_set_color_fast(c);

				gr_string(x, y, Joy_axis_text[j]);
			}
		}

		line++;
	}

	CC_Buttons[gr_screen.res][CLEAR_OTHER_BUTTON].button.enable(conflict);

	i = line - Scroll_offset;
	while (i < LIST_BUTTONS_MAX)
		List_buttons[i++].disable();

	// blit help overlay if active
	help_overlay_maybe_blit(CONTROL_CONFIG_OVERLAY);

	gr_flip();
}

void clear_key_binding(short key)
{
	int i;

	for (i=0; i<CCFG_MAX; i++) {
		if (Control_config[i].key_id == key)
			Control_config[i].key_id = -1;
	}
}

float check_control_timef(int id)
{
	float t1, t2;

	// if type isn't continuous, we shouldn't be using this function, cause it won't work.
	Assert(Control_config[id].type == CC_TYPE_CONTINUOUS);

	// first, see if control actually used (makes sure modifiers match as well)
	if (!check_control(id))
		return 0.0f;

	t1 = key_down_timef(Control_config[id].key_id);
	if (t1)
		control_used(id);

	t2 = joy_down_time(Control_config[id].joy_id);
	if (t2)
		control_used(id);

	if (t1 + t2)
		return t1 + t2;

	return 1.0f;
}

void control_check_indicate()
{
#ifndef NDEBUG
	if (Show_controls_info) {
		gr_set_color_fast(&HUD_color_debug);
		gr_printf(490, 15, NOX("Ctrls checked: %d"), Control_check_count);
	}
#endif

	Control_check_count = 0;
}

int check_control(int id, int key)
{
	int z, mask;
	static int last_key = 0;

	Control_check_count++;
	if (key < 0)
		key = last_key;

	last_key = key;

	// if we're in multiplayer text enter (for chat) mode, check to see if we should ignore controls
	if ((Game_mode & GM_MULTIPLAYER) && multi_ignore_controls()){
		return 0;
	}

	if (Control_config[id].type == CC_TYPE_CONTINUOUS) {
		if (joy_down(Control_config[id].joy_id) || joy_down_count(Control_config[id].joy_id, 1)) {
			control_used(id);
			return 1;
		}

		if ((Control_config[id].joy_id >= 0) && (Control_config[id].joy_id < MOUSE_NUM_BUTTONS))
			if (mouse_down(1 << Control_config[id].joy_id) || mouse_down_count(1 << Control_config[id].joy_id)) {
				control_used(id);
				return 1;
			}

		// check what current modifiers are pressed
		mask = 0;
		if (keyd_pressed[KEY_LSHIFT] || key_down_count(KEY_LSHIFT) || keyd_pressed[KEY_RSHIFT] || key_down_count(KEY_RSHIFT))
			mask |= KEY_SHIFTED;

		if (keyd_pressed[KEY_LALT] || key_down_count(KEY_LALT) || keyd_pressed[KEY_RALT] || key_down_count(KEY_RALT))
			mask |= KEY_ALTED;

		z = Control_config[id].key_id;
		if (z >= 0) {
			if ( (z != KEY_LALT) && (z != KEY_RALT) && (z != KEY_LSHIFT) && (z != KEY_RSHIFT) ) {
				// if current modifiers don't match action's modifiers, don't register control active.
				if ((z & (KEY_SHIFTED | KEY_ALTED)) != mask)
					return 0;
			}

			z &= KEY_MASK;

			if (keyd_pressed[z] || key_down_count(z)) {
				if ( !hud_squadmsg_read_key(z) ) {
					control_used(id);
					return 1;
				}
			}
		}

		return 0;
	}

	if ((Control_config[id].key_id == key) || joy_down_count(Control_config[id].joy_id, 1) || mouse_down_count(1 << Control_config[id].joy_id)) {
		//mprintf(("Key used %d", key));
		control_used(id);
		return 1;
	}

	return 0;
}

// get heading, pitch, bank, throttle abs. and throttle rel. values.
void control_get_axes_readings(int *values)
{
	int axes_values[JOY_NUM_AXES];

	joystick_read_raw_axis(JOY_NUM_AXES, axes_values);
	
	for(size_t i = 0; i < NUM_JOY_AXIS_ACTIONS; ++i)
	{
		values[i] = 0;
		
		//	joy_get_scaled_reading will return a value represents the joystick pos from -1 to +1 (fixed point)
		if (Axis_map_to[i] >= 0)
			values[i] = joy_get_scaled_reading(axes_values[Axis_map_to[i]], Axis_map_to[i]);
			
		if (Invert_axis[i])
			values[i] *= -1;
	}

	return;
}

void control_used(int id)
{
	Control_config[id].used = timestamp();
}

void control_config_clear_used_status()
{
	int i;

	for (i=0; i<CCFG_MAX; i++)
		Control_config[i].used = 0;
}

void control_config_clear()
{
	int i;

	// Reset keyboard defaults
	for (i=0; i<CCFG_MAX; i++)
		Control_config[i].key_id = Control_config[i].joy_id = -1;
}

int control_config_handle_conflict()
{/*
	if ((Selected_item == -1) && ((Conflicts[z].key >= 0) || (Conflicts[z].joy >= 0))) {  // we are deleting a conflict
		j = Conflicts[z].joy;
		if ((j >= 0) && (Control_config[j].joy_id < 0))
			j = -1;

		k = Conflicts[z].key;
		if ((k >= 0) && (Control_config[k].key_id < 0))
			k = -1;

		if ((j >= 0) && (k >= 0) && (j != k)) {  // deleting 2 conflicts, each in different actions
			ptr = get_undo_block(2);
			ptr->index[0] = j;
			ptr->list[0] = Control_config[j];
			Control_config[j].joy_id = (short) -1;

			ptr->index[1] = k;
			ptr->list[1] = Control_config[k];
			Control_config[k].key_id = (short) -1;

		} else {  // only 1 action in conflict with selected action (might be both controls, though)
			z = j;
			if (j < 0)
				z = k;

			Assert(z >= 0);
			ptr = get_undo_block(1);
			ptr->index[0] = z;
			ptr->list[0] = Control_config[z];

			if (j >= 0)
				Control_config[z].joy_id = (short) -1;

			if (k >= 0)
				Control_config[z].key_id = (short) -1;
		}*/

	return 0;
}
