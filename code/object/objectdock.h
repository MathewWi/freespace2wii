/*
 * Created by Ian "Goober5000" Warfield for the FreeSpace2 Source Code Project.
 * You may not sell or otherwise commercially exploit the source or things you
 * create based on the source.
 */ 

/*
 * $Logfile: /Freespace2/code/Object/ObjectDock.h $
 * $Revision: 2.8.2.2 $
 * $Date: 2006-09-08 06:14:44 $
 * $Author: taylor $
 *
 * New docking system
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.8.2.1  2006/07/21 05:41:07  Goober5000
 * add another method for calculating dimensions of docked objects, plus improve an existing one
 * --Goober5000
 *
 * Revision 2.8  2006/04/20 06:32:23  Goober5000
 * proper capitalization according to Volition
 *
 * Revision 2.7  2005/07/13 03:35:31  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.6  2005/05/27 03:37:53  Goober5000
 * moved some prototypes that WMC put in the header file back to the implementation file
 * --Goober5000
 *
 * Revision 2.5  2005/05/27 03:36:20  Goober5000
 * hmm, rolled back to 2.3
 * --Goober5000
 *
 * Revision 2.3  2005/04/05 05:53:21  taylor
 * s/vector/vec3d/g, better support for different compilers (Jens Granseuer)
 *
 * Revision 2.2  2005/03/03 06:05:30  wmcoolmon
 * Merge of WMC's codebase. "Features and bugs, making Goober say "Grr!", as release would be stalled now for two months for sure"
 *
 * Revision 2.1  2005/01/11 21:38:49  Goober5000
 * multiple ship docking :)
 * don't tell anyone yet... check the SCP internal
 * --Goober500
 *
 * Revision 1.0  2004/12/30 16:35:00  Goober5000
 * Addition to CVS repository
 *
 */

#ifndef _OBJECT_DOCK_H
#define _OBJECT_DOCK_H

#include "globalincs/globals.h"
#include "physics/physics.h"

struct object;

// info for each docked object
typedef struct dock_instance {
	dock_instance *next;	// next item in list

	int dockpoint_used;		// index into polymodel->docking_bays[]
	object *docked_objp;	// object that is docked to me
} dock_instance;

// struct used when a function must be evaluated for all docked objects
typedef struct dock_function_info {

	// Set this to true when the function should return early.
	bool early_return_condition;		
	
	// The following were originally unions, but it became necessary to use structs
	// for certain functions that need to maintain two or more values.
	struct {
		bool		bool_value;
		char		char_value;
		int			int_value;
		float		float_value;
		double		double_value;

		object*		objp_value;
		vec3d*		vecp_value;
		vec3d*		vecp_value2;
	} parameter_variables, maintained_variables;


	// constructor to initialize everything to 0
	dock_function_info()
	{
		memset(this, 0, sizeof(dock_function_info));
	}

} dock_function_info;

enum axis_type
{
	X_AXIS,
	Y_AXIS,
	Z_AXIS
};


// get the first object in objp's dock list
object *dock_get_first_docked_object(object *objp);

// check whether objp is part of a docked pair
bool dock_check_docked_one_on_one(object *objp);

// count objects directly docked to objp
int dock_count_direct_docked_objects(object *objp);

// count objects directly or indirectly docked with objp
int dock_count_total_docked_objects(object *objp);

// check whether other_objp is directly docked to objp
bool dock_check_find_direct_docked_object(object *objp, object *other_objp);

// check whether other_objp is directly or indirectly docked to objp
bool dock_check_find_docked_object(object *objp, object *other_objp);

// find the object occupying objp's specified dockpoint
object *dock_find_object_at_dockpoint(object *objp, int dockpoint);

// find objp's dockpoint being occupied by other_objp
int dock_find_dockpoint_used_by_object(object *objp, object *other_objp);

// calculate the center of all docked objects (returned in dest)
void dock_calc_docked_center(vec3d *dest, object *objp);

// calculate the center of mass of all docked objects (returned in dest)
// currently the game assumes the center of mass is the center of an object; this will need to
// be fixed eventually (though this function does weight the object masses properly)
void dock_calc_docked_center_of_mass(vec3d *dest, object *objp);

// sum the masses of all directly or indirectly docked ships
float dock_calc_total_docked_mass(object *objp);

// calculate cross-sectional radius of a set of docked models
float dock_calc_max_cross_sectional_radius_perpendicular_to_axis(object *objp, axis_type axis);

// *insert sophomoric jokes here*
// The semilatus rectum is analagous to the radius, but restricted to one dimension (whereas the radius
// covers two dimensions).  It is half of the latus rectum.  More information can be found in Wikipedia
// or in any math textbook. :p
float dock_calc_max_semilatus_rectum_parallel_to_axis(object *objp, axis_type axis);

// calculate the overall forward speed of the entire docked mass
float dock_calc_docked_fspeed(object *objp);

// calculate the overall speed of the entire docked mass
float dock_calc_docked_speed(object *objp);

// Überfunction for evaluating all objects that could possibly be docked to objp.  This will
// call "function" for each docked object.  The function should store its intermediate and
// return values in the dock_function_info struct.
void dock_evaluate_all_docked_objects(object *objp, dock_function_info *infop, void (*function)(object *, dock_function_info *));

// moves all docked objects; called only from obj_move_all in object.cpp
void dock_move_docked_objects(object *objp);

// add objp1 and objp2 to each others' dock lists; currently only called by ai_do_objects_docked_stuff
void dock_dock_objects(object *objp1, int dockpoint1, object *objp2, int dockpoint2);

// remove objp1 and objp2 from each others' dock lists; currently only called by ai_do_objects_undocked_stuff
void dock_undock_objects(object *objp1, object *objp2);

#endif	// _OBJECT_DOCK_H
