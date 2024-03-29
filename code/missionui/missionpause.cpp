/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/MissionUI/MissionPause.cpp $
 * $Revision: 2.18 $
 * $Date: 2006-03-26 08:23:06 $
 * $Author: taylor $
 * 
 * $Log: not supported by cvs2svn $
 * Revision 2.17  2006/03/21 14:19:18  taylor
 * add commands to view from and change target in viewer mode pause
 * also add reset ability to both view mode and hud state to return to pre-pause settings
 *
 * Revision 2.16  2005/10/10 17:21:06  taylor
 * remove NO_NETWORK
 *
 * Revision 2.15  2005/07/22 10:18:39  Goober5000
 * CVS header tweaks
 * --Goober5000
 *
 * Revision 2.14  2005/07/13 03:25:58  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.13  2005/07/02 19:43:54  taylor
 * ton of non-standard resolution fixes
 *
 * Revision 2.12  2005/06/19 02:38:33  taylor
 * fix possible crash from popups, especially in multi
 *
 * Revision 2.11  2005/03/25 06:57:36  wmcoolmon
 * Big, massive, codebase commit. I have not removed the old ai files as the ones I uploaded aren't up-to-date (But should work with the rest of the codebase)
 *
 * Revision 2.10  2005/03/02 21:24:45  taylor
 * more network/inferno goodness for Windows, takes care of a few warnings too
 *
 * Revision 2.9  2005/02/23 04:55:07  taylor
 * more bm_unload() -> bm_release() changes
 *
 * Revision 2.8  2005/02/21 09:00:58  wmcoolmon
 * Multi-res support
 *
 * Revision 2.7  2004/07/26 20:47:40  Kazan
 * remove MCD complete
 *
 * Revision 2.6  2004/07/12 16:32:55  Kazan
 * MCD - define _MCD_CHECK to use memory tracking
 *
 * Revision 2.5  2004/03/05 09:01:55  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.4  2003/03/18 10:07:04  unknownplayer
 * The big DX/main line merge. This has been uploaded to the main CVS since I can't manage to get it to upload to the DX branch. Apologies to all who may be affected adversely, but I'll work to debug it as fast as I can.
 *
 * Revision 2.3  2002/10/22 17:42:09  randomtiger
 * Fixed lighting bug that caused special pause to crash on debug build.
 * Also added TAB functionality for special pause that toggles HUD. - RT
 *
 * Revision 2.2  2002/10/19 03:50:29  randomtiger
 * Added special pause mode for easier action screenshots.
 * Added new command line parameter for accessing all single missions in tech room. - RT
 *
 * Revision 2.1.2.1  2002/09/24 18:56:44  randomtiger
 * DX8 branch commit
 *
 * This is the scub of UP's previous code with the more up to date RT code.
 * For full details check previous dev e-mails
 *
 * Revision 2.1  2002/08/01 01:41:07  penguin
 * The big include file move
 *
 * Revision 2.0  2002/06/03 04:02:25  penguin
 * Warpcore CVS sync
 *
 * Revision 1.2  2002/05/10 20:42:44  mharris
 * use "ifndef NO_NETWORK" all over the place
 *
 * Revision 1.1  2002/05/02 18:03:10  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 7     7/29/99 10:48p Dave
 * Multiplayer pause screen.
 * 
 * 6     6/29/99 7:39p Dave
 * Lots of small bug fixes.
 * 
 * 5     6/09/99 2:17p Dave
 * Fixed up pleasewait bitmap rendering.
 * 
 *
 * $NoKeywords: $
 */


#include "missionui/missionpause.h"
#include "ui/ui.h"
#include "popup/popup.h"
#include "io/key.h"
#include "sound/audiostr.h"
#include "gamesequence/gamesequence.h"
#include "freespace2/freespace.h"
#include "hud/hud.h"
#include "hud/hudmessage.h"
#include "object/object.h"
#include "graphics/font.h"
#include "globalincs/alphacolors.h"
#include "weapon/beam.h"	
#include "controlconfig/controlsconfig.h"
#include "network/multi_pause.h"




// ----------------------------------------------------------------------------------------------------------------
// PAUSE DEFINES/VARS
//

// pause bitmap name
char *Pause_bmp_name[GR_NUM_RESOLUTIONS] = {
	"PleaseWait",
	"2_PleaseWait"
};

// pause bitmap display stuff
int Please_wait_coords[GR_NUM_RESOLUTIONS][4] = {
	{ // GR_640
		152, 217, 316, 26
	},
	{ // GR_1024
		247, 346, 510, 36
	}	
};

// pause window objects
UI_WINDOW Pause_win;
UI_CHECKBOX Pause_single_step;
UI_CHECKBOX Pause_physics;
UI_CHECKBOX Pause_ai;
UI_CHECKBOX Pause_ai_render;
UI_CHECKBOX Pause_firing;
UI_CHECKBOX Pause_external_view_mode_check;
UI_BUTTON Pause_continue;
int Pause_type = PAUSE_TYPE_NORMAL;

// if we're already paused
int Paused = 0;

// background screen (for the chatbox)
int Pause_background_bitmap = -1;

// saved background screen
int Pause_saved_screen = -1;

// if we're in external vie wmode
int Pause_external_view_mode = 0;

// externs
extern int Ai_render_debug_flag;
extern int Ai_firing_enabled;
extern int physics_paused;
extern int ai_paused;


// ----------------------------------------------------------------------------------------------------------------
// PAUSE FUNCTIONS
//

int pause_get_type()
{
	return Pause_type;
}

void pause_set_type(int type)
{
	Pause_type = type;
}

// initialize the pause screen
void pause_init()
{
	// if we're already paused. do nothing
	if ( Paused ) {
		return;
	}

	Assert( !(Game_mode & GM_MULTIPLAYER) );

	// pause all beam weapon sounds
	beam_pause_sounds();

	if (Pause_type == PAUSE_TYPE_NORMAL)	{
		Pause_saved_screen = gr_save_screen();
	}

	// pause all game music
	audiostream_pause_all();

	//JAS: REMOVED CALL TO SET INTERFACE PALETTE TO GET RID OF SCREEN CLEAR WHEN PAUSING
	//common_set_interface_palette();  // set the interface palette
	Pause_win.create(0, 0, gr_screen.max_w_unscaled, gr_screen.max_h_unscaled, 0);	

	Pause_background_bitmap = bm_load(Pause_bmp_name[gr_screen.res]);

	Paused = 1;
}

extern int button_function_demo_valid(int n);
extern int button_function(int n);

// pause do frame - will handle running multiplayer operations if necessary
void pause_do()
{
	int k;
	char *pause_str = XSTR("Paused", 767);
	int str_w, str_h;
	// next two are for view resetting
	static int previous_Viewer_mode = -1;
	static int previous_hud_state = -1;

	Assert( !(Game_mode & GM_MULTIPLAYER) );
	
	//	RENDER A GAME FRAME HERE AS THE BACKGROUND (if normal pause)

	if(Pause_type == PAUSE_TYPE_NORMAL)	{			
		// Fall back to viewer just incase saved screen is invalid
		if(Pause_saved_screen == -1){
			Pause_type = PAUSE_TYPE_VIEWER;
		}
		else if(Pause_type == PAUSE_TYPE_NORMAL)	{
			gr_restore_screen(Pause_saved_screen);
		}
	}

	if(Pause_type == PAUSE_TYPE_NORMAL){
		if (Pause_background_bitmap >= 0) {
			gr_set_bitmap(Pause_background_bitmap);

			// draw the bitmap
			gr_bitmap(Please_wait_coords[gr_screen.res][0], Please_wait_coords[gr_screen.res][1]);
			
			// draw "Paused" on it
			gr_set_color_fast(&Color_normal);
			gr_set_font(FONT2);
			gr_get_string_size(&str_w, &str_h, pause_str);
			gr_string((gr_screen.max_w - str_w) / 2, (gr_screen.max_h - str_h) / 2, pause_str, false);
			gr_set_font(FONT1);
		}
	}

	if (Pause_type == PAUSE_TYPE_VIEWER) {
		if (previous_Viewer_mode < 0)
			previous_Viewer_mode = Viewer_mode;

		if (previous_hud_state < 0)
			previous_hud_state = hud_disabled();
	}

	// process the ui window here
	k = Pause_win.process() & ~KEY_DEBUGGED;
	switch (k)
	{ 
		case KEY_TAB:
			hud_toggle_draw();
			break;

		// view from outside of the ship
	   	case KEY_ENTER:
			if (Pause_type == PAUSE_TYPE_VIEWER) {
				button_function_demo_valid(VIEW_EXTERNAL);
			}
			break;

		// view from target
		case KEY_PADDIVIDE:
			if (Pause_type == PAUSE_TYPE_VIEWER) {
				button_function_demo_valid(VIEW_OTHER_SHIP);
			}
			break;

		// change target
		case KEY_PADMULTIPLY:
			if (Pause_type == PAUSE_TYPE_VIEWER) {
				button_function(TARGET_NEXT);
			}
			break;

		case KEY_ESC:
		case KEY_ALTED + KEY_PAUSE:
		case KEY_PAUSE:
			// reset previous view if we happened to be playing around with it during pause
			if (Pause_type == PAUSE_TYPE_VIEWER) {
				if (previous_Viewer_mode >= 0) {
					Viewer_mode = previous_Viewer_mode;
				}

				// NOTE remember that hud state is reversed here (0 == on, 1 == off)
				if ( (previous_hud_state >= 0) && (hud_disabled() != previous_hud_state) ) {
					hud_set_draw( !previous_hud_state );
				}
			}

			gameseq_post_event(GS_EVENT_PREVIOUS_STATE);		
			break;
	}	// end switch

	// draw the background window
	Pause_win.draw();

	// a very unique case where we shouldn't be doing the page flip because we're inside of popup code
	if(!popup_active()){
		if(Pause_type == PAUSE_TYPE_NORMAL) {
			gr_flip();
		}
	} else {
		// this should only be happening in a very unique multiplayer case
		Int3();
	}
}

// close the pause screen
void pause_close()
{
	// if we're not paused - do nothing
	if ( !Paused ) {
		return;
	}

	Assert( !(Game_mode & GM_MULTIPLAYER) );

	// unpause all beam weapon sounds
	beam_unpause_sounds();

	// deinit stuff
	if(Pause_saved_screen != -1) {
		gr_free_screen(Pause_saved_screen);
		Pause_saved_screen = -1;
	}

	if (Pause_background_bitmap != -1){
		bm_release(Pause_background_bitmap);
		Pause_background_bitmap = -1;
	}

	Pause_win.destroy();		
	game_flush();

	// unpause all the music
	audiostream_unpause_all();		

	Paused = 0;
}

// debug pause init
void pause_debug_init()
{
	Pause_win.create( 100,100,400,300, WIN_DIALOG );

	Pause_physics.create( &Pause_win, NOX("Physics Pause <P>"), 200, 150, physics_paused );
	Pause_ai.create( &Pause_win, NOX("AI Pause <A>"), 200, 175, ai_paused );
	#ifndef NDEBUG
	Pause_ai_render.create( &Pause_win, NOX("AI Render Stuff <R>"), 200, 200, Ai_render_debug_flag);
	#endif
	Pause_firing.create( &Pause_win, NOX("AI firing <F>"), 200, 225, Ai_firing_enabled);
	Pause_external_view_mode_check.create( &Pause_win, NOX("External View <E>"), 200, 250, Pause_external_view_mode);
	Pause_single_step.create( &Pause_win, NOX("Single Step <S>"), 200, 290, game_single_step );
	Pause_continue.create( &Pause_win, NOX("Leave Pause"), 200, 350, 200, 40 );

	Pause_single_step.set_hotkey( KEY_S );
	Pause_physics.set_hotkey( KEY_P );
	Pause_ai.set_hotkey( KEY_A );
	Pause_ai_render.set_hotkey( KEY_R );
	Pause_firing.set_hotkey( KEY_F );
	Pause_external_view_mode_check.set_hotkey( KEY_E );
	Pause_continue.set_hotkey( KEY_ESC );

	Pause_continue.set_focus();
}

// debug pause do frame
void pause_debug_do()
{
	int key;

	key = Pause_win.process();
	if ( Pause_single_step.changed())	{
		game_single_step = Pause_single_step.checked();
	}

	if ( Pause_physics.changed())	{
		physics_paused = Pause_physics.checked();
	}

	if ( Pause_ai.changed())	{
		ai_paused = Pause_ai.checked();
		if (ai_paused){
			obj_init_all_ships_physics();
		}
	}

	if ( Pause_ai_render.changed())	{
		Ai_render_debug_flag = Pause_ai_render.checked();
	}

	if ( Pause_firing.changed())	{
		Ai_firing_enabled = Pause_firing.checked();
	}

	if ( Pause_external_view_mode_check.changed())	{
		Pause_external_view_mode = Pause_external_view_mode_check.checked();
		if (Pause_external_view_mode){
			HUD_sourced_printf(HUD_SOURCE_HIDDEN, XSTR( "External view of player ship.", 182));
		} else {
			HUD_sourced_printf(HUD_SOURCE_HIDDEN, XSTR( "View from inside player ship.", 183));
		}
	}

	if ( Pause_continue.pressed() || (key == KEY_PAUSE) )	{	//	Changed, MK, 11/9/97, only Pause break pause.
		gameseq_post_event(GS_EVENT_PREVIOUS_STATE);
	}

	gr_clear();
	Pause_win.draw();

	gr_flip();
}

// debug pause close
void pause_debug_close()
{
	last_single_step = 0;	// Make so single step waits a frame before stepping
	Pause_win.destroy();
	game_flush();
}
