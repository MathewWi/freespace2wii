/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/

/*
 * $Logfile: /Freespace2/code/Fred2/Management.cpp $
 * $Revision: 1.12.2.23 $
 * $Date: 2007-11-22 04:31:12 $
 * $Author: taylor $
 *
 * This file handles the management of Objects, Ships, Wings, etc.  Basically
 * all the little structures we have that usually inter-relate that need to
 * be handled in a standard way, and thus should be handled by a single
 * function.
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.12.2.22  2007/11/21 07:27:45  Goober5000
 * add Wing Commander Saga's fiction viewer
 *
 * Revision 1.12.2.21  2007/10/12 18:57:16  karajorma
 * I meant to remove this ages ago!
 *
 * Revision 1.12.2.20  2007/09/02 02:07:40  Goober5000
 * added fixes for #1415 and #1483, made sure every read_file_text had a corresponding setjmp, and sync'd the parse error messages between HEAD and stable
 *
 * Revision 1.12.2.19  2007/07/28 21:31:05  Goober5000
 * this should really be capitalized
 *
 * Revision 1.12.2.18  2007/07/28 04:43:43  Goober5000
 * a couple of tweaks
 *
 * Revision 1.12.2.17  2007/05/28 18:27:33  wmcoolmon
 * Added armor support for asteroid, debris, ship, and beam damage
 *
 * Revision 1.12.2.16  2007/04/17 17:15:31  karajorma
 * Remove the -fredhtl command line.
 *
 * Revision 1.12.2.15  2007/02/20 04:19:09  Goober5000
 * the great big duplicate model removal commit
 *
 * Revision 1.12.2.14  2007/02/12 00:24:33  taylor
 * add back special "var" setting for flag_def_list and convert some of the lists to use it (more on the way)
 *
 * Revision 1.12.2.13  2007/02/11 09:24:08  taylor
 * remove -pcx32 and -jpgtga
 *
 * Revision 1.12.2.12  2007/02/09 05:40:40  Goober5000
 * merge the voice acting manager into the 3.6.9 (3.6.10?) branch
 *
 * Revision 1.12.2.11  2007/02/08 07:39:34  Goober5000
 * fix two bugs:
 * --default ship flags in the iff_defs table were not correctly translated from parse flags to ship/object flags
 * --ships were created with default allowed comm orders regardless of which team they were on
 *
 * Revision 1.12.2.10  2006/12/08 19:18:54  Goober5000
 * fix Mantis #1163
 *
 * Revision 1.12.2.9  2006/11/15 00:38:02  taylor
 * various bits of little cleanup
 * add the debug cmdline printer to FRED, so that we can make use of that info in the FRED-debug case too
 *
 * Revision 1.12.2.8  2006/10/27 06:34:40  taylor
 * fix compiler problem due to change in parse_init() function
 * fix dos EOL chars
 *
 * Revision 1.12.2.7  2006/10/24 13:44:54  taylor
 * add envmap selection to background editor
 * change skybox selection to be a text entry or browse instead of only text entry
 * allow envmap selected in background editor to be used by FRED
 * make texture replacement work in FRED for ships (Mantis bug #1068)
 *
 * Revision 1.12.2.6  2006/10/06 13:41:41  Goober5000
 * <nelson> Ha, ha! </nelson>
 *
 * Revision 1.12.2.5  2006/10/06 09:44:32  taylor
 * make sure that the armor stuff gets initted (should fix Mantis bug #1080)
 *
 * Revision 1.12.2.4  2006/08/27 18:12:41  taylor
 * make Species_info[] and Asteroid_info[] dynamic
 *
 * Revision 1.12.2.3  2006/08/06 18:47:12  Goober5000
 * add the multiple background feature
 * --Goober5000
 *
 * Revision 1.12.2.2  2006/07/10 21:47:17  taylor
 * fix undo system
 * restore the two autosave points that got removed for some reason (yes, these are needed for things to work properly)
 *
 * Revision 1.12.2.1  2006/07/08 19:36:03  Goober5000
 * iff defs should allow specification of both flag fields
 * --Goober5000
 *
 * Revision 1.12  2006/05/30 02:13:22  Goober5000
 * add substitute music boxes to FRED, and reset music properly when mission is cleared
 * --Goober5000
 *
 * Revision 1.11  2006/04/20 06:32:01  Goober5000
 * proper capitalization according to Volition
 *
 * Revision 1.10  2006/04/14 18:44:16  taylor
 * remove all of the *_ex() parsing functions added for use by EFFs
 * add a pause/unpause for parsing so that we can safely start parsing something new then continue parsing something old
 * make Mission_text and Mission_text_raw only use the memory needed, and free it when it doesn't need to parse anymore
 *   (should work ok with FRED2, but I wasn't able to test it)
 *
 * Revision 1.9  2006/04/12 05:07:00  phreak
 * obj_delete() needs to be called for jump nodes since the slot the jump node
 * that used to reside in hasn't been marked as empty.  This leads to the node still appearing in fred
 * if the user tries to delete the "ghosted" jump node, fred crashes.
 *
 * Revision 1.8  2006/03/18 22:18:24  Goober5000
 * blast
 *
 * Revision 1.7  2006/03/18 22:00:43  Goober5000
 * fix comm order initialization bug
 * --Goober5000
 *
 * Revision 1.6  2006/02/26 05:32:55  Goober5000
 * fix stupid bug
 *
 * Revision 1.5  2006/02/20 02:13:07  Goober5000
 * added ai-ignore-new which hopefully should fix the ignore bug
 * --Goober5000
 *
 * Revision 1.4  2006/02/11 02:58:23  Goober5000
 * yet more various and sundry fixes
 * --Goober5000
 *
 * Revision 1.3  2006/01/30 06:27:59  taylor
 * dynamic starfield bitmaps
 *
 * Revision 1.2  2006/01/26 04:01:58  Goober5000
 * spelling
 *
 * Revision 1.1  2006/01/19 02:27:31  Goober5000
 * import FRED2 back into fs2_open module
 * --Goober5000
 *
 * Revision 1.48  2006/01/16 00:34:34  Goober5000
 * final (I hope) IFF fixage involving the FRED dialogs :p
 * --Goober5000
 *
 * Revision 1.47  2006/01/15 23:58:31  Goober5000
 * small fix
 *
 * Revision 1.46  2006/01/15 23:51:42  Goober5000
 * oops
 *
 * Revision 1.45  2006/01/14 23:49:01  Goober5000
 * second pass; all the errors are fixed now; one more thing to take care of
 * --Goober5000
 *
 * Revision 1.44  2006/01/14 05:24:18  Goober5000
 * first pass at converting FRED to use new IFF stuff
 * --Goober5000
 *
 * Revision 1.43  2005/12/29 08:21:00  wmcoolmon
 * No my widdle FRED, I didn't forget about you ^_^ (codebase commit)
 *
 * Revision 1.42  2005/11/24 07:47:55  taylor
 * be sure to set up default ai_profile when loading a blank mission
 *
 * Revision 1.41  2005/11/22 09:07:59  Goober5000
 * fred will now load and save ai profiles
 * --Goober5000
 *
 * Revision 1.40  2005/10/29 23:02:33  Goober5000
 * partial FRED commit of changes
 * --Goober5000
 *
 * Revision 1.39  2005/10/21 23:44:21  phreak
 * preload briefing icons, otherwise they're not drawn
 *
 * Revision 1.38  2005/09/29 06:16:02  Goober5000
 * bah
 * --Goober5000
 *
 * Revision 1.37  2005/09/29 05:18:59  Goober5000
 * the FRED stuff
 * --Goober5000
 *
 * Revision 1.36  2005/09/26 07:10:20  Goober5000
 * fix a small bug with custom wing names
 * --Goober5000
 *
 * Revision 1.35  2005/09/26 03:37:35  Goober5000
 * species upgrade stuff for FRED
 * --Goober5000
 *
 * Revision 1.34  2005/09/01 03:26:51  phreak
 * Popup warning if -fredhtl isn't in the cmdline.  This will warn users that viewing high-poly models
 * may crash fred.
 *
 * Revision 1.33  2005/08/23 07:28:02  Goober5000
 * grammar
 * --Goober5000
 *
 * Revision 1.32  2005/08/23 06:45:14  Goober5000
 * fix parse initialization
 * --Goober5000
 *
 * Revision 1.31  2005/07/25 06:51:33  Goober5000
 * mission flag changes for FRED
 * --Goober5000
 *
 * Revision 1.30  2005/07/13 01:03:10  Goober5000
 * improved species support and removed need for #define
 * --Goober5000
 *
 * Revision 1.29  2005/06/29 18:56:12  taylor
 * make sure gamesnd gets initted properly to fix "within 0 and 0" error
 *
 * Revision 1.28  2005/04/13 20:11:06  chhogg
 * Changed vector to vec3d for Fred.
 *
 * Revision 1.26  2005/03/29 03:43:11  phreak
 * ai directory fixes as well as fixes for the new jump node code
 *
 * Revision 1.25  2005/03/27 12:59:13  Goober5000
 * stuff for FRED
 * --Goober5000
 *
 * Revision 1.24  2005/03/19 23:36:25  phreak
 * CString::GetBuffer() requires an integer parameter
 *
 * Revision 1.23  2005/03/06 23:19:27  wmcoolmon
 * Fixed the last of the jump node errors
 *
 * Revision 1.22  2005/03/06 22:41:57  wmcoolmon
 * Fixx0red some of the jump node errors
 *
 * Revision 1.21  2005/01/29 05:40:27  Goober5000
 * regular docking should work again in FRED now (multiple docking still isn't done yet)
 * --Goober5000
 *
 * Revision 1.20  2004/12/15 19:16:13  Goober5000
 * FRED code for custom wing names
 * --Goober5000
 *
 * Revision 1.19  2004/10/12 07:45:30  Goober5000
 * added contrail speed threshold
 * --Goober5000
 *
 * Revision 1.18  2004/09/17 07:56:51  Goober5000
 * bunch of FRED tweaks and fixes
 * --Goober5000
 *
 * Revision 1.17  2004/07/29 00:18:43  Kazan
 * taylor and I fixed fred2 by making code.lib and fred2's defines match up
 *
 * Revision 1.16  2004/06/15 20:50:28  wmcoolmon
 * hud-set-color, current-speed, and FRED fixes (Wouldn't compile).
 *
 * Revision 1.15  2004/05/09 14:38:56  taylor
 * init OGL with 32bpp when -jpgtga is used
 *
 * Revision 1.14  2004/05/02 17:46:19  wmcoolmon
 * fred2_open -fredhtl flag no longer reversed (Kudos to RT) -C
 *
 * Revision 1.13  2004/04/03 18:06:35  Kazan
 * Fixes:
 *
 * Revision 1.12  2004/02/13 04:25:37  randomtiger
 * Added messagebox to offer startup choice between htl, non htl and quitting.
 * Tidied up the background dialog box a bit and added some disabled controls for future ambient light feature.
 * Set non htl to use a lower level of model and texture detail to boost poor performance on this path.
 *
 * Revision 1.11  2004/02/05 23:23:19  randomtiger
 * Merging OGL Fred into normal Fred, no conflicts, should be perfect
 *
 * Revision 1.10.2.4  2004/01/24 13:00:43  randomtiger
 * Implemented basic OGL HT&L but some drawing routines missing from cobe.lib so keeping turned off for now.
 * Added lighting menu option and made about dialog more useful.
 * Ship menu now defaults to the first entry.
 * Loads of debug to try and trap crashes.
 *
 * Revision 1.10.2.3  2004/01/20 19:11:29  randomtiger
 * Cut unneeded code, text now works in graphics window.
 *
 * Revision 1.10.2.2  2004/01/19 00:53:37  randomtiger
 * More Fred OGL changes.
 *
 * Revision 1.10.2.1  2004/01/17 22:11:25  randomtiger
 * Large series of changes to make fred run though OGL. Requires code.lib to be up to date to compile.
 * Port still in progress, fred currently unusable, note this has been committed to fred_ogl branch.
 *
 * Revision 1.10  2004/01/14 06:28:39  Goober5000
 * made set-support-ship number align with general FS convention
 * --Goober5000
 *
 * Revision 1.9  2003/10/15 22:10:34  Kazan
 * Da Species Update :D
 *
 * Revision 1.8  2003/09/05 06:53:41  Goober5000
 * added code to change the player's ship in single player
 * --Goober5000
 *
 * Revision 1.7  2003/05/09 23:58:56  phreak
 * user-defined loading screen option is reset when the user creates a new mission
 *
 * Revision 1.6  2003/04/29 01:04:28  Goober5000
 * custom hitpoints for FRED
 * --Goober5000
 *
 * Revision 1.5  2003/01/19 07:02:15  Goober5000
 * fixed a bunch of bugs - "no-subspace-drive" should now work properly for
 * all ships, and all ships who have their departure anchor set to a capital ship
 * should exit to that ship when told to depart
 * --Goober5000
 *
 * Revision 1.4  2003/01/06 20:49:16  Goober5000
 * FRED2 support for wing squad logos - look in the wing editor
 * --Goober5000
 *
 * Revision 1.3  2002/08/15 04:35:44  penguin
 * Changes to build with fs2_open code.lib
 *
 * Revision 1.2  2002/08/15 01:06:34  penguin
 * Include filename reorg (to coordinate w/ fs2_open)
 *
 * Revision 1.1.1.1  2002/07/15 03:10:59  inquisitor
 * Initial FRED2 Checking
 *
 * 
 * 29    10/13/99 9:22a Daveb
 * Fixed Fred jumpnode placing bug. Fixed 1024 glide tiled texture problem
 * related to movies. Fixed launcher spawning from PXO screen.
 * 
 * 28    9/08/99 10:01p Dave
 * Make sure game won't run in a drive's root directory. Make sure
 * standalone routes suqad war messages properly to the host.
 * 
 * 27    7/23/99 2:02p Jamesa
 * Don't require gamepalettes
 * 
 * 26    7/15/99 3:07p Dave
 * 32 bit detection support. Mouse coord commandline.
 * 
 * 25    7/02/99 4:30p Dave
 * Much more sophisticated lightning support.
 * 
 * 24    5/20/99 6:59p Dave
 * Added alternate type names for ships. Changed swarm missile table
 * entries.
 * 
 * 23    4/26/99 8:47p Dave
 * Made all pof related nebula stuff customizable through Fred.
 * 
 * 22    4/16/99 2:34p Andsager
 * Second pass on debris fields
 * 
 * 21    4/15/99 5:00p Andsager
 * Frist pass on Debris field
 * 
 * 20    4/07/99 6:21p Dave
 * Fred and FreeSpace support for multiple background bitmaps and suns.
 * Fixed link errors on all subprojects. Moved encrypt_init() to
 * cfile_init() and lcl_init(), since its safe to call twice.
 * 
 * 19    3/31/99 9:50a Andsager
 * Interface for generalization of asteroid field (debris field)
 * 
 * 18    3/30/99 5:40p Dave
 * Fixed reinforcements for TvT in multiplayer.
 * 
 * 17    3/24/99 4:05p Dave
 * Put in support for assigning the player to a specific squadron with a
 * specific logo. Preliminary work for doing pos/orient checksumming in
 * multiplayer to reduce bandwidth.
 * 
 * 16    3/20/99 5:09p Dave
 * Fixed release build fred warnings and unhandled exception.
 * 
 * 15    2/23/99 7:03p Dave
 * Rewrote a horribly mangled and evil team loadout dialog. Bugs gone.
 * 
 * 14    2/17/99 2:11p Dave
 * First full run of squad war. All freespace and tracker side stuff
 * works.
 * 
 * 13    2/10/99 11:11a Johnson
 * Think I fixed the problem where the Neb2_awacs value was improperly
 * being checked for in Fred.
 * 
 * 12    2/07/99 8:51p Andsager
 * Add inner bound to asteroid field.  Inner bound tries to stay astroid
 * free.  Wrap when within and don't throw at ships inside.
 * 
 * 11    1/25/99 5:03a Dave
 * First run of stealth, AWACS and TAG missile support. New mission type
 * :)
 * 
 * 10    1/19/99 3:57p Andsager
 * Round 2 of variables
 * 
 * 9     12/18/98 1:14a Dave
 * Rough 1024x768 support for Direct3D. Proper detection and usage through
 * the launcher.
 * 
 * 8     11/06/98 10:15a Dave
 * 
 * 7     10/29/98 9:22p Dave
 * Removed minor bug concering externalization of campaign files.
 * 
 * 6     10/29/98 6:49p Dave
 * Finished up Fred support for externalizing mission and campaign files.
 * 
 * 5     10/29/98 10:41a Dave
 * Change the way cfile initializes exe directory.
 * 
 * 4     10/23/98 5:07p Dave
 * Put in beginnings of localization/externalization functionality.
 * 
 * 3     10/22/98 6:13p Dave
 * Added registry and localization support.
 * 
 * 2     10/07/98 6:28p Dave
 * Initial checkin. Renamed all relevant stuff to be Fred2 instead of
 * Fred. Globalized mission and campaign file extensions. Removed Silent
 * Threat specific code.
 * 
 * 1     10/07/98 3:02p Dave
 * 
 * 1     10/07/98 3:00p Dave
 * 
 * 227   6/17/98 2:05p Hoffoss
 * Fixed bug: Shield system for team or ship type flags new reset when
 * mission gets cleared out (new or load mission).
 * 
 * 226   5/20/98 10:19p Allender
 * make Fred work with NDEBUG build
 * 
 * 225   5/17/98 1:48p Allender
 * made number of respawns for new missions 3
 * 
 * 224   5/01/98 12:34p John
 * Added code to force FreeSpace to run in the same dir as exe and made
 * all the parse error messages a little nicer.
 * 
 * 223   4/30/98 8:23p John
 * Fixed some bugs with Fred caused by my new cfile code.
 * 
 * 222   4/27/98 4:07p Allender
 * make orders_accepted not be assigned to -1 when creating new ships.
 * Don't set the use_default_orders flag when orders are -1
 * 
 * 221   4/25/98 6:43p Allender
 * reset model data when initializeing new mission
 * 
 * 220   4/17/98 1:41p Allender
 * took out function calls in NDEBUG mode
 * 
 * 219   4/13/98 10:59a Hoffoss
 * Made "new mission" clear the cmd brief structure.
 * 
 * 218   4/13/98 10:11a John
 * Made timer functions thread safe.  Made timer_init be called in all
 * projects.
 * 
 * 217   4/06/98 5:37p Hoffoss
 * Added sexp tree support to briefings in Fred.
 * 
 * 216   4/06/98 12:55p John
 * Upped the gamma for Fred.
 * 
 * 215   4/03/98 11:34a John
 * Fixed the stuff I broke in Fred from the new briefing
 * 
 * 214   4/02/98 3:00p Johnson
 * Fixed a bug a release build turned up.
 * 
 * 213   3/24/98 1:36p Hoffoss
 * Moved call to load_filter_info() to after cfile_init(), as it is
 * dependent on that being set up.
 * 
 * 212   3/24/98 12:42p Allender
 * fixed a couple of minor problems with arrival targets
 * 
 * 211   3/21/98 7:36p Lawrance
 * Move jump nodes to own lib.
 * 
 * 210   3/17/98 4:16p Allender
 * minor changes to the kamikaze flag
 * 
 * 209   3/17/98 11:55a Johnson
 * Fixed bug where jump nodes wheren't being cleared on a new mission.
 * 
 * 208   3/10/98 4:26p Hoffoss
 * Changed jump node structure to include a name.  Position is now taken
 * from the object (each jump node has an associated object now).
 * 
 * 207   3/09/98 4:30p Allender
 * multiplayer secondary weapon changes.  red-alert and cargo-known-delay
 * sexpressions.  Add time cargo revealed to ship structure
 * 
 * 206   3/09/98 10:56a Hoffoss
 * Added jump node objects to Fred.
 * 
 * 205   3/06/98 5:10p Allender
 * made time to: field in extended targetbox use support time to dock code
 * for all docking shpis.  Only display for waypoints and docking (not
 * undocking).  Small fixups to message menu -- not allowing depart when
 * disabled.  Depart is now by default ignored for all non-small ships
 * 
 * 204   3/05/98 3:59p Hoffoss
 * Added a bunch of new command brief stuff, and asteroid initialization
 * to Fred.
 * 
 * 203   2/26/98 4:59p Allender
 * groundwork for team vs team briefings.  Moved weaponry pool into the
 * Team_data structure.  Added team field into the p_info structure.
 * Allow for mutliple structures in the briefing code.
 * 
 * 202   2/17/98 12:07p Hoffoss
 * Changed over to using SF_CARGO_REVEALED in fred.
 * 
 * 201   2/17/98 10:12a Hoffoss
 * Fixed bug with sprintf() in reference_handler().  Forgot the first
 * argument! :)  Amazing it never crashed before.
 * 
 * 200   2/13/98 11:45a Hoffoss
 * Made all new ships created in Fred default to 33% initial speed.
 * 
 * $NoKeywords: $
 */

#include "stdafx.h"
#include "FRED.h"
#include "MainFrm.h"
#include "FREDDoc.h"
#include "FREDView.h"
#include "FredRender.h"
#include "ai/ailocal.h"
#include "ai/aigoals.h"
#include "ship/ship.h"
#include "globalincs/linklist.h"
#include "mission/missionparse.h"
#include "mission/missionmessage.h"
#include "mission/missiongoals.h"
#include "mission/missionbriefcommon.h"
#include "Management.h"
#include "cfile/cfile.h"
#include "palman/palman.h"
#include "graphics/2d.h"
#include "render/3d.h"
#include "weapon/weapon.h"
#include "io/key.h"
#include "parse/parselo.h"
#include "math/fvi.h"
#include "starfield/starfield.h"
#include "parse/sexp.h"
#include "io/mouse.h"
#include "mission/missioncampaign.h"
#include "wing.h"
#include "MessageEditorDlg.h"
#include "EventEditor.h"
#include "MissionGoalsDlg.h"
#include "ShieldSysDlg.h"
#include "gamesnd/eventmusic.h"
#include "DebriefingEditorDlg.h"
#include "starfield/nebula.h"
#include "asteroid/asteroid.h"
#include "hud/hudsquadmsg.h"
#include "jumpnode/jumpnode.h"
#include "stats/medals.h"
#include "localization/localize.h" 
#include "osapi/osregistry.h"
#include "localization/fhash.h"
#include "io/timer.h"
#include "nebula/neb.h"
#include "nebula/neblightning.h"
#include "species_defs/species_defs.h"
#include "osapi/osapi.h"
#include "graphics/font.h"
#include "debugconsole/dbugfile.h"
#include "object/objectdock.h"
#include "gamesnd/gamesnd.h"
#include "iff_defs/iff_defs.h"
#include "menuui/techmenu.h"
#include "missionui/fictionviewer.h"

#include <direct.h>
#include "cmdline/cmdline.h"

#define MAX_DOCKS 50

#define UNKNOWN_USER		"Unknown"

int cur_wing = -1;
int cur_wing_index;
int cur_object_index = -1;
int cur_ship = -1;
int cur_model_index = 0;
int cur_waypoint = -1;
int cur_waypoint_list = -1;
int delete_flag;
int bypass_update = 0;
int Default_player_model = 0;
int Update_ship = 0;
int Update_wing = 0;
int Fred_font;

char Fred_exe_dir[512] = "";
char Fred_base_dir[512] = "";

char Fred_alt_names[MAX_SHIPS][NAME_LENGTH+1];
char Fred_callsigns[MAX_SHIPS][NAME_LENGTH+1];

// object numbers for ships in a wing.
int wing_objects[MAX_WINGS][MAX_SHIPS_PER_WING];

char *Docking_bay_list[MAX_DOCKS];

// Goober5000
bool Show_iff[MAX_IFFS];

CCriticalSection CS_cur_object_index;

ai_goal_list Ai_goal_list[] = {
	{ "Waypoints",				AI_GOAL_WAYPOINTS,			0 },
	{ "Waypoints once",			AI_GOAL_WAYPOINTS_ONCE,		0 },
	{ "Warp",					AI_GOAL_WARP,				0 },
	{ "Destroy subsystem",		AI_GOAL_DESTROY_SUBSYSTEM,	0 },
	{ "Attack",					AI_GOAL_CHASE | AI_GOAL_CHASE_WING,	0 },
	{ "Dock",					AI_GOAL_DOCK,				0 },
	{ "Undock",					AI_GOAL_UNDOCK,				0 },
	{ "Guard",					AI_GOAL_GUARD | AI_GOAL_GUARD_WING,	0 },
	{ "Attack any ship",		AI_GOAL_CHASE_ANY,			0 },
	{ "Disable ship",			AI_GOAL_DISABLE_SHIP,		0 },
	{ "Disarm ship",			AI_GOAL_DISARM_SHIP,		0 },
	{ "Evade ship",				AI_GOAL_EVADE_SHIP,			0 },
	{ "Ignore ship",			AI_GOAL_IGNORE,				0 },
	{ "Ignore ship (new)",		AI_GOAL_IGNORE_NEW,			0 },
	{ "Stay near ship",			AI_GOAL_STAY_NEAR_SHIP,		0 },
	{ "Keep safe distance",		AI_GOAL_KEEP_SAFE_DISTANCE,	0 },
	{ "Stay still",				AI_GOAL_STAY_STILL,			0 },
	{ "Play dead",				AI_GOAL_PLAY_DEAD,			0 }
};

int Ai_goal_list_size = sizeof(Ai_goal_list) / sizeof(ai_goal_list);

// internal function prototypes
void set_cur_indices(int obj);
int common_object_delete(int obj);
int create_waypoint(vec3d *pos, int list);
int create_ship(matrix *orient, vec3d *pos, int ship_type);
int query_ship_name_duplicate(int ship);
char *reg_read_string( char *section, char *name, char *default_value );

extern int Nmodel_num;
extern int Nmodel_bitmap;

void string_copy(char *dest, CString &src, int max_len, int modify)
{
	int len;

	if (modify)
		if (strcmp(src, dest))
			set_modified();

	len = strlen(src);
	if (len >= max_len)
		len = max_len - 1;

	strncpy(dest, src, len);
	dest[len] = 0;
}

// converts a multiline string (one with newlines in it) into a windows format multiline
// string (newlines changed to '\r\n').
CString convert_multiline_string(char *src)
{
	char *ptr, buf[256];
	int i;
	static CString str;

	str = _T("");
	while ((ptr = strchr(src, '\n'))!=NULL) {
		i = ptr - src;
		while (i > 250) {
			strncpy(buf, src, 250);
			buf[250] = 0;
			str += buf;
			src += 250;
			i -= 250;
		}

		if (i)
			strncpy(buf, src, i);

		buf[i] = 0;
		str += buf;
		str += "\r\n";
		src = ptr + 1;
	}

	i = strlen(src);
	if (i)
		str += src;

	return str;
}

// Converts a windows format multiline CString back into a normal multiline string.
void deconvert_multiline_string(char *buf, CString &str, int max_len)
{
	char *ptr = buf;
	int i, j;
	CString str2;

	Assert(max_len > 1);
	Assert(buf);
	max_len -= 2;
	while ((i = str.Find("\r\n")) >= 0) {
		for (j=0; j<i; j++)
			if (max_len) {
				*ptr++ = str[j];
				max_len--;
			}

		*ptr++ = '\n';
		str2 = str.Mid(i + 2);
		str = str2;
	}

	i = str.GetLength();
	for (j=0; j<i; j++)
		if (max_len) {
			*ptr++ = str[j];
			max_len--;
		}

	// removed 10/29/98 by DB
	// this was generating an extra newline - why?
	//if (*(ptr - 1) != '\n')
	//	*ptr++ = '\n';
	*ptr = 0;
}

// medal_stuff Medals[NUM_MEDALS];
/*
void parse_medal_tbl()
{
	int rval, num_medals;

	// open localization
	lcl_ext_open();

	if ((rval = setjmp(parse_abort)) != 0) {
		mprintf(("TABLES: Unable to parse '%s'!  Error code = %i.\n", "medals.tbl", rval));
		lcl_ext_close();
		return;
	} 

	read_file_text("medals.tbl");
	reset_parse();

	// parse in all the rank names
	num_medals = 0;
	required_string("#Medals");
	while ( required_string_either("#End", "$Name:") ) {
		Assert ( num_medals < NUM_MEDALS);
		required_string("$Name:");
		stuff_string( Medals[num_medals].name, F_NAME, NULL );
		required_string("$Bitmap:");
		stuff_string( Medals[num_medals].bitmap, F_NAME, NULL );
		required_string("$Num mods:");
		stuff_int( &Medals[num_medals].num_versions);

		// some medals are based on kill counts.  When string +Num Kills: is present, we know that
		// this medal is a badge and should be treated specially
		Medals[num_medals].kills_needed = 0;

		if ( optional_string("+Num Kills:") ) {
			char buf[MULTITEXT_LENGTH + 1];

			stuff_int( &Medals[num_medals].kills_needed );

			required_string("$Wavefile 1:");
			stuff_string(buf, F_NAME, NULL, MAX_FILENAME_LEN);

			required_string("$Wavefile 2:");
			stuff_string(buf, F_NAME, NULL, MAX_FILENAME_LEN);

			required_string("$Promotion Text:");
			stuff_string(buf, F_MULTITEXT, NULL);
		}

		num_medals++;
	}

	required_string("#End");      

	// close localization
	lcl_ext_close();
}
*/

// an atexit() call!!
void fred_close()
{
	if (Fred_texture_replacements != NULL) {
		delete[] Fred_texture_replacements;
		Fred_texture_replacements = NULL;
	}
}

void parse_init(bool basic = false);
void brief_init_colors();

void fred_preload_all_briefing_icons()
{
	uint i,j;
	for (i = 0; i < Species_info.size(); i++)
	{
		for (j = 0; j < MAX_BRIEF_ICONS; j++)
		{
			generic_anim_load(&Species_info[i].icon_bitmaps[j]);
			hud_anim_load(&Species_info[i].icon_fade_anims[j]);
			hud_anim_load(&Species_info[i].icon_highlight_anims[j]);
		}
	}
}

bool fred_init()
{
	int i;
	char palette_filename[1024];

	if (!vm_init(24*1024*1024)) {
		MessageBox( NULL, "Not enough memory to run Fred.\r\nTry closing down some other applications.\r\n", "Not Enough Memory", MB_OK );
		return false;
	}

	srand( (unsigned) time(NULL) );
	init_pending_messages();

	// initialize registry stuff
	os_init_registry_stuff(Osreg_company_name, Osreg_app_name, NULL);

	timer_init();

	Assert(strlen(Fred_exe_dir) > 0);


	// this should enable mods - Kazan
	fred2_parse_cmdline(__argc, __argv);

#ifndef NDEBUG
	extern void cmdline_debug_print_cmdline();
	cmdline_debug_print_cmdline();
#endif

	// d'oh
	DBUGFILE_OUTPUT_0("About to cfile_init");
	if(cfile_init(Fred_exe_dir)){
		exit(1);
	}

	// initialize localization module. Make sure this is done AFTER initialzing OS.
	// NOTE : Fred should ALWAYS run in English. Otherwise it might swap in another language
	// when saving - which would cause inconsistencies when externalizing to tstrings.tbl via Exstr
	// trust me on this :)
	lcl_init(LCL_ENGLISH);

	// Goober5000 - force init XSTRs (so they work, but only work in English, based on above comment)
	extern int Xstr_inited;
	Xstr_inited = 1;

#ifndef NDEBUG
	load_filter_info();
#endif

	//CFREDView *window = CFREDView::GetView();
	//HWND hwndApp = window->GetSafeHwnd();
	//os_set_window((uint) hwndApp);

	/*
	int result = MessageBox(NULL, 
		"Welcome to OGL Fred2_open. Do you want to run in htl? "
		"Its faster on any system and runs in full detail but is still buggy.", 
		"Question", MB_ICONQUESTION | MB_YESNOCANCEL);

	if(result == IDCANCEL) return false;

	Cmdline_nohtl = result != IDYES;
	*/

	/* - HTL is now on by default so the warning is redundant - Karajorma
	if (Cmdline_nohtl)
	{
		MessageBox(NULL, "You are not running in HTL mode for FRED.  Although HTL mode isn't required, there may be some crashes"
						 " when trying to render the new high polygon models.  To enable HTL mode, create a shortcut to FRED, right-click into properties"
						 " and add \"-fredhtl\" to the end of the string in the \"target\" box.", "FRED2", MB_ICONWARNING | MB_OK);
	}
	*/

	snd_init(0, 0, 44100, 16);

	// Not ready for this yet
  //	Cmdline_nospec = 1;
 // 	Cmdline_noglow = 1;
 	Cmdline_window = 1;
	DBUGFILE_OUTPUT_0("About to gr_init");

	gr_init(GR_OPENGL, 640, 480, 32);

	Mouse_hidden = 1;

	DBUGFILE_OUTPUT_0("About to init everything");
	gr_font_init();					// loads up all fonts  
//	Fred_font = gr_init_font("font01.vf");
	gr_set_gamma(3.0f);

	sprintf(palette_filename, "gamepalette%d-%02d", 1, 1);
	mprintf(("Loading palette %s\n", palette_filename));
	palette_load_table(palette_filename);

	key_init();
	mouse_init();

	iff_init();			// Goober5000
	species_init();		// Kazan

	// for fred specific replacement texture stuff
	//Fred_texture_replacements = (texture_replace*) vm_malloc( sizeof(texture_replace) * MAX_SHIPS * MAX_REPLACEMENT_TEXTURES );
	Fred_texture_replacements = new texture_replace[MAX_SHIPS*MAX_REPLACEMENT_TEXTURES];
	atexit(fred_close);

	// Goober5000
	for (i = 0; i < MAX_IFFS; i++)
		Show_iff[i] = true;

	// Goober5000
	strcpy(Voice_abbrev_briefing, "");
	strcpy(Voice_abbrev_campaign, "");
	strcpy(Voice_abbrev_command_briefing, "");
	strcpy(Voice_abbrev_debriefing, "");
	strcpy(Voice_abbrev_message, "");
	strcpy(Voice_abbrev_mission, "");
	Voice_no_replace_filenames = false;
	strcpy(Voice_script_entry_format, "Sender: $sender\r\nPersona: $persona\r\nFile: $filename\r\nMessage: $message");
	Voice_export_selection = 0;

	hud_init_comm_orders();		// Goober5000
	
	gamesnd_parse_soundstbl();		// needs to be loaded after species stuff but before interface/weapon/ship stuff - taylor
	mission_brief_common_init();	
	obj_init();
	model_free_all();				// Free all existing models
	ai_init();
	ai_profiles_init();
	armor_init();
	weapon_init();
	parse_medal_tbl();			// get medal names for sexpression usage
	ship_init();
	parse_init();
	techroom_intel_init();

	// initialize and activate external string hash table
	// make sure to do here so that we don't parse the table files into the hash table - waste of space
	fhash_init();
	fhash_activate();

	create_new_mission();
	neb2_init();						// fullneb stuff
	stars_init();
	brief_init_colors();
	brief_parse_icon_tbl();
	fred_preload_all_briefing_icons(); //phreak.  This needs to be done or else the briefing icons won't show up
	event_music_init();
	fiction_viewer_reset();
	cmd_brief_reset();
	Show_waypoints = TRUE;
	Campaign.filename[0] = 0;  // indicate initialized state

	stars_post_level_init();

	// neb lightning
	nebl_init();

	gr_reset_clip();
	g3_start_frame(0);
	g3_set_view_matrix(&eye_pos, &eye_orient, 0.5f);

	for (i=0; i<Num_ship_classes; i++)
		if (Ship_info[i].flags & SIF_DEFAULT_PLAYER_SHIP) {
			Default_player_model = cur_model_index = i;
			break;
		}

	Id_select_type_start = Num_ship_classes + 2;
	Id_select_type_jump_node = Num_ship_classes + 1;
	Id_select_type_waypoint = Num_ship_classes;
	Fred_main_wnd -> init_tools();	
	return true;
}

void set_physics_controls()
{
	physics_init(&view_physics);
	view_physics.max_vel.xyz.x *= physics_speed / 3.0f;
	view_physics.max_vel.xyz.y *= physics_speed / 3.0f;
	view_physics.max_vel.xyz.z *= physics_speed / 3.0f;
	view_physics.max_rear_vel *= physics_speed / 3.0f;

	view_physics.max_rotvel.xyz.x *= physics_rot / 30.0f;
	view_physics.max_rotvel.xyz.y *= physics_rot / 30.0f;
	view_physics.max_rotvel.xyz.z *= physics_rot / 30.0f;
	view_physics.flags |= PF_ACCELERATES | PF_SLIDE_ENABLED;
	theApp.write_ini_file(1);
}

int create_object_on_grid(int list)
{
	int obj = -1;
	float rval;
	vec3d dir,pos;

	g3_point_to_vec_delayed(&dir, marking_box.x2, marking_box.y2);

	rval = fvi_ray_plane(&pos, &The_grid->center, &The_grid->gmatrix.vec.uvec, &view_pos, &dir, 0.0f);

	if (rval>=0.0f) {
		unmark_all();
		obj = create_object(&pos, list);
		if (obj >= 0) {
			mark_object(obj);
			FREDDoc_ptr->autosave("object create");

		} else if (obj == -1)
			Fred_main_wnd->MessageBox("Maximum ship limit reached.  Can't add any more ships.");
	}

	return obj;
}

void fix_ship_name(int ship)
{
	int i = 1;

	do {
		sprintf(Ships[ship].ship_name, "U.R.A. Moron %d", i++);
	} while (query_ship_name_duplicate(ship));
}

int create_ship(matrix *orient, vec3d *pos, int ship_type)
{
	// Save the Current Working dir to restore in a minute - fred is being stupid
	char pwd[128];
	getcwd(pwd, 128); // get the present working dir - probably <fs2path>[/modpapth]/data/missions/
	

	int obj, z1, z2;
	float temp_max_hull_strength;
	ship_info *sip;

	// "pop" and cfile_chdirs off the sta
	chdir(Fred_base_dir);


	obj = ship_create(orient, pos, ship_type);
	if (obj == -1)
		return -1;

	// ok, done with file io, restore the pwd
	chdir(pwd);

	Objects[obj].phys_info.speed = 33.0f;

	ship *shipp = &Ships[Objects[obj].instance];
	sip = &Ship_info[shipp->ship_info_index];

	if (query_ship_name_duplicate(Objects[obj].instance))
		fix_ship_name(Objects[obj].instance);

	// default stuff according to species and IFF
	shipp->team = Species_info[Ship_info[shipp->ship_info_index].species].default_iff;
	resolve_parse_flags(&Objects[obj], Iff_info[shipp->team].default_parse_flags, Iff_info[shipp->team].default_parse_flags2);

	// default shield setting
	z1 = Shield_sys_teams[shipp->team];
	z2 = Shield_sys_types[ship_type];
	if (((z1 == 1) && z2) || (z2 == 1))
		Objects[obj].flags |= OF_NO_SHIELDS;

	// set orders according to whether the ship is on the player ship's team
	{
		object *temp_objp;
		ship *temp_shipp = NULL;

		// find the first player ship
		for (temp_objp = GET_FIRST(&obj_used_list); temp_objp != END_OF_LIST(&obj_used_list); temp_objp = GET_NEXT(temp_objp))
		{
			if (temp_objp->type == OBJ_START)
			{
				temp_shipp = &Ships[temp_objp->instance];
				break;
			}
		}

		// set orders if teams match, or if player couldn't be found
		if (temp_shipp == NULL || shipp->team == temp_shipp->team)
		{
			// if this ship is not a small ship, then make the orders be the default orders without
			// the depart item
			if (!(sip->flags & SIF_SMALL_SHIP))
			{
				shipp->orders_accepted = ship_get_default_orders_accepted( sip );
				shipp->orders_accepted &= ~DEPART_ITEM;
			}
		}
		else
		{
			shipp->orders_accepted = 0;
		}
	}
	
	// calc kamikaze stuff
	if (shipp->special_hitpoint_index != -1)
	{
		temp_max_hull_strength = (float) atoi(Sexp_variables[shipp->special_hitpoint_index+HULL_STRENGTH].text);
	}
	else
	{
		temp_max_hull_strength = sip->max_hull_strength;
	}

	Ai_info[shipp->ai_index].kamikaze_damage = min(1000.0f, 200.0f + (temp_max_hull_strength / 4.0f));

	return obj;
}

int query_ship_name_duplicate(int ship)
{
	int i;

	for (i=0; i<MAX_SHIPS; i++)
		if ((i != ship) && (Ships[i].objnum != -1))
			if (!stricmp(Ships[i].ship_name, Ships[ship].ship_name))
				return 1;

	return 0;
}

void copy_bits(int *dest, int src, int mask)
{
	*dest &= ~mask;
	*dest |= src & mask;
}

int dup_object(object *objp)
{
	int i, j, n, inst, obj = -1;
	ai_info *aip1, *aip2;
	object *objp1, *objp2;
	ship_subsys *subp1, *subp2;
	static int list;

	if (!objp) {
		list = -1;
		return 0;
	}

	inst = objp->instance;
	if ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START)) {
		obj = create_ship(&objp->orient, &objp->pos, Ships[inst].ship_info_index);
		if (obj == -1)
			return -1;

		n = Objects[obj].instance;
		Ships[n].team = Ships[inst].team;
		Ships[n].arrival_cue = dup_sexp_chain(Ships[inst].arrival_cue);
		Ships[n].departure_cue = dup_sexp_chain(Ships[inst].departure_cue);
		Ships[n].cargo1 = Ships[inst].cargo1;
		Ships[n].arrival_location = Ships[inst].arrival_location;
		Ships[n].departure_location = Ships[inst].departure_location;
		Ships[n].arrival_delay = Ships[inst].arrival_delay;
		Ships[n].departure_delay = Ships[inst].departure_delay;
		Ships[n].weapons = Ships[inst].weapons;
		Ships[n].hotkey = Ships[inst].hotkey;

		aip1 = &Ai_info[Ships[n].ai_index];
		aip2 = &Ai_info[Ships[inst].ai_index];
		aip1->behavior = aip2->behavior;
		aip1->ai_class = aip2->ai_class;
		for (i=0; i<MAX_AI_GOALS; i++)
			aip1->goals[i] = aip2->goals[i];

		if ( aip2->ai_flags & AIF_KAMIKAZE )
			aip1->ai_flags |= AIF_KAMIKAZE;
		if ( aip2->ai_flags & AIF_NO_DYNAMIC )
			aip2->ai_flags |= AIF_NO_DYNAMIC;

		aip1->kamikaze_damage = aip2->kamikaze_damage;

		objp1 = &Objects[obj];
		objp2 = &Objects[Ships[inst].objnum];
		objp1->phys_info.speed = objp2->phys_info.speed;
		objp1->phys_info.fspeed = objp2->phys_info.fspeed;
		objp1->hull_strength = objp2->hull_strength;
		objp1->shield_quadrant[0] = objp2->shield_quadrant[0];

		subp1 = GET_FIRST(&Ships[n].subsys_list);
		subp2 = GET_FIRST(&Ships[inst].subsys_list);
		while (subp1 != END_OF_LIST(&Ships[n].subsys_list)) {
			Assert(subp2 != END_OF_LIST(&Ships[inst].subsys_list));
			subp1 -> current_hits = subp2 -> current_hits;
			subp1 = GET_NEXT(subp1);
			subp2 = GET_NEXT(subp2);
		}

		for (i=0; i<Num_reinforcements; i++)
			if (!stricmp(Reinforcements[i].name, Ships[inst].ship_name)) {
				if (Num_reinforcements < MAX_REINFORCEMENTS) {
					j = Num_reinforcements++;
					strcpy(Reinforcements[j].name, Ships[n].ship_name);
					Reinforcements[j].type = Reinforcements[i].type;
					Reinforcements[j].uses = Reinforcements[i].uses;
				}

				break;
			}

	} else if (objp->type == OBJ_WAYPOINT) {
		obj = create_waypoint(&objp->pos, list);
		list = Objects[obj].instance;
	}

	if (obj == -1)
		return -1;

	Objects[obj].pos = objp->pos;
	Objects[obj].orient = objp->orient;
	Objects[obj].flags |= OF_TEMP_MARKED;
	return obj;
}

int create_object(vec3d *pos, int list)
{
	int obj, n;

	if (cur_model_index == Id_select_type_waypoint)
		obj = create_waypoint(pos, list);

	else if (cur_model_index == Id_select_type_start) {
		if (Player_starts >= MAX_PLAYERS) {
			Fred_main_wnd->MessageBox("Unable to create new player start point.\n"
				"You have reached the maximum limit.", NULL, MB_OK | MB_ICONEXCLAMATION);
			obj = -2;

		} else if (The_mission.game_type & MISSION_TYPE_SINGLE) {
			Fred_main_wnd->MessageBox("You can't have more than one player start in\n"
				"single player missions.\n", NULL, MB_OK | MB_ICONEXCLAMATION);
			obj = -2;

		} else if (The_mission.game_type & MISSION_TYPE_TRAINING) {
			Fred_main_wnd->MessageBox("You can't have more than one player start in\n"
				"a training missions.\n", NULL, MB_OK | MB_ICONEXCLAMATION);
			obj = -2;

		} else
			obj = create_player(Player_starts, pos, NULL, Default_player_model);

	} else if (cur_model_index == Id_select_type_jump_node) {
		obj = (new (vm_malloc(sizeof(jump_node))) jump_node(pos))->get_objnum();
	} else if(Ship_info[cur_model_index].flags & SIF_NO_FRED){		
		obj = -1;
	} else {  // creating a ship
		obj = create_ship(NULL, pos, cur_model_index);
		if (obj == -1)
			return -1;

		n = Objects[obj].instance;
		Ships[n].arrival_cue = alloc_sexp("true", SEXP_ATOM, SEXP_ATOM_OPERATOR, -1, -1);
		Ships[n].departure_cue = alloc_sexp("false", SEXP_ATOM, SEXP_ATOM_OPERATOR, -1, -1);
		Ships[n].cargo1 = 0;
	}

	if (obj < 0)
		return obj;

	obj_merge_created_list();
	set_modified();
	Update_window = 1;
	return obj;
}

int create_player(int num, vec3d *pos, matrix *orient, int type, int init)
{
	int obj;

	if (type == -1){
		type = Default_player_model;
	}

	Assert(type >= 0);
	Assert(Player_starts < MAX_PLAYERS);
	Player_starts++;
	obj = create_ship(orient, pos, type);
	Objects[obj].type = OBJ_START;

	// be sure arrival/departure cues are set
	Ships[Objects[obj].instance].arrival_cue = Locked_sexp_true;
	Ships[Objects[obj].instance].departure_cue = Locked_sexp_false;
	obj_merge_created_list();
	set_modified();
	return obj;
}

int query_waypoint_path_name_duplicate(int list)
{
	int i;

	for (i=0; i<Num_waypoint_lists; i++)
		if (i != list)
			if (!stricmp(Waypoint_lists[i].name, Waypoint_lists[list].name))
				return 1;

	return 0;
}

void get_unique_waypoint_path_name(int list)
{
	int i = 1;

	sprintf(Waypoint_lists[list].name, "Waypoint path %d", list + 1);
	while (query_waypoint_path_name_duplicate(list)) {
		sprintf(Waypoint_lists[list].name, "Waypoint path U%d", i++);
	}
}

int create_waypoint(vec3d *pos, int list)
{
	int i, obj, index = 0;
	object *ptr;

	if (list == -1) {  // find a new list to start.
		for (list=0; list<MAX_WAYPOINT_LISTS; list++){
			if (!Waypoint_lists[list].count) {
				get_unique_waypoint_path_name(list);
				break;
			}
		}
	} else {
		index = (list & 0xffff) + 1;
		list /= 65536;
	}

	if (list == MAX_WAYPOINT_LISTS) {
		Fred_main_wnd->MessageBox("Unable to create new waypoint path.  You\n"
			"have reached the maximum limit.", NULL, MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	Assert((list >= 0) && (list < MAX_WAYPOINT_LISTS));  // illegal index or out of lists.
	if (Waypoint_lists[list].count >= MAX_WAYPOINTS_PER_LIST) {
		Fred_main_wnd->MessageBox("Unable to create new waypoint.  You have\n"
			"reached the maximum limit on waypoints per list.", NULL, MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	if (Waypoint_lists[list].count > index) {
		i = Waypoint_lists[list].count;
		while (i > index) {
			Waypoint_lists[list].waypoints[i] = Waypoint_lists[list].waypoints[i - 1];
			Waypoint_lists[list].flags[i] = Waypoint_lists[list].flags[i - 1];
			i--;
		}
	}

	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		Assert(ptr->type != OBJ_NONE);
		if (ptr->type == OBJ_WAYPOINT) {
			i = ptr->instance;
			if ((i / 65536 == list) && ((i & 0xffff) >= index)){
				ptr->instance++;
			}
		}

		ptr = GET_NEXT(ptr);
	}

	Waypoint_lists[list].count++;
	Waypoint_lists[list].flags[index] = 0;
	Waypoint_lists[list].waypoints[index] = *pos;
	if (list >= Num_waypoint_lists){
		Num_waypoint_lists = list + 1;
	}

	obj = obj_create(OBJ_WAYPOINT, -1, list * 65536 + index, NULL, pos, 0.0f, OF_RENDERS);
	set_modified();
	return obj;
}

void create_new_mission()
{
	reset_mission();
	*Mission_filename = 0;
	FREDDoc_ptr->autosave("nothing");
	Undo_count = 0;
}

void reset_mission()
{
	clear_mission();
	player_start1 = create_player(0, &vmd_zero_vector, &vmd_identity_matrix);
}

void clear_mission()
{
	char *str;
	int i, j, count;
	CTime t;

	// clean up everything we need to before we reset back to defaults.
	if (Briefing_dialog){
		Briefing_dialog->reset_editor();
	}

	extern void allocate_mission_text(int size);
	allocate_mission_text( MISSION_TEXT_SIZE );

	fiction_viewer_reset();
	cmd_brief_reset();
	mission_event_shutdown();

	Asteroid_field.num_initial_asteroids = 0;  // disable asteroid field by default.
	Asteroid_field.speed = 0.0f;
	vm_vec_make(&Asteroid_field.min_bound, -1000.0f, -1000.0f, -1000.0f);
	vm_vec_make(&Asteroid_field.max_bound,  1000.0f,  1000.0f,  1000.0f);
	vm_vec_make(&Asteroid_field.inner_min_bound, -500.0f, -500.0f, -500.0f);
	vm_vec_make(&Asteroid_field.inner_max_bound,  500.0f,  500.0f,  500.0f);
	Asteroid_field.has_inner_bound = 0;
	Asteroid_field.field_type = FT_ACTIVE;
	Asteroid_field.debris_genre = DG_ASTEROID;
	Asteroid_field.field_debris_type[0] = -1;
	Asteroid_field.field_debris_type[1] = -1;
	Asteroid_field.field_debris_type[2] = -1;

	strcpy(Mission_parse_storm_name, "none");

	obj_init();
	model_free_all();				// Free all existing models
	ai_init();
	ai_profiles_init();
	ship_init();
	Num_ai_dock_names = 0;
	jumpnode_level_close();
	Num_wings = 0;
	for (i=0; i<MAX_WINGS; i++){
		Wings[i].wave_count = 0;
		Wings[i].wing_squad_filename[0] = '\0';
		Wings[i].wing_insignia_texture = -1;
	}

	for (i=0; i<MAX_WAYPOINT_LISTS; i++){
		Waypoint_lists[i].count = 0;
	}

	for (i=0; i<MAX_IFFS; i++){
		Shield_sys_teams[i] = 0;
	}

	for (i=0; i<MAX_SHIP_CLASSES; i++){
		Shield_sys_types[i] = 0;
	}

	Num_reinforcements = 0;
	set_cur_indices(-1);

	str = reg_read_string("SOFTWARE\\Microsoft\\Windows\\CurrentVersion", "RegisteredOwner", NULL);
	if (!str) {
		str = reg_read_string("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "RegisteredOwner", NULL);
		if (!str) {
			str = getenv("USERNAME");
			if (!str){
				str = UNKNOWN_USER;
			}
		}
	}

	t = CTime::GetCurrentTime();
	strcpy(The_mission.name, "Untitled");
	strncpy(The_mission.author, str, NAME_LENGTH - 1);
	The_mission.author[NAME_LENGTH - 1] = 0;
	strcpy(The_mission.created, t.Format("%x at %X"));
	strcpy(The_mission.modified, The_mission.created);
	strcpy(The_mission.notes, "This is a FRED2_OPEN created mission.\n");
	strcpy(The_mission.mission_desc, "Put mission description here\n");
	The_mission.game_type = MISSION_TYPE_SINGLE;
	strcpy(The_mission.squad_name, "");
	strcpy(The_mission.squad_filename, "");
	The_mission.num_respawns = 3;

	Player_starts = 0;
	Num_teams = 1;

	// reset alternate name & callsign stuff
	for(i=0; i<MAX_SHIPS; i++){
		strcpy(Fred_alt_names[i], "");
		strcpy(Fred_callsigns[i], "");
	}

	// set up the default ship types for all teams.  For now, this is the same class
	// of ships for all teams
	for (i=0; i<MAX_TVT_TEAMS; i++) {
		count = 0;
		for ( j = 0; j < MAX_SHIP_CLASSES; j++ ) {
			if (Ship_info[j].flags & SIF_DEFAULT_PLAYER_SHIP) {
				Team_data[i].ship_list[count] = j;
				Team_data[i].ship_count[count++] = 5;
			}
		}
		Team_data[i].number_choices = count;

		for (j=0; j<MAX_WEAPON_TYPES; j++){
			if (Weapon_info[j].wi_flags & WIF_PLAYER_ALLOWED){
				if(Weapon_info[j].subtype == WP_LASER){
					Team_data[i].weaponry_pool[j] = 16;
				} else {
					Team_data[i].weaponry_pool[j] = 500;
				}
			} else {
				Team_data[i].weaponry_pool[j] = 0;
			}
		}
	}

	*Mission_text = *Mission_text_raw = EOF_CHAR;
	Mission_text[1] = Mission_text_raw[1] = 0;

	Num_waypoint_lists = 0;
	Num_mission_events = 0;
	Num_goals = 0;
	unmark_all();
	obj_init();
	model_free_all();				// Free all existing models
	fred_render_init();
	init_sexp();
	messages_init();
	brief_reset();
	debrief_reset();
	ship_init();
	event_music_reset_choices();

	// alternate ship type names
	mission_parse_reset_alt();

	strcpy(Cargo_names[0], "Nothing");
	Num_cargo = 1;
	set_physics_controls();

	// reset background bitmaps and suns
	stars_pre_level_init();
	Nebula_index = 0;
	Mission_palette = 1;
	Nebula_pitch = (int) ((float) (rand() & 0x0fff) * 360.0f / 4096.0f);
	Nebula_bank = (int) ((float) (rand() & 0x0fff) * 360.0f / 4096.0f);
	Nebula_heading = (int) ((float) (rand() & 0x0fff) * 360.0f / 4096.0f);
	Neb2_awacs = -1.0f;
	Neb2_poof_flags = 0;
	strcpy(Neb2_texture_name, "");
	for(i=0; i<MAX_NEB2_POOFS; i++){
		Neb2_poof_flags |= (1<<i);
	}
	Nmodel_num = -1;
	Nmodel_bitmap = -1;

	The_mission.contrail_threshold = CONTRAIL_THRESHOLD_DEFAULT;

	// Goober5000
	The_mission.command_persona = Default_command_persona;
	strcpy(The_mission.command_sender, DEFAULT_COMMAND); 

	// Goober5000: reset ALL mission flags, not just nebula!
	The_mission.flags = 0;
	The_mission.support_ships.max_support_ships = -1;	// negative means infinite
	The_mission.support_ships.max_hull_repair_val = 0.0f;
	The_mission.support_ships.max_subsys_repair_val = 100.0f;
	The_mission.ai_profile = &Ai_profiles[Default_ai_profile];
	
	nebula_init(Nebula_filenames[Nebula_index], Nebula_pitch, Nebula_bank, Nebula_heading);

	char palette_filename[1024];
	strcpy(palette_filename, "gamepalette1-01");
//	sprintf( palette_filename, "gamepalette%d-%02d", 1, Mission_palette+1 );
	mprintf(( "Loading palette %s\n", palette_filename ));
	palette_load_table(palette_filename);

	strcpy(The_mission.loading_screen[GR_640],"");
	strcpy(The_mission.loading_screen[GR_1024],"");
	strcpy(The_mission.skybox_model, "");
	strcpy(The_mission.envmap_name, "");

	ENVMAP = -1;

	set_modified(FALSE);
	Update_window = 1;
}

int query_valid_object(int index)
{
	int obj_found = FALSE;
	object *ptr;

	if (index < 0 || index >= MAX_OBJECTS || Objects[index].type == OBJ_NONE)
		return FALSE;

	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		Assert(ptr->type != OBJ_NONE);
		if (OBJ_INDEX(ptr) == index)
			obj_found = TRUE;
		
		ptr = GET_NEXT(ptr);
	}

	Assert(obj_found);  // just to make sure it's in the list like it should be.	
	return TRUE;
}

int query_valid_ship(int index)
{
	int obj_found = FALSE;
	object *ptr;

	if (index < 0 || index >= MAX_OBJECTS || Objects[index].type != OBJ_SHIP)
		return FALSE;

	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		Assert(ptr->type != OBJ_NONE);
		if (OBJ_INDEX(ptr) == index)
			obj_found = TRUE;
		
		ptr = GET_NEXT(ptr);
	}

	Assert(obj_found);  // just to make sure it's in the list like it should be.	
	return TRUE;
}

int query_valid_waypoint(int index)
{
	int obj_found = FALSE;
	object *ptr;

	if (index < 0 || index >= MAX_OBJECTS || Objects[index].type != OBJ_WAYPOINT)
		return FALSE;

	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		Assert(ptr->type != OBJ_NONE);
		if (OBJ_INDEX(ptr) == index)
			obj_found = TRUE;
		
		ptr = GET_NEXT(ptr);
	}

	Assert(obj_found);  // just to make sure it's in the list like it should be.	
	return TRUE;
}

// Sets the current object to whatever is specified or advances to the next object
// in the list if nothing is passed.
void set_cur_object_index(int obj)
{
	if (obj < 0)
		unmark_all();
	else
		mark_object(obj);

	set_cur_indices(obj);  // select the new object
	Update_ship = Update_wing = 1;
	Waypoint_editor_dialog.initialize_data(1);
	Update_window = 1;
}

// changes the currently selected wing.  It is assumed that cur_wing == cur_ship's wing
// number.  Don't call this if this won't be true, or else you'll screw things up.
void set_cur_wing(int wing)
{
	cur_wing = wing;
/*	if (cur_ship != -1)
		Assert(cur_wing == Ships[cur_ship].wingnum);
	if ((cur_object_index != -1) && (Objects[cur_object_index].type == OBJ_SHIP))
		Assert(cur_wing == Ships[Objects[cur_object_index].instance].wingnum);*/
	Update_wing = 1;
	Update_window = 1;
}

// sets up the various cur_* global variables related to the selecting of an object.  This
// is an internal function that shouldn't typically get called directly.  Use set_cur_object_index() instead.
void set_cur_indices(int obj)
{
	int i;
	object *ptr;
	CSingleLock sync(&CS_cur_object_index);

	sync.Lock();  // Don't modify until it's unlocked (if it's locked elsewhere).
	if (query_valid_object(obj)) {
		cur_object_index = obj;
		cur_ship = cur_wing = cur_waypoint_list = cur_waypoint = -1;
		if ((Objects[obj].type == OBJ_SHIP) || (Objects[obj].type == OBJ_START)) {
			cur_ship = Objects[obj].instance;
			cur_wing = Ships[cur_ship].wingnum;
			if (cur_wing >= 0)
				for (i=0; i<Wings[cur_wing].wave_count; i++)
					if (wing_objects[cur_wing][i] == cur_object_index) {
						cur_wing_index = i;
						break;
					}

		} else if (Objects[obj].type == OBJ_WAYPOINT) {
			cur_waypoint_list = Objects[obj].instance / 65536;
			cur_waypoint = Objects[obj].instance & 0xffff;
		}

		return;
	}

	if (obj == -1 || !Num_objects) {
		cur_object_index = cur_ship = cur_wing = cur_waypoint_list = cur_waypoint = -1;
		return;
	}

	if (query_valid_object(cur_object_index))
		ptr = Objects[cur_object_index].next;
	else
		ptr = GET_FIRST(&obj_used_list);

	if (ptr == END_OF_LIST(&obj_used_list))
		ptr = ptr->next;

	Assert(ptr != END_OF_LIST(&obj_used_list));
	cur_object_index = OBJ_INDEX(ptr);
	Assert(ptr->type != OBJ_NONE);
	cur_ship = cur_wing = cur_waypoint_list = cur_waypoint = -1;
	if (ptr->type == OBJ_SHIP) {
		cur_ship = ptr->instance;
		cur_wing = Ships[cur_ship].wingnum;
		for (i=0; i<Wings[cur_wing].wave_count; i++)
			if (wing_objects[cur_wing][i] == cur_object_index) {
				cur_wing_index = i;
				break;
			}

	} else if (ptr->type == OBJ_WAYPOINT) {
		cur_waypoint_list = ptr->instance / 65536;
		cur_waypoint = ptr->instance & 0xffff;
	}
}

int update_dialog_boxes()
{
	int z;

	nprintf(("Fred routing", "updating dialog boxes\n"));

	// check wing first, since ships are dependent on wings, but not the reverse
	z = Wing_editor_dialog.update_data(0);
	if (z) {
		nprintf(("Fred routing", "wing dialog save failed\n"));
		Wing_editor_dialog.SetWindowPos(&Fred_main_wnd->wndTop, 0, 0, 0, 0,
			SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

		return z;
	}

	z = Ship_editor_dialog.update_data(0);
	if (z) {
		nprintf(("Fred routing", "ship dialog save failed\n"));
		Ship_editor_dialog.SetWindowPos(&Fred_main_wnd->wndTop, 0, 0, 0, 0,
			SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

		return z;
	}

	z = Waypoint_editor_dialog.update_data(0);
	if (z) {
		nprintf(("Fred routing", "waypoint dialog save failed\n"));
		Waypoint_editor_dialog.SetWindowPos(&Fred_main_wnd->wndTop, 0, 0, 0, 0,
			SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

		return z;
	}

	update_map_window();
	return 0;
}

int delete_object(int obj)
{
	int r;

	Ship_editor_dialog.bypass_all++;
	r = common_object_delete(obj);
	Ship_editor_dialog.bypass_all--;
	return r;
}

int delete_object(object *ptr)
{
	int r;

	Ship_editor_dialog.bypass_all++;
	r = common_object_delete(OBJ_INDEX(ptr));
	Ship_editor_dialog.bypass_all--;
	return r;
}

int delete_ship(int ship)
{
	int r;

	Ship_editor_dialog.bypass_all++;
	r = common_object_delete(Ships[ship].objnum);
	Ship_editor_dialog.bypass_all--;
	return r;
}

int common_object_delete(int obj)
{
	char msg[255], *name;
	int i, z, r, type;
	object *objp;

	type = Objects[obj].type;
	if (type == OBJ_START) {
		i = Objects[obj].instance;
		if (Player_starts < 2) {  // player 1 start
			Fred_main_wnd->MessageBox("Must have at least 1 player starting point!",
				NULL, MB_OK | MB_ICONEXCLAMATION);

			unmark_object(obj);
			return 1;
		}

		Assert((i >= 0) && (i < MAX_SHIPS));
		sprintf(msg, "Player %d", i + 1);
		name = msg;
		r = reference_handler(name, REF_TYPE_PLAYER, obj);
		if (r)
			return r;

		if (Ships[i].wingnum >= 0) {
			r = delete_ship_from_wing(i);
			if (r)
				return r;
		}

		Objects[obj].type = OBJ_SHIP;  // was allocated as a ship originally, so remove as such.
		invalidate_references(name, REF_TYPE_PLAYER);

		// check if any ship is docked with this ship and break dock if so
		while (object_is_docked(&Objects[obj]))
		{
			ai_do_objects_undocked_stuff(&Objects[obj], dock_get_first_docked_object(&Objects[obj]));
		}

		if (Player_start_shipnum == i) {  // need a new single player start.
			objp = GET_FIRST(&obj_used_list);
			while (objp != END_OF_LIST(&obj_used_list)) {
				if (objp->type == OBJ_START) {
					Player_start_shipnum = objp->instance;
					break;
				}

				objp = GET_NEXT(objp);
			}
		}

		Player_starts--;

	} else if (type == OBJ_WAYPOINT) {
		int list, count;

		list = Objects[obj].instance / 65536;
		i = Objects[obj].instance & 0xffff;
		Assert(list >= 0 && list < MAX_WAYPOINT_LISTS);
		count = Waypoint_lists[list].count;
		Assert(i >= 0 && i < count);

		if (Waypoint_lists[list].count == 1) {
			name = Waypoint_lists[list].name;
			r = reference_handler(name, REF_TYPE_PATH, obj);
			if (r)
				return r;
		}

		sprintf(msg, "%s:%d", Waypoint_lists[list].name, i + 1);
		name = msg;
		r = reference_handler(name, REF_TYPE_WAYPOINT, obj);
		if (r)
			return r;

		invalidate_references(name, REF_TYPE_WAYPOINT);
		objp = GET_FIRST(&obj_used_list);
		while (objp != END_OF_LIST(&obj_used_list)) {
			if ((objp->type == OBJ_WAYPOINT) && ((objp->instance / 65536) == list))
				if ((objp->instance & 0xffff) > i)
					objp->instance--;

			objp = GET_NEXT(objp);
		}

		while (i < count - 1) {
			Waypoint_lists[list].waypoints[i] = Waypoint_lists[list].waypoints[i + 1];
			i++;
		}

		Waypoint_lists[list].count--;
		if (!Waypoint_lists[list].count) {
			invalidate_references(Waypoint_lists[list].name, REF_TYPE_PATH);
			objp = GET_FIRST(&obj_used_list);
			while (objp != END_OF_LIST(&obj_used_list)) {
				if ((objp->type == OBJ_WAYPOINT) && ((objp->instance / 65536) > list))
					objp->instance -= 65536;

				objp = GET_NEXT(objp);
			}

			while (list < Num_waypoint_lists - 1) {
				Waypoint_lists[list] = Waypoint_lists[list + 1];
				list++;
			}

			Num_waypoint_lists--;
			Waypoint_lists[list].count = 0;
		}

	} else if (type == OBJ_SHIP) {
		name = Ships[Objects[obj].instance].ship_name;
		r = reference_handler(name, REF_TYPE_SHIP, obj);
		if (r)
			return r;

		z = Objects[obj].instance;
		if (Ships[z].wingnum >= 1) {
			invalidate_references(name, REF_TYPE_SHIP);
			r = delete_ship_from_wing(z);
			if (r)
				return r;

		} else if (Ships[z].wingnum >= 0) {
			r = delete_ship_from_wing(z);
			if (r)
				return r;

			invalidate_references(name, REF_TYPE_SHIP);
		}

		for (i=0; i<Num_reinforcements; i++)
			if (!stricmp(name, Reinforcements[i].name)) {
				delete_reinforcement(i);
				break;
			}

		// check if any ship is docked with this ship and break dock if so
		while (object_is_docked(&Objects[obj]))
		{
			ai_do_objects_undocked_stuff(&Objects[obj], dock_get_first_docked_object(&Objects[obj]));
		}

	} else if (type == OBJ_POINT) {
		Assert(Briefing_dialog);
		Briefing_dialog->delete_icon(Objects[obj].instance);
		Update_window = 1;
		return 0;

	} else if (type == OBJ_JUMP_NODE) {
		delete Objects[obj].jnp;
	}

	unmark_object(obj);

	//we need to call obj_delete() even if obj is a jump node
	//the statement "delete Objects[obj].jnp" deletes the jnp object
	//obj_delete() frees up the object slot where the node used to reside.
	//if we don't call this then the node will still show up in fred and you can try to delete it twice
	//this causes an ugly crash.
	obj_delete(obj);
	set_modified();
	Update_window = 1;
	return 0;
}

void delete_marked()
{
	object *ptr, *next;

	delete_flag = 0;
	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		next = GET_NEXT(ptr);
		if (ptr->flags & OF_MARKED)
			if (delete_object(ptr) == 2)  // user went to a reference, so don't get in the way.
				break;
		
		ptr = next;
	}

	if (!delete_flag)
		set_cur_object_index(-1);

	Update_window = 1;
}

void delete_reinforcement(int num)
{
	int i;

	for (i=num; i<Num_reinforcements-1; i++)
		Reinforcements[i] = Reinforcements[i + 1];

	Num_reinforcements--;
	set_modified();
}

// delete ship, removing it from its wing if necessary.
int delete_ship_from_wing(int ship)
{
	char name[NAME_LENGTH];
	int i, r, wing, end;

	wing = Ships[ship].wingnum;
	if (wing >= 0) {
		if (Wings[wing].wave_count == 1) {
			cur_wing = -1;
			Update_wing = 1;
			r = delete_wing(wing, 1);
			if (r) {
				if (r == 2){
					delete_flag = 1;
				}

				return r;
			}
		
		} else {
			i = Wings[wing].wave_count;
			end = i - 1;
			while (i--){
				if (wing_objects[wing][i] == Ships[ship].objnum){
					break;
				}
			}

			Assert(i != -1);  // Error, object should be in wing.
			if (Wings[wing].special_ship == i){
				Wings[wing].special_ship = 0;
			} else if (Wings[wing].special_ship > i) {
				Wings[wing].special_ship--;
			}

			if (i != end) {
				wing_objects[wing][i] = wing_objects[wing][end];
				Wings[wing].ship_index[i] = Wings[wing].ship_index[end];
				if (Objects[wing_objects[wing][i]].type == OBJ_SHIP) {
					sprintf(name, "%s %d", Wings[wing].name, i + 1);
					rename_ship(Wings[wing].ship_index[i], name);
				}
			}

			if (Wings[wing].threshold >= Wings[wing].wave_count){
				Wings[wing].threshold = Wings[wing].wave_count - 1;
			}

			Wings[wing].wave_count--;
			if (Wings[wing].wave_count && (Wings[wing].threshold >= Wings[wing].wave_count)){
				Wings[wing].threshold = Wings[wing].wave_count - 1;
			}
		}
	}

	set_modified();
	return 0;
}

// What does this do?
void add_ship_to_wing()
{
	int		org_object = cur_object_index;
	vec3d	tvec;

	set_cur_object_index();
	if (Objects[org_object].type == OBJ_NONE) {
		vm_vec_make(&tvec, 10.0f, 10.0f, 10.0f);
		create_object(&tvec);
	
	} else {
		Objects[cur_object_index] = Objects[org_object];
		Objects[cur_object_index].pos.xyz.x += 3.0f;
		Objects[cur_object_index].pos.xyz.y += 3.0f;
		physics_init(&Objects[cur_object_index].phys_info);
		Objects[cur_object_index].orient = Objects[org_object].orient;
	}

	set_modified();
}

//	Return true if current object is valid and is in a wing.
//	Else return false.
int query_object_in_wing(int obj)
{
	if (query_valid_object(obj)){
		if (Ships[Objects[obj].instance].wingnum != -1){
			return TRUE;
		}
	}
	
	return FALSE;
}

void mark_object(int obj)
{
	Assert(query_valid_object(obj));
	if (!(Objects[obj].flags & OF_MARKED)) {
		Objects[obj].flags |= OF_MARKED;  // set as marked
		Marked++;
		Update_window = 1;
		if (cur_object_index == -1){
			set_cur_object_index(obj);
		}
		Update_ship = Update_wing = 1;
		Waypoint_editor_dialog.initialize_data(1);
	}
}

void unmark_object(int obj)
{
	Assert(query_valid_object(obj));
	if (Objects[obj].flags & OF_MARKED) {
		Objects[obj].flags &= ~OF_MARKED;
		Marked--;
		Update_window = 1;
		if (obj == cur_object_index) {  // need to find a new index
			object *ptr;

			ptr = GET_FIRST(&obj_used_list);
			while (ptr != END_OF_LIST(&obj_used_list)) {
				if (ptr->flags & OF_MARKED) {
					set_cur_object_index(OBJ_INDEX(ptr));  // found one
					return;
				}

				ptr = GET_NEXT(ptr);
			}

			set_cur_object_index(-1);  // can't find one; nothing is marked.
		}
		Update_ship = Update_wing = 1;
		Waypoint_editor_dialog.initialize_data(1);
	}
}

// clears the marked flag of all objects (so nothing is marked)
void unmark_all()
{
	int i;

	if (Marked) {
		for (i=0; i<MAX_OBJECTS; i++){
			Objects[i].flags &= ~OF_MARKED;
		}

		Marked = 0;
		Update_window = 1;
		set_cur_object_index(-1);
	}
}

void clear_menu(CMenu *ptr)
{
	int count;

	count = ptr->GetMenuItemCount();
	while (count--){
		ptr->DeleteMenu(count, MF_BYPOSITION);
	}
}

void generate_wing_popup_menu(CMenu *mptr, int first_id, int state)
{
	int i, z, columns, rows, count;

	columns = 1;
	rows = Num_wings;
	while (rows > 25) {
		columns++;
		rows = Num_wings / columns;
	}

	count = rows + 1;
	for (i=0; i<MAX_WINGS; i++){
		if (Wings[i].wave_count) {
			z = state | MF_STRING;
			if (!count--) {
				count = rows;
				z |= MF_MENUBARBREAK;
			}

			mptr->AppendMenu(z, first_id + i, Wings[i].name);
		}
	}

	mptr->DeleteMenu(ID_PLACEHOLDER, MF_BYCOMMAND);
}

void generate_ship_popup_menu(CMenu *mptr, int first_id, int state, int filter)
{
	int z, ship, columns, rows, count, num_ships;
	object *ptr;

	columns = 1;
	num_ships = ship_get_num_ships();
	rows = num_ships;
	while (rows > 25) {
		columns++;
		rows = num_ships / columns;
	}

	count = rows + 1;
	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		if ((ptr->type == OBJ_SHIP) || ((ptr->type == OBJ_START) && (filter & SHIP_FILTER_PLAYERS))) {
			z = 1;
			if (filter & SHIP_FILTER_FLYABLE) {
				if (Ship_info[Ships[get_ship_from_obj(ptr)].ship_info_index].flags & SIF_NOT_FLYABLE){
					z = 0;
				}
			}

			if (z) {
				z = state | MF_STRING;
				if (!count--) {
					count = rows;
					z |= MF_MENUBARBREAK;
				}

				ship = ptr->instance;
				mptr->AppendMenu(z, first_id + ship, Ships[ship].ship_name);
			}
		}

		ptr = GET_NEXT(ptr);
	}

	mptr->DeleteMenu(ID_PLACEHOLDER, MF_BYCOMMAND);
}

// Alternate string lookup function, taking a CString instead.  The reason that it's here,
// instead of parselo.cpp, is because the class CString require an include of windows.h,
// which everyone wants to avoid including in any freespace header files.  So..
int string_lookup(CString str1, char *strlist[], int max)
{
	int	i;

	for (i=0; i<max; i++) {
		Assert(strlen(strlist[i]));

		if (!stricmp(str1, strlist[i])){
			return i;
		}
	}

	return -1;
}

int gray_menu_tree(CMenu *base)
{
	int i, z, count = 0;
	CMenu *submenu;

	i = base->GetMenuItemCount();
	while (i--) {
		if ((submenu = base->GetSubMenu(i))>0) {
			if (gray_menu_tree(submenu)) {
				count++;
			} else {
				base->EnableMenuItem(i, MF_GRAYED | MF_BYPOSITION);
			}

		} else {
			z = base->GetMenuState(i, MF_BYPOSITION);
			if (z == MF_ENABLED){
				count++;
			}
		}
	}

	return count;
}

int query_initial_orders_conflict(int wing)
{
	int i, z;

	Assert(wing != -1);
	if (wing == -1){
		return 0;
	}

	if (query_initial_orders_empty(Wings[wing].ai_goals)){
		return 0;
	}

	i = Wings[wing].wave_count;  // wing has orders, now check ships.
	while (i--) {
		z = Ships[Objects[wing_objects[wing][i]].instance].ai_index;
		if (!query_initial_orders_empty(Ai_info[z].goals)){  // ship also has orders
			return 1;
		}
	}

	return 0;
}

int query_initial_orders_empty(ai_goal *ai_goals)
{
	int i;

	for (i=0; i<MAX_AI_GOALS; i++){
		if (ai_goals[i].ai_mode != AI_GOAL_NONE){
			return 0;
		}
	}

	return 1;
}

int set_reinforcement(char *name, int state)
{
	int i, index, cur = -1;

	for (i=0; i<Num_reinforcements; i++){
		if (!stricmp(Reinforcements[i].name, name)){
			cur = i;
		}
	}

	if (!state && (cur != -1)) {
		Num_reinforcements--;
		Reinforcements[cur] = Reinforcements[Num_reinforcements];

		// clear the ship/wing flag for this reinforcement
		index = ship_name_lookup(name);
		if ( index != -1 ){
			Ships[index].flags &= ~SF_REINFORCEMENT;
		} else {
			index = wing_name_lookup(name);
			if ( index != -1 ){
				Wings[index].flags &= ~WF_REINFORCEMENT;
			}
		}
		if (index == -1 ){
			Int3();				// get allender -- coudln't find ship/wing for clearing reinforcement flag
		}

		set_modified();
		return -1;
	}

	if (state && (cur == -1) && (Num_reinforcements < MAX_REINFORCEMENTS)) {
		Assert(strlen(name) < NAME_LENGTH);
		strcpy(Reinforcements[Num_reinforcements].name, name);
		Reinforcements[Num_reinforcements].uses = 1;
		Reinforcements[Num_reinforcements].arrival_delay = 0;
		memset( Reinforcements[Num_reinforcements].no_messages, 0, MAX_REINFORCEMENT_MESSAGES * NAME_LENGTH );
		memset( Reinforcements[Num_reinforcements].yes_messages, 0, MAX_REINFORCEMENT_MESSAGES * NAME_LENGTH );
		Num_reinforcements++;

		// set the reinforcement flag on the ship or wing
		index = ship_name_lookup(name);
		if ( index != -1 ){
			Ships[index].flags |= SF_REINFORCEMENT;
		} else {
			index = wing_name_lookup(name);
			if ( index != -1 ){
				Wings[index].flags |= WF_REINFORCEMENT;
			}
		}
		if ( index == -1 ){
			Int3();				// get allender -- coudln't find ship/wing for setting reinforcement flag
		}

		set_modified();
		return 1;
	}

	// this code will take care of setting the bits for the ship/wing flags
	if ( state && (cur != -1) ) {
		// set the reinforcement flag on the ship or wing
		index = ship_name_lookup(name);
		if ( index != -1 ){
			Ships[index].flags |= SF_REINFORCEMENT;
		} else {
			index = wing_name_lookup(name);
			if ( index != -1 ){
				Wings[index].flags |= WF_REINFORCEMENT;
			}
		}
		if ( index == -1 ){
			Int3();				// get allender -- coudln't find ship/wing for setting reinforcement flag
		}
	}

	return 0;
}

int get_docking_list(int model_index)
{
	int i;
	polymodel *pm;

	pm = model_get(model_index);
	Assert(pm->n_docks <= MAX_DOCKS);
	for (i=0; i<pm->n_docks; i++)
		Docking_bay_list[i] = pm->docking_bays[i].name;

	return pm->n_docks;
}

// DA 1/7/99 These ship names are not variables
int rename_ship(int ship, char *name)
{
	int i;

	Assert(ship >= 0);
	Assert(strlen(name) < NAME_LENGTH);

	update_sexp_references(Ships[ship].ship_name, name);
	ai_update_goal_references(REF_TYPE_SHIP, Ships[ship].ship_name, name);
	for (i=0; i<Num_reinforcements; i++)
		if (!stricmp(Ships[ship].ship_name, Reinforcements[i].name)) {
			strcpy(Reinforcements[i].name, name);
		}

	strcpy(Ships[ship].ship_name, name);
	if (ship == cur_ship)
		Ship_editor_dialog.m_ship_name = _T(name);

	return 0;
}

int invalidate_references(char *name, int type)
{
	char new_name[512];
	int i;

	sprintf(new_name, "<%s>", name);
	update_sexp_references(name, new_name);
	ai_update_goal_references(type, name, new_name);
	for (i=0; i<Num_reinforcements; i++)
		if (!stricmp(name, Reinforcements[i].name)) {
			strcpy(Reinforcements[i].name, new_name);
		}

	return 0;
}

int internal_integrity_check()
{
	int i;

	for (i=0; i<Num_mission_events; i++)
		verify_sexp_tree(Mission_events[i].formula);

	for (i=0; i<Num_goals; i++)
		verify_sexp_tree(Mission_goals[i].formula);

	for (i=0; i<MAX_WINGS; i++)
		if (Wings[i].wave_count) {
			verify_sexp_tree(Wings[i].arrival_cue);
			verify_sexp_tree(Wings[i].departure_cue);
		}

	for (i=0; i<MAX_SHIPS; i++)
		if (Ships[i].objnum >= 0) {
			verify_sexp_tree(Ships[i].arrival_cue);
			verify_sexp_tree(Ships[i].departure_cue);
			if (Ships[i].ai_index < 0)
				Assert(0);
			if (Ai_info[Ships[i].ai_index].shipnum != i)
				Int3();
		}

	return 0;
}

void correct_marking()
{
	object *ptr;

	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		if (ptr->flags & OF_MARKED) {
			if (ptr->flags & OF_HIDDEN)
				unmark_object(OBJ_INDEX(ptr));

			else switch (ptr->type) {
				case OBJ_WAYPOINT:
					if (!Show_waypoints)
						unmark_object(OBJ_INDEX(ptr));
					break;

				case OBJ_START:
					if (!Show_starts || !Show_ships)
						unmark_object(OBJ_INDEX(ptr));
					break;

				case OBJ_SHIP:
					if (!Show_ships)
						unmark_object(OBJ_INDEX(ptr));

					if (!Show_iff[Ships[ptr->instance].team])
						unmark_object(OBJ_INDEX(ptr));

					break;
			}
		}

		ptr = GET_NEXT(ptr);
	}
}

// Fills a combo box with a list of all docking points of type 'type' on ship 'ship'.
// Item data is the actual docking point index.
void set_valid_dock_points(int ship, int type, CComboBox *box)
{
	int i, z, num, model;

	model = Ship_info[Ships[ship].ship_info_index].model_num;
	num = model_get_num_dock_points(model);
	for (i=0; i<num; i++)
		if (model_get_dock_index_type(model, i) & type) {
			z = box->AddString(model_get_dock_name(model, i));
			box->SetItemData(z, i);
		}

	Assert(box->GetCount());
}

// Given an object index, find the ship index for that object.
int get_ship_from_obj(int obj)
{
	if ((Objects[obj].type == OBJ_SHIP) || (Objects[obj].type == OBJ_START))
		return Objects[obj].instance;

	Int3();
	return 0;
}

int get_ship_from_obj(object *objp)
{
	if ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START))
		return objp->instance;

	Int3();
	return 0;
}

void ai_update_goal_references(int type, char *old_name, char *new_name)
{
	int i;

	for (i=0; i<MAX_AI_INFO; i++)  // loop through all Ai_info entries
		if (Ai_info[i].shipnum != -1)  // skip if unused
			ai_update_goal_references(Ai_info[i].goals, type, old_name, new_name);

	for (i=0; i<MAX_WINGS; i++)
		if (Wings[i].wave_count)
			ai_update_goal_references(Wings[i].ai_goals, type, old_name, new_name);
}

int query_referenced_in_ai_goals(int type, char *name)
{
	int i;

	for (i=0; i<MAX_AI_INFO; i++)  // loop through all Ai_info entries
		if (Ai_info[i].shipnum >= 0)  // skip if unused
			if (query_referenced_in_ai_goals(Ai_info[i].goals, type, name))
				return Ai_info[i].shipnum | SRC_SHIP_ORDER;

	for (i=0; i<MAX_WINGS; i++)
		if (Wings[i].wave_count)
			if (query_referenced_in_ai_goals(Wings[i].ai_goals, type, name))
				return i | SRC_WING_ORDER;

	return 0;
}

int advanced_stricmp(char *one, char *two)
{
	if (!one && !two)
		return 0;

	if (!one)
		return -1;

	if (!two)
		return 1;

	return stricmp(one, two);
}

// returns 0: go ahead change object
//			  1: don't change it
//			  2: abort (they used cancel to go to reference)
int reference_handler(char *name, int type, int obj)
{
	char msg[2048], text[128], type_name[128];
	int r, n, node;

	switch (type) {
		case REF_TYPE_SHIP:
			sprintf(type_name, "Ship \"%s\"", name);
			break;

		case REF_TYPE_WING:
			sprintf(type_name, "Wing \"%s\"", name);
			break;

		case REF_TYPE_PLAYER:
			strcpy(type_name, name);
			break;

		case REF_TYPE_WAYPOINT:
			sprintf(type_name, "Waypoint \"%s\"", name);
			break;

		case REF_TYPE_PATH:
			sprintf(type_name, "Waypoint path \"%s\"", name);
			break;

		default:
			Error(LOCATION, "Type unknown for object \"%s\".  Let Hoffos know now!", name);
	}

	r = query_referenced_in_sexp(type, name, &node);
	if (r) {
		n = r & SRC_DATA_MASK;
		switch (r & SRC_MASK) {
			case SRC_SHIP_ARRIVAL:
				sprintf(text, "the arrival cue of ship \"%s\"", Ships[n].ship_name);
				break;

			case SRC_SHIP_DEPARTURE:
				sprintf(text, "the departure cue of ship \"%s\"", Ships[n].ship_name);
				break;

			case SRC_WING_ARRIVAL:
				sprintf(text, "the arrival cue of wing \"%s\"", Wings[n].name);
				break;

			case SRC_WING_DEPARTURE:
				sprintf(text, "the departure cue of wing \"%s\"", Wings[n].name);
				break;

			case SRC_EVENT:
				if (*Mission_events[n].name)
					sprintf(text, "event \"%s\"", Mission_events[n].name);
				else
					sprintf(text, "event #%d", n);

				break;

			case SRC_MISSION_GOAL:
				if (*Mission_goals[n].name)
					sprintf(text, "mission goal \"%s\"", Mission_goals[n].name);
				else
					sprintf(text, "mission goal #%d", n);

				break;

			case SRC_DEBRIEFING:
				sprintf(text, "debriefing #%d", n);
				break;

			case SRC_BRIEFING:
				sprintf(text, "briefing #%d", n);
				break;

			default:  // very bad.  Someone added an sexp somewhere and didn't change this.
				Warning(LOCATION, "\"%s\" referenced by an unknown sexp source!  "
					"Run for the hills and let Hoffoss know right now!", name);

				delete_flag = 1;
				return 2;
		}

		sprintf(msg, "%s is referenced by %s (possibly more sexps).\n"
			"Do you want to delete it anyway?\n\n"
			"(click Cancel to go to the reference)", type_name, text);

		r = sexp_reference_handler(node, r, msg);
		if (r == 1) {
			if (obj >= 0)
				unmark_object(obj);

			return 1;
		}

		if (r == 2) {
			delete_flag = 1;
			return 2;
		}
	}

	r = query_referenced_in_ai_goals(type, name);
	if (r) {
		n = r & SRC_DATA_MASK;
		switch (r & SRC_MASK) {
			case SRC_SHIP_ORDER:
				sprintf(text, "ship \"%s\"", Ships[n].ship_name);
				break;

			case SRC_WING_ORDER:
				sprintf(text, "wing \"%s\"", Wings[n].name);
				break;

			default:  // very bad.  Someone added an sexp somewhere and didn't change this.
				Error(LOCATION, "\"%s\" referenced by an unknown initial orders source!  "
					"Run for the hills and let Hoffoss know right now!", name);
		}

		sprintf(msg, "%s is referenced by the initial orders of %s (possibly \n"
			"more initial orders).  Do you want to delete it anyway?\n\n"
			"(click Cancel to go to the reference)", type_name, text);

		r = orders_reference_handler(r, msg);
		if (r == 1) {
			if (obj >= 0)
				unmark_object(obj);

			return 1;
		}

		if (r == 2) {
			delete_flag = 1;
			return 2;
		}
	}

	if ((type != REF_TYPE_SHIP) && (type != REF_TYPE_WING))
		return 0;

	for (n=0; n<Num_reinforcements; n++)
		if (!stricmp(name, Reinforcements[n].name))
			break;

	if (n < Num_reinforcements) {
		sprintf(msg, "Ship \"%s\" is a reinforcement unit.\n"
			"Do you want to delete it anyway?", name);

		r = Fred_main_wnd->MessageBox(msg, NULL, MB_YESNO | MB_ICONEXCLAMATION);
		if (r == IDNO) {
			if (obj >= 0)
				unmark_object(obj);

			return 1;
		}
	}

	return 0;
}

int orders_reference_handler(int code, char *msg)
{
	int r, n;

	r = Fred_main_wnd->MessageBox(msg, "Warning", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
	if (r == IDNO)
		return 1;

	if (r == IDYES)
		return 0;

	ShipGoalsDlg dlg_goals;

	n = code & SRC_DATA_MASK;
	switch (code & SRC_MASK) {
		case SRC_SHIP_ORDER:
			unmark_all();
			mark_object(Ships[n].objnum);

			dlg_goals.self_ship = n;
			dlg_goals.DoModal();
			if (!query_initial_orders_empty(Ai_info[Ships[n].ai_index].goals))
				if ((Ships[n].wingnum >= 0) && (query_initial_orders_conflict(Ships[n].wingnum)))
					Fred_main_wnd->MessageBox("This ship's wing also has initial orders", "Possible conflict");

			break;

		case SRC_WING_ORDER:
			unmark_all();
			mark_wing(n);

			dlg_goals.self_wing = n;
			dlg_goals.DoModal();
			if (query_initial_orders_conflict(n))
				Fred_main_wnd->MessageBox("One or more ships of this wing also has initial orders", "Possible conflict");

			break;

		default:  // very bad.  Someone added an sexp somewhere and didn't change this.
			Error(LOCATION, "Unknown initial order reference source");
	}

	delete_flag = 1;
	return 2;
}

int sexp_reference_handler(int node, int code, char *msg)
{
	int r;

	r = Fred_main_wnd->MessageBox(msg, "Warning", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
	if (r == IDNO)
		return 1;

	if (r == IDYES)
		return 0;

	switch (code & SRC_MASK) {
		case SRC_SHIP_ARRIVAL:
		case SRC_SHIP_DEPARTURE:
			if (!Ship_editor_dialog.GetSafeHwnd())
				Ship_editor_dialog.Create();

			Ship_editor_dialog.SetWindowPos(&Fred_main_wnd->wndTop, 0, 0, 0, 0,
				SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
			Ship_editor_dialog.ShowWindow(SW_RESTORE);

			Ship_editor_dialog.select_sexp_node = node;
			unmark_all();
			mark_object(Ships[code & SRC_DATA_MASK].objnum);
			break;

		case SRC_WING_ARRIVAL:
		case SRC_WING_DEPARTURE:
			if (!Wing_editor_dialog.GetSafeHwnd())
				Wing_editor_dialog.Create();

			Wing_editor_dialog.SetWindowPos(&Fred_main_wnd->wndTop, 0, 0, 0, 0,
				SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
			Wing_editor_dialog.ShowWindow(SW_RESTORE);

			Wing_editor_dialog.select_sexp_node = node;
			unmark_all();
			mark_wing(code & SRC_DATA_MASK);
			break;

		case SRC_EVENT:
			if (Message_editor_dlg) {
				Fred_main_wnd->MessageBox("You must close the message editor before the event editor can be opened");
				break;
			}

			if (!Event_editor_dlg) {
				Event_editor_dlg = new event_editor;
				Event_editor_dlg->select_sexp_node = node;
				Event_editor_dlg->Create(event_editor::IDD);
			}

			Event_editor_dlg->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
			Event_editor_dlg->ShowWindow(SW_RESTORE);
			break;

		case SRC_MISSION_GOAL: {
			CMissionGoalsDlg dlg;

			dlg.select_sexp_node = node;
			dlg.DoModal();
			break;
		}

		case SRC_DEBRIEFING: {
			debriefing_editor_dlg dlg;

			dlg.select_sexp_node = node;
			dlg.DoModal();
			break;
		}

		case SRC_BRIEFING: {
			if (!Briefing_dialog) {
				Briefing_dialog = new briefing_editor_dlg;
				Briefing_dialog->create();
			}

			Briefing_dialog->SetWindowPos(&Briefing_dialog->wndTop, 0, 0, 0, 0,
				SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
			Briefing_dialog->ShowWindow(SW_RESTORE);
			Briefing_dialog->focus_sexp(node);
			break;
		}

		default:  // very bad.  Someone added an sexp somewhere and didn't change this.
			Error(LOCATION, "Unknown sexp reference source");
	}

	delete_flag = 1;
	return 2;
}

char *object_name(int obj)
{
	static char text[80];
	int i;

	if (!query_valid_object(obj))
		return "*none*";

	i = Objects[obj].instance;
	switch (Objects[obj].type) {
		case OBJ_SHIP:
		case OBJ_START:
			return Ships[i].ship_name;

		case OBJ_WAYPOINT:
			sprintf(text, "%s:%d", Waypoint_lists[i / 65536].name, (i & 0xffff) + 1);
			return text;

		case OBJ_POINT:
			return "Briefing icon";
	}

	return "*unknown*";
}

char *get_order_name(int order)
{
	int i;

	if (order == AI_GOAL_NONE)  // special case
		return "None";

	for (i=0; i<Ai_goal_list_size; i++)
		if (Ai_goal_list[i].def & order)
			return Ai_goal_list[i].name;

	return "???";
}

void object_moved(object *objp)
{
	if (objp->type == OBJ_WAYPOINT)
		Waypoint_lists[objp->instance / 65536].waypoints[objp->instance & 0xffff] = objp->pos;

	if ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START)) // do we have a ship?
	{
		// reset the already-handled flag (inefficient, but it's FRED, so who cares)
		for (int i = 0; i < MAX_OBJECTS; i++)
			Objects[i].flags &= ~OF_DOCKED_ALREADY_HANDLED;

		// move all docked objects docked to me
		dock_move_docked_objects(objp);
	}
}

// determine if all the ships in a given wing are all marked or not.
int query_whole_wing_marked(int wing)
{
	int count = 0;
	object *ptr;

	if (!Wings[wing].wave_count)
		return 0;

	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		if (ptr->flags & OF_MARKED)
			if ((ptr->type == OBJ_SHIP) || (ptr->type == OBJ_START))
				if (Ships[get_ship_from_obj(ptr)].wingnum == wing)
					count++;

		ptr = GET_NEXT(ptr);
	}

	if (count == Wings[wing].wave_count)
		return 1;

	return 0;
}

void generate_weaponry_usage_list(int *arr, int wing)
{
	int i, j;
	ship_weapon *swp;

	if (wing < 0)
		return;

	i = Wings[wing].wave_count;
	while (i--) {
		swp = &Ships[Wings[wing].ship_index[i]].weapons;
		j = swp->num_primary_banks;
		while (j--)
			arr[swp->primary_bank_weapons[j]]++;

		j = swp->num_secondary_banks;
		while (j--)
			arr[swp->secondary_bank_weapons[j]] += int(ceil(swp->secondary_bank_ammo[j] * swp->secondary_bank_capacity[j] / 100 / Weapon_info[swp->secondary_bank_weapons[j]].cargo_size));
	}
}

void generate_weaponry_usage_list(int *arr)
{
	int i;

	for (i=0; i<MAX_WEAPON_TYPES; i++)
		arr[i] = 0;

	// Goober5000 - I have no idea why :V: did things this way (or even what exactly they're doing);
	// I'm guessing Zeta needs to be accounted for somewhere but I'm not sure how to do that
	for (i=0; i<MAX_STARTING_WINGS; i++)
		generate_weaponry_usage_list(arr, Starting_wings[i]);
}

jump_node *jumpnode_get_by_name(CString& name)
{
	jump_node *jnp = jumpnode_get_by_name(name.GetBuffer(name.GetLength()));
	name.ReleaseBuffer();

	return jnp;
}

// function which adds all current ships in the Fred mission to the passed in combo box.  useful for
// building up ship lists for arrival/departure targets
void management_add_ships_to_combo( CComboBox *box, int flags )
{
	int get_special_anchor(char *name);
	object *objp;
	int id, i, restrict_to_players;

	box->ResetContent();
	
	// add the "special" targets, i.e. any friendly, any hostile, etc.
	if (flags & SHIPS_2_COMBO_SPECIAL)
	{
		for (restrict_to_players = 0; restrict_to_players < 2; restrict_to_players++)
		{
			for (i = 0; i < Num_iffs; i++)
			{
				char tmp[NAME_LENGTH + 15];
				stuff_special_arrival_anchor_name(tmp, i, restrict_to_players, 0);

				id = box->AddString(tmp);
				box->SetItemData(id, get_special_anchor(tmp));
			}
		}
	}

	// either add all ships to the list, or only add ships with docking bays.
	if ( flags & SHIPS_2_COMBO_ALL_SHIPS ) {
		for ( objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list); objp = GET_NEXT(objp) ) {
			if ( ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START)) && !(objp->flags & OF_MARKED) ) {
				id = box->AddString(Ships[get_ship_from_obj(objp)].ship_name);
				box->SetItemData(id, get_ship_from_obj(objp));
			}
		}
	} else if ( flags & SHIPS_2_COMBO_DOCKING_BAY_ONLY ) {
		for ( objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list); objp = GET_NEXT(objp) ) {
			if ( ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START)) && !(objp->flags & OF_MARKED) ) {
				polymodel *pm;

				// determine if this ship has a docking bay
				pm = model_get( Ship_info[Ships[objp->instance].ship_info_index].model_num );
				Assert( pm );
				if ( pm->ship_bay && (pm->ship_bay->num_paths > 0) ) {
					id = box->AddString(Ships[get_ship_from_obj(objp)].ship_name);
					box->SetItemData(id, get_ship_from_obj(objp));
				}
			}
		}
	}
}

char *reg_read_string( char *section, char *name, char *default_value )
{
	HKEY hKey = NULL;
	DWORD dwType, dwLen;
	char keyname[1024];
	static char tmp_string_data[1024];
	LONG lResult;

	strcpy( keyname, section );

	lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,							// Where it is
											 keyname,								// name of key
											 NULL,									// DWORD reserved
											 KEY_QUERY_VALUE,						// Allows all changes
											 &hKey );								// Location to store key

	if ( lResult != ERROR_SUCCESS )	{
		mprintf(( "Error opening registry key '%s'\n", keyname ));
		goto Cleanup;
	}

	if ( !name )	 {
		mprintf(( "No variable name passed\n" ));
		goto Cleanup;
	}

	dwLen = 1024;
	lResult = RegQueryValueEx( hKey,									// Handle to key
									 name,											// The values name
									 NULL,											// DWORD reserved
	                         &dwType,										// What kind it is
									 (ubyte *)&tmp_string_data,						// value to set
									 &dwLen );								// How many bytes to set
																				
	if ( lResult != ERROR_SUCCESS )	{
		mprintf(( "Error reading registry key '%s'\n", name ));
		goto Cleanup;
	}

	default_value = tmp_string_data;

Cleanup:
	if ( hKey )
		RegCloseKey(hKey);

	return default_value;
}

// Goober5000
int wing_is_player_wing(int wing)
{
	int i;

	if (wing < 0)
		return 0;

	if (The_mission.game_type & MISSION_TYPE_MULTI_TEAMS)
	{
		for (i=0; i<MAX_TVT_WINGS; i++)
		{
			if (wing == TVT_wings[i])
				return 1;
		}
	}
	else
	{
		for (i=0; i<MAX_STARTING_WINGS; i++)
		{
			if (wing == Starting_wings[i])
				return 1;
		}
	}

	return 0;
}

// Goober5000
// This must be done when either the wing name or the custom name is changed.
// (It's also duplicated in FS2, in post_process_mission, for setting the indexes at mission load.)
void update_custom_wing_indexes()
{
	int i;

	for (i = 0; i < MAX_STARTING_WINGS; i++)
	{
		Starting_wings[i] = wing_name_lookup(Starting_wing_names[i], 1);
	}

	for (i = 0; i < MAX_SQUADRON_WINGS; i++)
	{
		Squadron_wings[i] = wing_name_lookup(Squadron_wing_names[i], 1);
	}

	for (i = 0; i < MAX_TVT_WINGS; i++)
	{
		TVT_wings[i] = wing_name_lookup(TVT_wing_names[i], 1);
	}
}

// Goober5000
void stuff_special_arrival_anchor_name(char *buf, int iff_index, int restrict_to_players, int retail_format)
{
	char *iff_name = Iff_info[iff_index].iff_name;

	// stupid retail hack
	if (retail_format && !stricmp(iff_name, "hostile") && !restrict_to_players)
		iff_name = "enemy";

	if (restrict_to_players)
		sprintf(buf, "<any %s player>", iff_name);
	else
		sprintf(buf, "<any %s>", iff_name);

	strlwr(buf);
}

// Goober5000
void stuff_special_arrival_anchor_name(char *buf, int anchor_num, int retail_format)
{
	// filter out iff
	int iff_index = anchor_num;
	iff_index &= ~SPECIAL_ARRIVAL_ANCHOR_FLAG;
	iff_index &= ~SPECIAL_ARRIVAL_ANCHOR_PLAYER_FLAG;

	// filter players
	int restrict_to_players = (anchor_num & SPECIAL_ARRIVAL_ANCHOR_PLAYER_FLAG);

	// get name
	stuff_special_arrival_anchor_name(buf, iff_index, restrict_to_players, retail_format);
}
