/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/Asteroid/Asteroid.cpp $
 * $Revision: 2.35.2.12 $
 * $Date: 2007-12-28 02:10:34 $
 * $Author: Backslash $
 *
 * C module for asteroid code
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.35.2.11  2007/09/02 02:07:38  Goober5000
 * added fixes for #1415 and #1483, made sure every read_file_text had a corresponding setjmp, and sync'd the parse error messages between HEAD and stable
 *
 * Revision 2.35.2.10  2007/05/28 18:27:32  wmcoolmon
 * Added armor support for asteroid, debris, ship, and beam damage
 *
 * Revision 2.35.2.9  2007/02/20 04:19:09  Goober5000
 * the great big duplicate model removal commit
 *
 * Revision 2.35.2.8  2007/02/11 09:35:11  taylor
 * add VALID_FNAME() macro and put it around a few places (more to come)
 * clean out some old variables
 * move CLAMP() macro from opengl header to global header
 * update COUNT_ESTIMATE to match new bmpman changes
 *
 * Revision 2.35.2.7  2006/12/07 17:53:08  taylor
 * a little clean up for bits of code that isn't used
 *
 * Revision 2.35.2.6  2006/11/15 00:36:08  taylor
 * various bits of little cleanup
 * get rid of some more compiler warnings
 *
 * Revision 2.35.2.5  2006/09/11 01:15:03  taylor
 * fixes for stuff_string() bounds checking
 *
 * Revision 2.35.2.4  2006/08/27 18:12:38  taylor
 * make Species_info[] and Asteroid_info[] dynamic
 *
 * Revision 2.35.2.3  2006/08/19 04:38:46  taylor
 * maybe optimize the (PI/2), (PI*2) and (RAND_MAX/2) stuff a little bit
 *
 * Revision 2.35.2.2  2006/07/06 04:06:00  Goober5000
 * 1) complete (almost) changeover to reorganized texture mapping system
 * 2) finally fix texture animation; textures now animate at the correct speed
 * --Goober5000
 *
 * Revision 2.35.2.1  2006/07/04 07:42:09  Goober5000
 * --in preparation for fixing an annoying animated texture bug, reorganize the various texture structs and glow point structs and clarify several parts of the texture code :P
 * --this breaks animated glow maps, and animated regular maps still aren't fixed, but these will be remedied shortly
 * --Goober5000
 *
 * Revision 2.35  2006/05/13 07:02:27  taylor
 * add lazy (ie, faster) checking of asteroid collisions for time-to-impact
 *
 * Revision 2.34  2006/01/26 03:23:29  Goober5000
 * pare down the pragmas some more
 * --Goober5000
 *
 * Revision 2.33  2006/01/13 03:30:59  Goober5000
 * übercommit of custom IFF stuff :)
 *
 * Revision 2.32  2005/11/21 02:43:30  Goober5000
 * change from "setting" to "profile"; this way makes more sense
 * --Goober5000
 *
 * Revision 2.31  2005/11/21 00:46:05  Goober5000
 * add ai_settings.tbl
 * --Goober5000
 *
 * Revision 2.30  2005/10/11 12:24:04  taylor
 * little cleaner since the two sets weren't needed
 *
 * Revision 2.29  2005/10/10 17:21:03  taylor
 * remove NO_NETWORK
 *
 * Revision 2.28  2005/10/08 15:37:26  taylor
 * fix several asteroid problems, little code duplication but this actually works and is less error prone than the previous method
 *
 * Revision 2.27  2005/09/25 22:29:35  Kazan
 * my bad
 *
 * Revision 2.26  2005/09/25 22:23:39  Kazan
 * correct an error
 *
 * Revision 2.25  2005/09/25 20:55:28  Goober5000
 * oh noes!
 * --Goober5000
 *
 * Revision 2.24  2005/09/25 20:31:42  Goober5000
 * okay; everything should be good to go
 * --Goober5000
 *
 * Revision 2.23  2005/09/25 08:25:14  Goober5000
 * Okay, everything should now work again. :p Still have to do a little more with the asteroids.
 * --Goober5000
 *
 * Revision 2.20  2005/07/22 10:18:36  Goober5000
 * CVS header tweaks
 * --Goober5000
 *
 * Revision 2.19  2005/07/13 02:50:48  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.18  2005/07/13 02:01:28  Goober5000
 * fixed a bunch of "issues" caused by me with the species stuff
 * --Goober5000
 *
 * Revision 2.17  2005/07/13 00:44:20  Goober5000
 * improved species support and removed need for #define
 * --Goober5000
 *
 * Revision 2.16  2005/06/01 09:34:59  taylor
 * newline fix
 *
 * Revision 2.15  2005/04/28 05:29:28  wmcoolmon
 * Removed FS2_DEMO defines that looked like they wouldn't cause the universe to collapse
 *
 * Revision 2.14  2005/04/28 01:32:57  wmcoolmon
 * Switch stuff_string_white to stuff_string (I don't see why _white was used in the first place)
 *
 * Revision 2.13  2005/04/15 11:41:28  taylor
 * stupid <expletive-delete> terminal, I <expletive-deleted> <expletive-deleted>!!!
 *
 * Revision 2.12  2005/04/15 11:36:55  taylor
 * new GCC = new warning messages, yippeeee!!
 *
 * Revision 2.11  2005/04/05 05:53:14  taylor
 * s/vector/vec3d/g, better support for different compilers (Jens Granseuer)
 *
 * Revision 2.10  2005/03/02 21:24:43  taylor
 * more network/inferno goodness for Windows, takes care of a few warnings too
 *
 * Revision 2.9  2004/07/26 20:47:23  Kazan
 * remove MCD complete
 *
 * Revision 2.8  2004/07/12 16:32:41  Kazan
 * MCD - define _MCD_CHECK to use memory tracking
 *
 * Revision 2.7  2004/03/31 05:42:25  Goober5000
 * got rid of all those nasty warnings from xlocale and so forth; also added comments
 * to indicate which warnings were being disabled
 * --Goober5000
 *
 * Revision 2.6  2004/03/05 09:01:53  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.5  2004/02/14 00:18:29  randomtiger
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
 * Revision 2.4  2003/11/17 06:52:51  bobboau
 * got assert to work again
 *
 * Revision 2.3  2003/10/15 22:03:23  Kazan
 * Da Species Update :D
 *
 * Revision 2.2  2003/04/29 01:03:22  Goober5000
 * implemented the custom hitpoints mod
 * --Goober5000
 *
 * Revision 2.1  2002/08/01 01:41:04  penguin
 * The big include file move
 *
 * Revision 2.0  2002/06/03 04:02:21  penguin
 * Warpcore CVS sync
 *
 * Revision 1.3  2002/05/10 20:42:43  mharris
 * use "ifndef NO_NETWORK" all over the place
 *
 * Revision 1.2  2002/05/03 22:07:08  mharris
 * got some stuff to compile
 *
 * Revision 1.1  2002/05/02 18:03:04  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 31    9/08/99 10:52a Mikek
 * Reduce the number of asteroids that can be thrown at a target at
 * Medium+.
 * 
 * 30    9/06/99 12:46a Andsager
 * Add weapon_explosion_ani LOD
 * 
 * 29    9/05/99 3:13p Mikek
 * Slightly decrease number of asteroids thrown at escorted ships.
 * 
 * 28    8/26/99 8:51p Dave
 * Gave multiplayer TvT messaging a heavy dose of sanity. Cheat codes.
 * 
 * 27    7/30/99 7:01p Dave
 * Dogfight escort gauge. Fixed up laser rendering in Glide.
 * 
 * 26    7/19/99 8:58p Andsager
 * Don't try to throw at objnum -1
 * 
 * 25    7/15/99 9:20a Andsager
 * FS2_DEMO initial checkin
 * 
 * 24    7/09/99 5:54p Dave
 * Seperated cruiser types into individual types. Added tons of new
 * briefing icons. Campaign screen.
 * 
 * 23    6/10/99 11:06a Andsager
 * Mission designed selection of asteroid types.
 * 
 * 22    6/09/99 2:55p Andsager
 * Allow multiple asteroid subtypes (of large, medium, small) and follow
 * family.
 * 
 * 21    6/08/99 7:19p Dave
 * Fixed asteroid lighting. Was because on non-darkening textures in Glide
 * only. Dumb.
 * 
 * 20    6/07/99 1:18p Andsager
 * Make asteroids choose consistent texture from large to small.  Modify
 * fireball radius of dying asteroids.
 * 
 * 19    5/03/99 10:50p Andsager
 * Make Asteroid_obj_list.  Change get_nearest_turret_objnum() to use
 * Asteroid_obj_list, Ship_obj_list and Missile_obj_list vs.
 * obj_used_list.
 * 
 * 18    4/23/99 12:01p Johnson
 * Added SIF_HUGE_SHIP
 * 
 * 17    4/21/99 6:15p Dave
 * Did some serious housecleaning in the beam code. Made it ready to go
 * for anti-fighter "pulse" weapons. Fixed collision pair creation. Added
 * a handy macro for recalculating collision pairs for a given object.
 * 
 * 16    4/16/99 2:34p Andsager
 * Second pass on debris fields
 * 
 * 15    4/15/99 5:27p Andsager
 * Fix explosion sound for Ships debris
 * 
 * 14    4/15/99 5:00p Andsager
 * Frist pass on Debris field
 * 
 * 13    2/07/99 8:51p Andsager
 * Add inner bound to asteroid field.  Inner bound tries to stay astroid
 * free.  Wrap when within and don't throw at ships inside.
 * 
 * 12    1/20/99 6:04p Dave
 * Another bit of stuff for beam weapons. Ships will properly use them
 * now, although they're really deadly.
 * 
 * 11    1/06/99 2:24p Dave
 * Stubs and release build fixes.
 * 
 * 10    12/03/98 3:14p Andsager
 * Check in code that checks rotating submodel actually has ship subsystem
 * 
 * 9     11/19/98 11:07p Andsager
 * Check in of physics and collision detection of rotating submodels
 * 
 * 8     11/13/98 10:12a Andsager
 * simplify collision code
 * 
 * 7     11/05/98 5:55p Dave
 * Big pass at reducing #includes
 * 
 * 6     10/23/98 3:51p Dave
 * Full support for tstrings.tbl and foreign languages. All that remains
 * is to make it active in Fred.
 * 
 * 5     10/23/98 1:11p Andsager
 * Make ship sparks emit correctly from rotating structures.
 * 
 * 4     10/16/98 1:22p Andsager
 * clean up header files
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
 * 80    7/14/98 3:30p Dave
 * 
 * 79    5/18/98 12:05p Mike
 * Make sm1-06a a little harder.
 * 
 * 78    5/06/98 12:36p Lawrance
 * disable ambient asteroid sound for now
 * 
 * 77    4/30/98 12:49a Allender
 * deal with asteroid problems in multiplayer
 * 
 * 76    4/27/98 1:46p Mike
 * Make asteroid_density have no meaning.
 * 
 * 75    4/24/98 11:59a Dan
 * fix bug where a null vector would occur when death_hit_pos was assinged
 * to asteroid_pos
 * 
 * 74    4/23/98 4:43p Andsager
 * Don't call 	asteroid_update_collide() on creating asteroids since it
 * creates sets of collisions pairs.  Pairs are created when objects are
 * merged in obj_merge_created_list()
 * 
 * 73    4/21/98 1:23a Mike
 * Tone down difficulty in asteroid toss missions.
 * 
 * 72    4/16/98 11:57a John
 * Removed debug and unused registry values.
 * 
 * 71    4/08/98 1:27a Lawrance
 * Fix typo.
 * 
 * 70    4/08/98 1:17a Lawrance
 * Fix bug with red and white brackets being drawn on top of one another.
 * 
 * 69    4/02/98 6:28p Lawrance
 * remove asteroid code from demo
 * 
 * 68    4/02/98 5:11p Mike
 * Quick out of asteroid code if none present.
 * 
 * 67    4/02/98 1:34p Mike
 * Minor difficulty balance, made a bit easier.
 * 
 * 66    4/02/98 1:29p Andsager
 * 
 * 65    4/01/98 5:34p John
 * Made only the used POFs page in for a level.   Reduced some interp
 * arrays.    Made custom detail level work differently.
 * 
 * 64    3/31/98 5:11p John
 * Removed demo/save/restore.  Made NDEBUG defined compile.  Removed a
 * bunch of debug stuff out of player file.  Made model code be able to
 * unload models and malloc out only however many models are needed.
 * 
 * 63    3/30/98 4:02p John
 * Made machines with < 32 MB of RAM use every other frame of certain
 * bitmaps.   Put in code to keep track of how much RAM we've malloc'd.
 * 
 * 62    3/30/98 2:38p Mike
 * Add asteroid_density to detail level support.
 * No force explosion damage in training missions.
 * Make cargo deathrolls shorter.
 * 
 * 61    3/30/98 10:07a Lawrance
 * Um, make asteroids work again.
 * 
 * 60    3/29/98 12:55a Lawrance
 * Get demo build working with limited set of data.
 * 
 * 59    3/26/98 9:19a Lawrance
 * Support multiple asteroid pofs
 * 
 * 58    3/25/98 3:44p Andsager
 * Fixed bug in asteroid_sub_create, where last_pos field was invalid in
 * its creation frame but needed for collision in that frame.
 * 
 * 57    3/25/98 2:03p Sandeep
 * Remove "f" from top of file.
 * 
 * 56    3/25/98 1:50p Mike
 * Asteroid destruction (sm1-06a) balancing.
 * 
 * 55    3/25/98 10:43a Andsager
 * Hack for ship_asteroid collisions when erroneous relative_vel >150
 * 
 * 54    3/23/98 12:20p Andsager
 * Enable collision from rotation in ship_debris and ship_debris
 * collisions.
 * 
 * 53    3/22/98 4:09p Andsager
 * Make moment of inertia based on model.
 * 
 * 52    3/21/98 3:33p Lawrance
 * When asteroid collides with an escort ship, make the sound carry
 * farther
 * 
 * 51    3/19/98 12:09p John
 * Fixed a bug using 6 characters.   r_heavy was using local coordinates
 * instead of world so all asteroid-ship and debris-ship hitpos's were in
 * the wrong spot.  Ok, I rearreanged some code to make it clearer also.
 * 
 * 50    3/17/98 5:55p Lawrance
 * Support object linked sounds for asteroids.
 * 
 * 49    3/17/98 3:44p Mike
 * Make asteroids-thrown-at-target not only happen when an asteroid wraps.
 * 
 * 48    3/17/98 9:53a Lawrance
 * Asteroid area effect only affect ships
 * 
 * 47    3/17/98 9:25a Allender
 * some minor asteroid changes for multiplayer
 * 
 * 46    3/17/98 12:16a Allender
 * asteroids in multiplayer -- minor problems with position being correct
 * 
 * 45    3/14/98 1:44p Mike
 * Todolist items 3365..3368.  Make child asteroids collide properly.
 * Make asteroid throwing less bunchy, toss asteroids earlier, make
 * facing-ness not break mission balance.
 * 
 * 44    3/13/98 9:06a John
 * Made missile thrusters use a different min dist for scaling.   Made
 * missilies not use lighting.
 * 
 * 43    3/12/98 4:01p Lawrance
 * Only show warning brackets for cruiser/capital ships on the same team
 * 
 * $NoKeywords: $
 */


#include "asteroid/asteroid.h"
#include "object/object.h"
#include "object/objcollide.h"
#include "freespace2/freespace.h"
#include "io/timer.h"
#include "render/3d.h"
#include "fireball/fireballs.h"
#include "gamesnd/gamesnd.h"
#include "particle/particle.h"
#include "globalincs/linklist.h"
#include "hud/hud.h"
#include "hud/hudescort.h"
#include "hud/hudgauges.h"
#include "ship/shiphit.h"
#include "math/staticrand.h"
#include "globalincs/systemvars.h"
#include "localization/localize.h"
#include "stats/scoring.h"
#include "hud/hudtarget.h"
#include "weapon/weapon.h"
#include "ship/ship.h"
#include "parse/parselo.h"
#include "math/vecmat.h"
#include "model/model.h"
#include "species_defs/species_defs.h"
#include "iff_defs/iff_defs.h"
#include "network/multiutil.h"
#include "network/multimsgs.h"
#include "network/multi.h"
#include "parse/scripting.h"

#include <vector>


//WMC - we want asteroid ability with demo!!
//#ifndef FS2_DEMO

#define			ASTEROID_OBJ_USED	(1<<0)				// flag used in asteroid_obj struct
#define			MAX_ASTEROID_OBJS	MAX_ASTEROIDS		// max number of asteroids tracked in asteroid list
asteroid_obj	Asteroid_objs[MAX_ASTEROID_OBJS];	// array used to store asteroid object indexes
asteroid_obj	Asteroid_obj_list;						// head of linked list of asteroid_obj structs

// used for randomly generating debris type when there are multiple sizes.
#define SMALL_DEBRIS_WEIGHT	8
#define MEDIUM_DEBRIS_WEIGHT	4
#define LARGE_DEBRIS_WEIGHT	1

int	Asteroids_enabled = 1;
int	Num_asteroids = 0;
int	Asteroid_throw_objnum = -1;		//	Object index of ship to throw asteroids at.
int	Next_asteroid_throw;

std::vector<asteroid_info> Asteroid_info;
asteroid			Asteroids[MAX_ASTEROIDS];
asteroid_field	Asteroid_field;


static int		Asteroid_impact_explosion_ani;
static float	Asteroid_impact_explosion_radius;

#define	ASTEROID_CHECK_WRAP_TIMESTAMP			2000	// how often an asteroid gets checked for wrapping
#define	ASTEROID_UPDATE_COLLIDE_TIMESTAMP	2000	// how often asteroid is checked for impending collisions with escort ships
#define	ASTEROID_MIN_COLLIDE_TIME				24		// time in seconds to check for asteroid colliding

//	Force updating of pair stuff for asteroid *objp.
void asteroid_update_collide(object *objp)
{
	// Asteroid has wrapped, update collide objnum and flags
	Asteroids[objp->instance].collide_objnum = -1;
	Asteroids[objp->instance].collide_objsig = -1;
	OBJ_RECALC_PAIRS(objp);	
}

// Clear out the Asteroid_obj_list
//
void asteroid_obj_list_init()
{
	int i;

	list_init(&Asteroid_obj_list);
	for ( i = 0; i < MAX_ASTEROID_OBJS; i++ ) {
		Asteroid_objs[i].flags = 0;
	}
}

// ---------------------------------------------------
// asteroid_obj_list_add()
//
// Function to add a node from the Asteroid_obj_list.  Only
// called from weapon_create()
int asteroid_obj_list_add(int objnum)
{
	int index;

	asteroid *cur_asteroid = &Asteroids[Objects[objnum].instance];
	index = cur_asteroid - Asteroids;

	Assert(index >= 0 && index < MAX_ASTEROID_OBJS);
	Assert(!Asteroid_objs[index].flags & ASTEROID_OBJ_USED);

	Asteroid_objs[index].flags = 0;
	Asteroid_objs[index].objnum = objnum;
	list_append(&Asteroid_obj_list, &Asteroid_objs[index]);
	Asteroid_objs[index].flags |= ASTEROID_OBJ_USED;

	return index;
}

// ---------------------------------------------------
// missle_obj_list_remove()
//
// Function to remove a node from the Asteroid_obj_list.  Only
// called from weapon_delete()
void asteroid_obj_list_remove(object * obj)
{
	int index = obj->instance;

	Assert(index >= 0 && index < MAX_ASTEROID_OBJS);
	Assert(Asteroid_objs[index].flags & ASTEROID_OBJ_USED);

	list_remove(&Asteroid_obj_list, &Asteroid_objs[index]);	
	Asteroid_objs[index].flags = 0;
}


//	Prevent speed from getting too huge so it's hard to catch up to an asteroid.
float asteroid_cap_speed(int asteroid_info_index, float speed)
{
	float max, double_max;

	Assert( asteroid_info_index < (int)Asteroid_info.size() );

	max = Asteroid_info[asteroid_info_index].max_speed;
	double_max = max * 2;

	while (speed > double_max){
		speed *= 0.5f;
	}
	
	if (speed > max){
		speed *= 0.75f;
	}

	return speed;
}

// Returns whether position is inside inner bounding volume
// sum together the following: 1 inside x, 2 inside y, 4 inside z
// inside only when sum = 7
int asteroid_in_inner_bound_with_axes(asteroid_field *asfieldp, vec3d *pos, float delta)
{
	Assert(asfieldp->has_inner_bound);

	int rval = 0;
	if ( (pos->xyz.x > asfieldp->inner_min_bound.xyz.x - delta) && (pos->xyz.x < asfieldp->inner_max_bound.xyz.x + delta) ) {
		rval += 1;
	}

	if ( (pos->xyz.y > asfieldp->inner_min_bound.xyz.y - delta) && (pos->xyz.y < asfieldp->inner_max_bound.xyz.y + delta) ) {
		rval += 2;
	}

	if ( (pos->xyz.z > asfieldp->inner_min_bound.xyz.z - delta) && (pos->xyz.z < asfieldp->inner_max_bound.xyz.z + delta) ) {
		rval += 4;
	}

	return rval;
}

// check if asteroid is within inner bound
// return 0 if not inside or no inner bound, 1 if inside inner bound
int asteroid_in_inner_bound(asteroid_field *asfieldp, vec3d *pos, float delta) {

	if (!asfieldp->has_inner_bound) {
		return 0;
	}

	return (asteroid_in_inner_bound_with_axes(asfieldp, pos, delta) == 7);
}

// repositions asteroid outside the inner box on all 3 axes
// moves to the other side of the inner box a distance delta from edge of box
void inner_bound_pos_fixup(asteroid_field *asfieldp, vec3d *pos)
{
	if (!asteroid_in_inner_bound(asfieldp, pos, 0)) {
		return;
	}

	float dist1, dist2;
	int axis;

	for (axis=0; axis<3; axis++) {
		dist1 = pos->a1d[axis] - asfieldp->inner_min_bound.a1d[axis];
		dist2 = asfieldp->inner_max_bound.a1d[axis] - pos->a1d[axis];
		Assert(dist1 >= 0 && dist2 >= 0);

		if (dist1 < dist2) {
			pos->a1d[axis] = asfieldp->inner_max_bound.a1d[axis] + dist1;
		} else {
			pos->a1d[axis] = asfieldp->inner_min_bound.a1d[axis] - dist2;
		}
	}
}


// Create a single asteroid 
object *asteroid_create(asteroid_field *asfieldp, int asteroid_type, int asteroid_subtype)
{
	int				n, objnum;
	matrix			orient;
	object			*objp;
	asteroid			*asp;
	asteroid_info	*asip;
	vec3d			pos, delta_bound;
	angles			angs;
	float				radius;
	ushort			signature;
	int				rand_base;

	// bogus
	if(asfieldp == NULL) {
		return NULL;
	}

	for (n=0; n<MAX_ASTEROIDS; n++) {
		if (!(Asteroids[n].flags & AF_USED)) {
			break;
		}
	}

	if (n >= MAX_ASTEROIDS) {
		nprintf(("Warning","Could not create asteroid, no more slots left\n"));
		return NULL;
	}

	if((asteroid_type < 0) || (asteroid_type >= (((int)Species_info.size() + 1) * NUM_DEBRIS_SIZES))) {
		return NULL;
	}

	if((asteroid_subtype < 0) || (asteroid_subtype >= NUM_DEBRIS_POFS)) {
		return NULL;
	}

	// HACK: multiplayer asteroid subtype always 0 to keep subtype in sync
	if ( Game_mode & GM_MULTIPLAYER) {
		asteroid_subtype = 0;
	}	

	asip = &Asteroid_info[asteroid_type];

	// bogus
	if(asip->modelp[asteroid_subtype] == NULL) {
		return NULL;
	}	

	asp = &Asteroids[n];
	asp->asteroid_type = asteroid_type;
	asp->asteroid_subtype = asteroid_subtype;
	asp->flags = 0;
	asp->flags |= AF_USED;
	asp->check_for_wrap = timestamp_rand(0, ASTEROID_CHECK_WRAP_TIMESTAMP);
	asp->check_for_collide = timestamp_rand(0, ASTEROID_UPDATE_COLLIDE_TIMESTAMP);
	asp->final_death_time = timestamp(-1);
	asp->collide_objnum = -1;
	asp->collide_objsig = -1;
	asp->target_objnum = -1;

	radius = model_get_radius(asip->model_num[asteroid_subtype]);

	vm_vec_sub(&delta_bound, &asfieldp->max_bound, &asfieldp->min_bound);

	// for multiplayer, we want to do a static_rand so that everything behaves the same on all machines
	signature = 0;
	rand_base = 0;
	if ( Game_mode & GM_NORMAL ) {
		pos.xyz.x = asfieldp->min_bound.xyz.x + delta_bound.xyz.x * frand();
		pos.xyz.y = asfieldp->min_bound.xyz.y + delta_bound.xyz.y * frand();
		pos.xyz.z = asfieldp->min_bound.xyz.z + delta_bound.xyz.z * frand();

		inner_bound_pos_fixup(asfieldp, &pos);
		// vm_set_identity(&orient);
		angs.p = frand() * PI2;
		angs.b = frand() * PI2;
		angs.h = frand() * PI2;
	} else {
		signature = multi_assign_network_signature( MULTI_SIG_ASTEROID );
		rand_base = signature;

		pos.xyz.x = asfieldp->min_bound.xyz.x + delta_bound.xyz.x * static_randf( rand_base++ );
		pos.xyz.y = asfieldp->min_bound.xyz.y + delta_bound.xyz.y * static_randf( rand_base++ );
		pos.xyz.z = asfieldp->min_bound.xyz.z + delta_bound.xyz.z * static_randf( rand_base++ );

		inner_bound_pos_fixup(asfieldp, &pos);
		// vm_set_identity(&orient);
		angs.p = static_randf( rand_base++ ) * PI2;
		angs.b = static_randf( rand_base++ ) * PI2;
		angs.h = static_randf( rand_base++ ) * PI2;
	}

	vm_angles_2_matrix(&orient, &angs);

	objnum = obj_create( OBJ_ASTEROID, -1, n, &orient, &pos, radius, OF_RENDERS | OF_PHYSICS | OF_COLLIDES);
	// mprintf(("Framecount: %d asteroid create: obj = %d\n", Framecount, objnum));
	
	if ( (objnum == -1) || (objnum >= MAX_OBJECTS) ) {
		mprintf(("Couldn't create asteroid -- out of object slots\n"));
		return NULL;
	}

	asp->objnum = objnum;

	// Add to Asteroid_used_list
	asteroid_obj_list_add(objnum);

	objp = &Objects[objnum];

	if ( Game_mode & GM_MULTIPLAYER ){
		objp->net_signature = signature;
	}

	Num_asteroids++;

	if (radius < 1.0) {
		radius = 1.0f;
	}

	vec3d rotvel;
	if ( Game_mode & GM_NORMAL ) {
		vm_vec_rand_vec_quick(&rotvel);
		vm_vec_scale(&rotvel, frand()/4.0f + 0.1f);
		objp->phys_info.rotvel = rotvel;
		vm_vec_rand_vec_quick(&objp->phys_info.vel);
	} else {
		static_randvec( rand_base++, &rotvel );
		vm_vec_scale(&rotvel, static_randf(rand_base++)/4.0f + 0.1f);
		objp->phys_info.rotvel = rotvel;
		static_randvec( rand_base++, &objp->phys_info.vel );
	}


	float speed;

	if ( Game_mode & GM_NORMAL ) {
		speed = asteroid_cap_speed(asteroid_type, asfieldp->speed*frand_range(0.5f + (float) Game_skill_level/NUM_SKILL_LEVELS, 2.0f + (float) (2*Game_skill_level)/NUM_SKILL_LEVELS));
	} else {
		speed = asteroid_cap_speed(asteroid_type, asfieldp->speed*static_randf_range(rand_base++, 0.5f + (float) Game_skill_level/NUM_SKILL_LEVELS, 2.0f + (float) (2*Game_skill_level)/NUM_SKILL_LEVELS));
	}
	
	vm_vec_scale(&objp->phys_info.vel, speed);
	objp->phys_info.desired_vel = objp->phys_info.vel;

	// blow out his reverse thrusters. Or drag, same thing.
	objp->phys_info.rotdamp = 10000.0f;
	objp->phys_info.side_slip_time_const = 10000.0f;
	objp->phys_info.flags |= (PF_REDUCED_DAMP | PF_DEAD_DAMP);	// set damping equal for all axis and not changable

	// Fill in the max_vel field, so the collision pair stuff knows
	// how fast this can move maximum in order to throw out collisions.
	// This is in local coordinates, so Z is forward velocity.
	objp->phys_info.max_vel.xyz.x = 0.0f;
	objp->phys_info.max_vel.xyz.y = 0.0f;
	objp->phys_info.max_vel.xyz.z = vm_vec_mag(&objp->phys_info.desired_vel);
	
	objp->phys_info.mass = asip->modelp[asteroid_subtype]->rad * 700.0f;
	objp->phys_info.I_body_inv.vec.rvec.xyz.x = 1.0f / (objp->phys_info.mass*asip->modelp[asteroid_subtype]->rad);
	objp->phys_info.I_body_inv.vec.uvec.xyz.y = objp->phys_info.I_body_inv.vec.rvec.xyz.x;
	objp->phys_info.I_body_inv.vec.fvec.xyz.z = objp->phys_info.I_body_inv.vec.rvec.xyz.x;
	objp->hull_strength = asip->initial_asteroid_strength * (0.8f + (float)Game_skill_level/NUM_SKILL_LEVELS)/2.0f;

	// ensure vel is valid
	Assert( !vm_is_vec_nan(&objp->phys_info.vel) );	

	// assign a persistant sound to the asteroid
//	obj_snd_assign(objnum, SND_ASTEROID);

	return objp;
}

//	Create asteroids when parent_objp blows up.
void asteroid_sub_create(object *parent_objp, int asteroid_type, vec3d *relvec)
{
	object	*new_objp;
	float speed;

	Assert(parent_objp->type == OBJ_ASTEROID);
	int subtype = Asteroids[parent_objp->instance].asteroid_subtype;
	new_objp = asteroid_create(&Asteroid_field, asteroid_type, subtype);

	if (new_objp == NULL)
		return;

	if ( MULTIPLAYER_MASTER ){
		send_asteroid_create( new_objp, parent_objp, asteroid_type, relvec );
	}

	//	Now, bash some values.
	vm_vec_scale_add(&new_objp->pos, &parent_objp->pos, relvec, 0.5f * parent_objp->radius);
	float parent_speed = vm_vec_mag_quick(&parent_objp->phys_info.vel);

	if ( parent_speed < 0.1f ) {
		parent_speed = vm_vec_mag_quick(&Asteroid_field.vel);
	}

	new_objp->phys_info.vel = parent_objp->phys_info.vel;
	if ( Game_mode & GM_NORMAL )
		speed = asteroid_cap_speed(asteroid_type, (frand() + 2.0f) * parent_speed);
	else
		speed = asteroid_cap_speed(asteroid_type, (static_randf(new_objp->net_signature)+2.0f) * parent_speed);

	vm_vec_scale_add2(&new_objp->phys_info.vel, relvec, speed);
	if (vm_vec_mag_quick(&new_objp->phys_info.vel) > 80.0f)
		vm_vec_scale(&new_objp->phys_info.vel, 0.5f);

	new_objp->phys_info.desired_vel = new_objp->phys_info.vel;
	vm_vec_scale_add(&new_objp->last_pos, &new_objp->pos, &new_objp->phys_info.vel, -flFrametime);
	// DA: 4/22/98  We get next line for free when new object (in obj_create_list) is merged.
	// this line gives too many collision pairs.
	//	asteroid_update_collide(new_objp);
}

// Load in an asteroid model
void asteroid_load(int asteroid_info_index, int asteroid_subtype)
{
	asteroid_info	*asip;
//	int				pof_index;

	Assert( asteroid_info_index < (int)Asteroid_info.size() );
	Assert( asteroid_subtype < NUM_DEBRIS_POFS );

	if ( (asteroid_info_index >= (int)Asteroid_info.size()) || (asteroid_subtype >= NUM_DEBRIS_POFS) ) {
		return;
	}

	asip = &Asteroid_info[asteroid_info_index];

	// pick one of MAX_ASTEROID_POFS models
	// LOAD ALL TEXTURES USED
//	static int asteroid_pof_index = rand() % MAX_ASTEROID_POFS;
//	if (Asteroid_field.debris_genre == DG_ASTEROID) {
//		pof_index = asteroid_pof_index;
//	} else {
//		// only 1 pof for ship debris type
//		pof_index = 0;
//	}

	if ( !VALID_FNAME(asip->pof_files[asteroid_subtype]) )
		return;

	asip->model_num[asteroid_subtype] = model_load( asip->pof_files[asteroid_subtype], 0, NULL );

	if (asip->model_num[asteroid_subtype] > -1) {
		asip->modelp[asteroid_subtype] = model_get(asip->model_num[asteroid_subtype]);
		
		// Stuff detail level distances.
		for (int i = 0; i < asip->num_detail_levels; i++) {
			asip->modelp[asteroid_subtype]->detail_depth[i] = i2fl(asip->detail_distance[i]);
		}
	}
}

// randomly choose a debris model within the current group
int get_debris_from_same_group(int index) {
	int group_base, group_offset;

	group_base = (index / NUM_DEBRIS_SIZES) * NUM_DEBRIS_SIZES;
	group_offset = index - group_base;

	// group base + offset
	// offset is formed by adding 1 or 2 (since 3 in model group) and mapping back in the 0-2 range
	return group_base + ((group_offset + rand()%(NUM_DEBRIS_SIZES-1) + 1) % NUM_DEBRIS_SIZES);
}

// returns a weight that depends on asteroid size.
// the weight is then used to determine the frequencty of different sizes of ship debris
int get_debris_weight(int ship_debris_index)
{
	int size = ship_debris_index % NUM_DEBRIS_SIZES;
	switch (size)
	{
		case ASTEROID_TYPE_SMALL:
			return SMALL_DEBRIS_WEIGHT;

		case ASTEROID_TYPE_MEDIUM:
			return MEDIUM_DEBRIS_WEIGHT;

		case ASTEROID_TYPE_LARGE:
			return LARGE_DEBRIS_WEIGHT;

		default:
			Int3();
			return 1;
	}
}

// Create all the asteroids for the mission, called from 
void asteroid_create_all()
{
	int i, idx;

	// ship_debris_odds_table keeps track of debris type of the next debris piece
	// each different type (size) of debris piece has a diffenent weight, smaller weighted more heavily than larger.
	// choose next type from table ship_debris_odds_table by rand()%max_weighted_range,
	// the threshold *below* which the debris type is selected.
	struct {
		int random_threshold;
		int debris_type;
	} ship_debris_odds_table[MAX_ACTIVE_DEBRIS_TYPES];

	int max_weighted_range = 0;

	if (!Asteroids_enabled)
		return;

	if (Asteroid_field.num_initial_asteroids <= 0 ) {
		return;
	}

	int max_asteroids = Asteroid_field.num_initial_asteroids; // * (1.0f - 0.1f*(MAX_DETAIL_LEVEL-Detail.asteroid_density)));

	int num_debris_types = 0;

	// get number of ship debris types
	if (Asteroid_field.debris_genre == DG_SHIP) {
		for (idx=0; idx<MAX_ACTIVE_DEBRIS_TYPES; idx++) {
			if (Asteroid_field.field_debris_type[idx] != -1) {
				num_debris_types++;
			}
		}

		// Calculate the odds table
		for (idx=0; idx<num_debris_types; idx++) {
			int debris_weight = get_debris_weight(Asteroid_field.field_debris_type[idx]);
			ship_debris_odds_table[idx].random_threshold = max_weighted_range + debris_weight;
			ship_debris_odds_table[idx].debris_type = Asteroid_field.field_debris_type[idx];
			max_weighted_range += debris_weight;
		}
	}

	// Load Asteroid/ship models
	if (Asteroid_field.debris_genre == DG_SHIP) {
		for (idx=0; idx<num_debris_types; idx++) {
			asteroid_load(Asteroid_field.field_debris_type[idx], 0);
		}
	} else {
		if (Asteroid_field.field_debris_type[0] != -1) {
			asteroid_load(ASTEROID_TYPE_SMALL, 0);
			asteroid_load(ASTEROID_TYPE_MEDIUM, 0);
			asteroid_load(ASTEROID_TYPE_LARGE, 0);
		}

		if (Asteroid_field.field_debris_type[1] != -1) {
			asteroid_load(ASTEROID_TYPE_SMALL, 1);
			asteroid_load(ASTEROID_TYPE_MEDIUM, 1);
			asteroid_load(ASTEROID_TYPE_LARGE, 1);
		}

		if (Asteroid_field.field_debris_type[2] != -1) {
			asteroid_load(ASTEROID_TYPE_SMALL, 2);
			asteroid_load(ASTEROID_TYPE_MEDIUM, 2);
			asteroid_load(ASTEROID_TYPE_LARGE, 2);
		}
	}

	// load all the asteroid/debris pieces
	for (i=0; i<max_asteroids; i++) {
		if (Asteroid_field.debris_genre == DG_ASTEROID) {
			// For asteroid, load only large asteroids

			// get a valid subtype
			int subtype = rand() % NUM_DEBRIS_POFS;
			while (Asteroid_field.field_debris_type[subtype] == -1) {
				subtype = (subtype + 1) % NUM_DEBRIS_POFS;
			}

			asteroid_create(&Asteroid_field, ASTEROID_TYPE_LARGE, subtype);
		} else {
			Assert(num_debris_types > 0);

			int rand_choice = rand() % max_weighted_range;

			for (idx=0; idx<MAX_ACTIVE_DEBRIS_TYPES; idx++) {
				// for ship debris, choose type according to odds table
				if (rand_choice < ship_debris_odds_table[idx].random_threshold) {
					asteroid_create(&Asteroid_field, ship_debris_odds_table[idx].debris_type, 0);
					break;
				}
			}
		}
	}
}

// Init asteriod system for the level, called from game_level_init()
void asteroid_level_init()
{
	Asteroid_field.num_initial_asteroids=0;
	Num_asteroids = 0;
	Next_asteroid_throw = timestamp(1);
	asteroid_obj_list_init();
}

// return !0 if asteroid should be wrapped, 0 otherwise.  Multiplayer clients will always return
// 0 from this function.  We will force a wrap on the clients when server tells us
int asteroid_should_wrap(object *objp, asteroid_field *asfieldp)
{
	if ( MULTIPLAYER_CLIENT )
		return 0;

	if (objp->pos.xyz.x < asfieldp->min_bound.xyz.x) {
		return 1;
	}

	if (objp->pos.xyz.y < asfieldp->min_bound.xyz.y) {
		return 1;
	}

	if (objp->pos.xyz.z < asfieldp->min_bound.xyz.z) {
		return 1;
	}

	if (objp->pos.xyz.x > asfieldp->max_bound.xyz.x) {
		return 1;
	}

	if (objp->pos.xyz.y > asfieldp->max_bound.xyz.y) {
		return 1;
	}

	if (objp->pos.xyz.z > asfieldp->max_bound.xyz.z) {
		return 1;
	}

	// check against inner bound
	if (asfieldp->has_inner_bound) {
		if ( (objp->pos.xyz.x > asfieldp->inner_min_bound.xyz.x) && (objp->pos.xyz.x < asfieldp->inner_max_bound.xyz.x)
		  && (objp->pos.xyz.y > asfieldp->inner_min_bound.xyz.y) && (objp->pos.xyz.y < asfieldp->inner_max_bound.xyz.y)
		  && (objp->pos.xyz.z > asfieldp->inner_min_bound.xyz.z) && (objp->pos.xyz.z < asfieldp->inner_max_bound.xyz.z) ) {

			return 1;
		}
	}

	return 0;
}

// Wrap an asteroid from one end of the asteroid field to the other
void asteroid_wrap_pos(object *objp, asteroid_field *asfieldp)
{
	if (objp->pos.xyz.x < asfieldp->min_bound.xyz.x) {
		objp->pos.xyz.x = asfieldp->max_bound.xyz.x + (objp->pos.xyz.x - asfieldp->min_bound.xyz.x);
	}

	if (objp->pos.xyz.y < asfieldp->min_bound.xyz.y) {
		objp->pos.xyz.y = asfieldp->max_bound.xyz.y + (objp->pos.xyz.y - asfieldp->min_bound.xyz.y);
	}
	
	if (objp->pos.xyz.z < asfieldp->min_bound.xyz.z) {
		objp->pos.xyz.z = asfieldp->max_bound.xyz.z + (objp->pos.xyz.z - asfieldp->min_bound.xyz.z);
	}

	if (objp->pos.xyz.x > asfieldp->max_bound.xyz.x) {
		objp->pos.xyz.x = asfieldp->min_bound.xyz.x + (objp->pos.xyz.x - asfieldp->max_bound.xyz.x);
	}

	if (objp->pos.xyz.y > asfieldp->max_bound.xyz.y) {
		objp->pos.xyz.y = asfieldp->min_bound.xyz.y + (objp->pos.xyz.y - asfieldp->max_bound.xyz.y);
	}

	if (objp->pos.xyz.z > asfieldp->max_bound.xyz.z) {
		objp->pos.xyz.z = asfieldp->min_bound.xyz.z + (objp->pos.xyz.z - asfieldp->max_bound.xyz.z);
	}

	// wrap on inner bound, check all 3 axes as needed, use of rand ok for multiplayer with send_asteroid_throw()
	inner_bound_pos_fixup(asfieldp, &objp->pos);

}


// return !0 if this asteroid is a target for any ship, otherwise return 0
int asteroid_is_targeted(object *objp)
{
	ship_obj	*so;
	object	*ship_objp;
	int		asteroid_obj_index;

	asteroid_obj_index=OBJ_INDEX(objp);

	for ( so = GET_FIRST(&Ship_obj_list); so != END_OF_LIST(&Ship_obj_list); so = GET_NEXT(so) ) {
		ship_objp = &Objects[so->objnum];
		if ( Ai_info[Ships[ship_objp->instance].ai_index].target_objnum == asteroid_obj_index ) {
			return 1;
		}
	}
	
	return 0;
}

//	Create an asteroid that will hit object *objp in delta_time seconds
void asteroid_aim_at_target(object *objp, object *asteroid_objp, float delta_time)
{
	vec3d	predicted_center_pos;
	vec3d	rand_vec;
	float		speed;

	vm_vec_scale_add(&predicted_center_pos, &objp->pos, &objp->phys_info.vel, delta_time);
	vm_vec_rand_vec_quick(&rand_vec);
	vm_vec_scale_add2(&predicted_center_pos, &rand_vec, objp->radius/2.0f);

	vm_vec_add2(&rand_vec, &objp->orient.vec.fvec);
	if (vm_vec_mag_quick(&rand_vec) < 0.1f)
		vm_vec_add2(&rand_vec, &objp->orient.vec.rvec);
	vm_vec_normalize(&rand_vec);

	speed = Asteroid_info[0].max_speed * (frand()/2.0f + 0.5f);
	
	vm_vec_copy_scale(&asteroid_objp->phys_info.vel, &rand_vec, -speed);
	asteroid_objp->phys_info.desired_vel = asteroid_objp->phys_info.vel;
	vm_vec_scale_add(&asteroid_objp->pos, &predicted_center_pos, &asteroid_objp->phys_info.vel, -delta_time);
	vm_vec_scale_add(&asteroid_objp->last_pos, &asteroid_objp->pos, &asteroid_objp->phys_info.vel, -flFrametime);
}

//	Call once per frame to maybe throw an asteroid at a ship.
//	"count" asteroids already targeted on 
void maybe_throw_asteroid(int count)
{
	if (!timestamp_elapsed(Next_asteroid_throw)) {
		return;
	}

	if (Asteroid_throw_objnum == -1) {
		return;
	}

	nprintf(("AI", "Incoming asteroids: %i\n", count));

	if (count > The_mission.ai_profile->max_incoming_asteroids[Game_skill_level])
		return;

	Next_asteroid_throw = timestamp(1000 + 1200 * count/(Game_skill_level+1));

	ship_obj	*so;
	for ( so = GET_FIRST(&Ship_obj_list); so != END_OF_LIST(&Ship_obj_list); so = GET_NEXT(so) ) {
		object *A = &Objects[so->objnum];
		if (so->objnum == Asteroid_throw_objnum) {
			int subtype = rand() % NUM_DEBRIS_POFS;
			while (Asteroid_field.field_debris_type[subtype] == -1) {
				subtype = (subtype + 1) % NUM_DEBRIS_POFS;
			}
			object *objp = asteroid_create(&Asteroid_field, ASTEROID_TYPE_LARGE, subtype);
			if (objp != NULL) {
				asteroid_aim_at_target(A, objp, ASTEROID_MIN_COLLIDE_TIME + frand() * 20.0f);

				// if asteroid is inside inner bound, kill it
				if (asteroid_in_inner_bound(&Asteroid_field, &objp->pos, 0.0f)) {
					objp->flags |= OF_SHOULD_BE_DEAD;
				} else {
					Asteroids[objp->instance].target_objnum = so->objnum;
					// DA: 4/22/98  We get next line for free when new object (in obj_create_list) is merged.
					// this line gives too many collision pairs.
					// asteroid_update_collide(objp);

					if ( MULTIPLAYER_MASTER ) {
						send_asteroid_throw( objp );
					}
				}
			}

			return;
		}
	}

}

void asteroid_delete( object * obj )
{
	int		num;
	asteroid	*asp;

	num = obj->instance;
	Assert( Asteroids[num].objnum == OBJ_INDEX(obj));

	asp = &Asteroids[num];

	Assert( Num_asteroids >= 0 );

	asp->flags = 0;
	Num_asteroids--;

	// Delete asteroid from Asteroid_used_list
	asteroid_obj_list_remove( obj );
}

// See if we should reposition the asteroid.  Only reposition if oustide the bounding volume and
// the player isn't looking towards the asteroid.
void asteroid_maybe_reposition(object *objp, asteroid_field *asfieldp)
{
	// passive field does not wrap
	if (asfieldp->field_type == FT_PASSIVE) {
		return;
	}

	if ( asteroid_should_wrap(objp, asfieldp) ) {
		vec3d	vec_to_asteroid, old_asteroid_pos, old_vel;
		float		dot, dist;

		old_asteroid_pos = objp->pos;
		old_vel = objp->phys_info.vel;

		//nprintf(("AI", "Frame %i, reposition #%i\n", Framecount, objp-Objects));

		// don't wrap asteroid if it is a target of some ship
		if ( !asteroid_is_targeted(objp) ) {

			// only wrap if player won't see asteroid disappear/reverse direction
			dist = vm_vec_normalized_dir(&vec_to_asteroid, &objp->pos, &Eye_position);
			dot = vm_vec_dot(&Eye_matrix.vec.fvec, &vec_to_asteroid);
			
			if ((dot < 0.7f) || (dist > 3000.0f)) {
				if (Num_asteroids > MAX_ASTEROIDS-10) {
					objp->flags |= OF_SHOULD_BE_DEAD;
				} else {
					// check to ensure player won't see asteroid appear either
					asteroid_wrap_pos(objp, asfieldp);
					Asteroids[objp->instance].target_objnum = -1;

					vm_vec_normalized_dir(&vec_to_asteroid, &objp->pos, &Eye_position);
					dot = vm_vec_dot(&Eye_matrix.vec.fvec, &vec_to_asteroid);
					dist = vm_vec_dist_quick(&objp->pos, &Eye_position);
					
					if (( dot > 0.7f) && (dist < 3000.0f)) {
						// player would see asteroid pop out other side, so reverse velocity instead of wrapping
						objp->pos = old_asteroid_pos;		
						vm_vec_copy_scale(&objp->phys_info.vel, &old_vel, -1.0f);
						objp->phys_info.desired_vel = objp->phys_info.vel;
						Asteroids[objp->instance].target_objnum = -1;
					}

					// update last pos (after vel is known)
					vm_vec_scale_add(&objp->last_pos, &objp->pos, &objp->phys_info.vel, -flFrametime);

					asteroid_update_collide(objp);

					if ( MULTIPLAYER_MASTER )
						send_asteroid_throw( objp );
				}
			}
		}
	}
}

void lerp(float *goal, float f1, float f2, float scale)
{
	*goal = (f2 - f1) * scale + f1;
}

void asteroid_process_pre( object *objp, float frame_time)
{
	if (Asteroids_enabled) {
	//	vec3d	*v, *vv;

	//	v = &objp->phys_info.vel;
	//	vv = &objp->phys_info.desired_vel;

		//nprintf(("AI", "Frm %i: Obj #%2i: Hull: %5.1f Vel: %5.1f %5.1f %5.1f Des: %5.1f %5.1f %5.1f\n", Framecount, objp-Objects, objp->hull_strength, v->xyz.x, v->xyz.y, v->xyz.z, vv->xyz.x, vv->xyz.y, vv->xyz.z));

		//	Make vel chase desired_vel
		lerp(&objp->phys_info.vel.xyz.x, objp->phys_info.vel.xyz.x, objp->phys_info.desired_vel.xyz.x, flFrametime);
		lerp(&objp->phys_info.vel.xyz.y, objp->phys_info.vel.xyz.y, objp->phys_info.desired_vel.xyz.y, flFrametime);
		lerp(&objp->phys_info.vel.xyz.z, objp->phys_info.vel.xyz.z, objp->phys_info.desired_vel.xyz.z, flFrametime);
	}
}

int asteroid_check_collision(object *pasteroid, object *other_obj, vec3d *hitpos, collision_info_struct *asteroid_hit_info)
{
	if (!Asteroids_enabled) {
		return 0;
	}

	mc_info	mc;
	int		num, asteroid_subtype;

	Assert( pasteroid->type == OBJ_ASTEROID );

	num = pasteroid->instance;
	Assert( num >= 0 );

	Assert( Asteroids[num].objnum == OBJ_INDEX(pasteroid));
	asteroid_subtype = Asteroids[num].asteroid_subtype;

	// asteroid_hit_info NULL  --  asteroid-weapon collision
	if ( asteroid_hit_info == NULL ) {
		// asteroid weapon collision
		Assert( other_obj->type == OBJ_WEAPON );
		mc.model_num = Asteroid_info[Asteroids[num].asteroid_type].model_num[asteroid_subtype];	// Fill in the model to check
		model_clear_instance( mc.model_num );
		mc.orient = &pasteroid->orient;					// The object's orient
		mc.pos = &pasteroid->pos;							// The object's position
		mc.p0 = &other_obj->last_pos;				// Point 1 of ray to check
		mc.p1 = &other_obj->pos;					// Point 2 of ray to check
		mc.flags = (MC_CHECK_MODEL);

		if (model_collide(&mc))
			*hitpos = mc.hit_point_world;

		return mc.num_hits;
	}

	// asteroid ship collision -- use asteroid_hit_info to calculate physics
	object *ship_obj = other_obj;
	Assert( ship_obj->type == OBJ_SHIP );

	object* heavy = asteroid_hit_info->heavy;
	object* light = asteroid_hit_info->light;
	object *heavy_obj = heavy;
	object *light_obj = light;

	vec3d zero, p0, p1;
	vm_vec_zero( &zero );
	vm_vec_sub( &p0, &light->last_pos, &heavy->last_pos );
	vm_vec_sub( &p1, &light->pos, &heavy->pos );

	mc.pos = &zero;								// The object's position
	mc.p0 = &p0;									// Point 1 of ray to check
	mc.p1 = &p1;									// Point 2 of ray to check

	// find the light object's position in the heavy object's reference frame at last frame and also in this frame.
	vec3d p0_temp, p0_rotated;
		
	// Collision detection from rotation enabled if at rotaion is less than 30 degree in frame
	// This should account for all ships
	if ( (vm_vec_mag_squared( &heavy->phys_info.rotvel ) * flFrametime*flFrametime) < (PI*PI/36) ) {
		// collide_rotate calculate (1) start position and (2) relative velocity
		asteroid_hit_info->collide_rotate = 1;
		vm_vec_rotate( &p0_temp, &p0, &heavy->last_orient );
		vm_vec_unrotate( &p0_rotated, &p0_temp, &heavy->orient );
		mc.p0 = &p0_rotated;				// Point 1 of ray to check
		vm_vec_sub( &asteroid_hit_info->light_rel_vel, &p1, &p0_rotated );
		vm_vec_scale( &asteroid_hit_info->light_rel_vel, 1/flFrametime );
		// HACK - this applies to big ships warping in/out of asteroid fields - not sure what it does
		if (vm_vec_mag(&asteroid_hit_info->light_rel_vel) > 300) {
			// nprintf(("Physics", "Asteroid type %d\n", Asteroids[asteroid_hit_info->light->instance].type));
			asteroid_hit_info->collide_rotate = 0;
			vm_vec_sub( &asteroid_hit_info->light_rel_vel, &light->phys_info.vel, &heavy->phys_info.vel );
		}
	} else {
		asteroid_hit_info->collide_rotate = 0;
		vm_vec_sub( &asteroid_hit_info->light_rel_vel, &light->phys_info.vel, &heavy->phys_info.vel );
	}

	int mc_ret_val = 0;

	if ( asteroid_hit_info->heavy == ship_obj ) {	// ship is heavier, so asteroid is sphere. Check sphere collision against ship poly model
		mc.model_num = Ship_info[Ships[ship_obj->instance].ship_info_index].model_num;		// Fill in the model to check
		mc.orient = &ship_obj->orient;								// The object's orient
		mc.radius = pasteroid->radius;
		mc.flags = (MC_CHECK_MODEL | MC_CHECK_SPHERELINE);

		// copy important data
		int copy_flags = mc.flags;  // make a copy of start end positions of sphere in  big ship RF
		vec3d copy_p0, copy_p1;
		copy_p0 = *mc.p0;
		copy_p1 = *mc.p1;

		// first test against the sphere - if this fails then don't do any submodel tests
		mc.flags = MC_ONLY_SPHERE | MC_CHECK_SPHERELINE;

		int submodel_list[MAX_ROTATING_SUBMODELS];
		int num_rotating_submodels = 0;
		polymodel *pm;

		ship_model_start(ship_obj);

		if (model_collide(&mc)) {

			// Set earliest hit time
			asteroid_hit_info->hit_time = FLT_MAX;

			// Do collision the cool new way
			if ( asteroid_hit_info->collide_rotate ) {
				// We collide with the sphere, find the list of rotating submodels and test one at a time
				model_get_rotating_submodel_list(submodel_list, &num_rotating_submodels, heavy_obj);

				// Get polymodel and turn off all rotating submodels, collide against only 1 at a time.
				pm = model_get(Ship_info[Ships[heavy_obj->instance].ship_info_index].model_num);

				// turn off all rotating submodels and test for collision
				int i;
				for (i=0; i<num_rotating_submodels; i++) {
					pm->submodel[submodel_list[i]].blown_off = 1;
				}

				// reset flags to check MC_CHECK_MODEL | MC_CHECK_SPHERELINE and maybe MC_CHECK_INVISIBLE_FACES and MC_SUBMODEL_INSTANCE
				mc.flags = copy_flags | MC_SUBMODEL_INSTANCE;


				// check each submodel in turn
				for (i=0; i<num_rotating_submodels; i++) {
					// turn on submodel for collision test
					pm->submodel[submodel_list[i]].blown_off = 0;

					// set angles for last frame (need to set to prev to get p0)
					angles copy_angles = pm->submodel[submodel_list[i]].angs;

					// find the start and end positions of the sphere in submodel RF
					pm->submodel[submodel_list[i]].angs = pm->submodel[submodel_list[i]].sii->prev_angs;
					world_find_model_point(&p0, &light_obj->last_pos, pm, submodel_list[i], &heavy_obj->last_orient, &heavy_obj->last_pos);

					pm->submodel[submodel_list[i]].angs = copy_angles;
					world_find_model_point(&p1, &light_obj->pos, pm, submodel_list[i], &heavy_obj->orient, &heavy_obj->pos);

					mc.p0 = &p0;
					mc.p1 = &p1;
					// mc.pos = zero	// in submodel RF

					mc.orient = &vmd_identity_matrix;
					mc.submodel_num = submodel_list[i];

					if ( model_collide(&mc) ) {
						if ( mc.hit_dist < asteroid_hit_info->hit_time ) {
							mc_ret_val = 1;

							// set up asteroid_hit_info common
							set_hit_struct_info(asteroid_hit_info, &mc, SUBMODEL_ROT_HIT);

							// set up asteroid_hit_info for rotating submodel
							if (asteroid_hit_info->edge_hit == 0) {
								model_find_obj_dir(&asteroid_hit_info->collision_normal, &mc.hit_normal, heavy_obj, mc.hit_submodel);
							}

							// find position in submodel RF of light object at collison
							vec3d int_light_pos, diff;
							vm_vec_sub(&diff, mc.p1, mc.p0);
							vm_vec_scale_add(&int_light_pos, mc.p0, &diff, mc.hit_dist);
							model_find_world_point(&asteroid_hit_info->light_collision_cm_pos, &int_light_pos, mc.model_num, mc.hit_submodel, &heavy_obj->orient, &zero);
						}
					}
					// Don't look at this submodel again
					pm->submodel[submodel_list[i]].blown_off = 1;
				}

			}

			// Recover and do usual ship_ship collision, but without rotating submodels
			mc.flags = copy_flags;
			*mc.p0 = copy_p0;
			*mc.p1 = copy_p1;
			mc.orient = &heavy_obj->orient;

			// usual ship_ship collision test
			if ( model_collide(&mc) )	{
				// check if this is the earliest hit
				if (mc.hit_dist < asteroid_hit_info->hit_time) {
					mc_ret_val = 1;

					set_hit_struct_info(asteroid_hit_info, &mc, SUBMODEL_NO_ROT_HIT);

					// get collision normal if not edge hit
					if (asteroid_hit_info->edge_hit == 0) {
						model_find_obj_dir(&asteroid_hit_info->collision_normal, &mc.hit_normal, heavy_obj, mc.hit_submodel);
					}

					// find position in submodel RF of light object at collison
					vec3d diff;
					vm_vec_sub(&diff, mc.p1, mc.p0);
					vm_vec_scale_add(&asteroid_hit_info->light_collision_cm_pos, mc.p0, &diff, mc.hit_dist);

				}
			}

			ship_model_stop( ship_obj );
		}

	} else {
		// Asteroid is heavier obj
		mc.model_num = Asteroid_info[Asteroids[num].asteroid_type].model_num[asteroid_subtype];		// Fill in the model to check
		model_clear_instance( mc.model_num );
		mc.orient = &pasteroid->orient;				// The object's orient
		mc.radius = model_get_core_radius(Ship_info[Ships[ship_obj->instance].ship_info_index].model_num);

		// check for collision between asteroid model and ship sphere
		mc.flags = (MC_CHECK_MODEL | MC_CHECK_SPHERELINE);

		mc_ret_val = model_collide(&mc);

		if (mc_ret_val) {
			set_hit_struct_info(asteroid_hit_info, &mc, SUBMODEL_NO_ROT_HIT);

			// set normal if not edge hit
			if ( !asteroid_hit_info->edge_hit ) {
				vm_vec_unrotate(&asteroid_hit_info->collision_normal, &mc.hit_normal, &heavy->orient);
			}

			// find position in submodel RF of light object at collison
			vec3d diff;
			vm_vec_sub(&diff, mc.p1, mc.p0);
			vm_vec_scale_add(&asteroid_hit_info->light_collision_cm_pos, mc.p0, &diff, mc.hit_dist);

		}
	}
	

	if ( mc_ret_val )	{

		// SET PHYSICS PARAMETERS
		// already have (hitpos - heavy) and light_cm_pos
		// get heavy cm pos - already have light_cm_pos
		asteroid_hit_info->heavy_collision_cm_pos = zero;

		// get r_heavy and r_light
		asteroid_hit_info->r_heavy = asteroid_hit_info->hit_pos;
		vm_vec_sub(&asteroid_hit_info->r_light, &asteroid_hit_info->hit_pos, &asteroid_hit_info->light_collision_cm_pos);

		// set normal for edge hit
		if ( asteroid_hit_info->edge_hit ) {
			vm_vec_copy_normalize(&asteroid_hit_info->collision_normal, &asteroid_hit_info->r_light);
			vm_vec_negate(&asteroid_hit_info->collision_normal);
		}

		// get world hitpos
		vm_vec_add(hitpos, &asteroid_hit_info->heavy->pos, &asteroid_hit_info->r_heavy);

		return 1;
	} else {
		// no hit
		return 0;
	}
}

void asteroid_render(object * obj)
{
	if (Asteroids_enabled) {
		int			num;
		polymodel	*pm;
		asteroid		*asp;

		pm = NULL;	
		num = obj->instance;

		Assert((num >= 0) && (num < MAX_ASTEROIDS));
		asp = &Asteroids[num];

		Assert( asp->flags & AF_USED );

		model_clear_instance( Asteroid_info[asp->asteroid_type].model_num[asp->asteroid_subtype]);
		model_render(Asteroid_info[asp->asteroid_type].model_num[asp->asteroid_subtype], &obj->orient, &obj->pos, MR_NORMAL|MR_IS_ASTEROID, OBJ_INDEX(obj) );	//	Replace MR_NORMAL with 0x07 for big yellow blobs
	}
}

//	Create a normalized vector generally in the direction from *hitpos to other_obj->pos
void asc_get_relvec(vec3d *relvec, object *other_obj, vec3d *hitpos)
{
	vec3d	tvec, rand_vec;
	int		count = 0;

	vm_vec_normalized_dir(&tvec, &other_obj->pos, hitpos);

	//	Try up to three times to get a good vector.
	while (count++ < 3) {
		vm_vec_rand_vec_quick(&rand_vec);
		vm_vec_add(relvec, &tvec, &rand_vec);
		float mag = vm_vec_mag_quick(relvec);
		if ((mag > 0.2f) && (mag < 1.7f))
			break;
	}

	vm_vec_normalize_quick(relvec);
}

// return multiplier on asteroid radius for fireball
float asteroid_get_fireball_scale_multiplier(int num)
{
	if (Asteroids[num].flags & AF_USED) {

		switch(Asteroids[num].asteroid_type) {
		case ASTEROID_TYPE_LARGE:
			return 1.5f;
			break;

		default:
			return 1.0f;
			break;
		}
	}

	Int3();	// this should not happen.  asteroid should be used.
	return 1.0f;
}


// create asteroid explosion
// exit: expected time for explosion anim to last, in seconds
float asteroid_create_explosion(object *objp)
{
	int	fireball_objnum;
	float	explosion_life, fireball_scale_multiplier;

	fireball_scale_multiplier = asteroid_get_fireball_scale_multiplier(objp->instance);

	fireball_objnum = fireball_create( &objp->pos, FIREBALL_ASTEROID, FIREBALL_LARGE_EXPLOSION, OBJ_INDEX(objp), objp->radius*fireball_scale_multiplier, 0, &objp->phys_info.vel );
	if ( fireball_objnum > -1 )	{
		explosion_life = fireball_lifeleft(&Objects[fireball_objnum]);
	} else {
		explosion_life = 0.0f;
	}

	return explosion_life;
}

// play sound when asteroid explodes
void asteriod_explode_sound(object *objp, int type, int play_loud)
{
	int	sound_index = -1;
	float range_factor = 1.0f;		// how many times sound should traver farther than normal

	if (type % NUM_DEBRIS_SIZES <= 1)
	{
		sound_index = SND_ASTEROID_EXPLODE_SMALL;
		range_factor = 5.0;
	}
	else
	{
		sound_index = SND_ASTEROID_EXPLODE_LARGE;
		range_factor = 10.0f;
	}

	Assert(sound_index != -1);

	if ( !play_loud ) {
		range_factor = 1.0f;
	}

	snd_play_3d( &Snds[sound_index], &objp->pos, &Eye_position, objp->radius, NULL, 0, 1.0f, SND_PRIORITY_MUST_PLAY, NULL, range_factor );
}

//	asteroid_do_area_effect()
//
// Do the area effect for an asteroid exploding
//
// input:	asteroid_objp	=>		object pointer to asteriod causing explosion
void asteroid_do_area_effect(object *asteroid_objp)
{
	object			*ship_objp;
	float				damage, blast;
	ship_obj			*so;
	asteroid			*asp;
	asteroid_info	*asip;

	asp = &Asteroids[asteroid_objp->instance];
	asip = &Asteroid_info[asp->asteroid_type];

	if ( asip->damage <= 0 ) {		// do a quick out if there is no damage to apply
		return;
	}

	for ( so = GET_FIRST(&Ship_obj_list); so != END_OF_LIST(&Ship_obj_list); so = GET_NEXT(so) ) {
		ship_objp = &Objects[so->objnum];
	
		// don't blast navbuoys
		if ( ship_get_SIF(ship_objp->instance) & SIF_NAVBUOY ) {
			continue;
		}

		if ( weapon_area_calc_damage(ship_objp, &asteroid_objp->pos, asip->inner_rad, asip->outer_rad, asip->blast, asip->damage, &blast, &damage, asip->outer_rad) == -1 )
			continue;

		ship_apply_global_damage(ship_objp, asteroid_objp, &asteroid_objp->pos, damage );
		weapon_area_apply_blast(NULL, ship_objp, &asteroid_objp->pos, blast, 0);
	}	// end for
}

//	Asteroid asteroid_obj was hit.
//	Apply damage.  Maybe make it break into smaller asteroids.
// input:	asteroid_obj	=>		pointer to asteroid object getting hit
//				other_obj		=>		object that hit asteroid, can be NULL if asteroid hit by area effect
//				hitpos			=>		world position asteroid was hit, can be NULL if hit by area effect
//				damage			=>		amount of damage to apply to asteroid
void asteroid_hit( object * asteroid_obj, object * other_obj, vec3d * hitpos, float damage )
{
	float		explosion_life;
	asteroid	*asp;

	asp = &Asteroids[asteroid_obj->instance];

	if (asteroid_obj->flags & OF_SHOULD_BE_DEAD){
		return;
	}

	if ( MULTIPLAYER_MASTER ){
		send_asteroid_hit( asteroid_obj, other_obj, hitpos, damage );
	}

	asteroid_obj->hull_strength -= damage;

	//nprintf(("AI", "Asteroid collided with %s, hull = %.2f\n", Object_type_names[other_obj->type], asteroid_obj->hull_strength));

	if (asteroid_obj->hull_strength < 0.0f) {
		if ( asp->final_death_time <= 0 ) {
			int play_loud_collision = 0;

			explosion_life = asteroid_create_explosion(asteroid_obj);
			if ( asp->collide_objnum == OBJ_INDEX(other_obj) ) {
//				play_loud_collision = 1;
			}
			asteriod_explode_sound(asteroid_obj, asp->asteroid_type, play_loud_collision);
			asteroid_do_area_effect(asteroid_obj);

			asp->final_death_time = timestamp( fl2i(explosion_life*1000.0f)/5 );	// Wait till 30% of vclip time before breaking the asteroid up.
			if ( hitpos ) {
				asp->death_hit_pos = *hitpos;
			} else {
				asp->death_hit_pos = asteroid_obj->pos;
				// randomize hit pos a bit, otherwise we will get a NULL vector when trying to find direction to toss child asteroids
				vec3d rand_vec;
				vm_vec_rand_vec_quick(&rand_vec);
				vm_vec_add2(&asp->death_hit_pos, &rand_vec);
			}
		}
	} else if ( other_obj ) {
		if ( other_obj->type == OBJ_WEAPON ) {
			weapon_info *wip;
			wip = &Weapon_info[Weapons[other_obj->instance].weapon_info_index];
			// If the weapon didn't play any impact animation, play custom asteroid impact animation
			if ( wip->impact_weapon_expl_index < 0 ) {
				particle_create( hitpos, &vmd_zero_vector, 0.0f, Asteroid_impact_explosion_radius, PARTICLE_BITMAP, Asteroid_impact_explosion_ani );
			}
		}
	}

	// evaluate any relevant player scoring implications
	scoring_eval_hit(asteroid_obj,other_obj);
}

// De-init asteroids, called from game_level_close()
void asteroid_level_close()
{
	int	i;

	for (i=0; i<MAX_ASTEROIDS; i++) {
		if (Asteroids[i].flags & AF_USED) {
			Asteroids[i].flags &= ~AF_USED;
			Assert(Asteroids[i].objnum >=0 && Asteroids[i].objnum < MAX_OBJECTS);
			Objects[Asteroids[i].objnum].flags |= OF_SHOULD_BE_DEAD;
		}
	}

	Asteroid_field.num_initial_asteroids=0;
}

DCF(asteroids,"Turns asteroids on/off")
{	
	if ( Dc_command )	{	
		dc_get_arg(ARG_TRUE|ARG_FALSE|ARG_NONE);		
		if ( Dc_arg_type & ARG_TRUE )	
			Asteroids_enabled = 1;	
		else if ( Dc_arg_type & ARG_FALSE ) 
			Asteroids_enabled = 0;	
		else if ( Dc_arg_type & ARG_NONE ) 
			Asteroids_enabled ^= 1;	
	}	
	if ( Dc_help )	
		dc_printf( "Usage: asteroids [bool]\nTurns asteroid system on/off.  If nothing passed, then toggles it.\n" );	
	
	if ( Dc_status )	
		dc_printf( "asteroids are %s\n", (Asteroids_enabled?"ON":"OFF") );	

/*
	if ((old_asteroids_enabled == 0) && (Asteroids_enabled == 1)) {
		asteroid_init();
	} else if ((old_asteroids_enabled == 1) && (Asteroids_enabled == 0)) {
		asteroid_uninit();
	}
*/
}

void hud_target_asteroid()
{
	int	i;
	int	start_index = 0, end_index = MAX_ASTEROIDS;

	if (Player_ai->target_objnum != -1) {
		if (Objects[Player_ai->target_objnum].type == OBJ_ASTEROID) {
			start_index = Objects[Player_ai->target_objnum].instance+1;
			end_index = start_index-1;
			if (end_index < 0)
				end_index = MAX_ASTEROIDS;
		}
	}

	i = start_index;
	while (i != end_index) {
		if (i == MAX_ASTEROIDS)
			i = 0;

		if (Asteroids[i].flags & AF_USED) {
			Assert(Objects[Asteroids[i].objnum].type == OBJ_ASTEROID);
			set_target_objnum( Player_ai, Asteroids[i].objnum);
			break;
		}

		i++;
	}
}

// Return the number of active asteroids
int asteroid_count()
{
	return Num_asteroids;
}

// See if asteroid should split up.  We delay splitting up to allow the explosion animation
// to play for a bit.
void asteroid_maybe_break_up(object *asteroid_obj)
{
	asteroid *asp;

	asp = &Asteroids[asteroid_obj->instance];

	if ( timestamp_elapsed(asp->final_death_time) ) {
		vec3d	relvec, vfh, tvec;

		Script_system.SetHookObject("Self", asteroid_obj);
		if(!Script_system.IsConditionOverride(CHA_DEATH, asteroid_obj))
		{
			asteroid_obj->flags |= OF_SHOULD_BE_DEAD;

			// multiplayer clients won't go through the following code.  asteroid_sub_create will send
			// a create packet to the client in the above named function
			if ( !MULTIPLAYER_CLIENT ) {
				// if this isn't true it's just debris, and debris doesn't break up
				if (asp->asteroid_type <= ASTEROID_TYPE_LARGE)
				{
					switch (asp->asteroid_type) {
						case ASTEROID_TYPE_SMALL:
							break;
						case ASTEROID_TYPE_MEDIUM:
							asc_get_relvec(&relvec, asteroid_obj, &asp->death_hit_pos);
							asteroid_sub_create(asteroid_obj, ASTEROID_TYPE_SMALL, &relvec);
						
							vm_vec_normalized_dir(&vfh, &asteroid_obj->pos, &asp->death_hit_pos);
							vm_vec_copy_scale(&tvec, &vfh, 2.0f);
							vm_vec_sub2(&tvec, &relvec);
							asteroid_sub_create(asteroid_obj, ASTEROID_TYPE_SMALL, &tvec);
							
							break;
						case ASTEROID_TYPE_LARGE:
							asc_get_relvec(&relvec, asteroid_obj, &asp->death_hit_pos);
							asteroid_sub_create(asteroid_obj, ASTEROID_TYPE_MEDIUM, &relvec);
						
							vm_vec_normalized_dir(&vfh, &asteroid_obj->pos, &asp->death_hit_pos);
							vm_vec_copy_scale(&tvec, &vfh, 2.0f);
							vm_vec_sub2(&tvec, &relvec);
							asteroid_sub_create(asteroid_obj, ASTEROID_TYPE_MEDIUM, &tvec);

							while (frand() > 0.6f) {
								vec3d	rvec, tvec2;
								vm_vec_rand_vec_quick(&rvec);
								vm_vec_scale_add(&tvec2, &vfh, &rvec, 0.75f);
								asteroid_sub_create(asteroid_obj, ASTEROID_TYPE_SMALL, &tvec2);
							}
							break;

						default: // this isn't going to happen.. really
							break;
					}
				}
			}
			asp->final_death_time = timestamp(-1);
		}
		Script_system.RunCondition(CHA_DEATH, '\0', NULL, asteroid_obj);
		Script_system.RemHookVar("Self");
	}
}

int asteroid_get_random_in_cone(vec3d *pos, vec3d *dir, float ang, int danger)
{
	int idx;
	vec3d asteroid_dir;

	// just pick the first asteroid which satisfies our condition
	for(idx=0; idx<Num_asteroids; idx++){
		vm_vec_sub(&asteroid_dir, &Objects[Asteroids[idx].objnum].pos, pos);
		vm_vec_normalize_quick(&asteroid_dir);

		// if it satisfies the condition
		if(vm_vec_dot(dir, &asteroid_dir) >= ang){
			return idx;
		}
	}

	return -1;
}

void asteroid_test_collide(object *asteroid_obj, object *ship_obj, mc_info *mc, bool lazy = false)
{
	float		asteroid_ray_dist;
	vec3d	asteroid_fvec, terminus;

	// See if ray from asteroid intersects bounding box of escort ship
	asteroid_ray_dist = vm_vec_mag_quick(&asteroid_obj->phys_info.desired_vel) * ASTEROID_MIN_COLLIDE_TIME;
	asteroid_fvec = asteroid_obj->phys_info.desired_vel;	

	if(IS_VEC_NULL(&asteroid_fvec)){
		terminus = asteroid_obj->pos;
	} else {
		vm_vec_normalize(&asteroid_fvec);
		vm_vec_scale_add(&terminus, &asteroid_obj->pos, &asteroid_fvec, asteroid_ray_dist);
	}

	Assert(ship_obj->type == OBJ_SHIP);

	ship_model_start(ship_obj);

	mc->model_num = Ship_info[Ships[ship_obj->instance].ship_info_index].model_num;			// Fill in the model to check
	mc->orient = &ship_obj->orient;										// The object's orientation
	mc->pos = &ship_obj->pos;												// The object's position
	mc->p0 = &asteroid_obj->pos;											// Point 1 of ray to check
	mc->p1 = &terminus;														// Point 2 of ray to check
	if (lazy) {
		mc->flags = MC_CHECK_MODEL | MC_ONLY_BOUND_BOX;
	} else {
		mc->flags = MC_CHECK_MODEL | MC_CHECK_SPHERELINE;
	}
	mc->radius = asteroid_obj->radius;

	model_collide(mc);

	ship_model_stop(ship_obj);
}

// Return !0 is the asteroid will collide with the escort ship within ASTEROID_MIN_COLLIDE_TIME
// seconds
int asteroid_will_collide(object *asteroid_obj, object *escort_objp)
{
	mc_info	mc;

	asteroid_test_collide(asteroid_obj, escort_objp, &mc);

	if ( !mc.num_hits ) {
		return 0;
	}	

	return 1;
}

// return !0 if we should warn about asteroid hitting ship, otherwise return 0
int asteroid_valid_ship_to_warn_collide(ship *shipp)
{
	if ( !(Ship_info[shipp->ship_info_index].flags & (SIF_BIG_SHIP | SIF_HUGE_SHIP)) ) {
		return 0;
	}

	if ( shipp->flags & (SF_DYING|SF_DEPART_WARP) ) {
		return 0;
	}

	// Goober5000 used to be if teams were unequal and player was not traitor, but this works for allies not on your team
	if ( iff_x_attacks_y(Player_ship->team, shipp->team) ) {
		return 0;
	}

	return 1;
}

// See if asteroid will collide with a large ship on the escort list in the next
// ASTEROID_MIN_COLLIDE_TIME seconds.
void asteroid_update_collide_flag(object *asteroid_objp)
{
	int		i, num_escorts, escort_objnum, will_collide=0;
	ship		*escort_shipp;
	asteroid	*asp;

	asp = &Asteroids[asteroid_objp->instance];
	asp->collide_objnum = -1;
	asp->collide_objsig = -1;

	// multiplayer dogfight
	if((Game_mode & GM_MULTIPLAYER) && (Netgame.type_flags & NG_TYPE_DOGFIGHT)){
		return;
	}

	num_escorts = hud_escort_num_ships_on_list();

	if ( num_escorts <= 0 ) {
		return;
	}

	for ( i = 0; i < num_escorts; i++ ) {
		escort_objnum = hud_escort_return_objnum(i);
		if ( escort_objnum >= 0 ) {
			escort_shipp = &Ships[Objects[escort_objnum].instance];
			if ( asteroid_valid_ship_to_warn_collide(escort_shipp) ) {
				will_collide = asteroid_will_collide(asteroid_objp, &Objects[escort_objnum]);
				if ( will_collide ) {
					asp->collide_objnum = escort_objnum;
					asp->collide_objsig = Objects[escort_objnum].signature;
				}
			}
		}
	}
}

// ensure that the collide objnum for the asteroid is still valid
void asteroid_verify_collide_objnum(asteroid *asp)
{
	if ( asp->collide_objnum >= 0 ) {
		if ( Objects[asp->collide_objnum].signature != asp->collide_objsig ) {
			asp->collide_objnum = -1;
			asp->collide_objsig = -1;
		}
	}
}

void asteroid_process_post(object * obj, float frame_time)
{
	if (Asteroids_enabled) {
		int num;
		num = obj->instance;
		
		//Assert( Asteroids[num].objnum == objnum );
		asteroid	*asp = &Asteroids[num];

		// Only wrap if active field
		if (Asteroid_field.field_type == FT_ACTIVE) {
			if ( timestamp_elapsed(asp->check_for_wrap) ) {
				asteroid_maybe_reposition(obj, &Asteroid_field);
				asp->check_for_wrap = timestamp(ASTEROID_CHECK_WRAP_TIMESTAMP);
			}
		}

		asteroid_verify_collide_objnum(asp);

		if ( timestamp_elapsed(asp->check_for_collide) ) {
			asteroid_update_collide_flag(obj);
			asp->check_for_collide = timestamp(ASTEROID_UPDATE_COLLIDE_TIMESTAMP);
		}

		asteroid_maybe_break_up(obj);
	}
}

// return the object number that the asteroid is about to impact
int asteroid_collide_objnum(object *asteroid_objp)
{
	return Asteroids[asteroid_objp->instance].collide_objnum;
}

// return the time until the asteroid will impact its collide_objnum
float asteroid_time_to_impact(object *asteroid_objp)
{
	float		time=-1.0f, total_dist, speed;
	asteroid	*asp;
	mc_info	mc;

	asp = &Asteroids[asteroid_objp->instance];

	if ( asp->collide_objnum < 0 ) {
		return time;
	}
	
	asteroid_test_collide(asteroid_objp, &Objects[asp->collide_objnum], &mc, true);

	if ( mc.num_hits ) {
		total_dist = vm_vec_dist(&mc.hit_point_world, &asteroid_objp->pos) - asteroid_objp->radius;
		if ( total_dist < 0 ) {
			total_dist = 0.0f;
		}
		speed = vm_vec_mag(&asteroid_objp->phys_info.vel);
		time = total_dist/speed;
	}	

	return time;
}

// read in a single asteroid section from asteroid.tbl
void asteroid_parse_section(asteroid_info *asip)
{
	required_string("$Name:");
	stuff_string(asip->name, F_NAME, NAME_LENGTH);

	required_string( "$POF file1:" );
	stuff_string(asip->pof_files[0], F_NAME, MAX_FILENAME_LEN);

	required_string( "$POF file2:" );
	stuff_string(asip->pof_files[1], F_NAME, MAX_FILENAME_LEN);

	if ( (stristr(asip->name,"Asteroid") != NULL) ) {
		required_string( "$POF file3:" );
		stuff_string(asip->pof_files[2], F_NAME, MAX_FILENAME_LEN);
	}

	asip->num_detail_levels = 0;

	required_string("$Detail distance:");
	asip->num_detail_levels = stuff_int_list(asip->detail_distance, MAX_ASTEROID_DETAIL_LEVELS, RAW_INTEGER_TYPE);

	required_string("$Max Speed:");
	stuff_float(&asip->max_speed);

	if(optional_string("$Damage Type:")) {
		char buf[NAME_LENGTH];
		stuff_string(buf, F_NAME, NAME_LENGTH);
		asip->damage_type_idx = damage_type_add(buf);
	}

	required_string("$Expl inner rad:");
	stuff_float(&asip->inner_rad);

	required_string("$Expl outer rad:");
	stuff_float(&asip->outer_rad);

	required_string("$Expl damage:");
	stuff_float(&asip->damage);

	required_string("$Expl blast:");
	stuff_float(&asip->blast);

	required_string("$Hitpoints:");
	stuff_float(&asip->initial_asteroid_strength);
}

// read in data from asteroid.tbl into Asteroid_info[] array
void asteroid_parse_tbl()
{
	char impact_ani_file[MAX_FILENAME_LEN];
	int *asteroid_tally = NULL;
	int i, rval;

	// open localization
	lcl_ext_open();

	if ((rval = setjmp(parse_abort)) != 0) {
		mprintf(("TABLES: Unable to parse '%s'!  Error code = %i.\n", "asteroid.tbl", rval));
		lcl_ext_close();
		return;
	}

	read_file_text("asteroid.tbl", CF_TYPE_TABLES);
	reset_parse();

	required_string("#Asteroid Types");

	asteroid_tally = (int*)vm_malloc(sizeof(int)*(Species_info.size() + 1));

	memset(asteroid_tally, 0, sizeof(int) * (Species_info.size()+1));


	while (required_string_either("#End","$Name:"))
	{
		asteroid_info new_asteroid;

		for (i = 0; i < NUM_DEBRIS_POFS; i++)
			new_asteroid.model_num[i] = -1;

		asteroid_parse_section( &new_asteroid );

		Assert( Species_info.size() );
		// sanity check for debris type sizes
		for (i = Species_info.size(); i >= 0; i--) {
			// must remain in proper order
			//   0 - generic debris types
			// > 0 - species specific debris types
			if ( (i == 0) || stristr(new_asteroid.name, Species_info[i-1].species_name) ) {
				Assert( asteroid_tally[i] < NUM_DEBRIS_SIZES );

				if ( asteroid_tally[i] >= NUM_DEBRIS_SIZES ) {
					// too many sizes specified, don't increment Num_debris_types and we'll overwrite
					// the current entry with the next
					break;
				}

				// we're safe to continue
				asteroid_tally[i]++;
				Asteroid_info.push_back( new_asteroid );
				break;
			}
		}
	}

	required_string("#End");

	Asteroid_impact_explosion_ani = -1;
	required_string("$Impact Explosion:");
	stuff_string(impact_ani_file, F_NAME, MAX_FILENAME_LEN);

	if ( VALID_FNAME(impact_ani_file) ) {
		int num_frames;
		Asteroid_impact_explosion_ani = bm_load_animation( impact_ani_file, &num_frames, NULL, 1);
	}

	required_string("$Impact Explosion Radius:");
	stuff_float(&Asteroid_impact_explosion_radius);

	// close localization
	lcl_ext_close();


	// check for any missing info
	char *errormsg = (char*)vm_malloc(sizeof(char)*(75 + (Species_info.size() * (NAME_LENGTH))));
	bool species_missing = false;
	strcpy(errormsg, "The following species are missing debris types in asteroids.tbl:\n");
	for (i = 0; i < (int)Species_info.size(); i++)
	{
		int idx = (i+1);	// offset from generic asteroids at 0..NUM_DEBRIS_SIZES

		if (asteroid_tally[idx] < NUM_DEBRIS_SIZES)
		{
			strcat(errormsg, Species_info[i].species_name);
			strcat(errormsg, "\n");
			species_missing = true;
		}
	}
	strcat(errormsg, "\0");

	if (species_missing)
	{
		Error(LOCATION, errormsg);
	}

	vm_free(asteroid_tally);
	vm_free(errormsg);
}

//	Return number of asteroids expected to collide with a ship.
int count_incident_asteroids()
{
	object	*asteroid_objp;
	int		count;

	count = 0;

	for ( asteroid_objp = GET_FIRST(&obj_used_list); asteroid_objp !=END_OF_LIST(&obj_used_list); asteroid_objp = GET_NEXT(asteroid_objp) ) {
		if (asteroid_objp->type == OBJ_ASTEROID ) {
			asteroid *asp = &Asteroids[asteroid_objp->instance];

			if ( asp->target_objnum >= 0 ) {
				count++;
			}
		}
	}

	return count;
}

//	Pick object to throw asteroids at.
//	Pick any capital or big ship inside the bounds of the asteroid field.
int set_asteroid_throw_objnum()
{
	if (Asteroid_field.num_initial_asteroids < 1)
		return -1;

	ship_obj	*so;
	object	*ship_objp;

	for ( so = GET_FIRST(&Ship_obj_list); so != END_OF_LIST(&Ship_obj_list); so = GET_NEXT(so) ) {
		ship_objp = &Objects[so->objnum];
		float		radius = ship_objp->radius*2.0f;

		if (Ship_info[Ships[ship_objp->instance].ship_info_index].flags & (SIF_HUGE_SHIP | SIF_BIG_SHIP)) {
			if (ship_objp->pos.xyz.x + radius > Asteroid_field.min_bound.xyz.x)
				if (ship_objp->pos.xyz.y + radius > Asteroid_field.min_bound.xyz.y)
				if (ship_objp->pos.xyz.z + radius > Asteroid_field.min_bound.xyz.z)
				if (ship_objp->pos.xyz.x - radius < Asteroid_field.max_bound.xyz.x)
				if (ship_objp->pos.xyz.y - radius < Asteroid_field.max_bound.xyz.y)
				if (ship_objp->pos.xyz.z - radius < Asteroid_field.max_bound.xyz.z)
				if (!asteroid_in_inner_bound(&Asteroid_field, &ship_objp->pos, radius))
					return so->objnum;
		}
	}
	return -1;

}

void asteroid_frame()
{
	if (Num_asteroids < 1)
		return;

	// Only throw if active field
	if (Asteroid_field.field_type == FT_PASSIVE) {
		return;
	}

	Asteroid_throw_objnum = set_asteroid_throw_objnum();

	maybe_throw_asteroid(count_incident_asteroids());
}

// Called once, at game start.  Do any one-time initializations here
void asteroid_init()
{
	asteroid_parse_tbl();
}

extern int Cmdline_targetinfo;

// Draw brackets around on-screen asteroids that are about to collide, otherwise draw an offscreen indicator
void asteroid_show_brackets()
{
	vertex	asteroid_vertex;
	object	*asteroid_objp, *player_target;
	asteroid	*asp;

	// get pointer to player target, so we don't need to take OBJ_INDEX() of asteroid_objp to compare to target_objnum
	if ( Player_ai->target_objnum >= 0 ) {
		player_target = &Objects[Player_ai->target_objnum];
	} else {
		player_target = NULL;
	}

	for ( asteroid_objp = GET_FIRST(&obj_used_list); asteroid_objp !=END_OF_LIST(&obj_used_list); asteroid_objp = GET_NEXT(asteroid_objp) ) {
		if (asteroid_objp->type != OBJ_ASTEROID ) {
			continue;
		}

		asp = &Asteroids[asteroid_objp->instance];

		if ( asp->collide_objnum < 0 ) {
			continue;
		}

		if ( asteroid_objp == player_target ) {
			continue;
		}

		g3_rotate_vertex(&asteroid_vertex,&asteroid_objp->pos);
		g3_project_vertex(&asteroid_vertex);

		if (!(asteroid_vertex.flags & PF_OVERFLOW)) {
			gr_set_color_fast(iff_get_color(IFF_COLOR_SELECTION, 1));
			hud_show_brackets(asteroid_objp, &asteroid_vertex);
			if ( Cmdline_targetinfo ) {
				hud_show_lead_indicator_quick(&asteroid_objp->pos, asteroid_objp);
			}
		}

		// if asteroid is not on screen, draw an offscreen indicator
		if ( hud_gauge_active(HUD_OFFSCREEN_INDICATOR) ) {
			if (asteroid_vertex.codes != 0) {
				float dist;
//				dist = vm_vec_dist_quick(&Player_obj->pos, &asteroid_objp->pos);
				dist = hud_find_target_distance( asteroid_objp, Player_obj );
				gr_set_color_fast(iff_get_color(IFF_COLOR_SELECTION, 1));
				hud_draw_offscreen_indicator(&asteroid_vertex, &asteroid_objp->pos, dist);
			}
		}
	}
}

// target the closest danger asteroid to the player
void asteroid_target_closest_danger()
{
	object	*asteroid_objp, *closest_asteroid_objp = NULL;
	asteroid	*asp;
	float		dist, closest_dist = 999999.0f;

	for ( asteroid_objp = GET_FIRST(&obj_used_list); asteroid_objp !=END_OF_LIST(&obj_used_list); asteroid_objp = GET_NEXT(asteroid_objp) ) {
		if (asteroid_objp->type != OBJ_ASTEROID ) {
			continue;
		}

		asp = &Asteroids[asteroid_objp->instance];

		if ( asp->collide_objnum < 0 ) {
			continue;
		}

		dist = vm_vec_dist_quick(&Player_obj->pos, &asteroid_objp->pos);

		if ( dist < closest_dist ) {
			closest_dist = dist;
			closest_asteroid_objp = asteroid_objp;
		}
	}

	if ( closest_asteroid_objp ) {
		set_target_objnum( Player_ai, OBJ_INDEX(closest_asteroid_objp) );
	}
}

void asteroid_page_in()
{
	if (Asteroid_field.num_initial_asteroids > 0 ) {
		int i, j, k;

		nprintf(( "Paging", "Paging in asteroids\n" ));


		// max of MAX_ACTIVE_DEBRIS_TYPES possible debris field models
		for (i=0; i<MAX_ACTIVE_DEBRIS_TYPES; i++) {
			asteroid_info	*asip;

			if (Asteroid_field.debris_genre == DG_ASTEROID) {
				// asteroid
				Assert(i < NUM_DEBRIS_SIZES);
				asip = &Asteroid_info[i];
			} else {
				// ship debris - always full until empty
				if (Asteroid_field.field_debris_type[i] != -1) {
					asip = &Asteroid_info[Asteroid_field.field_debris_type[i]];
				} else {
					break;
				}
			}


			for (k=0; k<NUM_DEBRIS_POFS; k++) {

				// SHIP DEBRIS - use subtype 0
				if (Asteroid_field.debris_genre == DG_SHIP) {
					if (k > 0) {
						break;
					}
				} else {
					// ASTEROID DEBRIS - use subtype (Asteroid_field.field_debris_type[] != -1)
					if (Asteroid_field.field_debris_type[k] == -1) {
						continue;
					}
				}

				if (asip->model_num[k] < 0)
					continue;

				asip->modelp[k] = model_get(asip->model_num[k]);

				// Page in textures
				for (j=0; j<asip->modelp[k]->n_textures; j++ )	{
					asip->modelp[k]->maps[j].PageIn();			
				}

			}
		} 
	}
}
/*
#else

// stubbed out functions not used in the demo
void	asteroid_init() {}
void	asteroid_level_init() {}
void	asteroid_level_close() {}
void	asteroid_create_all() {}
void	asteroid_render( object *asteroid_objp ) {}
void	asteroid_delete( object *asteroid_objp ) {}
void	asteroid_process_pre( object *asteroid_objp, float frame_time) {}
void	asteroid_process_post( object *asteroid_objp, float frame_time) {}
int	asteroid_check_collision( object *asteroid_objp, object * other_obj, vec3d * hitpos, collision_info_struct *asteroid_hit_info ) {return 0;}
void	asteroid_hit( object *asteroid_objp, object *other_objp, vec3d *hitpos, float damage ) {}
void	asteroid_save_restore(CFILE *fp) {}
int	asteroid_count() {return 0;}
int	asteroid_collide_objnum(object *asteroid_objp) {return 0;}
float asteroid_time_to_impact(object *asteroid_objp) {return 0.0f;}
void	asteroid_show_brackets() {}
void	asteroid_target_closest_danger() {}
void	asteroid_page_in() {}
void	asteroid_sub_create(object *parent_objp, int asteroid_type, vec3d *relvec) {}
void	asteroid_frame() {}


#endif
*/
