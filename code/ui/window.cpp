/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/

/*
 * $Logfile: /Freespace2/code/Ui/WINDOW.cpp $
 * $Revision: 2.8.2.1 $
 * $Date: 2007-09-02 02:07:48 $
 * $Author: Goober5000 $
 *
 * Routines to handle UI windows.
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.8  2005/07/22 03:53:32  taylor
 * fix crash on commit in the multi ship selection screen
 * mask_data in the UI code is always cast to ubyte so why it was a ushort type I'll never know
 * fix unsized text in input boxes (fixed the real problem earlier but forgot to remove this old fix)
 *
 * Revision 2.7  2005/07/18 03:45:10  taylor
 * more non-standard res fixing
 *  - I think everything should default to resize now (much easier than having to figure that crap out)
 *  - new mouse_get_pos_unscaled() function to return 1024x768/640x480 relative values so we don't have to do it later
 *  - lots of little cleanups which fix several strange offset/size problems
 *  - fix gr_resize/unsize_screen_pos() so that it won't wrap on int (took too long to track this down)
 *
 * Revision 2.6  2005/07/02 19:45:02  taylor
 * ton of non-standard resolution fixes
 *
 * Revision 2.5  2005/05/12 17:49:17  taylor
 * use vm_malloc(), vm_free(), vm_realloc(), vm_strdup() rather than system named macros
 *   fixes various problems and is past time to make the switch
 *
 * Revision 2.4  2005/01/31 23:27:55  taylor
 * merge with Linux/OSX tree - p0131-2
 *
 * Revision 2.3  2004/07/26 20:47:55  Kazan
 * remove MCD complete
 *
 * Revision 2.2  2004/07/12 16:33:08  Kazan
 * MCD - define _MCD_CHECK to use memory tracking
 *
 * Revision 2.1  2002/08/01 01:41:10  penguin
 * The big include file move
 *
 * Revision 2.0  2002/06/03 04:02:29  penguin
 * Warpcore CVS sync
 *
 * Revision 1.2  2002/05/04 04:52:22  mharris
 * 1st draft at porting
 *
 * Revision 1.1  2002/05/02 18:03:13  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 30    10/14/99 2:52p Jefff
 * localization fixes.  added support for hi-res specific xstr offsets
 * 
 * 29    8/16/99 4:06p Dave
 * Big honking checkin.
 * 
 * 28    7/16/99 1:50p Dave
 * 8 bit aabitmaps. yay.
 * 
 * 27    7/15/99 3:07p Dave
 * 32 bit detection support. Mouse coord commandline.
 * 
 * 26    7/13/99 5:38p Jefff
 * Added support for x offsets when drawing strings
 * 
 * 25    6/25/99 11:59a Dave
 * Multi options screen.
 * 
 * 24    6/22/99 7:03p Dave
 * New detail options screen.
 * 
 * 23    6/19/99 2:46p Dave
 * New control config screen.
 * 
 * 22    6/18/99 5:16p Dave
 * Added real beam weapon lighting. Fixed beam weapon sounds. Added MOTD
 * dialog to PXO screen.
 * 
 * 21    6/04/99 11:32a Dave
 * Added reset text to ship select screen. Fixed minor xstr bug in ui
 * window
 * 
 * 20    6/02/99 6:18p Dave
 * Fixed TNT lockup problems! Wheeeee!
 * 
 * 19    6/01/99 3:52p Dave
 * View footage screen. Fixed xstrings to not display the & symbol. Popup,
 * dead popup, pxo find player popup, pxo private room popup.
 * 
 * 18    5/22/99 5:35p Dave
 * Debrief and chatbox screens. Fixed small hi-res HUD bug.
 * 
 * 17    5/21/99 6:45p Dave
 * Sped up ui loading a bit. Sped up localization disk access stuff. Multi
 * start game screen, multi password, and multi pxo-help screen.
 * 
 * 16    5/17/99 11:28a Dave
 * 
 * 15    5/17/99 9:25a Dave
 * Updated PXO screen. Still needs popups.
 * 
 * 14    5/03/99 8:33p Dave
 * New version of multi host options screen.
 * 
 * 13    4/16/99 5:22p Neilk
 * Fixed UI_SLIDER2 bug
 * 
 * 12    2/21/99 6:02p Dave
 * Fixed standalone WSS packets. 
 * 
 * 11    2/01/99 5:55p Dave
 * Removed the idea of explicit bitmaps for buttons. Fixed text
 * highlighting for disabled gadgets.
 * 
 * 10    1/29/99 2:08a Dave
 * Fixed beam weapon collisions with players. Reduced size of scoring
 * struct for multiplayer. Disabled PXO.
 * 
 * 9     1/29/99 12:47a Dave
 * Put in sounds for beam weapon. A bunch of interface screens (tech
 * database stuff).
 * 
 * 8     12/18/98 1:13a Dave
 * Rough 1024x768 support for Direct3D. Proper detection and usage through
 * the launcher.
 * 
 * 7     12/02/98 5:47p Dave
 * Put in interface xstr code. Converted barracks screen to new format.
 * 
 * 6     11/30/98 1:07p Dave
 * 16 bit conversion, first run.
 * 
 * 5     10/23/98 3:51p Dave
 * Full support for tstrings.tbl and foreign languages. All that remains
 * is to make it active in Fred.
 * 
 * 4     10/13/98 9:29a Dave
 * Started neatening up freespace.h. Many variables renamed and
 * reorganized. Added AlphaColors.[h,cpp]
 * 
 * 3     10/07/98 6:27p Dave
 * Globalized mission and campaign file extensions. Removed Silent Threat
 * special code. Moved \cache \players and \multidata into the \data
 * directory.
 * 
 * 2     10/07/98 10:54a Dave
 * Initial checkin.
 * 
 * 1     10/07/98 10:51a Dave
 * 
 *
 * $NoKeywords: $
 */

#include "globalincs/pstypes.h"
#include "osapi/osapi.h"
#include "ui/uidefs.h"
#include "ui/ui.h"
#include "freespace2/freespace.h"
#include "bmpman/bmpman.h"
#include "palman/palman.h"
#include "parse/parselo.h"
#include "io/timer.h"
#include "globalincs/alphacolors.h"
#include "graphics/font.h"
#include "localization/localize.h"
#include "cmdline/cmdline.h"

#include "wii_port/wiiosk.h"


// global xstr colors
color *Xstr_colors[UI_NUM_XSTR_COLORS][3] = {
	{	// UI_XSTR_COLOR_GREEN
		&Color_normal,
		&Color_ui_light_green,
		&Color_ui_green,
	},
	{	// UI_XSTR_COLOR_PINK
		&Color_normal,
		&Color_ui_light_pink,
		&Color_ui_pink,
	},
};

// --------------------------------------------------------------------
// UI_WINDOW constructor
//
//
UI_WINDOW::UI_WINDOW()
{
	int idx;

	mask_bmap_id = -1;				// bitmap id of the mask bitmap to define hotspots
	foreground_bmap_id = -1;		// bitmap id of the foreground bitmap to display
	mask_bmap_ptr = NULL;			// pointer to bitmap of the mask
	mask_data = NULL;					// points to raw mask bitmap data
	mask_w = -1;						// bitmap width
	mask_h = -1;						// bitmap height
	osk_active = false;
	osk_disable = false;
	tooltip_handler = NULL;			// pointer to function to handle custom tooltips

	// NULL all the xstring structs
	for(idx=0; idx<MAX_UI_XSTRS; idx++){
		xstrs[idx] = NULL;
	}
}

// --------------------------------------------------------------------
// UI_WINDOW destructor
//
//
UI_WINDOW::~UI_WINDOW()
{
}

// --------------------------------------------------------------------
// UI_WINDOW::set_mask_bmap()
//
// Specify the filename for the mask bitmap to use.  This has the hotspots
// for all the different controls.
//
void UI_WINDOW::set_mask_bmap(char *fname)
{
	int bmap;

	bmap = bm_load(fname);

	if (bmap < 0) {
		Error(LOCATION, "Could not load in %s!", fname);

	} else {
		set_mask_bmap(bmap, fname);
	}
}

void UI_WINDOW::set_mask_bmap(int bmap, char *name)
{
	// int i;

	// init_tooltips();
	Assert(bmap >= 0);	

	if (bmap != mask_bmap_id) {
		if (mask_bmap_id >= 0){
			bm_unlock(mask_bmap_id);
		}

		mask_w = -1;
		mask_h = -1;

		mask_bmap_id = bmap;
		mask_bmap_ptr = bm_lock(mask_bmap_id, 8, BMP_AABITMAP);
		mask_data = (ubyte *) mask_bmap_ptr->data;		
		bm_get_info( bmap, &mask_w, &mask_h );
		tt_group = -1;
		/*
		for (i=0; i<Num_tooltip_groups; i++){
			if (!stricmp(Tooltip_groups[i].mask, name)){
				tt_group = i;
			}
		}
		*/
	} else {
		nprintf(("UI", "Warning: tried to switch bitmap mask to the same bitmap\n"));
	}
}

// --------------------------------------------------------------------
// UI_WINDOW::set_foreground_bmap()
//
// Specify the filename for the mask bitmap to display on the ui window as
// a background.
//
void UI_WINDOW::set_foreground_bmap(char *fname)
{
	// load in the background bitmap 
	foreground_bmap_id = bm_load(fname);
	if (foreground_bmap_id < 0) {
		Error(LOCATION,"Could not load in %s!",fname);
	}
#ifndef HARDWARE_ONLY
	palette_use_bm_palette(foreground_bmap_id);
#endif
}


void UI_WINDOW::create( int _x, int _y, int _w, int _h, int _flags )
{
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	flags = _flags;
	first_gadget = NULL;
	selected_gadget = NULL;
	tooltip_handler = NULL;  // pointer to function to handle custom tooltips
	ignore_gadgets = 0;
	use_hack_to_get_around_stupid_problem_flag = 0;

	f_id = gr_init_font("font01.vf");

	if (_x < 0)
		_x = 0;
	if (_x + _w - 1 >= gr_screen.max_w_unscaled)
		_x = gr_screen.max_w_unscaled - _w;
	if (_y < 0)
		_y = 0;
	if (_y + _h - 1 >= gr_screen.max_h_unscaled)
		_y = gr_screen.max_h_unscaled - _h;

	game_flush();
}

void UI_WINDOW::release_bitmaps()
{
	if (mask_bmap_ptr) {
		// done with the mask bitmap, so unlock it
		bm_unlock(mask_bmap_id);

		// unload the bitmaps
		if (mask_bmap_id >= 0) {
			bm_release(mask_bmap_id);
			mask_bmap_id = -1;
		}

		mask_data = NULL;
		mask_bmap_ptr = NULL;
	}

	if (foreground_bmap_id >= 0) {
		bm_release(foreground_bmap_id);
		foreground_bmap_id = -1;
	}
}

void UI_WINDOW::destroy()
{
	UI_GADGET *cur;
	int idx;

	// free up any bitmaps
	release_bitmaps();

	// destroy all gadgets
	if (first_gadget) {
		cur = first_gadget;
		do {
			cur->destroy();
			cur = cur->next;

		} while (cur != first_gadget);
	}

	// free up all xstrs
	for(idx=0; idx<MAX_UI_XSTRS; idx++){
		// free up this struct
		if(xstrs[idx] != NULL){
			if(xstrs[idx]->xstr != NULL){
				vm_free(xstrs[idx]->xstr);
			}
			vm_free(xstrs[idx]);
			xstrs[idx] = NULL;
		}
	}
}

void UI_WINDOW::draw()
{
	UI_GADGET *tmp;

	gr_reset_clip();
	gr_set_font(f_id);

	if (foreground_bmap_id >= 0) {
		gr_set_bitmap(foreground_bmap_id);
		gr_bitmap(x, y);
	}

	if (flags & WIN_FILLED)	{
		ui_draw_box_out(x, y, x+w-1, y+h-1);
	}

	if (flags & WIN_BORDER)	{
		ui_draw_frame(x-BORDER_WIDTH, y-BORDER_WIDTH, x+w+BORDER_WIDTH-1, y+h+BORDER_WIDTH-1);
	}

	if (first_gadget) {
		tmp = first_gadget;
		do	{
			if (!tmp->hidden)
				tmp->draw();

			tmp = tmp->next;

		} while (tmp != first_gadget);
	}

	if (first_gadget) {
		tmp = first_gadget;
		do	{
			if (!tmp->hidden && (tmp->kind == UI_KIND_BUTTON) && ((UI_BUTTON *) tmp)->button_down()){
				tmp->draw();
			}

			tmp = tmp->next;
		} while (tmp != first_gadget);
	}

	// draw all xstrs
	draw_xstrs();

	// draw tooltips
	draw_tooltip();
	
	if(osk_active)
	{
		draw_osk();
	}

	// convenient debug code for showing mouse coords
	if(Cmdline_mouse_coords){
		int mx, my;
		mouse_get_pos(&mx, &my);
		// mprintf(("MOUSE (%d, %d)\n", mx, my));					
		gr_set_color_fast(&Color_normal);
		gr_printf_no_resize(mx, my - 12, "%d %d", mx, my);
	}	
}

void UI_WINDOW::draw_XSTR_forced(UI_GADGET *owner, int frame)
{
	int idx;

	// lookup the xstr and draw it if necessary
	for(idx=0; idx<MAX_UI_XSTRS; idx++){
		if((xstrs[idx] != NULL) && (xstrs[idx]->assoc == owner)){
			draw_one_xstr(xstrs[idx], frame);			
		}
	}	
}

int UI_WINDOW::get_current_hotspot()
{
	int offset, pixel_val;

	if (!mask_data){
		return -1;
	}

	// check the pixel value under the mouse
	offset = ui_mouse.y * gr_screen.max_w_unscaled + ui_mouse.x;
	pixel_val = *(mask_data + offset);
	return pixel_val;
}

void UI_WINDOW::draw_tooltip()
{
	// int i;
	// tooltip_group *ptr;
	int hotspot;	
	UI_GADGET *gadget;

	if (tt_group < 0)
		return;

	// ptr = &Tooltip_groups[tt_group];
	hotspot = get_current_hotspot();

//	mprintf(("HOTSPOT: %d [%d]\n",hotspot, Framecount));

	/*
	if (hotspot != last_tooltip_hotspot) {
		last_tooltip_hotspot = hotspot;
		last_tooltip_time = timer_get_milliseconds();
		ttx = tty = -1;
		return;

	} else if (timer_get_milliseconds() - last_tooltip_time < TOOLTIP_DELAY)
		return;
	*/

	if (first_gadget) {
		gadget = first_gadget;
		do	{
			if (gadget->get_hotspot() == hotspot)
				if (gadget->hidden)  // if control is hidden, don't draw tooltip for it.
					return;
				else
					break;

			gadget = gadget->next;

		} while (gadget != first_gadget);
	}

	/*
	for (i=ptr->start; i<ptr->end; i++) {
		if (Tooltips[i].hotspot == hotspot) {
			char *str;
			int w, h;

			str = Tooltips[i].text;
			if (str[0] == '@') {
				if (!tooltip_handler)
					Error(LOCATION, "No tooltip handler for screen with mask \"%s\"", ptr->mask);

				str = (*tooltip_handler)(str);  // Let the screen handle the custom tooltips
				if (!str)
					return;
			}

			if (ttx < 0 || tty < 0) {
				gr_get_string_size(&w, &h, str);
				Assert(w < 320 && h < 100);
				ttx = ui_mouse.x - w / 2;
				tty = ui_mouse.y - h;
			}

			render_tooltip(str);
		}
	}
	*/
}

void UI_WINDOW::render_tooltip(char *str)
{
	int str_w, str_h;

	gr_get_string_size(&str_w, &str_h, str);
	Assert(str_w < gr_screen.max_w_unscaled - 4 && str_h < gr_screen.max_h_unscaled - 4);

	if (ttx < 2)
		ttx = 2;

	if (tty < 2)
		tty = 2;

	if (ttx + str_w + 2 > gr_screen.max_w_unscaled)
		ttx = gr_screen.max_w_unscaled - str_w;

	if (tty + str_h + 2 > gr_screen.max_w_unscaled)
		tty = gr_screen.max_h_unscaled - str_h;

	gr_set_color_fast(&Color_black);
	gr_rect(ttx - 1, tty - 1, str_w + 2, str_h + 1);

	gr_set_color_fast(&Color_bright_white);
	gr_string(ttx, tty, str);
}





extern int SDLtoFS2[SDLK_LAST];

static const char *osk_buffer;

const char * take_osk(int key, int modifier)
{
	if((modifier & SDL_KEYDOWN) == SDL_KEYDOWN)
	{
		key_mark(SDLtoFS2[key], 1, 0);
	} else if((modifier & SDL_KEYUP) == SDL_KEYUP)
	{
		key_mark(SDLtoFS2[key], 0, 0);
	}	
	
	return osk_buffer;
}



void UI_WINDOW::activate_OSK(const char * buf)
{
	osk_active = true;
	osk_disable = false;
	osk_buffer = buf;
}

void UI_WINDOW::deactive_OSK()
{
	osk_disable = true;
}

// key_in: If not -1, this means to use this key as input, and not call game_poll()
int UI_WINDOW::process(int key_in,int process_mouse)
{
	osk_disable = true;

	UI_GADGET *tmp;

	// only does stuff in non THREADED mode
	os_poll();

	if (process_mouse){
		ui_mouse_process();
	}

	if (key_in == -1){
		keypress = game_check_key();
	} else {
		keypress = key_in;
	}

	last_keypress = keypress;
	do_dump_check();
	if (mouse_captured_gadget && B1_RELEASED){
		mouse_captured_gadget = NULL;
	}

	// The following code was commented out by NeilK on 4/15/99 to fix a problem we were having with
	//	the UI_SLIDER2 class not receiving the process event when the mouse was dragging the scroller
	// but outside the mask region. I checked a handful of other screens and so no adverse affects
	// of this change at the time.

/*
	if (mouse_captured_gadget) {
		mouse_captured_gadget->process();  // if a control has captured the mouse, only it gets processed
		use_hack_to_get_around_stupid_problem_flag = 0;
		return last_keypress;
	}
*/
	if (!first_gadget) {
		use_hack_to_get_around_stupid_problem_flag = 0;
		return last_keypress;
	}

	check_focus_switch_keys();

	// run through all top level gadgets and process them (they are responsible for processing
	// their children, which UI_GADGET will handle if you don't override process() or if you
	// do, you call UI_GADGET::process()).
	if ( !ignore_gadgets ) {
		tmp = first_gadget;
		do	{
			if ( !tmp->check_move() )
				tmp->process();

			tmp = tmp->next;

		} while (tmp != first_gadget);
	}
	
	if(osk_active)
	{
		if(osk_disable)
		{
			osk_active = false;
			osk_buffer = NULL;
		}
		else
		{
			if(!process_mouse) ui_mouse_process();
			
			process_osk(ui_mouse.x, ui_mouse.y, (ui_mouse.b1_status & BUTTON_PRESSED) != 0, (ui_mouse.b2_status & BUTTON_PRESSED) != 0, take_osk);
		}
	}

	use_hack_to_get_around_stupid_problem_flag = 0;
	return last_keypress;
}

void UI_WINDOW::check_focus_switch_keys()
{
	return;
}

void UI_WINDOW::capture_mouse(UI_GADGET *gadget)
{
	mouse_captured_gadget = gadget;
}

void UI_WINDOW::set_ignore_gadgets(int state)
{
	ignore_gadgets = state;
}

void UI_WINDOW::add_XSTR(char *string, int _xstr_id, int _x, int _y, UI_GADGET *_assoc, int _color_type, int _font_id)
{
	int idx;
	int found = -1;
	UI_XSTR *ui_x;

	// try and find a free xstr
	for(idx=0; idx<MAX_UI_XSTRS; idx++){
		if(xstrs[idx] == NULL){
			found = idx;
			break;
		}
	}

	// if we don't have a free spot
	if(found < 0){
		Int3();			// aieee! we need to up the max # of xstrs allowed in a window.
		return;
	}

	// allocate a new struct
	xstrs[idx] = (UI_XSTR*)vm_malloc(sizeof(UI_XSTR));
	if(xstrs[idx] == NULL){
		return;
	}
	ui_x = xstrs[idx];	

	// fill in the data
	ui_x->xstr = vm_strdup(string);		
	if(ui_x->xstr == NULL){
		vm_free(ui_x);
		xstrs[idx] = NULL;
		return;
	}
	ui_x->xstr_id = _xstr_id;
	ui_x->x = _x;
	ui_x->y = _y;
	ui_x->assoc = _assoc;
	ui_x->font_id = _font_id;	
	ui_x->clr = _color_type;
	Assert((ui_x->clr >= 0) && (ui_x->clr < UI_NUM_XSTR_COLORS));
	if((ui_x->clr < 0) || (ui_x->clr >= UI_NUM_XSTR_COLORS)){
		ui_x->clr = 0;
	}	
}

void UI_WINDOW::add_XSTR(UI_XSTR *xstr)
{
	int idx;
	int found = -1;
	UI_XSTR *ui_x;

	// try and find a free xstr
	for(idx=0; idx<MAX_UI_XSTRS; idx++){
		if(xstrs[idx] == NULL){
			found = idx;
			break;
		}
	}

	// if we don't have a free spot
	if(found < 0){
		Int3();			// aieee! we need to up the max # of xstrs allowed in a window.
		return;
	}

	// allocate a new struct
	xstrs[idx] = (UI_XSTR*)vm_malloc(sizeof(UI_XSTR));
	if(xstrs[idx] == NULL){
		return;
	}
	ui_x = xstrs[idx];	

	// fill in the data
	ui_x->xstr = vm_strdup(xstr->xstr);
	if(ui_x->xstr == NULL){
		vm_free(ui_x);
		xstrs[idx] = NULL;
		return;
	}
	ui_x->xstr_id = xstr->xstr_id;
	ui_x->x = xstr->x;
	ui_x->y = xstr->y;
	ui_x->assoc = xstr->assoc;
	ui_x->font_id = xstr->font_id;	
	ui_x->clr = xstr->clr;
	Assert((ui_x->clr >= 0) && (ui_x->clr < UI_NUM_XSTR_COLORS));
	if((ui_x->clr < 0) || (ui_x->clr >= UI_NUM_XSTR_COLORS)){
		ui_x->clr = 0;
	}	
}

void UI_WINDOW::draw_one_xstr(UI_XSTR *xs, int frame)
{
	font *f_backup = NULL;		
	char str[255] = "";

	// sanity
	if((xs == NULL) || (xs->xstr == NULL)){
		return;
	}

	// if it has an associated gadet that is hidden, do nothing
	if((xs->assoc != NULL) && (xs->assoc->hidden)){
		return;
	}
	
	// maybe set the font
	if(xs->font_id >= 0){
		// backup the current font
		Assert(Current_font != NULL);
		f_backup = Current_font;

		// set the new font
		gr_set_font(xs->font_id);
	}

	// set the color
	if(xs->assoc == NULL){			
		gr_set_color_fast(&Color_normal);
	} else {
		// just buttons for now
		switch(xs->assoc->kind){
		case UI_KIND_BUTTON:					
			// override case
			if((frame != -1) && (frame < 3)){
				gr_set_color_fast(Xstr_colors[xs->clr][frame]);
			}
			// normal checking
			else {
				// if the button is pressed
				if(((UI_BUTTON*)xs->assoc)->button_down()){
					gr_set_color_fast(Xstr_colors[xs->clr][2]);
				} 
				// if the mouse is just over it
				else if(xs->assoc->is_mouse_on()){
					gr_set_color_fast(Xstr_colors[xs->clr][1]);
				} else {
					gr_set_color_fast(Xstr_colors[xs->clr][0]);
				}
				break;
			}
			break;

		// all other controls just draw the normal frame
		default :
			if((frame != -1) && (frame < 3)){
				gr_set_color_fast(Xstr_colors[xs->clr][frame]);
			} else {
				gr_set_color_fast(Xstr_colors[xs->clr][0]);
			}
			break;
		}		

		// if the gadget disabled, just draw the normal nonhighlighted frame
		if(xs->assoc->disabled()){
			gr_set_color_fast(Xstr_colors[xs->clr][0]);
		}
	}

	// print this puppy out	
	int xoffset = lcl_get_xstr_offset(xs->xstr_id, gr_screen.res);
	strncpy(str, XSTR(xs->xstr, xs->xstr_id), 254);
	if(str[0] == '&'){
		if(strlen(str) > 1){			
			gr_string((xs->x) + xoffset, xs->y, str + 1);
		}
	} else {
		gr_string((xs->x) + xoffset, xs->y, str);
	}

	// maybe restore the old font
	if(f_backup != NULL){
		Current_font = f_backup;
	}			
}

void UI_WINDOW::draw_xstrs()
{
	int idx = 0;
	
	// draw the xstrs
	do {
		draw_one_xstr(xstrs[idx], -1);		

		// next xstr
		idx++;
	} while(idx < MAX_UI_XSTRS);
}

// TEST CODE 

void UI_WINDOW::do_dump_check()
{
#if 0
	if ( keypress == KEY_SHIFTED+KEY_CTRLED+KEY_ALTED+KEY_F12 ) {
		FILE *fp;

		last_keypress = keypress = 0;

		mprintf(( "\n========== WINDOW GADGETS =========\n" ));
		mprintf(( "(Also dumped to ui.out)\n" ));

		fp = fopen( "ui.out", "wt" );
		tmp = first_gadget;
		do	{
			if ( tmp->parent == NULL ) {	
				switch ( tmp->kind ) {
				case UI_KIND_BUTTON:
					mprintf(( "UI: Button at %d,%d\n", tmp->x, tmp->y ));
					fprintf( fp, "UI: Button at %d,%d\n", tmp->x, tmp->y );
					break;
				case UI_KIND_KEYTRAP:
					mprintf(( "UI: Keytrap at %d,%d\n", tmp->x, tmp->y ));
					fprintf( fp, "UI: Keytrap at %d,%d\n", tmp->x, tmp->y );
					break;
				case UI_KIND_CHECKBOX:
					mprintf(( "UI: Checkbox at %d,%d\n", tmp->x, tmp->y ));
					fprintf( fp, "UI: Checkbox at %d,%d\n", tmp->x, tmp->y );
					break;
				case UI_KIND_RADIO:
					mprintf(( "UI: Radiobutton at %d,%d\n", tmp->x, tmp->y ));
					fprintf( fp, "UI: Radiobutton at %d,%d\n", tmp->x, tmp->y );
					break;
				case UI_KIND_SCROLLBAR:
					mprintf(( "UI: Scrollbar at %d,%d\n", tmp->x, tmp->y ));
					fprintf( fp, "UI: Scrollbar at %d,%d\n", tmp->x, tmp->y );
					break;
				case UI_KIND_LISTBOX:
					mprintf(( "UI: Listbox at %d,%d\n", tmp->x, tmp->y ));
					fprintf( fp, "UI: Listbox at %d,%d\n", tmp->x, tmp->y );
					break;
				case UI_KIND_INPUTBOX:
					mprintf(( "UI: Inputbox at %d,%d\n", tmp->x, tmp->y ));
					fprintf( fp, "UI: Inputbox at %d,%d\n", tmp->x, tmp->y );
					break;
				default:
					mprintf(( "UI: Unknown type %d at %d,%d\n", tmp->kind, tmp->x, tmp->y ));
					fprintf( fp, "UI: Unknown type %d at %d,%d\n", tmp->kind, tmp->x, tmp->y );
				}
			}
			tmp = tmp->next;
		} while( tmp != first_gadget );
		fclose(fp);
		mprintf(( "===================================\n" ));
	}
#endif
}

/*
void parse_tooltip(int n)
{
	char buf[MESSAGE_LENGTH];

	stuff_int(&Tooltips[n].hotspot);
	stuff_string(buf, F_MESSAGE, NULL);
	Tooltips[n].text = vm_strdup(buf);
}

int parse_tooltips_group(int group, int n)
{
	char buf[NAME_LENGTH];

	Assert(group < MAX_TOOLTIP_GROUPS);
	required_string("$Mask Filename:");
	stuff_string(buf, F_NAME, NULL);
	Tooltip_groups[group].mask = vm_strdup(buf);
	Tooltip_groups[group].start = n;

	while (1) {
		if (check_for_string("#") || check_for_string("$")) {
			Tooltip_groups[group].end = n;
			return n;
		}

		Assert(n < MAX_TOOLTIPS);
		parse_tooltip(n++);
	}
}

void parse_ship_tooltip(int n)
{
}

void parse_weapon_tooltip(int n)
{
}

void parse_tooltips()
{
	int n, rval;

	// open localization
	lcl_ext_open();

	if ((rval = setjmp(parse_abort)) != 0) {
		mprintf(("TABLES: Unable to parse '%s'!  Error code = %i.\n", "tooltips.tbl", rval));
		lcl_ext_close();
		return;
	}

	read_file_text("tooltips.tbl");

	n = Num_tooltip_groups = 0;
	reset_parse();

	if (optional_string("#UI"))
		while (required_string_either("#", "$")) {
			n = parse_tooltips_group(Num_tooltip_groups, n);
			Num_tooltip_groups++;
		}

	if (optional_string("#Ships"))
		while (required_string_either("#", "$")) {
			parse_ship_tooltip(Num_ship_tooltips);
			Num_ship_tooltips++;
		}

	if (optional_string("#Weapons"))
		while (required_string_either("#", "$")) {
			parse_ship_tooltip(Num_weapon_tooltips);
			Num_weapon_tooltips++;
		}

	required_string("#End");

	// close localization
	lcl_ext_close();
}

void init_tooltips()
{
	static int inited = 0;

	if (!inited) {
#ifndef DEMO
		parse_tooltips();
#endif

		inited = 1;
	}
}
*/

void ok_clicked()
{
	mprintf(( "OK Clicked!\n" ));
}

void do_help()
{
	mprintf(( "Help!\n" ));
}

