/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/Mission/MissionMessage.cpp $
 * $Revision: 2.52.2.14 $
 * $Date: 2007-12-08 20:07:54 $
 * $Author: karajorma $
 *
 * Controls messaging to player during the mission
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.52.2.13  2007/09/02 02:07:44  Goober5000
 * added fixes for #1415 and #1483, made sure every read_file_text had a corresponding setjmp, and sync'd the parse error messages between HEAD and stable
 *
 * Revision 2.52.2.12  2007/07/24 05:08:08  Goober5000
 * allow mission messages in dogfights (Mantis #1436)
 *
 * Revision 2.52.2.11  2007/07/23 16:08:26  Kazan
 * Autopilot updates, minor misc fixes, working MSVC2005 project files
 *
 * Revision 2.52.2.10  2007/05/26 12:08:18  Goober5000
 * when importing FS1 missions, account for the shuffled Head-TP4
 *
 * Revision 2.52.2.9  2007/04/13 00:36:11  taylor
 * cleanup and initialize persona_index properly for general safety reasons
 * slight change to karajorma's NO_BUILTIN_MESSAGES check, now it will be properly compiled away in release builds rather than an empty function
 *
 * Revision 2.52.2.8  2007/04/05 16:33:25  karajorma
 * What was I thinking? Fixed
 *
 * Revision 2.52.2.7  2007/03/21 20:54:25  karajorma
 * Bump the number of debriefing stages.
 * Fix an annoying (and erroneous) warning in the campaign editor.
 *
 * Revision 2.52.2.6  2007/02/11 09:07:33  taylor
 * little bit of debug message cleanup
 * don't bother dealing with message wav file if sound is disabled
 *
 * Revision 2.52.2.5  2006/10/24 13:36:05  taylor
 * fix Personas memory leak (not really an issue, I just got tired of seeing it in the Valgrind reports)
 *
 * Revision 2.52.2.4  2006/09/30 21:58:05  Goober5000
 * more flexible checking of generic messages
 *
 * Revision 2.52.2.3  2006/09/11 01:16:31  taylor
 * fixes for stuff_string() bounds checking
 *
 * Revision 2.52.2.2  2006/08/27 18:12:41  taylor
 * make Species_info[] and Asteroid_info[] dynamic
 *
 * Revision 2.52.2.1  2006/07/20 00:41:26  Goober5000
 * add WCS screaming stuff to RC branch
 * --Goober5000
 *
 * Revision 2.53  2006/07/06 20:46:39  Goober5000
 * WCS screaming stuff
 * --Goober5000
 *
 * Revision 2.52  2006/05/27 17:00:02  taylor
 * only display that "no wavefile for voice" message if sound is enabled
 *
 * Revision 2.51  2006/04/20 06:32:07  Goober5000
 * proper capitalization according to Volition
 *
 * Revision 2.50  2006/04/05 16:12:41  karajorma
 * Changes to support the new Enable/Disable-Builtin-Messages SEXP
 *
 * Revision 2.49  2006/01/27 06:21:10  Goober5000
 * replace quick sort with insertion sort in many places
 * --Goober5000
 *
 * Revision 2.48  2006/01/26 04:01:58  Goober5000
 * spelling
 *
 * Revision 2.47  2006/01/13 03:31:09  Goober5000
 * übercommit of custom IFF stuff :)
 *
 * Revision 2.46  2005/10/10 17:21:05  taylor
 * remove NO_NETWORK
 *
 * Revision 2.45  2005/09/25 05:13:07  Goober5000
 * hopefully complete species upgrade
 * --Goober5000
 *
 * Revision 2.44  2005/09/24 07:07:16  Goober5000
 * another species overhaul
 * --Goober5000
 *
 * Revision 2.43  2005/08/25 22:40:03  taylor
 * basic cleaning, removing old/useless code, sanity stuff, etc:
 *  - very minor performance boost from not doing stupid things :)
 *  - minor change to 3d shockwave sizing to better approximate 2d effect movements
 *  - for shields, Gobal_tris was only holding half as many as the game can/will use, buffer is now set to full size to avoid possible rendering issues
 *  - removed extra tcache_set on OGL spec map code, not sure how that slipped in
 *
 * Revision 2.42  2005/08/11 12:18:53  taylor
 * handle rare cases where anims would get freaky if one happened to still be playing or otherwise queued
 *
 * Revision 2.41  2005/07/22 10:18:39  Goober5000
 * CVS header tweaks
 * --Goober5000
 *
 * Revision 2.40  2005/07/13 03:25:59  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.39  2005/07/13 02:01:29  Goober5000
 * fixed a bunch of "issues" caused by me with the species stuff
 * --Goober5000
 *
 * Revision 2.38  2005/07/13 00:44:22  Goober5000
 * improved species support and removed need for #define
 * --Goober5000
 *
 * Revision 2.37  2005/06/30 02:36:16  Goober5000
 * the message queue now waits for simulated speech to finish
 * --Goober5000
 *
 * Revision 2.36  2005/06/30 01:48:52  Goober5000
 * * NOX'd none.wav
 * * changed comparisons on none.wav to only look at the first four letters in case
 *   we don't have an extension or in case some weirdo decides to put none.ogg
 * * simulated speech pre-empts "beeps" in in-game messages
 * --Goober5000
 *
 * Revision 2.35  2005/05/26 05:07:19  Goober5000
 * some bracket happiness, plus fixed the training message version of the crazy semicolon bug
 * (thanks to taylor for fixing the regular message version)
 * --Goober500
 *
 * Revision 2.34  2005/05/26 04:29:23  taylor
 * those crazy semi-colons
 *
 * Revision 2.33  2005/05/17 20:57:05  taylor
 * more missed stuff from the vm_* macro change (thanks Alpha0)
 *
 * Revision 2.32  2005/05/13 02:47:58  taylor
 * missed a strdup to vm_strdup conversion
 *
 * Revision 2.31  2005/05/12 17:49:14  taylor
 * use vm_malloc(), vm_free(), vm_realloc(), vm_strdup() rather than system named macros
 *   fixes various problems and is past time to make the switch
 *
 * Revision 2.30  2005/05/12 03:50:10  Goober5000
 * repeating messages with variables should work properly now
 * --Goober5000
 *
 * Revision 2.29  2005/05/12 01:34:50  Goober5000
 * removed variables in messages for now
 * --Goober5000
 *
 * Revision 2.28  2005/05/11 08:10:20  Goober5000
 * variables should now work properly in messages that are sent multiple times
 * --Goober5000
 *
 * Revision 2.27  2005/03/24 23:38:49  taylor
 * make sure we don't go one over the number of message avis
 *
 * Revision 2.26  2005/03/10 08:00:08  taylor
 * change min/max to MIN/MAX to fix GCC problems
 * add lab stuff to Makefile
 * build unbreakage for everything that's not MSVC++ 6
 * lots of warning fixes
 * fix OpenGL rendering problem with ship insignias
 * no Warnings() in non-debug mode for Linux (like Windows)
 * some campaign savefile fixage to stop reverting everyones data
 *
 * Revision 2.25  2005/03/02 21:24:45  taylor
 * more network/inferno goodness for Windows, takes care of a few warnings too
 *
 * Revision 2.24  2004/12/23 15:58:55  phreak
 * commits for scramble-messages and unscramble-messages
 * -phreak
 *
 * Revision 2.23  2004/12/17 06:43:01  taylor
 * fix crash from dynamically allocated Personas
 *
 * Revision 2.22  2004/12/14 14:46:13  Goober5000
 * allow different wing names than ABGDEZ
 * --Goober5000
 *
 * Revision 2.21  2004/12/10 17:21:00  taylor
 * dymanic allocation of Personas
 *
 * Revision 2.20  2004/10/09 17:58:48  taylor
 * one more try to fix the memory leak and end-of-mission crash for talking head anis
 *
 * Revision 2.19  2004/07/26 20:47:37  Kazan
 * remove MCD complete
 *
 * Revision 2.18  2004/07/17 18:46:08  taylor
 * various OGL and memory leak fixes
 *
 * Revision 2.17  2004/07/12 16:32:54  Kazan
 * MCD - define _MCD_CHECK to use memory tracking
 *
 * Revision 2.16  2004/05/11 02:52:12  Goober5000
 * completed the FRED import conversion stuff that I started ages ago
 * --Goober5000
 *
 * Revision 2.15  2004/05/10 13:07:22  Goober5000
 * fixed the AWACS help message
 * --Goober5000
 *
 * Revision 2.14  2004/05/02 01:26:52  taylor
 * comment out avi free code for now
 *
 * Revision 2.13  2004/04/26 01:40:53  taylor
 * better anim free stuff, disable for now to prevent CTD
 *
 * Revision 2.12  2004/03/06 23:28:23  bobboau
 * fixed motion debris
 * animated laser textures
 * and added a new error check called a safepoint, mostly for tracking the 'Y bug'
 *
 * Revision 2.11  2004/03/05 09:02:06  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.10  2004/02/20 04:29:55  bobboau
 * pluged memory leaks,
 * 3D HTL lasers (they work perfictly)
 * and posably fixed Turnsky's shinemap bug
 *
 * Revision 2.9  2004/02/13 04:17:13  randomtiger
 * Turned off fog in OGL for Fred.
 * Simulated speech doesnt say tags marked by $ now.
 * The following are fixes to issues that came up testing TBP in fs2_open and fred2_open:
 * Changed vm_vec_mag and parse_tmap to fail gracefully on bad data.
 * Error now given on missing briefing icon and bad ship normal data.
 * Solved more species divide by zero error.
 * Fixed neb cube crash.
 *
 * Revision 2.8  2004/02/06 21:26:07  Goober5000
 * fixed a small compatibility bug
 * --Goober5000
 *
 * Revision 2.7  2004/02/05 14:29:33  Goober5000
 * fixed the talking head error
 * --Goober5000
 *
 * Revision 2.6  2004/01/14 21:12:24  Goober5000
 * I think this will fix the problem of the death head ani sometimes incorrectly playing
 * --Goober5000
 *
 * Revision 2.5  2003/11/11 02:15:45  Goober5000
 * ubercommit - basically spelling and language fixes with some additional
 * warnings disabled
 * --Goober5000
 *
 * Revision 2.4  2003/10/16 16:38:16  Kazan
 * couple more types in species_defs.cpp, also finished up "Da Species Upgrade"
 *
 * Revision 2.3  2003/09/07 18:14:53  randomtiger
 * Checked in new speech code and calls from relevent modules to make it play.
 * Should all work now if setup properly with version 2.4 of the launcher.
 * FS2_SPEECH can be used to make the speech code compile if you have SAPI 5.1 SDK installed.
 * Otherwise the compile flag should not be set and it should all compile OK.
 *
 * - RT
 *
 * Revision 2.2  2002/10/17 20:40:51  randomtiger
 * Added ability to remove HUD ingame on keypress shift O
 * So I've added a new key to the bind list and made use of already existing hud removal code.
 *
 * Revision 2.1  2002/08/01 01:41:07  penguin
 * The big include file move
 *
 * Revision 2.0  2002/06/03 04:02:25  penguin
 * Warpcore CVS sync
 *
 * Revision 1.4  2002/05/13 15:11:03  mharris
 * More NO_NETWORK ifndefs added
 *
 * Revision 1.3  2002/05/10 20:42:44  mharris
 * use "ifndef NO_NETWORK" all over the place
 *
 * Revision 1.2  2002/05/10 06:08:08  mharris
 * Porting... added ifndef NO_SOUND
 *
 * Revision 1.1  2002/05/02 18:03:10  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 32    9/12/99 8:09p Dave
 * Fixed problem where skip-training button would cause mission messages
 * not to get paged out for the current mission.
 * 
 * 31    9/01/99 2:52p Andsager
 * Add new heads to FRED and some debug code for playing heads
 * 
 * 30    8/28/99 7:29p Dave
 * Fixed wingmen persona messaging. Make sure locked turrets don't count
 * towards the # attacking a player.
 * 
 * 29    8/26/99 8:51p Dave
 * Gave multiplayer TvT messaging a heavy dose of sanity. Cheat codes.
 * 
 * 28    8/23/99 5:04p Jefff
 * Added new mission flag to disable built-in messages from playing.
 * Added fred support as well.
 * 
 * 27    8/19/99 10:12a Alanl
 * preload mission-specific messages on machines greater than 48MB
 * 
 * 26    8/18/99 12:09p Andsager
 * Add debug if message has no anim for message.  Make messages come from
 * wing leader.
 * 
 * 25    7/31/99 2:30p Dave
 * Added nifty mission message debug viewing keys.
 * 
 * 24    7/24/99 2:19p Dave
 * Fixed broken build.
 * 
 * 23    7/23/99 5:44p Andsager
 * make personas consistently choose same ship
 * 
 * 22    7/15/99 9:20a Andsager
 * FS2_DEMO initial checkin
 * 
 * 21    7/14/99 4:27p Andsager
 * Added multiple message debug check
 * 
 * 20    7/06/99 10:41a Andsager
 * Add AWACS need help messages
 * 
 * 19    7/02/99 11:16a Andsager
 * Removed mult message debug check.
 * 
 * 18    7/02/99 11:13a Andsager
 * max debug version
 * 
 * 17    6/16/99 10:20a Dave
 * Added send-message-list sexpression.
 * 
 * 16    6/14/99 5:53p Dave
 * Removed duplicate message check temporarily.
 * 
 * 15    6/10/99 3:43p Dave
 * Do a better job of syncing text colors to HUD gauges.
 * 
 * 14    6/09/99 2:56p Andsager
 * Check all messages for repeat.  Allow multiple versions of same message
 * if queued > 20 apart.
 * 
 * 13    6/07/99 11:33a Anoop
 * Get rid of erroneous Int3() in multiple message check.
 * 
 * 12    6/07/99 10:31a Andsager
 * Get rid of false multiplayer multiple messages catch.
 * 
 * 11    6/03/99 2:56p Andsager
 * DOH!!
 * 
 * 10    6/03/99 2:44p Andsager
 * Fix stupid bug in debug code.
 * 
 * 9     6/03/99 2:08p Andsager
 * Put in debug code to find multiple mission messages.
 * 
 * 8     3/29/99 6:17p Dave
 * More work on demo system. Got just about everything in except for
 * blowing ships up, secondary weapons and player death/warpout.
 * 
 * 7     1/28/99 12:19a Dave
 * Fixed a dumb debug build unhandled exception.
 * 
 * 6     1/07/99 10:08a Jasen
 * coords
 * 
 * 5     1/07/99 9:24a Dave
 * Put in hi-res coord support for head anim.
 * 
 * 4     11/05/98 4:18p Dave
 * First run nebula support. Beefed up localization a bit. Removed all
 * conditional compiles for foreign versions. Modified mission file
 * format.
 * 
 * 3     10/23/98 3:51p Dave
 * Full support for tstrings.tbl and foreign languages. All that remains
 * is to make it active in Fred.
 * 
 * 2     10/07/98 10:53a Dave
 * Initial checkin.
 * 
 * 1     10/07/98 10:49a Dave
 * 
 * 127   8/25/98 1:48p Dave
 * First rev of EMP effect. Player side stuff basically done. Next comes
 * AI code.
 * 
 * 126   6/01/98 11:43a John
 * JAS & MK:  Classified all strings for localization.
 * 
 * 125   5/24/98 12:55a Mike
 * Fix bug with scream from Installation.  Should also fix bug with double
 * screams from some ships.
 * 
 * 124   5/18/98 6:06p Lawrance
 * Don't play messages or auto-target on first frame
 * 
 * 123   5/15/98 8:36p Lawrance
 * Add 'target ship that last sent transmission' target key
 * 
 * 122   5/09/98 10:00p Allender
 * make vasudan persona for support use terran support persona
 * 
 * 121   5/08/98 11:21a Allender
 * fix ingame join trouble.  Small messaging fix.  Enable collisions for
 * friendlies again
 * 
 * 120   5/06/98 12:19p Lawrance
 * Fix typo for 'Stray Warning Final'
 * 
 * 119   5/05/98 9:12p Allender
 * fix large problem introduced last checkin when changiing Assert to if
 * 
 * 118   5/05/98 4:12p Chad
 * changed Assert info if statement when removing messages from queue when
 * too old
 * 
 * 117   5/01/98 12:34p John
 * Added code to force FreeSpace to run in the same dir as exe and made
 * all the parse error messages a little nicer.
 * 
 * 116   4/27/98 9:00p Allender
 * mission specific messages from #<someone> are now sourced to terran
 * command
 * 
 * 115   4/26/98 11:35a Allender
 * make traitor message play by iteself in all cases
 * 
 * 114   4/25/98 11:49p Lawrance
 * Add Terran Command stray messages
 * 
 * 113   4/22/98 9:17a Allender
 * be sure that builtin command messages play with the correct hud source.
 * Also be sure that messages which get converted to Terran command to the
 * same
 * 
 * 112   4/20/98 1:30a Lawrance
 * Don't load head animations if talking head gauge is disabled.
 * 
 * 111   4/17/98 11:03a Allender
 * some rearm message being played too often and sent with incorrect
 * 'who-from'
 * 
 * 110   4/13/98 5:06p Lawrance
 * Cut off talking head about 250ms before wave ends
 * 
 * 109   4/10/98 9:14a Lawrance
 * fix up persona code for the demo
 * 
 * 108   4/09/98 2:15p Allender
 * fixed compiler warnings
 * 
 * 107   4/09/98 12:36p Allender
 * don't allow the same ship to have messages overlapping.  Put in code to
 * check for ship's existence (wingman only) before actually playing
 * message
 * 
 * 106   4/09/98 12:32a Lawrance
 * Fix bugs related to multiple screams from same ship, builtin messages
 * playing after screams, or praising while severly damaged.
 * 
 * 105   4/08/98 3:45p Allender
 * mission message overhaul.  Make message from any wingman mean any
 * wingman with that persona.  Terran command wave and ani's for dead
 * ships now play correctly.
 * 
 * 104   4/07/98 8:09p Lawrance
 * don't play talking heads in the demo
 * 
 * 103   4/07/98 5:30p Lawrance
 * Player can't send/receive messages when comm is destroyed.  Garble
 * messages when comm is damaged.
 * 
 * 102   4/07/98 5:26p Allender
 * low priority mission specific messages won't interrupt anything.
 * 
 * 101   4/07/98 10:51a Allender
 * remove any allied from message senders.  Make heads for mission
 * specific messages play appropriately
 * 
 * 100   4/07/98 12:04a Mike
 * New system for instructor chastising player if he fires at instructor.
 * 
 * 99    4/03/98 11:39a Lawrance
 * only allow 1 wingman persona in demo
 * 
 * 98    4/02/98 1:09p Allender
 * don't process messages before player "enters" mission (i.e. due to
 * player entry delay)
 * 
 * 97    4/02/98 10:06a Allender
 * wing arrival message for delta and epsilon wings
 * 
 * 96    4/01/98 10:47p Lawrance
 * Supporting builtin messages for rearm and repair requests
 * 
 * 95    3/25/98 8:43p Hoffoss
 * Changed anim_play() to not be so complex when you try and call it.
 * 
 * 94    3/24/98 12:46p Allender
 * save shipnum before killing currently playing message in preparation
 * for playing death scream.
 * 
 * 93    3/22/98 3:54p Andsager
 * AL: Prevent -1 index into Ships[] array when playing a scream
 * 
 * 92    3/18/98 10:20p Allender
 * force wingman scream when he's talking and then dies
 * 
 * 91    3/18/98 12:03p John
 * Marked all the new strings as externalized or not.
 * 
 * 90    3/17/98 4:01p Hoffoss
 * Added HUD_SOURCE_TERRAN_CMD and changed code to utilize it when a
 * message is being sent from Terran Command.
 * 
 * 89    3/05/98 10:18p Lawrance
 * Play voice cue sound when there is no voice file present
 * 
 * 88    3/02/98 5:42p John
 * Removed WinAVI stuff from FreeSpace.  Made all HUD gauges wriggle from
 * afterburner.  Made gr_set_clip work good with negative x &y.  Made
 * model_caching be on by default.  Made each cached model have it's own
 * bitmap id.  Made asteroids not rotate when model_caching is on.  
 * 
 * 87    3/02/98 9:34a Allender
 * don't allow mission specific messages to timeout.  Assert when trying
 * to remove a mission specific messages from the queue.  Print out in the
 * log file if the voice didn't play.
 * 
 * 86    2/23/98 8:45a John
 * Externalized Strings
 * 
 * 85    2/20/98 8:33p Lawrance
 * Add the 'All Alone' message
 * 
 * 84    2/16/98 2:20p Allender
 * make death scream kill any other messages from that ship
 * 
 * 83    2/12/98 4:58p Lawrance
 * Add support for 'All Clear' radio message
 * 
 * 82    2/11/98 9:44p Allender
 * rearm repair code fixes.  hud support view shows abort status.  New
 * support ship killed message.  More network stats
 * 
 * 81    2/04/98 10:44p Allender
 * mark personas as not used between missions.   Don't ever randomly
 * choose a Vasudan persona
 * 
 * 80    1/29/98 11:38a Allender
 * support for Vasudan personas
 * 
 * 79    1/25/98 10:04p Lawrance
 * Fix nasty divide-by-zero bug in message_calc_anim_start_frame().
 * 
 * 78    1/24/98 4:46p Lawrance
 * add in support for new voice messasges
 * 
 * 77    1/22/98 5:13p Lawrance
 * pick useful starting frame when playing animation
 * 
 * 76    1/21/98 7:20p Lawrance
 * Make subsystem locking only work with line-of-sight, cleaned up locking
 * code, moved globals to player struct.
 * 
 * 75    1/21/98 11:54a Duncan
 * Commended out assert for the moment to allow Fred to run.  Mark can fix
 * this properly later, since he understands it.
 * 
 * 74    1/21/98 10:33a Allender
 * fixed up messaging code to play a random head when playing a builtin
 * message
 * 
 * 73    1/20/98 6:21p Lawrance
 * Stop animation from playing when voice clip ends early.
 * 
 * 72    1/20/98 3:43p Allender
 * don't queue messages when player becomes traitor
 * 
 * 71    1/20/98 12:52p Lawrance
 * Draw talking head as alpha-color bitmap, black out region behind
 * animation.
 * 
 * 70    1/20/98 10:20a Lawrance
 * Draw head animation as alpha-colored.
 * 
 * 69    1/18/98 9:51p Lawrance
 * Add support for 'Player Died' messages.
 * 
 * 68    1/14/98 9:49p Allender
 * removed 3'oclock and 9'oclock messages
 * 
 * 67    1/13/98 3:11p Allender
 * new messages for disable/disarm
 * 
 * 66    1/12/98 11:16p Lawrance
 * Wonderful HUD config.
 * 
 * 65    1/07/98 4:41p Allender
 * minor modification to special messages.  Fixed cargo_revealed problem
 * for multiplayer and problem with is-cargo-known sexpression
 * 
 * 64    12/15/97 12:14p Allender
 * implemented overlapping messages
 * 
 * 63    12/12/97 4:58p Allender
 * make messages interruptable.  Put in code to support multiple messages
 * at once, although this feature not fully implemented yet.
 * 
 * 62    12/04/97 9:37p Dave
 * Fixed a bunch of multiplayer messaging bugs.
 * 
 * 61    12/02/97 2:37p Allender
 * added asserts to be sure that an actual ship (i.e. not cargo or
 * otherwise) is the ship sending a message to the player
 *
 * $NoKeywords: $
 */


#include "mission/missionmessage.h"
#include "mission/missiontraining.h"
#include "hud/hudmessage.h"
#include "hud/hudgauges.h"
#include "hud/hudtarget.h"
#include "io/timer.h"
#include "parse/parselo.h"
#include "gamesnd/gamesnd.h"
#include "gamesequence/gamesequence.h"
#include "anim/animplay.h"
#include "hud/hud.h"
#include "ship/ship.h"
#include "ship/subsysdamage.h"
#include "weapon/emp.h"
#include "localization/localize.h"
#include "demo/demo.h"
#include "hud/hudconfig.h"
#include "sound/fsspeech.h"
#include "species_defs/species_defs.h"
#include "parse/sexp.h"
#include "iff_defs/iff_defs.h"
#include "network/multi.h"
#include "network/multimsgs.h"
#include "network/multiutil.h"


// here is a text list of the builtin message names.  These names are used to match against
// names read in for builtin message radio bits to see what message to play.  These are
// generic names, meaning that there will be the same message type for a number of different
// personas
char *Builtin_message_types[MAX_BUILTIN_MESSAGE_TYPES] =
{
//XSTR:OFF
	"Arrive Enemy",
	"Attack Target",
	"Beta Arrived",
	"Check 6",
	"Engage",
	"Gamma Arrived",
	"Help",
	"Praise",
	"Backup",
	"Ignore Target",
	"No",
	"Oops 1",
	"Permission",			// AL: no code support yet
	"Stray",					// DA: no code support
	"Depart",
	"yes",
	"Rearm on Way",
	"On way",
	"Rearm warping in",
	"No Target",
	"Docking Start",		// AL: no message seems to exist for this
	"Repair Done",
	"Repair Aborted",
	"Traitor",
	"Rearm",
	"Disable Target",
	"Disarm Target",
	"Player Dead",
	"Death",
	"Support Killed",
	"All Clear",			// DA: no code support
	"All Alone",
	"Repair",
	"Delta Arrived",
	"Epsilon Arrived",
	"Instructor Hit",
	"Instructor Attack",
	"Stray Warning",
	"Stray Warning Final",
	"AWACS at 75",
	"AWACS at 25"
//XSTR:ON
};

MMessage Messages[MAX_MISSION_MESSAGES];
int Message_times[MAX_MISSION_MESSAGES];

int Num_messages, Num_message_avis, Num_message_waves;
int Num_builtin_messages, Num_builtin_avis, Num_builtin_waves;

int Message_debug_index = -1;

message_extra Message_avis[MAX_MESSAGE_AVIS];
message_extra Message_waves[MAX_MESSAGE_WAVES];

#define MAX_PLAYING_MESSAGES		2

#ifdef FS2_DEMO
	#define MAX_WINGMAN_HEADS			1
	#define MAX_COMMAND_HEADS			1
#else
#define MAX_WINGMAN_HEADS			2
#define MAX_COMMAND_HEADS			3
#endif

//XSTR:OFF
#define HEAD_PREFIX_STRING			"head-"
#define COMMAND_HEAD_PREFIX		"head-cm1"
#define COMMAND_WAVE_PREFIX		"TC_"
#define SUPPORT_NAME					"Support"
//XSTR:ON

// variables to keep track of messages that are currently playing
int Num_messages_playing;						// number of is a message currently playing?

typedef struct pmessage {
	anim_instance *anim;		// handle of anim currently playing
	int wave;					// handle of wave currently playing
	int id;						// id of message currently playing
	int priority;				// priority of message currently playing
	int shipnum;				// shipnum of ship sending this message,  -1 if from Terran command
	int builtin_type;			// if a builtin message, type of the message
} pmessage;

LOCAL pmessage Playing_messages[MAX_PLAYING_MESSAGES];

int Message_shipnum;						// ship number of who is sending message to player -- used outside this module

// variables to control message queuing.  All new messages to the player are queued.  The array
// will be ordered by priority, then time submitted.

#define MQF_CONVERT_TO_COMMAND		(1<<0)			// convert this queued message to terran command
#define MQF_CHECK_ALIVE					(1<<1)			// check for the existence of who_from before sending

typedef struct message_q {
	fix	time_added;					// time at which this entry was added
	int	window_timestamp;			// timestamp which will tell us how long we have to play the message
	int	priority;					// priority of the message
	int	message_num;				// index into the Messages[] array
	char *special_message;			// Goober5000 - message to play if we've replaced stuff (like variables)
	char who_from[NAME_LENGTH];		// who this message is from
	int	source;						// who the source of the message is (HUD_SOURCE_* type)
	int	builtin_type;				// type of builtin message (-1 if mission message)
	int	flags;						// should this message entry be converted to Terran Command head/wave file
	int	min_delay_stamp;			// minimum delay before this message will start playing
	int	group;						// message is part of a group, don't time it out
} message_q;

#define MAX_MESSAGE_Q				30
#define MAX_MESSAGE_LIFE			F1_0*30		// After being queued for 30 seconds, don't play it
#define DEFAULT_MESSAGE_LENGTH	3000			// default number of milliseconds to display message indicator on hud
message_q	MessageQ[MAX_MESSAGE_Q];
int MessageQ_num;			// keeps track of number of entries on the queue.

#define MESSAGE_IMMEDIATE_TIMESTAMP		1000		// immediate messages must play within 1 second
#define MESSAGE_SOON_TIMESTAMP			5000		// "soon" messages must play within 5 seconds
#define MESSAGE_ANYTIME_TIMESTAMP		-1			// anytime timestamps are invalid

// Persona information
int Num_personas;
Persona *Personas = NULL;

char *Persona_type_names[MAX_PERSONA_TYPES] = 
{
//XSTR:OFF
	"wingman",
	"support",
	"large", 
	"command",
//XSTR:ON
};

int Default_command_persona;

///////////////////////////////////////////////////////////////////
// used to distort incoming messages when comms are damaged
///////////////////////////////////////////////////////////////////
static int Message_wave_muted;
static int Message_wave_duration;
static int Next_mute_time;

#define MAX_DISTORT_PATTERNS	2
#define MAX_DISTORT_LEVELS		6
static float Distort_patterns[MAX_DISTORT_PATTERNS][MAX_DISTORT_LEVELS] = 
{
	{0.20f, 0.20f, 0.20f, 0.20f, 0.20f, 0.20f},
	{0.10f, 0.20f, 0.25f, 0.25f, 0.05f, 0.15f}
};

static int Distort_num;		// which distort pattern is being used
static int Distort_next;	// which section of distort pattern is next

int Head_coords[GR_NUM_RESOLUTIONS][2] = {
	{ // GR_640
		7, 45
	},
	{ // GR_1024
		7, 66
	}
};

// forward declaration
void message_maybe_distort_text(char *text);

// following functions to parse messages.tbl -- code pretty much ripped from weapon/ship table parsing code

static void persona_parse_close()
{
	if (Personas != NULL) {
		vm_free(Personas);
		Personas = NULL;
	}
}

// functions to deal with parsing personas.  Personas are just a list of names that give someone
// sending a message an identity which spans the life of the mission
void persona_parse()
{
	int i;
	char type[NAME_LENGTH];

	static bool done_at_exit = false;
	if ( !done_at_exit ) {
		atexit( persona_parse_close );
		done_at_exit = true;
	}

	// this way should cause the least amount of problems on the various platforms - taylor
	Personas = (Persona*)vm_realloc( Personas, sizeof(Persona) * (Num_personas + 1) );

	if (Personas == NULL)
		Error(LOCATION, "Not enough memory to allocate Personas!" );

	memset(&Personas[Num_personas], 0, sizeof(Persona));

	required_string("$Persona:");
	stuff_string(Personas[Num_personas].name, F_NAME, NAME_LENGTH);

	// get the type name and set the appropriate flag
	required_string("$Type:");
	stuff_string( type, F_NAME, NAME_LENGTH );
	for ( i = 0; i < MAX_PERSONA_TYPES; i++ ) {
		if ( !stricmp( type, Persona_type_names[i]) ) {

			Personas[Num_personas].flags |= (1<<i);

			// save the Command persona in a global
			if ( Personas[Num_personas].flags & PERSONA_FLAG_COMMAND ) {
				if (Default_command_persona < 0)
					Default_command_persona = Num_personas;
			}

			break;
		}
	}

	char cstrtemp[NAME_LENGTH];
	if ( optional_string("+") )
	{
		stuff_string(cstrtemp, F_NAME, NAME_LENGTH);

		for (int j = 0; j < (int)Species_info.size(); j++)
		{
			if (!strcmp(cstrtemp, Species_info[j].species_name))
			{
				Personas[Num_personas].species = j;
				break;
			}
		}
	}

	if ( i == MAX_PERSONA_TYPES )
		Error(LOCATION, "Unknown persona type in messages.tbl -- %s\n", type );


	Num_personas++;
}

// two functions to add avi/wave names into a table
int add_avi( char *avi_name )
{
	int i;

	Assert ( Num_message_avis < MAX_MESSAGE_AVIS );
	Assert (strlen(avi_name) < MAX_FILENAME_LEN );

	// check to see if there is an existing avi being used here
	for ( i = 0; i < Num_message_avis; i++ ) {
		if ( !stricmp(Message_avis[i].name, avi_name) )
			return i;
	}

	// would have returned if a slot existed.
	strcpy( Message_avis[Num_message_avis].name, avi_name );
	Message_avis[Num_message_avis].num = -1;
	Num_message_avis++;
	return (Num_message_avis - 1);
}

int add_wave( char *wave_name )
{
	int i;

	Assert ( Num_message_waves < MAX_MESSAGE_WAVES );
	Assert (strlen(wave_name) < MAX_FILENAME_LEN );

	// check to see if there is an existing wave being used here
	for ( i = 0; i < Num_message_waves; i++ ) {
		if ( !stricmp(Message_waves[i].name, wave_name) )
			return i;
	}

	strcpy( Message_waves[Num_message_waves].name, wave_name );
	Message_waves[Num_message_waves].num = -1;
	Num_message_waves++;
	return (Num_message_waves - 1);
}

// parses an individual message
void message_parse(bool importing_from_fsm)
{
	MissionMessage *msgp;
	char persona_name[NAME_LENGTH];

	Assert ( Num_messages < MAX_MISSION_MESSAGES );
	msgp = &Messages[Num_messages];

	required_string("$Name:");
	stuff_string(msgp->name, F_NAME, NAME_LENGTH);

	// team
	msgp->multi_team = -1;
	if(optional_string("$Team:")){
		int mt;
		stuff_int(&mt);

		// keep it real
		if((mt < 0) || (mt >= MAX_TVT_TEAMS)){
			mt = -1;
		}

		// only bother with filters if multiplayer and TvT
		if(Fred_running || ((Game_mode & GM_MULTIPLAYER) && (Netgame.type_flags & NG_TYPE_TEAM)) ){
			msgp->multi_team = mt;
		}
	}

	// backwards compatibility for old fred missions - all new ones should use $MessageNew
	if(optional_string("$Message:")){
		stuff_string(msgp->message, F_MESSAGE, MESSAGE_LENGTH);
	} else {
		required_string("$MessageNew:");
		stuff_string(msgp->message, F_MULTITEXT, MESSAGE_LENGTH);
	}

	msgp->persona_index = -1;
	if ( optional_string("+Persona:") ) {
		stuff_string(persona_name, F_NAME, NAME_LENGTH);
		msgp->persona_index = message_persona_name_lookup( persona_name );
	}

	if ( !Fred_running)
		msgp->avi_info.index = -1;
	else
		msgp->avi_info.name = NULL;

	if ( optional_string("+AVI Name:") ) {
		char avi_name[MAX_FILENAME_LEN];

		stuff_string(avi_name, F_NAME, MAX_FILENAME_LEN);

		// Goober5000 - for some reason :V: swapped Head-TP1
		// and Head-TP4 in FS2
		if (importing_from_fsm && !strnicmp(avi_name, "Head-TP1", 8))
			avi_name[7] = '4';

		if ( !Fred_running ) {
			msgp->avi_info.index = add_avi(avi_name);
		} else {
			msgp->avi_info.name = vm_strdup(avi_name);
		}
	}

	if ( !Fred_running )
		msgp->wave_info.index = -1;
	else
		msgp->wave_info.name = NULL;

	if ( optional_string("+Wave Name:") ) {
		char wave_name[MAX_FILENAME_LEN];

		stuff_string(wave_name, F_NAME, MAX_FILENAME_LEN);
		if ( !Fred_running ) {
			msgp->wave_info.index = add_wave(wave_name);
		} else {
			msgp->wave_info.name = vm_strdup(wave_name);
		}
	}

	Num_messages++;
}

void parse_msgtbl()
{
	char *p1, *p2, *p3;

	// open localization
	lcl_ext_open();

	read_file_text("messages.tbl", CF_TYPE_TABLES);
	reset_parse();
	Num_messages = 0;
	Num_personas = 0;

	// Goober5000 - ugh, nasty nasty hack to fix the FS2 retail tables
	p1 = strstr(Mp, "#End");
	*(p1+4)=0;
	p1 = strstr(Mp, "2926");
	if (p1)
	{
		p2 = strstr(p1, "Vawacs25.wav");
		p3 = strstr(p1, "$Name");
		if (p2 && p3 && (p2 < p3))
		{
			replace_one(p2, "Vawacs25.wav", "Awacs25.wav", 500);
		}
	}
	p1 = strstr(Mp, "2927");
	if (p1)
	{
		p2 = strstr(p1, "Awacs75.wav");
		p3 = strstr(p1, "$Name");
		if (p2 && p3 && (p2 < p3))
		{
			replace_one(p2, "Awacs75.wav", "Vawacs75.wav", 500);
		}
	}

	// Goober5000 - now we can start parsing
	required_string("#Personas");
	while ( required_string_either("#Messages", "$Persona:")){
		persona_parse();
	}

	required_string("#Messages");
	while (required_string_either("#End", "$Name:")){
		message_parse();
	}

	required_string("#End");

	// save the number of builtin message things -- make initing between missions easier
	Num_builtin_messages = Num_messages;
	Num_builtin_avis = Num_message_avis;
	Num_builtin_waves = Num_message_waves;

	// close localization
	lcl_ext_close();
}

// this is called at the start of each level
void messages_init()
{
	int rval, i;
	static int table_read = 0;

	if ( !table_read ) {
		Default_command_persona = -1;

		if ((rval = setjmp(parse_abort)) != 0) {
			mprintf(("TABLES: Unable to parse '%s'!  Error code = %i.\n", "messages.tbl", rval));
			return;
		}

		parse_msgtbl();
		table_read = 1;
	}

	// reset the number of messages that we have for this mission
	Num_messages = Num_builtin_messages;
	Num_message_avis = Num_builtin_avis;
	Num_message_waves = Num_builtin_waves;
	Message_debug_index = Num_builtin_messages - 1;

	// initialize the stuff for the linked lists of messages
	MessageQ_num = 0;
	for (i = 0; i < MAX_MESSAGE_Q; i++) {
		MessageQ[i].priority = -1;
		MessageQ[i].time_added = -1;
		MessageQ[i].message_num = -1;
		MessageQ[i].builtin_type = -1;
		MessageQ[i].min_delay_stamp = -1;
		MessageQ[i].group = 0;

		// Goober5000
		MessageQ[i].special_message = NULL;
	}
	
	// this forces a reload of the AVI's and waves for builtin messages.  Needed because the flic and
	// sound system also get reset between missions!
	for (i = 0; i < Num_builtin_avis; i++ ) {
		Message_avis[i].anim_data = NULL;
	}

	for (i = 0; i < Num_builtin_waves; i++ ){
		Message_waves[i].num = -1;
	}

	Message_shipnum = -1;
	Num_messages_playing = 0;
	for ( i = 0; i < MAX_PLAYING_MESSAGES; i++ ) {
		Playing_messages[i].anim = NULL;
		Playing_messages[i].wave = -1;
		Playing_messages[i].id = -1;
		Playing_messages[i].priority = -1;
		Playing_messages[i].shipnum = -1;
		Playing_messages[i].builtin_type = -1;
	}

	// reinitialize the personas.  mark them all as not used
	for ( i = 0; i < Num_personas; i++ ){
		Personas[i].flags &= ~PERSONA_FLAG_USED;
	}

	Message_wave_muted = 0;
	Next_mute_time = 1;

	memset(Message_times, 0, sizeof(int)*MAX_MISSION_MESSAGES);

}

// free a loaded avi
void message_mission_free_avi(int m_index)
{
	int rc = 0, try_count = 0;

	// check for bogus index
	if ( (m_index < 0) || (m_index >= Num_message_avis) )
		return;

	// Make sure this code doesn't get run if the talking head guage is off
	// helps prevent a crash on jump out if this code doesn't work right
	if ( !hud_gauge_active(HUD_TALKING_HEAD) )
		return;

	if (Message_avis[m_index].anim_data != NULL) {
		do {
			rc = anim_free( Message_avis[m_index].anim_data );
			try_count++;

			// -2 is to catch a point where the data isn't valid and we want
			// to just abort right now rather than to keep trying
			if (rc == -2)
				break;

			// stop at 25 tries to avoid a possible endless loop
		} while ( rc && (try_count < 25) );

		Message_avis[m_index].anim_data = NULL;
	}
}

// called to do cleanup when leaving a mission
void message_mission_shutdown()
{
	int i;

	mprintf(("Unloading in mission messages\n"));

	training_mission_shutdown();

	// kill/stop all playing messages sounds and animations if we need to
	if (Num_messages_playing) {
		message_kill_all(1);
	}

	// remove the wave sounds from memory
	for (i = 0; i < Num_message_waves; i++ ) {
		if ( Message_waves[i].num != -1 ){
			snd_unload( Message_waves[i].num );
		}
	}

	fsspeech_stop();

	// free up remaining anim data - taylor
	for (i=0; i<Num_message_avis; i++) {
		message_mission_free_avi(i);
	}

	// Goober5000 - free up special messages
	for (i = 0; i < MAX_MESSAGE_Q; i++)
	{
		if (MessageQ[i].special_message != NULL)
		{
			vm_free(MessageQ[i].special_message);
			MessageQ[i].special_message = NULL;
		}
	}
}

// call from game_shutdown() ONLY!!!
void message_mission_close()
{
	// free the persona data
	if (Personas != NULL) {
		vm_free( Personas );
		Personas = NULL;
	}
}

// functions to deal with queuing messages to the message system.

//	Compare function for sorting message queue entries based on priority.
//	Return values set to sort array in _decreasing_ order.  If priorities equal, sort based
// on time added into queue
int message_queue_priority_compare(const void *a, const void *b)
{
	message_q *ma, *mb;

	ma = (message_q *) a;
	mb = (message_q *) b;

	if (ma->priority > mb->priority) {
		return -1;
	} else if (ma->priority < mb->priority) {
		return 1;
	} else if (ma->time_added < mb->time_added) {
		return -1;
	} else if (ma->time_added > mb->time_added) {
		return 1;
	} else {
		return 0;
	}
}

// function to kill all currently playing messages.  kill_all parameter tells us to
// kill only the animations that are playing, or wave files too
void message_kill_all( int kill_all )
{
	int i;

	Assert( Num_messages_playing );

	// kill sounds for all voices currently playing
	for ( i = 0; i < Num_messages_playing; i++ ) {
		if ( (Playing_messages[i].anim != NULL) && anim_playing(Playing_messages[i].anim) ) {
			anim_stop_playing( Playing_messages[i].anim );
			Playing_messages[i].anim=NULL;
		}

		if ( kill_all ) {
			if ( (Playing_messages[i].wave != -1 ) && snd_is_playing(Playing_messages[i].wave) ){
				snd_stop( Playing_messages[i].wave );
			}

			Playing_messages[i].shipnum = -1;
		}
	}

	if ( kill_all ) {
		Num_messages_playing = 0;
	}

	fsspeech_stop();
}

// function to kill nth playing message
void message_kill_playing( int message_num )
{
	Assert( message_num < Num_messages_playing );

	if ( (Playing_messages[message_num].anim != NULL) && anim_playing(Playing_messages[message_num].anim) ) {
		anim_stop_playing( Playing_messages[message_num].anim );
		Playing_messages[message_num].anim=NULL;
	}
	if ( (Playing_messages[message_num].wave != -1 ) && snd_is_playing(Playing_messages[message_num].wave) )
		snd_stop( Playing_messages[message_num].wave );

	Playing_messages[message_num].shipnum = -1;

	fsspeech_stop();
}


// returns true if all messages currently playing are builtin messages
int message_playing_builtin()
{
	int i;

	for ( i = 0; i < Num_messages_playing; i++ ) {
		if ( Playing_messages[i].id >= Num_builtin_messages ){
			break;
		}
	}

	// if we got through the list without breaking, all playing messages are builtin messages
	if ( i == Num_messages_playing ){
		return 1;
	} else {
		return 0;
	}
}

// returns true in any playing message is of the specific builtin type
int message_playing_specific_builtin( int builtin_type )
{
	int i;

	for (i = 0; i < Num_messages_playing; i++ ) {
		if ( (Playing_messages[i].id < Num_builtin_messages) && (Playing_messages[i].builtin_type == builtin_type) ){
			return 1;
		}
	}

	return 0;
}

// returns true if all messages current playing are unique messages
int message_playing_unique()
{
	int i;

	for ( i = 0; i < Num_messages_playing; i++ ) {
		if ( Playing_messages[i].id < Num_builtin_messages ){
			break;
		}
	}

	// if we got through the list without breaking, all playing messages are builtin messages
	if ( i == Num_messages_playing ){
		return 1;
	} else {
		return 0;
	}
}


// returns the highest priority of the currently playing messages
#define MESSAGE_GET_HIGHEST		1
#define MESSAGE_GET_LOWEST			2
int message_get_priority(int which)
{
	int i;
	int priority;

	if ( which == MESSAGE_GET_HIGHEST ){
		priority = MESSAGE_PRIORITY_LOW;
	} else {
		priority = MESSAGE_PRIORITY_HIGH;
	}

	for ( i = 0; i < Num_messages_playing; i++ ) {
		if ( (which == MESSAGE_GET_HIGHEST) && (Playing_messages[i].priority > priority) ){
			priority = Playing_messages[i].priority;
		} else if ( (which == MESSAGE_GET_LOWEST) && (Playing_messages[i].priority < priority) ){
			priority = Playing_messages[i].priority;
		}
	}

	return priority;
}


// removes current message from the queue
void message_remove_from_queue(message_q *q)
{	
	// quick out if nothing to do.
	if ( MessageQ_num <= 0 ) {
		return;
	}	

	MessageQ_num--;
	q->priority = -1;
	q->time_added = -1;
	q->message_num = -1;
	q->builtin_type = -1;
	q->min_delay_stamp = -1;
	q->group = 0;

	// Goober5000
	if (q->special_message != NULL)
	{
		vm_free(q->special_message);
		q->special_message = NULL;
	}

	if ( MessageQ_num > 0 ) {
		insertion_sort(MessageQ, MAX_MESSAGE_Q, sizeof(message_q), message_queue_priority_compare);
	}
}

// Load in the sound data for a message.
//
// index - index into the Message_waves[] array
//
void message_load_wave(int index, const char *filename)
{
	if (index == -1) {
		Int3();
		return;
	}

	if ( Message_waves[index].num >= 0) {
		return;
	}

	if ( !Sound_enabled ) {
		Message_waves[index].num = -1;
		return;
	}

	game_snd tmp_gs;
	memset(&tmp_gs, 0, sizeof(game_snd));
	strcpy( tmp_gs.filename, filename );
	Message_waves[index].num = snd_load( &tmp_gs, 0 );

	if (Message_waves[index].num == -1)
		nprintf(("messaging", "Cannot load message wave: %s.  Will not play\n", Message_waves[index].name));
}

// Goober5000
bool message_filename_is_generic(char *filename)
{
	if (!strnicmp(filename, "cuevoice.wav", 8)) return true;
	if (!strnicmp(filename, "emptymsg.wav", 8)) return true;
	if (!strnicmp(filename, "generic.wav", 7)) return true;
	if (!strnicmp(filename, "msgstart.wav", 8)) return true;

	return false;
}

// Play wave file associated with message
// input: m		=>		pointer to message description
//
// note: changes Messave_wave_duration, Playing_messages[].wave, and Message_waves[].num
bool message_play_wave( message_q *q )
{
	int index;
	MissionMessage *m;
	char filename[MAX_FILENAME_LEN];

	// check for multiple messages playing.  don't check builtin messages.
	if (q->message_num >= Num_builtin_messages) {
		if ( (f2fl(Missiontime - Message_times[q->message_num]) < 10) && (f2fl(Missiontime) > 10) ) {
			// Int3();  // Get Andsager
		}
		Message_times[q->message_num] = Missiontime;
	}

	m = &Messages[q->message_num];

	if ( m->wave_info.index >= 0 ) {
		index = m->wave_info.index;
		strcpy( filename, Message_waves[index].name );

		// Goober5000 - if we're using simulated speech, it should pre-empt the generic beeps
		if (fsspeech_play_from(FSSPEECH_FROM_INGAME) && message_filename_is_generic(filename))
			return false;

		// if we need to bash the wave name because of "conversion" to terran command, do it here
		if ( q->flags & MQF_CONVERT_TO_COMMAND ) {
			char *p, new_filename[MAX_FILENAME_LEN];

			Message_waves[index].num = -1;					// forces us to reload the message

			// bash the filename here. Look for "[1-6]_" at the front of the message.  If found, then
			// convert to TC_*
			p = strchr(filename, '_' );
			if ( p == NULL ) {
				mprintf(("Cannot convert %s to terran command wave -- find Sandeep or Allender\n", Message_waves[index].name));
				return false;
			}

			// prepend the command name, and then the rest of the filename.
			p++;
			strcpy( new_filename, COMMAND_WAVE_PREFIX );
			strcat( new_filename, p );
			strcpy( filename, new_filename );
		}

		// load the sound file into memory
		message_load_wave(index, filename);
		if ( Message_waves[index].num == -1 ) {
			m->wave_info.index = -1;
		}

		if ( m->wave_info.index >= 0 ) {
			// this call relies on the fact that snd_play returns -1 if the sound cannot be played
			Message_wave_duration = snd_get_duration(Message_waves[index].num);
			Playing_messages[Num_messages_playing].wave = snd_play_raw( Message_waves[index].num, 0.0f );

			return (Playing_messages[Num_messages_playing].wave != -1);
		}
	}

	return false;
}

// Determine the starting frame for the animation
// input:	time	=>		time of voice clip, in ms
//				ani	=>		pointer to anim data
//				reverse	=>	flag to indicate that the start should be time ms from the end (used for death screams)
int message_calc_anim_start_frame(int time, anim *ani, int reverse)
{
	float	wave_time, anim_time;
	int	start_frame;

	start_frame=0;

	// If no voice clip exists, start from beginning of anim
	if ( time <= 0 ) {
		return start_frame;
	}

	// convert time to seconds
	wave_time = time/1000.0f;
	anim_time = ani->time;

	// If voice clip is longer than anim, start from beginning of anim
	if ( wave_time >= (anim_time) ) {
		return start_frame;
	}

	if ( reverse ) {
		start_frame = (ani->total_frames-1) - fl2i(ani->fps * wave_time + 0.5f);
	} else {
		int num_frames_extra;
		num_frames_extra = fl2i(ani->fps * (anim_time - wave_time) + 0.5f);
		if ( num_frames_extra > 0 ) {
			start_frame=rand()%num_frames_extra;
		}
	}

	if ( start_frame < 0 ) {
		Int3();
		start_frame=0;
	}

	return start_frame;
}

// Play animation associated with message
// input:	m		=>		pointer to message description
//				q		=>		message queue data
//
// note: changes Messave_wave_duration, Playing_messages[].wave, and Message_waves[].num
void message_play_anim( message_q *q )
{
	message_extra	*anim_info;
	int				is_death_scream=0, persona_index=-1, rand_index=0;
	char				ani_name[MAX_FILENAME_LEN], *p;
	MissionMessage	*m;

	// don't even bother with this stuff if the gauge is disabled - taylor
	if ( !hud_gauge_active(HUD_TALKING_HEAD) ) {
		return;
	}

	m = &Messages[q->message_num];

	// check to see if the avi_index is valid -- try and load/play the avi if so.
	if ( m->avi_info.index < 0 ) {
		return;
	}

	anim_info = &Message_avis[m->avi_info.index];

	// get the filename.  Strip off the extension since we won't need it anyway
	strcpy(ani_name, anim_info->name);
	p = strchr(ani_name, '.');			// gets us to the extension
	if ( p ) {
		*p = '\0';
	}

	// builtin messages are given a base ani which we should add a suffix on before trying
	// to load the animation.  See if this message is a builtin message which has a persona
	// attached to it.  Deal with munging the name

	// support ships use a wingman head.
	// terran command uses its own set of heads.
	int subhead_selected = FALSE;
	if ( (q->message_num < Num_builtin_messages) || !(_strnicmp(HEAD_PREFIX_STRING, ani_name, strlen(HEAD_PREFIX_STRING)-1)) ) {
		persona_index = m->persona_index;
		
		// if this ani should be converted to a terran command, set the persona to the command persona
		// so the correct head plays.
		if ( q->flags & MQF_CONVERT_TO_COMMAND ) {
			persona_index = The_mission.command_persona;
			strcpy( ani_name, COMMAND_HEAD_PREFIX );
		}

		// Goober5000 - guard against negative array indexing; this way, if no persona was
		// assigned, the logic will drop down below like it's supposed to
		if (persona_index >= 0)
		{
			if ( Personas[persona_index].flags & (PERSONA_FLAG_WINGMAN | PERSONA_FLAG_SUPPORT) ) {
				// get a random head
				if ( q->builtin_type == MESSAGE_WINGMAN_SCREAM ) {
					rand_index = MAX_WINGMAN_HEADS;		// [0,MAX) are regular heads; MAX is always death head
					is_death_scream = 1;
				} else {
					rand_index = ((int) Missiontime % MAX_WINGMAN_HEADS);
				}
				sprintf(ani_name, "%s%c", ani_name, 'a'+rand_index);
				subhead_selected = TRUE;
			} else if ( Personas[persona_index].flags & (PERSONA_FLAG_COMMAND | PERSONA_FLAG_LARGE) ) {
				// get a random head
				// Goober5000 - *sigh*... if mission designers assign a command persona
				// to a wingman head, they risk having the death ani play
				Assert(strlen(ani_name) >= 7);
				if (!strnicmp(ani_name+5,"CM",2) || !strnicmp(ani_name+5,"BS",2))	// Head-CM* or Head-BSH
					rand_index = ((int) Missiontime % MAX_COMMAND_HEADS);
				else
					rand_index = ((int) Missiontime % MAX_WINGMAN_HEADS);
				sprintf(ani_name, "%s%c", ani_name, 'a'+rand_index);
				subhead_selected = TRUE;
			}
		}

		if (!subhead_selected) {
			// choose between a and b
			rand_index = ((int) Missiontime % MAX_WINGMAN_HEADS);
			sprintf(ani_name, "%s%c", ani_name, 'a'+rand_index);
			mprintf(("message '%s' with invalid head.  Fix by assigning persona to the message.\n", m->name));
		}
		nprintf(("Messaging", "playing head %s for %s\n", ani_name, q->who_from));
	}

	// check to see if the avi has been loaded.  If not, then load the AVI.  On an error loading
	// the avi, set the top level index to -1 to avoid multiple tries at loading the flick.

	// if there is something already here that's not this same file then go ahead a let go of it - taylor
	if ( (anim_info->anim_data != NULL) && !strstr(anim_info->anim_data->name, ani_name) ) {
		message_mission_free_avi( m->avi_info.index );
	}

	anim_info->anim_data = anim_load( ani_name, CF_TYPE_ANY, 0 );

	if ( anim_info->anim_data == NULL ) {
		nprintf (("messaging", "Cannot load message avi %s.  Will not play.\n", ani_name));
		m->avi_info.index = -1;			// if cannot load the avi -- set this index to -1 to avoid trying to load multiple times
	}

	if ( m->avi_info.index >= 0 ) {
		// This call relies on the fact that AVI_play will return -1 if the AVI cannot be played
		// if any messages are already playing, kill off any head anims that are currently playing.  We will
		// only play a head anim of the newest messages being played
		if ( Num_messages_playing > 0 ) {
			nprintf(("messaging", "killing off any currently playing head animations\n"));
			message_kill_all( 0 );
		}

		if ( hud_disabled() ) {
			return;
		}

		int anim_start_frame;
		anim_play_struct aps;

		// figure out anim start frame
		anim_start_frame = message_calc_anim_start_frame(Message_wave_duration, anim_info->anim_data, is_death_scream);
		anim_play_init(&aps, anim_info->anim_data, Head_coords[gr_screen.res][0], Head_coords[gr_screen.res][1]);
		aps.start_at = anim_start_frame;

		// aps.color = &HUD_color_defaults[HUD_color_alpha];
		aps.color = &HUD_config.clr[HUD_TALKING_HEAD];

		Playing_messages[Num_messages_playing].anim = anim_play(&aps);
	}
}

// process the message queue -- called once a frame
void message_queue_process()
{	
	char	buf[MESSAGE_LENGTH];
	char who_from[NAME_LENGTH];	
	message_q *q;
	int i;
	MissionMessage *m;

	// Don't play messages until first frame has been rendered
	if ( Framecount < 2 ) {
		return;
	}

	// determine if all playing messages (if any) are done playing.  If any are done, remove their
	// entries collapsing the Playing_messages array if necessary
	if ( Num_messages_playing > 0 ) {

		// for each message playing, determine if it is done.
		i = 0;
		while ( i < Num_messages_playing ) {
			int ani_done, wave_done, j;

			ani_done = 1;
			if ( (Playing_messages[i].anim != NULL) && anim_playing(Playing_messages[i].anim) )
				ani_done = 0;

			wave_done = 1;

//			if ( (Playing_messages[i].wave != -1) && snd_is_playing(Playing_messages[i].wave) )
			if ( (Playing_messages[i].wave != -1) && (snd_time_remaining(Playing_messages[i].wave) > 250) )
				wave_done = 0;

			// Goober5000
			if (fsspeech_playing())
				wave_done = 0;

			// AL 1-20-98: If voice message is done, kill the animation early
			if ( (Playing_messages[i].wave != -1) && wave_done ) {
				if ( !ani_done ) {
					anim_stop_playing( Playing_messages[i].anim );
				}
			}

			//if player is a traitor remove all messages that aren't traitor related
			if ((Playing_messages[i].builtin_type != MESSAGE_OOPS) && (Playing_messages[i].builtin_type != MESSAGE_HAMMER_SWINE)) {
				if ( (Player_ship->team == Iff_traitor) && ( !(Game_mode & GM_MULTIPLAYER) || !(Netgame.type_flags & NG_TYPE_DOGFIGHT) ) ) {
					message_kill_playing(i);
					i++;
					continue;
				}
			}

			// see if the ship sending this message is dying.  If do, kill wave and anim
			if ( Playing_messages[i].shipnum != -1 ) {
				if ( (Ships[Playing_messages[i].shipnum].flags & SF_DYING) && (Playing_messages[i].builtin_type != MESSAGE_WINGMAN_SCREAM) ) {
					int shipnum;

					shipnum = Playing_messages[i].shipnum;
					message_kill_playing( i );
					// force this guy to scream
					// AL 22-2-98: Ensure don't use -1 to index into ships array.  Mark, something is incorrect 
					//             here, since message_kill_playing() seems to always set Playing_messages[i].shipnum to -1
					// MWA 3/24/98 -- save shipnum before killing message
					// 
					Assert( shipnum >= 0 );
					if ( !(Ships[shipnum].flags & SF_SHIP_HAS_SCREAMED) && !(Ships[shipnum].flags2 & SF2_NO_DEATH_SCREAM) ) {
						ship_scream( &Ships[shipnum] );
					}
					continue;							// this should keep us in the while() loop with same value of i.														
				}											// we should enter the next 'if' statement during next pass
			}

			// if both ani and wave are done, mark internal variable so we can do next message on queue, and
			// global variable to clear voice brackets on hud
			if ( wave_done && ani_done ) {
				nprintf(("messaging", "Message %d is done playing\n", i));
				Message_shipnum = -1;
				Num_messages_playing--;
				if ( Num_messages_playing == 0 )
					break;

				// there is still another message playing.  Collapse the playing_message array
				nprintf(("messaging", "Collapsing playing message stack\n"));
				for ( j = i+1; j < Num_messages_playing + 1; j++ ) {
					Playing_messages[j-1] = Playing_messages[j];
				}
			} else {
				// messages is not done playing -- move to next message
				i++;
			}
		}
	}

	// preprocess message queue and remove anything on the queue that is too old.  If next message on
	// the queue can be played, then break out of the loop.  Otherwise, loop until nothing on the queue
	while ( MessageQ_num > 0 ) {
		q = &MessageQ[0];		
		if ( timestamp_valid(q->window_timestamp) && timestamp_elapsed(q->window_timestamp) && !q->group) {
			// remove message from queue and see if more to remove
			nprintf(("messaging", "Message %s didn't play because it didn't fit into time window.\n", Messages[q->message_num].name));
			if ( q->message_num < Num_builtin_messages ){			// we should only ever remove builtin messages this way
				message_remove_from_queue(q);
			} else {
				break;
			}
		} else {
			break;
		}
	}

	// no need to process anything if there isn't anything on the queue
	if ( MessageQ_num <= 0 ){
		return;
	}

	// get a pointer to an item on the queue
	int found = -1;
	int idx = 0;
	while((found == -1) && (idx < MessageQ_num)){
		// if this guy has no min delay timestamp, or it has expired, select him
		if((MessageQ[idx].min_delay_stamp == -1) || timestamp_elapsed(MessageQ[idx].min_delay_stamp)){
			found = idx;
			break;
		}

		// next
		idx++;
	}

	// if we didn't find anything, bail
	if(found == -1){
		return;
	}
	// if this is not the first item on the queue, make it the first item
	if(found != 0){
		message_q temp;

		// store the entry
		memcpy(&temp, &MessageQ[found], sizeof(message_q));

		// move all other entries up
		for(idx=found; idx>0; idx--){
			memcpy(&MessageQ[idx], &MessageQ[idx-1], sizeof(message_q));
		}

		// plop the entry down as being first
		memcpy(&MessageQ[0], &temp, sizeof(message_q));
	}

	q = &MessageQ[0];
	Assert ( q->message_num != -1 );
	Assert ( q->priority != -1 );
	Assert ( q->time_added != -1 );

	if ( Num_messages_playing ) {
		// peek at the first message on the queue to see if it should interrupt, or overlap a currently
		// playing message.  Mission specific messages will always interrupt builtin messages.  They
		// will never interrupt other mission specific messages.
		//
		//  Builtin message might interrupt other builtin messages, or overlap them, all depending on
		// message priority.

		if ( q->builtin_type == MESSAGE_HAMMER_SWINE ) {
			message_kill_all(1);
		} else if ( message_playing_specific_builtin(MESSAGE_HAMMER_SWINE) ) {
			MessageQ_num = 0;
			return;
		} else if ( message_playing_builtin() && ( q->message_num >= Num_builtin_messages) && (q->priority > MESSAGE_PRIORITY_LOW) ) {
			// builtin is playing and we have a unique message to play.  Kill currently playing message
			// so unique can play uninterrupted.  Only unique messages higher than low priority will interrupt
			// other messages.
			message_kill_all(1);
			nprintf(("messaging", "Killing all currently playing messages to play unique message\n"));
		} else if ( message_playing_builtin() && (q->message_num < Num_builtin_messages) ) {
			// when a builtin message is queued, we might either overlap or interrupt the currently
			// playing message.
			//
			// we have to check for num_messages_playing (again), since code for death scream might
			// kill all messages.
			if ( Num_messages_playing ) {
				if ( message_get_priority(MESSAGE_GET_HIGHEST) < q->priority ) {
					// lower priority message playing -- kill it.
					message_kill_all(1);
					nprintf(("messaging", "Killing all currently playing messages to play high priority builtin\n"));
				} else if ( message_get_priority(MESSAGE_GET_LOWEST) > q->priority ) {
					// queued message is a lower priority, so wait it out
					return;
				} else {
					// if we get here, then queued messages is a builtin message with the same priority
					// as the currently playing messages.  This state will cause messages to overlap.
					nprintf(("messaging", "playing builtin message (overlap) because priorities match\n"));
				}
			}
		} else if ( message_playing_unique() && (q->message_num < Num_builtin_messages) ) {
			// code messages can kill any low priority mission specific messages
			if ( Num_messages_playing ) {
				if ( message_get_priority(MESSAGE_GET_HIGHEST) == MESSAGE_PRIORITY_LOW ) {
					message_kill_all(1);
					nprintf(("messaging", "Killing low priority unique messages to play code message\n"));
				} else {
					return;			// do nothing.
				}
			}
		} else {
			return;
		}
	}

	// if we are playing the maximum number of voices, then return.  Make the check here since the above
	// code might kill off currently playing messages
	if ( Num_messages_playing == MAX_PLAYING_MESSAGES )
		return;

	Message_shipnum = ship_name_lookup( q->who_from );

	// see if we need to check if sending ship is alive
	if ( q->flags & MQF_CHECK_ALIVE ) {
		if ( Message_shipnum == -1 ) {
			goto all_done;
		}
	}

	// if this is a ship, then don't play anything if this ship is already talking
	if ( Message_shipnum != -1 ) {
		for ( i = 0; i < Num_messages_playing; i++ ) {
			if ( (Playing_messages[i].shipnum != -1) && (Playing_messages[i].shipnum == Message_shipnum) ){
				return;
			}
		}
	}

	// set up module globals for this message
	m = &Messages[q->message_num];
	Playing_messages[Num_messages_playing].anim = NULL;
	Playing_messages[Num_messages_playing].wave  = -1;
	Playing_messages[Num_messages_playing].id  = q->message_num;
	Playing_messages[Num_messages_playing].priority = q->priority;
	Playing_messages[Num_messages_playing].shipnum = Message_shipnum;
	Playing_messages[Num_messages_playing].builtin_type = q->builtin_type;

	Message_wave_duration = 0;

	// translate tokens in message to the real things
	if (q->special_message == NULL)
		message_translate_tokens(buf, m->message);
	else
		message_translate_tokens(buf, q->special_message);

	// AL: added 07/14/97.. only play avi/sound if in gameplay
	if ( gameseq_get_state() != GS_STATE_GAME_PLAY )
		goto all_done;

	// AL 4-7-98: Can't receive messages if comm is destroyed
	if ( hud_communications_state(Player_ship) == COMM_DESTROYED ) {
		goto all_done;
	}

	//	Don't play death scream unless a small ship.
	if ( q->builtin_type == MESSAGE_WINGMAN_SCREAM ) {
		int t = Ship_info[Ships[Message_shipnum].ship_info_index].flags;
		int t2 = SIF_SMALL_SHIP;
		int t3 = t & t2;
		if (!t3) {
			goto all_done;
		}
	}

	// play wave first, since need to know duration for picking anim start frame
	if(message_play_wave(q) == false) {
		fsspeech_play(FSSPEECH_FROM_INGAME, buf);
	}

	// play animation for head
	#ifndef DEMO // do we want this for FS2_DEMO
		message_play_anim(q);
	#endif
	
	// distort the message if comms system is damaged
	message_maybe_distort_text(buf);

#ifndef NDEBUG
	// debug only -- if the message is a builtin message, put in parens whether or not the voice played
	if ( Sound_enabled && (Playing_messages[Num_messages_playing].wave == -1) ) {
		strcat( buf, NOX("..(no wavefile for voice)"));
		snd_play(&Snds[SND_CUE_VOICE]);
	}
#endif
	
	strncpy (who_from, q->who_from, NAME_LENGTH);

	// if this is a ship, do we use name or callsign or ship class?
	if ( Message_shipnum >= 0 ) {
		ship *shipp = &Ships[Message_shipnum];
		if ( shipp->callsign_index >= 0 ) {
			hud_stuff_ship_callsign( who_from, shipp );
		} else if ( ((Iff_info[shipp->team].flags & IFFF_WING_NAME_HIDDEN) && (shipp->wingnum != -1)) || (shipp->flags2 & SF2_HIDE_SHIP_NAME) ) {
			hud_stuff_ship_class( who_from, shipp );
		} else {
			end_string_at_first_hash_symbol(who_from);
		}
	}

	HUD_sourced_printf( q->source, NOX("%s: %s"), who_from, buf );

	if ( Message_shipnum >= 0 ) {
		hud_target_last_transmit_add(Message_shipnum);
	}

all_done:
	Num_messages_playing++;
	message_remove_from_queue( q );
}

// queues up a message to display to the player
void message_queue_message( int message_num, int priority, int timing, char *who_from, int source, int group, int delay, int builtin_type )
{
	int i, m_persona;
	char temp_buf[MESSAGE_LENGTH];

	if ( message_num < 0 ) return;

	// some messages can get queued quickly.  Try to filter out certain types of messages before
	// they get queued if there are other messages of the same type already queued
	if ( (builtin_type == MESSAGE_REARM_ON_WAY) || (builtin_type == MESSAGE_OOPS) ) {
		// if it is already playing, then don't play it
		if ( message_playing_specific_builtin(builtin_type) ) 
			return;

		for ( i = 0; i < MessageQ_num; i++ ) {
			// if one of these messages is already queued, then don't play
			if ( (MessageQ[i].message_num == message_num) && (MessageQ[i].builtin_type == builtin_type) )
				return;

		}
	}

	// check to be sure that we haven't reached our max limit on these messages yet.
	if ( MessageQ_num == MAX_MESSAGE_Q ) {
		Int3();											
		return;
	}

	// if player is a traitor, no messages for him!!!
	// unless those messages are traitor related
	// Goober5000 - allow messages during multiplayer dogfight (Mantis #1436)
	if ( (Player_ship->team == Iff_traitor) && ( !(Game_mode & GM_MULTIPLAYER) || !(Netgame.type_flags & NG_TYPE_DOGFIGHT) ) ) {
		if ((builtin_type != MESSAGE_OOPS) && (builtin_type != MESSAGE_HAMMER_SWINE)) {
			return;
		}
	}

	m_persona = Messages[message_num].persona_index;

	// put the message into a slot
	i = MessageQ_num;
	MessageQ[i].time_added = Missiontime;
	MessageQ[i].priority = priority;
	MessageQ[i].message_num = message_num;
	MessageQ[i].source = source;
	MessageQ[i].builtin_type = builtin_type;
	MessageQ[i].min_delay_stamp = timestamp(delay);
	MessageQ[i].group = group;
	strcpy(MessageQ[i].who_from, who_from);

	// Goober5000 - this shouldn't happen, but let's be safe
	if (MessageQ[i].special_message != NULL)
	{
		Int3();
		vm_free(MessageQ[i].special_message);
		MessageQ[i].special_message = NULL;
	}

	// Goober5000 - replace variables if necessary
	strcpy(temp_buf, Messages[message_num].message);
	if (sexp_replace_variable_names_with_values(temp_buf, MESSAGE_LENGTH))
		MessageQ[i].special_message = vm_strdup(temp_buf);

	// SPECIAL HACK -- if the who_from is terran command, and there is a wingman persona attached
	// to this message, then set a bit to tell the wave/anim playing code to play the command version
	// of the wave and head
	MessageQ[i].flags = 0;
	if ( !stricmp(who_from, The_mission.command_sender) && (m_persona != -1) && (Personas[m_persona].flags & PERSONA_FLAG_WINGMAN) ) {
		MessageQ[i].flags |= MQF_CONVERT_TO_COMMAND;
		MessageQ[i].source = HUD_SOURCE_TERRAN_CMD;
	}

	if ( (m_persona != -1) && (Personas[m_persona].flags & PERSONA_FLAG_WINGMAN) ) {
		if ( !strstr(who_from, ".wav") ) {
			MessageQ[i].flags |= MQF_CHECK_ALIVE;
		}
	}

	// set the timestamp of when to play this message based on the 'timing' value
	if ( timing == MESSAGE_TIME_IMMEDIATE )
		MessageQ[i].window_timestamp = timestamp(MESSAGE_IMMEDIATE_TIMESTAMP);
	else if ( timing == MESSAGE_TIME_SOON )
		MessageQ[i].window_timestamp = timestamp(MESSAGE_SOON_TIMESTAMP);
	else
		MessageQ[i].window_timestamp = timestamp(MESSAGE_ANYTIME_TIMESTAMP);		// make invalid

	MessageQ_num++;
	insertion_sort(MessageQ, MAX_MESSAGE_Q, sizeof(message_q), message_queue_priority_compare);

	// Try to start it!
	// MWA -- called every frame from game loop
	//message_queue_process();
}

// function to return the persona index of the given ship.  If it isn't assigned, it will be
// in this function.  persona_type could be a wingman, Terran Command, or other generic ship
// type personas.  ship is the ship we should assign a persona to
int message_get_persona( ship *shipp )
{
	int i = 0, ship_type, count;
	int *slist = (int*)vm_malloc(sizeof(int)*Num_personas);
	memset( slist, 0, sizeof(int) * Num_personas );

	if ( shipp != NULL ) {
		// see if this ship has a persona
		if ( shipp->persona_index != -1 ) {
		//	return shipp->persona_index;
			i = shipp->persona_index;
			goto I_Done;
		}

		// get the type of ship (i.e. support, fighter/bomber, etc)
		ship_type = Ship_info[shipp->ship_info_index].flags;

		int persona_needed;
		count = 0;

		if ( ship_type & (SIF_FIGHTER|SIF_BOMBER) )
		{
			persona_needed = PERSONA_FLAG_WINGMAN;
		} else if ( ship_type & SIF_SUPPORT ) 
		{
			persona_needed = PERSONA_FLAG_SUPPORT;
		}
		else 
		{
			persona_needed = PERSONA_FLAG_LARGE;
		}

		// first try to go for an unused persona
		for (i = 0; i < Num_personas; i++)
		{
			// this Persona is not our species - skip it
			if (Personas[i].species != Ship_info[shipp->ship_info_index].species)
				continue;

			// check the ship types, and don't try to assign those which don't type match
			if ( Personas[i].flags & persona_needed)
			{
				if (!(Personas[i].flags & PERSONA_FLAG_USED))
				{
					// if it hasn't been used - USE IT!
					Personas[i].flags |= PERSONA_FLAG_USED;
				//	return i;
					goto I_Done;
				}
				else
				{
					// otherwise add it to our list of valid options to randomly select from
					slist[count] = i;
					count++;
				}
			}
		}

		// we didn't find an unused one - so we randomly select one
		if(count != 0)
		{
			i = (rand() % count);
			i = slist[i];
		}
		// RT Protect against count being zero
		else
			i = slist[0];

		//return i;
		goto I_Done;
	}

	// for now -- we don't support other types of personas (non-wingman personas)
	Int3();
//	return 0;

I_Done:
	vm_free(slist);

	return i;
}

// given a message id#, should it be filtered for me?
int message_filter_multi(int id)
{
	// not multiplayer
	if(!(Game_mode & GM_MULTIPLAYER)){
		return 0;
	}

	// bogus
	if((id < 0) || (id >= Num_messages)){
		mprintf(("Filtering bogus mission message!\n"));
		return 1;
	}

	// builtin messages
	if(id < Num_builtin_messages){
	}
	// mission-specific messages
	else {
		// not team filtered
		if(Messages[id].multi_team < 0){
			return 0;
		}

		// not TvT
		if(!(Netgame.type_flags & NG_TYPE_TEAM)){
			return 0;
		}

		// is this for my team?
		if((Net_player != NULL) && (Net_player->p_info.team != Messages[id].multi_team)){
			mprintf(("Filtering team-based mission message!\n"));
			return 1;
		}
	}		
	
	return 0;
}

// send_unique_to_player sends a mission unique (specific) message to the player (possibly a multiplayer
// person).  These messages are *not* the builtin messages
void message_send_unique_to_player( char *id, void *data, int m_source, int priority, int group, int delay )
{
	int i, source;
	char *who_from;

	source = 0;
	who_from = NULL;
	for (i=0; i<Num_messages; i++) {
		// find the message
		if ( !stricmp(id, Messages[i].name) ) {

			// if the ship pointer and special_who are both NULL then this is from generic "Terran Command"
			// if the ship is NULL and special_who is not NULL, then this is from special_who
			// otherwise, message is from ship.
			if ( m_source == MESSAGE_SOURCE_COMMAND ) {
				who_from = The_mission.command_sender;
				source = HUD_SOURCE_TERRAN_CMD;
			} else if ( m_source == MESSAGE_SOURCE_SPECIAL ) {
				who_from = (char *)data;
				source = HUD_SOURCE_TERRAN_CMD;
			} else if ( m_source == MESSAGE_SOURCE_WINGMAN ) {
				int m_persona, ship_index;

				// find a wingman with the same persona as this message.  If the message's persona doesn't
				// exist, we will use Terran command
				m_persona = Messages[i].persona_index;
				if ( m_persona == -1 ) {
					mprintf(("Warning:  Message %d has no persona assigned.\n", i));
				}

				// get a ship. we allow silenced ships since this is a unique messange and therefore the mission designer 
				// should have taken into account that the ship may have been silenced.
				ship_index = ship_get_random_player_wing_ship( SHIP_GET_NO_PLAYERS, 0.0f, m_persona, 1, Messages[i].multi_team);

				// if the ship_index is -1, then make the message come from Terran command
				if ( ship_index == -1 ) {
					who_from = The_mission.command_sender;
					source = HUD_SOURCE_TERRAN_CMD;
				} else {
					who_from = Ships[ship_index].ship_name;
					source = HUD_team_get_source(Ships[ship_index].team);
				}

			} else if ( m_source == MESSAGE_SOURCE_SHIP ) {
				ship *shipp;

				shipp = (ship *)data;
				who_from = shipp->ship_name;
				source = HUD_team_get_source(shipp->team);

				// be sure that this ship can actually send a message!!! (i.e. not-not-flyable -- get it!)
				Assert( !(Ship_info[shipp->ship_info_index].flags & SIF_NOT_FLYABLE) );		// get allender or alan
			}

			// not multiplayer or this message is for me, then queue it
			// if ( !(Game_mode & GM_MULTIPLAYER) || ((multi_target == -1) || (multi_target == MY_NET_PLAYER_NUM)) ){

			// maybe filter it out altogether
			if (!message_filter_multi(i)) {
				message_queue_message( i, priority, MESSAGE_TIME_ANYTIME, who_from, source, group, delay );
			}

			// record to the demo if necessary
			if(Game_mode & GM_DEMO_RECORD){
				demo_POST_unique_message(id, who_from, m_source, priority);
			}
			// }

			// send a message packet to a player if destined for everyone or only a specific person
			if ( MULTIPLAYER_MASTER ){
				send_mission_message_packet( i, who_from, priority, MESSAGE_TIME_SOON, source, -1, -1, -1);
			}			

			return;		// all done with displaying		
		}
	}
	nprintf (("messaging", "Couldn't find message id %s to send to player!\n", id ));
}

// send builtin_to_player sends a message (from messages.tbl) to the player.  These messages are
// the generic informational type messages.  The have priorities like misison specific messages,
// and use a timing to tell how long we should wait before playing this message
void message_send_builtin_to_player( int type, ship *shipp, int priority, int timing, int group, int delay, int multi_target, int multi_team_filter )
{
	int i, persona_index = -1;
	int source;	

	// if we aren't showing builtin msgs, bail
	if (The_mission.flags & MISSION_FLAG_NO_BUILTIN_MSGS)
		return;

	// Karajorma - If we aren't showing builtin msgs from command and this is not a ship, bail
	if ( (shipp == NULL) && (The_mission.flags & MISSION_FLAG_NO_BUILTIN_COMMAND) ) 
		return;

	// see if there is a persona assigned to this ship.  If not, then try to assign one!!!
	if ( shipp ) {
		// Karajorma - the game should assert if a silenced ship gets this far
		Assert( !(shipp->flags2 & SF2_NO_BUILTIN_MESSAGES) );

		if ( shipp->persona_index == -1 )
			shipp->persona_index = message_get_persona( shipp );

		persona_index = shipp->persona_index;

		if ( persona_index == -1 )
			nprintf(("messaging", "Couldn't find persona for %s\n", shipp->ship_name ));	

		// be sure that this ship can actually send a message!!! (i.e. not-not-flyable -- get it!)
		Assert( !(Ship_info[shipp->ship_info_index].flags & SIF_NOT_FLYABLE) );		// get allender or alan
	} else {
		persona_index = The_mission.command_persona;				// use the terran command persona
	}

	// try to find a builtin message with the given type for the given persona
	// make a loop out of this routne since we may try to play a message in the wrong
	// persona if we can't find the right message for the given persona
	do {
		for ( i = 0; i < Num_builtin_messages; i++ ) {
			char *name, *who_from;

			name = Builtin_message_types[type];

			// see if the have the type of message
			if ( stricmp(Messages[i].name, name) ){
				continue;
			}

			// must have the correct persona.  persona_index of -1 means find the first
			// message possibly of the correct type
			if ( (persona_index != -1 ) && (Messages[i].persona_index != persona_index) ){
				continue;
			}

			// get who this message is from -- kind of a hack since we assume Terran Command in the
			// absence of a ship.  This will be fixed later
			if ( shipp ) {
				who_from = shipp->ship_name;
				source = HUD_team_get_source( shipp->team );
			} else {
				who_from = The_mission.command_sender;

				// Goober5000 - if Command is a ship that is present, change the source accordingly
				int shipnum = ship_name_lookup(who_from);
				if (shipnum >= 0)
					source = HUD_team_get_source( Ships[shipnum].team );
				else
					source = HUD_SOURCE_TERRAN_CMD;
			}

			// maybe change the who from here for special rearm cases (always seems like that is the case :-) )
			if ( !stricmp(who_from, The_mission.command_sender) && (type == MESSAGE_REARM_ON_WAY) ){
				who_from = SUPPORT_NAME;
			}

			// determine what we should actually do with this dang message.  In multiplayer, we must
			// deal with the fact that this message might not get played on my machine if I am a server

			// not multiplayer or this message is for me, then queue it
			if ( !(Game_mode & GM_MULTIPLAYER) || ((multi_target == -1) || (multi_target == MY_NET_PLAYER_NUM)) ){

				// if this filter matches mine
				if( (multi_team_filter < 0) || !(Netgame.type_flags & NG_TYPE_TEAM) || ((Net_player != NULL) && (Net_player->p_info.team == multi_team_filter)) ){
					message_queue_message( i, priority, timing, who_from, source, group, delay, type );

					// post a builtin message
					if(Game_mode & GM_DEMO_RECORD){
						demo_POST_builtin_message(type, shipp, priority, timing);
					}
				}
			}

			// send a message packet to a player if destined for everyone or only a specific person
			if ( MULTIPLAYER_MASTER ) {
				// only send a message if it is of a particular type
				if(multi_target == -1){
					if(multi_message_should_broadcast(type)){				
						send_mission_message_packet( i, who_from, priority, timing, source, type, -1, multi_team_filter );
					}
				} else {
					send_mission_message_packet( i, who_from, priority, timing, source, type, multi_target, multi_team_filter );
				}
			}

			return;		// all done with displaying
		}

		if ( persona_index >= 0 ) {
			nprintf(("messaging", "Couldn't find builtin message %s for persona %d\n", Builtin_message_types[type], persona_index ));
			nprintf(("messaging", "looking for message for any persona\n"));
			persona_index = -1;
		} else {
			persona_index = -999;		// used here and the next line only -- hard code bad, but I'm lazy
		}
	} while ( persona_index != -999 );
}

// message_is_playing()
//
// Return the Message_playing flag.  Message_playing is local to MissionMessage.cpp, but
// this info is needed by code in HUDsquadmsg.cpp
//
int message_is_playing()
{
	return Num_messages_playing?1:0;
}

// Functions below pertain only to personas!!!!

// given a character string, try to find the persona index
int message_persona_name_lookup( char *name )
{
	int i;

	for (i = 0; i < Num_personas; i++ ) {
		if ( !stricmp(Personas[i].name, name) )
			return i;
	}

	return -1;
}

extern bool Sexp_Messages_Scrambled;
// Blank out portions of the audio playback for the sound identified by Message_wave
// This works by using the same Distort_pattern[][] that was used to distort the associated text
void message_maybe_distort()
{
	int i;
	int was_muted;

	if ( Num_messages_playing == 0 )
		return;
	
	for ( i = 0; i < Num_messages_playing; i++ ) {
		if ( !snd_is_playing(Playing_messages[i].wave) )
			return;
	}

	// distort the number of voices currently playing
	for ( i = 0; i < Num_messages_playing; i++ ) {
		Assert(Playing_messages[i].wave >= 0 );

		was_muted = 0;

		// added check to see if EMP effect was active
		// 8/24/98 - DB
		if ( (hud_communications_state(Player_ship) != COMM_OK) || emp_active_local() || Sexp_Messages_Scrambled ) {
			was_muted = Message_wave_muted;
			if ( timestamp_elapsed(Next_mute_time) ) {
				Next_mute_time = fl2i(Distort_patterns[Distort_num][Distort_next++] * Message_wave_duration);
				if ( Distort_next >= MAX_DISTORT_LEVELS )
					Distort_next = 0;

				Message_wave_muted ^= 1;
			}
		
			if ( Message_wave_muted ) {
				if ( !was_muted )
					snd_set_volume(Playing_messages[i].wave, 0.0f);
			} else {
				if ( was_muted )
					snd_set_volume(Playing_messages[i].wave, Master_sound_volume);
			}
		}
	}
}


// if the player communications systems are heavily damaged, distort incoming messages.
//
// first case: Message_wave_duration == 0 (this occurs when there is no associated voice playback)
//					Blank out random runs of characters in the message
//
// second case: Message_wave_duration > 0 (occurs when voice playback accompainies message)
//					 Blank out portions of the sound based on Distort_num, this this is that same
//					 data that will be used to blank out portions of the audio playback
//
void message_maybe_distort_text(char *text)
{
	int i, j, len, run, curr_offset, voice_duration, next_distort;

	if ( (hud_communications_state(Player_ship) == COMM_OK) && !emp_active_local() && !Sexp_Messages_Scrambled ) { 
		return;
	}

	len = strlen(text);
	if ( Message_wave_duration == 0 ) {
		next_distort = 5+myrand()%5;
		for ( i = 0; i < len; i++ ) {
			if ( i == next_distort ) {
				run = 3+myrand()%5;
				if ( i+run > len )
					run = len-i;
				for ( j = 0; j < run; j++) {
					text[i++] = '-';
					if ( i >= len )
						break;
				}
				next_distort = i + (5+myrand()%5);
			}
		}
		return;
	}

	voice_duration = Message_wave_duration;

	// distort text
	Distort_num = myrand()%MAX_DISTORT_PATTERNS;
	Distort_next = 0;
	curr_offset = 0;
	while (voice_duration > 0) {
		run = fl2i(Distort_patterns[Distort_num][Distort_next] * len);
		if (Distort_next & 1) {
			for ( i = curr_offset; i < MIN(len, curr_offset+run); i++ ) {
				if ( text[i] != ' ' ) 
					text[i] = '-';
			}
			curr_offset = i;
			if ( i >= len )
				break;
		} else {
			curr_offset += run;
		}

		voice_duration -= fl2i(Distort_patterns[Distort_num][Distort_next]*Message_wave_duration);
		Distort_next++;
		if ( Distort_next >= MAX_DISTORT_LEVELS )
			Distort_next = 0;
	};
	
	Distort_next = 0;
}

// return 1 if a talking head animation is playing, otherwise return 0
int message_anim_is_playing()
{
	int i;

	for (i = 0; i < Num_messages_playing; i++ ) {
		if ( (Playing_messages[i].anim != NULL) && anim_playing(Playing_messages[i].anim) )
			return 1;
	}

	return 0;
}

// Load mission messages (this is called by the level paging code when running with low memory)
void message_pagein_mission_messages()
{
	int i;
	
	mprintf(("Paging in mission messages\n"));

	if (Num_messages <= Num_builtin_messages) {
		return;
	}

	char *sound_filename;

	for (i=Num_builtin_messages; i<Num_messages; i++) {
		if (Messages[i].wave_info.index != -1) {
			sound_filename = Message_waves[Messages[i].wave_info.index].name;
			message_load_wave(Messages[i].wave_info.index, sound_filename);
		}
	}
}

// ---------------------------------------------------
// Add and remove messages - used by autopilot code now, but useful elswhere

bool add_message(char *name, char *message, int persona_index, int multi_team)
{
	if (MAX_MISSION_MESSAGES == Num_messages)
		return false;
	strcpy(Messages[Num_messages].name, name);
	strcpy(Messages[Num_messages].message, message);
	Messages[Num_messages].persona_index = persona_index;
	Messages[Num_messages].multi_team = multi_team;
	Messages[Num_messages].avi_info.index = -1;
	Messages[Num_messages].wave_info.index = -1;
	Num_messages++;

	return true;
}

bool change_message(char *name, char *message, int persona_index, int multi_team)
{
	for (int i = Num_builtin_messages; i < Num_messages; i++) 
	{
		if (!strcmp(Messages[i].name, name)) 
		{
			strcpy(Messages[i].message, message);
			Messages[i].persona_index = persona_index;
			Messages[i].multi_team = multi_team;

			Messages[i].avi_info.index = -1;
			Messages[i].wave_info.index = -1;
			return true;
		}
	}

	// not found.. fall through
	return add_message(name, message, persona_index, multi_team);
}
