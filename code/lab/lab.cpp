/*
 * lab.cpp
 * created by WMCoolmon
 *
 * You may not sell or otherwise commercially exploit the source or things you
 * create based on the source.
 *
 */

/*
 * $Logfile: /Freespace2/code/lab/lab.cpp $
 * $Revision: 1.28.2.9 $
 * $Date: 2007-07-15 02:45:49 $
 * $Author: Goober5000 $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.28.2.8  2007/02/27 01:44:44  Goober5000
 * add two features for WCS: specifyable shield/weapon recharge rates, and removal of linked fire penalty
 *
 * Revision 1.28.2.7  2007/02/12 02:19:45  taylor
 * fix a couple of things that I missed earlier
 *
 * Revision 1.28.2.6  2007/02/11 06:19:07  Goober5000
 * invert the do-collision flag into a don't-do-collision flag, plus fixed a wee lab bug
 *
 * Revision 1.28.2.5  2006/10/27 06:35:29  taylor
 * add "Render full detail" support to the lab as well
 *
 * Revision 1.28.2.4  2006/08/27 18:12:41  taylor
 * make Species_info[] and Asteroid_info[] dynamic
 *
 * Revision 1.28.2.3  2006/08/19 04:26:32  taylor
 * add render option for no glowmaps
 * remove render option for fog (why was this even there??)
 * add tech model view for missiles with special tech models (will hopefully help spur some work towards fixing the currently broken models)
 * handle Z-buf issue that made the lab interface disappear when the transparent render option was ticked
 *
 * Revision 1.28.2.2  2006/07/08 11:27:28  taylor
 * hopefully this should get missile viewing working decently again
 *
 * Revision 1.28.2.1  2006/07/06 21:49:12  taylor
 * add a render option to make the model not spin around, easier to work with in some cases, and better screenshots
 *
 * Revision 1.28  2006/05/13 07:09:25  taylor
 * minor cleanup and a couple extra error checks
 * get rid of some wasteful math from the gr_set_proj_matrix() calls
 *
 * Revision 1.27  2006/04/20 06:32:07  Goober5000
 * proper capitalization according to Volition
 *
 * Revision 1.26  2006/01/18 16:17:33  taylor
 * use HTL wireframe view if in HTL mode
 *
 * Revision 1.25  2005/12/29 08:08:36  wmcoolmon
 * Codebase commit, most notably including objecttypes.tbl
 *
 * Revision 1.24  2005/12/04 19:12:53  wmcoolmon
 * Post-final commit
 *
 * Revision 1.22  2005/10/30 06:44:57  wmcoolmon
 * Codebase commit - nebula.tbl, scripting, new dinky explosion/shockwave stuff, moving muzzle flashes
 *
 * Revision 1.21  2005/10/17 01:54:15  wmcoolmon
 * Make weapons clip less
 *
 * Revision 1.20  2005/10/17 01:51:01  wmcoolmon
 * Weapon models now shown in lab
 *
 * Revision 1.19  2005/10/09 09:18:10  wmcoolmon
 * Added warpin/warpout speed values to the lab
 *
 * Revision 1.18  2005/10/09 00:43:08  wmcoolmon
 * Extendable modular tables (XMTs); added weapon dialogs to the Lab
 *
 * Revision 1.17  2005/09/25 07:27:33  Goober5000
 * and again
 * --Goober5000
 *
 * Revision 1.16  2005/09/25 07:26:05  Goober5000
 * try to fix the headers
 * --Goober5000
 *
 */

#include "lab/wmcgui.h"
#include "gamesequence/gamesequence.h"
#include "io/key.h"
#include "io/timer.h"
#include "freespace2/freespace.h"
#include "cmdline/cmdline.h"
#include "ship/ship.h"
#include "weapon/weapon.h"
#include "render/3d.h"
#include "lighting/lighting.h"
#include "model/model.h"
#include "missionui/missionscreencommon.h"
#include "weapon/beam.h"
#include "mission/missionparse.h"
#include "species_defs/species_defs.h"
#include "playerman/managepilot.h"

#include <vector>


// flags
#define LAB_FLAG_NORMAL				(0)		// default
#define LAB_FLAG_NO_ROTATION		(1<<0)	// don't rotate models
#define LAB_FLAG_SHOW_INSIGNIA		(1<<1)	// show ships with insignia applied
#define LAB_FLAG_SHOW_DEBRIS		(1<<2)	// render debris instead of normal LOD
#define LAB_FLAG_SUBMODEL_ROTATE	(1<<3)	// do rotation for any rotating ship subobjects
#define LAB_FLAG_LIGHTNING_ARCS		(1<<4)	// show damage lightning

// modes
#define LAB_MODE_NONE		0	// not showing anything
#define LAB_MODE_SHIP		1	// dealing with ships
#define LAB_MODE_WEAPON		2	// dealing with weapons


// variables
static GUIScreen *Lab_screen = NULL;
static Window *Lab_toolbar = NULL;
static Window *Lab_class_window = NULL;
static Window *Lab_class_toolbar = NULL;
static Window *Lab_flags_window = NULL;
static Window *Lab_render_options_window = NULL;
static Window *Lab_variables_window = NULL;
static Window *Lab_description_window = NULL;
static Text *Lab_description_text = NULL;
static TreeItem **Lab_species_nodes = NULL;

static bool Lab_in_mission = false;
static int Lab_screen_save_bitmap = -1;
static shader Lab_shader;

static int Lab_insignia_bitmap = -1;
static int Lab_insignia_index = -1;

static ubyte Lab_mode = LAB_MODE_NONE;
static int Lab_selected_index = -1;
static int Lab_last_selected_ship = -1;

static int Lab_model_num = -1;
static int Lab_model_LOD = 0;
static int Lab_model_flags = MR_LOCK_DETAIL | MR_AUTOCENTER | MR_NO_FOGGING;
static char Lab_model_filename[MAX_FILENAME_LEN];

static int Lab_bitmap_id = -1;
static char Lab_bitmap_filename[MAX_FILENAME_LEN];

static float Lab_viewer_zoom = 1.2f;
static vec3d Lab_viewer_pos = ZERO_VECTOR;
static matrix Lab_viewer_orient = IDENTITY_MATRIX;
static float Lab_viewer_rotation = 0.0f;
static int Lab_viewer_flags = LAB_MODE_NONE;

static ship_subsys *Lab_ship_subsys = NULL;
static std::vector<model_subsystem> Lab_ship_model_subsys;

static int Lab_detail_texture_save = 0;

// damage lightning stuff
static vec3d Lab_arc_pts[MAX_SHIP_ARCS][2];
static int Lab_arc_timestamp[MAX_SHIP_ARCS];
static ubyte Lab_arc_type[MAX_SHIP_ARCS];
static int Lab_arc_next_time = -1;
static bool Lab_arc_disrupted = false;

static float Lab_thrust_len = 1.0f;
static bool Lab_thrust_afterburn = false;

// Trackball_mode:
//   1  ==  rotate	(left-mouse)
//   2  ==  pan		(shift-left-mouse)
//   3  ==  zoom	(right-mouse)
static int Trackball_mode = 1;
static int Trackball_active = 0;

// functions
void labviewer_change_ship_lod(Tree *caller);
void labviewer_change_ship(Tree *caller);
void labviewer_make_desc_window(Button *caller);
void labviewer_close_weap_window(GUIObject *caller);
void labviewer_update_flags_window();


// ---------------------- General/Utility Functions ----------------------------
void labviewer_setup_subsys_rotation()
{
	if (Lab_ship_subsys != NULL) {
		vm_free(Lab_ship_subsys);
		Lab_ship_subsys = NULL;
	}

	Lab_ship_model_subsys.clear();

	// whole lot of of get-outs
	if (Lab_mode != LAB_MODE_SHIP) {
		return;
	}

	if (Lab_selected_index < 0) {
		return;
	}

	if (Lab_selected_index >= Num_ship_classes) {
		Int3();
		return;
	}

	if (Ship_info[Lab_selected_index].n_subsystems <= 0) {
		return;
	}


	uint n_subsystems = Ship_info[Lab_selected_index].n_subsystems;

	Lab_ship_subsys = (ship_subsys *) vm_malloc( n_subsystems * sizeof(ship_subsys) );
	memset( Lab_ship_subsys, 0, n_subsystems * sizeof(ship_subsys) );

	Lab_ship_model_subsys.reserve(n_subsystems);
	for (uint idx = 0; idx < n_subsystems; idx++) {
		Lab_ship_model_subsys.push_back( Ship_info[Lab_selected_index].subsystems[idx] );
	}
//	memcpy( &Lab_ship_model_subsys[0], Ship_info[Lab_selected_index].subsystems, n_subsystems * sizeof(model_subsystem) );
}

void labviewer_change_bitmap(int ship_index = -1, int weapon_index = -1)
{
	if ( (ship_index < 0) && (weapon_index < 0) ) {
		if (Lab_bitmap_id >= 0) {
			bm_release(Lab_bitmap_id);
			Lab_bitmap_id = -1;
		}

		return;
	}

	if ( !Lab_in_mission ) {
		if (ship_index >= 0) {
			// TODO:  Ship stuff!!
		} else if (weapon_index >= 0) {
			// load up the weapon bitmaps
			extern void weapon_load_bitmaps(int);
			weapon_load_bitmaps(weapon_index);
			Lab_bitmap_id = Weapon_info[weapon_index].laser_bitmap.first_frame;
		}
	}

	if (Lab_bitmap_id >= 0) {
		bm_get_filename(Lab_bitmap_id, Lab_bitmap_filename);
	} else {
		memset( Lab_bitmap_filename, 0, sizeof(Lab_bitmap_filename) );
	}

	labviewer_update_flags_window();
}

void labviewer_change_model(char *model_fname, int lod = 0, int sel_index = -1)
{
	bool change_model = true;

	if ( (model_fname != NULL) && (Lab_mode == LAB_MODE_SHIP) ) {
		if ( (Lab_selected_index >= 0) && (Lab_selected_index == sel_index) ) {
			change_model = false;
		}
	}

	Lab_selected_index = sel_index;

	if (change_model) {
		bool valid_ship = ( (Lab_mode == LAB_MODE_SHIP) && (sel_index >= 0) );

		if (Lab_model_num != -1) {
			model_page_out_textures(Lab_model_num, true);
			model_unload(Lab_model_num);
			Lab_model_num = -1;

			if (Lab_last_selected_ship >= 0) {
				ship_page_out_textures(Lab_last_selected_ship, true);
			}
		}

		labviewer_setup_subsys_rotation();

		Lab_viewer_zoom = 1.2f;

		if (valid_ship) {
			memcpy( &Lab_viewer_pos, &Ship_info[sel_index].closeup_pos, sizeof(vec3d) );
		} else {
			Lab_viewer_pos.xyz.x = Lab_viewer_pos.xyz.y = 0.0f;
		}

		// only load a new model if we are supposed to (so that we can use this function to reset states)
		if (model_fname != NULL) {
			model_subsystem *subsystems = NULL;

			if ( !Lab_ship_model_subsys.empty() ) {
				subsystems = &Lab_ship_model_subsys[0];
			}

			Lab_model_num = model_load(model_fname, (valid_ship) ? Ship_info[sel_index].n_subsystems : 0, subsystems, 0);

			if (Lab_model_num >= 0) {
				strcpy(Lab_model_filename, model_fname);
			} else {
				memset( Lab_model_filename, 0, sizeof(Lab_model_filename) );
			}

			if (valid_ship) {
				if (Lab_model_num >= 0) {
					model_page_in_textures(Lab_model_num, sel_index);
				}

				for (int idx = 0; idx < Ship_info[sel_index].n_subsystems; idx++) {
					model_set_instance_info(&Lab_ship_subsys[idx].submodel_info_1, Lab_ship_model_subsys[idx].turn_rate, 0.5f);
				}
			}
		} else {
			// clear out the model filename
			memset( Lab_model_filename, 0, sizeof(Lab_model_filename) );
		}
	}

	if (lod == 99) {
		Lab_model_LOD = 0;
		Lab_viewer_flags |= LAB_FLAG_SHOW_DEBRIS;
	} else {
		Lab_model_LOD = lod;
		Lab_viewer_flags &= ~LAB_FLAG_SHOW_DEBRIS;
	}

	if (model_fname == NULL) {
		Lab_last_selected_ship = -1;
	}

	labviewer_update_flags_window();
}

void labviewer_add_model_arcs()
{
	int i;
	float mradius = 0.0f;
	const float MAX_ARC_LENGTH_PERCENTAGE = 0.25f;

	if (Lab_mode != LAB_MODE_SHIP) {
		return;
	}


	// Kill off old sparks
	for (i = 0; i < MAX_SHIP_ARCS; i++) {
		if ( timestamp_valid(Lab_arc_timestamp[i]) && timestamp_elapsed(Lab_arc_timestamp[i]) ) {
			Lab_arc_timestamp[i] = timestamp(-1);
		}
	}


	if ( !timestamp_valid(Lab_arc_next_time) ) {
		// start the next fireball up in the next 10 seconds or so... 
		int freq;

		if (Lab_arc_disrupted) {
			freq = 150;
		} else {
			// 1% hull left
			freq = 550;
		}

		// set the next arc time
		Lab_arc_next_time = timestamp_rand(freq * 2, freq * 4);
	}

	mradius = model_get_radius(Lab_model_num);

	if ( timestamp_elapsed(Lab_arc_next_time) ) {

		Lab_arc_next_time = timestamp(-1);		// invalid, so it gets restarted next frame

		int n, n_arcs = ((rand() >> 5) % 3) + 1;		// Create 1-3 sparks

		vec3d v1, v2, v3, v4;
		submodel_get_two_random_points(Lab_model_num, -1, &v1, &v2);
		submodel_get_two_random_points(Lab_model_num, -1, &v3, &v4);

		// For large ships, cap the length to be 25% of max radius
		if (mradius > 200.0f)	{
			float max_dist = mradius * MAX_ARC_LENGTH_PERCENTAGE;
			
			vec3d tmp;
			float d;

			// Cap arc 2->1
			vm_vec_sub( &tmp, &v1, &v2 );
			d = vm_vec_mag_quick( &tmp );
			if ( d > max_dist )	{
				vm_vec_scale_add( &v1, &v2, &tmp, max_dist / d );
			}

			// Cap arc 2->3
			vm_vec_sub( &tmp, &v3, &v2 );
			d = vm_vec_mag_quick( &tmp );
			if ( d > max_dist )	{
				vm_vec_scale_add( &v3, &v2, &tmp, max_dist / d );
			}


			// Cap arc 2->4
			vm_vec_sub( &tmp, &v4, &v2 );
			d = vm_vec_mag_quick( &tmp );
			if ( d > max_dist )	{
				vm_vec_scale_add( &v4, &v2, &tmp, max_dist / d );
			}
		}
		
		n = 0;

		float factor = 1.0f + 0.0025f * mradius;
		int a = fl2i(factor * 100.0f);
		int b = fl2i(factor * 1000.0f);
		int lifetime = (myrand() % ((b) - (a) + 1)) + (a);

		// Create the arc effects
		for (i = 0; i < MAX_SHIP_ARCS; i++) {
			if ( !timestamp_valid(Lab_arc_timestamp[i]) )	{
				Lab_arc_timestamp[i] = timestamp(lifetime);	// live up to a second

				switch (n) {
					case 0:
						Lab_arc_pts[i][0] = v1;
						Lab_arc_pts[i][1] = v2;
						break;

					case 1:
						Lab_arc_pts[i][0] = v2;
						Lab_arc_pts[i][1] = v3;
						break;

					case 2:
						Lab_arc_pts[i][0] = v2;
						Lab_arc_pts[i][1] = v4;
						break;

					default:
						Int3();
				}

				// determine what kind of arc to create
				if (Lab_arc_disrupted) {
					Lab_arc_type[i] = MARC_TYPE_EMP;
				} else {
					Lab_arc_type[i] = MARC_TYPE_NORMAL;
				}

				if ( ++n == n_arcs ) {
					// Don't need to create anymore
					break;
				}
			}
		}
	}

	// maybe move arc points around
	for (i = 0; i < MAX_SHIP_ARCS; i++) {
		if ( timestamp_valid(Lab_arc_timestamp[i]) )	{
			if ( !timestamp_elapsed(Lab_arc_timestamp[i]) ) {							
				// Maybe move a vertex....  20% of the time maybe?
				int mr = myrand();

				if ( mr < (RAND_MAX / 5) ) {
					vec3d v1, v2;
					submodel_get_two_random_points(Lab_model_num, -1, &v1, &v2);

					vec3d static_one;

					if (mr % 2) {
						static_one = Lab_arc_pts[i][0];
					} else {
						static_one = Lab_arc_pts[i][1];
					}

					// For large ships, cap the length to be 25% of max radius
					if (mradius > 200.0f) {
						float max_dist = mradius * MAX_ARC_LENGTH_PERCENTAGE;
						
						vec3d tmp;
						float d;

						// Cap arc 2->1
						vm_vec_sub( &tmp, &v1, &static_one );
						d = vm_vec_mag_quick( &tmp );

						if (d > max_dist) {
							vm_vec_scale_add( &v1, &static_one, &tmp, max_dist / d );
						}
					}

					Lab_arc_pts[i][mr % 2] = v1;
				}
			}
		}
	}

	for (i = 0; i < MAX_SHIP_ARCS; i++) {
		if ( timestamp_valid(Lab_arc_timestamp[i]) ) {
			model_add_arc(Lab_model_num, -1, &Lab_arc_pts[i][0], &Lab_arc_pts[i][1], Lab_arc_type[i]);

			// maybe add lighting for the arc
			if (Detail.lighting > 2) {
				// Move arc endpoints into world coordinates	
				vec3d tmp1, tmp2;
				vm_vec_unrotate(&tmp1, &Lab_arc_pts[i][0], &Lab_viewer_orient);
				vm_vec_unrotate(&tmp2, &Lab_arc_pts[i][1], &Lab_viewer_orient);

				light_add_point( &tmp1, 10.0f, 20.0f, frand(), 1.0f, 1.0f, 1.0f, -1 );
				light_add_point( &tmp2, 10.0f, 20.0f, frand(), 1.0f, 1.0f, 1.0f, -1 );
			}
		}
	}
}

void labviewer_add_model_thrusters(ship_info *sip)
{
	static float thruster_frame = 0.0f;
	static float thruster_glow_frame = 0.0f;
	static int thruster_bitmap = -1;
	static int thruster_glow_bitmap = -1;
	static float thruster_glow_noise = 1.0f;
	
	float rate;
	int framenum;
	int secondary_glow_bitmap = -1;
	int tertiary_glow_bitmap = -1;
	generic_anim *flame_anim = NULL, *glow_anim = NULL;
	species_info *species = &Species_info[0];
	mst_info mst;


	if (Lab_mode == LAB_MODE_SHIP) {
		Assert( sip != NULL );
		species = &Species_info[sip->species];
	}

	// init thruster graphics (species stuff)
	extern void ship_init_thrusters();
	ship_init_thrusters();

	if ( Lab_thrust_afterburn && (Lab_mode != LAB_MODE_WEAPON) ) {
		flame_anim = &species->thruster_info.flames.afterburn;		// select afterburner flame
		glow_anim = &sip->thruster_glow_info.afterburn;			// select afterburner glow
		secondary_glow_bitmap = sip->thruster_secondary_glow_info.afterburn.bitmap_id;
		tertiary_glow_bitmap = sip->thruster_tertiary_glow_info.afterburn.bitmap_id;

		rate = 1.5f;		// go at 1.5x faster when afterburners on
	} else {
		flame_anim = &species->thruster_info.flames.normal;			// select normal flame
		glow_anim = &species->thruster_info.glow.normal;			// select normal glow

		if (Lab_mode == LAB_MODE_SHIP) {
			glow_anim = &sip->thruster_glow_info.normal;				// select normal glow
			secondary_glow_bitmap = sip->thruster_secondary_glow_info.normal.bitmap_id;
			tertiary_glow_bitmap = sip->thruster_tertiary_glow_info.normal.bitmap_id;
		}

		// If thrust at 0, go at half as fast, full thrust; full framerate
		// so set rate from 0.5 to 1.0, depending on thrust from 0 to 1
		// rate = 0.5f + objp->phys_info.forward_thrust / 2.0f;
		rate = 0.67f * (1.0f + Lab_thrust_len);
	}

	Assert( flFrametime > 0.0f );

	if (flame_anim->first_frame >= 0) {
		thruster_frame += flFrametime * rate;

		// Sanity checks
		if (thruster_frame < 0.0f) {
			thruster_frame = 0.0f;
		} else if (thruster_frame > 100.0f) {
			thruster_frame = 0.0f;
		}

		while (thruster_frame > flame_anim->total_time) {
			thruster_frame -= flame_anim->total_time;
		}

		framenum = fl2i( (thruster_frame * flame_anim->num_frames) / flame_anim->total_time );

		CLAMP(framenum, 0, flame_anim->num_frames-1);

		// Get the bitmap for this frame
		thruster_bitmap = flame_anim->first_frame + framenum;
	} else {
		thruster_frame = 0.0f;
		thruster_bitmap = -1;
	}

	// Do it for glow bitmaps

	if (glow_anim->first_frame >= 0) {
		thruster_glow_frame += flFrametime * rate;

		// Sanity checks
		if (thruster_glow_frame < 0.0f) {
			thruster_glow_frame = 0.0f;
		} else if (thruster_glow_frame > 100.0f) {
			thruster_glow_frame = 0.0f;
		}

		while (thruster_glow_frame > glow_anim->total_time) {
			thruster_glow_frame -= glow_anim->total_time;
		}

		framenum = fl2i( (thruster_glow_frame * glow_anim->num_frames) / glow_anim->total_time );

		CLAMP(framenum, 0, glow_anim->num_frames-1);

		// Get the bitmap for this frame
		thruster_glow_bitmap = glow_anim->first_frame;	// + framenum;
		thruster_glow_noise = Noise[framenum];
	} else {
		thruster_glow_frame = 0.0f;
		thruster_glow_bitmap = -1;
		thruster_glow_noise = 1.0f;
	}


	// ok, we've worked out the bitmaps, so now we need to set up the thrust info
	mst.length.xyz.z = Lab_thrust_len;
	mst.length.xyz.x = 0.0f;
	mst.length.xyz.y = 0.0f;

	//	Add noise to thruster geometry.
	mst.length.xyz.z *= (1.0f + frand()/5.0f - 0.1f);
	mst.length.xyz.y *= (1.0f + frand()/5.0f - 0.1f);
	mst.length.xyz.x *= (1.0f + frand()/5.0f - 0.1f);

	CLAMP(mst.length.xyz.z, -1.0f, 1.0f);
	CLAMP(mst.length.xyz.y, -1.0f, 1.0f);
	CLAMP(mst.length.xyz.x, -1.0f, 1.0f);

	mst.primary_bitmap = thruster_bitmap;
	mst.primary_glow_bitmap =thruster_glow_bitmap;
	mst.secondary_glow_bitmap = secondary_glow_bitmap;
	mst.tertiary_glow_bitmap = tertiary_glow_bitmap;

	mst.use_ab = Lab_thrust_afterburn;
	mst.glow_noise = thruster_glow_noise;
	mst.rotvel = &vmd_zero_vector;

	if (Lab_mode == LAB_MODE_SHIP) {
		mst.glow_rad_factor = sip->thruster01_glow_rad_factor;
		mst.secondary_glow_rad_factor = sip->thruster02_glow_rad_factor;
		mst.tertiary_glow_rad_factor = sip->thruster03_glow_rad_factor;
		mst.glow_length_factor = sip->thruster02_glow_len_factor;
	}

	// set, and go...
	model_set_thrust(Lab_model_num, &mst);
}

void labviewer_render_model(float frametime)
{
	int i;
	float rev_rate;
	angles rot_angles, view_angles;
	ship_info *sip = NULL;

	if ( (Lab_mode == LAB_MODE_SHIP) && (Lab_selected_index >= 0) ) {
		sip = &Ship_info[Lab_selected_index];
	}


	// get correct revolution rate
	rev_rate = REVOLUTION_RATE;

	if (sip != NULL) {
		if (sip->flags & SIF_BIG_SHIP) {
			rev_rate *= 1.7f;
		} else if (sip->flags & SIF_HUGE_SHIP) {
			rev_rate *= 3.0f;
		}
	}

	// rotate/pan/zoom the model as much as required for this frame
	if (Trackball_active) {
		int dx, dy;
		matrix mat1, mat2;

		mouse_get_delta(&dx, &dy);

		if (dx || dy) {
			// rotation mode
			if (Trackball_mode == 1) {
				vm_trackball(-dx, -dy, &mat1);
				vm_matrix_x_matrix(&mat2, &mat1, &Lab_viewer_orient);
				Lab_viewer_orient = mat2;
			}
			// pan mode
			else if (Trackball_mode == 2) {
				double scale_x = dx * (double)model_get_radius(Lab_model_num) * 0.005;
				double scale_y = dy * (double)model_get_radius(Lab_model_num) * 0.005;

				Lab_viewer_pos.xyz.x -= (float)scale_x;
				Lab_viewer_pos.xyz.y += (float)scale_y;
			}
			// zoom mode
			else if ( dy && (Trackball_mode == 3) ) {
				float scale_y = dy * 0.01f;

				Lab_viewer_zoom += scale_y;
			}
		}
	}
	// otherwise do orient/rotation calculation, if we are supposed to
	else if ( !(Lab_viewer_flags & LAB_FLAG_NO_ROTATION) ) {
		Lab_viewer_rotation += PI2 * frametime / rev_rate;

		while (Lab_viewer_rotation > PI2)
			Lab_viewer_rotation -= PI2;

		// setup stuff needed to render the ship
		view_angles.p = -0.6f;
		view_angles.b = 0.0f;
		view_angles.h = 0.0f;
		vm_angles_2_matrix(&Lab_viewer_orient, &view_angles);

		rot_angles.p = 0.0f;
		rot_angles.b = 0.0f;
		rot_angles.h = Lab_viewer_rotation;
		vm_rotate_matrix_by_angles(&Lab_viewer_orient, &rot_angles);
	}


	// render the ship
	g3_start_frame(1);

	if (sip != NULL) {
		Lab_viewer_pos.xyz.z = sip->closeup_pos.xyz.z;

		float my_zoom = sip->closeup_zoom * Lab_viewer_zoom;
		// clamp it down so that we don't get too close or too far away
		CLAMP(my_zoom, 0.08f, 1.8f);

		g3_set_view_matrix(&Lab_viewer_pos, &vmd_identity_matrix, my_zoom);
	} else {
		// find the largest radius
		polymodel *pm = model_get(Lab_model_num);
		float largest_radius = 0.0f;

		Assert( pm != NULL );

		for (i = 0; i < pm->n_models; i++) {
			if ( !pm->submodel[i].is_thruster ) {
				if (pm->submodel[i].rad > largest_radius) {
					largest_radius = pm->submodel[i].rad;
				}
			}
		}

		Lab_viewer_pos.xyz.z = -(largest_radius * 2.0f);

		CLAMP(Lab_viewer_zoom, 0.08f, 1.8f);

		g3_set_view_matrix(&Lab_viewer_pos, &vmd_identity_matrix, Lab_viewer_zoom);
	}

	if (!Cmdline_nohtl) {
		gr_set_proj_matrix(Proj_fov, gr_screen.clip_aspect, 1.0f, Max_draw_distance);
		gr_set_view_matrix(&Eye_position, &Eye_matrix);
	}

	// lighting for techroom
	light_reset();
	vec3d light_dir = vmd_zero_vector;
	light_dir.xyz.y = 1.0f;
	light_add_directional(&light_dir, 0.65f, 1.0f, 1.0f, 1.0f);
	// light_filter_reset();
	light_rotate_all();
	// lighting for techroom

	model_set_outline_color(255, 255, 255);
	model_clear_instance(Lab_model_num);
	model_set_detail_level(Lab_model_LOD);

	int flagggs = Lab_model_flags;

//	if (sip && sip->flags2 & SIF2_TRANSPARENT) {
//		flagggs |= MR_ALL_XPARENT;
//		model_set_alpha(1.0f);
//	}

	// only render the insignia when the flag is set
	if (Lab_viewer_flags & LAB_FLAG_SHOW_INSIGNIA) {
		model_set_insignia_bitmap(Lab_insignia_bitmap);
	}

	// render special if we are showing debris
	if (Lab_viewer_flags & LAB_FLAG_SHOW_DEBRIS) {
		polymodel *pm = model_get(Lab_model_num);

		for (i = 0; i < pm->num_debris_objects; i++) {
			vec3d world_point = ZERO_VECTOR;

			model_find_world_point(&world_point, &pm->submodel[pm->debris_objects[i]].offset, Lab_model_num, -1, &Lab_viewer_orient, &vmd_zero_vector);
	
			submodel_render(Lab_model_num, pm->debris_objects[i], &Lab_viewer_orient, &world_point, Lab_model_flags, -1);
		}
	}
	// render normally otherwise
	else {
		// show damage arcs if wanted
		if ( (sip != NULL) && (Lab_viewer_flags & LAB_FLAG_LIGHTNING_ARCS) ) {
			labviewer_add_model_arcs();
		}

		// ship/weapon thrusters
		if (Lab_model_flags & MR_SHOW_THRUSTERS) {
			labviewer_add_model_thrusters(sip);
		}

		// rotate submodels if wanted
		if ( (sip != NULL) && (Lab_viewer_flags & LAB_FLAG_SUBMODEL_ROTATE) ) {
			for (i = 0; i < sip->n_subsystems; i++) {
				if ( !(Lab_ship_model_subsys[i].flags & MSS_FLAG_ROTATES) ) {
					continue;
				}

				model_set_instance(Lab_model_num, Lab_ship_model_subsys[i].subobj_num, &Lab_ship_subsys[i].submodel_info_1 );

				// if we got this far, we can rotate - so choose which method to use
				if (Lab_ship_model_subsys[i].flags & MSS_FLAG_STEPPED_ROTATE) {
					submodel_stepped_rotate(&Lab_ship_model_subsys[i], &Lab_ship_subsys[i].submodel_info_1);
				} else {
					submodel_rotate(&Lab_ship_model_subsys[i], &Lab_ship_subsys[i].submodel_info_1 );
				}
			}
		}

		model_render(Lab_model_num, &Lab_viewer_orient, &vmd_zero_vector, /*Lab_model_flags*/flagggs, -1, -1);
	}

	if ( !Cmdline_nohtl ) {
		gr_end_view_matrix();
		gr_end_proj_matrix();
	}

	g3_end_frame();
}

void labviewer_render_bitmap(float frametime)
{
	static float current_frame = 0.0f;
	static float current_glow_frame = 0.0f;
	int framenum = 0;

	// if not valid then bail
	if ( (Lab_selected_index < 0) || (Lab_mode != LAB_MODE_WEAPON) || (Lab_bitmap_id < 0) ) {
		return;
	}


	weapon_info *wip = &Weapon_info[Lab_selected_index];

	if (wip->laser_bitmap.first_frame < 0) {
		return;
	}


	float rev_rate = REVOLUTION_RATE;
	angles rot_angles, view_angles;

	if (Trackball_active) {
		int dx, dy;
		matrix mat1, mat2;

		mouse_get_delta(&dx, &dy);

		if (dx || dy) {
			// rotation mode
			if (Trackball_mode == 1) {
				vm_trackball(-dx, -dy, &mat1);
				vm_matrix_x_matrix(&mat2, &mat1, &Lab_viewer_orient);
				Lab_viewer_orient = mat2;
			}
			// pan mode
			else if (Trackball_mode == 2) {
				double scale_x = dx * (double)wip->laser_length * 0.005;
				double scale_y = dy * (double)wip->laser_length * 0.005;

				Lab_viewer_pos.xyz.x -= (float)scale_x;
				Lab_viewer_pos.xyz.y += (float)scale_y;
			}
			// zoom mode
			else if ( dy && (Trackball_mode == 3) ) {
				float scale_y = dy / 100.0f;

				Lab_viewer_zoom += scale_y;
			}
		}
	}
	// otherwise do orient/rotation calculation, if we are supposed to
	else if ( !(Lab_viewer_flags & LAB_FLAG_NO_ROTATION) ) {
		Lab_viewer_rotation += PI2 * frametime / rev_rate;

		while (Lab_viewer_rotation > PI2) {
			Lab_viewer_rotation -= PI2;
		}

		// setup stuff needed to render the ship
		view_angles.p = -0.6f;
		view_angles.b = 0.0f;
		view_angles.h = 0.0f;
		vm_angles_2_matrix(&Lab_viewer_orient, &view_angles);

		rot_angles.p = 0.0f;
		rot_angles.b = 0.0f;
		rot_angles.h = Lab_viewer_rotation;
		vm_rotate_matrix_by_angles(&Lab_viewer_orient, &rot_angles);
	}

	g3_start_frame(1);

	Lab_viewer_pos.xyz.z = -(wip->laser_length * 2.0f);

	CLAMP(Lab_viewer_zoom, 0.08f, 1.8f);

	g3_set_view_matrix(&Lab_viewer_pos, &vmd_identity_matrix, Lab_viewer_zoom * 1.3f);

	if ( !Cmdline_nohtl ) {
		gr_set_proj_matrix(Proj_fov, gr_screen.clip_aspect, 1.0f, Max_draw_distance);
		gr_set_view_matrix(&Eye_position, &Eye_matrix);
	}


	// draw the primary laser bitmap
	gr_set_color_fast(&wip->laser_color_1);

	if (wip->laser_bitmap.num_frames > 1) {
		current_frame += frametime;

		// Sanity checks
		if (current_frame < 0.0f) {
			current_frame = 0.0f;
		} else if (current_frame > 100.0f) {
			current_frame = 0.0f;
		}

		while (current_frame > wip->laser_bitmap.total_time) {
			current_frame -= wip->laser_bitmap.total_time;
		}

		framenum = fl2i( (current_frame * wip->laser_bitmap.num_frames) / wip->laser_bitmap.total_time );

		CLAMP(framenum, 0, wip->laser_bitmap.num_frames-1);
	}

	vec3d headp;
	vm_vec_scale_add(&headp, &vmd_zero_vector, &Lab_viewer_orient.vec.fvec, wip->laser_length);

	gr_set_bitmap(wip->laser_bitmap.first_frame + framenum, GR_ALPHABLEND_FILTER, GR_BITBLT_MODE_NORMAL, 0.99999f);
	g3_draw_laser(&headp, wip->laser_head_radius, &vmd_zero_vector, wip->laser_tail_radius, TMAP_FLAG_TEXTURED | TMAP_FLAG_XPARENT | TMAP_HTL_3D_UNLIT);


	// now draw the laser glow bitmap, if there is one, and if we are supposed to
	if ( !(Lab_model_flags & MR_NO_GLOWMAPS) && (wip->laser_glow_bitmap.first_frame >= 0) ) {
		vec3d headp2, tailp;
		color c;

		float weapon_glow_scale_f = 2.3f;
		float weapon_glow_scale_r = 2.3f;
		float weapon_glow_scale_l = 1.5f;
		float weapon_glow_alpha = 0.85f;

    	// get the laser color
		if ( (wip->laser_color_2.red == 0) && (wip->laser_color_2.green == 0) && (wip->laser_color_2.blue == 0) ) {
			c = wip->laser_color_1;
		} else {
			// lifetime pct
			float pct = 1.0f;

			// otherwise interpolate between the colors
			gr_init_color( &c, (int)((float)wip->laser_color_1.red + (((float)wip->laser_color_2.red - (float)wip->laser_color_1.red) * pct)), 
								(int)((float)wip->laser_color_1.green + (((float)wip->laser_color_2.green - (float)wip->laser_color_1.green) * pct)), 
								(int)((float)wip->laser_color_1.blue + (((float)wip->laser_color_2.blue - (float)wip->laser_color_1.blue) * pct)) );
		}

		vm_vec_scale_add(&headp2, &vmd_zero_vector, &Lab_viewer_orient.vec.fvec, wip->laser_length * weapon_glow_scale_l);
		vm_vec_scale_add(&tailp, &vmd_zero_vector, &Lab_viewer_orient.vec.fvec, wip->laser_length * (1 -  weapon_glow_scale_l) );

		framenum = 0;

		if (wip->laser_glow_bitmap.num_frames > 1) {
			current_glow_frame += frametime;

			// Sanity checks
			if (current_glow_frame < 0.0f) {
				current_glow_frame = 0.0f;
			} else if (current_glow_frame > 100.0f) {
				current_glow_frame = 0.0f;
			}

			while (current_glow_frame > wip->laser_glow_bitmap.total_time) {
				current_glow_frame -= wip->laser_glow_bitmap.total_time;
			}

			framenum = fl2i( (current_glow_frame * wip->laser_glow_bitmap.num_frames) / wip->laser_glow_bitmap.total_time );

			CLAMP(framenum, 0, wip->laser_glow_bitmap.num_frames-1);
		}

		gr_set_bitmap(wip->laser_glow_bitmap.first_frame + framenum, GR_ALPHABLEND_FILTER, GR_BITBLT_MODE_NORMAL, weapon_glow_alpha);
		g3_draw_laser_rgb(&headp2, wip->laser_head_radius * weapon_glow_scale_f, &tailp, wip->laser_tail_radius * weapon_glow_scale_r, c.red, c.green, c.blue,  TMAP_FLAG_TEXTURED | TMAP_FLAG_XPARENT  | TMAP_FLAG_RGB | TMAP_HTL_3D_UNLIT);
	}

	// clean up and move on ...

	if ( !Cmdline_nohtl ) {
		gr_end_view_matrix();
		gr_end_proj_matrix();
	}

	g3_end_frame();
}

void labviewer_do_render(float frametime)
{
	int w, h;

	if ( (Lab_model_num < 0) && (Lab_bitmap_id < 0) ) {
		gr_get_string_size(&w, &h, "Viewer off");
		gr_set_color_fast(&Color_white);
		gr_string(gr_screen.clip_right - w, gr_screen.clip_bottom - h, "Viewer off", false);

		return;
	}

	// render our particular thing
	if (Lab_model_num >= 0) {
		labviewer_render_model(frametime);

		// print out the current pof filename, to help with... something
		if ( strlen(Lab_model_filename) ) {
			gr_get_string_size(&w, &h, Lab_model_filename);
			gr_set_color_fast(&Color_white);
			gr_string(gr_screen.clip_right - w, gr_screen.clip_bottom - h, Lab_model_filename, false);
		}
	} else if (Lab_bitmap_id >= 0) {
		labviewer_render_bitmap(frametime);

		// print out the current bitmap filename, to help with... something
		if ( strlen(Lab_bitmap_filename) ) {
			gr_get_string_size(&w, &h, Lab_bitmap_filename);
			gr_set_color_fast(&Color_white);
			gr_string(gr_screen.clip_right - w, gr_screen.clip_bottom - h, Lab_bitmap_filename, false);
		}
	}

	// print FPS at bottom left, might be helpful
	game_get_framerate();

	gr_set_color_fast(&Color_white);

	if (frametotal != 0.0f) {
		gr_printf_no_resize(gr_screen.clip_left + 2, gr_screen.clip_bottom - gr_get_font_height(), "FPS: %i", fl2i(Framerate + 0.5f));
	} else {
		gr_string(gr_screen.clip_left + 10, gr_screen.clip_bottom - gr_get_font_height(), "FPS: ?", false);
	}
}

void labviewer_exit(Button *caller)
{
	gameseq_post_event(GS_EVENT_PREVIOUS_STATE);
}

// ----------------------------  Class Window ----------------------------------
void labviewer_close_class_window(GUIObject *caller)
{
	if (Lab_class_toolbar) {
		Lab_class_toolbar->DeleteChildren();
	}

	Lab_class_window = NULL;

	Lab_mode = LAB_MODE_NONE;

	// reset any existing model/bitmap that is showing
	labviewer_change_model(NULL);
	labviewer_change_bitmap();
}

void labviewer_set_class_window(int mode)
{
	if (Lab_class_window == NULL) {
		Lab_class_window = (Window*)Lab_screen->Add(new (vm_malloc(sizeof(Window))) Window("Class Window", 50, 50));
		Lab_class_window->SetCloseFunction(labviewer_close_class_window);
	}

	if (Lab_class_toolbar == NULL) {
		Lab_class_toolbar = (Window*)Lab_screen->Add(new (vm_malloc(sizeof(Window))) Window("Class Toolbar", 0, Lab_toolbar->GetHeight(), -1, -1, WS_NOTITLEBAR | WS_NONMOVEABLE));
	}

	// clear out all existing children
	Lab_class_window->ClearContent();
	Lab_class_toolbar->ClearContent();

	// set our new title
	if (mode == LAB_MODE_SHIP) {
		Lab_class_window->SetCaption("Ship Classes");
	} else if (mode == LAB_MODE_WEAPON) {
		Lab_class_window->SetCaption("Weapon Classes");
	}

	// reset any existing model/bitmap that is showing
	labviewer_change_model(NULL);
	labviewer_change_bitmap();
}


// ------------------------------  Flags Window --------------------------------
typedef struct lab_flag {
	Checkbox *cb;
	int flag;
	bool second;
} lab_flag;

static std::vector<lab_flag> Lab_flags;

void labviewer_flags_clear()
{
	if (Lab_flags_window != NULL) {
		Lab_flags_window->DeleteChildren();
	}

	Lab_flags.clear();
}

void labviewer_flags_add(int *X, int *Y, char *flag_name, int flag, bool flags2 = false)
{
	int x = 0, y = 0;

	Assert( (Lab_flags_window != NULL) && (flag_name != NULL) );

	lab_flag new_flag;

	if (X) {
		x = *X;
	}

	if (Y) {
		y = *Y;
	}

	new_flag.cb = (Checkbox*) Lab_flags_window->AddChild(new (vm_malloc(sizeof(Checkbox))) Checkbox(flag_name, x, y));
	new_flag.flag = flag;
	new_flag.second = flags2;

	Lab_flags.push_back( new_flag );

	if (X) {
		*X += new_flag.cb->GetWidth() + 2;
	}

	if (Y) {
		*Y += new_flag.cb->GetHeight() + 1;
	}
}

void labviewer_populate_flags_window()
{
	int y;

	if (Lab_mode == LAB_MODE_NONE) {
		return;
	}

	if (Lab_flags_window == NULL) {
		return;
	}

	// clear out anything that already exists
	labviewer_flags_clear();

	y = 0;

	// ship flags ...
	if (Lab_mode == LAB_MODE_SHIP) {
		labviewer_flags_add(NULL, &y, "SUPPORT", SIF_SUPPORT);
		labviewer_flags_add(NULL, &y, "CARGO", SIF_CARGO);
		labviewer_flags_add(NULL, &y, "FIGHTER", SIF_FIGHTER);
		labviewer_flags_add(NULL, &y, "BOMBER", SIF_BOMBER);
		labviewer_flags_add(NULL, &y, "CRUISER", SIF_CRUISER);
		labviewer_flags_add(NULL, &y, "CORVETTE", SIF_CORVETTE);
		labviewer_flags_add(NULL, &y, "FREIGHTER", SIF_FREIGHTER);
		labviewer_flags_add(NULL, &y, "CAPITAL", SIF_CAPITAL);
		labviewer_flags_add(NULL, &y, "TRANSPORT", SIF_TRANSPORT);
		labviewer_flags_add(NULL, &y, "NAVBUOY", SIF_NAVBUOY);
		labviewer_flags_add(NULL, &y, "SENTRYGUN", SIF_SENTRYGUN);
		labviewer_flags_add(NULL, &y, "ESCAPEPOD", SIF_ESCAPEPOD);
		labviewer_flags_add(NULL, &y, "GAS MINER", SIF_GAS_MINER);
		labviewer_flags_add(NULL, &y, "AWACS", SIF_AWACS);
		labviewer_flags_add(NULL, &y, "STEALTH", SIF_STEALTH);
		labviewer_flags_add(NULL, &y, "SUPERCAP", SIF_SUPERCAP);
		labviewer_flags_add(NULL, &y, "KNOSSOS DEVICE", SIF_KNOSSOS_DEVICE);
		labviewer_flags_add(NULL, &y, "DRYDOCK", SIF_DRYDOCK);
		labviewer_flags_add(NULL, &y, "SHIP COPY", SIF_SHIP_COPY);
		labviewer_flags_add(NULL, &y, "BIG DAMAGE", SIF_BIG_DAMAGE);
		labviewer_flags_add(NULL, &y, "HAS AWACS", SIF_HAS_AWACS);
		labviewer_flags_add(NULL, &y, "NO COLLIDE INVISIBLE", SIF_SHIP_CLASS_DONT_COLLIDE_INVIS);
		labviewer_flags_add(NULL, &y, "DO COLLISION CHECK", SIF_NO_COLLIDE);
		labviewer_flags_add(NULL, &y, "PLAYER SHIP", SIF_PLAYER_SHIP);
		labviewer_flags_add(NULL, &y, "DEFAULT PLAYER SHIP", SIF_DEFAULT_PLAYER_SHIP);
		labviewer_flags_add(NULL, &y, "BALLISTIC PRIMARIES", SIF_BALLISTIC_PRIMARIES);
		labviewer_flags_add(NULL, &y, "FLASH", SIF2_FLASH, true);
		labviewer_flags_add(NULL, &y, "SURFACE SHIELDS", SIF2_SURFACE_SHIELDS, true);
		labviewer_flags_add(NULL, &y, "SHOW SHIP MODEL", SIF2_SHOW_SHIP_MODEL, true);
		labviewer_flags_add(NULL, &y, "IN TECH DATABASE", SIF_IN_TECH_DATABASE);
		labviewer_flags_add(NULL, &y, "IN TECH DATABASE MULTI", SIF_IN_TECH_DATABASE_M);
	}
	// weapon flags ...
	else if (Lab_mode == LAB_MODE_WEAPON) {
		labviewer_flags_add(NULL, &y, "HEAT SEEKING", WIF_HOMING_HEAT);
		labviewer_flags_add(NULL, &y, "ASPECT SEEKING", WIF_HOMING_ASPECT);
		labviewer_flags_add(NULL, &y, "ELECTRONICS", WIF_ELECTRONICS);
		labviewer_flags_add(NULL, &y, "PUNCTURE", WIF_PUNCTURE);
		labviewer_flags_add(NULL, &y, "SUPERCAP", WIF_SUPERCAP);
		labviewer_flags_add(NULL, &y, "COUNTERMEASURE", WIF_CMEASURE);
		labviewer_flags_add(NULL, &y, "BIG ONLY", WIF_BIG_ONLY);
		labviewer_flags_add(NULL, &y, "HUGE", WIF_HUGE);
		labviewer_flags_add(NULL, &y, "PLAYER ALLOWED", WIF_PLAYER_ALLOWED);
		labviewer_flags_add(NULL, &y, "PARTICLE SPEW", WIF_PARTICLE_SPEW);
		labviewer_flags_add(NULL, &y, "EMP", WIF_EMP);
		labviewer_flags_add(NULL, &y, "ENERGY SUCK", WIF_ENERGY_SUCK);
		labviewer_flags_add(NULL, &y, "SHUDDER", WIF_SHUDDER);
		labviewer_flags_add(NULL, &y, "BALLISTIC", WIF2_BALLISTIC, true);
		labviewer_flags_add(NULL, &y, "PIERCE SHIELDS", WIF2_PIERCE_SHIELDS, true);
		labviewer_flags_add(NULL, &y, "CYCLE", WIF2_CYCLE, true);
		labviewer_flags_add(NULL, &y, "NO LIGHTING", WIF2_MR_NO_LIGHTING, true);
		labviewer_flags_add(NULL, &y, "TRANSPARENT", WIF2_TRANSPARENT, true);
		labviewer_flags_add(NULL, &y, "IN TECH DATABASE", WIF_IN_TECH_DATABASE);
	}
}

void labviewer_update_flags_window()
{
	uint i;

	if ( (Lab_selected_index < 0) || (Lab_mode == LAB_MODE_NONE) ) {
		return;
	}

	// no flags? then don't bother
	if (Lab_flags.size() == 0) {
		return;
	}


	if (Lab_mode == LAB_MODE_SHIP) {
		ship_info *sip = &Ship_info[Lab_selected_index];

		for (i = 0; i < Lab_flags.size(); i++) {
			if (Lab_flags[i].second) {
				Lab_flags[i].cb->SetFlag(&sip->flags2, Lab_flags[i].flag);
			} else {
				Lab_flags[i].cb->SetFlag(&sip->flags, Lab_flags[i].flag);
			}
		}
	} else if (Lab_mode == LAB_MODE_WEAPON) {
		weapon_info *wip = &Weapon_info[Lab_selected_index];

		for (i = 0; i < Lab_flags.size(); i++) {
			if (Lab_flags[i].second) {
				Lab_flags[i].cb->SetFlag(&wip->wi_flags2, Lab_flags[i].flag);
			} else {
				Lab_flags[i].cb->SetFlag(&wip->wi_flags, Lab_flags[i].flag);
			}
		}
	}
}

void labviewer_close_flags_window(GUIObject *caller)
{
	Lab_flags_window = NULL;

	Lab_flags.clear();
}

void labviewer_make_flags_window(Button *caller)
{
	if (Lab_flags_window == NULL) {
		Lab_flags_window = (Window*) Lab_screen->Add(new (vm_malloc(sizeof(Window))) Window("Flags Window", gr_screen.max_w - 205, 200));
		Lab_flags_window->SetCloseFunction(labviewer_close_flags_window);
	}

	// set our new title
	if (Lab_mode == LAB_MODE_SHIP) {
		Lab_flags_window->SetCaption("Ship Flags");
	} else if (Lab_mode == LAB_MODE_WEAPON) {
		Lab_flags_window->SetCaption("Weapon Flags");
	}

	// populate the window with our flags (controls both ships and weapons flags)
	labviewer_populate_flags_window();

	// update content, if we need to
	labviewer_update_flags_window();
}

// -----------------------   Variables Window   --------------------------------
#define VAR_POS_LEFTWIDTH		150
#define VAR_POS_RIGHTWIDTH		100
#define VAR_POS_RIGHTX			160

#define VAR_ADD_HEADER(name) {	\
	ntp = (Text*)Lab_variables_window->AddChild(new (vm_malloc(sizeof(Text))) Text((name), (name), VAR_POS_RIGHTX/2, y + 8, VAR_POS_RIGHTWIDTH));	\
	y += ntp->GetHeight() + 10;	\
}

static std::vector<Text*> Lab_variables;

void labviewer_close_variables_window(GUIObject *caller)
{
	Lab_variables_window = NULL;

	Lab_variables.clear();
}

void labviewer_variables_clear()
{
	if (Lab_variables_window != NULL) {
		Lab_variables_window->DeleteChildren();
	}

	Lab_variables.clear();
}

void labviewer_variables_add(int *Y, char *var_name)
{
	int y = 0;
	Text *new_text;

	Assert( (Lab_variables_window != NULL) && (var_name != NULL) );

	if (Y) {
		y = *Y;
	}

	// variable
	Lab_variables_window->AddChild(new (vm_malloc(sizeof(Text))) Text((var_name), (var_name), 0, y, VAR_POS_LEFTWIDTH));
	// edit box
	new_text = (Text*)Lab_variables_window->AddChild(new (vm_malloc(sizeof(Text))) Text(std::string((var_name)) + std::string("Editbox"), "", VAR_POS_RIGHTX, y, VAR_POS_RIGHTWIDTH, 12, T_EDITTABLE));

	if (Y) {
		*Y += new_text->GetHeight() + 2;
	}

	Lab_variables.push_back( new_text );
}

void labviewer_populate_variables_window()
{
	Text *ntp;
	int y;

	if (Lab_mode == LAB_MODE_NONE) {
		return;
	}

	if (Lab_variables_window == NULL) {
		return;
	}

	// clear out anything that already exists
	labviewer_variables_clear();

	y = 0;

	// ship variables ...
	if (Lab_mode == LAB_MODE_SHIP) {
		labviewer_variables_add(&y, "Name");
		labviewer_variables_add(&y, "Species");
		labviewer_variables_add(&y, "Type");

		// physics
		VAR_ADD_HEADER("Physics");
		labviewer_variables_add(&y, "Density");
		labviewer_variables_add(&y, "Damp");
		labviewer_variables_add(&y, "Rotdamp");
		labviewer_variables_add(&y, "Max vel (x)");
		labviewer_variables_add(&y, "Max vel (y)");
		labviewer_variables_add(&y, "Max vel (z)");
		labviewer_variables_add(&y, "Warp in speed");
		labviewer_variables_add(&y, "Warp out speed");

		// other
		VAR_ADD_HEADER("Stats");
		labviewer_variables_add(&y, "Shields");
		labviewer_variables_add(&y, "Hull");
		labviewer_variables_add(&y, "Subsys repair rate");
		labviewer_variables_add(&y, "Hull repair rate");
		labviewer_variables_add(&y, "Countermeasures");
		labviewer_variables_add(&y, "HUD Icon");

		VAR_ADD_HEADER("Power");
		labviewer_variables_add(&y, "Power output");
		labviewer_variables_add(&y, "Max oclk speed");
		labviewer_variables_add(&y, "Max weapon reserve");

		VAR_ADD_HEADER("Afterburner");
		labviewer_variables_add(&y, "Fuel");
		labviewer_variables_add(&y, "Burn rate");
		labviewer_variables_add(&y, "Recharge rate");

		VAR_ADD_HEADER("Explosion");
		labviewer_variables_add(&y, "Inner radius");
		labviewer_variables_add(&y, "Outer radius");
		labviewer_variables_add(&y, "Damage");
		labviewer_variables_add(&y, "Blast");
		labviewer_variables_add(&y, "Propagates");
		labviewer_variables_add(&y, "Shockwave speed");
		labviewer_variables_add(&y, "Shockwave count");

		// techroom
		VAR_ADD_HEADER("Techroom");
		labviewer_variables_add(&y, "Closeup zoom");
		labviewer_variables_add(&y, "Closeup pos (x)");
		labviewer_variables_add(&y, "Closeup pos (y)");
		labviewer_variables_add(&y, "Closeup pos (z)");
	}
	// weapon variables ...
	else if (Lab_mode == LAB_MODE_WEAPON) {
		labviewer_variables_add(&y, "Name");
		labviewer_variables_add(&y, "Subtype");

		// physics
		VAR_ADD_HEADER("Physics");
		labviewer_variables_add(&y, "Mass");
		labviewer_variables_add(&y, "Max speed");
		labviewer_variables_add(&y, "Lifetime");
		labviewer_variables_add(&y, "Range");
		labviewer_variables_add(&y, "Min Range");
	
		VAR_ADD_HEADER("Damage");
		labviewer_variables_add(&y, "Fire wait");
		labviewer_variables_add(&y, "Damage");
		labviewer_variables_add(&y, "Armor factor");
		labviewer_variables_add(&y, "Shield factor");
		labviewer_variables_add(&y, "Subsys factor");
	
		VAR_ADD_HEADER("Armor");
		labviewer_variables_add(&y, "Damage type");
	
		VAR_ADD_HEADER("Shockwave");
		labviewer_variables_add(&y, "Speed");
	
		VAR_ADD_HEADER("Missiles");
		labviewer_variables_add(&y, "Turn time");
		labviewer_variables_add(&y, "FOV");
		labviewer_variables_add(&y, "Min locktime");
		labviewer_variables_add(&y, "Pixels/sec");
		labviewer_variables_add(&y, "Catchup pixels/sec");
		labviewer_variables_add(&y, "Catchup pixel pen.");
		labviewer_variables_add(&y, "Swarm count");
		labviewer_variables_add(&y, "Swarm wait");
	}
}

#define VAR_SET_VALUE(value) {	\
	Assert( i < Lab_variables.size() );	\
	Lab_variables[i]->SetText((value));	\
	i++;	\
}

#define VAR_SET_VALUE_SAVE(value, max_size) {	\
	Assert( i < Lab_variables.size() );	\
	Lab_variables[i]->SetText((value));	\
	if ((max_size) < 1) {	\
		Assert( (max_size) == 0 );	\
		Lab_variables[i]->SetSaveLoc(&(value), T_ST_ONENTER);	\
	} else {	\
		Lab_variables[i]->SetSaveLoc(&(value), T_ST_ONENTER, (max_size), 0);	\
	}	\
	i++;	\
}

extern int Hud_shield_filename_count;

void labviewer_update_variables_window()
{
	uint i = 0;

	if (Lab_mode == LAB_MODE_NONE) {
		return;
	}

	if (Lab_variables_window == NULL) {
		return;
	}

	if (Lab_selected_index < 0) {
		return;
	}


	// ship variables ...
	if (Lab_mode == LAB_MODE_SHIP) {
		Assert( Lab_selected_index < Num_ship_classes );
		ship_info *sip = &Ship_info[Lab_selected_index];

		VAR_SET_VALUE(sip->name);
		VAR_SET_VALUE_SAVE(sip->species, Species_info.size()-1);
		VAR_SET_VALUE_SAVE(sip->class_type, Ship_types.size()-1);

		VAR_SET_VALUE_SAVE(sip->density, 0);
		VAR_SET_VALUE_SAVE(sip->damp, 0);
		VAR_SET_VALUE_SAVE(sip->rotdamp, 0);
		VAR_SET_VALUE_SAVE(sip->max_vel.xyz.x, 0);
		VAR_SET_VALUE_SAVE(sip->max_vel.xyz.y, 0);
		VAR_SET_VALUE_SAVE(sip->max_vel.xyz.z, 0);
		VAR_SET_VALUE_SAVE(sip->warpin_speed, 0);
		VAR_SET_VALUE_SAVE(sip->warpout_speed, 0);

		VAR_SET_VALUE_SAVE(sip->max_shield_strength, 0);
		VAR_SET_VALUE_SAVE(sip->max_hull_strength, 0);
		VAR_SET_VALUE_SAVE(sip->subsys_repair_rate, 0);
		VAR_SET_VALUE_SAVE(sip->hull_repair_rate, 0);
		VAR_SET_VALUE_SAVE(sip->cmeasure_max, 0);
		VAR_SET_VALUE_SAVE(sip->shield_icon_index, Hud_shield_filename_count-1);

		VAR_SET_VALUE_SAVE(sip->power_output, 0);
		VAR_SET_VALUE_SAVE(sip->max_overclocked_speed, 0);
		VAR_SET_VALUE_SAVE(sip->max_weapon_reserve, 0);
	//	VAR_SET_VALUE_SAVE(sip->max_shield_regen_per_second, 0);
	//	VAR_SET_VALUE_SAVE(sip->max_weapon_regen_per_second, 0);

		VAR_SET_VALUE_SAVE(sip->afterburner_fuel_capacity, 0);
		VAR_SET_VALUE_SAVE(sip->afterburner_burn_rate, 0);
		VAR_SET_VALUE_SAVE(sip->afterburner_recover_rate, 0);

		VAR_SET_VALUE_SAVE(sip->shockwave.inner_rad, 0);
		VAR_SET_VALUE_SAVE(sip->shockwave.outer_rad, 0);
		VAR_SET_VALUE_SAVE(sip->shockwave.damage, 0);
		VAR_SET_VALUE_SAVE(sip->shockwave.blast, 0);
		VAR_SET_VALUE_SAVE(sip->explosion_propagates, 0);
		VAR_SET_VALUE_SAVE(sip->shockwave.speed, 0);
		VAR_SET_VALUE_SAVE(sip->shockwave_count, 0);

		VAR_SET_VALUE_SAVE(sip->closeup_zoom, 0);
		VAR_SET_VALUE_SAVE(sip->closeup_pos.xyz.x, 0);
		VAR_SET_VALUE_SAVE(sip->closeup_pos.xyz.y, 0);
		VAR_SET_VALUE_SAVE(sip->closeup_pos.xyz.z, 0);
	}
	// weapon variables ...
	else if (Lab_mode == LAB_MODE_WEAPON) {
		Assert( Lab_selected_index < Num_weapon_types );
		weapon_info *wip = &Weapon_info[Lab_selected_index];

		VAR_SET_VALUE(wip->name);
		VAR_SET_VALUE_SAVE(wip->subtype, Num_weapon_subtypes-1);

		VAR_SET_VALUE_SAVE(wip->mass, 0);
		VAR_SET_VALUE_SAVE(wip->max_speed, 0);
		VAR_SET_VALUE_SAVE(wip->lifetime, 0);
		VAR_SET_VALUE_SAVE(wip->weapon_range, 0);
		VAR_SET_VALUE_SAVE(wip->WeaponMinRange, 0);
	
		VAR_SET_VALUE_SAVE(wip->fire_wait, 0);
		VAR_SET_VALUE_SAVE(wip->damage, 0);
		VAR_SET_VALUE_SAVE(wip->armor_factor, 0);
		VAR_SET_VALUE_SAVE(wip->shield_factor, 0);
		VAR_SET_VALUE_SAVE(wip->subsystem_factor, 0);
	
		VAR_SET_VALUE_SAVE(wip->damage_type_idx, 0);
	
		VAR_SET_VALUE_SAVE(wip->shockwave.speed, 0);
	
		VAR_SET_VALUE_SAVE(wip->turn_time, 0);
		VAR_SET_VALUE_SAVE(wip->fov, 0);
		VAR_SET_VALUE_SAVE(wip->min_lock_time, 0);
		VAR_SET_VALUE_SAVE(wip->lock_pixels_per_sec, 0);
		VAR_SET_VALUE_SAVE(wip->catchup_pixels_per_sec, 0);
		VAR_SET_VALUE_SAVE(wip->catchup_pixel_penalty, 0);
		VAR_SET_VALUE_SAVE(wip->swarm_count, 0);
		VAR_SET_VALUE_SAVE(wip->SwarmWait, 0);
	}
}

void labviewer_make_variables_window(Button *caller)
{
	if (Lab_variables_window != NULL) {
		return;
	}

	Lab_variables_window = (Window*)Lab_screen->Add(new (vm_malloc(sizeof(Window))) Window("Class Variables", gr_screen.max_w - (VAR_POS_RIGHTX + VAR_POS_RIGHTWIDTH + 25), 200));

	// set our new title
	if (Lab_mode == LAB_MODE_SHIP) {
		Lab_variables_window->SetCaption("Ship Variables");
	} else if (Lab_mode == LAB_MODE_WEAPON) {
		Lab_variables_window->SetCaption("Weapon Variables");
	}

	// populate the window with our flags (controls both ships and weapons flags)
	labviewer_populate_variables_window();

	// update content, if we need to
	labviewer_update_variables_window();

	Lab_variables_window->SetCloseFunction(labviewer_close_variables_window);
}

// --------------------   Render Options Window   ------------------------------
void labviewer_change_detail_texture(Tree *caller)
{
	int slider_pos = (int)(caller->GetSelectedItem()->GetData());
	Assert( (slider_pos >= 0) && (slider_pos <= MAX_DETAIL_LEVEL) );

	Detail.hardware_textures = slider_pos;
}

void labviewer_close_render_options_window(GUIObject *caller)
{
	Lab_render_options_window = NULL;
}

#define ADD_RENDER_FLAG(text, flag, var) {	\
	cbp = (Checkbox*)Lab_render_options_window->AddChild(new (vm_malloc(sizeof(Checkbox))) Checkbox((text), 2, y));	\
	cbp->SetFlag(&(flag), (var));	\
	y += cbp->GetHeight() + 2;	\
}

#define ADD_RENDER_BOOL(text, flag) {	\
	cbp = (Checkbox*)Lab_render_options_window->AddChild(new (vm_malloc(sizeof(Checkbox))) Checkbox((text), 2, y));	\
	cbp->SetBool(&(flag));	\
	y += cbp->GetHeight() + 1;	\
}

extern int Use_GLSL;

void labviewer_make_render_options_window(Button *caller)
{
	Checkbox *cbp;
	Tree *cmp;
	TreeItem *ctip;
	int y = 0;

	if (Lab_render_options_window != NULL) {
		return;
	}

	Lab_render_options_window = (Window*)Lab_screen->Add(new (vm_malloc(sizeof(Window))) Window("Render Options", gr_screen.max_w - 300, 200));
	Assert( Lab_render_options_window != NULL );

	// add all of the flags that we want/need...

	// viewer flags
	ADD_RENDER_FLAG("Disable Model Rotation", Lab_viewer_flags, LAB_FLAG_NO_ROTATION);
	ADD_RENDER_FLAG("Show Insignia", Lab_viewer_flags, LAB_FLAG_SHOW_INSIGNIA);
	ADD_RENDER_FLAG("Show Damage Lightning", Lab_viewer_flags, LAB_FLAG_LIGHTNING_ARCS);
	ADD_RENDER_FLAG("Rotate Subsystems", Lab_viewer_flags, LAB_FLAG_SUBMODEL_ROTATE);
	if (Use_GLSL) {
		ADD_RENDER_BOOL("Fixed Render Pipeline", GLSL_override);
	}
	// map related flags
	ADD_RENDER_BOOL("No Diffuse Map", Basemap_override);
	if (Cmdline_glow) {
		ADD_RENDER_FLAG("No Glow Map", Lab_model_flags, MR_NO_GLOWMAPS);
	}
	if (Cmdline_spec) {
		ADD_RENDER_BOOL("No Specular Map", Specmap_override);
	}
	if (Cmdline_env) {
		ADD_RENDER_BOOL("No Environment Map", Envmap_override);
	}
	if (Cmdline_normal) {
		ADD_RENDER_BOOL("No Normal Map", Normalmap_override);
	}
	if (Cmdline_height) {
		ADD_RENDER_BOOL("No Height Map", Heightmap_override);
	}
	ADD_RENDER_BOOL("No Glow Points", Glowpoint_override);
	// model flags
	ADD_RENDER_FLAG("Wireframe", Lab_model_flags, ((Cmdline_nohtl) ? MR_SHOW_OUTLINE : MR_SHOW_OUTLINE_HTL) | MR_NO_POLYS);
	ADD_RENDER_FLAG("Transparent", Lab_model_flags, MR_ALL_XPARENT);
	ADD_RENDER_FLAG("No Lighting", Lab_model_flags, MR_NO_LIGHTING);
	ADD_RENDER_FLAG("No Z-Buffer", Lab_model_flags, MR_NO_ZBUFFER);
	ADD_RENDER_FLAG("No Culling", Lab_model_flags, MR_NO_CULL);
	ADD_RENDER_FLAG("Show Full Detail", Lab_model_flags, MR_FULL_DETAIL);
	ADD_RENDER_FLAG("Show Pivots", Lab_model_flags, MR_SHOW_PIVOTS);
	ADD_RENDER_FLAG("Show Paths", Lab_model_flags, MR_SHOW_PATHS);
	ADD_RENDER_FLAG("Show Bay Paths", Lab_model_flags, MR_BAY_PATHS);
	ADD_RENDER_FLAG("Show Radius", Lab_model_flags, MR_SHOW_RADIUS);
	ADD_RENDER_FLAG("Show Shields", Lab_model_flags, MR_SHOW_SHIELDS);
	ADD_RENDER_FLAG("Show Thrusters", Lab_model_flags, MR_SHOW_THRUSTERS);


	// start tree
	cmp = (Tree*)Lab_render_options_window->AddChild(new (vm_malloc(sizeof(Tree))) Tree("Detail Options Tree", 0, y + 2, NULL, Lab_render_options_window->GetWidth()));

	// 3d hardware texture slider options
	ctip = cmp->AddItem(NULL, "3D Hardware Textures", 0, false);

	cmp->AddItem(ctip, "Minimum", 0, false, labviewer_change_detail_texture);
	cmp->AddItem(ctip, "Low", 1, false, labviewer_change_detail_texture);
	cmp->AddItem(ctip, "Medium", 2, false, labviewer_change_detail_texture);
	cmp->AddItem(ctip, "High", 3, false, labviewer_change_detail_texture);
	cmp->AddItem(ctip, "Maximum", 4, false, labviewer_change_detail_texture);

	// set close function
	Lab_render_options_window->SetCloseFunction(labviewer_close_render_options_window);
}

// -------------------------  Description Window  ------------------------------
void labviewer_close_desc_window(GUIObject *caller)
{
	Lab_description_text = NULL;
	Lab_description_window = NULL;
}

void labviewer_update_desc_window()
{
	if ( (Lab_description_window == NULL) || (Lab_description_text == NULL) ) {
		return;
	}

	if (Lab_selected_index != -1) {
		if (Lab_mode == LAB_MODE_SHIP) {
			Lab_description_window->SetCaption(Ship_info[Lab_selected_index].name);

			if (Ship_info[Lab_selected_index].tech_desc != NULL) {
				Lab_description_text->SetText(Ship_info[Lab_selected_index].tech_desc);
			} else {
				Lab_description_text->SetText("No description available.");
			}
		} else if (Lab_mode == LAB_MODE_WEAPON) {
			Lab_description_window->SetCaption(Weapon_info[Lab_selected_index].name);

			if (Weapon_info[Lab_selected_index].tech_desc != NULL) {
				Lab_description_text->SetText(Weapon_info[Lab_selected_index].tech_desc);
			} else {
				Lab_description_text->SetText("No description available.");
			}
		}
	}
}

void labviewer_make_desc_window(Button *caller)
{
	if (Lab_description_window != NULL) {
		return;
	}

	Lab_description_window = (Window*)Lab_screen->Add(new (vm_malloc(sizeof(Window))) Window("Description", gr_screen.max_w - gr_screen.max_w/3 - 50,
														gr_screen.max_h - gr_screen.max_h/6 - 50, gr_screen.max_w/3,
														gr_screen.max_h/6));
	Lab_description_text = (Text*)Lab_description_window->AddChild(new (vm_malloc(sizeof(Text))) Text("Description Text", "No ship selected.", 0, 0));

	labviewer_update_desc_window();

	Lab_description_window->SetCloseFunction(labviewer_close_desc_window);
}

// ------------------------   Ships Window   -----------------------------------
void labviewer_make_ship_window(Button *caller)
{
	GUIObject *cbp;
	TreeItem *ctip, *stip;
	int x, idx;

	if (Lab_mode == LAB_MODE_SHIP) {
		return;
	}


	// switch the class window to ship mode
	labviewer_set_class_window(LAB_MODE_SHIP);

	if ( (Lab_class_window == NULL) || (Lab_class_toolbar == NULL) ) {
		Int3();
		Lab_mode = LAB_MODE_SHIP;
		return;
	}


	// populate the class toolbar
	x = 0;
	cbp = Lab_class_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Class Description", x, 0, labviewer_make_desc_window));

	x += cbp->GetWidth() + 10;
	cbp = Lab_class_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Class Options", x, 0, labviewer_make_flags_window));

	x += cbp->GetWidth() + 10;
	cbp = Lab_class_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Class Variables", x, 0, labviewer_make_variables_window));


	// populate ship class window
	Tree *cmp = (Tree*)Lab_class_window->AddChild(new (vm_malloc(sizeof(Tree))) Tree("Ship Tree", 0, 0));

	if (Lab_species_nodes != NULL) {
		for (idx = 0; idx < (int)Species_info.size(); idx++) {
			Lab_species_nodes[idx]->ClearAllItems();
		}

		vm_free(Lab_species_nodes);
		Lab_species_nodes = NULL;
	}

	Lab_species_nodes = (TreeItem **) vm_malloc(sizeof(TreeItem*)*(Species_info.size()+1));

	// Add species nodes
	for (idx = 0; idx < (int)Species_info.size(); idx++) {
		Lab_species_nodes[idx] = cmp->AddItem(NULL, Species_info[idx].species_name, 0, false);
	}

	// Just in case. I don't actually think this is possible though.
	Lab_species_nodes[Species_info.size()] = cmp->AddItem(NULL, "Other", 0, false);

	// Now add the ships
	std::string lod_name;
	char buf[33];

	for (idx = 0; idx < Num_ship_classes; idx++) {
		if ( (Ship_info[idx].species >= 0) && (Ship_info[idx].species < (int)Species_info.size()) ) {
			stip = Lab_species_nodes[Ship_info[idx].species];
		} else {
			stip = Lab_species_nodes[Species_info.size()];
		}

		ctip = cmp->AddItem(stip, Ship_info[idx].name, idx, false, labviewer_change_ship);

		if ( !Lab_in_mission ) {
			for (int j = 0; j < Ship_info[idx].num_detail_levels; j++) {
				sprintf(buf, "%d", j);
				lod_name = "LOD ";
				lod_name += buf;

				cmp->AddItem(ctip, lod_name, j, false, labviewer_change_ship_lod);
			}

			cmp->AddItem(ctip, "Debris", 99, false, labviewer_change_ship_lod);
		}
	}

	// if any nodes are empty, just add a single "<none>" entry so we know that species doesn't have anything yet
	// No the <= is not a mistake :)
	for (idx = 0; idx < (int)Species_info.size(); idx++) {
		if ( !Lab_species_nodes[idx]->HasChildren() ) {
			cmp->AddItem(Lab_species_nodes[idx], "<none>", 0, false, NULL);
		}
	}

	// if the "Other" entry doesn't contain anything then just delete it
	if ( !Lab_species_nodes[Species_info.size()]->HasChildren() ) {
		Lab_species_nodes[Species_info.size()]->~TreeItem();
		vm_free(Lab_species_nodes[Species_info.size()]);
	}


	// and... we're done!
	Lab_mode = LAB_MODE_SHIP;

	// populate the flags window, if it exists
	// (NOTE: must be done *after* Lab_mode is set properly)
	if (Lab_flags_window != NULL) {
		Lab_flags_window->SetCaption("Ship Flags");

		labviewer_populate_flags_window();
		labviewer_update_flags_window();
	}

	if (Lab_description_window != NULL) {
		labviewer_update_desc_window();
	}

	if (Lab_variables_window != NULL) {
		Lab_variables_window->SetCaption("Ship Variables");

		labviewer_populate_variables_window();
		labviewer_update_variables_window();
	}
}

void labviewer_change_ship_lod(Tree* caller)
{
	int ship_index = (int)(caller->GetSelectedItem()->GetParentItem()->GetData());
	Assert( ship_index >= 0 );

	Lab_last_selected_ship = Lab_selected_index;

	labviewer_change_model(Ship_info[ship_index].pof_file, caller->GetSelectedItem()->GetData(), ship_index);

	labviewer_update_desc_window();
	labviewer_update_flags_window();
	labviewer_update_variables_window();

	for (int i = 0; i < MAX_SHIP_ARCS; i++) {
		Lab_arc_timestamp[i] = -1;
		Lab_arc_type[i] = MARC_TYPE_NORMAL;
		Lab_arc_next_time = -1;
	}
}

void labviewer_change_ship(Tree *caller)
{
	// this is really only for when we are in a mission
	if ( !Lab_in_mission ) {
		return;
	}
	
	Lab_selected_index = (int)(caller->GetSelectedItem()->GetData());

	labviewer_update_desc_window();
	labviewer_update_flags_window();
	labviewer_update_variables_window();
}

// ---------------------------  Weapons Window  --------------------------------
void labviewer_show_tech_model(Tree *caller)
{
	int weap_index = (int)(caller->GetSelectedItem()->GetParentItem()->GetData());
	Assert( weap_index >= 0 );

	labviewer_change_model(Weapon_info[weap_index].tech_model, caller->GetSelectedItem()->GetData(), weap_index);
}

void labviewer_change_weapon(Tree *caller)
{
	int weap_index = (int)(caller->GetSelectedItem()->GetData());
	Assert( weap_index >= 0 );

	if ( !(Weapon_info[weap_index].wi_flags & WIF_BEAM) ) {
		switch (Weapon_info[weap_index].render_type) {
			case WRT_POF:
				labviewer_change_bitmap();
				labviewer_change_model(Weapon_info[weap_index].pofbitmap_name, 0, weap_index);
				break;

			case WRT_LASER:
				labviewer_change_model(NULL);
				labviewer_change_bitmap(-1, weap_index);
				break;

			default:
				labviewer_change_model(NULL);
				labviewer_change_bitmap();
				break;
		}
	}

	Lab_selected_index = weap_index;

	labviewer_update_desc_window();
	labviewer_update_flags_window();
	labviewer_update_variables_window();
}

// weapon window create function
void labviewer_make_weap_window(Button* caller)
{
	GUIObject *cbp;
	TreeItem *cwip, *stip;
	int x;

	if (Lab_mode == LAB_MODE_WEAPON) {
		return;
	}


	// switch the class window to weapon mode
	labviewer_set_class_window(LAB_MODE_WEAPON);

	if ( (Lab_class_window == NULL) || (Lab_class_toolbar == NULL) ) {
		Int3();
		Lab_mode = LAB_MODE_WEAPON;
		return;
	}


	// populate the weapons toolbar
	x = 0;
	cbp = Lab_class_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Class Description", x, 0, labviewer_make_desc_window));

	x += cbp->GetWidth() + 10;
	cbp = Lab_class_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Class Options", x, 0, labviewer_make_flags_window));

	x += cbp->GetWidth() + 10;
	cbp = Lab_class_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Class Variables", x, 0, labviewer_make_variables_window));


	// populate the weapons window
	Tree *cmp = (Tree*)Lab_class_window->AddChild(new (vm_malloc(sizeof(Tree))) Tree("Weapon Tree", 0, 0));
	
	// Unfortunately these are hardcoded
	TreeItem **type_nodes = (TreeItem**)vm_malloc(sizeof(TreeItem*)*Num_weapon_subtypes);
	int i;

	// Add type nodes
	for (i = 0; i < Num_weapon_subtypes; i++) {
		type_nodes[i] = cmp->AddItem(NULL, Weapon_subtype_names[i], 0, false);
	}

	// Now add the weapons
	for (i = 0; i < Num_weapon_types; i++) {
		if (Weapon_info[i].subtype == WP_UNUSED) {
			continue;
		} else if (Weapon_info[i].subtype >= Num_weapon_subtypes) {
			Warning(LOCATION, "Invalid weapon subtype found on weapon %s", Weapon_info[i].name);
			continue;
		}
		
		if (Weapon_info[i].wi_flags & WIF_BEAM) {
			stip = type_nodes[WP_BEAM];
		} else {
			stip = type_nodes[Weapon_info[i].subtype];
		}

		cwip = cmp->AddItem(stip, Weapon_info[i].name, i, false, labviewer_change_weapon);

		if ( strlen(Weapon_info[i].tech_model) > 0 ) {
			cmp->AddItem(cwip, "Tech Model", 0, false, labviewer_show_tech_model);
		}
	}

	// Get rid of any empty nodes
	for (i = 0; i < Num_weapon_subtypes; i++) {
		if ( !type_nodes[i]->HasChildren() ) {
			type_nodes[i]->~TreeItem();
			vm_free(type_nodes[i]);
		}
	}
	
	vm_free(type_nodes);

	Lab_mode = LAB_MODE_WEAPON;

	// populate the flags window, if it exists
	// (NOTE: must be done *after* Lab_mode is set properly)
	if (Lab_flags_window != NULL) {
		Lab_flags_window->SetCaption("Weapon Flags");

		labviewer_populate_flags_window();
		labviewer_update_flags_window();
	}

	if (Lab_description_window != NULL) {
		labviewer_update_desc_window();
	}

	if (Lab_variables_window != NULL) {
		Lab_variables_window->SetCaption("Weapon Variables");

		labviewer_populate_variables_window();
		labviewer_update_variables_window();
	}
}



// ----------------------------- Lab functions ---------------------------------

void lab_init()
{
	GUIObject *cbp;
	int x;

	beam_pause_sounds();

	if (gameseq_get_pushed_state() == GS_STATE_GAME_PLAY) {
		Lab_in_mission = true;
	} else {
		Lab_in_mission = false;
	}

	if (Lab_in_mission) {
		Lab_screen_save_bitmap = gr_save_screen();
		gr_create_shader(&Lab_shader, 0, 0, 0, 127);
	} else {
		gr_set_clear_color(0, 0, 0);
	}


	//We start by creating the screen/toolbar
	Lab_screen = GUI_system.PushScreen(new (vm_malloc(sizeof(GUIScreen))) GUIScreen("Lab"));

	Lab_toolbar = (Window*)Lab_screen->Add(new (vm_malloc(sizeof(Window))) Window("Toolbar", 0, 0, -1, -1, WS_NOTITLEBAR | WS_NONMOVEABLE));

	// start filling the main toolbar
	x = 0;
	cbp = Lab_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Ships", x, 0, labviewer_make_ship_window));

	x += cbp->GetWidth() + 10;
	cbp = Lab_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Weapons", x, 0, labviewer_make_weap_window));

	if ( !Lab_in_mission ) {
		x += cbp->GetWidth() + 10;
		cbp = Lab_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Render Options", x, 0, labviewer_make_render_options_window));
	}

	x += cbp->GetWidth() + 20;
	cbp = Lab_toolbar->AddChild(new (vm_malloc(sizeof(Button))) Button("Exit", x, 0, labviewer_exit));


	// reset some defaults, just to be sure
	Lab_viewer_zoom = 1.2f;
	Lab_viewer_pos.xyz.x = Lab_viewer_pos.xyz.y = 0.0f;
	Lab_mode = LAB_MODE_NONE;
	Lab_thrust_len = 1.0f;
	Lab_thrust_afterburn = false;
	Lab_arc_next_time = -1;
	Lab_arc_disrupted = false;

	// save detail options
	Lab_detail_texture_save = Detail.hardware_textures;

	if ( !Lab_in_mission ) {
		// load up the list of insignia that we might use on the ships
		pilot_load_squad_pic_list();

		// the default insignia bitmap
		Lab_insignia_index = 0;
		Assert( (Lab_insignia_index < Num_pilot_squad_images) );

		Lab_insignia_bitmap = bm_load_duplicate(Pilot_squad_image_names[Lab_insignia_index]);
	}
}

#include "controlconfig/controlsconfig.h"
void lab_do_frame(float frametime)
{
	gr_reset_clip();
	gr_clear();

	if (Lab_in_mission) {
		gr_restore_screen(Lab_screen_save_bitmap);
		gr_set_shader(&Lab_shader);
		gr_shade(0, 0, gr_screen.max_w, gr_screen.max_h, false);
	} else {
		labviewer_do_render(frametime);
	}


	bool test1 = ( GUI_system.OnFrame(frametime, !(Trackball_active) ? true : false, false) == GSOF_NOTHINGPRESSED );

	if (test1) {
		int key = GUI_system.GetKeyPressed();
		int status = GUI_system.GetStatus();

		// set trackball modes
		if (status & GST_MOUSE_LEFT_BUTTON) {
			Trackball_active = 1;
			Trackball_mode = 1;	// rotate

			if ( key_get_shift_status() & KEY_SHIFTED ) {
				Trackball_mode = 2;	// pan
			}
		} else if (status & GST_MOUSE_RIGHT_BUTTON) {
			Trackball_active = 1;
			Trackball_mode = 3;	// Zoom
		} else if ( !mouse_down(MOUSE_LEFT_BUTTON | MOUSE_RIGHT_BUTTON) ) {
			// reset trackball modes
			Trackball_active = 0;
			Trackball_mode = 0;
		}

		// handle any key presses
		switch (key) {
			// switch between the current insignia bitmap to render with
			case KEY_DIVIDE: {
				if ( !(Lab_viewer_flags & LAB_FLAG_SHOW_INSIGNIA) ) {
					break;
				}

				Lab_insignia_index = (Lab_insignia_index+1) % Num_pilot_squad_images;
				Assert( (Lab_insignia_index >= 0) && (Lab_insignia_index < Num_pilot_squad_images) );

				if (Lab_insignia_bitmap >= 0) {
					bm_release(Lab_insignia_bitmap);
					Lab_insignia_bitmap = -1;
				}

				Lab_insignia_bitmap = bm_load_duplicate(Pilot_squad_image_names[Lab_insignia_index]);

				break;
			}

			// change between damage lightning effects
			case KEY_L:
				Lab_arc_disrupted = !Lab_arc_disrupted;
				Lab_arc_next_time = -1;
				break;

			// bail...
			case KEY_ESC:
				labviewer_exit(NULL);
				break;

			default: {
				// check for game-specific controls
				if (Lab_mode == LAB_MODE_SHIP) {
					if ( check_control(PLUS_5_PERCENT_THROTTLE, key) )
						Lab_thrust_len += 0.05f;
					else if ( check_control(MINUS_5_PERCENT_THROTTLE, key) )
						Lab_thrust_len -= 0.05f;

					CLAMP( Lab_thrust_len, 0.0f, 1.0f );

					if ( check_control(AFTERBURNER, key) )
						Lab_thrust_afterburn = !Lab_thrust_afterburn;
				}

				break;
			}
		}
	}

	gr_flip();
}

void lab_close()
{
	int i;

	Lab_toolbar = NULL;
	Lab_class_toolbar = NULL;
	Lab_class_window = NULL;
	Lab_flags_window = NULL;
	Lab_render_options_window = NULL;

	Lab_screen->~GUIScreen();
	vm_free(Lab_screen);

	Lab_screen = NULL;


	if (Lab_species_nodes != NULL) {
		for (i = 0; i < (int)Species_info.size(); i++) {
			Lab_species_nodes[i]->~TreeItem();
			vm_free(Lab_species_nodes[i]);
		}

		vm_free(Lab_species_nodes);
		Lab_species_nodes = NULL;
	}

	if (Lab_insignia_bitmap >= 0) {
		bm_release(Lab_insignia_bitmap);
		Lab_insignia_bitmap = -1;
	}

	Lab_insignia_index = -1;

	if (Lab_model_num != -1) {
		model_page_out_textures(Lab_model_num, true);
		model_unload(Lab_model_num);
		Lab_model_num = -1;
	}

	if (Lab_screen_save_bitmap != 1) {
		gr_free_screen(Lab_screen_save_bitmap);
		Lab_screen_save_bitmap = -1;
	}

	Lab_ship_model_subsys.clear();

	if (Lab_ship_subsys != NULL) {
		vm_free(Lab_ship_subsys);
		Lab_ship_subsys = NULL;
	}

	memset( Lab_model_filename, 0, sizeof(Lab_model_filename) );

	Basemap_override = false;
	Envmap_override = false;
	Specmap_override = false;
	Normalmap_override = false;
	Heightmap_override = false;
	Glowpoint_override = false;

	// reset detail levels to default
	Detail.hardware_textures = Lab_detail_texture_save;

	beam_unpause_sounds();
	//audiostream_unpause_all();
	game_flush();
}
