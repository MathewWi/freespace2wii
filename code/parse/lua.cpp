#include "ai/ai.h"
#include "ai/aigoals.h"
#include "asteroid/asteroid.h"
#include "camera/camera.h"
#include "cfile/cfilesystem.h"
#include "debris/debris.h"
#include "cmdline/cmdline.h"
#include "freespace2/freespace.h"
#include "gamesequence/gamesequence.h"
#include "graphics/2d.h"
#include "graphics/font.h"
#include "globalincs/linklist.h"
#include "globalincs/pstypes.h"
#include "iff_defs/iff_defs.h"
#include "io/key.h"
#include "io/mouse.h"
#include "io/timer.h"
#include "lighting/lighting.h"
#include "mission/missioncampaign.h"
#include "mission/missiongoals.h"
#include "mission/missionload.h"
#include "model/model.h"
#include "object/objectshield.h"
#include "object/waypoint.h"
#include "parse/lua.h"
#include "parse/parselo.h"
#include "parse/scripting.h"
#include "particle/particle.h"
#include "playerman/player.h"
#include "render/3d.h"
#include "render/3dinternal.h"
#include "ship/ship.h"
#include "ship/shipfx.h"
#include "ship/shiphit.h"
#include "sound/audiostr.h"
#include "sound/ds.h"
#include "weapon/weapon.h"

//*************************Lua globals*************************
std::vector<ade_table_entry> Ade_table_entries;

//*************************Lua classes************************

//Library class
//This is what you define a variable of to make new libraryes
class ade_lib : public ade_lib_handle {
public:
	ade_lib(char *in_name, ade_lib_handle *parent=NULL, char *in_shortname=NULL, char *in_desc=NULL) {
		ade_table_entry ate;

		ate.Name = in_name;
		ate.ShortName = in_shortname;
		ate.Instanced = true;

		//WMC - Here's a little hack.
		//Lua did not work with __len on standard table objects.
		//So instead, all FS2 libraries are now userdata.
		//This means that no new functions can be added from
		//within the scripting environment, but I don't think
		//there will be any catastrophic consequences.
		ate.Type = 'o';
		ate.Value.Object.idx = Ade_table_entries.size();
		ate.Value.Object.sig = NULL;
		ate.Value.Object.buf = &Num_ship_classes;	//WMC - I just chose Num_ship_classes randomly.
		ate.Value.Object.size = sizeof(Num_ship_classes);
		ate.Description = in_desc;

		if(parent != NULL)
			Ade_table_entries[parent->GetIdx()].AddSubentry(ate);
		else
			Ade_table_entries.push_back(ate);
		LibIdx = Ade_table_entries.size()-1;
	}

	char *GetName();
};

char *ade_lib::GetName()
{
	if(GetIdx() == UINT_MAX)
		return "<Invalid>";

	return Ade_table_entries[GetIdx()].GetName();
}

//Function helper class
//Lets us add functions via its constructor
class ade_func : public ade_lib_handle {
public:
	ade_func(char *name, lua_CFunction func, ade_lib_handle &parent, char *args=NULL, char *desc=NULL, char *ret_type=NULL, char*ret_desc=NULL) {
		ade_table_entry ate;

		ate.Name = name;
		ate.Instanced = true;
		ate.Type = 'u';
		ate.Value.Function = func;
		ate.Arguments = args;
		ate.Description = desc;
		ate.ReturnType = ret_type;
		ate.ReturnDescription = ret_desc;

		Ade_table_entries[parent.GetIdx()].AddSubentry(ate);
		LibIdx = Ade_table_entries.size()-1;
	}
};

class ade_virtvar : public ade_lib_handle {
public:
	ade_virtvar(char *name, lua_CFunction func, ade_lib_handle &parent, char *args=NULL, char *desc=NULL, char *ret_type=NULL, char*ret_desc=NULL) {
		ade_table_entry ate;

		ate.Name = name;
		ate.Instanced = true;
		ate.Type = 'v';
		ate.Value.Function = func;
		ate.Arguments = args;
		ate.Description = desc;
		ate.ReturnType = ret_type;
		ate.ReturnDescription = ret_desc;

		Ade_table_entries[parent.GetIdx()].AddSubentry(ate);
		LibIdx = Ade_table_entries.size()-1;
	}
};

class ade_indexer : public ade_lib_handle {
public:
	ade_indexer(lua_CFunction func, ade_lib_handle &parent, char *args=NULL, char *desc=NULL, char *ret_type=NULL, char*ret_desc=NULL) {
		//Add function for meta
		ade_table_entry ate;

		ate.Name = "__indexer";
		ate.Instanced = true;
		ate.Type = 'u';
		ate.Value.Function = func;
		ate.Arguments = args;
		ate.Description = desc;
		ate.ReturnType = ret_type;
		ate.ReturnDescription = ret_desc;

		Ade_table_entries[parent.GetIdx()].AddSubentry(ate);
		LibIdx = Ade_table_entries.size()-1;
	}
};

//Struct for converting one string for another. whee!
struct string_conv {
	char *src;
	char *dest;
};

//*************************Lua operators*************************
//These are the various types of operators you can
//set in Lua. Use these as function name to activate.
//
//Format string should be "*o" or "o*", where "*" is the type of
//variable you want to deal with.
//The order varies with order of variables
string_conv ade_Operators[] = {
	{"__add",		"+"},			//var +  obj
	{"__sub",		"-"},			//var -  obj
	{"__mul",		"*"},			//var *  obj
	{"__div",		"/"},			//var /  obj
	{"__mod",		"%"},			//var %  obj
	{"__pow",		"^"},			//var ^  obj
	{"__unm",		"~"},			//var ~  obj
	{"__concat",	".."},			//var .. obj
	{"__len",		"#"},			//#var
	{"__eq",		"=="},			//var == obj
	{"__lt",		"<"},			//var <  obj
	{"__le",		"<="},			//var <= obj
	{"__newindex",	"="},			//var =  obj
	{"__call",		""},			//*shrug*
	{"__gc",		"__gc"},		//Lua's equivelant of a destructor
	//WMC - Used with tostring() lua operator.
	{"__tostring",	"(string)"},	//tostring(var)
	//WMC - This is NOT a Lua type, but for the LUA_INDEXER define
	{"__indexer",	"[]"},			//obj[var]
};

int ade_Num_operators = sizeof(ade_Operators)/sizeof(string_conv);

int ade_get_operator(char *tablename)
{
	for(int i = 0; i < ade_Num_operators; i++)
	{
		if(!strcmp(tablename, ade_Operators[i].src))
			return i;
	}

	return -1;
}

//*************************Lua helpers*************************
//Function macro
//This is what you call to make new functions
#define ADE_FUNC(name, parent, args, desc, ret_type, ret_desc)	\
	static int parent##_##name##_f(lua_State *L);	\
	ade_func parent##_##name(#name, parent##_##name##_f, parent, args, desc, ret_type, ret_desc);	\
	static int parent##_##name##_f(lua_State *L)

//Use this to handle forms of type vec.x and vec['x']. Basically an indexer for a specific variable.
//Format string should be "o*%", where * is indexing value, and % is the value to set to when LUA_SETTTING_VAR is set
#define ADE_VIRTVAR(name, parent, args, desc, ret_type, ret_desc)			\
	static int parent##_##name##_f(lua_State *L);	\
	ade_virtvar parent##_##name(#name, parent##_##name##_f, parent, args, desc, ret_type, ret_desc);	\
	static int parent##_##name##_f(lua_State *L)

//Use this with objects to deal with forms such as vec.x, vec['x'], vec[0]
//Format string should be "o*%", where * is indexing value, and % is the value to set to when LUA_SETTTING_VAR is set
#define ADE_INDEXER(parent, args, desc, ret_type, ret_desc)			\
	static int parent##___indexer_f(lua_State *L);		\
	ade_indexer parent##___indexer(parent##___indexer_f, parent, args, desc, ret_type, ret_desc);	\
	static int parent##___indexer_f(lua_State *L)

//Checks to determine whether ADE_VIRTVAR or LUA_INDEXER should set the variable
#define ADE_FUNCNAME_UPVALUE_INDEX	1
#define ADE_SETTING_UPVALUE_INDEX	2
#define ADE_SETTING_VAR	lua_toboolean(L,lua_upvalueindex(ADE_SETTING_UPVALUE_INDEX))

//*************************Lua return values*************************
#define ADE_RETURN_NIL				0
#define ADE_RETURN_TRUE				ade_set_args(L, "b", true)
#define ADE_RETURN_FALSE			ade_set_args(L, "b", false)
#define ade_set_error				ade_set_args

//*************************Begin non-lowlevel stuff*************************
//*************************Helper function declarations*********************
//WMC - Sets object handle with proper type
int ade_set_object_with_breed(lua_State *L, int obj_idx);

//**********Handles
/*ade_obj<int> l_Camera("camera", "Camera handle");
ade_obj<int> l_Cmission("cmission", "Campaign mission handle"); //WMC - We can get away with a pointer right now, but if it ever goes dynamic, it'd be a prob
ade_obj<enum_h> l_Enum("enumeration", "Enumeration object");
ade_obj<int> l_Event("event", "Mission event handle");
ade_obj<int> l_Font("font", "font handle");
ade_obj<matrix_h> l_Matrix("orientation", "Orientation matrix object");
ade_obj<int> l_Model("model", "3D Model (POF) handle");
ade_obj<object_h> l_Object("object", "Object handle");
ade_obj<physics_info_h> l_Physics("physics", "Physics handle");
ade_obj<int> l_Player("player", "Player handle");
ade_obj<object_h> l_Shields("shields", "Shields handle");
ade_obj<object_h> l_Ship("ship", "Ship handle", &l_Object);
ade_obj<int> l_Shipclass("shipclass", "Ship class handle");
ade_obj<object_h> l_ShipTextures("shiptextures", "Ship textures handle");
ade_obj<int> l_Shiptype("shiptype", "Ship type handle");
ade_obj<int> l_Species("species", "Species handle");
ade_obj<ship_subsys_h> l_Subsystem("subsystem", "Ship subsystem handle");
ade_obj<int> l_Team("team", "Team handle");
ade_obj<int> l_Texture("texture", "Texture handle");
ade_obj<int> l_Wing("wing", "Wing handle");
ade_obj<vec3d> l_Vector("vector", "Vector object");
ade_obj<object_h> l_Weapon("weapon", "Weapon handle", &l_Object);
ade_obj<ship_bank_h> l_WeaponBank("weaponbank", "Ship/subystem weapons bank handle");
ade_obj<ship_banktype_h> l_WeaponBankType("weaponbanktype", "Ship/subsystem weapons bank type handle");
ade_obj<int> l_Weaponclass("weaponclass", "Weapon class handle");
*/
//###########################################################
//########################<IMPORTANT>########################
//###########################################################
//If you are a coder who wants to add libraries, functions,
//or objects to Lua, then you want to be below this point.
//###########################################################
//########################</IMPORTANT>#######################
//###########################################################

//**********OBJECT: orientation matrix
//WMC - So matrix can use vector, I define it up here.
ade_obj<vec3d> l_Vector("vector", "Vector object");
//WMC - Due to the exorbitant times required to store matrix data,
//I initially store the matrix in this struct.
#define MH_FINE					0
#define MH_MATRIX_OUTOFDATE		1
#define MH_ANGLES_OUTOFDATE		2
struct matrix_h {
private:
	int status;

	matrix mtx;
	angles ang;

	//WMC - Call these to make sure what you want
	//is up to date
	void ValidateAngles() {
		if(status == MH_ANGLES_OUTOFDATE) {
			vm_extract_angles_matrix(&ang, &mtx);
			status = MH_FINE;
		}
	}

	void ValidateMatrix() {
		if(status == MH_MATRIX_OUTOFDATE) {
			vm_angles_2_matrix(&mtx, &ang);
			status = MH_FINE;
		}
	}
public:
	matrix_h(){mtx = vmd_identity_matrix; status = MH_ANGLES_OUTOFDATE;}
	matrix_h(matrix *in){mtx = *in; status = MH_ANGLES_OUTOFDATE;}
	matrix_h(angles *in){ang = *in; status = MH_MATRIX_OUTOFDATE;}

	angles *GetAngles()
	{
		this->ValidateAngles();
		return &ang;
	}

	matrix *GetMatrix()
	{
		this->ValidateMatrix();
		return &mtx;
	}

	void SetStatus(int n_status)
	{
		status = n_status;
	}

	//LOOK LOOK LOOK LOOK LOOK LOOK 
	//IMPORTANT!!!:
	//LOOK LOOK LOOK LOOK LOOK LOOK 
	//Don't forget to set status appropriately when you change ang or mtx.
};
ade_obj<matrix_h> l_Matrix("orientation", "Orientation matrix object");

ADE_INDEXER(l_Matrix, "p,b,h or 0-9", "Orientation component - pitch, bank, heading, or index into 3x3 matrix (1-9)", "number", "Number at the specified index, or 0 if index is invalid.")
{
	matrix_h *mh;
	char *s = NULL;
	float newval = 0.0f;
	int numargs = ade_get_args(L, "os|f", l_Matrix.GetPtr(&mh), &s, &newval);

	if(!numargs || s[1] != '\0')
		return ade_set_error(L, "f", 0.0f);

	int idx=0;
	if(s[0]=='p')
		idx = -1;
	else if(s[0]=='b')
		idx = -2;
	else if(s[0]=='h')
		idx = -3;
	else if(atoi(s))
		idx = atoi(s);

	if(idx < -3 || idx==0 || idx > 9)
		return ade_set_error(L, "f", 0.0f);

	//Handle out of date stuff.
	float *val = NULL;
	if(idx < 0)
	{
		angles *ang = mh->GetAngles();

		if(idx == -1)
			val = &ang->p;
		if(idx == -2)
			val = &ang->b;
		if(idx == -3)
			val = &ang->h;
	}
	else
	{
		idx--;	//Lua->FS2
		val = &mh->GetMatrix()->a1d[idx];
	}

	if(ADE_SETTING_VAR && *val != newval)
	{
		//WMC - I figure this is quicker
		//than just assuming matrix or angles is diff
		//and recalculating every time.

		if(idx < 0)
			mh->SetStatus(MH_MATRIX_OUTOFDATE);
		else
			mh->SetStatus(MH_ANGLES_OUTOFDATE);

		//Might as well put this here
		*val = newval;
	}

	return ade_set_args(L, "f", *val);
}

ADE_FUNC(__mul, l_Matrix, "orientation", "Multiplies two matrix objects)", "orientation", "matrix, or empty matrix if unsuccessful")
{
	matrix_h *mha=NULL, *mhb=NULL;
	if(!ade_get_args(L, "oo", l_Matrix.GetPtr(&mha), l_Matrix.GetPtr(&mhb)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	matrix mr;

	vm_matrix_x_matrix(&mr, mha->GetMatrix(), mhb->GetMatrix());

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&mr)));
}

ADE_FUNC(__tostring, l_Matrix, NULL, "Converts a matrix to a string with format \"[r1c1 r2c1 r3c1 | r1c2 r2c2 r3c2| r1c3 r2c3 r3c3]\"", "string", "Formatted string or \"<NULL\"")
{
	matrix_h *mh;
	if(!ade_get_args(L, "o", l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "s", "<NULL>");

	char buf[128];
	float *a = &mh->GetMatrix()->a1d[0];
	snprintf(buf, sizeof(buf), "[%f %f %f | %f %f %f | %f %f %f]", a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);

	return ade_set_args(L, "s", buf);
}

ADE_FUNC(getInterpolated, l_Matrix, "orientation Final, number Factor", "Returns orientation that has been interpolated to Final by Factor (0.0-1.0)", "orientation", "Interpolated orientation, or null orientation on failure")
{
	matrix_h *oriA = NULL;
	matrix_h *oriB = NULL;
	float factor = 0.0f;
	if(!ade_get_args(L, "oof", l_Matrix.GetPtr(&oriA), l_Matrix.GetPtr(&oriB), &factor))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	matrix *A = oriA->GetMatrix();
	matrix *B = oriB->GetMatrix();
	matrix final = vmd_identity_matrix;

	//matrix subtraction & scaling
	for(int i = 0; i < 9; i++)
	{
		final.a1d[i] = A->a1d[i] + (B->a1d[i] - A->a1d[i])*factor;
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&final)));
}

ADE_FUNC(getTranspose, l_Matrix, NULL, "Returns a transpose version of the specified orientation", "orientation", "Transpose matrix, or null orientation on failure")
{
	matrix_h *mh = NULL;
	if(!ade_get_args(L, "o", l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	matrix final = *mh->GetMatrix();
	vm_transpose_matrix(&final);

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&final)));
}


ADE_FUNC(rotateVector, l_Matrix, "vector Input", "Returns rotated version of given vector", "vector", "Rotated vector, or empty vector on error")
{
	matrix_h *mh;
	vec3d *v3;
	if(!ade_get_args(L, "oo", l_Matrix.GetPtr(&mh), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	vec3d v3r;
	vm_vec_rotate(&v3r, v3, mh->GetMatrix());

	return ade_set_args(L, "o", l_Vector.Set(v3r));
}

ADE_FUNC(unrotateVector, l_Matrix, "vector Input", "Returns unrotated version of given vector", "vector", "Unrotated vector, or empty vector on error")
{
	matrix_h *mh;
	vec3d *v3;
	if(!ade_get_args(L, "oo", l_Matrix.GetPtr(&mh), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	vec3d v3r;
	vm_vec_unrotate(&v3r, v3, mh->GetMatrix());

	return ade_set_args(L, "o", l_Vector.Set(v3r));
}

//**********HANDLE: cmission
/*
ade_obj<int> l_Cmission("cmission", "Campaign mission handle");
//WMC - We can get away with a pointer right now, but if it ever goes dynamic, it'd be a prob

int lua_cmission_helper(lua_State *L, int *idx)
{
	*idx = -1;
	if(!ade_get_args(L, "o", idx))
		return 0;

	if(*idx < 0 || *idx > Campaign.num_missions)
		return 0;

	return 1;
}

ADE_FUNC(isValid, l_Cmission, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Cmission.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Campaign.num_missions)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getName, l_Cmission, NULL, "Gets mission name", "string", "Mission name, or empty string if no name")
{
	int idx;
	if(!lua_cmission_helper(L, &idx))
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Campaign.missions[idx].name);
}

ADE_FUNC(isCompleted, l_Cmission, NULL, "Checks if mission is completed", "boolean", "True if mission completed, false if mission incomplete or the handle is invalid")
{
	int idx;
	if(!lua_cmission_helper(L, &idx))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", Campaign.missions[idx].completed ? true : false);
}

ADE_FUNC(getNotes, l_Cmission, NULL, "Gets mission notes", "string", "Mission notes, or an empty string if no notes or handle is invalid")
{
	int idx;
	if(!lua_cmission_helper(L, &idx))
		return ade_set_error(L, "s", "");

	if(Campaign.missions[idx].notes == NULL)
		return ade_set_args(L, "s", "");

	return ade_set_args(L, "s", Campaign.missions[idx].notes);
}

ADE_FUNC(getMainHallNum, l_Cmission, NULL, "Gets the main hall number for this mission", "number", "Main hall number, or 0 if invalid handle" )
{
	int idx;
	if(!lua_cmission_helper(L, &idx))
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", Campaign.missions[idx].main_hall);
}

ADE_FUNC(getCutsceneName, l_Cmission, NULL, "Gets the name of the cutscene for this mission (Usually played before command briefing)", "boolean", "Cutscene name or empty string if none.",)
{
	int idx;
	if(!lua_cmission_helper(L, &idx))
		return ade_set_error(L, "s", "");

	if(!strlen(Campaign.missions[idx].briefing_cutscene))
		return ade_set_args(L, "s", "");

	return ade_set_args(L, "s", Campaign.missions[idx].briefing_cutscene);
}

ADE_FUNC(getNumGoals, l_Cmission, NULL, "Gets the number of goals for this mission", "number", "Number of goals")
{
	int idx;
	if(!lua_cmission_helper(L, &idx))
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", Campaign.missions[idx].num_goals);
}

ADE_FUNC(getGoalName, l_Cmission, "Goal number (Zero-based)", "Name of goal", "Gets the name of the goal")
{
	int idx = -1;
	int gidx = -1;
	if(!ade_get_args(L, "oi", &idx, &gidx))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx > Campaign.num_missions)
		return ade_set_error(L, "s", "");

	if(gidx < 0 || gidx > Campaign.missions[idx].num_goals)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "i", Campaign.missions[idx].goals[gidx].name);
}

ADE_FUNC(getGoalStatus, l_Cmission, "Goal number (Zero-based)", "Goal status (string)", "Gets the status of the goal - Failed, Complete, or Incomplete")
{
	int idx = -1;
	int gidx = -1;
	if(!ade_get_args(L, "oi", &idx, &gidx))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Campaign.num_missions)
		return ade_set_error(L, "s", "");

	if(gidx < 0 || gidx > Campaign.missions[idx].num_goals)
		return ade_set_error(L, "s", "");

	char buf[NAME_LENGTH];

	switch( Campaign.missions[idx].goals[gidx].status)
	{
		case GOAL_FAILED:
			strcpy(buf, "Failed");
			break;
		case GOAL_COMPLETE:
			strcpy(buf, "Complete");
			break;
		case GOAL_INCOMPLETE:
			strcpy(buf, "Incomplete");
			break;
		default:
			Int3();		//????
			return ADE_RETURN_FALSE;
	}

	return ade_set_args(L, "s", buf);
}

ADE_FUNC(getNumEvents, l_Cmission, NULL, "Number of events", "Gets the number of events for this mission")
{
	int idx;
	if(!lua_cmission_helper(L, &idx))
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", Campaign.missions[idx].num_events);
}

ADE_FUNC(getEventName, l_Cmission, "Event number (Zero-based)", "Name of event", "Gets the name of the event")
{
	int idx = -1;
	int eidx = -1;
	if(!ade_get_args(L, "oi", &idx, &eidx))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Campaign.num_missions)
		return ade_set_error(L, "s", "");

	if(eidx < 0 || eidx > Campaign.missions[idx].num_events)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Campaign.missions[idx].events[eidx].name);
}

ADE_FUNC(getEventStatus, l_Cmission, "Event number (Zero-based)", "Event status (string)", "Gets the status of the event - Failed, Complete, or Incomplete")
{
	int idx = -1;
	int eidx = -1;
	if(!ade_get_args(L, "oi", &idx, &eidx))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Campaign.num_missions)
		return ade_set_error(L, "s", "");

	if(eidx < 0 || eidx > Campaign.missions[idx].num_events)
		return ade_set_error(L, "s", "");

	char buf[NAME_LENGTH];

	switch( Campaign.missions[idx].goals[eidx].status)
	{
		case EVENT_FAILED:
			strcpy(buf, "Failed");
			break;
		case EVENT_SATISFIED:
			strcpy(buf, "Complete");
			break;
		case EVENT_INCOMPLETE:
			strcpy(buf, "Incomplete");
			break;
		default:
			Int3();		//????
			return ADE_RETURN_FALSE;
	}

	return ade_set_args(L, "s", buf);
}

ADE_FUNC(getNumVariables, l_Cmission, NULL, "Number of variables", "Gets the number of saved SEXP variables for this mission")
{
	int idx;
	if(!lua_cmission_helper(L, &idx))
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", Campaign.missions[idx].num_saved_variables);
}

ADE_FUNC(getVariableName, l_Cmission, "Variable number (Zero-based)", "Variable name", "Gets the name of the variable")
{
	int idx = -1;
	int vidx = -1;
	if(!ade_get_args(L, "oi", &idx, &vidx))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Campaign.num_missions)
		return ade_set_error(L, "s", "");

	if(vidx < 0 || vidx > Campaign.missions[idx].num_saved_variables)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "i", Campaign.missions[idx].saved_variables[vidx].variable_name);
}

ADE_FUNC(getVariableType, l_Cmission, "Variable number (Zero-based)", "Variable type (string)", "Gets the type of the variable (Number or string)")
{
	int idx = -1;
	int vidx = -1;
	if(!ade_get_args(L, "oi", &idx, &vidx))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Campaign.num_missions)
		return ade_set_error(L, "s", "");

	if(vidx < 0 || vidx > Campaign.missions[idx].num_saved_variables)
		return ade_set_error(L, "s", "");

	char buf[NAME_LENGTH];

	if(Campaign.missions[idx].saved_variables[vidx].type & SEXP_VARIABLE_NUMBER)
		strcpy(buf, "number");
	if(Campaign.missions[idx].saved_variables[vidx].type & SEXP_VARIABLE_STRING)
		strcpy(buf, "string");

	return ade_set_args(L, "i", Campaign.missions[idx].saved_variables[vidx].variable_name);
}

ADE_FUNC(getVariableValue, l_Cmission, "Variable number (Zero-based)", "Variable value (number or string)", "Gets the value of a variable")
{
	int idx = -1;
	int vidx = -1;
	if(!ade_get_args(L, "oi", &idx, &vidx))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx > Campaign.num_missions)
		return ADE_RETURN_NIL;

	if(vidx < 0 || vidx > Campaign.missions[idx].num_saved_variables)
		return ADE_RETURN_NIL;

	if(Campaign.missions[idx].saved_variables[vidx].type & SEXP_VARIABLE_NUMBER)
		return ade_set_args(L, "i", atoi(Campaign.missions[idx].saved_variables[vidx].text));
	else if(Campaign.missions[idx].saved_variables[vidx].type & SEXP_VARIABLE_STRING)
		return ade_set_args(L, "s", atoi(Campaign.missions[idx].saved_variables[vidx].text));
	
	Warning(LOCATION, "LUA::getVariableName - Unknown variable type (%d) for variable (%s)", Campaign.missions[idx].saved_variables[vidx].type, Campaign.missions[idx].saved_variables[vidx].variable_name);
	return ADE_RETURN_FALSE;
}
*/

//**********OBJECT: constant class
//WMC NOTE -
//While you can have enumeration indexes in any order, make sure
//that any new enumerations have indexes of NEXT INDEX (see below)
//or after. Don't forget to increment NEXT INDEX after you're done.
//=====================================
static const int ENUM_NEXT_INDEX = 48; // <<<<<<<<<<<<<<<<<<<<<<
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
static flag_def_list Enumerations[] = {
	#define LE_ALPHABLEND_FILTER			14
	{		"ALPHABLEND_FILTER",			LE_ALPHABLEND_FILTER,			0},

	#define LE_ALPHABLEND_NONE				27
	{		"ALPHABLEND_NONE",				LE_ALPHABLEND_NONE,				0},

	#define LE_CFILE_TYPE_NORMAL			20
	{		"CFILE_TYPE_NORMAL",			LE_CFILE_TYPE_NORMAL,			0},

	#define LE_CFILE_TYPE_MEMORY_MAPPED		21
	{		"CFILE_TYPE_MEMORY_MAPPED",		LE_CFILE_TYPE_MEMORY_MAPPED,	0},

	#define LE_MOUSE_LEFT_BUTTON			1
	{		"MOUSE_LEFT_BUTTON",			LE_MOUSE_LEFT_BUTTON,			0},

	#define LE_MOUSE_RIGHT_BUTTON			2
	{		"MOUSE_RIGHT_BUTTON",			LE_MOUSE_RIGHT_BUTTON,			0},

	#define LE_MOUSE_MIDDLE_BUTTON			3
	{		"MOUSE_MIDDLE_BUTTON",			LE_MOUSE_MIDDLE_BUTTON,			0},

	#define LE_ORDER_ATTACK					28
	{		"ORDER_ATTACK",					LE_ORDER_ATTACK,				0},

	#define LE_ORDER_ATTACK_ANY				29
	{		"ORDER_ATTACK_ANY",				LE_ORDER_ATTACK_ANY,			0},

	#define LE_ORDER_DEPART					30
	{		"ORDER_DEPART",					LE_ORDER_DEPART,				0},

	#define LE_ORDER_DISABLE				31
	{		"ORDER_DISABLE",				LE_ORDER_DISABLE,				0},

	#define LE_ORDER_DISARM					32
	{		"ORDER_DISARM",					LE_ORDER_DISARM,				0},

	#define LE_ORDER_DOCK					33
	{		"ORDER_DOCK",					LE_ORDER_DOCK,					0},

	#define LE_ORDER_EVADE					34
	{		"ORDER_EVADE",					LE_ORDER_EVADE,					0},

	#define LE_ORDER_FLY_TO					35
	{		"ORDER_FLY_TO",					LE_ORDER_FLY_TO,				0},

	#define LE_ORDER_FORM_ON_WING			36
	{		"ORDER_FORM_ON_WING",			LE_ORDER_FORM_ON_WING,			0},

	#define LE_ORDER_GUARD					37
	{		"ORDER_GUARD",					LE_ORDER_GUARD,					0},

	#define LE_ORDER_IGNORE					38
	{		"ORDER_IGNORE_SHIP",			LE_ORDER_IGNORE,				0},

	#define LE_ORDER_KEEP_SAFE_DISTANCE		39
	{		"ORDER_KEEP_SAFE_DISTANCE",		LE_ORDER_KEEP_SAFE_DISTANCE,	0},

	#define LE_ORDER_PLAY_DEAD				40
	{		"ORDER_PLAY_DEAD",				LE_ORDER_PLAY_DEAD,				0},

	#define LE_ORDER_REARM					41
	{		"ORDER_REARM",					LE_ORDER_REARM,					0},

	#define LE_ORDER_STAY_NEAR				42
	{		"ORDER_STAY_NEAR",				LE_ORDER_STAY_NEAR,				0},

	#define LE_ORDER_STAY_STILL				43
	{		"ORDER_STAY_STILL",				LE_ORDER_STAY_STILL,			0},

	#define LE_ORDER_UNDOCK					44
	{		"ORDER_UNDOCK",					LE_ORDER_UNDOCK,				0},

	#define LE_ORDER_WAYPOINTS				45
	{		"ORDER_WAYPOINTS",				LE_ORDER_WAYPOINTS,				0},

	#define LE_ORDER_WAYPOINTS_ONCE			46
	{		"ORDER_WAYPOINTS_ONCE",			LE_ORDER_WAYPOINTS_ONCE,		0},

	#define LE_PARTICLE_DEBUG				4
	{		"PARTICLE_DEBUG",				LE_PARTICLE_DEBUG,				0},

	#define LE_PARTICLE_BITMAP				5
	{		"PARTICLE_BITMAP",				LE_PARTICLE_BITMAP,				0},

	#define LE_PARTICLE_FIRE				6
	{		"PARTICLE_FIRE",				LE_PARTICLE_FIRE,				0},

	#define LE_PARTICLE_SMOKE				7
	{		"PARTICLE_SMOKE",				LE_PARTICLE_SMOKE,				0},

	#define LE_PARTICLE_SMOKE2				8
	{		"PARTICLE_SMOKE2",				LE_PARTICLE_SMOKE2,				0},

	#define LE_PARTICLE_PERSISTENT_BITMAP	9
	{		"PARTICLE_PERSISTENT_BITMAP",	LE_PARTICLE_PERSISTENT_BITMAP,	0},

	#define LE_SEXPVAR_CAMPAIGN_PERSISTENT	22
	{		"SEXPVAR_CAMPAIGN_PERSISTENT",	LE_SEXPVAR_CAMPAIGN_PERSISTENT,	0},

	#define LE_SEXPVAR_NOT_PERSISTENT		23
	{		"SEXPVAR_NOT_PERSISTENT",		LE_SEXPVAR_NOT_PERSISTENT,		0},

	#define LE_SEXPVAR_PLAYER_PERSISTENT	24
	{		"SEXPVAR_PLAYER_PERSISTENT",	LE_SEXPVAR_PLAYER_PERSISTENT,	0},

	#define LE_SEXPVAR_TYPE_NUMBER			25
	{		"SEXPVAR_TYPE_NUMBER",			LE_SEXPVAR_TYPE_NUMBER,			0},

	#define LE_SEXPVAR_TYPE_STRING			26
	{		"SEXPVAR_TYPE_STRING",			LE_SEXPVAR_TYPE_STRING,			0},

	#define LE_TEXTURE_STATIC				10
	{		"TEXTURE_STATIC",				LE_TEXTURE_STATIC,				0},

	#define LE_TEXTURE_DYNAMIC				11
	{		"TEXTURE_DYNAMIC",				LE_TEXTURE_DYNAMIC,				0},

	#define LE_LOCK							12
	{		"LOCK",							LE_LOCK,						0},

	#define LE_UNLOCK						13
	{		"UNLOCK",						LE_UNLOCK,						0},

	#define	LE_NONE							15
	{		"NONE",							LE_NONE,						0},

	#define	LE_SHIELD_FRONT					16
	{		"SHIELD_FRONT",					LE_SHIELD_FRONT,				0},

	#define	LE_SHIELD_LEFT					17
	{		"SHIELD_LEFT",					LE_SHIELD_LEFT,					0},

	#define	LE_SHIELD_RIGHT					18
	{		"SHIELD_RIGHT",					LE_SHIELD_RIGHT,				0},

	#define	LE_SHIELD_BACK					19
	{		"SHIELD_BACK",					LE_SHIELD_BACK,					0},

	#define	LE_MISSION_REPEAT				47
	{		"MISSION_REPEAT",				LE_MISSION_REPEAT,				0},
};

//DO NOT FORGET to increment NEXT INDEX: !!!!!!!!!!!!!

static uint Num_enumerations = sizeof(Enumerations) / sizeof(flag_def_list);

struct enum_h {
	int index;
	bool is_constant;

	enum_h(){index=-1; is_constant=false;}
	enum_h(int n_index){index=n_index; is_constant=false;}

	bool IsValid(){return (this != NULL && index > -1 && index < ENUM_NEXT_INDEX);}
};
ade_obj<enum_h> l_Enum("enumeration", "Enumeration object");

ADE_FUNC(__newindex, l_Enum, "enumeration", "Sets enumeration to specified value (if it is not a global", "enumeration", "enumeration")
{
	enum_h *e1=NULL, *e2=NULL;
	if(!ade_get_args(L, "oo", l_Enum.GetPtr(&e1), l_Enum.GetPtr(&e2)))
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	if(!e1->is_constant)
		e1->index = e2->index;

	return ade_set_args(L, "o", l_Enum.Set(*e1));
}

ADE_FUNC(__tostring, l_Enum, NULL, "Returns enumeration name", "string", "Enumeration name, or \"<INVALID>\" if invalid")
{
	enum_h *e = NULL;
	if(!ade_get_args(L, "o", l_Enum.GetPtr(&e)))
		return ade_set_args(L, "s", "<INVALID>");

	if(e->index < 0 || e->index >= (int)Num_enumerations)
		return ade_set_args(L, "s", "<INVALID>");

	uint i;
	for(i = 0; i < Num_enumerations; i++)
	{
		if(Enumerations[i].def == e->index)
			return ade_set_args(L, "s", Enumerations[i].name);
	}

	return ade_set_args(L, "s", "<INVALID>");
}

//**********HANDLE: event
ade_obj<int> l_Event("event", "Mission event handle");

ADE_VIRTVAR(Name, l_Event, "string", "Mission event name", "string", NULL)
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Event.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_mission_events)
		return ade_set_error(L, "s", "");

	mission_event *mep = &Mission_events[idx];

	if(ADE_SETTING_VAR) {
		strncpy(mep->name, s, sizeof(mep->name) - sizeof(char));
	}

	return ade_set_args(L, "s", mep->name);
}

ADE_VIRTVAR(DirectiveText, l_Event, "string", "Directive text", "string", NULL)
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Event.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_mission_events)
		return ade_set_error(L, "s", "");

	mission_event *mep = &Mission_events[idx];

	if(ADE_SETTING_VAR && s != NULL) {
		if(mep->objective_text != NULL)
			vm_free(mep->objective_text);

		mep->objective_text = vm_strdup(s);
	}

	if(mep->objective_text != NULL)
		return ade_set_args(L, "s", mep->objective_text);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(DirectiveKeypressText, l_Event, "string", "Raw directive keypress text, as seen in FRED.", "string", NULL)
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Event.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_mission_events)
		return ade_set_error(L, "s", "");

	mission_event *mep = &Mission_events[idx];

	if(ADE_SETTING_VAR && s != NULL) {
		if(mep->objective_text != NULL)
			vm_free(mep->objective_key_text);

		mep->objective_key_text = vm_strdup(s);
	}

	if(mep->objective_key_text != NULL)
		return ade_set_args(L, "s", mep->objective_key_text);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(Interval, l_Event, "number", "Time for event to repeat (in seconds)", "number", "Repeat time, or 0 if invalid handle")
{
	int idx;
	int newinterval = 0;
	if(!ade_get_args(L, "o|i", l_Event.Get(&idx), &newinterval))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx >= Num_mission_events)
		return ade_set_error(L, "i", 0);

	mission_event *mep = &Mission_events[idx];

	if(ADE_SETTING_VAR) {
		mep->interval = newinterval;
	}

	return ade_set_args(L, "i", mep->interval);
}

ADE_VIRTVAR(ObjectCount, l_Event, "number", "Number of objects left for event", "number", "Repeat count, or 0 if invalid handle")
{
	int idx;
	int newobject = 0;
	if(!ade_get_args(L, "o|i", l_Event.Get(&idx), &newobject))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx >= Num_mission_events)
		return ade_set_error(L, "i", 0);

	mission_event *mep = &Mission_events[idx];

	if(ADE_SETTING_VAR) {
		mep->count = newobject;
	}

	return ade_set_args(L, "i", mep->count);
}

ADE_VIRTVAR(RepeatCount, l_Event, "number", "Event repeat count", "number", "Repeat count, or 0 if invalid handle")
{
	int idx;
	int newrepeat = 0;
	if(!ade_get_args(L, "o|i", l_Event.Get(&idx), &newrepeat))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx >= Num_mission_events)
		return ade_set_error(L, "i", 0);

	mission_event *mep = &Mission_events[idx];

	if(ADE_SETTING_VAR) {
		mep->repeat_count = newrepeat;
	}

	return ade_set_args(L, "i", mep->repeat_count);
}

ADE_VIRTVAR(Score, l_Event, "number", "Event score", "number", "Event score, or 0 if invalid handle")
{
	int idx;
	int newscore = 0;
	if(!ade_get_args(L, "o|i", l_Event.Get(&idx), &newscore))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx >= Num_mission_events)
		return ade_set_error(L, "i", 0);

	mission_event *mep = &Mission_events[idx];

	if(ADE_SETTING_VAR) {
		mep->score = newscore;
	}

	return ade_set_args(L, "i", mep->score);
}

ADE_FUNC(isValid, l_Event, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Event.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_mission_events)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

/*
ADE_FUNC(getStatus, l_Event, NULL, "Gets event's current status - Current, Completed, or Failed", "string", "EVENT_* enumeration")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Event.Get(&idx), &s))
		return ade_set_error(L, ";

	if(idx < 0 || idx >= Num_mission_events)
		return ade_set_error(L, "s", "");

	int rval = mission_get_event_status(idx);
	switch(rval)
	{
		case EVENT_CURRENT:
			return ade_set_args(L, "s", "Current");
		case EVENT_FAILED:
			return ade_set_args(L, "s", "Failed");
		case EVENT_SATISFIED:
			return ade_set_args(L, "s", "Completed");
		default:
			break;
	}

	return ADE_RETURN_FALSE;
}
*/

//**********HANDLE: File
//static CFILE *Lua_file_current = NULL;
static int Lua_file_handle_instances = 0;
static int Lua_max_file_handle_instances = 5;

ade_obj<CFILE*> l_File("file", "File handle");

//WMC - Unfortunately, this didn't pan out. Because I
//couldn't figure out a way to increment file_handle_instances
//if someone does file_handle_2 = file_handle_1, the number would
//not be reliable and you could end up with the file getting closed
//well before you ran out of handles.
/*
ADE_FUNC(__gc, l_File, NULL, NULL, "Destructor")
{
	Lua_file_handle_instances--;

	return ADE_RETURN_NIL;
}
*/
ADE_FUNC(isValid, l_File, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	CFILE *cfp = NULL;
	if(!ade_get_args(L, "o", l_File.Get(&cfp)))
		return ADE_RETURN_NIL;

	if(!cf_is_valid(cfp))
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(close, l_File, NULL, "Instantly closes file and invalidates all file handles", NULL, NULL)
{
	CFILE *cfp;
	if(!ade_get_args(L, "o", l_File.Get(&cfp)))
		return ADE_RETURN_FALSE;

	if(!cf_is_valid(cfp))
		return ADE_RETURN_FALSE;

	int rval = cfclose(cfp);
	if(rval != 0)
	{
		LuaError(L, "Attempt to close file resulted in error %d", rval);
		return ADE_RETURN_FALSE;
	}
	return ADE_RETURN_TRUE;
}

ADE_FUNC(flush, l_File, NULL, "Flushes file buffer to disk.", "boolean", "True for success, false on failure")
{
	CFILE *cfp = NULL;
	if(!ade_get_args(L, "o", l_File.Get(&cfp)))
		return ADE_RETURN_FALSE;

	if(!cf_is_valid(cfp))
		return ADE_RETURN_FALSE;

	//WMC - this looks reversed, yes, it's right. Look at cflush.
	int cf_result = cflush(cfp);
	return ade_set_args(L, "b", cf_result ? false : true);
}

ADE_FUNC(getPath, l_File, NULL, "Determines path of the given file", "string", "Path string of the file handle, or an empty string if it doesn't have one, or the handle is invalid")
{
	CFILE *cfp = NULL;
	if(!ade_get_args(L, "o", l_File.Get(&cfp)))
		return ade_set_error(L, "s", "");

	if(!cf_is_valid(cfp))
		return ade_set_error(L, "s", "");

	int id = cf_get_dir_type(cfp);
	if(Pathtypes[id].path != NULL)
		return ade_set_args(L, "s", Pathtypes[id].path);
	else
		return ade_set_args(L, "s", "");
}

extern int cfread_lua_number(double *buf, CFILE *cfile);
ADE_FUNC(read, l_File, "number or string, ...",
		 "Reads part of or all of a file, depending on arguments passed. Based on basic Lua file:read function."
		 "Returns nil when the end of the file is reached."
		 "<br><ul><li>\"*n\" - Reads a number.</li>"
		 "<li>\"*a\" - Reads the rest of the file and returns it as a string.</li>"
		 "<li>\"*l\" - Reads a line. Skips the end of line markers.</li>"
		 "<li>(number) - Reads given number of characters, then returns them as a string.</li></ul>",
		 "number or string, ...",
		 "Requested data, or nil if the function fails")
{
	CFILE *cfp = NULL;
	if(!ade_get_args(L, "o", l_File.Get(&cfp)))
		return ADE_RETURN_NIL;

	if(!cf_is_valid(cfp))
		return ADE_RETURN_NIL;

	int i;
	int num_returned = 0;
	int type = LUA_TNONE;

	//WMC - Since we push stuff onto the stack, we must get
	//the original arguments NOW.
	int lastarg = lua_gettop(L);
	for(i = 2; i <= lastarg; i++)
	{
		type = lua_type(L, i);
		char *fmt = NULL;
		//int num = 0;
		if(type == LUA_TSTRING)
		{
			fmt = (char*)lua_tostring(L, i);
			if(!stricmp(fmt, "*n"))
			{
				double d = 0.0f;
				if(cfread_lua_number(&d, cfp) == EOF)
					return ADE_RETURN_NIL;

				lua_pushnumber(L, d);
				num_returned++;
			}
			else if(!stricmp(fmt, "*a"))
			{
				int tell_res = cftell(cfp);
				if(tell_res < 0)
				{
					Error(LOCATION, "Critical error reading Lua file; could not cftell.");
				}
				int read_len = cfilelength(cfp) - tell_res;

				char *buf = (char *)vm_malloc(read_len + 1);
				int final_len = cfread(buf, 1, read_len, cfp);
				buf[final_len] = '\0';

				lua_pushstring(L, buf);
				vm_free(buf);
				num_returned++;
			}
			else if(!stricmp(fmt, "*l"))
			{
				char buf[10240];
				if(cfgets(buf, (int)(sizeof(buf)/sizeof(char)), cfp) == NULL)
				{
					lua_pushnil(L);
				}
				else
				{
					//WMC - strip all newlines so this works like the Lua original
					char *pos = buf + strlen(buf);
					while(*--pos == '\r' || *pos == '\n') *pos = '\0';

					lua_pushstring(L, buf);
				}
				num_returned++;
			}
		}
		if(type == LUA_TNUMBER || (type == LUA_TSTRING && strpbrk(fmt, "1234567890")))
		{
			int num = 0;
			if(type == LUA_TSTRING)
				num = atoi(fmt);
			else
				num = fl2i(lua_tonumber(L, i));

			if(num < 1)
			{
				if(cfeof(cfp))
					lua_pushstring(L, "");
				else
					lua_pushnil(L);
			}

			char *buf = (char*)vm_malloc(num+1);
			int total_read = cfread(buf, 1, num, cfp);
			if(total_read)
			{
				buf[total_read] = '\0';
				lua_pushstring(L, buf);
			}
			else
			{
				lua_pushnil(L);
			}
			vm_free(buf);
			num_returned++;
		}
		if(type != LUA_TNUMBER && type != LUA_TSTRING)
			LuaError(L, "Invalid argument passed to file:read");
	}

	return num_returned;
}

ADE_FUNC(seek, l_File, "[string Whence=\"cur\", number Offset=0]",
		 "Changes position of file, or gets location."
		 "Whence can be:"
		 "<li>\"set\" - File start.</li>"
		 "<li>\"cur\" - Current position in file.</li>"
		 "<li>\"end\" - File end.</li></ul>",
		 "number",
		 "new offset, or false or nil on failure")
{
	char *w = NULL;
	int o = 0;

	CFILE *cfp = NULL;
	if(!ade_get_args(L, "o|si", l_File.Get(&cfp), &w, &o))
		return ADE_RETURN_NIL;

	if(!cf_is_valid(cfp))
		return ADE_RETURN_NIL;

	if(!(w == NULL || (!stricmp(w, "cur") && o != 0)))
	{
		int seek_type = CF_SEEK_CUR;
		if(!stricmp(w, "set"))
			seek_type = CF_SEEK_SET;
		else if(!stricmp(w, "cur"))
			seek_type = CF_SEEK_CUR;
		else if(!stricmp(w, "end"))
			seek_type = CF_SEEK_END;
		else
			LuaError(L, "Invalid where argument passed to seek() - '%s'", w);

		if(cfseek(cfp, o, seek_type))
			return ADE_RETURN_FALSE;
	}

	int res = cftell(cfp);
	if(res >= 0)
		return ade_set_args(L, "i", res);
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(write, l_File, "string or number, ...",
		 "Writes a series of Lua strings or numbers to the current file.", "number", "Number of items successfully written.")
{
	CFILE *cfp = NULL;
	if(!ade_get_args(L, "o", l_File.Get(&cfp)))
		return ade_set_error(L, "i", 0);

	if(!cf_is_valid(cfp))
		return ade_set_error(L, "i", 0);

	int l_pos = 2;
	int type = LUA_TNONE;

	int num_successful = 0;
	while((type = lua_type(L, l_pos)) != LUA_TNONE)
	{
		if(type == LUA_TSTRING)
		{
			char *s = (char*)lua_tostring(L, l_pos);
			if(cfwrite(s, sizeof(char), strlen(s), cfp))
				num_successful++;
		}
		else if(type == LUA_TNUMBER)
		{
			double d = lua_tonumber(L, l_pos);
			char buf[32]= {0};
			snprintf(buf, sizeof(buf), LUA_NUMBER_FMT, d);
			if(cfwrite(buf, sizeof(char), strlen(buf), cfp))
				num_successful++;
		}

		l_pos++;
	}

	return ade_set_args(L, "i", num_successful);
}

//**********HANDLE: Font
ade_obj<int> l_Font("font", "font handle");

ADE_FUNC(__tostring, l_Font, NULL, "Filename of font", "string", "Font filename, or an empty string if the handle is invalid")
{
	int font = -1;
	if(!ade_get_args(L, "o", l_Font.Get(&font)))
		return ade_set_error(L, "s", "");

	if(font < 0 || font >= Num_fonts)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Fonts[font].filename);
}

ADE_VIRTVAR(Filename, l_Font, "string", "Filename of font (including extension)", "string", NULL)
{
	int font = -1;
	char *newname = NULL;
	if(!ade_get_args(L, "o|s", l_Font.Get(&font), &newname))
		return ade_set_error(L, "s", "");

	if(font < 0 || font >= Num_fonts)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR) {
		strncpy(Fonts[font].filename, newname, sizeof(Fonts[font].filename)-1);
	}

	return ade_set_args(L, "s", Fonts[font].filename);
}

ADE_VIRTVAR(Height, l_Font, "number", "Height of font (in pixels)", "number", "Font height, or 0 if the handle is invalid")
{
	int font = -1;
	int newheight = -1;
	if(!ade_get_args(L, "o|i", l_Font.Get(&font), &newheight))
		return ade_set_error(L, "i", 0);

	if(font < 0 || font >= Num_fonts)
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR && newheight > 0) {
		Fonts[font].h = newheight;
	}

	return ade_set_args(L, "i", Fonts[font].h);
}

ADE_FUNC(isValid, l_Font, NULL, "True if valid, false or nil if not", "boolean", "Detects whether handle is valid")
{
	int font;
	if(!ade_get_args(L, "o", l_Font.Get(&font)))
		return ADE_RETURN_NIL;

	if(font < 0 || font >= Num_fonts)
		return ADE_RETURN_FALSE;
	else
		return ADE_RETURN_TRUE;
}

//**********HANDLE: gameevent
class gameevent_h
{
private:
	int edx;
public:
	gameevent_h(){edx=-1;}
	gameevent_h(int n_event){edx=n_event;}

	bool IsValid(){return (this != NULL && edx > -1 && edx < Num_gs_event_text);}

	int Get(){return edx;}
};

ade_obj<gameevent_h> l_GameEvent("gameevent", "Game event");

ADE_FUNC(__tostring, l_GameEvent, NULL, "Game event name", "string", "Game event name, or empty string if handle is invalid")
{
	gameevent_h *gh = NULL;
	if(!ade_get_args(L, "o", l_GameEvent.GetPtr(&gh)))
		return ade_set_error(L, "s", "");

	if(!gh->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", GS_event_text[gh->Get()]);
}

ADE_VIRTVAR(Name, l_GameEvent, "string", "Game event name", "string", "Game event name, or empty string if handle is invalid")
{
	gameevent_h *gh = NULL;
	char *n_name = NULL;
	if(!ade_get_args(L, "o|s", l_GameEvent.GetPtr(&gh), &n_name))
		return ade_set_error(L, "s", "");

	if(!gh->IsValid())
		return ade_set_error(L, "s", "");

	int edx = gh->Get();

	if(ADE_SETTING_VAR)
	{
		Error(LOCATION, "Can't set game event names at this time");
	}

	return ade_set_args(L, "s", GS_event_text[edx]);
}

//**********HANDLE: gamestate
class gamestate_h
{
private:
	int sdx;
public:
	gamestate_h(){sdx=-1;}
	gamestate_h(int n_state){sdx=n_state;}

	bool IsValid(){return (this != NULL && sdx > -1 && sdx < Num_gs_state_text);}

	int Get(){return sdx;}
};

ade_obj<gamestate_h> l_GameState("gamestate", "Game state");

ADE_FUNC(__tostring, l_GameState, NULL, "Game state name", "string", "Game state name, or empty string if handle is invalid")
{
	gamestate_h *gh = NULL;
	if(!ade_get_args(L, "o", l_GameState.GetPtr(&gh)))
		return ade_set_error(L, "s", "");

	if(!gh->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", GS_state_text[gh->Get()]);
}

ADE_VIRTVAR(Name, l_GameState,"string", "Game state name", "string", "Game state name, or empty string if handle is invalid")
{
	gamestate_h *gh = NULL;
	char *n_name = NULL;
	if(!ade_get_args(L, "o|s", l_GameState.GetPtr(&gh), &n_name))
		return ade_set_error(L, "s", "");

	if(!gh->IsValid())
		return ade_set_error(L, "s", "");

	int sdx = gh->Get();

	if(ADE_SETTING_VAR)
	{
		Error(LOCATION, "Can't set game state names at this time");
	}

	return ade_set_args(L, "s", GS_state_text[sdx]);
}

//**********HANDLE: model
class model_h
{
protected:
	int mid;
	polymodel *model;

public:
	polymodel *Get(){
		if(!this->IsValid())
			return NULL; 

		return model;
	}

	int GetID(){
		if(!this->IsValid())
			return -1;

		return mid;
	}

	bool IsValid(){
		return (this != NULL && model != NULL && mid > -1 && model_get(mid) == model);
	}

	model_h(int n_modelnum) {
		mid = n_modelnum;
		if(mid > 0)
			model = model_get(mid);
		else
			model = NULL;
	}
	model_h(polymodel *n_model) {
		model = n_model;
		if(n_model != NULL)
			mid = n_model->id;
		else
			mid = -1;
	}
	model_h(){
		mid=-1;
		model = NULL;
	}
};

class modeltextures_h : public model_h
{
public:
	modeltextures_h(polymodel *pm) : model_h(pm){}
	modeltextures_h() : model_h(){}
};
ade_obj<model_h> l_Model("model", "3D Model (POF) handle");

ade_obj<modeltextures_h> l_ModelTextures("modeltextures_h", "Array of materials");

ADE_VIRTVAR(Textures, l_Model, "modeltextures", "Model textures", "modeltextures", "Model textures, or an invalid modeltextures handle if the model handle is invalid")
{
	model_h *mdl = NULL;
	modeltextures_h *oth = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_ModelTextures.GetPtr(&oth)))
		return ade_set_error(L, "o", l_ModelTextures.Set(modeltextures_h()));

	polymodel *pm = mdl->Get();
	if(pm == NULL)
		return ade_set_error(L, "o", l_ModelTextures.Set(modeltextures_h()));

	if(ADE_SETTING_VAR && oth->IsValid()) {
		//WMC TODO: Copy code
		LuaError(L, "Attempt to use Incomplete Feature: Modeltextures copy");
	}

	return ade_set_args(L, "o", l_ModelTextures.Set(modeltextures_h(pm)));
}

extern void model_calc_bound_box( vec3d *box, vec3d *big_mn, vec3d *big_mx);

ADE_VIRTVAR(BoundingBoxMax, l_Model, "vector", "Model bounding box maximum", "vector", "Model bounding box, or an empty vector if the handle is invalid")
{
	model_h *mdl = NULL;
	vec3d *v = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v != NULL) {
		pm->maxs = *v;
		
		//Recalculate this, so it stays valid
		model_calc_bound_box(pm->bounding_box, &pm->mins, &pm->maxs);
	}

	return ade_set_args(L, "o", l_Vector.Set(pm->maxs));
}

ADE_VIRTVAR(BoundingBoxMin, l_Model, "vector", "Model bounding box minimum", "vector", "Model bounding box, or an empty vector if the handle is invalid")
{
	model_h *mdl = NULL;
	vec3d *v = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v != NULL) {
		pm->mins = *v;
		
		//Recalculate this, so it stays valid
		model_calc_bound_box(pm->bounding_box, &pm->mins, &pm->maxs);
	}

	return ade_set_args(L, "o", l_Vector.Set(pm->mins));
}

ADE_VIRTVAR(Filename, l_Model, "string", "Model filename", "string", "Model filename, or an empty string if the handle is invalid")
{
	model_h *mdl = NULL;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Model.GetPtr(&mdl), &s))
		return ade_set_error(L, "s", "");

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR) {
		strncpy(pm->filename, s, sizeof(pm->filename) - sizeof(char));
	}

	return ade_set_args(L, "s", pm->filename);
}

ADE_VIRTVAR(Mass, l_Model, "number", "Model radius (Used for collision and culling detection)", "number", "Model radius, or 0 if the model handle is invalid")
{
	model_h *mdl = NULL;
	float nm = 0.0f;
	if(!ade_get_args(L, "o|s", l_Model.GetPtr(&mdl), &nm))
		return ade_set_error(L, "f", 0.0f);

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pm->mass = nm;
	}

	return ade_set_args(L, "f", pm->mass);
}

ADE_VIRTVAR(MomentOfInertia, l_Model, "orientation", "Model moment of inertia", "orientation", NULL)
{
	model_h *mdl = NULL;
	matrix_h *mh = NULL;
	if(!ade_get_args(L, "o|s", l_Model.GetPtr(&mdl), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	matrix *mtx = mh->GetMatrix();
	if(ADE_SETTING_VAR) {
		memcpy(&pm->moment_of_inertia, mtx, sizeof(*mtx));
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&pm->moment_of_inertia)));
}

ADE_VIRTVAR(Radius, l_Model, "number", "Model radius (Used for collision & culling detection)", "number", NULL)
{
	model_h *mdl = NULL;
	float nr = 0.0f;
	if(!ade_get_args(L, "o|s", l_Model.GetPtr(&mdl), &nr))
		return ade_set_error(L, "f", 0.0f);

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pm->rad = nr;
	}

	return ade_set_args(L, "f", pm->rad);
}

ADE_FUNC(isValid, l_Model, NULL, "True if valid, false or nil if not", "boolean", "Detects whether handle is valid")
{
	model_h *mdl;
	if(!ade_get_args(L, "o", l_Model.GetPtr(&mdl)))
		return ADE_RETURN_NIL;

	return mdl->IsValid();
}

//**********HANDLE: order
struct order_h
{
	object_h objh;
	int odx;
	int sig;

	order_h() {
		objh = object_h();
		odx = -1;
		sig = -1;
	}

	order_h(object *objp, int n_odx)
	{
		objh = object_h(objp);
		if(objh.IsValid() && objh.objp->type == OBJ_SHIP && odx > -1 && odx < MAX_AI_GOALS)
		{
			odx = n_odx;
			sig = Ai_info[Ships[objh.objp->instance].ai_index].goals[odx].signature;
		}
		else
		{
			odx = -1;
			sig = -1;
		}
	}

	bool IsValid()
	{
		return (this != NULL && objh.IsValid() && objh.objp->type == OBJ_SHIP && odx > -1 && odx < MAX_AI_GOALS && sig == Ai_info[Ships[objh.objp->instance].ai_index].goals[odx].signature);
	}
};

ade_obj<order_h> l_Order("order", "order handle");

ADE_FUNC(remove, l_Order, NULL, "Removes the given order from the ship's priority queue.", "boolean", "True if order was successfully removed, otherwise false or nil.")
{
	order_h *ohp = NULL;
	if(!ade_get_args(L, "o", l_Order.GetPtr(&ohp)))
		return ADE_RETURN_NIL;

	if(!ohp->IsValid())
		return ADE_RETURN_FALSE;

	ai_info *aip = &Ai_info[Ships[ohp->objh.objp->instance].ai_index];

	ai_remove_ship_goal(aip, ohp->odx);

	return ADE_RETURN_TRUE;
}

//**********HANDLE: shiporders
ade_obj<object_h> l_ShipOrders("shiporders", "Ship orders");

ADE_FUNC(__len, l_ShipOrders, NULL, "Number of textures on ship", "number", "Number of textures on ship, or 0 if handle is invalid")
{
	object_h *objh = NULL;
	if(!ade_get_args(L, "o", l_ShipOrders.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid() || objh->objp->type != OBJ_SHIP || Ships[objh->objp->instance].ai_index < 0)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", ai_goal_num(&Ai_info[Ships[objh->objp->instance].ai_index].goals[0]));
}

ADE_INDEXER(l_ShipOrders, "number Index/string TextureFilename", "Array of ship orders", "order", "Order, or invalid texture handle on failure")
{
	object_h *objh = NULL;
	char *s = NULL;
	order_h *oh = NULL;
	int i; 

	if (!ade_get_args(L, "os|o", l_ShipOrders.GetPtr(&objh), &s, l_Order.GetPtr(&oh)))
		return ade_set_error(L, "o", l_Order.Set(order_h()));

	if (!objh->IsValid() || s==NULL)
		return ade_set_error(L, "o", l_Order.Set(order_h()));

	ship *shipp = &Ships[objh->objp->instance];
	ai_info *aip = &Ai_info[Ships[objh->objp->instance].ai_index];

	//Determine index
	int idx = atoi(s) - 1;	//Lua->FS2

	if (idx < 0 || idx >= MAX_AI_GOALS)
		return ade_set_error(L, "o", l_Order.Set(order_h()));

	int num = 0;
	for(i = 0; i < MAX_AI_GOALS; i++)
	{
		if(aip->goals[i].ai_mode != AI_GOAL_NONE)
		{
			if(idx == num)
				break;

			num++;
		}
	}

	if(i >= MAX_AI_GOALS)
		return ade_set_error(L, "o", l_Order.Set(order_h()));

	//LuaError(L, "%d: %d", lua_type(L,lua_upvalueindex(2)), lua_toboolean(L,lua_upvalueindex(2)));
	if (ADE_SETTING_VAR)
	{
		if(!oh->IsValid())
		{
			ai_remove_ship_goal(aip, i);
		}
		else
		{
			aip->goals[i] = Ai_info[Ships[oh->objh.objp->instance].ai_index].goals[oh->odx];
		}
	}

	return ade_set_args(L, "o", l_Order.Set(order_h(objh->objp, i)));
}

ADE_FUNC(isValid, l_ShipOrders, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_ShipOrders.GetPtr(&oh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", oh->IsValid());
}

//**********HANDLE: physics
struct physics_info_h
{
	object_h objh;
	physics_info *pi;

	physics_info_h() {
		objh = object_h();
		pi = NULL;
	}

	physics_info_h(object *objp) {
		objh = object_h(objp);
		pi = &objp->phys_info;
	}

	physics_info_h(physics_info *in_pi) {
		pi = in_pi;
	}

	bool IsValid(){if(objh.objp != NULL) return objh.IsValid(); else return (pi != NULL);}
};

ade_obj<physics_info_h> l_Physics("physics", "Physics handle");

ADE_VIRTVAR(AfterburnerAccelerationTime, l_Physics, "number", "Afterburner acceleration time", "number", "Afterburner acceleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->afterburner_forward_accel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->afterburner_forward_accel_time_const);
}

ADE_VIRTVAR(AfterburnerVelocityMax, l_Physics, "vector", "Afterburner max velocity (Local vector)", "vector", "Afterburner max velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->afterburner_max_vel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->afterburner_max_vel));
}

ADE_VIRTVAR(BankingConstant, l_Physics, "number", "Banking constant", "number", "Banking constant, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->delta_bank_const = f;
	}

	return ade_set_args(L, "f", pih->pi->delta_bank_const);
}

ADE_VIRTVAR(ForwardAccelerationTime, l_Physics, "number", "Forward acceleration time", "number", "Forward acceleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->forward_accel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->forward_accel_time_const);
}

ADE_VIRTVAR(ForwardDecelerationTime, l_Physics, "number", "Forward deceleration time", "number", "Forward decleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->forward_decel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->forward_decel_time_const);
}

ADE_VIRTVAR(ForwardThrust, l_Physics, "number", "Forward thrust amount (0-1), used primarily for thruster graphics", "number", "Forward thrust, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->forward_thrust = f;
	}

	return ade_set_args(L, "f", pih->pi->forward_thrust);
}

ADE_VIRTVAR(Mass, l_Physics, "number", "Object mass", "number", "Object mass, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->mass = f;
	}

	return ade_set_args(L, "f", pih->pi->mass);
}

ADE_VIRTVAR(RotationalVelocity, l_Physics, "vector", "Rotational velocity (Local vector)", "vector", "Rotational velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->rotvel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->rotvel));
}

ADE_VIRTVAR(RotationalVelocityDamping, l_Physics, "number", "Rotational damping, ie derivative of rotational speed", "number", "Rotational damping, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->rotdamp = f;
	}

	return ade_set_args(L, "f", pih->pi->rotdamp);
}

ADE_VIRTVAR(RotationalVelocityDesired, l_Physics, "lvector", "Desired rotational velocity", "number", "Desired rotational velocity, or 0 if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->desired_rotvel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->desired_rotvel));
}

ADE_VIRTVAR(RotationalVelocityMax, l_Physics, "vector", "Maximum rotational velocity (Local vector)", "vector", "Maximum rotational velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->max_rotvel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->max_rotvel));
}

ADE_VIRTVAR(ShockwaveShakeAmplitude, l_Physics, "number", "How much shaking from shockwaves is applied to object", "number", "Shockwave shake amplitude, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->shockwave_shake_amp = f;
	}

	return ade_set_args(L, "f", pih->pi->shockwave_shake_amp);
}

ADE_VIRTVAR(SideThrust, l_Physics, "number", "Side thrust amount (0-1), used primarily for thruster graphics", "number", "Side thrust amount, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->side_thrust = f;
	}

	return ade_set_args(L, "f", pih->pi->side_thrust);
}

ADE_VIRTVAR(SlideAccelerationTime, l_Physics, "number", "Time to accelerate to maximum slide velocity", "number", "Sliding acceleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->slide_accel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->slide_accel_time_const);
}

ADE_VIRTVAR(SlideDecelerationTime, l_Physics, "number", "Time to decelerate from maximum slide speed", "number", "Sliding deceleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->slide_decel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->slide_decel_time_const);
}

ADE_VIRTVAR(Velocity, l_Physics, "vector", "Object world velocity (World vector)", "vector", "Object velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->vel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->vel));
}

ADE_VIRTVAR(VelocityDesired, l_Physics, "vector", "Desired velocity (World vector)", "vector", "Desired velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->desired_vel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->desired_vel));
}

ADE_VIRTVAR(VelocityMax, l_Physics, "vector", "Object max local velocity (Local vector)", "vector", "Maximum velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->max_vel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->max_vel));
}

ADE_VIRTVAR(VerticalThrust, l_Physics, "number", "Vertical thrust amount (0-1), used primarily for thruster graphics", "number", "Vertical thrust amount, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->vert_thrust = f;
	}

	return ade_set_args(L, "f", pih->pi->vert_thrust);
}

ADE_FUNC(isValid, l_Physics, NULL, "True if valid, false or nil if not", "boolean", "Detects whether handle is valid")
{
	physics_info_h *pih;
	if(!ade_get_args(L, "o", l_Physics.GetPtr(&pih)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", pih->IsValid());
}

ADE_FUNC(getSpeed, l_Physics, NULL, "Gets total speed as of last frame", "number", "Total speed, or 0 if handle is invalid")
{
	physics_info_h *pih;
	if(!ade_get_args(L, "o", l_Physics.GetPtr(&pih)))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", pih->pi->speed);
}

ADE_FUNC(getForwardSpeed, l_Physics, NULL, "Gets total speed in the ship's 'forward' direction as of last frame", "number", "Total forward speed, or 0 if handle is invalid")
{
	physics_info_h *pih;
	if(!ade_get_args(L, "o", l_Physics.GetPtr(&pih)))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", pih->pi->fspeed);
}

//**********HANDLE: sexpvariable
struct sexpvar_h
{
	int idx;
	char variable_name[TOKEN_LENGTH];

	sexpvar_h(){idx=-1;variable_name[0]='\0';}
	sexpvar_h(int n_idx){idx = n_idx; strcpy(variable_name, Sexp_variables[n_idx].variable_name);}
	bool IsValid(){
		return (idx > -1
				&& idx < MAX_SEXP_VARIABLES
				&& (Sexp_variables[idx].type & SEXP_VARIABLE_SET)
				&& !strcmp(Sexp_variables[idx].variable_name, variable_name));}
};

ade_obj<sexpvar_h> l_SEXPVariable("sexpvariable", "SEXP Variable handle");

ADE_VIRTVAR(Persistence, l_SEXPVariable, "enumeration", "SEXP Variable persistance, uses SEXPVAR_*_PERSISTENT enumerations", "enumeration", "SEXPVAR_*_PERSISTENT enumeration, or invalid numeration if handle is invalid")
{
	sexpvar_h *svh = NULL;
	enum_h *type = NULL;
	if(!ade_get_args(L, "o|o", l_SEXPVariable.GetPtr(&svh), l_Enum.GetPtr(&type)))
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	if(!svh->IsValid())
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	sexp_variable *sv = &Sexp_variables[svh->idx];

	if(ADE_SETTING_VAR && type != NULL)
	{
		if(type->index == LE_SEXPVAR_PLAYER_PERSISTENT)
		{
			sv->type &= ~(SEXP_VARIABLE_CAMPAIGN_PERSISTENT);
			sv->type |= SEXP_VARIABLE_PLAYER_PERSISTENT;
		}
		else if(type->index == LE_SEXPVAR_CAMPAIGN_PERSISTENT)
		{
			sv->type |= SEXP_VARIABLE_CAMPAIGN_PERSISTENT;
			sv->type &= ~(SEXP_VARIABLE_PLAYER_PERSISTENT);
		}
		else if(type->index == LE_SEXPVAR_NOT_PERSISTENT)
		{
			sv->type &= ~(SEXP_VARIABLE_CAMPAIGN_PERSISTENT);
			sv->type &= ~(SEXP_VARIABLE_PLAYER_PERSISTENT);
		}
	}

	enum_h ren;
	if(sv->type & SEXP_VARIABLE_PLAYER_PERSISTENT)
		ren.index = LE_SEXPVAR_PLAYER_PERSISTENT;
	else if(sv->type & SEXP_VARIABLE_CAMPAIGN_PERSISTENT)
		ren.index = LE_SEXPVAR_CAMPAIGN_PERSISTENT;
	else
		ren.index = LE_SEXPVAR_NOT_PERSISTENT;

	return ade_set_args(L, "o", l_Enum.Set(ren));
}

ADE_VIRTVAR(Type, l_SEXPVariable, "enumeration", "SEXP Variable type, uses SEXPVAR_TYPE_* enumerations", "enumeration", "SEXPVAR_TYPE_* enumeration, or invalid numeration if handle is invalid")
{
	sexpvar_h *svh = NULL;
	enum_h *type = NULL;
	if(!ade_get_args(L, "o|o", l_SEXPVariable.GetPtr(&svh), l_Enum.GetPtr(&type)))
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	if(!svh->IsValid())
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	sexp_variable *sv = &Sexp_variables[svh->idx];

	if(ADE_SETTING_VAR && type != NULL)
	{
		if(type->index == LE_SEXPVAR_TYPE_NUMBER)
		{
			sv->type &= ~(SEXP_VARIABLE_NUMBER);
			sv->type |= SEXP_VARIABLE_STRING;
		}
		else if(type->index == LE_SEXPVAR_TYPE_STRING)
		{
			sv->type |= SEXP_VARIABLE_NUMBER;
			sv->type &= ~(SEXP_VARIABLE_STRING);
		}
	}

	enum_h ren;
	if(sv->type & SEXP_VARIABLE_NUMBER)
		ren.index = LE_SEXPVAR_TYPE_NUMBER;
	else if(sv->type & SEXP_VARIABLE_STRING)
		ren.index = LE_SEXPVAR_TYPE_STRING;

	return ade_set_args(L, "o", l_Enum.Set(ren));
}

ADE_VIRTVAR(Value, l_SEXPVariable, "number/string", "SEXP variable value", "string", "SEXP variable contents, or nil if the variable is of an invalid type or the handle is invalid")
{
	sexpvar_h *svh = NULL;
	char *newvalue = NULL;
	char number_as_str[TOKEN_LENGTH];

	if(lua_type(L, 2) == LUA_TNUMBER)
	{
		int newnumber = 0;
		if(!ade_get_args(L, "o|i", l_SEXPVariable.GetPtr(&svh), &newnumber))
			return ADE_RETURN_NIL;

		snprintf(number_as_str, sizeof(number_as_str), "%d", newnumber);
		newvalue = number_as_str;
	}
	else
	{
		if(!ade_get_args(L, "o|s", l_SEXPVariable.GetPtr(&svh), &newvalue))
			return ADE_RETURN_NIL;
	}

	if(!svh->IsValid())
		return ADE_RETURN_NIL;

	sexp_variable *sv = &Sexp_variables[svh->idx];

	if(ADE_SETTING_VAR && newvalue)
	{
		sexp_modify_variable(newvalue, svh->idx);
	}

	if(sv->type && SEXP_VARIABLE_NUMBER)
		return ade_set_args(L, "i", atoi(sv->text));
	else if(sv->type && SEXP_VARIABLE_STRING)
		return ade_set_args(L, "s", sv->text);
	else
		return ADE_RETURN_NIL;
}

ADE_FUNC(__tostring, l_SEXPVariable, NULL, "Returns SEXP name", "string", "SEXP name, or empty string if handle is invalid")
{
	sexpvar_h *svh = NULL;
	if(!ade_get_args(L, "o", l_SEXPVariable.GetPtr(&svh)))
		return ade_set_error(L, "s", "");

	if(!svh->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Sexp_variables[svh->idx].variable_name);
}

ADE_FUNC(isValid, l_SEXPVariable, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	sexpvar_h *svh = NULL;
	if(!ade_get_args(L, "o", l_SEXPVariable.GetPtr(&svh)))
		return ADE_RETURN_NIL;

	if(!svh->IsValid())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(delete, l_SEXPVariable, NULL, "Deletes a SEXP Variable", "boolean", "True if successful, false if the handle is invalid")
{
	sexpvar_h *svh = NULL;
	if(!ade_get_args(L, "o", l_SEXPVariable.GetPtr(&svh)))
		return ade_set_error(L, "b", false);

	if(!svh->IsValid())
		return ade_set_error(L, "b", false);

	sexp_variable_delete(svh->idx);

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Shields
ade_obj<object_h> l_Shields("shields", "Shields handle");

ADE_INDEXER(l_Shields, "enumeration/number", "Gets or sets shield quadrant strength. Use \"SHIELD_*\" enumeration or 1-4 for a specific quadrant, or NONE for the entire shield", "number", "Quadrant/shield strength, or 0 if handle is invalid")
{
	object_h *objh;
	float nval = -1.0f;

	object *objp = NULL;
	int qdx = -1;

	if(lua_isstring(L, 2))
	{
		char *qd = NULL;
		if(!ade_get_args(L, "os|f", l_Shields.GetPtr(&objh), &qd, &nval))
			return ade_set_error(L, "f", 0.0f);

		if(!objh->IsValid())
			return ade_set_error(L, "f", 0.0f);

		objp = objh->objp;

		//Which quadrant?
		int qdi;
		if(qd == NULL)
			qdx = -1;
		else if((qdi = atoi(qd)) > 0 && qdi < 5)
			qdx = qdi-1;	//LUA->FS2
		else
			return ade_set_error(L, "f", 0.0f);
	} else {
		enum_h *qd = NULL;
		if(!ade_get_args(L, "oo|f", l_Shields.GetPtr(&objh), l_Enum.GetPtr(&qd), &nval))
			return 0;

		if(!objh->IsValid())
			return ade_set_error(L, "f", 0.0f);

		objp = objh->objp;

		switch(qd->index)
		{
			case LE_NONE:
				qdx = -1;
				break;
			case LE_SHIELD_FRONT:
				qdx = 0;
				break;
			case LE_SHIELD_LEFT:
				qdx = 1;
				break;
			case LE_SHIELD_RIGHT:
				qdx = 2;
				break;
			case LE_SHIELD_BACK:
				qdx = 3;
				break;
			default:
				return ade_set_error(L, "f", 0.0f);
		}
	}

	//Set/get all quadrants
	if(qdx == -1) {
		if(ADE_SETTING_VAR && nval >= 0.0f)
			shield_set_strength(objp, nval);

		return ade_set_args(L, "f", shield_get_strength(objp));
	}

	//Set one quadrant?
	if(ADE_SETTING_VAR && nval >= 0.0f)
		shield_set_quad(objp, qdx, nval);

	//Get one quadrant
	return ade_set_args(L, "f", shield_get_quad(objp, qdx));
}

//WMC - Not sure if I want this to be a variable. It'd make more sense
//as a function, since it modifies all quadrant variables
//WMC - Ehh, screw it.
ADE_VIRTVAR(CombinedLeft, l_Shields, "number", "Total shield hitpoints left (for all quadrants combined)", "number", "Combined shield strength, or 0 if handle is invalid")
{
	object_h *objh;
	float nval = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shields.GetPtr(&objh), &nval))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && nval >= 0.0f) {
		shield_set_strength(objh->objp, nval);
	}

	return ade_set_args(L, "f", shield_get_strength(objh->objp));
}

ADE_VIRTVAR(CombinedMax, l_Shields, "number", "Maximum shield hitpoints (for all quadrants combined)", "number", "Combined maximum shield strength, or 0 if handle is invalid")
{
	object_h *objh;
	float nval = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shields.GetPtr(&objh), &nval))
			return 0;

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && nval >= 0.0f) {
		shield_set_max_strength(objh->objp, nval);
	}

	return ade_set_args(L, "f", shield_get_max_strength(objh->objp));
}

ADE_FUNC(isValid, l_Shields, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_Shields.GetPtr(&oh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", oh->IsValid());
}

//**********HANDLE: Shiptype
ade_obj<int> l_Shiptype("shiptype", "Ship type handle");
extern int Species_initted;

ADE_VIRTVAR(Name, l_Shiptype, "string", "Ship type name", "string", "Ship type name, or empty string if handle is invalid")
{
	if(!Species_initted)
		return ade_set_error(L, "s", "");

	char *s = NULL;
	int idx;
	if(!ade_get_args(L, "o|s", l_Shiptype.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > (int)Ship_types.size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Ship_types[idx].name, s, sizeof(Ship_types[idx].name)-1);
	}

	return ade_set_args(L, "s", Ship_types[idx].name);
}

ADE_FUNC(isValid, l_Shiptype, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shiptype.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= (int)Ship_types.size())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Species
ade_obj<int> l_Species("species", "Species handle");
extern int Species_initted;

ADE_VIRTVAR(Name, l_Species, "string", "Species name", "string", "Species name, or empty string if handle is invalid")
{
	if(!Species_initted)
		return ade_set_error(L, "s", "");

	char *s = NULL;
	int idx;
	if(!ade_get_args(L, "o|s", l_Species.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= (int)Species_info.size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Species_info[idx].species_name, s, sizeof(Species_info[idx].species_name)-1);
	}

	return ade_set_args(L, "s", Species_info[idx].species_name);
}

ADE_FUNC(isValid, l_Species, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Species.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= (int)Species_info.size())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Team
ade_obj<int> l_Team("team", "Team handle");

ADE_FUNC(__eq, l_Team, "team, team", "Checks whether two teams are the same team", "boolean", "true if equal")
{
	int t1, t2;
	if(!ade_get_args(L, "oo", l_Team.Get(&t1), l_Team.Get(&t2)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", (t1 == t2));
}

ADE_VIRTVAR(Name, l_Team, "string", "Team name", "string", "Team name, or empty string if handle is invalid")
{
	int tdx=-1;
	char *s=NULL;
	if(!ade_get_args(L, "o|s", l_Team.Get(&tdx), &s))
		return ade_set_error(L, "s", "");

	if(tdx < 0 || tdx > Num_iffs)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Iff_info[tdx].iff_name, s, NAME_LENGTH-1);
	}

	return ade_set_args(L, "s", Iff_info[tdx].iff_name);
}

ADE_FUNC(isValid, l_Team, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Team.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_iffs)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Texture
ade_obj<int> l_Texture("texture", "Texture handle");
//WMC - int should NEVER EVER be an invalid handle. Return Nil instead. Nil FTW.

static float lua_Opacity = 1.0f;
static int lua_Opacity_type = GR_ALPHABLEND_NONE;

ADE_FUNC(__gc, l_Texture, NULL, "Auto-deletes texture", NULL, NULL)
{
	int idx;

	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx > -1 && bm_is_valid(idx))
		bm_unload(idx);

	return ADE_RETURN_NIL;
}

ADE_FUNC(__eq, l_Texture, "texture, texture", "Checks if two texture handles refer to the same texture", "boolean", "True if textures are equal")
{
	int idx,idx2;

	if(!ade_get_args(L, "oo", l_Texture.Get(&idx), l_Texture.Get(&idx2)))
		return ADE_RETURN_NIL;

	if(idx == idx2)
		return ADE_RETURN_TRUE;

	return ADE_RETURN_FALSE;
}

ADE_INDEXER(l_Texture, "number",
			"Returns texture handle to specified frame number in current texture's animation."
			"This means that [1] will always return the first frame in an animation, no matter what frame an animation is."
			"You cannot change a texture animation frame.",
			"texture",
			"Texture handle, or invalid texture handle if index is invalid")
{
	int idx;
	int frame=-1;
	int newframe=-1;	//WMC - Ignore for now
	if(!ade_get_args(L, "oi|i", l_Texture.Get(&idx), &frame, &newframe))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(frame < 1)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	//Get me some info
	int num=-1;
	int first=-1;
	first = bm_get_info(idx, NULL, NULL, NULL, &num);

	//Check it's a valid one
	if(first < 0 || frame > num)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	frame--; //Lua->FS2

	//Get actual texture handle
	frame = first + frame;

	return ade_set_args(L, "o", l_Texture.Set(frame));
}

ADE_FUNC(isValid, l_Texture, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", bm_is_valid(idx));
}

ADE_FUNC(unload, l_Texture, NULL, "Unloads a texture from memory", NULL, NULL)
{
	int *idx;

	if(!ade_get_args(L, "o", l_Texture.GetPtr(&idx)))
		return ADE_RETURN_NIL;

	if(!bm_is_valid(*idx))
		return ADE_RETURN_NIL;

	bm_unload(*idx);

	//WMC - invalidate this handle
	*idx = -1;

	return ADE_RETURN_NIL;
}

ADE_FUNC(getFilename, l_Texture, NULL, "Returns filename for texture", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ade_set_error(L, "s", "");

	if(!bm_is_valid(idx))
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", bm_get_filename(idx));
}

ADE_FUNC(getWidth, l_Texture, NULL, "Gets texture width", "number", "Texture width, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ade_set_error(L, "i", 0);

	if(!bm_is_valid(idx))
		return ade_set_error(L, "i", 0);

	int w = -1;

	if(bm_get_info(idx, &w) < 0)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", w);
}

ADE_FUNC(getHeight, l_Texture, NULL, "Gets texture height", "number", "Texture height, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ade_set_error(L, "i", 0);

	if(!bm_is_valid(idx))
		return ade_set_error(L, "i", 0);

	int h=-1;

	if(bm_get_info(idx, NULL, &h) < 0)
		return ade_set_error(L, "i", 0);
	
	return ade_set_args(L, "i", h);
}

ADE_FUNC(getFPS, l_Texture, NULL,"Gets frames-per-second of texture", "number", "Texture FPS, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ade_set_error(L, "i", 0);

	if(!bm_is_valid(idx))
		return ade_set_error(L, "i", 0);

	int fps=-1;

	if(bm_get_info(idx, NULL, NULL, NULL, NULL, &fps) < 0)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", fps);
}

ADE_FUNC(getFramesLeft, l_Texture, NULL, "Gets number of frames left, from handle's position in animation", "number", "Frames left, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ADE_RETURN_NIL;

	if(!bm_is_valid(idx))
		return ADE_RETURN_NIL;

	int num=-1;

	if(bm_get_info(idx, NULL, NULL, NULL, &num) < 0)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", num);
}

//**********OBJECT: vector
//WMC - see matrix for ade_obj def

ADE_INDEXER(l_Vector, "x,y,z or 1-3", "Vector component", "number", "Value at index, or 0 if vector handle is invalid")
{
	vec3d *v3;
	char *s = NULL;
	float newval = 0.0f;
	int numargs = ade_get_args(L, "os|f", l_Vector.GetPtr(&v3), &s, &newval);

	if(!numargs || s[1] != '\0')
		return ade_set_error(L, "f", 0.0f);

	int idx=-1;
	if(s[0]=='x' || s[0] == '1')
		idx = 0;
	else if(s[0]=='y' || s[0] == '2')
		idx = 1;
	else if(s[0]=='z' || s[0] == '3')
		idx = 2;

	if(idx < 0 || idx > 3)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		v3->a1d[idx] = newval;
	}

	return ade_set_args(L, "f", v3->a1d[idx]);
}

ADE_FUNC(__add, l_Vector, "number/vector", "Adds vector by another vector, or adds all axes by value", "vector", "Final vector, or null vector if error occurs")
{
	vec3d v3 = vmd_zero_vector;
	if(lua_isnumber(L, 1) || lua_isnumber(L, 2))
	{
		float f;
		if(lua_isnumber(L, 1) && ade_get_args(L, "fo", &f, l_Vector.Get(&v3))
			|| lua_isnumber(L, 2) && ade_get_args(L, "of", l_Vector.Get(&v3), &f))
		{
			v3.xyz.x += f;
			v3.xyz.y += f;
			v3.xyz.z += f;
		}
	}
	else
	{
		vec3d v3b;
		//WMC - doesn't really matter which is which
		if(ade_get_args(L, "oo", l_Vector.Get(&v3), l_Vector.Get(&v3b)))
		{
			vm_vec_add2(&v3, &v3b);
		}
	}
	return ade_set_args(L, "o", l_Vector.Set(v3));
}

ADE_FUNC(__sub, l_Vector, "number/vector", "Subtracts vector from another vector, or subtracts all axes by value", "vector", "Final vector, or null vector if error occurs")
{
	vec3d v3 = vmd_zero_vector;
	if(lua_isnumber(L, 1) || lua_isnumber(L, 2))
	{
		float f;
		if(lua_isnumber(L, 1) && ade_get_args(L, "fo", &f, l_Vector.Get(&v3))
			|| lua_isnumber(L, 2) && ade_get_args(L, "of", l_Vector.Get(&v3), &f))
		{
			v3.xyz.x += f;
			v3.xyz.y += f;
			v3.xyz.z += f;
		}
	}
	else
	{
		vec3d v3b;
		//WMC - doesn't really matter which is which
		if(ade_get_args(L, "oo", l_Vector.Get(&v3), l_Vector.Get(&v3b)))
		{
			vm_vec_sub2(&v3, &v3b);
		}
	}

	return ade_set_args(L, "o", l_Vector.Set(v3));
}

ADE_FUNC(__mul, l_Vector, "number/vector", "Scales vector object (Multiplies all axes by number), or multiplies each axes by the other vector's axes.", "vector", "Final vector, or null vector if error occurs")
{
	vec3d v3 = vmd_zero_vector;
	if(lua_isnumber(L, 1) || lua_isnumber(L, 2))
	{
		float f;
		if(lua_isnumber(L, 1) && ade_get_args(L, "fo", &f, l_Vector.Get(&v3))
			|| lua_isnumber(L, 2) && ade_get_args(L, "of", l_Vector.Get(&v3), &f))
		{
			vm_vec_scale(&v3, f);
		}
	}
	else
	{
		vec3d *v1 = NULL;
		vec3d *v2 = NULL;
		if(!ade_get_args(L, "oo", l_Vector.GetPtr(&v1), l_Vector.GetPtr(&v2)))
			return ade_set_args(L, "o", l_Vector.Set(vmd_zero_vector));

		v3.xyz.x = v1->xyz.x * v2->xyz.x;
		v3.xyz.y = v1->xyz.y * v2->xyz.y;
		v3.xyz.z = v1->xyz.z * v2->xyz.z;
	}

	return ade_set_args(L, "o", l_Vector.Set(v3));
}

ADE_FUNC(__div, l_Vector, "number/vector", "Scales vector object (Divide all axes by number), or divides each axes by the dividing vector's axes.", "vector", "Final vector, or null vector if error occurs")
{
	vec3d v3 = vmd_zero_vector;
	if(lua_isnumber(L, 1) || lua_isnumber(L, 2))
	{
		float f;
		if(lua_isnumber(L, 1) && ade_get_args(L, "fo", &f, l_Vector.Get(&v3))
			|| lua_isnumber(L, 2) && ade_get_args(L, "of", l_Vector.Get(&v3), &f))
		{
			vm_vec_scale(&v3, 1.0f/f);
		}
	}
	else
	{
		vec3d *v1 = NULL;
		vec3d *v2 = NULL;
		if(!ade_get_args(L, "oo", l_Vector.GetPtr(&v1), l_Vector.GetPtr(&v2)))
			return ade_set_args(L, "o", l_Vector.Set(vmd_zero_vector));

		v3.xyz.x = v1->xyz.x / v2->xyz.x;
		v3.xyz.y = v1->xyz.y / v2->xyz.y;
		v3.xyz.z = v1->xyz.z / v2->xyz.z;
	}

	return ade_set_args(L, "o", l_Vector.Set(v3));
}


ADE_FUNC(__tostring, l_Vector, NULL, "Converts a vector to string with format \"(x,y,z)\"", "string", "Vector as string, or empty string if handle is invalid")
{
	vec3d *v3;
	if(!ade_get_args(L, "o", l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "s", "");

	char buf[128];
	snprintf(buf, sizeof(buf), "(%f,%f,%f)", v3->xyz.x, v3->xyz.y, v3->xyz.z);

	return ade_set_args(L, "s", buf);
}

ADE_FUNC(getOrientation, l_Vector, NULL,
		 "Returns orientation object representing the direction of the vector. Does not require vector to be normalized.",
		 "orientation",
		 "Orientation object, or null orientation object if handle is invalid")
{
	vec3d v3;
	if(!ade_get_args(L, "o", l_Vector.Get(&v3)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	matrix mt = vmd_identity_matrix;

	vm_vec_normalize_safe(&v3);
	vm_vector_2_matrix_norm(&mt, &v3);
	matrix_h mh(&mt);
	
	return ade_set_args(L, "o", l_Matrix.Set(mh));
}

ADE_FUNC(getMagnitude, l_Vector, NULL, "Returns the magnitude of a vector (Total regardless of direction)", "number", "Magnitude of vector, or 0 if handle is invalid")
{
	vec3d *v3;
	if(!ade_get_args(L, "o", l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", vm_vec_mag(v3));
}

ADE_FUNC(getDistance, l_Vector, "Vector", "Distance", "number", "Returns distance from another vector")
{
	vec3d *v3a, *v3b;
	if(!ade_get_args(L, "oo", l_Vector.GetPtr(&v3a), l_Vector.GetPtr(&v3b)))
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f",vm_vec_dist(v3a, v3b));
}

ADE_FUNC(getDotProduct, l_Vector, "vector OtherVector", "Returns dot product of vector object with vector argument", "number", "Dot product, or 0 if a handle is invalid")
{
	vec3d *v3a, *v3b;
	if(!ade_get_args(L, "oo", l_Vector.GetPtr(&v3a), l_Vector.GetPtr(&v3b)))
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", vm_vec_dotprod(v3a, v3b));
}

ADE_FUNC(getCrossProduct, l_Vector, "vector OtherVector", "Returns cross product of vector object with vector argument", "vector", "Cross product, or null vector if a handle is invalid")
{
	vec3d *v3a, *v3b;
	if(!ade_get_args(L, "oo", l_Vector.GetPtr(&v3a), l_Vector.GetPtr(&v3b)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	vec3d v3r;
	vm_vec_crossprod(&v3r, v3a, v3b);

	return ade_set_args(L, "o",l_Vector.Set(v3r));
}

ADE_FUNC(getScreenCoords, l_Vector, NULL, "Gets screen cordinates of a world vector", "number,number", "X (number), Y (number), or false if off-screen")
{
	vec3d v3;
	if(!ade_get_args(L, "o", l_Vector.Get(&v3)))
		return ADE_RETURN_NIL;

	vertex vtx;
	bool do_g3 = G3_count < 1;
	if(do_g3)
		g3_start_frame(1);
	
	g3_rotate_vertex(&vtx,&v3);
	g3_project_vertex(&vtx);

	if(do_g3)
		g3_end_frame();

	if(vtx.flags & PF_OVERFLOW)
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "ff", vtx.sx, vtx.sy);
}

//**********HANDLE: material
static const int THT_INDEPENDENT	= 0;
static const int THT_OBJECT			= 1;
static const int THT_MODEL			= 2;
class texture_map_h
{
protected:
	int type;
	object_h obj;
	model_h mdl;

	texture_map *tmap;	//Pointer to subsystem, or NULL for the hull

public:
	texture_map_h(){
		type = THT_INDEPENDENT;
		tmap = NULL;
	}

	texture_map_h(object *objp, texture_map *n_tmap = NULL) {
		type = THT_OBJECT;
		obj = object_h(objp);
		tmap = n_tmap;
	}

	texture_map_h(int modelnum, texture_map *n_tmap = NULL) {
		type = THT_MODEL;
		mdl = model_h(modelnum);
		tmap = n_tmap;
	}

	texture_map_h(polymodel *n_model, texture_map *n_tmap = NULL) {
		type = THT_MODEL;
		mdl = model_h(n_model);
		tmap = n_tmap;
	}

	texture_map *Get()
	{
		if(!this->IsValid())
			return NULL;

		return tmap;
	}

	int GetSize()
	{
		if(!this->IsValid())
			return 0;

		switch(type)
		{
			case THT_MODEL:
				return mdl.Get()->n_textures;
			case THT_OBJECT:
				return 0;	//Can't do this right now.
			default:
				return 0;
		}
	}

	bool IsValid() {
		if(tmap == NULL)
			return false;

		switch(type)
		{
			case THT_INDEPENDENT:
				return true;
			case THT_OBJECT:
				return obj.IsValid();
			case THT_MODEL:
				return mdl.IsValid();
			default:
				Error(LOCATION, "Bad type in texture_map_h; debug this.");
				return false;
		}
	}
};
ade_obj<texture_map_h> l_TextureMap("material", "Texture map, including diffuse, glow, and specular textures");

ADE_VIRTVAR(BaseMap, l_TextureMap, "texture", "Base texture", "texture", "Base texture, or invalid texture handle if material handle is invalid")
{
	texture_map_h *tmh = NULL;
	int new_tex = -1;
	if(!ade_get_args(L, "o|o", l_TextureMap.GetPtr(&tmh), l_Texture.Get(&new_tex)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	texture_map *tmap = tmh->Get();
	if(tmap == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(ADE_SETTING_VAR && new_tex > -1) {
		tmap->textures[TM_BASE_TYPE].SetTexture(new_tex);
	}

	return ade_set_args(L, "o", l_Texture.Set(tmap->textures[TM_BASE_TYPE].GetTexture()));
}

ADE_VIRTVAR(GlowMap, l_TextureMap, "texture", "Glow texture", "texture", "Glow texture, or invalid texture handle if material handle is invalid")
{
	texture_map_h *tmh = NULL;
	int new_tex = -1;
	if(!ade_get_args(L, "o|o", l_TextureMap.GetPtr(&tmh), l_Texture.Get(&new_tex)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	texture_map *tmap = tmh->Get();
	if(tmap == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(ADE_SETTING_VAR && new_tex > -1) {
		tmap->textures[TM_GLOW_TYPE].SetTexture(new_tex);
	}

	return ade_set_args(L, "o", l_Texture.Set(tmap->textures[TM_GLOW_TYPE].GetTexture()));
}

ADE_VIRTVAR(SpecularMap, l_TextureMap, "texture", "Specular texture", "texture", "Texture handle, or invalid texture handle if material handle is invalid")
{
	texture_map_h *tmh = NULL;
	int new_tex = -1;
	if(!ade_get_args(L, "o|o", l_TextureMap.GetPtr(&tmh), l_Texture.Get(&new_tex)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	texture_map *tmap = tmh->Get();
	if(tmap == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(ADE_SETTING_VAR && new_tex > -1) {
		tmap->textures[TM_SPECULAR_TYPE].SetTexture(new_tex);
	}

	return ade_set_args(L, "o", l_Texture.Set(tmap->textures[TM_SPECULAR_TYPE].GetTexture()));
}

//**********HANDLE: directives
/*
ade_obj<bool> l_Directives("directives", "Mission directives handle");

ADE_INDEXER(l_Directives, "Directive number", "directive handle", NULL)
{
	bool b;
	int idx;
	if(!ade_get_args(L, "o|i", l_Directives.Get(&b), &idx))
		return ADE_RETURN_NIL;

	if(idx < 1 || idx > Num_mission_events)
		return ADE_RETURN_FALSE;

	idx--;	//Lua->FS2

	return ade_set_args(L, "o", l_Event.Set(idx));
}
*/

//**********HANDLE: Weaponclass
ade_obj<int> l_Weaponclass("weaponclass", "Weapon class handle");

ADE_FUNC(__tostring, l_Weaponclass, NULL, "Weapon class name", "string", "Weapon class name, or an empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Weapon_info[idx].name);
}

ADE_VIRTVAR(Name, l_Weaponclass, "string", "Weapon class name", "string", "Weapon class name, or empty string if handle is invalid")

{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Weapon_info[idx].name, s, sizeof(Weapon_info[idx].name)-1);
	}

	return ade_set_args(L, "s", Weapon_info[idx].name);
}

ADE_VIRTVAR(Title, l_Weaponclass, "string", "Weapon class title", "string", "Weapon class title, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Weapon_info[idx].title, s, sizeof(Weapon_info[idx].title)-1);
	}

	return ade_set_args(L, "s", Weapon_info[idx].title);
}

ADE_VIRTVAR(Description, l_Weaponclass, "string", "Weapon class description string", "string", "Description string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "s", "");

	weapon_info *wip = &Weapon_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(wip->desc);
		if(s != NULL) {
			wip->desc = (char*)vm_malloc(strlen(s)+1);
			strcpy(wip->desc, s);
		} else {
			wip->desc = NULL;
		}
	}

	if(wip->desc != NULL)
		return ade_set_args(L, "s", wip->desc);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(TechTitle, l_Weaponclass, "string", "Weapon class tech title", "string", "Tech title, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Weapon_info[idx].tech_title, s, sizeof(Weapon_info[idx].tech_title)-1);
	}

	return ade_set_args(L, "s", Weapon_info[idx].tech_title);
}

ADE_VIRTVAR(TechAnimationFilename, l_Weaponclass, "string", "Weapon class animation filename", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Weapon_info[idx].tech_anim_filename, s, sizeof(Weapon_info[idx].tech_anim_filename)-1);
	}

	return ade_set_args(L, "s", Weapon_info[idx].tech_anim_filename);
}

ADE_VIRTVAR(TechDescription, l_Weaponclass, "string", "Weapon class tech description string", "string", "Description string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "s", "");

	weapon_info *wip = &Weapon_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(wip->tech_desc);
		if(s != NULL) {
			wip->tech_desc = (char*)vm_malloc(strlen(s)+1);
			strcpy(wip->tech_desc, s);
		} else {
			wip->tech_desc = NULL;
		}
	}

	if(wip->tech_desc != NULL)
		return ade_set_args(L, "s", wip->tech_desc);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(ArmorFactor, l_Weaponclass, "number", "Amount of weapon damage applied to ship hull (0-1.0)", "number", "Armor factor, or empty string if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].armor_factor = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].armor_factor);
}

ADE_VIRTVAR(Damage, l_Weaponclass, "number", "Amount of damage that weapon deals", "number", "Damage amount, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].damage = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].damage);
}

ADE_VIRTVAR(FireWait, l_Weaponclass, "number", "Weapon fire wait (cooldown time) in seconds", "number", "Fire wait time, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].fire_wait = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].fire_wait);
}

ADE_VIRTVAR(Mass, l_Weaponclass, "number", "Weapon mass", "number", "Weapon mass, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].mass = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].mass);
}

ADE_VIRTVAR(ShieldFactor, l_Weaponclass, "number", "Amount of weapon damage applied to ship shields (0-1.0)", "number", "Shield damage factor, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].shield_factor = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].shield_factor);
}

ADE_VIRTVAR(SubsystemFactor, l_Weaponclass, "number", "Amount of weapon damage applied to ship subsystems (0-1.0)", "number", "Subsystem damage factor, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].subsystem_factor = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].subsystem_factor);
}

ADE_VIRTVAR(TargetLOD, l_Weaponclass, "number", "LOD used for weapon model in the targeting computer", "number", "LOD number, or 0 if handle is invalid")
{
	int idx;
	int lod = 0;
	if(!ade_get_args(L, "o|i", l_Weaponclass.Get(&idx), &lod))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].hud_target_lod = lod;
	}

	return ade_set_args(L, "i", Weapon_info[idx].hud_target_lod);
}

ADE_VIRTVAR(Speed, l_Weaponclass, "number", "Weapon max speed, aka $Velocity in weapons.tbl", "number", "Weapon speed, or 0 if handle is invalid")
{
	int idx;
	float spd = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &spd))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx > Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].max_speed = spd;
	}

	return ade_set_args(L, "f", Weapon_info[idx].max_speed);
}

ADE_FUNC(isValid, l_Weaponclass, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Eyepoint
struct eye_h
{
	int model;
	int eye_idx;

	eye_h(){model=-1;eye_idx=-1;}
	eye_h(int n_m, int n_e){model=n_m; eye_idx=n_e;}
	bool IsValid(){
		polymodel *pm = NULL;
		return (model > -1
			&& (pm = model_get(model)) != NULL
			&& eye_idx > -1
			&& eye_idx < pm->n_view_positions);
	}
};
ade_obj<eye_h> l_Eyepoint("eyepoint", "Eyepoint handle");

ADE_VIRTVAR(Normal, l_Eyepoint, "vector", "Eyepoint normal", "vector", "Eyepoint normal, or null vector if handle is invalid")
{
	eye_h *eh;
	vec3d *v;
	if(!ade_get_args(L, "o|o", l_Eyepoint.GetPtr(&eh), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!eh->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	polymodel *pm = model_get(eh->model);

	if(ADE_SETTING_VAR && v != NULL)
	{
		pm->view_positions[eh->eye_idx].norm = *v;
	}

	return ade_set_args(L, "o", l_Vector.Set(pm->view_positions[eh->eye_idx].norm));
}

ADE_VIRTVAR(Position, l_Eyepoint, "vector", "Eyepoint location (Local vector)", "vector", "Eyepoint location, or null vector if handle is invalid")
{
	eye_h *eh;
	vec3d *v;
	if(!ade_get_args(L, "o|o", l_Eyepoint.GetPtr(&eh), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!eh->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	polymodel *pm = model_get(eh->model);

	if(ADE_SETTING_VAR && v != NULL)
	{
		pm->view_positions[eh->eye_idx].pnt = *v;
	}

	return ade_set_args(L, "o", l_Vector.Set(pm->view_positions[eh->eye_idx].pnt));
}

//**********HANDLE: modeltextures
ADE_FUNC(__len, l_ModelTextures, NULL, "Number of textures on model", "number", "Number of model textures")
{
	modeltextures_h *mth;
	if(!ade_get_args(L, "o", l_ModelTextures.GetPtr(&mth)))
		return ade_set_error(L, "i", 0);

	if(!mth->IsValid())
		return ade_set_error(L, "i", 0);

	polymodel *pm = mth->Get();

	if(pm == NULL)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", TM_NUM_TYPES*pm->n_textures);
}

ADE_INDEXER(l_ModelTextures, "texture", "number Index/string TextureName", "texture", "Model textures, or invalid modeltextures handle if model handle is invalid")
{
	modeltextures_h *mth = NULL;
	int new_tex = -1;
	char *s = NULL;

	if (!ade_get_args(L, "os|o", l_ModelTextures.GetPtr(&mth), &s, l_Texture.Get(&new_tex)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	polymodel *pm = mth->Get();

	if (!mth->IsValid() || s == NULL || pm == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	texture_info *tinfo = NULL;
	texture_map *tmap = NULL;

	if(strspn(s, "0123456789") == strlen(s))
	{
		int num_textures = TM_NUM_TYPES*pm->n_textures;
		int idx = atoi(s) - 1;	//Lua->FS2

		if (idx < 0 || idx >= num_textures)
			return ade_set_error(L, "o", l_Texture.Set(-1));

		tmap = &pm->maps[idx / TM_NUM_TYPES];
		tinfo = &tmap->textures[idx % TM_NUM_TYPES];
	}

	if(tinfo == NULL)
	{
		for (int i = 0; i < pm->n_textures; i++)
		{
			tmap = &pm->maps[i];

			int tnum = tmap->FindTexture(s);
			if(tnum > -1)
				tinfo = &tmap->textures[tnum];
		}
	}

	//LuaError(L, "%d: %d", lua_type(L,lua_upvalueindex(2)), lua_toboolean(L,lua_upvalueindex(2)));
	if (ADE_SETTING_VAR) {
		tinfo->SetTexture(new_tex);
	}

	return ade_set_args(L, "o", l_Texture.Set(tinfo->GetTexture()));
}

ADE_FUNC(isValid, l_ModelTextures, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	modeltextures_h *mth;
	if(!ade_get_args(L, "o", l_ModelTextures.GetPtr(&mth)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", mth->IsValid());
}

//**********HANDLE: Object
ade_obj<object_h> l_Object("object", "Object handle");
//Helper function
//Returns 1 if object sig stored in idx exists, and stores Objects[] index in idx
//Returns 0 if object sig does not exist, and does not change idx

ADE_FUNC(__eq, l_Object, "object, object", "Checks whether two object handles are for the same object", "boolean", "True if equal, false if not or a handle is invalid")
{
	object_h *o1, *o2;
	if(!ade_get_args(L, "oo", l_Object.GetPtr(&o1), l_Object.GetPtr(&o2)))
		return ADE_RETURN_FALSE;

	if(!o1->IsValid() || !o2->IsValid())
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", o1->sig == o2->sig);
}

ADE_FUNC(__tostring, l_Object, NULL, "Returns name of object (if any)", "string", "Object name, or empty string if handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Object.GetPtr(&objh)))
		return ade_set_error(L, "s", "");

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	char buf[512];

	switch(objh->objp->type)
	{
		case OBJ_SHIP:
			snprintf(buf, sizeof(buf), "%s", Ships[objh->objp->instance].ship_name);
			break;
		case OBJ_WEAPON:
			snprintf(buf, sizeof(buf), "%s projectile", Weapon_info[Weapons[objh->objp->instance].weapon_info_index].name);
			break;
	//	case OBJ_JUMP_NODE:
	//		sprintf(buf, "%s", objh->objp->jnp->get_name_ptr());
	//		break;
		default:
			snprintf(buf, sizeof(buf), "Object %d [%d]", OBJ_INDEX(objh->objp), objh->sig);
	}

	return ade_set_args(L, "s", buf);
}

ADE_VIRTVAR(Parent, l_Object, "object", "Parent of the object. Value may also be a deriviative of the 'object' class, such as 'ship'.", "object", "Parent handle, or invalid handle if object is invalid")
{
	object_h *objh;
	object_h *newparenth = NULL;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Object.GetPtr(&newparenth)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR)
	{
		if(newparenth != NULL && newparenth->IsValid())
		{
			objh->objp->parent = OBJ_INDEX(newparenth->objp);
			objh->objp->parent_sig = newparenth->sig;
			objh->objp->parent_type = newparenth->objp->type;
		}
		else
		{
			objh->objp->parent = -1;
			objh->objp->parent_sig = 0;
			objh->objp->parent_type = OBJ_NONE;
		}
	}

	if(objh->objp->parent > -1)
		return ade_set_object_with_breed(L, objh->objp->parent);
	else
		return ade_set_args(L, "o", l_Object.Set(object_h()));
}

ADE_VIRTVAR(Position, l_Object, "vector", "Object world position (World vector)", "vector", "World position, or null vector if handle is invalid")
{
	object_h *objh;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		objh->objp->pos = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(objh->objp->pos));
}

ADE_VIRTVAR(Orientation, l_Object, "orientation", "Object world orientation (World orientation)", "orientation", "Orientation, or null orientation if handle is invalid")
{
	object_h *objh;
	matrix_h *mh=NULL;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h(&vmd_identity_matrix)));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h(&vmd_identity_matrix)));

	if(ADE_SETTING_VAR && mh != NULL) {
		objh->objp->orient = *mh->GetMatrix();
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&objh->objp->orient)));
}

ADE_VIRTVAR(Physics, l_Object, "physics", "Physics data used to move ship between frames", "physics", "Physics data, or invalid physics handle if object handle is invalid")
{
	object_h *objh;
	physics_info_h *pih;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Physics.GetPtr(&pih)))
		return ade_set_error(L, "o", l_Physics.Set(physics_info_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Physics.Set(physics_info_h()));

	if(ADE_SETTING_VAR && pih->IsValid()) {
		objh->objp->phys_info = *pih->pi;
	}

	return ade_set_args(L, "o", l_Physics.Set(physics_info_h(objh->objp)));
}

ADE_VIRTVAR(HitpointsLeft, l_Object, "number", "Hitpoints an object has left", "number", "Hitpoints left, or 0 if handle is invalid")
{
	object_h *objh = NULL;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Object.GetPtr(&objh), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	//Set hull strength.
	if(ADE_SETTING_VAR) {
		objh->objp->hull_strength = f;
	}

	return ade_set_args(L, "f", objh->objp->hull_strength);
}

ADE_VIRTVAR(Shields, l_Object, "shields", "Shields", "shields", "Shields handle, or invalid shields handle if object handle is invalid")
{
	object_h *objh;
	object_h *sobjh;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Shields.GetPtr(&sobjh)))
		return ade_set_error(L, "o", l_Shields.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Shields.Set(object_h()));

	//WMC - copy shields
	if(ADE_SETTING_VAR && sobjh != NULL && sobjh->IsValid())
	{
		for(int i = 0; i < MAX_SHIELD_SECTIONS; i++)
			shield_set_quad(objh->objp, i, shield_get_quad(sobjh->objp, i));
	}

	return ade_set_args(L, "o", l_Shields.Set(object_h(objh->objp)));
}

ADE_FUNC(isValid, l_Object, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_Object.GetPtr(&oh)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", oh->IsValid());
}

ADE_FUNC(getBreedName, l_Object, NULL, "Gets object type", "string", "Object type name, or empty string if handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Object.GetPtr(&objh)))
		return ade_set_error(L, "s", "");

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Object_type_names[objh->objp->type]);
}

//**********HANDLE: Asteroid
ade_obj<object_h> l_Asteroid("asteroid", "Asteroid handle", &l_Object);

ADE_VIRTVAR(Target, l_Asteroid, "object", "Asteroid target object; may be object derivative, such as ship.", "object", "Target object, or invalid handle if asteroid handle is invalid")
{
	object_h *oh = NULL;
	object_h *th = NULL;
	if(!ade_get_args(L, "o|o", l_Asteroid.GetPtr(&oh), l_Object.GetPtr(&th)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	asteroid *asp = &Asteroids[oh->objp->instance];

	if(ADE_SETTING_VAR && th != NULL) {
		if(th->IsValid())
			asp->target_objnum = OBJ_INDEX(th->objp);
		else
			asp->target_objnum = -1;
	}

	if(asp->target_objnum > 0 && asp->target_objnum < MAX_OBJECTS)
		return ade_set_object_with_breed(L, asp->target_objnum);
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

}

//**********HANDLE: Debris
ade_obj<object_h> l_Debris("debris", "Debris handle", &l_Object);

ADE_VIRTVAR(IsHull, l_Debris, "boolean", "Whether or not debris is a piece of hull", "boolean", "Whether debris is a hull fragment, or false if handle is invalid")
{
	object_h *oh;
	bool b=false;
	if(!ade_get_args(L, "o|b", l_Debris.GetPtr(&oh), &b))
		return ade_set_error(L, "b", false);

	if(!oh->IsValid())
		return ade_set_error(L, "b", false);

	debris *db = &Debris[oh->objp->instance];

	if(ADE_SETTING_VAR) {
		db->is_hull = b ? 1 : 0;
	}

	return ade_set_args(L, "b", db->is_hull ? true : false);

}

//**********HANDLE: Shipclass
ade_obj<int> l_Shipclass("shipclass", "Ship class handle");
extern int ships_inited;

ADE_FUNC(__tostring, l_Shipclass, NULL, "Ship class name", "string", "Ship class name, or an empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Ship_info[idx].name);
}

ADE_VIRTVAR(Name, l_Shipclass, "string", "Ship class name", "string", "Ship class name, or an empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Ship_info[idx].name, s, sizeof(Ship_info[idx].name)-1);
	}

	return ade_set_args(L, "s", Ship_info[idx].name);
}

ADE_VIRTVAR(ShortName, l_Shipclass, "string", "Ship class short name", "string", "Ship short name, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Ship_info[idx].short_name, s, sizeof(Ship_info[idx].short_name)-1);
	}

	return ade_set_args(L, "s", Ship_info[idx].short_name);
}

ADE_VIRTVAR(TypeString, l_Shipclass, "string", "Ship class type string", "string", "Type string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->type_str);
		if(s != NULL) {
			sip->type_str = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->type_str, s);
		} else {
			sip->type_str = NULL;
		}
	}

	if(sip->type_str != NULL)
		return ade_set_args(L, "s", sip->type_str);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(ManeuverabilityString, l_Shipclass, "string", "Ship class maneuverability string", "string", "Maneuverability string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->maneuverability_str);
		if(s != NULL) {
			sip->maneuverability_str = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->maneuverability_str, s);
		} else {
			sip->maneuverability_str = NULL;
		}
	}

	if(sip->maneuverability_str != NULL)
		return ade_set_args(L, "s", sip->maneuverability_str);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(ArmorString, l_Shipclass, "string", "Ship class armor string", "string", "Armor string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->armor_str);
		if(s != NULL) {
			sip->armor_str = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->armor_str, s);
		} else {
			sip->armor_str = NULL;
		}
	}

	if(sip->armor_str != NULL)
		return ade_set_args(L, "s", sip->armor_str);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(ManufacturerString, l_Shipclass, "string", "Ship class manufacturer", "string", "Manufacturer, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->manufacturer_str);
		if(s != NULL) {
			sip->manufacturer_str = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->manufacturer_str, s);
		} else {
			sip->manufacturer_str = NULL;
		}
	}

	if(sip->manufacturer_str != NULL)
		return ade_set_args(L, "s", sip->manufacturer_str);
	else
		return ade_set_args(L, "s", "");
}


ADE_VIRTVAR(Description, l_Shipclass, "string", "Ship class description", "string", "Description, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->desc);
		if(s != NULL) {
			sip->desc = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->desc, s);
		} else {
			sip->desc = NULL;
		}
	}

	if(sip->desc != NULL)
		return ade_set_args(L, "s", sip->desc);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(TechDescription, l_Shipclass, "string", "Ship class tech description", "string", "Tech description, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->tech_desc);
		if(s != NULL) {
			sip->tech_desc = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->tech_desc, s);
		} else {
			sip->tech_desc = NULL;
		}
	}

	if(sip->tech_desc != NULL)
		return ade_set_args(L, "s", sip->tech_desc);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(AfterburnerFuelMax, l_Shipclass, "number", "Afterburner fuel capacity", "number", "Afterburner capacity, or 0 if handle is invalid")
{
	int idx;
	float fuel = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &fuel))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && fuel >= 0.0f)
		Ship_info[idx].afterburner_fuel_capacity = fuel;

	return ade_set_args(L, "f", Ship_info[idx].afterburner_fuel_capacity);
}

ADE_VIRTVAR(CountermeasuresMax, l_Shipclass, "number", "Maximum number of countermeasures the ship can carry", "number", "Countermeasure capacity, or 0 if handle is invalid")
{
	int idx;
	int i = -1;
	if(!ade_get_args(L, "o|i", l_Shipclass.Get(&idx), &i))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR && i > -1) {
		Ship_info[idx].cmeasure_max = i;
	}

	return ade_set_args(L, "i", Ship_info[idx].cmeasure_max);
}

ADE_VIRTVAR(Model, l_Shipclass, "model", "Model", "model", "Ship class model, or invalid model handle if shipclass handle is invalid")
{
	int ship_info_idx=-1;
	model_h *mdl = NULL;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&ship_info_idx), l_Model.GetPtr(&mdl)))
		return ade_set_error(L, "o", l_Model.Set(-1));

	if(ship_info_idx < 0 || ship_info_idx > Num_ship_classes)
		return ade_set_error(L, "o", l_Model.Set(-1));

	ship_info *sip = &Ship_info[ship_info_idx];

	int mid = mdl->GetID();

	if(ADE_SETTING_VAR && mid > -1) {
		sip->model_num = mid;
	}

	return ade_set_args(L, "o", l_Model.Set(model_h(sip->model_num)));
}

ADE_VIRTVAR(CockpitModel, l_Shipclass, "model", "Model used for first-person cockpit", "model", "Cockpit model")
{
	int ship_info_idx=-1;
	model_h *mdl = NULL;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&ship_info_idx), l_Model.GetPtr(&mdl)))
		return ade_set_error(L, "o", l_Model.Set(model_h()));

	if(ship_info_idx < 0 || ship_info_idx > Num_ship_classes)
		return ade_set_error(L, "o", l_Model.Set(model_h()));

	ship_info *sip = &Ship_info[ship_info_idx];

	int mid = mdl->GetID();

	if(ADE_SETTING_VAR) {
		sip->cockpit_model_num = mid;
	}

	return ade_set_args(L, "o", l_Model.Set(model_h(sip->cockpit_model_num)));
}

ADE_VIRTVAR(HitpointsMax, l_Shipclass, "number", "Ship class hitpoints", "number", "Hitpoints, or 0 if handle is invalid")
{
	int idx;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && f >= 0.0f) {
		Ship_info[idx].max_hull_strength = f;
	}

	return ade_set_args(L, "f", Ship_info[idx].max_hull_strength);
}

ADE_VIRTVAR(Species, l_Shipclass, "Species", "Ship class species", "species", "Ship class species, or invalid species handle if shipclass handle is invalid")
{
	int idx;
	int sidx;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&idx), l_Species.Get(&sidx)))
		return ade_set_error(L, "o", l_Species.Set(-1));

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "o", l_Species.Set(-1));

	if(ADE_SETTING_VAR && sidx > -1 && sidx < (int)Species_info.size()) {
		Ship_info[idx].species = sidx;
	}

	return ade_set_args(L, "o", l_Species.Set(Ship_info[idx].species));
}

ADE_VIRTVAR(Type, l_Shipclass, "shiptype", "Ship class type", "shiptype", "Ship type, or invalid handle if shipclass handle is invalid")
{
	int idx;
	int sidx;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&idx), l_Shiptype.Get(&sidx)))
		return ade_set_error(L, "o", l_Shiptype.Set(-1));

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "o", l_Shiptype.Set(-1));

	if(ADE_SETTING_VAR && sidx > -1 && sidx < (int)Ship_types.size()) {
		Ship_info[idx].class_type = sidx;
	}

	return ade_set_args(L, "o", l_Shiptype.Set(Ship_info[idx].class_type));
}

ADE_FUNC(isValid, l_Shipclass, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_ship_classes)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(isInTechroom, l_Shipclass, NULL, "Gets whether or not the ship class is available in the techroom", "boolean", "Whether ship has been revealed in the techroom, false if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_error(L, "b", false);

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_error(L, "b", false);

	bool b = false;
	if(Player != NULL && (Player->flags & PLAYER_FLAGS_IS_MULTI) && (Ship_info[idx].flags & SIF_IN_TECH_DATABASE_M)) {
		b = true;
	} else if(Ship_info[idx].flags & SIF_IN_TECH_DATABASE) {
		b = true;
	}

	return ade_set_args(L, "b", b);
}


ADE_FUNC(renderTechModel, l_Shipclass, "X1, Y1, X2, Y2, [Rotation %, Pitch %, Bank %, Zoom multiplier]", "Draws ship model as if in techroom", "boolean", "Whether ship was rendered")
{
	int x1,y1,x2,y2;
	angles rot_angles = {0.0f, 0.0f, 40.0f};
	int idx;
	float zoom = 1.3f;
	if(!ade_get_args(L, "oiiii|ffff", l_Shipclass.Get(&idx), &x1, &y1, &x2, &y2, &rot_angles.h, &rot_angles.p, &rot_angles.b, &zoom))
		return ade_set_error(L, "b", false);

	if(idx < 0 || idx > Num_ship_classes)
		return ade_set_args(L, "b", false);

	if(x2 < x1 || y2 < y1)
		return ade_set_args(L, "b", false);

	if(rot_angles.p < 0.0f)
		rot_angles.p = 0.0f;
	if(rot_angles.p > 100.0f)
		rot_angles.p = 100.0f;
	if(rot_angles.b < 0.0f)
		rot_angles.b = 0.0f;
	if(rot_angles.b > 100.0f)
		rot_angles.b = 100.0f;
	if(rot_angles.h < 0.0f)
		rot_angles.h = 0.0f;
	if(rot_angles.h > 100.0f)
		rot_angles.h = 100.0f;

	ship_info *sip = &Ship_info[idx];

	//Make sure model is loaded
	sip->model_num = model_load(sip->pof_file, sip->n_subsystems, &sip->subsystems[0], 0);

	if(sip->model_num < 0)
		return ade_set_args(L, "b", false);

	//Handle angles
	matrix orient = vmd_identity_matrix;
	angles view_angles = {-0.6f, 0.0f, 0.0f};
	vm_angles_2_matrix(&orient, &view_angles);

	rot_angles.p = (rot_angles.p*0.01f) * PI2;
	rot_angles.b = (rot_angles.b*0.01f) * PI2;
	rot_angles.h = (rot_angles.h*0.01f) * PI2;
	vm_rotate_matrix_by_angles(&orient, &rot_angles);

	//Clip
	gr_set_clip(x1,y1,x2-x1,y2-y1,false);

	//Handle 3D init stuff
	g3_start_frame(1);
	g3_set_view_matrix(&sip->closeup_pos, &vmd_identity_matrix, sip->closeup_zoom * zoom);

	if (!Cmdline_nohtl) {
		gr_set_proj_matrix( Proj_fov, gr_screen.clip_aspect, Min_draw_distance, Max_draw_distance);
		gr_set_view_matrix(&Eye_position, &Eye_matrix);
	}

	//Handle light
	light_reset();
	vec3d light_dir = vmd_zero_vector;
	light_dir.xyz.y = 1.0f;	
	light_add_directional(&light_dir, 0.65f, 1.0f, 1.0f, 1.0f);
	light_rotate_all();

	//Draw the ship!!
	model_clear_instance(sip->model_num);
	model_set_detail_level(0);
	model_render(sip->model_num, &orient, &vmd_zero_vector, MR_LOCK_DETAIL | MR_AUTOCENTER | MR_NO_FOGGING);

	//OK we're done
	if (!Cmdline_nohtl) 
	{
		gr_end_view_matrix();
		gr_end_proj_matrix();
	}

	//Bye!!
	g3_end_frame();
	gr_reset_clip();

	return ade_set_args(L, "b", true);
}

//**********HANDLE: Waypoint
ade_obj<object_h> l_Waypoint("waypoint", "waypoint handle", &l_Object);

//**********HANDLE: WaypointList
struct waypointlist_h
{
	waypoint_list *wlp;
	char name[NAME_LENGTH];
	waypointlist_h(){wlp=NULL;name[0]='\0';}
	waypointlist_h(waypoint_list *n_wlp){
		wlp = n_wlp;
		if(n_wlp != NULL)
			strcpy(name, wlp->name);
	}
	waypointlist_h(char wlname[NAME_LENGTH]) {
		wlp = NULL;
		if ( wlname != NULL ) {
			strcpy(name, wlname);
			for ( int i = 0; i < Num_waypoint_lists; i++ ) {
				if ( !stricmp( Waypoint_lists[i].name, wlname ) ) {
					wlp = &Waypoint_lists[i];
					return;
				}
			}
		}
	}
	bool IsValid() {
		return (this != NULL && wlp != NULL && !strcmp(wlp->name, name));
	}
};

ade_obj<waypointlist_h> l_WaypointList("waypointlist", "waypointlist handle");

ADE_INDEXER(l_WaypointList, "number Index", "Array of waypoints that are part of the waypoint list", "waypoint", "Waypoint, or invalid handle if the index or waypointlist handle is invalid")
{
	int idx = -1;
	waypointlist_h* wlh = NULL;
	char wpname[128];
	if( !ade_get_args(L, "oi", l_WaypointList.GetPtr( &wlh ), &idx))
		return ade_set_error( L, "o", l_Waypoint.Set( object_h() ) );

	if(!wlh->IsValid())
		return ade_set_error( L, "o", l_Waypoint.Set( object_h() ) );

	//Lua-->FS2
	idx--;

	//Get waypoint name
	snprintf(wpname, sizeof(wpname), "%s:%d", wlh->wlp->name, (idx & 0xffff) + 1);
	int i = waypoint_lookup( wpname );
	if( idx > -1 && idx < wlh->wlp->count && i != -1 ) {
		return ade_set_args( L, "o", l_Waypoint.Set( object_h( &Objects[i] ), Objects[i].signature ) );
	}

	return ade_set_error(L, "o", l_Waypoint.Set( object_h() ) );
}

ADE_FUNC(__len, l_WaypointList,
		 NULL,
		 "Number of waypoints in the list. "
		 "Note that the value returned cannot be relied on for more than one frame.",
		 "number",
		 "Number of waypoints in the list, or 0 if handle is invalid")
{
	waypointlist_h* wlh = NULL;
	if ( !ade_get_args(L, "o", l_WaypointList.GetPtr(&wlh)) ) {
		return ade_set_error( L, "o", l_Waypoint.Set( object_h() ) );
	}
	return ade_set_args(L, "i", wlh->wlp->count);
}

ADE_VIRTVAR(Name, l_WaypointList, "string", "Name of WaypointList", "string", "Waypointlist name, or empty string if handle is invalid")
{
	waypointlist_h* wlh = NULL;
	if ( !ade_get_args(L, "o", l_WaypointList.GetPtr(&wlh)) ) {
		return ade_set_error( L, "o", l_Waypoint.Set( object_h() ) );
	}
	return ade_set_args( L, "s", wlh->name);
}

//WMC - Waypoints are messed. Gonna leave this for later.
/*
ade_lib l_WaypointList_Waypoints("Waypoints", &l_Waypoint, NULL, NULL);

ADE_INDEXER(l_WaypointList_Waypoints, "waypoint index", "waypoint", "Gets waypoint handle")
{
	waypoint_list *wlp = NULL;
	int idx;
	if(!ade_get_args(L, "o|i", &idx))
		return ade_set_error(L, "o", l_Waypoint.Set(object_h()));

	//Remember, Lua indices start at 0.
	int count=0;

	object *ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list))
	{
		if (ptr->type == OBJ_WAYPOINT)
			count++;

		if(count == idx) {
			return ade_set_args(L, "o", l_Waypoint.Set(object_h(ptr)));
		}

		ptr = GET_NEXT(ptr);
	}

	return ade_set_error(L, "o", l_Weapon.Set(object_h()));
}

ADE_FUNC(__len, l_WaypointList_Waypoints, NULL, "Number of waypoints in the mission",
		 "Gets number of waypoints in mission. Note that this is only accurate for one frame.")
{
	int count=0;
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if (Objects[i].type == OBJ_WAYPOINT)
			count++;
	}

	return ade_set_args(L, "i", count);
}*/

//**********HANDLE: Weaponbank
#define SWH_NONE		0
#define SWH_PRIMARY		1
#define SWH_SECONDARY	2
#define SWH_TERTIARY	3

struct ship_banktype_h : public object_h
{
	int type;
	ship_weapon *sw;

	ship_banktype_h() : object_h () {
		sw = NULL;
		type = SWH_NONE;
	}
	ship_banktype_h(object *objp, ship_weapon *wpn, int in_type) : object_h(objp) {
		sw = wpn;
		type = in_type;
	}

	bool IsValid()
	{
		return object_h::IsValid() && sw != NULL && (type == SWH_PRIMARY || type == SWH_SECONDARY || type == SWH_TERTIARY);
	}
};
struct ship_bank_h : public ship_banktype_h
{
	int bank;

	ship_bank_h() : ship_banktype_h() {
		bank = -1;
	}
	ship_bank_h(object *objp, ship_weapon *wpn, int in_type, int in_bank) : ship_banktype_h(objp, wpn, in_type) {
		bank = in_bank;
	}

	bool IsValid()
	{
		if(!ship_banktype_h::IsValid())
			return false;

		if(bank < 0)
			return false;

		if(type == SWH_PRIMARY && bank >= sw->num_primary_banks)
			return false;
		if(type == SWH_SECONDARY && bank >= sw->num_secondary_banks)
			return false;
		if(type == SWH_TERTIARY && bank >= sw->num_tertiary_banks)
			return false;

		return true;
	}
};

//**********HANDLE: Ship bank
ade_obj<ship_bank_h> l_WeaponBank("weaponbank", "Ship/subystem weapons bank handle");

ADE_VIRTVAR(WeaponClass, l_WeaponBank, "weaponclass", "Class of weapon mounted in the bank", "weaponclass", "Weapon class, or an invalid weaponclass handle if bank handle is invalid")
{
	ship_bank_h *bh = NULL;
	int weaponclass=-1;
	if(!ade_get_args(L, "o|o", l_WeaponBank.GetPtr(&bh), l_Weaponclass.Get(&weaponclass)))
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	if(!bh->IsValid())
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR && weaponclass > -1) {
				bh->sw->primary_bank_weapons[bh->bank] = weaponclass;
			}

			return ade_set_args(L, "o", l_Weaponclass.Set(bh->sw->primary_bank_weapons[bh->bank]));
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR && weaponclass > -1) {
				bh->sw->secondary_bank_weapons[bh->bank] = weaponclass;
			}

			return ade_set_args(L, "o", l_Weaponclass.Set(bh->sw->secondary_bank_weapons[bh->bank]));
		case SWH_TERTIARY:
			if(ADE_SETTING_VAR && weaponclass > -1) {
				//bh->sw->tertiary_bank_weapons[bh->bank] = weaponclass;
			}

			//return ade_set_args(L, "o", l_Weaponclass.Set(bh->sw->tertiary_bank_weapons[bh->bank]));
			return ADE_RETURN_FALSE;
	}

	return ade_set_error(L, "o", l_Weaponclass.Set(-1));
}

ADE_VIRTVAR(AmmoLeft, l_WeaponBank, "number", "Ammo left for the current bank", "number", "Ammo left, or 0 if handle is invalid")
{
	ship_bank_h *bh = NULL;
	int ammo;
	if(!ade_get_args(L, "o|i", l_WeaponBank.GetPtr(&bh), &ammo))
		return ade_set_error(L, "i", 0);

	if(!bh->IsValid())
		return ade_set_error(L, "i", 0);

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR && ammo > -1) {
				bh->sw->primary_bank_ammo[bh->bank] = ammo;
			}

			return ade_set_args(L, "i", bh->sw->primary_bank_ammo[bh->bank]);
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR && ammo > -1) {
				bh->sw->secondary_bank_ammo[bh->bank] = ammo;
			}

			return ade_set_args(L, "i", bh->sw->secondary_bank_ammo[bh->bank]);
		case SWH_TERTIARY:
			if(ADE_SETTING_VAR && ammo > -1) {
				bh->sw->tertiary_bank_ammo = ammo;
			}
			return ade_set_args(L, "i", bh->sw->tertiary_bank_ammo);
	}

	return ade_set_error(L, "i", 0);
}

ADE_VIRTVAR(AmmoMax, l_WeaponBank, "number", "Maximum ammo for the current bank", "number", "Ammo capacity, or 0 if handle is invalid")
{
	ship_bank_h *bh = NULL;
	int ammomax;
	if(!ade_get_args(L, "o|i", l_WeaponBank.GetPtr(&bh), &ammomax))
		return ade_set_error(L, "i", 0);

	if(!bh->IsValid())
		return ade_set_error(L, "i", 0);

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR && ammomax > -1) {
				bh->sw->primary_bank_start_ammo[bh->bank] = ammomax;
			}

			return ade_set_args(L, "i", bh->sw->primary_bank_start_ammo[bh->bank]);
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR && ammomax > -1) {
				bh->sw->secondary_bank_start_ammo[bh->bank] = ammomax;
			}

			return ade_set_args(L, "i", bh->sw->secondary_bank_start_ammo[bh->bank]);
		case SWH_TERTIARY:
			if(ADE_SETTING_VAR && ammomax > -1) {
				bh->sw->tertiary_bank_ammo = ammomax;
			}

			return ade_set_args(L, "i", bh->sw->tertiary_bank_start_ammo);
	}

	return ade_set_error(L, "i", 0);
}

ADE_VIRTVAR(Armed, l_WeaponBank, "boolean", "Weapon armed status. Does not take linking into account.", "boolean", "True if armed, false if unarmed or handle is invalid")
{
	ship_bank_h *bh = NULL;
	bool armthis=false;
	if(!ade_get_args(L, "o|b", l_WeaponBank.GetPtr(&bh), &armthis))
		return ade_set_error(L, "b", false);

	if(!bh->IsValid())
		return ade_set_error(L, "b", false);

	int new_armed_bank = -1;
	if(armthis)
		new_armed_bank = bh->bank;

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR) {
				bh->sw->current_primary_bank = new_armed_bank;
			}
			return ade_set_args(L, "b", bh->sw->current_primary_bank == bh->bank);
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR) {
				bh->sw->current_secondary_bank = new_armed_bank;
			}
			return ade_set_args(L, "b", bh->sw->current_secondary_bank == bh->bank);
		case SWH_TERTIARY:
			if(ADE_SETTING_VAR) {
				bh->sw->current_tertiary_bank = new_armed_bank;
			}
			return ade_set_args(L, "b", bh->sw->current_tertiary_bank == bh->bank);
	}

	return ade_set_error(L, "b", false);
}

ADE_FUNC(isValid, l_WeaponBank, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	ship_bank_h *bh;
	if(!ade_get_args(L, "o", l_WeaponBank.GetPtr(&bh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", bh->IsValid());
}

//**********HANDLE: Weaponbanktype
ade_obj<ship_banktype_h> l_WeaponBankType("weaponbanktype", "Ship/subsystem weapons bank type handle");

ADE_INDEXER(l_WeaponBankType, "number Index", "Array of weapon banks", "weaponbank", "Weapon bank, or invalid handle on failure")
{
	ship_banktype_h *sb=NULL;
	int idx = -1;
	ship_bank_h *newbank;
	if(!ade_get_args(L, "oi|o", l_WeaponBankType.GetPtr(&sb), &idx, l_WeaponBank.GetPtr(&newbank)))
		return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

	if(!sb->IsValid())
		return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

	switch(sb->type)
	{
		case SWH_PRIMARY:
				if(idx < 1 || idx > sb->sw->num_primary_banks)
					return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

				idx--; //Lua->FS2

				if(ADE_SETTING_VAR && newbank->IsValid()) {
					sb->sw->primary_bank_weapons[idx] = newbank->sw->primary_bank_weapons[idx];
					sb->sw->next_primary_fire_stamp[idx] = timestamp(0);
					sb->sw->primary_bank_ammo[idx] = newbank->sw->primary_bank_ammo[idx];
					sb->sw->primary_bank_start_ammo[idx] = newbank->sw->primary_bank_start_ammo[idx];
					sb->sw->primary_bank_capacity[idx] = newbank->sw->primary_bank_capacity[idx];
					sb->sw->primary_bank_rearm_time[idx] = timestamp(0);
				}
				break;
		case SWH_SECONDARY:
				if(idx < 1 || idx > sb->sw->num_secondary_banks)
					return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

				idx--; //Lua->FS2

				if(ADE_SETTING_VAR && newbank->IsValid()) {
					sb->sw->primary_bank_weapons[idx] = newbank->sw->primary_bank_weapons[idx];
					sb->sw->next_primary_fire_stamp[idx] = timestamp(0);
					sb->sw->primary_bank_ammo[idx] = newbank->sw->primary_bank_ammo[idx];
					sb->sw->primary_bank_start_ammo[idx] = newbank->sw->primary_bank_start_ammo[idx];
					sb->sw->primary_bank_capacity[idx] = newbank->sw->primary_bank_capacity[idx];
				}
				break;
		case SWH_TERTIARY:
				if(idx < 1 || idx > sb->sw->num_tertiary_banks)
					return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

				idx--; //Lua->FS2

				if(ADE_SETTING_VAR && newbank->IsValid()) {
					Error(LOCATION, "Tertiary bank support is still in progress");
					//WMC: TODO
				}
				break;
		default:
			return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));	//Invalid type
	}

	return ade_set_args(L, "o", l_WeaponBank.Set(ship_bank_h(sb->objp, sb->sw, sb->type, idx)));
}

ADE_VIRTVAR(Linked, l_WeaponBankType, "boolean", "Whether bank is in linked or unlinked fire mode (Primary-only)", "boolean", "Link status, or false if handle is invalid")
{
	ship_banktype_h *bh;
	bool newlink = false;
	int numargs = ade_get_args(L, "o|b", l_WeaponBankType.GetPtr(&bh), &newlink);

	if(!numargs)
		return ade_set_error(L, "b", false);

	if(!bh->IsValid())
		return ade_set_error(L, "b", false);

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR && numargs > 1) {
				if(newlink)
					Ships[bh->objp->instance].flags |= SF_PRIMARY_LINKED;
				else
					Ships[bh->objp->instance].flags &= ~SF_PRIMARY_LINKED;
			}

			return ade_set_args(L, "b", (Ships[bh->objp->instance].flags & SF_PRIMARY_LINKED) > 0);

		case SWH_SECONDARY:
		case SWH_TERTIARY:
			return ADE_RETURN_FALSE;
	}

	return ade_set_error(L, "b", false);
}

ADE_VIRTVAR(DualFire, l_WeaponBankType, "boolean", "Whether bank is in dual fire mode (Secondary-only)", "boolean", "Dual fire status, or false if handle is invalid")
{
	ship_banktype_h *bh;
	bool newfire = false;
	int numargs = ade_get_args(L, "o|b", l_WeaponBankType.GetPtr(&bh), &newfire);

	if(!numargs)
		return ade_set_error(L, "b", false);

	if(!bh->IsValid())
		return ade_set_error(L, "b", false);

	switch(bh->type)
	{
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR && numargs > 1) {
				if(newfire)
					Ships[bh->objp->instance].flags |= SF_SECONDARY_DUAL_FIRE;
				else
					Ships[bh->objp->instance].flags &= ~SF_SECONDARY_DUAL_FIRE;
			}

			return ade_set_args(L, "b", (Ships[bh->objp->instance].flags & SF_SECONDARY_DUAL_FIRE) > 0);

		case SWH_PRIMARY:
		case SWH_TERTIARY:
			return ADE_RETURN_FALSE;
	}

	return ade_set_error(L, "b", false);
}

ADE_FUNC(isValid, l_WeaponBankType, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	ship_banktype_h *sb;
	if(!ade_get_args(L, "o", l_WeaponBankType.GetPtr(&sb)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", sb->IsValid());
}

ADE_FUNC(__len, l_WeaponBankType, NULL, "Number of weapons in the mounted bank", "number", "Number of bank weapons, or 0 if handle is invalid")
{
	ship_banktype_h *sb=NULL;
	if(!ade_get_args(L, "o", l_WeaponBankType.GetPtr(&sb)))
		return ade_set_error(L, "i", 0);

	if(!sb->IsValid())
		return ade_set_error(L, "i", 0);

	switch(sb->type)
	{
		case SWH_PRIMARY:
			return ade_set_args(L, "i", sb->sw->num_primary_banks);
		case SWH_SECONDARY:
			return ade_set_args(L, "i", sb->sw->num_secondary_banks);
		case SWH_TERTIARY:
			return ade_set_args(L, "i", sb->sw->num_tertiary_banks);
		default:
			return ade_set_error(L, "i", 0);	//Invalid type
	}
}

//**********HANDLE: Subsystem
struct ship_subsys_h : public object_h
{
	ship_subsys *ss;	//Pointer to subsystem, or NULL for the hull

	bool IsValid(){return object_h::IsValid() && objp->type == OBJ_SHIP && ss != NULL;}
	ship_subsys_h() : object_h() {
		ss = NULL;
	}
	ship_subsys_h(object *objp, ship_subsys *sub) : object_h(objp) {
		ss = sub;
	}
};
ade_obj<ship_subsys_h> l_Subsystem("subsystem", "Ship subsystem handle");

ADE_FUNC(__tostring, l_Subsystem, NULL, "Returns name of subsystem", "string", "Subsystem name, or empty string if handle is invalid")
{
	ship_subsys_h *sso;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "s", "");

	if(!sso->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", ship_subsys_get_name(sso->ss));
}

ADE_VIRTVAR(AWACSIntensity, l_Subsystem, "number", "Subsystem AWACS intensity", "number", "AWACS intensity, or 0 if handle is invalid")
{
	ship_subsys_h *sso;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!sso->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && f >= 0.0f)
		sso->ss->awacs_intensity = f;

	return ade_set_args(L, "f", sso->ss->awacs_intensity);
}

ADE_VIRTVAR(AWACSRadius, l_Subsystem, "number", "Subsystem AWACS radius", "number", "AWACS radius, or 0 if handle is invalid")
{
	ship_subsys_h *sso;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!sso->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && f >= 0.0f)
		sso->ss->awacs_radius = f;

	return ade_set_args(L, "f", sso->ss->awacs_radius);
}

ADE_VIRTVAR(Orientation, l_Subsystem, "orientation", "Orientation of subobject or turret base", "orientation", "Subsystem orientation, or null orientation if handle is invalid")
{
	ship_subsys_h *sso;
	matrix_h *mh;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	if(ADE_SETTING_VAR && mh != NULL)
	{
		sso->ss->submodel_info_1.angs = *mh->GetAngles();
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&sso->ss->submodel_info_1.angs)));
}

ADE_VIRTVAR(GunOrientation, l_Subsystem, "orientation", "Orientation of turret gun", "orientation", "Gun orientation, or null orientation if handle is invalid")
{
	ship_subsys_h *sso;
	matrix_h *mh;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	if(ADE_SETTING_VAR && mh != NULL)
	{
		sso->ss->submodel_info_2.angs = *mh->GetAngles();
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&sso->ss->submodel_info_2.angs)));
}

ADE_VIRTVAR(HitpointsLeft, l_Subsystem, "number", "Subsystem hitpoints left", "number", "Hitpoints left, or 0 if handle is invalid. Setting a value of 0 will disable it - set a value of -1 or lower to actually blow it up.")
{
	ship_subsys_h *sso;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!sso->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR)
	{
		//Only go down to 0 hits
		sso->ss->current_hits = MAX(0.0f, f);

		ship *shipp = &Ships[sso->objp->instance];
		if (f <= -1.0f && sso->ss->current_hits <= 0.0f) {
			do_subobj_destroyed_stuff(shipp, sso->ss, NULL);
		}
		ship_recalc_subsys_strength(shipp);
	}

	return ade_set_args(L, "f", sso->ss->current_hits);
}

ADE_VIRTVAR(HitpointsMax, l_Subsystem, "number", "Subsystem hitpoints max", "number", "Max hitpoints, or 0 if handle is invalid")
{
	ship_subsys_h *sso;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!sso->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR)
	{
		sso->ss->max_hits = MIN(0.0f, f);

		ship_recalc_subsys_strength(&Ships[sso->objp->instance]);
	}

	return ade_set_args(L, "f", sso->ss->max_hits);
}

ADE_VIRTVAR(Position, l_Subsystem, "vector", "Subsystem position with regards to main ship (Local Vector)", "vector", "Subsystem position, or null vector if subsystem handle is invalid")
{
	ship_subsys_h *sso;
	vec3d *v = NULL;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	/*
	polymodel *pm = model_get(Ship_info[Ships[sso->objp->instance].ship_info_index].model_num);

	if(pm == NULL)
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	bsp_info *sm = &pm->submodel[sso->ss->system_info->subobj_num];
	*/

	if(ADE_SETTING_VAR && v != NULL)
	{
		sso->ss->system_info->pnt = *v;
	}

	return ade_set_args(L, "o", l_Vector.Set(sso->ss->system_info->pnt));
}

ADE_VIRTVAR(GunPosition, l_Subsystem, "vector", "Subsystem gun position with regards to main ship (Local vector)", "vector", "Gun position, or null vector if subsystem handle is invalid")
{
	ship_subsys_h *sso;
	vec3d *v = NULL;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	polymodel *pm = model_get(Ship_info[Ships[sso->objp->instance].ship_info_index].model_num);
	Assert(pm != NULL);

	if(sso->ss->system_info->turret_gun_sobj < 0)
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	bsp_info *sm = &pm->submodel[sso->ss->system_info->turret_gun_sobj];

	if(ADE_SETTING_VAR && v != NULL)
		sm->offset = *v;

	return ade_set_args(L, "o", l_Vector.Set(sm->offset));
}

ADE_VIRTVAR(Name, l_Subsystem, "string", "Subsystem name", "string", "Subsystem name, or an empty string if handle is invalid")
{
	ship_subsys_h *sso;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Subsystem.GetPtr(&sso), &s))
		return ade_set_error(L, "s", "");

	if(!sso->IsValid())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL && strlen(s))
	{
		ship_subsys_set_name(sso->ss, s);
	}

	return ade_set_args(L, "s", ship_subsys_get_name(sso->ss));
}


ADE_VIRTVAR(PrimaryBanks, l_Subsystem, "weaponbanktype", "Array of primary weapon banks", "weaponbanktype", "Primary banks, or invalid weaponbanktype handle if subsystem handle is invalid")
{
	ship_subsys_h *sso, *sso2;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Subsystem.GetPtr(&sso2)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &sso->ss->weapons;

	if(ADE_SETTING_VAR && sso2->IsValid()) {
		ship_weapon *src = &sso2->ss->weapons;

		dst->current_primary_bank = src->current_primary_bank;
		dst->num_primary_banks = src->num_primary_banks;

		memcpy(dst->next_primary_fire_stamp, src->next_primary_fire_stamp, sizeof(dst->next_primary_fire_stamp));
		memcpy(dst->primary_animation_done_time, src->primary_animation_done_time, sizeof(dst->primary_animation_done_time));
		memcpy(dst->primary_animation_position, src->primary_animation_position, sizeof(dst->primary_animation_position));
		memcpy(dst->primary_bank_ammo, src->primary_bank_ammo, sizeof(dst->primary_bank_ammo));
		memcpy(dst->primary_bank_capacity, src->primary_bank_capacity, sizeof(dst->primary_bank_capacity));
		memcpy(dst->primary_bank_rearm_time, src->primary_bank_rearm_time, sizeof(dst->primary_bank_rearm_time));
		memcpy(dst->primary_bank_start_ammo, src->primary_bank_start_ammo, sizeof(dst->primary_bank_start_ammo));
		memcpy(dst->primary_bank_weapons, src->primary_bank_weapons, sizeof(dst->primary_bank_weapons));
		//memcpy(dst->primary_next_slot, src->primary_next_slot, sizeof(dst->primary_next_slot));
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(sso->objp, dst, SWH_PRIMARY)));
}

ADE_VIRTVAR(SecondaryBanks, l_Subsystem, "weaponbanktype", "Array of secondary weapon banks", "weaponbanktype", "Secondary banks, or invalid weaponbanktype handle if subsystem handle is invalid")
{
	ship_subsys_h *sso, *sso2;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Subsystem.GetPtr(&sso2)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &sso->ss->weapons;

	if(ADE_SETTING_VAR && sso2->IsValid()) {
		ship_weapon *src = &sso2->ss->weapons;

		dst->current_secondary_bank = src->current_secondary_bank;
		dst->num_secondary_banks = src->num_secondary_banks;

		memcpy(dst->next_secondary_fire_stamp, src->next_secondary_fire_stamp, sizeof(dst->next_secondary_fire_stamp));
		memcpy(dst->secondary_animation_done_time, src->secondary_animation_done_time, sizeof(dst->secondary_animation_done_time));
		memcpy(dst->secondary_animation_position, src->secondary_animation_position, sizeof(dst->secondary_animation_position));
		memcpy(dst->secondary_bank_ammo, src->secondary_bank_ammo, sizeof(dst->secondary_bank_ammo));
		memcpy(dst->secondary_bank_capacity, src->secondary_bank_capacity, sizeof(dst->secondary_bank_capacity));
		memcpy(dst->secondary_bank_rearm_time, src->secondary_bank_rearm_time, sizeof(dst->secondary_bank_rearm_time));
		memcpy(dst->secondary_bank_start_ammo, src->secondary_bank_start_ammo, sizeof(dst->secondary_bank_start_ammo));
		memcpy(dst->secondary_bank_weapons, src->secondary_bank_weapons, sizeof(dst->secondary_bank_weapons));
		memcpy(dst->secondary_next_slot, src->secondary_next_slot, sizeof(dst->secondary_next_slot));
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(sso->objp, dst, SWH_SECONDARY)));
}


ADE_VIRTVAR(Target, l_Subsystem, "object", "Object targetted by this subsystem", "object", "Targeted object, or invalid object handle if subsystem handle is invalid")
{
	ship_subsys_h *sso;
	object_h *objh;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Object.GetPtr(&objh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	ship_subsys *ss = sso->ss;

	if(ADE_SETTING_VAR && objh->IsValid())
	{
		ss->turret_enemy_objnum = OBJ_INDEX(objh->objp);
		ss->turret_enemy_sig = objh->sig;
		ss->targeted_subsys = NULL;
	}

	return ade_set_object_with_breed(L, ss->turret_enemy_objnum);
}

ADE_FUNC(isValid, l_Subsystem, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	ship_subsys_h *sso;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", sso->IsValid());
}

bool turret_fire_weapon(int weapon_num, ship_subsys *turret, int parent_objnum, vec3d *turret_pos, vec3d *turret_fvec, vec3d *predicted_pos = NULL, float flak_range_override = 100.0f);
ADE_FUNC(fireWeapon, l_Subsystem, "[Turret weapon index = 1, Flak range = 100]", "Fires weapon on turret", NULL, NULL)
{
	ship_subsys_h *sso;
	int wnum = 1;
	float flak_range = 100.0f;
	if(!ade_get_args(L, "o|if", l_Subsystem.GetPtr(&sso), &wnum, &flak_range))
		return ADE_RETURN_NIL;

	if(!sso->IsValid())
		return ADE_RETURN_NIL;

	wnum--;	//Lua->FS2

	//Get default turret info
	vec3d gpos, gvec;
	model_subsystem *tp = sso->ss->system_info;
	//ship_get_global_turret_info(sso->objp, sso->ss->system_info, &gpos, &gvec);

	//Rotate turret position with ship
	vm_vec_unrotate(&gpos, &tp->pnt, &sso->objp->orient);
	//Add turret position to appropriate world space
	vm_vec_add2(&gpos, &sso->objp->pos);

	//Rotate turret heading with turret base and gun
	//Now rotate a matrix by angles
	vec3d turret_heading = vmd_zero_vector;
	matrix m = IDENTITY_MATRIX;
	vm_rotate_matrix_by_angles(&m, &sso->ss->submodel_info_1.angs);
	vm_rotate_matrix_by_angles(&m, &sso->ss->submodel_info_2.angs);
	vm_vec_unrotate(&turret_heading, &tp->turret_norm, &m);

	//Rotate into world space
	vm_vec_unrotate(&gvec, &turret_heading, &sso->objp->orient);	

	bool rtn = turret_fire_weapon(wnum, sso->ss, OBJ_INDEX(sso->objp), &gpos, &gvec, NULL, flak_range);

	return ade_set_args(L, "b", rtn);
}

//**********HANDLE: shiptextures
ade_obj<object_h> l_ShipTextures("shiptextures", "Ship textures handle");

ADE_FUNC(__len, l_ShipTextures, NULL, "Number of textures on ship", "number", "Number of textures on ship, or 0 if handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_ShipTextures.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	polymodel *pm = model_get(Ship_info[Ships[objh->objp->instance].ship_info_index].model_num);

	if(pm == NULL)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", pm->n_textures*TM_NUM_TYPES);
}

ADE_INDEXER(l_ShipTextures, "number Index/string TextureFilename", "Array of ship textures", "texture", "Texture, or invalid texture handle on failure")
{
	object_h *oh;
	char *s;
	int tdx=-1;
	if (!ade_get_args(L, "os|o", l_ShipTextures.GetPtr(&oh), &s, l_Texture.Get(&tdx)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if (!oh->IsValid() || s==NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	ship *shipp = &Ships[oh->objp->instance];
	polymodel *pm = model_get(Ship_info[shipp->ship_info_index].model_num);
	int final_index = -1;
	int i;

	char fname[MAX_FILENAME_LEN];
	if (shipp->ship_replacement_textures != NULL)
	{
		for(i = 0; i < MAX_REPLACEMENT_TEXTURES; i++)
		{
			bm_get_filename(shipp->ship_replacement_textures[i], fname);

			if(!strextcmp(fname, s)) {
				final_index = i;
				break;
			}
		}
	}

	if(final_index < 0)
	{
		for (i = 0; i < pm->n_textures; i++)
		{
			int tm_num = pm->maps[i].FindTexture(s);
			if(tm_num > -1)
			{
				final_index = i*TM_NUM_TYPES+tm_num;
				break;
			}
		}
	}

	if (final_index < 0)
	{
		final_index = atoi(s) - 1;	//Lua->FS2

		if (final_index < 0 || final_index >= MAX_REPLACEMENT_TEXTURES)
			return ade_set_error(L, "o", l_Texture.Set(-1));
  	}

	//LuaError(L, "%d: %d", lua_type(L,lua_upvalueindex(2)), lua_toboolean(L,lua_upvalueindex(2)));
	if (ADE_SETTING_VAR) {
		if (shipp->ship_replacement_textures == NULL) {
			shipp->ship_replacement_textures = (int *) vm_malloc(MAX_REPLACEMENT_TEXTURES * sizeof(int));

			for (i = 0; i < MAX_REPLACEMENT_TEXTURES; i++)
				shipp->ship_replacement_textures[i] = -1;
		}

		if(bm_is_valid(tdx))
			shipp->ship_replacement_textures[final_index] = tdx;
		else
			shipp->ship_replacement_textures[final_index] = -1;
	}

	if (shipp->ship_replacement_textures != NULL && shipp->ship_replacement_textures[final_index] >= 0)
		return ade_set_args(L, "o", l_Texture.Set(shipp->ship_replacement_textures[final_index]));
	else
		return ade_set_args(L, "o", l_Texture.Set(pm->maps[final_index / TM_NUM_TYPES].textures[final_index % TM_NUM_TYPES].GetTexture()));
}

ADE_FUNC(isValid, l_ShipTextures, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_ShipTextures.GetPtr(&oh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", oh->IsValid());
}

//**********HANDLE: Ship
ade_obj<object_h> l_Ship("ship", "Ship handle", &l_Object);

ADE_INDEXER(l_Ship, "string Name/number Index", "Array of ship subsystems", "subsystem", "Subsystem handle, or invalid subsystem handle if index or ship handle is invalid")
{
	object_h *objh;
	char *s = NULL;
	ship_subsys_h *sub;
	if(!ade_get_args(L, "o|so", l_Ship.GetPtr(&objh), &s, l_Subsystem.GetPtr(&sub)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	ship *shipp = &Ships[objh->objp->instance];
	ship_subsys *ss = ship_get_subsys(shipp, s);

	if(ss == NULL)
	{
		int idx = atoi(s);
		if(idx > 0 && idx <= ship_get_num_subsys(shipp))
		{
			idx--; //Lua->FS2
			ss = ship_get_indexed_subsys(shipp, idx);
		}
	}

	if(ss == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(objh->objp, ss)));
}

ADE_FUNC(__len, l_Ship, NULL, "Number of subsystems on ship", "number", "Subsystem number, or 0 if handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", ship_get_num_subsys(&Ships[objh->objp->instance]));
}

ADE_VIRTVAR(Name, l_Ship, "string", "Ship name", "string", "Ship name, or empty string if handle is invalid")
{
	object_h *objh;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Ship.GetPtr(&objh), &s))
		return ade_set_error(L, "s", "");

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(shipp->ship_name, s, sizeof(shipp->ship_name)-1);
	}

	return ade_set_args(L, "s", shipp->ship_name);
}

ADE_VIRTVAR(AfterburnerFuelLeft, l_Ship, "number", "Afterburner fuel left", "number", "Afterburner fuel left, or 0 if handle is invalid")
{
	object_h *objh;
	float fuel = -1.0f;
	if(!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &fuel))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && fuel >= 0.0f)
		shipp->afterburner_fuel = fuel;

	return ade_set_args(L, "f", shipp->afterburner_fuel);
}

ADE_VIRTVAR(AfterburnerFuelMax, l_Ship, "number", "Afterburner fuel capacity", "number", "Afterburner fuel capacity, or 0 if handle is invalid")
{
	object_h *objh;
	float fuel = -1.0f;
	if(!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &fuel))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	ship_info *sip = &Ship_info[Ships[objh->objp->instance].ship_info_index];

	if(ADE_SETTING_VAR && fuel >= 0.0f)
		sip->afterburner_fuel_capacity = fuel;

	return ade_set_args(L, "f", sip->afterburner_fuel_capacity);
}

ADE_VIRTVAR(Class, l_Ship, "shipclass", "Ship class", "shipclass", "Ship class, or invalid shipclass handle if ship handle is invalid")
{
	object_h *objh;
	int idx=-1;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_Shipclass.Get(&idx)))
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && idx > -1)
		change_ship_type(objh->objp->instance, idx, 1);
		//shipp->ship_info_index = idx;

	if(shipp->ship_info_index < 0)
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	return ade_set_args(L, "o", l_Shipclass.Set(shipp->ship_info_index));
}

ADE_VIRTVAR(CountermeasuresLeft, l_Ship, "number", "Number of countermeasures left", "number", "Countermeasures left, or 0 if ship handle is invalid")
{
	object_h *objh;
	int newcm = -1;
	if(!ade_get_args(L, "o|i", l_Ship.GetPtr(&objh), &newcm))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && newcm > -1)
		shipp->cmeasure_count = newcm;

	return ade_set_args(L, "i", shipp->cmeasure_count);
}

ADE_VIRTVAR(CountermeasureClass, l_Ship, "weaponclass", "Weapon class mounted on this ship's countermeasure point", "weaponclass", "Countermeasure hardpoint weapon class, or invalid weaponclass handle if no countermeasure class or ship handle is invalid")
{
	object_h *objh;
	int newcm = -1;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_Weaponclass.Get(&newcm)))
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));;

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));;

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR) {
			shipp->current_cmeasure = newcm;
	}

	if(shipp->current_cmeasure > -1)
		return ade_set_args(L, "o", l_Weaponclass.Set(shipp->current_cmeasure));
	else
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));;
}

ADE_VIRTVAR(HitpointsMax, l_Ship, "number", "Total hitpoints", "number", "Ship maximum hitpoints, or 0 if handle is invalid")
{
	object_h *objh;
	float newhits = -1;
	if(!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &newhits))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && newhits > -1)
		shipp->ship_max_hull_strength = newhits;

	return ade_set_args(L, "f", shipp->ship_max_hull_strength);
}

ADE_VIRTVAR(PrimaryBanks, l_Ship, "weaponbanktype", "Array of primary weapon banks", "weaponbanktype", "Primary weapon banks, or invalid weaponbanktype handle if ship handle is invalid")
{
	object_h *objh;
	ship_banktype_h *swh;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_WeaponBankType.GetPtr(&swh)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &Ships[objh->objp->instance].weapons;

	if(ADE_SETTING_VAR && swh->IsValid()) {
		ship_weapon *src = &Ships[swh->objp->instance].weapons;

		dst->current_primary_bank = src->current_primary_bank;
		dst->num_primary_banks = src->num_primary_banks;

		memcpy(dst->next_primary_fire_stamp, src->next_primary_fire_stamp, sizeof(dst->next_primary_fire_stamp));
		memcpy(dst->primary_animation_done_time, src->primary_animation_done_time, sizeof(dst->primary_animation_done_time));
		memcpy(dst->primary_animation_position, src->primary_animation_position, sizeof(dst->primary_animation_position));
		memcpy(dst->primary_bank_ammo, src->primary_bank_ammo, sizeof(dst->primary_bank_ammo));
		memcpy(dst->primary_bank_capacity, src->primary_bank_capacity, sizeof(dst->primary_bank_capacity));
		memcpy(dst->primary_bank_rearm_time, src->primary_bank_rearm_time, sizeof(dst->primary_bank_rearm_time));
		memcpy(dst->primary_bank_start_ammo, src->primary_bank_start_ammo, sizeof(dst->primary_bank_start_ammo));
		memcpy(dst->primary_bank_weapons, src->primary_bank_weapons, sizeof(dst->primary_bank_weapons));
		//memcpy(dst->primary_next_slot, src->primary_next_slot, sizeof(dst->primary_next_slot));
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(objh->objp, dst, SWH_PRIMARY)));
}

ADE_VIRTVAR(SecondaryBanks, l_Ship, "weaponbanktype", "Array of secondary weapon banks", "weaponbanktype", "Secondary weapon banks, or invalid weaponbanktype handle if ship handle is invalid")
{
	object_h *objh;
	ship_banktype_h *swh;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_WeaponBankType.GetPtr(&swh)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &Ships[objh->objp->instance].weapons;

	if(ADE_SETTING_VAR && swh->IsValid()) {
		ship_weapon *src = &Ships[swh->objp->instance].weapons;

		dst->current_secondary_bank = src->current_secondary_bank;
		dst->num_secondary_banks = src->num_secondary_banks;

		memcpy(dst->next_secondary_fire_stamp, src->next_secondary_fire_stamp, sizeof(dst->next_secondary_fire_stamp));
		memcpy(dst->secondary_animation_done_time, src->secondary_animation_done_time, sizeof(dst->secondary_animation_done_time));
		memcpy(dst->secondary_animation_position, src->secondary_animation_position, sizeof(dst->secondary_animation_position));
		memcpy(dst->secondary_bank_ammo, src->secondary_bank_ammo, sizeof(dst->secondary_bank_ammo));
		memcpy(dst->secondary_bank_capacity, src->secondary_bank_capacity, sizeof(dst->secondary_bank_capacity));
		memcpy(dst->secondary_bank_rearm_time, src->secondary_bank_rearm_time, sizeof(dst->secondary_bank_rearm_time));
		memcpy(dst->secondary_bank_start_ammo, src->secondary_bank_start_ammo, sizeof(dst->secondary_bank_start_ammo));
		memcpy(dst->secondary_bank_weapons, src->secondary_bank_weapons, sizeof(dst->secondary_bank_weapons));
		memcpy(dst->secondary_next_slot, src->secondary_next_slot, sizeof(dst->secondary_next_slot));
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(objh->objp, dst, SWH_SECONDARY)));
}

ADE_VIRTVAR(TertiaryBanks, l_Ship, "weaponbanktype", "Array of tertiary weapon banks", "weaponbanktype", "Tertiary weapon banks, or invalid weaponbanktype handle if ship handle is invalid")
{
	object_h *objh;
	ship_banktype_h *swh;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_WeaponBankType.GetPtr(&swh)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &Ships[objh->objp->instance].weapons;

	if(ADE_SETTING_VAR && swh->IsValid()) {
		ship_weapon *src = &Ships[swh->objp->instance].weapons;

		dst->current_tertiary_bank = src->current_tertiary_bank;
		dst->num_tertiary_banks = src->num_tertiary_banks;

		dst->next_tertiary_fire_stamp = src->next_tertiary_fire_stamp;
		dst->tertiary_bank_ammo = src->tertiary_bank_ammo;
		dst->tertiary_bank_capacity = src->tertiary_bank_capacity;
		dst->tertiary_bank_rearm_time = src->tertiary_bank_rearm_time;
		dst->tertiary_bank_start_ammo = src->tertiary_bank_start_ammo;
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(objh->objp, dst, SWH_TERTIARY)));
}

ADE_VIRTVAR(Target, l_Ship, "object", "Target of ship. Value may also be a deriviative of the 'object' class, such as 'ship'.", "object", "Target object, or invalid object handle if no target or ship handle is invalid")
{
	object_h *objh;
	object_h *newh;
	//WMC - Maybe use two argument return capabilities of Lua to set/return subsystem?
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_Object.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	ai_info *aip = NULL;
	if(Ships[objh->objp->instance].ai_index > -1)
		aip = &Ai_info[Ships[objh->objp->instance].ai_index];
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR)
	{
		if(aip->target_signature != newh->sig)
		{
			if(newh->IsValid())
			{
				aip->target_objnum = OBJ_INDEX(newh->objp);
				aip->target_signature = newh->sig;
				aip->target_time = 0.0f;
			}
			else
			{
				aip->target_objnum = -1;
				aip->target_signature = 0;
				aip->target_time = 0.0f;
			}

			set_targeted_subsys(aip, NULL, -1);
		}
	}

	return ade_set_object_with_breed(L, aip->target_objnum);
}

ADE_VIRTVAR(TargetSubsystem, l_Ship, "subsystem", "Target subsystem of ship.", "subsystem", "Target subsystem, or invalid subsystem handle if no target or ship handle is invalid")
{
	object_h *oh;
	ship_subsys_h *newh;
	//WMC - Maybe use two argument return capabilities of Lua to set/return subsystem?
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&oh), l_Subsystem.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	ai_info *aip = NULL;
	if(Ships[oh->objp->instance].ai_index > -1)
		aip = &Ai_info[Ships[oh->objp->instance].ai_index];
	else
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR)
	{
		if(aip->target_signature != newh->sig)
		{
			if(newh->IsValid())
			{
				aip->target_objnum = OBJ_INDEX(newh->objp);
				aip->target_signature = newh->sig;
				aip->target_time = 0.0f;
				set_targeted_subsys(aip, newh->ss, aip->target_objnum);
			}
			else
			{
				aip->target_objnum = -1;
				aip->target_signature = 0;
				aip->target_time = 0.0f;

				set_targeted_subsys(aip, NULL, -1);
			}
		}
	}

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(&Objects[aip->target_objnum], aip->targeted_subsys)));
}

ADE_VIRTVAR(Team, l_Ship, "team", "Ship's team", "team", "Ship team, or invalid team handle if ship handle is invalid")
{
	object_h *oh=NULL;
	int nt=-1;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&oh), l_Team.Get(&nt)))
		return ade_set_error(L, "o", l_Team.Set(-1));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Team.Set(-1));

	ship *shipp = &Ships[oh->objp->instance];

	if(ADE_SETTING_VAR && nt > -1) {
		shipp->team = nt;
	}

	return ade_set_args(L, "o", l_Team.Set(shipp->team));
}

ADE_VIRTVAR(Textures, l_Ship, "shiptextures", "Gets ship textures", "shiptextures", "Ship textures, or invalid shiptextures handle if ship handle is invalid")
{
	object_h *sh;
	object_h *dh;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&dh), l_Ship.GetPtr(&sh)))
		return ade_set_error(L, "o", l_ShipTextures.Set(object_h()));

	if(!dh->IsValid())
		return ade_set_error(L, "o", l_ShipTextures.Set(object_h()));

	if(ADE_SETTING_VAR && sh != NULL && sh->IsValid()) {
		ship *src = &Ships[sh->objp->instance];
		ship *dest = &Ships[dh->objp->instance];
		
		if (src->ship_replacement_textures != NULL)
		{
			if (dest->ship_replacement_textures == NULL)
				dest->ship_replacement_textures = (int *) vm_malloc(MAX_REPLACEMENT_TEXTURES * sizeof(int));

			memcpy(dest->ship_replacement_textures, src->ship_replacement_textures, MAX_REPLACEMENT_TEXTURES * sizeof(int));
		}
	}

	return ade_set_args(L, "o", l_ShipTextures.Set(object_h(dh->objp)));
}

ADE_FUNC(kill, l_Ship, "[object Killer]", "Kills the ship. Set \"Killer\" to the ship you are killing to self-destruct", "boolean", "True if successful, false or nil otherwise")
{
	object_h *victim,*killer=NULL;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&victim), l_Ship.GetPtr(&killer)))
		return ADE_RETURN_NIL;

	if(!victim->IsValid())
		return ADE_RETURN_NIL;

	if(!killer->IsValid())
		killer = NULL;

	//Ripped straight from shiphit.cpp
	float percent_killed = -get_hull_pct(victim->objp);
	if (percent_killed > 1.0f){
		percent_killed = 1.0f;
	}

	ship_hit_kill(victim->objp, killer->objp, percent_killed, (victim->sig == killer->sig) ? 1 : 0);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(fireCountermeasure, l_Ship, NULL, "Launches a countermeasure from the ship", "boolean", "Whether countermeasure was launched or not")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ade_set_error(L, "b", false);

	if(!objh->IsValid())
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", ship_launch_countermeasure(objh->objp));
}

ADE_FUNC(firePrimary, l_Ship, NULL, "Fires ship primary bank(s)", "number", "Number of primary banks fired")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", ship_fire_primary(objh->objp, 0));
}

ADE_FUNC(fireSecondary, l_Ship, NULL, "Fires ship secondary bank(s)", "number", "Number of secondary banks fired")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", ship_fire_secondary(objh->objp, 0));
}

ADE_FUNC(getAnimationDoneTime, l_Ship, "number Type, number Subtype", "Gets time that animation will be done", "number", "Time (seconds), or 0 if ship handle is invalid")
{
	object_h *objh;
	char *s = NULL;
	int subtype=-1;
	if(!ade_get_args(L, "o|si", l_Ship.GetPtr(&objh), &s, &subtype))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	int type = model_anim_match_type(s);
	if(type < 0)
		return ADE_RETURN_FALSE;

	int time_ms = model_anim_get_time_type(&Ships[objh->objp->instance], type, subtype);
	float time_s = (float)time_ms * 1000.0f;

	return ade_set_args(L, "f", time_s);
}

ADE_FUNC(giveOrder, l_Ship, "enumeration Order, [object Target=nil, subsystem TargetSubsystem=nil, number Priority=1.0]", "Uses the goal code to execute orders", "boolean", "True if order was given, otherwise false or nil")
{
	object_h *objh = NULL;
	enum_h *eh = NULL;
	float priority = 1.0f;
	object_h *tgh = NULL;
	ship_subsys_h *tgsh = NULL;
	if(!ade_get_args(L, "oo|oof", l_Object.GetPtr(&objh), l_Enum.GetPtr(&eh), l_Object.GetPtr(&tgh), l_Subsystem.GetPtr(&tgsh), &priority))
		return ADE_RETURN_NIL;

	if(!objh->IsValid() || !eh->IsValid())
		return ade_set_error(L, "b", false);

	//wtf...
	if(priority < 0.0f)
		return ade_set_error(L, "b", false);

	if(priority > 1.0f)
		priority = 1.0f;

	bool tgh_valid = tgh->IsValid();
	bool tgsh_valid = tgsh->IsValid();
	int ai_mode = AI_GOAL_NONE;
	int ai_submode = -1234567;
	char *ai_shipname = NULL;
	switch(eh->index)
	{
		case LE_ORDER_ATTACK:
		{
			if(tgsh_valid)
			{
				ai_mode = AI_GOAL_DESTROY_SUBSYSTEM;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = ship_get_subsys_index( &Ships[tgsh->objp->instance], tgsh->ss->system_info->subobj_name );
			}
			else if(tgh_valid && tgh->objp->type == OBJ_WEAPON)
			{
				ai_mode = AI_GOAL_CHASE_WEAPON;
				ai_submode = tgh->objp->instance;
			}
			else if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_CHASE;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = SM_ATTACK;
			}
			break;
		}
		case LE_ORDER_DOCK:
		{
			ai_shipname = Ships[tgh->objp->instance].ship_name;
			ai_mode = AI_GOAL_DOCK;
			ai_submode = AIS_DOCK_0;
			break;
		}
		case LE_ORDER_WAYPOINTS:
		{
			if(tgh_valid && tgh->objp->type == OBJ_WAYPOINT)
			{
				ai_mode = AI_GOAL_WAYPOINTS;
				int wp_index = waypoint_get_list(tgh->objp);
				if(wp_index > -1)
					ai_shipname = Waypoint_lists[wp_index].name;
			}
			break;
		}
		case LE_ORDER_WAYPOINTS_ONCE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_WAYPOINT)
			{
				ai_mode = AI_GOAL_WAYPOINTS_ONCE;
				int wp_index = waypoint_get_list(tgh->objp);
				if(wp_index > -1)
					ai_shipname = Waypoint_lists[wp_index].name;
			}
			break;
		}
		case LE_ORDER_DEPART:
		{
			ai_mode = AI_GOAL_WARP;
			ai_submode = -1;
			break;
		}
		case LE_ORDER_FORM_ON_WING:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_FORM_ON_WING;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = 0;
			}
			break;
		}
		case LE_ORDER_UNDOCK:
		{
			ai_mode = AI_GOAL_UNDOCK;
			ai_submode = AIS_UNDOCK_0;

			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_GUARD:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_GUARD;
				ai_submode = AIS_GUARD_PATROL;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_DISABLE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_DISABLE_SHIP;
				ai_submode = -SUBSYSTEM_ENGINE;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_DISARM:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_DISARM_SHIP;
				ai_submode = -SUBSYSTEM_TURRET;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_ATTACK_ANY:
		{
			ai_mode = AI_GOAL_CHASE_ANY;
			ai_submode = SM_ATTACK;
			break;
		}
		case LE_ORDER_IGNORE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_IGNORE_NEW;
				ai_submode = 0;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_EVADE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_EVADE_SHIP;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
		}
		case LE_ORDER_STAY_NEAR:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_STAY_NEAR_SHIP;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = -1;
			}
			break;
		}
		case LE_ORDER_KEEP_SAFE_DISTANCE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_KEEP_SAFE_DISTANCE;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = -1;
			}
			break;
		}
		case LE_ORDER_REARM:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_REARM_REPAIR;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = 0;
			}
			break;
		}
		case LE_ORDER_STAY_STILL:
		{
			ai_mode = AI_GOAL_STAY_STILL;
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_PLAY_DEAD:
		{
			ai_mode = AI_GOAL_PLAY_DEAD;
			break;
		}
		case LE_ORDER_FLY_TO:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_FLY_TO_SHIP;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = 0;
			}
			break;
		}
	}

	//Nothing got set!
	if(ai_mode == AI_GOAL_NONE)
		return ade_set_error(L, "b", false);

	//Fire off the goal
	ai_add_ship_goal_scripting(ai_mode, ai_submode, (int)(priority*100.0f), ai_shipname, &Ai_info[Ships[objh->objp->instance].ai_index]);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(triggerAnimation, l_Ship, "string Type, [number Subtype, boolean Forwards]",
		 "Triggers an animation. Type is the string name of the animation type, "
		 "Subtype is the subtype number, such as weapon bank #, and Forwards is boolean."
		 "<br><strong>IMPORTANT: Function is in testing and should not be used with official mod releases</strong>",
		 "boolean",
		 "True if successful, false or nil otherwise")
{
	object_h *objh;
	char *s = NULL;
	bool b = true;
	int subtype=-1;
	if(!ade_get_args(L, "o|sib", l_Ship.GetPtr(&objh), &s, &subtype, &b))
		return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	int type = model_anim_match_type(s);
	if(type < 0)
		return ADE_RETURN_FALSE;

	int dir = 1;
	if(!b)
		dir = -1;

	model_anim_start_type(&Ships[objh->objp->instance], type, subtype, dir);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(warpIn, l_Ship, NULL, "Warps ship in", "boolean", "True if successful, or nil if ship handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	shipfx_warpin_start(objh->objp);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(warpOut, l_Ship, NULL, "Warps ship out", "boolean", "True if successful, or nil if ship handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	shipfx_warpout_start(objh->objp);

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Weapon
ade_obj<object_h> l_Weapon("weapon", "Weapon handle", &l_Object);

ADE_VIRTVAR(Class, l_Weapon, "weaponclass", "Weapon's class", "weaponclass", "Weapon class, or invalid weaponclass handle if weapon handle is invalid")
{
	object_h *oh=NULL;
	int nc=-1;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&oh), l_Weaponclass.Get(&nc)))
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	weapon *wp = &Weapons[oh->objp->instance];

	if(ADE_SETTING_VAR && nc > -1) {
		wp->weapon_info_index = nc;
	}

	return ade_set_args(L, "o", l_Weaponclass.Set(wp->weapon_info_index));
}

ADE_VIRTVAR(DestroyedByWeapon, l_Weapon, "boolean", "Whether weapon was destroyed by another weapon", "boolean", "True if weapon was destroyed by another weapon, false if weapon was destroyed by another object or if weapon handle is invalid")
{
	object_h *oh=NULL;
	bool b = false;

	int numargs = ade_get_args(L, "o|b", l_Weapon.GetPtr(&oh), &b);
	
	if(!numargs)
		return ade_set_error(L, "b", false);

	if(!oh->IsValid())
		return ade_set_error(L, "b", false);

	weapon *wp = &Weapons[oh->objp->instance];

	if(ADE_SETTING_VAR && numargs > 1) {
		if(b)
			wp->weapon_flags |= WF_DESTROYED_BY_WEAPON;
		else
			wp->weapon_flags &= ~WF_DESTROYED_BY_WEAPON;
	}

	return ade_set_args(L, "b", (wp->weapon_flags & WF_DESTROYED_BY_WEAPON) > 0);
}

ADE_VIRTVAR(LifeLeft, l_Weapon, "number", "Weapon life left (in seconds)", "number", "Life left (seconds) or 0 if weapon handle is invalid")
{
	object_h *oh=NULL;
	float nll = -1.0f;
	if(!ade_get_args(L, "o|f", l_Weapon.GetPtr(&oh), &nll))
		return ade_set_error(L, "f", 0.0f);

	if(!oh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	weapon *wp = &Weapons[oh->objp->instance];

	if(ADE_SETTING_VAR && nll >= 0.0f) {
		wp->lifeleft = nll;
	}

	return ade_set_args(L, "f", wp->lifeleft);
}

ADE_VIRTVAR(Target, l_Weapon, "object", "Target of weapon. Value may also be a deriviative of the 'object' class, such as 'ship'.", "object", "Weapon target, or invalid object handle if weapon handle is invalid")
{
	object_h *objh;
	object_h *newh;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&objh), l_Object.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	weapon *wp = NULL;
	if(objh->objp->instance > -1)
		wp = &Weapons[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh != NULL && newh->IsValid())
		{
			if(wp->target_sig != newh->sig)
			{
				wp->target_num = OBJ_INDEX(newh->objp);
				wp->target_sig = newh->sig;
			}
		}
		else
		{
			wp->target_num = -1;
			wp->target_sig = 0;
		}
	}

	return ade_set_object_with_breed(L, wp->target_num);
}

ADE_VIRTVAR(HomingObject, l_Weapon, "object", "Object that weapon will home in on. Value may also be a deriviative of the 'object' class, such as 'ship'", "object", "Object that weapon is homing in on, or an invalid object handle if weapon is not homing or the weapon handle is invalid")
{
	object_h *objh;
	object_h *newh;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&objh), l_Object.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	weapon *wp = NULL;
	if(objh->objp->instance > -1)
		wp = &Weapons[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh != NULL && newh->IsValid())
		{
			if(wp->target_sig != newh->sig)
			{
				wp->homing_object = newh->objp;
				wp->homing_pos = newh->objp->pos;
				wp->homing_subsys = NULL;
			}
		}
		else
		{
			wp->homing_object = NULL;
			wp->homing_pos = vmd_zero_vector;
			wp->homing_subsys = NULL;
		}
	}

	if(wp->homing_object == NULL)
		return ade_set_args(L, "o", l_Object.Set(object_h()));
	else
		return ade_set_object_with_breed(L, OBJ_INDEX(wp->homing_object));
}

ADE_VIRTVAR(HomingPosition, l_Weapon, "vector", "Position that weapon will home in on (World vector)", "vector", "Homing point, or null vector if weapon handle is invalid")
{
	object_h *objh;
	vec3d *v3;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&objh), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	weapon *wp = NULL;
	if(objh->objp->instance > -1)
		wp = &Weapons[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR)
	{
		if(v3 != NULL)
		{
			wp->homing_object = NULL;
			wp->homing_subsys = NULL;
			wp->homing_pos = *v3;
		}
		else
		{
			wp->homing_object = NULL;
			wp->homing_subsys = NULL;
			wp->homing_pos = vmd_zero_vector;
		}
	}

	return ade_set_args(L, "o", l_Vector.Set(wp->homing_pos));
}

ADE_VIRTVAR(HomingSubsystem, l_Weapon, "subsystem", "Subsystem that weapon will home in on.", "subsystem", "Homing subsystem, or invalid subsystem handle if weapon is not homing or weapon handle is invalid")
{
	object_h *objh;
	ship_subsys_h *newh;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&objh), l_Subsystem.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	weapon *wp = NULL;
	if(objh->objp->instance > -1)
		wp = &Weapons[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh != NULL && newh->IsValid())
		{
			if(wp->target_sig != newh->sig)
			{
				wp->homing_object = newh->objp;
				wp->homing_subsys = newh->ss;
				get_subsystem_pos(&wp->homing_pos, wp->homing_object, wp->homing_subsys);
			}
		}
		else
		{
			wp->homing_object = NULL;
			wp->homing_pos = vmd_zero_vector;
			wp->homing_subsys = NULL;
		}
	}

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(wp->homing_object, wp->homing_subsys)));
}

ADE_VIRTVAR(Team, l_Weapon, "team", "Weapon's team", "team", "Weapon team, or invalid team handle if weapon handle is invalid")
{
	object_h *oh=NULL;
	int nt=-1;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&oh), l_Team.Get(&nt)))
		return ade_set_error(L, "o", l_Team.Set(-1));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Team.Set(-1));

	weapon *wp = &Weapons[oh->objp->instance];

	if(ADE_SETTING_VAR && nt > -1 && nt < Num_teams) {
		wp->team = nt;
	}

	return ade_set_args(L, "o", l_Team.Set(wp->team));
}


//**********HANDLE: Wing
ade_obj<int> l_Wing("wing", "Wing handle");

ADE_INDEXER(l_Wing, "number Index", "Array of ships in the wing", "ship", "Ship handle, or invalid ship handle if index is invawing handle is invalid")
{
	int wdx;
	int sdx;
	object_h *ndx=NULL;
	if(!ade_get_args(L, "oi|o", l_Wing.Get(&wdx), &sdx, l_Ship.GetPtr(&ndx)))
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	if(sdx < 1 || sdx < Wings[wdx].current_count) {
		return ade_set_error(L, "o", l_Ship.Set(object_h()));
	}

	//Lua-->FS2
	sdx--;

	if(ADE_SETTING_VAR && ndx != NULL && ndx->IsValid()) {
		Wings[wdx].ship_index[sdx] = ndx->objp->instance;
	}

	return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[Ships[Wings[wdx].ship_index[sdx]].objnum])));
}
//**********HANDLE: Player
ade_obj<int> l_Player("player", "Player handle");

ADE_FUNC(isValid, l_Player, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Player_num)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getName, l_Player, NULL, "Gets current player name", "string", "Player name, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Player_num)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Players[idx].callsign);
}

ADE_FUNC(getCampaignFilename, l_Player, NULL, "Gets current player campaign filename", "string", "Campaign name, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Player_num)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Players[idx].current_campaign);
}

ADE_FUNC(getImageFilename, l_Player, NULL, "Gets current player image filename", "string", "Player image filename, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Player_num)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Players[idx].image_filename);
}


ADE_FUNC(getMainHall, l_Player, NULL, "Gets player's main hall number", "number", "Main hall index, or 1 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "i", 1);

	if(idx < 0 || idx >= Player_num)
		return ade_set_error(L, "i", 1);

	//FS2-->Lua
	int hallnum = (int)Players[idx].main_hall + 1;

	return ade_set_args(L, "i", hallnum);
}

ADE_FUNC(getSquadronName, l_Player, NULL, "Gets current player squad name", "string", "Squadron name, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Player_num)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Players[idx].squad_name);
}

//WMC - This isn't working
/*
ADE_FUNC(getSquadronImage, l_Player, NULL, "Squad image (string)", "Gets current player squad image")
{
	int idx;
	player_helper(L, &idx);

	return ade_set_args(L, "s", Players[idx].squad_filename);
}*/

//**********HANDLE: Camera
ade_obj<camid> l_Camera("camera", "Camera handle");

ADE_FUNC(__tostring, l_Camera, NULL, "Camera name", "string", "Camera name, or an empty string if handle is invalid")
{
	camid cid;
	if(!ade_get_args(L, "o", l_Camera.Get(&cid)))
		return ade_set_error(L, "s", "");

	if(!cid.isValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", cid.getCamera()->get_name());
}

ADE_FUNC(isValid, l_Camera, NULL, "True if valid, false or nil if not", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	camid cid;
	if(!ade_get_args(L, "o", l_Camera.Get(&cid)))
		return ADE_RETURN_NIL;

	if(!cid.isValid())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_VIRTVAR(Name, l_Camera, "string", "New camera name", "string", "Camera name")
{
	camid cid;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Camera.Get(&cid), &s))
		return ade_set_error(L, "s", "");

	if(!cid.isValid())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		cid.getCamera()->set_name(s);
	}

	return ade_set_args(L, "s", cid.getCamera()->get_name());
}

ADE_VIRTVAR(FOV, l_Camera, "number", "New camera FOV (in radians)", "number", "Camera FOV (in radians)")
{
	camid cid;
	float f = VIEWER_ZOOM_DEFAULT;
	if(!ade_get_args(L, "o|f", l_Camera.Get(&cid), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!cid.isValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		cid.getCamera()->set_fov(f);
	}

	return ade_set_args(L, "f", cid.getCamera()->get_fov());
}

ADE_VIRTVAR(Orientation, l_Camera, "orientation", "New camera orientation", "orientation", "Camera orientation")
{
	camid cid;
	matrix_h *mh = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "f", 0.0f);

	if(!cid.isValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && mh != NULL) {
		cid.getCamera()->set_rotation(mh->GetMatrix());
	}

	matrix mtx;
	cid.getCamera()->get_info(NULL, &mtx);
	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&mtx)));
}

ADE_VIRTVAR(Position, l_Camera, "vector", "New camera position", "vector", "Camera position")
{
	camid cid;
	vec3d *pos = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Vector.GetPtr(&pos)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR) {
		cid.getCamera()->set_position(pos);
	}

	vec3d v = vmd_zero_vector;
	cid.getCamera()->get_info(&v, NULL);
	return ade_set_args(L, "o", l_Vector.Set(v));
}

ADE_VIRTVAR(Self, l_Camera, "object", "New mount object", "object", "Camera object")
{
	camid cid;
	object_h *oh = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Object.GetPtr(&oh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR && oh->IsValid()) {
		cid.getCamera()->set_object_host(oh->objp);
	}

	return ade_set_args(L, "o", l_Object.Set(object_h(cid.getCamera()->get_object_host())));
}

ADE_VIRTVAR(SelfSubsystem, l_Camera, "subsystem", "New mount object subsystem", "subsystem", "Subsystem that the camera is mounted on")
{
	camid cid;
	ship_subsys_h *sso = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR && sso->IsValid()) {
		cid.getCamera()->set_object_host(sso->objp, sso->ss->system_info->subobj_num);
	}

	object *objp = cid.getCamera()->get_object_host();
	if(objp == NULL || objp->type != OBJ_SHIP)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	int submodel = cid.getCamera()->get_object_host_submodel();
	if(submodel < 0)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	ship *shipp = &Ships[objp->instance];
	polymodel *pm = model_get(Ship_info[shipp->ship_info_index].model_num);

	if(pm == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	bsp_info *sm = &pm->submodel[submodel];

	ship_subsys *ss = ship_get_subsys(shipp, sm->name);
	
	if(ss == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(objp, ss)));
}

ADE_VIRTVAR(Target, l_Camera, "object", "New target object", "object", "Camera target object")
{
	camid cid;
	object_h *oh = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Object.GetPtr(&oh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR && oh->IsValid()) {
		cid.getCamera()->set_object_target(oh->objp);
	}

	return ade_set_args(L, "o", l_Object.Set(object_h(cid.getCamera()->get_object_target())));
}

ADE_VIRTVAR(TargetSubsystem, l_Camera, "subsystem", "New target subsystem", "subsystem", "Subsystem that the camera is pointed at")
{
	camid cid;
	ship_subsys_h *sso = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR && sso->IsValid()) {
		cid.getCamera()->set_object_target(sso->objp, sso->ss->system_info->subobj_num);
	}

	object *objp = cid.getCamera()->get_object_target();
	if(objp == NULL || objp->type != OBJ_SHIP)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	int submodel = cid.getCamera()->get_object_target_submodel();
	if(submodel < 0)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	ship *shipp = &Ships[objp->instance];
	polymodel *pm = model_get(Ship_info[shipp->ship_info_index].model_num);

	if(pm == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	bsp_info *sm = &pm->submodel[submodel];

	ship_subsys *ss = ship_get_subsys(shipp, sm->name);
	
	if(ss == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(objp, ss)));
}

ADE_FUNC(setFOV, l_Camera, "[number FOV, number Zoom Time, number Zoom Acceleration Time]",
		 "Sets camera FOV"
		 "<br>FOV is the final field of view, in radians, of the camera."
		 "<br>Zoom Time is the total time to take zooming in or out."
		 "<br>Acceleration Time is the total time it should take the camera to get up to full zoom speed."
		 "<br>Deceleration Time is the total time it should take the camera to slow down from full zoom speed.",
		 "boolean", "true if successful, false or nil otherwise")
{
	camid cid;
	float n_fov = VIEWER_ZOOM_DEFAULT;
	float time=0.0f;
	float acc_time=0.0f;
	float dec_time=0.0f;
	if(!ade_get_args(L, "o|offf", l_Camera.Get(&cid), &n_fov, &time, &acc_time, &dec_time))
		return ADE_RETURN_NIL;
	
	if(!cid.isValid())
		return ADE_RETURN_NIL;

	cid.getCamera()->set_fov(n_fov, time, acc_time, dec_time);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(setOrientation, l_Camera, "[world orientation Orientation, number Rotation Time, number Acceleration Time, number Deceleration time]",
		"Sets camera orientation and velocity data."
		"<br>Orientation is the final orientation for the camera, after it has finished moving. If not specified, the camera will simply stop at its current orientation."
		"<br>Rotation time is how long total, including acceleration, the camera should take to rotate. If it is not specified, the camera will jump to the specified orientation."
		"<br>Acceleration time is how long it should take the camera to get 'up to speed'. If not specified, the camera will instantly start moving."
		"<br>Deceleration time is how long it should take the camera to slow down. If not specified, the camera will instantly stop moving.",
		"boolean", "true if successful, false or nil otherwise")
{
	camid cid;
	matrix_h *mh=NULL;
	float time=0.0f;
	float acc_time=0.0f;
	float dec_time=0.0f;
	if(!ade_get_args(L, "o|offf", l_Camera.Get(&cid), l_Matrix.GetPtr(&mh), &time, &acc_time, &dec_time))
		return ADE_RETURN_NIL;
	
	if(!cid.isValid())
		return ADE_RETURN_NIL;

	camera *cam = cid.getCamera();

	if(mh != NULL)
	{
		cam->set_rotation(mh->GetMatrix(), time, acc_time, dec_time);
	}
	else
	{
		cam->set_rotation();
	}

	return ADE_RETURN_TRUE;
}

ADE_FUNC(setPosition, l_Camera, "[wvector Position, number Translation Time, number Acceleration Time, number Deceleration Time]",
		"Sets camera position and velocity data."
		"<br>Position is the final position for the camera. If not specified, the camera will simply stop at its current position."
		"<br>Translation time is how long total, including acceleration, the camera should take to move. If it is not specified, the camera will jump to the specified position."
		"<br>Acceleration time is how long it should take the camera to get 'up to speed'. If not specified, the camera will instantly start moving."
		"<br>Deceleration time is how long it should take the camera to slow down. If not specified, the camera will instantly stop moving.",
		"boolean", "true if successful, false or nil otherwise")
{
	camid cid;
	vec3d *pos=NULL;
	float time=0.0f;
	float acc_time=0.0f;
	float dec_time=0.0f;
	if(!ade_get_args(L, "o|offf", l_Camera.Get(&cid), l_Vector.GetPtr(&pos), &time, &acc_time, &dec_time))
		return ADE_RETURN_NIL;
	
	if(!cid.isValid())
		return ADE_RETURN_NIL;

	cid.getCamera()->set_position(pos, time, acc_time, dec_time);

	return ADE_RETURN_TRUE;
}

//**********HANDLE: SoundEntry
struct sound_entry_h
{
	int type;
	int idx;

	sound_entry_h(){type=idx=-1;}
	sound_entry_h(int n_type, int n_idx){type=n_type;idx=n_idx;}
	bool IsValid()
	{
		//WMC - sound stuff is under construction
		return false;
		/*
		if((type < 0 || type > GS_NUM_SND_TYPES || idx < 0)
			|| (type == GS_GAME_SND && idx >= Num_game_sounds)
			|| (type == GS_IFACE_SND && idx >= Num_iface_sounds))
		{
			return false;
		}

		return true;
		*/
	}
};

ade_obj<sound_entry_h> l_SoundEntry("soundentry", "sounds.tbl table entry handle");

ADE_FUNC(isValid, l_SoundEntry, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	sound_entry_h *seh;
	if(!ade_get_args(L, "o", l_SoundEntry.GetPtr(&seh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", seh->IsValid());
}

//**********HANDLE: Sound
struct sound_h : public sound_entry_h
{
	int sig;
	sound_h():sound_entry_h(){sig=-1;}
	sound_h(int n_gs_type, int n_gs_idx, int n_sig):sound_entry_h(n_gs_type, n_gs_idx){sig=n_sig;}
	bool IsValid()
	{
		if(!sound_entry_h::IsValid())
			return false;

		if(sig < 0 || ds_get_channel(sig) < 0)
			return false;

		return true;
	}
};

ade_obj<sound_h> l_Sound("sound", "sound instance handle");

/*
ADE_VIRTVAR(Pan, l_Sound, "number", "Panning of sound, from -1.0 to 1.0")
{
	sound_h *sh;
	float newpan=0.0f;
	if(!ade_get_args(L, "o|f", l_Sound.GetPtr(&sh), &newpan))
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR)
	{
		if(newpan < -1.0f)
			newpan = -1.0f;
		if(newpan > 1.0f)
			newpan = 1.0f;

		snd_set_pan(sh->sig, newpan);
	}

	return ade_set_args(L, "f", snd_get_pan(sh->sig));
}*/

ADE_VIRTVAR(Pitch, l_Sound, "number", "Panning of sound, from 100 to 100000", "number", "Pitch, or 0 if handle is invalid")
{
	sound_h *sh;
	int newpitch = 100;
	if(!ade_get_args(L, "o|i", l_Sound.GetPtr(&sh), &newpitch))
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR)
	{
		if(newpitch < 100)
			newpitch = 100;
		if(newpitch > 100000)
			newpitch = 100000;

		snd_set_pitch(sh->sig, newpitch);
	}

	return ade_set_args(L, "f", snd_get_pitch(sh->sig));
}

/*
ADE_VIRTVAR(Volume, l_Sound, "number", "Volume of sound, from 0.0 to 1.0", "number", "Volume, or 0 if handle is invalid")
{
	sound_h *sh;
	float newvol=-1.0f;
	if(!ade_get_args(L, "o|f", l_Sound.GetPtr(&sh), &newvol))
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR)
	{
		if(newvol < 0.0f)
			newvol = 0.0f;
		if(newvol > 1.0f)
			newvol = 1.0f;

		snd_set_volume(sh->sig, newvol);
	}

	return ade_set_args(L, "f", snd_get_volume(sh->sig));
}
*/

ADE_FUNC(isValid, l_Sound, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	sound_h *sh;
	if(!ade_get_args(L, "o", l_Sound.GetPtr(&sh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", sh->IsValid());
}
/*


ADE_VIRTVAR(Position, l_Sound, "vector", "Position of sound (World vector)", "vector", "Sound position, or null vector if handle is invalid")
{
	vec3d *vp;
	int sh;
	if(!ade_get_args(L, "o|o", l_Sound.Get(&sh), l_Vector.GetPtr(&vp)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_identity_vector));

	if(sh < 0)
		return ade_set_error(L, "o", l_Vector.Set(vmd_identity_vector));
}*/

//**********LIBRARY: Audio
ade_lib l_Audio("Audio", NULL, "ad", "Sound/Music Library");

/*
ADE_FUNC(playGameSound, l_Audio, "Sound filename, [Panning (-1.0 left to 1.0 right), Volume %, Priority 0-3, Voice Message?]", "True if sound was played, false if not (Replaced with a sound instance object in the future)", "Plays a sound from #Game Sounds in sounds.tbl. A priority of 0 indicates that the song must play; 1-3 will specify the maximum number of that sound that can be played")
{
	char *s;
	float pan=0.0f;
	float vol=100.0f;
	int pri=0;
	bool voice_msg = false;
	if(!ade_get_args(L, "s|ffib", &s, &pan, &vol, &pri, &voice_msg))
		return ADE_RETURN_NIL;

	int idx = gamesnd_get_by_name(s);
	
	if(idx < 0)
		return ADE_RETURN_FALSE;

	if(pri < 0 || pri > 3)
		pri = 0;

	if(pan < -1.0f)
		pan = -1.0f;
	if(pan > 1.0f)
		pan = 1.0f;
	if(vol < 0.0f)
		vol = 0.0f;
	if(vol > 100.0f)
		vol = 100.0f;

	idx = snd_play(&Snds[idx], pan, vol*0.01f, pri, voice_msg);

	return ade_set_args(L, "b", idx > -1);
}

ADE_FUNC(playInterfaceSound, l_Audio, "Sound filename", "True if sound was played, false if not", "Plays a sound from #Interface Sounds in sounds.tbl")
{
	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "b", false);

	int idx;
	for(idx = 0; idx < Num_iface_sounds; idx++)
	{
		if(!strextcmp(Snds_iface[idx].filename, s))
			break;
	}
	
	if(idx == Num_iface_sounds)
		return ADE_RETURN_FALSE;

	gamesnd_play_iface(idx);

	return ade_set_args(L, "b", idx > -1);
}
*/
/*
class track_h
{
private:
	int tdx;
public:
	track_h(){tdx=-1;}
	track_h(int n_track){tdx=n_track;}

	bool IsValid(){return (this != NULL && tdx > -1 && tdx < MAX_AUDIO_STREAMS);}

	int Get(){return tdx;}
};

ade_obj<track_h> l_Track("track", "Music track");
*/
ADE_FUNC(playMusic, l_Audio, "string Filename", "Plays a music file using FS2Open's builtin music system", NULL, NULL)
{
	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "b", false);

	int ah = audiostream_open(s, ASF_MENUMUSIC);
	if(ah < 0)
		return ade_set_error(L, "b", false);

	audiostream_play(ah);
	return ADE_RETURN_NIL;
}

//**********LIBRARY: Base
ade_lib l_Base("Base", NULL, "ba", "Base FreeSpace 2 functions");

ADE_FUNC(print, l_Base, "string Message", "Prints a string", NULL, NULL)
{
	mprintf(("%s", lua_tostring(L, -1)));

	return ADE_RETURN_NIL;
}

ADE_FUNC(warning, l_Base, "string Message", "Displays a FreeSpace warning (debug build-only) message with the string provided", NULL, NULL)
{
	Warning(LOCATION, "%s", lua_tostring(L, -1));

	return ADE_RETURN_NIL;
}

ADE_FUNC(error, l_Base, "string Message", "Displays a FreeSpace error message with the string provided", NULL, NULL)
{
	Error(LOCATION, "%s", lua_tostring(L, -1));

	return ADE_RETURN_NIL;
}

ADE_FUNC(createOrientation, l_Base, "[p/r1c1, b/r1c2, h/r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3]", "Given 0, 3, or 9 arguments, creates an orientation object with that orientation.", "orientation", "New orientation object, or null orientation on failure")
{
	matrix m;
	int numargs = ade_get_args(L, "|fffffffff", &m.a1d[0], &m.a1d[1], &m.a1d[2], &m.a1d[3], &m.a1d[4], &m.a1d[5], &m.a1d[6], &m.a1d[7], &m.a1d[8]);
	if(!numargs)
	{
		return ade_set_args(L, "o", l_Matrix.Set( matrix_h(&vmd_identity_matrix) ));
	}
	else if(numargs == 3)
	{
		angles a = {m.a1d[0], m.a1d[1], m.a1d[2]};
		return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&a)));
	}
	else if(numargs == 9)
	{
		return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&m)));
	}

	return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));
}

ADE_FUNC(createVector, l_Base, "[x, y, z]", "Creates a vector object", "vector", "Vector object")
{
	vec3d v3 = vmd_zero_vector;
	ade_get_args(L, "|fff", &v3.xyz.x, &v3.xyz.y, &v3.xyz.z);

	return ade_set_args(L, "o", l_Vector.Set(v3));
}

ADE_FUNC(getFrametime, l_Base, "[Do not adjust for time compression (Boolean)]", "Gets how long this frame is calculated to take. Use it to for animations, physics, etc to make incremental changes.", "number", "Frame time (seconds)")
{
	bool b=false;
	ade_get_args(L, "|b", &b);

	return ade_set_args(L, "f", b ? flRealframetime : flFrametime);
}

ADE_FUNC(getCurrentGameState, l_Base, "[Depth (number)]", "Gets current FreeSpace state; if a depth is specified, the state at that depth is returned. (IE at the in-game options game, a depth of 1 would give you the game state, while the function defaults to 0, which would be the options screen.", "state", "Current game state at specified depth, or invalid handle if no game state is active yet")
{
	int depth = 0;
	ade_get_args(L, "|i", &depth);

	if(depth > gameseq_get_depth())
		return ade_set_args(L, "o", l_GameState.Set(gamestate_h()));

	return ade_set_args(L, "o", l_GameState.Set(gamestate_h(gameseq_get_state(depth))));
}

/*
ADE_FUNC(getStateNameByIndex, l_Base, "Index of state (number)", "Gets the name of a state type by its index; this function may be used to list all state types.", "string", "State name, or an empty string if index is invalid")
{
	int i;
	if(!ade_get_args(L, "i", &i))
		return ade_set_error(L, "s", "");

	//Lua->FS2
	i--;

	if(i < 0 || i >= Num_gs_state_text)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", GS_state_text[i]);
}

ADE_FUNC(getNumStates, l_Base, NULL, "Number of states", "Gets the number of different state types currently implemented in FS2_Open")
{
	return ade_set_args(L, "i", Num_gs_state_text);
}*/

ADE_FUNC(postGameEvent, l_Base, "event Event", "Sets current game event. Note that you can crash FreeSpace 2 by posting an event at an improper time, so test extensively if you use it.", "boolean", "True if event was posted, false if passed event was invalid")
{
	gameevent_h *gh = NULL;
	if(!ade_get_args(L, "o", l_GameEvent.GetPtr(&gh)))
		return ade_set_error(L, "b", false);

	if(!gh->IsValid())
		return ade_set_error(L, "b", false);

	gameseq_post_event(gh->Get());

	return ADE_RETURN_TRUE;
}
/*
ADE_FUNC(getEventNameByIndex, l_Base, "Index of event type (number)", "Event name (string)", "Gets the name of a event type, given an index; this function may be used to list all event dealt with by setEvent()")
{
	int i;
	if(!ade_get_args(L, "i", &i))
		return ade_set_error(L, "s", "");

	//Lua->FS2
	i--;

	if(i < 0 || i >= Num_gs_event_text)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", GS_event_text[i]);
}

ADE_FUNC(getNumEvents, l_Base, NULL, "Number of event types", "Gets the number of different event types currently implemented in FS2")
{
	return ade_set_args(L, "i", Num_gs_event_text);
}
*/
/*
ADE_FUNC(getCurrentPlayer, l_Base, NULL, "Current player", "Gets the current player")
{
	if(Player == NULL)
		return ADE_RETURN_NIL;

	int idx = Player - Players;
	return ade_set_args(L, "o", l_Player.Set(idx));
}

ADE_FUNC(getNumPlayers, l_Base, NULL, "Number of players", "Gets the number of currently loaded players")
{
	return ade_set_args(L, "i", Player_num);
}

ADE_FUNC(getPlayerByIndex, l_Base, "Player index", "Player object", "Gets the named player")
{
	if(Player == NULL)
		return ADE_RETURN_NIL;

	int idx;
	if(!ade_get_args(L, "i", &idx))
		return ADE_RETURN_NIL;

	//Lua->FS2
	idx--;

	if(idx < 0 || idx > Player_num)
		return ADE_RETURN_NIL;

	return ade_set_args(L, "o", l_Player.Set(idx));
}
*/

//**********SUBLIBRARY: Base/Events
ade_lib l_Base_Events("GameEvents", &l_Base, NULL, "Freespace 2 game events");

ADE_INDEXER(l_Base_Events, "number Index/string Name", "Array of game events", "event", "Game event, or invalid gameevent handle if index is invalid")
{
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_GameEvent.Set(gameevent_h()));

	int idx = gameseq_get_event_idx(name);

	if(idx < 0)
	{
		idx = atoi(name);

		//Lua-->FS2
		idx--;

		if(idx < 0 || idx >= Num_gs_event_text)
			return ade_set_error(L, "o", l_GameEvent.Set(gameevent_h()));
	}

	return ade_set_args(L, "o", l_GameEvent.Set(gameevent_h(idx)));
}

ADE_FUNC(__len, l_Base_Events, NULL, "Number of events", "number", "Number of events")
{
	return ade_set_args(L, "i", Num_gs_event_text);
}

//**********SUBLIBRARY: Base/States
ade_lib l_Base_States("GameStates", &l_Base, NULL, "Freespace 2 states");

ADE_INDEXER(l_Base_States, "number Index/string Name", "Array of game states", "state", "Game state, or invalid gamestate handle if index is invalid")
{
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_GameState.Set(gamestate_h()));

	int idx = gameseq_get_state_idx(name);

	if(idx < 0)
	{
		idx = atoi(name);

		//Lua-->FS2
		idx--;

		if(idx < 0 || idx >= Num_gs_state_text)
			return ade_set_error(L, "o", l_GameState.Set(gamestate_h()));
	}

	return ade_set_args(L, "o", l_GameState.Set(gamestate_h(idx)));
}

ADE_FUNC(__len, l_Base_States, NULL, "Number of states", "number", "Number of states")
{
	return ade_set_args(L, "i", Num_gs_state_text);
}

//**********LIBRARY: Campaign
/*
ade_lib l_Campaign("Campaign", NULL, "cn", "Campaign Library");

ADE_FUNC(getName, l_Campaign, NULL, "Campaign name", "Gets campaign name")
{
	return ade_set_args(L, "s", Campaign.name);
}

ADE_FUNC(getDescription, l_Campaign, NULL, "Campaign description or false if there is none", "Gets campaign description")
{
	if(Campaign.desc != NULL)
		return ade_set_args(L, "s", Campaign.desc);
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(getNumMissions, l_Campaign, NULL, "Number of missions", "Gets the number of missions in the campaign")
{
	return ade_set_args(L, "i", Campaign.num_missions);
}

ADE_FUNC(getNumMissionsCompleted, l_Campaign, NULL, "Number of missions completed", "Gets the number of missions in the campaign that have been completed")
{
	return ade_set_args(L, "i", Campaign.num_missions_completed);
}

ADE_FUNC(getNextMissionName, l_Campaign, NULL, "Mission name, or false if there is no next mission", "Gets the name of the next mission in the campaign")
{
	if(Campaign.next_mission < 0)
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "s", Campaign.missions[Campaign.next_mission].name);
}

ADE_FUNC(getNextMission, l_Campaign, NULL, "Cmission object, or false if there is no next mission", "Gets the next mission in the campaign")
{
	if(Campaign.next_mission < 0)
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "o", l_Cmission.Set(Campaign.next_mission));
}

ADE_FUNC(getPrevMissionName, l_Campaign, NULL, "Mission name, or false if there is no next mission", "Gets the name of the next mission in the campaign")
{
	if(Campaign.prev_mission < 0)
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "s", Campaign.missions[Campaign.prev_mission].name);
}

ADE_FUNC(getPrevMission, l_Campaign, NULL, "Cmission object, or false if there is no next mission", "Gets the previous mission in the campaign")
{
	if(Campaign.prev_mission < 0)
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "o", l_Cmission.Set(Campaign.prev_mission));
}

ADE_FUNC(getMissionByName, l_Campaign, "Mission name", "Cmission object, or false if mission does not exist", "Gets the specified mission from the campaign by its name")
{
	char *s;

	if(!ade_get_args(L, "s", &s))
		return ADE_RETURN_NIL;

	for(int idx = 0; idx < Campaign.num_missions; idx++)
	{
		if(!stricmp(Campaign.missions[idx].name, s))
			return ade_set_args(L, "o", l_Cmission.Set(idx));
	}

	return ADE_RETURN_FALSE;
}


ADE_FUNC(getMissionByIndex, l_Campaign, "Mission number (Zero-based index)", "Cmission object", "Gets the specified mission by its index in the campaign")
{
	int idx;

	if(!ade_get_args(L, "i", &idx))
		return ADE_RETURN_NIL;

	//Lua->FS2
	idx--;

	if(idx < 0 || idx > Campaign.num_missions)
		return ADE_RETURN_NIL;

	return ade_set_args(L, "o", l_Cmission.Set(idx));
}
*/

//**********LIBRARY: CFILE
//WMC - It's on my to-do list! (Well, if I had one anyway)
//WMC - Did it. I had to invent a to-do list first, though.
//Ironically, I never actually put this on it.
ade_lib l_CFile("CFile", NULL, "cf", "CFile FS2 filesystem access");

int l_cf_get_path_id(char* n_path)
{
	uint i;

	size_t path_len = strlen(n_path);
	char *buf = (char*) malloc((strlen(n_path)+1) * sizeof(char));
	strcpy(buf, n_path);

	//Remove trailing slashes
	i = path_len -1;
	while(i >= 0 && (buf[i] == '\\' || buf[i] == '/'))
		buf[i--] = '\0';

	//Remove leading slashes
	i = 0;
	while(i < path_len && (buf[i] == '\\' || buf[i] == '/'))
		buf[i++] = '\0';

	//Use official DIR_SEPARATOR_CHAR
	for(i = 0; i < path_len; i++)
	{
		if(buf[i] == '\\' || buf[i] == '/')
			buf[i] = DIR_SEPARATOR_CHAR;
	}
	for(i = 0; i < CF_MAX_PATH_TYPES; i++)
	{
		if(Pathtypes[i].path != NULL && !stricmp(buf, Pathtypes[i].path))
			return Pathtypes[i].index;
	}

	return CF_TYPE_INVALID;
}

ADE_FUNC(deleteFile, l_CFile, "string Filename, string Path", "Deletes given file. Path must be specified. Use a slash for the root directory.", "boolean", "True if deleted, false")
{
	char *n_filename = NULL;
	char *n_path = "";
	if(!ade_get_args(L, "ss", &n_filename, &n_path))
		return ade_set_error(L, "b", false);

	int path = CF_TYPE_INVALID;
	if(n_path != NULL && strlen(n_path))
		path = l_cf_get_path_id(n_path);

	if(path == CF_TYPE_INVALID)
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", cf_delete(n_filename, path) != 0);
}

ADE_FUNC(fileExists, l_CFile, "string Filename, [string Path = \"\", boolean CheckVPs = false]", "Checks if a file exists. Use a blank string for path for any directory, or a slash for the root directory.", "boolean", "True if file exists, false or nil otherwise")
{
	char *n_filename = NULL;
	char *n_path = "";
	bool check_vps = false;
	if(!ade_get_args(L, "s|sb", &n_filename, &n_path, &check_vps))
		return ADE_RETURN_NIL;

	int path = CF_TYPE_ANY;
	if(n_path != NULL && strlen(n_path))
		path = l_cf_get_path_id(n_path);

	if(path == CF_TYPE_INVALID)
		return ade_set_error(L, "b", false);

	const char *ext_list[] = {".ogg", ".txt", ".*"};
	int ext_list_num = sizeof(ext_list)/sizeof(char *);
	if(!check_vps)
		return ade_set_args(L, "b", cf_exists(n_filename, path) != 0);
	else
		return ade_set_args(L, "b", cf_exists_full(n_filename, path ) != 0);
}

ADE_FUNC(openFile, l_CFile, "string Filename, [string Mode=\"r\", string Path = \"\"]",
		 "Opens a file. 'Mode' uses standard C fopen arguments. Use a blank string for path for any directory, or a slash for the root directory."
		 "Be EXTREMELY CAREFUL when using this function, as you may PERMANENTLY delete any file by accident",
		 "file",
		 "File handle, or invalid file handle if the specified file couldn't be opened")
{
	char *n_filename = NULL;
	char *n_mode = "r";
	//enum_h *n_type = NULL;
	char *n_path = "";
	if(!ade_get_args(L, "s|ss", &n_filename, &n_mode, &n_path))
		return ade_set_error(L, "o", l_File.Set(NULL));

	int type = CFILE_NORMAL;
	/*
	if(n_type != NULL && n_type->index == LE_CFILE_TYPE_MEMORY_MAPPED)
	{
		type = CFILE_MEMORY_MAPPED;
		if(strcmp(n_mode,"rb"))
			LuaError(L, "Attempt to open file '%s' as memory mapped, but not in 'rb' mode. This is the only mode supported for memory-mapped files at this time.", n_filename);
	}*/

	int path = CF_TYPE_ANY;
	if(n_path != NULL && strlen(n_path))
		path = l_cf_get_path_id(n_path);

	if(path == CF_TYPE_INVALID)
		return ade_set_error(L, "o", l_File.Set(NULL));

	//if(strpbrk(n_mode, "wa+") != NULL && path != CF_TYPE_ROOT)
		//return ade_set_error(L, "o", l_File.Set(NULL));

	CFILE *cfp = cfopen(n_filename, n_mode, type, path);
	
	if(!cf_is_valid(cfp))
		return ade_set_error(L, "o", l_File.Set(NULL));

	return ade_set_args(L, "o", l_File.Set(cfp));
}

ADE_FUNC(openTempFile, l_CFile, NULL, "Opens a temp file that is automatically deleted when closed", "file", "File handle, or invalid file handle if tempfile couldn't be created")
{
	return ade_set_args(L, "o", l_File.Set(ctmpfile()));
}

ADE_FUNC(renameFile, l_CFile, "string CurrentFilename, string NewFilename, string Path", "Renames given file. Path must be specified. Use a slash for the root directory.", "boolean", "True if file was renamed, otherwise false")
{
	char *n_filename = NULL;
	char *n_new_filename = NULL;
	char *n_path = "";
	if(!ade_get_args(L, "ss|s", &n_filename, &n_new_filename, &n_path))
		return ade_set_error(L, "b", false);

	int path = CF_TYPE_INVALID;
	if(n_path != NULL && strlen(n_path))
		path = l_cf_get_path_id(n_path);

	if(path == CF_TYPE_INVALID)
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", cf_rename(n_filename, n_new_filename, path) != 0);
}

//**********LIBRARY: Controls library
ade_lib l_Mouse("Controls", NULL, "io", "Controls library");

extern int mouse_inited;

ADE_FUNC(getMouseX, l_Mouse, NULL, "Gets Mouse X pos", "number", "Mouse x position, or 0 if mouse is not initialized yet")
{
	if(!mouse_inited)
		return ade_set_error(L, "i", 0);

	int x;

	mouse_get_pos(&x, NULL);

	return ade_set_args(L, "i", x);
}

ADE_FUNC(getMouseY, l_Mouse, NULL, "Gets Mouse Y pos", "number", "Mouse y position, or 0 if mouse is not initialized yet")
{
	if(!mouse_inited)
		return ade_set_error(L, "i", 0);

	int y;

	mouse_get_pos(NULL, &y);

	return ade_set_args(L, "i", y);
}

ADE_FUNC(isMouseButtonDown, l_Mouse, "{MOUSE_*_BUTTON enumeration}, [..., ...]", "Returns whether the specified mouse buttons are up or down", "boolean", "Whether specified mouse buttons are down, or false if mouse is not initialized yet")
{
	if(!mouse_inited)
		return ade_set_error(L, "b", false);

	enum_h *e[3] = {NULL, NULL, NULL};
	ade_get_args(L, "o|oo", l_Enum.GetPtr(&e[0]), l_Enum.GetPtr(&e[1]), l_Enum.GetPtr(&e[2]));	//Like a snake!

	bool rtn = false;
	int check_flags = 0;

	for(int i = 0; i < 3; i++)
	{
		if(e[i] == NULL)
			break;

		if(e[i]->index == LE_MOUSE_LEFT_BUTTON)
			check_flags |= MOUSE_LEFT_BUTTON;
		if(e[i]->index == LE_MOUSE_MIDDLE_BUTTON)
			check_flags |= MOUSE_MIDDLE_BUTTON;
		if(e[i]->index == LE_MOUSE_RIGHT_BUTTON)
			check_flags |= MOUSE_RIGHT_BUTTON;
	}

	if(mouse_down(check_flags))
		rtn = true;

	return ade_set_args(L, "b", rtn);
}

ADE_FUNC(setCursorImage, l_Mouse, "Image filename, [LOCK or UNLOCK]", "Sets mouse cursor image, and allows you to lock/unlock the image. (A locked cursor may only be changed with the unlock parameter)", NULL, NULL)
{
	if(!mouse_inited || !Gr_inited)
		return ADE_RETURN_NIL;

	char *s = NULL;
	enum_h *u = NULL;
	if(!ade_get_args(L, "s|o", &s, l_Enum.GetPtr(&u)))
		return ADE_RETURN_NIL;

	int ul = 0;
	if(u != NULL)
	{
		if(u->index == LE_LOCK)
			ul = GR_CURSOR_LOCK;
		else if(u->index == LE_UNLOCK)
			ul = GR_CURSOR_UNLOCK;
	}

	gr_set_cursor_bitmap(bm_load(s), ul);

	return ADE_RETURN_NIL;
}

ADE_FUNC(setCursorHidden, l_Mouse, "True to hide mouse, false to show it", "Shows or hides mouse cursor", NULL, NULL)
{
	if(!mouse_inited)
		return ADE_RETURN_NIL;

	bool b = false;
	ade_get_args(L, "b", &b);

	if(b)
		Mouse_hidden = 1;
	else
		Mouse_hidden = 0;

	return ADE_RETURN_NIL;
}

//**********LIBRARY: Graphics
ade_lib l_Graphics("Graphics", NULL, "gr", "Graphics Library");

//****SUBLIBRARY: Graphics/Cameras
ade_lib l_Graphics_Cameras("Cameras", &l_Graphics, NULL, "Cameras");

ADE_INDEXER(l_Graphics_Cameras, "number Index/string Name", "Gets camera", "camera", "Ship handle, or invalid ship handle if index was invalid")
{
	char *s = NULL;
	if(!ade_get_args(L, "*s", &s))
		return ade_set_error(L, "o", l_Camera.Set(camid()));

	camid cid = cam_lookup(s);
	if(!cid.isValid())
	{
		int cn = atoi(s);
		if(cn > 0)
		{
			//Lua-->FS2
			cn--;
			cid = cam_get_camera(cn);
		}
	}

	return ade_set_args(L, "o", l_Camera.Set(cid));
}

ADE_FUNC(__len, l_Graphics_Cameras, NULL, "Gets number of cameras", "number", "Number of cameras")
{
	return ade_set_args(L, "i", (int)cam_get_num());
}

//****SUBLIBRARY: Graphics/Fonts
ade_lib l_Graphics_Fonts("Fonts", &l_Graphics, NULL, "Font library");

ADE_FUNC(__len, l_Graphics_Fonts, NULL, "Number of loaded fonts", "number", "Number of loaded fonts")
{
	return ade_set_args(L, "i", Num_fonts);
}

ADE_INDEXER(l_Graphics_Fonts, "number Index/string Filename", "Array of loaded fonts", "font", "Font handle, or invalid font handle if index is invalid")
{
	char *s = NULL;

	if(!ade_get_args(L, "*s", &s))
		return ade_set_error(L, "o", l_Font.Set(-1));

	int fn = gr_get_fontnum(s);
	if(fn < 0)
	{
		fn = atoi(s);
		if(fn < 1 || fn > Num_fonts)
			return ade_set_error(L, "o", l_Font.Set(-1));

		//Lua->FS2
		fn--;
	}

	return ade_set_args(L, "o", l_Font.Set(fn));
}
/*
ADE_VIRTVAR(CurrentColor, l_Graphics, "color", "Current color")
{
	
}*/

//WMC - This is a cubemap, unfortunately...
/*
ADE_VIRTVAR(CurrentEnvironmentMap, l_Graphics, "texture", "Current environment map")
{
	int newtx = -1;

	if(!ade_get_args(L, "*|o", l_Texture.Get(&newtx)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(ADE_SETTING_VAR && bm_is_valid(newtx)) {
		ENVMAP = newtx;
	}

	int tx = ENVMAP;
	if(!bm_is_valid(tx))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	return ade_set_args(L, "o", l_Texture.Set(tx));
}
*/

ADE_VIRTVAR(CurrentFont, l_Graphics, "font", "Current font", "font", NULL)
{
	int newfn = -1;

	if(!ade_get_args(L, "*|o", l_Font.Get(&newfn)))
		return ade_set_error(L, "o", l_Font.Set(-1));

	if(ADE_SETTING_VAR && newfn < Num_fonts) {
		gr_set_font(newfn);
	}

	int fn = FONT_INDEX(Current_font);

	if(fn < 0 || fn > Num_fonts)
		return ade_set_error(L, "o", l_Font.Set(-1));

	return ade_set_args(L, "o", l_Font.Set(fn));
}

ADE_VIRTVAR(CurrentOpacityType, l_Graphics, "enumeration", "Current alpha blending type; uses ALPHABLEND_* enumerations", "enumeration", NULL)
{
	enum_h *alphatype = NULL;

	if(!ade_get_args(L, "*|o", l_Enum.GetPtr(&alphatype)))
		return ADE_RETURN_NIL;

	if(ADE_SETTING_VAR)
	{
		if(alphatype != NULL && alphatype->index == LE_ALPHABLEND_FILTER)
			lua_Opacity_type = GR_ALPHABLEND_FILTER;
		else
			lua_Opacity_type = GR_ALPHABLEND_NONE;
	}

	int rtn;
	switch(lua_Opacity_type)
	{
		case GR_ALPHABLEND_FILTER:
			rtn = LE_ALPHABLEND_FILTER;
			break;
		default:
			rtn = LE_ALPHABLEND_NONE;
	}

	return ade_set_args(L, "o", l_Enum.Set(rtn));
}

/*
ADE_VIRTVAR(CurrentOpacity, l_Graphics, "number", "Opacity (transparency) of images; 0.0 - 1.0.", "number", NULL)
{
	float f;

	if(!ade_get_args(L, "*|f", &f))
		return ade_set_error(L, "f", 0.0f);

	if(f > 1.0f)
		f = 1.0f;
	if(f < 0.0f)
		f = 0.0f;

	lua_Opacity = f;

	return ade_set_args(L, "f", lua_Opacity);
}
*/

ADE_VIRTVAR(CurrentRenderTarget, l_Graphics, "texture", "Current rendering target", "texture", "Current rendering target, or invalid texture handle if screen is render target")
{
	int newtx = -1;

	if(ADE_SETTING_VAR && lua_isnil(L, 2))
	{
		bm_set_render_target(-1);
		return ade_set_args(L, "o", l_Texture.Set(gr_screen.rendering_to_texture));
	}
	else
	{

		if(!ade_get_args(L, "*|o", l_Texture.Get(&newtx)))
			return ade_set_error(L, "o", l_Texture.Set(-1));

		if(ADE_SETTING_VAR) {
			if(newtx > -1 && bm_is_valid(newtx))
				bm_set_render_target(newtx, 0);
			else
				bm_set_render_target(-1);
		}

		return ade_set_args(L, "o", l_Texture.Set(gr_screen.rendering_to_texture));
	}
}

ADE_FUNC(clearScreen, l_Graphics, "[number Red, number green, number blue]", "Clears the screen to black, or the color specified.", NULL, NULL)
{
	int r,g,b;
	r=g=b=0;
	ade_get_args(L, "|iii", &r, &g, &b);

	//WMC - Set to valid values
	if(r != 0 || g != 0 || b != 0)
	{
		CAP(r,0,255);
		CAP(g,0,255);
		CAP(b,0,255);
		gr_set_clear_color(r,g,b);
		gr_clear();
		gr_set_clear_color(0,0,0);

		return ADE_RETURN_NIL;
	}

	gr_clear();
	return ADE_RETURN_NIL;
}

ADE_FUNC(createCamera, l_Graphics,
		 "string Name, [wvector Position, orientation Orientation]",
		 "Creates a new camera using the specified position and orientation (World)",
		 "camera",
		 "Camera handle, or invalid camera handle if camera couldn't be created")
{
	char *s = NULL;
	vec3d *v = &vmd_zero_vector;
	matrix_h *mh = NULL;
	if(!ade_get_args(L, "s|oo", &s, l_Vector.GetPtr(&v), l_Matrix.GetPtr(&mh)))
		return ADE_RETURN_NIL;

	matrix *mtx = &vmd_identity_matrix;
	if(mh != NULL)
		mtx = mh->GetMatrix();
	camid cid = cam_create(s, v, mtx);

	//Set position
	return ade_set_args(L, "o", l_Camera.Set(cid));
}

ADE_FUNC(getScreenWidth, l_Graphics, NULL, "Gets screen width", "number", "Width in pixels, or 0 if graphics are not initialized yet")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", gr_screen.max_w);
}

ADE_FUNC(getScreenHeight, l_Graphics, NULL, "Gets screen height", "number", "Height in pixels, or 0 if graphics are not initialized yet")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", gr_screen.max_h);
}

ADE_FUNC(getVectorFromCoords, l_Graphics,
		 "[number X=center, number Y=center, number Depth]",
		 "Returns a normalized vector through screen coordinates x and y. "
		 "If depth is specified, vector is extended to Depth units into space",
		 "vector",
		 "Vector, or zero vector on failure")
{
	int x = gr_screen.max_w/2;
	int y = gr_screen.max_h/2;
	float depth = 0.0f;
	ade_get_args(L, "|iif", &x, &y, &depth);

	vec3d pos = vmd_zero_vector;

	bool in_frame = g3_in_frame() > 0;
	if(!in_frame)
		g3_start_frame(0);

	g3_point_to_vec(&pos, x, y);

	if(!in_frame)
		g3_end_frame();

	if(depth)
		vm_vec_scale(&pos, depth);

	vm_vec_add2(&pos, &View_position);

	return ade_set_args(L, "o", l_Vector.Set(pos));
}

ADE_FUNC(setTarget, l_Graphics, "[texture Texture]",
		"If texture is specified, sets current rendering surface to a texture."
		"Otherwise, sets rendering surface back to screen.",
		"boolean",
		"True if successful, false otherwise")
{
	if(!Gr_inited)
		return ade_set_error(L, "b", false);

	int idx = -1;
	ade_get_args(L, "|o", l_Texture.Get(&idx));

	int i = bm_set_render_target(idx, 0);

	return ade_set_args(L, "b", i ? true : false);
}

ADE_FUNC(setCamera, l_Graphics, "[camera handle Camera]", "Sets current camera, or resets camera if none specified", "boolean", "true if successful, false or nil otherwise")
{
	camid cid;
	if(!ade_get_args(L, "|o", l_Camera.Get(&cid)))
	{
		cam_reset_camera();
		return ADE_RETURN_NIL;
	}

	if(!cid.isValid())
		return ADE_RETURN_NIL;

	cam_set_camera(cid);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(setColor, l_Graphics, "number Red, number Green, number Blue, [number Alpha]", "Sets 2D drawing color; each color number should be from 0 (darkest) to 255 (brightest)", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int r,g,b,a=255;

	if(!ade_get_args(L, "iii|i", &r, &g, &b, &a))
		return ADE_RETURN_NIL;

	color ac;
	gr_init_alphacolor(&ac,r,g,b,a);
	gr_set_color_fast(&ac);

	return ADE_RETURN_NIL;
}
/*
ADE_FUNC(setOpacity, l_Graphics, "Opacity %, [Opacity Type]",
		 "Sets opacity for 2D image drawing functions to specified amount and type. Valid types are:"
		 "<br>NONE"
		 "<br>ALPHA_FILTER",
		 NULL,
		 NULL)
{
	float f;
	enum_h *alphatype = NULL;
	int idx=-1;

	if(!ade_get_args(L, "f|s", &f, l_Enum.GetPtr(&alphatype)))
		return ADE_RETURN_NIL;

	if(f > 100.0f)
		f = 100.0f;
	if(f < 0.0f)
		f = 0.0f;

	if(alphatype != NULL)
	{
		if(alphatype->index == LE_ALPHABLEND_FILTER)
			idx = GR_ALPHABLEND_FILTER;
		else
			idx = GR_ALPHABLEND_NONE;
	}

	lua_Opacity = f*0.01f;
	if(idx > -1)
		lua_Opacity_type = idx;

	return ADE_RETURN_NIL;
}


ADE_FUNC(setFont, l_Graphics, "string Filename", "Sets current font", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	char *s;
	int fn = -1;

	if(lua_isstring(L, 1))
	{
		if(!ade_get_args(L, "s", &s))
			return ADE_RETURN_NIL;

		fn = gr_get_fontnum(s);
	}
	else
	{
		if(!ade_get_args(L, "o", l_Font.Get(&fn)))
			return ADE_RETURN_NIL;
	}

	if(fn < 0 || fn > Num_fonts)
		return ADE_RETURN_FALSE;

	gr_set_font(fn);

	return ADE_RETURN_TRUE;
}
*/
ADE_FUNC(drawCircle, l_Graphics, "number Radius, number X, number Y", "Draws a circle", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x,y,ra;

	if(!ade_get_args(L, "iii", &ra,&x,&y))
		return ADE_RETURN_NIL;

	//WMC - Circle takes...diameter.
	gr_circle(x,y, ra*2, false);

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawCurve, l_Graphics, "number X, number Y, number Radius", "Draws a curve", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x,y,ra;

	if(!ade_get_args(L, "iii", &x,&y,&ra))
		return ADE_RETURN_NIL;

	//WMC - direction should be settable at a certain point via enumerations.
	//Not gonna deal with it now.
	gr_curve(x,y,ra,0);

	return ADE_RETURN_NIL;
}

/*
ADE_FUNC(drawLaser, l_Graphics, "texture Texture, [wvector HeadPosition, number HeadRadius = 1.0, number TailPosition, number TailRadius = 1.0]", "Draws a sphere", "boolean", "True if successful, false or nil otherwise")
{
	int tx = -1;
	float hrad = 1.0f;
	float trad = 1.0f;
	vec3d hpos = vmd_zero_vector;
	vec3d tpos = vmd_zero_vector;
	tpos.xyz.z = -1.0f;
	if(!ade_get_args(L, "o|ofof",l_Texture.Get(&tx), l_Vector.Get(&hpos), &hrad, l_Vector.Get(&tpos), &trad))
		return ADE_RETURN_FALSE;

	if(!bm_is_valid(tx))
		return ADE_RETURN_FALSE;

	gr_set_bitmap(tx, lua_Opacity_type, GR_BITBLT_MODE_NORMAL, lua_Opacity);

	bool in_frame = g3_in_frame();
	if(!in_frame)
		g3_start_frame(0);

	g3_draw_laser(&hpos, hrad, &tpos, trad);

	if(!in_frame)
		g3_end_frame();

	return ADE_RETURN_TRUE;
}*/

ADE_FUNC(drawGradientLine, l_Graphics, "number X1, number Y1, number X2, number Y2", "Draws a line from (x1,y1) to (x2,y2) with the CurrentColor that steadily fades out", NULL, NULL)
{
	if(!Gr_inited)
		return 0;

	int x1,y1,x2,y2;

	if(!ade_get_args(L, "iiii", &x1, &y1, &x2, &y2))
		return ADE_RETURN_NIL;

	gr_gradient(x1,y1,x2,y2,false);

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawLine, l_Graphics, "number X1, number Y1, number X2, number Y2", "Draws a line from (x1,y1) to (x2,y2) with CurrentColor", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x1,y1,x2,y2;

	if(!ade_get_args(L, "iiii", &x1, &y1, &x2, &y2))
		return ADE_RETURN_NIL;

	gr_line(x1,y1,x2,y2,false);

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawPixel, l_Graphics, "number X, number Y", "Sets pixel to CurrentColor", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x,y;

	if(!ade_get_args(L, "ii", &x, &y))
		return ADE_RETURN_NIL;

	gr_pixel(x,y,false);

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawPolygon, l_Graphics, "texture Texture, [vector Position={0,0,0}, orientation Orientation=null, number Width=1.0, number Height=1.0]", "Draws a polygon", NULL, NULL)
{
	int tdx = -1;
	vec3d pos = vmd_zero_vector;
	matrix_h *mh = NULL;
	float width = 1.0f;
	float height = 1.0f;
	if(!ade_get_args(L, "o|ooff", l_Texture.Get(&tdx), l_Vector.Get(&pos), l_Matrix.GetPtr(&mh), &width, &height))
		return ADE_RETURN_NIL;

	if(!bm_is_valid(tdx))
		return ADE_RETURN_FALSE;

	matrix *orip = &vmd_identity_matrix;
	if(mh != NULL)
		orip = mh->GetMatrix();

	//Do 3D stuff
	bool in_frame = g3_in_frame() > 0;
	if(!in_frame)
		g3_start_frame(0);

	gr_set_bitmap(tdx, lua_Opacity_type, GR_BITBLT_MODE_NORMAL, lua_Opacity);
	g3_draw_polygon(&pos, orip, width, height, TMAP_FLAG_TEXTURED | TMAP_HTL_3D_UNLIT);

	if(!in_frame)
		g3_end_frame();

	return ADE_RETURN_TRUE;
}

ADE_FUNC(drawRectangle, l_Graphics, "number X1, number Y1, number X2, number Y2, [boolean Filled=true]", "Draws a rectangle with CurrentColor", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x1,y1,x2,y2;
	bool f=true;

	if(!ade_get_args(L, "iiii|b", &x1, &y1, &x2, &y2, &f))
		return ADE_RETURN_NIL;

	if(f)
	{
		gr_rect(x1, y1, x2-x1, y2-y1, false);
	}
	else
	{
		gr_line(x1,y1,x2,y1,false);	//Top
		gr_line(x1,y2,x2,y2,false); //Bottom
		gr_line(x1,y1,x1,y2,false);	//Left
		gr_line(x2,y1,x2,y2,false);	//Right
	}

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawSphere, l_Graphics, "[number Radius = 1.0, vector Position]", "Draws a sphere with radius Radius at world vector Position", "boolean", "True if successful, false or nil otherwise")
{
	float rad = 1.0f;
	vec3d pos = vmd_zero_vector;
	ade_get_args(L, "|fo", &rad, l_Vector.Get(&pos));

	bool in_frame = g3_in_frame() > 0;
	if(!in_frame)
		g3_start_frame(0);

	vertex vtx;
	vm_vec2vert(&pos, &vtx);
	g3_rotate_vertex(&vtx, &pos);
	//g3_project_vertex(&vtx);
	g3_draw_sphere(&vtx, rad);

	if(!in_frame)
		g3_end_frame();

	return ADE_RETURN_TRUE;
}

#define MAX_TEXT_LINES		256
static char *BooleanValues[] = {"False", "True"};
static const int NextDrawStringPosInitial[] = {0, 0};
static int NextDrawStringPos[] = {NextDrawStringPosInitial[0], NextDrawStringPosInitial[1]};
ADE_FUNC(drawString, l_Graphics, "string Message, [number X1, number Y1, number X2, number Y2]",
		 "Draws a string. Use x1/y1 to control position, x2/y2 to limit textbox size."
		 "Text will automatically move onto new lines, if x2/y2 is specified."
		 "Additionally, calling drawString with only a string argument will automatically"
		 "draw that string below the previously drawn string (or 0,0 if no strings"
		 "have been drawn yet",
		 "number",
		 "Number of lines drawn, or 0 on failure")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	int x=NextDrawStringPos[0];
	int y = NextDrawStringPos[1];

	char *s = "(null)";
	int x2=-1,y2=-1;
	int num_lines = 0;

	if(lua_isboolean(L, 1))
	{
		bool b = false;
		if(!ade_get_args(L, "b|iiii", &b, &x, &y, &x2, &y2))
			return ade_set_error(L, "i", 0);

		if(b)
			s = BooleanValues[1];
		else
			s = BooleanValues[0];
	}
	else if(lua_isstring(L, 1))
	{
		if(!ade_get_args(L, "s|iiii", &s, &x, &y, &x2, &y2))
			return ade_set_error(L, "i", 0);
	}
	else
	{
		ade_get_args(L, "|*iiii", &x, &y, &x2, &y2);
	}

	NextDrawStringPos[0] = x;
	if(x2 < 0)
	{
		num_lines = 1;
		gr_string(x,y,s,false);

		int height = 0;
		gr_get_string_size(NULL, &height, s);
		NextDrawStringPos[1] = y+height;
	}
	else
	{
		int *linelengths = (int*)vm_malloc(sizeof(int)*MAX_TEXT_LINES);
		char **linestarts = (char**)vm_malloc(sizeof(char*)*MAX_TEXT_LINES);

		int num_lines = split_str(s, x2-x, linelengths, linestarts, MAX_TEXT_LINES);

		//Make sure we don't go over size
		int line_ht = gr_get_font_height();
		y2 = line_ht * (y2-y);
		if(y2 < num_lines)
			num_lines = y2;

		y2 = y;

		char rep;
		char *reptr;
		for(int i = 0; i < num_lines; i++)
		{
			//Increment line height
			y2 += line_ht;
			//WMC - rather than make a new string each line, set the right character to null
			reptr = &linestarts[i][linelengths[i]];
			rep = *reptr;
			*reptr = '\0';

			//Draw the string
			gr_string(x,y2,linestarts[i],false);

			//Set character back
			*reptr = rep;
		}

		vm_free(linelengths);
		vm_free(linestarts);
		NextDrawStringPos[1] = y2+gr_get_font_height();
	}
	return ade_set_error(L, "i", num_lines);
}

ADE_FUNC(getStringWidth, l_Graphics, "string String", "Gets string width", "number", "String width, or 0 on failure")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "i", 0);

	int w;

	gr_get_string_size(&w, NULL, s);
	
	return ade_set_args(L, "i", w);
}

ADE_FUNC(createTexture, l_Graphics, "[number Width=512, number Height=512, enumeration Type=TEXTURE_DYNAMIC]",
		 "Creates a texture for rendering to."
		 "Types are TEXTURE_STATIC - for infrequent rendering - and TEXTURE_DYNAMIC - for frequent rendering.",
		 "texture",
		 "New texture handle, or invalid texture handle if texture couldn't be created")
{
	int w=512;
	int h=512;
	enum_h *e = NULL;

	//GET ARGS
	ade_get_args(L, "|iio", &w, &h, l_Enum.GetPtr(&e));

	int t = BMP_FLAG_RENDER_TARGET_DYNAMIC;
	if(e != NULL)
	{
		if(e->index == LE_TEXTURE_STATIC)
			t = BMP_FLAG_RENDER_TARGET_STATIC;
		else if(e->index == LE_TEXTURE_DYNAMIC)
			t = BMP_FLAG_RENDER_TARGET_DYNAMIC;
	}

	int idx = bm_make_render_target(w, h, t);

	if(idx < 0)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	return ade_set_args(L, "o", l_Texture.Set(idx));
}

ADE_FUNC(loadTexture, l_Graphics, "string Filename, [boolean LoadIfAnimation, boolean NoDropFrames]",
		 "Gets a handle to a texture. If second argument is set to true, animations will also be loaded."
		 "If third argument is set to true, every other animation frame will not be loaded if system has less than 48 MB memory."
		 "<br><strong>IMPORTANT:</strong> Textures will not be unload themselves unless you explicitly tell them to do so."
		 "When you are done with a texture, call the Unload() function to free up memory.",
		 "texture",
		 "Texture handle, or invalid texture handle if texture couldn't be loaded")
{
	char *s;
	int idx;
	bool b=false;
	bool d=false;

	if(!ade_get_args(L, "s|bb", &s, &b, &d))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	idx = bm_load(s);
	if(idx < 0 && b) {
		idx = bm_load_animation(s, NULL, NULL, d ? 1 : 0);
	}

	if(idx < 0)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	return ade_set_args(L, "o", l_Texture.Set(idx));
}

ADE_FUNC(drawImage, l_Graphics, "string Filename/texture Texture, [number X1=0, Y1=0, number X2, number Y2, number UVX1 = 0.0, number UVY1 = 0.0, number UVX2=1.0, number UVY2=1.0]",
		 "Draws an image or texture. Any image extension passed will be ignored."
		 "The UV variables specify the UV value for each corner of the image. "
		 "In UV coordinates, (0,0) is the top left of the image; (1,1) is the lower right.",
		 "boolean",
		 "Whether image was drawn")
{
	if(!Gr_inited)
		return ade_set_error(L, "b", false);

	int idx;
	int x1 = 0;
	int y1 = 0;
	int x2=INT_MAX;
	int y2=INT_MAX;
	float uv_x1=0.0f;
	float uv_y1=0.0f;
	float uv_x2=1.0f;
	float uv_y2=1.0f;

	if(lua_isstring(L, 1))
	{
		char *s = NULL;
		if(!ade_get_args(L, "s|iiiiffff", &s,&x1,&y1,&x2,&y2,&uv_x1,&uv_y1,&uv_x2,&uv_y2))
			return ade_set_error(L, "b", false);

		idx = Script_system.LoadBm(s);

		if(idx < 0)
			return ADE_RETURN_FALSE;
	}
	else
	{
		if(!ade_get_args(L, "o|iiiiffff", l_Texture.Get(&idx),&x1,&y1,&x2,&y2,&uv_x1,&uv_y1,&uv_x2,&uv_y2))
			return ade_set_error(L, "b", false);
	}

	if(!bm_is_valid(idx))
		return ade_set_error(L, "b", false);

	int w, h;
	if(bm_get_info(idx, &w, &h) < 0)
		return ADE_RETURN_FALSE;

	if(x2!=INT_MAX)
		w = x2-x1;

	if(y2!=INT_MAX)
		h = y2-y1;

	gr_set_bitmap(idx, lua_Opacity_type, GR_BITBLT_MODE_NORMAL, lua_Opacity);
	bitmap_rect_list brl = bitmap_rect_list(x1, y1, w, h, uv_x1, uv_y1, uv_x2, uv_y2);
	gr_bitmap_list(&brl, 1, false);
	//gr_bitmap_ex(x1, y1, w, h, sx, sy, false);
	//gr_bitmap(x1, y1, false);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(drawMonochromeImage, l_Graphics, "string Filename/texture Texture, number X1, number Y1, [number X2, number Y2]", "Draws a monochrome image using the current color", "boolean", "Whether image was drawn")
{
	if(!Gr_inited)
		return ade_set_error(L, "b", false);

	int idx;
	int x,y;
	int x2=INT_MAX;
	int y2=INT_MAX;
	int sx=0;
	int sy=0;
	bool m = false;

	if(lua_isstring(L, 1))
	{
		char *s = NULL;
		if(!ade_get_args(L, "sii|ii", &s,&x,&y,&x2,&y2))
			return ade_set_error(L, "b", false);

		idx = Script_system.LoadBm(s);

		if(idx < 0)
			return ADE_RETURN_FALSE;
	}
	else
	{
		if(!ade_get_args(L, "oii|ii", l_Texture.Get(&idx),&x,&y,&x2,&y2))
			return ade_set_error(L, "b", false);
	}

	if(!bm_is_valid(idx))
		return ade_set_error(L, "b", false);

	int w, h;
	if(bm_get_info(idx, &w, &h) < 0)
		return ADE_RETURN_FALSE;

	if(sx < 0)
		sx = w + sx;

	if(sy < 0)
		sy = h + sy;
	
	if(x2!=INT_MAX)
		w = x2-x;

	if(y2!=INT_MAX)
		h = y2-y;

	gr_set_bitmap(idx, lua_Opacity_type, GR_BITBLT_MODE_NORMAL,lua_Opacity);
	gr_aabitmap_ex(x, y, w, h, sx, sy, false, m);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getImageWidth, l_Graphics, "string Filename", "Gets image width", "number", "Image width, or 0 if filename is invalid")
{
	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "i", 0);

	int w;
	
	int idx = bm_load(s);

	if(idx < 0)
		return ade_set_error(L, "i", 0);

	bm_get_info(idx, &w);
	return ade_set_args(L, "i", w);
}

ADE_FUNC(getImageHeight, l_Graphics, "Image name", "Gets image height", "number", "Image height, or 0 if filename is invalid")
{
	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "i", 0);

	int h;
	
	int idx = bm_load(s);

	if(idx < 0)
		return ade_set_error(L, "i", 0);

	bm_get_info(idx, NULL, &h);
	return ade_set_args(L, "i", h);
}

ADE_FUNC(flashScreen, l_Graphics, "number Red, number Green, number Blue", "Flashes the screen", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int r,g,b;

	if(!ade_get_args(L, "iii", &r, &g, &b))
		return ADE_RETURN_NIL;

	gr_flash(r,g,b);

	return ADE_RETURN_NIL;
}


//**********LIBRARY: Scripting Variables
ade_lib l_HookVar("HookVariables", NULL, "hv", "Hook variables repository");

//WMC: IMPORTANT
//Be very careful when modifying this library, as the Globals[] library does depend
//on the current number of items in the library. If you add _anything_, modify __len.
//Or run changes by me.

//*****LIBRARY: Scripting Variables
ade_lib l_HookVar_Globals("Globals", &l_HookVar);

ADE_INDEXER(l_HookVar_Globals, "number Index", "Array of current HookVariable names", "string", "Hookvariable name, or empty string if invalid index specified")
{
	int idx;
	if(!ade_get_args(L, "*i", &idx))
		return ade_set_error(L, "s", "");

	//Get lib
	lua_getglobal(L, l_HookVar.GetName());
	int lib_ldx = lua_gettop(L);
	if(!lua_isuserdata(L, lib_ldx))
	{
		lua_pop(L, 1);
		return ade_set_error(L, "s", "");
	}

	//Get metatable
	lua_getmetatable(L, lib_ldx);
	int mtb_ldx = lua_gettop(L);
	if(!lua_istable(L, mtb_ldx))
	{
		lua_pop(L, 2);
		return ade_set_error(L, "s", "");
	}

	//Get ade members table
	lua_pushstring(L, "__ademembers");
	lua_rawget(L, mtb_ldx);
	int amt_ldx = lua_gettop(L);
	if(!lua_istable(L, amt_ldx))
	{
		lua_pop(L, 3);
		return ade_set_error(L, "s", "");
	}

	//List 'em
	char *keyname = NULL;
	int count = 1;
	lua_pushnil(L);
	while(lua_next(L, amt_ldx))
	{
		//Now on stack: Key, value
		lua_pushvalue(L, -2);
		keyname = (char *)lua_tostring(L, -1);
		if(strcmp(keyname, "Globals"))
		{
			if(count == idx)
			{
				//lib, mtb, amt, key, value, string go bye-bye
				lua_pop(L, 5);
				return ade_set_args(L, "s", keyname);
			}
			count++;
		}
		lua_pop(L, 2);	//Value, string
	}

	lua_pop(L, 3);	//lib, mtb, amt

	return ade_set_error(L, "s", "");
}

ADE_FUNC(__len, l_HookVar_Globals, NULL, "Number of HookVariables", "number", "Number of HookVariables")
{
	//Get metatable
	lua_getglobal(L, l_HookVar.GetName());
	int lib_ldx = lua_gettop(L);
	if(!lua_isuserdata(L, lib_ldx))
	{
		lua_pop(L, 1);
		return ade_set_error(L, "i", 0);
	}

	lua_getmetatable(L, lib_ldx);
	int mtb_ldx = lua_gettop(L);
	if(!lua_istable(L, mtb_ldx))
	{
		lua_pop(L, 2);
		return ade_set_error(L, "i", 0);
	}

	//Get ade members table
	lua_pushstring(L, "__ademembers");
	lua_rawget(L, mtb_ldx);
	int amt_ldx = lua_gettop(L);
	if(!lua_istable(L, amt_ldx))
	{
		lua_pop(L, 3);
		return ade_set_error(L, "i", 0);
	}

	//int total_len = lua_objlen(L, amt_ldx);

	//WMC - Fine. Make me do the calculation manually.
	//See if I care.
	int total_len = 0;
	lua_pushnil(L);
	while(lua_next(L, amt_ldx))
	{
		total_len++;
		lua_pop(L, 1);	//value
	}
	int num_sub = Ade_table_entries[l_HookVar.GetIdx()].Num_subentries;

	lua_pop(L, 3);

	//WMC - Return length, minus the 'Globals' library
	return ade_set_args(L, "i", total_len - num_sub);
}

//**********LIBRARY: Mission
ade_lib l_Mission("Mission", NULL, "mn", "Mission library");

//****SUBLIBRARY: Mission/Asteroids
ade_lib l_Mission_Asteroids("Asteroids", &l_Mission, NULL, "Asteroids in the mission");

ADE_INDEXER(l_Mission_Asteroids, "number Index", "Gets asteroid", "asteroid", "Asteroid handle, or invalid handle if invalid index specified")
{
	int idx = -1;
	if( !ade_get_args(L, "*i", &idx) ) {
		return ade_set_error( L, "o", l_Asteroid.Set( object_h() ) );
	}
	if( idx > -1 && idx < asteroid_count() ) {
		return ade_set_args( L, "o", l_Asteroid.Set( object_h( &Objects[Asteroids[idx].objnum] ), Objects[Asteroids[idx].objnum].signature ) );
	}

	return ade_set_error(L, "o", l_Asteroid.Set( object_h() ) );
}

ADE_FUNC(__len, l_Mission_Asteroids, NULL,
		 "Number of asteroids in mission. Note that the value returned is only good until an asteroid is destroyed, and so cannot be relied on for more than one frame.",
		 "number",
		 "Number of asteroids in the mission, or 0 if asteroids are not enabled")
{
	if(Asteroids_enabled) {
		return ade_set_args(L, "i", asteroid_count());
	}
	return ade_set_args(L, "i", 0);
}

//****SUBLIBRARY: Mission/Debris
ade_lib l_Mission_Debris("Debris", &l_Mission, NULL, "debris in the mission");

ADE_INDEXER(l_Mission_Debris, "number Index", "Array of debris in the current mission", "debris", "Debris handle, or invalid debris handle if index wasn't valid")
{
	int idx = -1;
	if( !ade_get_args( L, "*i", &idx ) ) {
		return ade_set_error(L, "o", l_Debris.Set(object_h()));
	}
	if( idx > -1 && idx < Num_debris_pieces ) {
		return ade_set_args(L, "o", l_Debris.Set(object_h(&Objects[Debris[idx].objnum]), Objects[Debris[idx].objnum].signature));
	}

	return ade_set_error(L, "o", l_Debris.Set(object_h()));
}

ADE_FUNC(__len, l_Mission_Debris, NULL, 
		 "Number of debris pieces in the mission. "
		 "Note that the value returned is only good until a piece of debris is destroyed, and so cannot be relied on for more than one frame.",
		 "number",
		 "Current number of debris particles")
{
	return ade_set_args(L, "i", Num_debris_pieces);
}

//****SUBLIBRARY: Mission/EscortShips
ade_lib l_Mission_EscortShips("EscortShips", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_EscortShips, "number Index", "Gets escort ship at specified index on escort list", "ship", "Specified ship, or invalid ship handle if invalid index")
{
	int idx;
	if(!ade_get_args(L, "i", &idx))
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	if(idx < 1 || idx > hud_escort_num_ships_on_list())
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	//Lua->FS2
	idx--;

	idx = hud_escort_return_objnum(idx);
	
	if(idx < 0)
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[idx])));
}

ADE_FUNC(__len, l_Mission_EscortShips, NULL, "Current number of escort ships", "number", "Current number of escort ships")
{
	return ade_set_args(L, "i", hud_escort_num_ships_on_list());
}

//****SUBLIBRARY: Mission/Events
ade_lib l_Mission_Events("Events", &l_Mission, NULL, "Events");

ADE_INDEXER(l_Mission_Events, "number Index/string Name", "Indexes events list", "event", "Event handle, or invalid event handle if index was invalid")
{
	char *s;
	if(!ade_get_args(L, "*s", &s))
		return ade_set_error(L, "o", l_Event.Set(-1));

	int i;
	for(i = 0; i < Num_mission_events; i++)
	{
		if(!stricmp(Mission_events[i].name, s))
			return ade_set_args(L, "o", l_Event.Set(i));
	}

	//Now try as a number
	i = atoi(s);
	if(i < 1 || i > Num_mission_events)
		return ade_set_error(L, "o", l_Event.Set(-1));

	//Lua-->FS2
	i--;

	return ade_set_args(L, "o", l_Event.Set(i));
}

ADE_FUNC(__len, l_Mission_Events, NULL, "Number of events in mission", "number", "Number of events in mission")
{
	return ade_set_args(L, "i", Num_mission_events);
}

//****SUBLIBRARY: Mission/SEXPVariables
ade_lib l_Mission_SEXPVariables("SEXPVariables", &l_Mission, NULL, "SEXP Variables");

ADE_INDEXER(l_Mission_SEXPVariables, "number Index/string Name", "Array of SEXP variables. Note that you can set a sexp variable using the array, eg \'SEXPVariables[1] = \"newvalue\"\'", "sexpvariable", "Handle to SEXP variable, or invalid sexpvariable handle if index was invalid")
{
	char *name = NULL;
	char *newval = NULL;
	if(!ade_get_args(L, "*s|s", &name, &newval))
		return ade_set_error(L, "o", l_SEXPVariable.Set(sexpvar_h()));

	int idx = get_index_sexp_variable_name(name);
	if(idx < 0)
	{
		idx = atoi(name);

		//Lua-->FS2
		idx--;
	}

	if(idx < 0 || idx >= MAX_SEXP_VARIABLES)
	{
		if(ADE_SETTING_VAR && newval != NULL)
		{
			idx = sexp_add_variable(newval, name, lua_type(L, 2) == LUA_TNUMBER ? SEXP_VARIABLE_NUMBER : SEXP_VARIABLE_STRING);
		}

		//We have failed.
		if(idx < 0)
		{
			return ade_set_error(L, "o", l_SEXPVariable.Set(sexpvar_h()));
		}
	}
	else
	{
		if(ADE_SETTING_VAR && newval != NULL)
		{
			sexp_modify_variable(newval, idx);
		}
	}

	return ade_set_args(L, "o", l_SEXPVariable.Set(sexpvar_h(idx)));
}

ADE_FUNC(__len, l_Mission_SEXPVariables, NULL, "Current number of SEXP variables", "number", "Counts number of loaded SEXP Variables. May be slow.")
{
	return ade_set_args(L, "i", sexp_variable_count());
}

//****SUBLIBRARY: Mission/Ships
ade_lib l_Mission_Ships("Ships", &l_Mission, NULL, "Ships in the mission");

ADE_INDEXER(l_Mission_Ships, "number Index/string Name", "Gets ship", "ship", "Ship handle, or invalid ship handle if index was invalid")
{
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	int idx = ship_name_lookup(name);

	if(idx > -1)
	{
		return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[Ships[idx].objnum]), Objects[Ships[idx].objnum].signature));
	}
	else
	{
		idx = atoi(name);
		if(idx > 0)
		{
			int count=1;

			for(int i = 0; i < MAX_SHIPS; i++)
			{
				if (Ships[i].objnum < 0 || Objects[Ships[i].objnum].type != OBJ_SHIP)
					continue;

				if(count == idx) {
					return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[Ships[i].objnum]), Objects[Ships[i].objnum].signature));
				}

				count++;
			}
		}
	}

	return ade_set_error(L, "o", l_Ship.Set(object_h()));
}

ADE_FUNC(__len, l_Mission_Ships, NULL,
		 "Number of ships in the mission. "
		 "This function is somewhat slow, and should be set to a variable for use in looping situations. "
		 "Note that the value returned is only good until a ship is destroyed, and so cannot be relied on for more than one frame.",
		 "number",
		 "Number of ships in the mission, or 0 if ships haven't been initialized yet")
{
	if(ships_inited)
		return ade_set_args(L, "i", ship_get_num_ships());
	else
		return ade_set_args(L, "i", 0);
}

//****SUBLIBRARY: Mission/Waypoints
ade_lib l_Mission_Waypoints("Waypoints", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Waypoints, "number Index", "Array of waypoints in the current mission", "waypoint", "Waypoint handle, or invalid waypoint handle if index was invalid")
{
	int idx;
	if(!ade_get_args(L, "*i", &idx))
		return ade_set_error(L, "o", l_Waypoint.Set(object_h()));

	//Remember, Lua indices start at 0.
	int count=0;

	object *ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list))
	{
		if (ptr->type == OBJ_WAYPOINT)
			count++;

		if(count == idx) {
			return ade_set_args(L, "o", l_Waypoint.Set(object_h(ptr)));
		}

		ptr = GET_NEXT(ptr);
	}

	return ade_set_error(L, "o", l_Weapon.Set(object_h()));
}

ADE_FUNC(__len, l_Mission_Waypoints, NULL, "Gets number of waypoints in mission. Note that this is only accurate for one frame.", "number", "Number of waypoints in the mission")
{
	uint count=0;
	for(uint i = 0; i < MAX_OBJECTS; i++)
	{
		if (Objects[i].type == OBJ_WAYPOINT)
			count++;
	}

	return ade_set_args(L, "i", count);
}

//****SUBLIBRARY: Mission/WaypointLists
ade_lib l_Mission_WaypointLists("WaypointLists", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_WaypointLists, "number Index/string WaypointListName", "Array of waypoint lists", "waypointlist", "Gets waypointlist handle")
{
	waypointlist_h wpl;
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_WaypointList.Set(waypointlist_h()));

	wpl = waypointlist_h(name);

	if (!wpl.IsValid()) {
		int idx = atoi(name) - 1;
		if(idx > -1 && idx < Num_waypoint_lists) {
			wpl = waypointlist_h(&Waypoint_lists[idx]);
		}
	}

	if (wpl.IsValid()) {
		return ade_set_args(L, "o", l_WaypointList.Set(wpl));
	}

	return ade_set_error(L, "o", l_WaypointList.Set(waypointlist_h()));
}

ADE_FUNC(__len, l_Mission_WaypointLists, NULL, "Number of waypoint lists in mission. Note that this is only accurate for one frame.", "number", "Number of waypoint lists in the mission")
{
	return ade_set_args(L, "i", Num_waypoint_lists);
}

//****SUBLIBRARY: Mission/Weapons
ade_lib l_Mission_Weapons("Weapons", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Weapons, "number Index", "Gets handle to a weapon object in the mission.", "weapon", "Weapon handle, or invalid weapon handle if index is invalid")
{
	int idx;
	if(!ade_get_args(L, "*i", &idx))
		return ade_set_error(L, "o", l_Weapon.Set(object_h()));

	//Remember, Lua indices start at 0.
	int count=1;

	for(int i = 0; i < MAX_WEAPONS; i++)
	{
		if (Weapons[i].weapon_info_index < 0 || Weapons[i].objnum < 0 || Objects[Weapons[i].objnum].type != OBJ_WEAPON)
			continue;

		if(count == idx) {
			return ade_set_args(L, "o", l_Weapon.Set(object_h(&Objects[Weapons[i].objnum])));
		}

		count++;
	}

	return ade_set_error(L, "o", l_Weapon.Set(object_h()));
}
ADE_FUNC(__len, l_Mission_Weapons, NULL, "Number of weapon objects in mission. Note that this is only accurate for one frame.", "number", "Number of weapon objects in mission")
{
	return ade_set_args(L, "i", Num_weapons);
}

//****SUBLIBRARY: Mission/Wings
ade_lib l_Mission_Wings("Wings", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Wings, "number Index/string WingName", "Wings in the mission", "wing", "Wing handle, or invalid wing handle if index or name was invalid")
{
	char *name;
	if(!ade_get_args(L, "s", &name))
		return ade_set_error(L, "o", l_Wing.Set(-1));

	int idx = wing_name_lookup(name);
	
	if(idx < 0)
	{
		idx = atoi(name);
		if(idx < 1 || idx > Num_wings)
			return ade_set_error(L, "o", l_Wing.Set(-1));
	}

	return ade_set_args(L, "o", l_Wing.Set(idx));
}

ADE_FUNC(__len, l_Mission_Wings, NULL, "Number of wings in mission", "number", "Number of wings in mission")
{
	return ade_set_args(L, "i", Num_wings);
}

ADE_FUNC(createShip, l_Mission, "[string Name, shipclass Class=Shipclass[1], orientation Orientation=null, vector Position={0,0,0}]", "Creates a ship and returns a handle to it using the specified name, class, world orientation, and world position", "ship", "Ship handle, or invalid ship handle if ship couldn't be created")
{
	char *name = NULL;
	int sclass = -1;
	matrix_h *orient = NULL;
	vec3d pos = vmd_zero_vector;
	ade_get_args(L, "|sooo", &name, l_Shipclass.Get(&sclass), l_Matrix.GetPtr(&orient), l_Vector.Get(&pos));

	matrix *real_orient = &vmd_identity_matrix;
	if(orient != NULL)
	{
		real_orient = orient->GetMatrix();
	}
	
	int obj_idx = ship_create(real_orient, &pos, sclass, name);

	if(obj_idx > -1)
		return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[obj_idx]), Objects[obj_idx].signature));
	else
		return ade_set_error(L, "o", l_Ship.Set(object_h()));
}

ADE_FUNC(createWaypoint, l_Mission, "[vector Position, waypointlist List]",
		 "Creates a waypoint",
		 "waypoint",
		 "Waypoint handle, or invalid waypoint handle if waypoint couldn't be created")
{
	vec3d *v3 = NULL;
	waypointlist_h *wlh = NULL;
	if(!ade_get_args(L, "|oo", l_Vector.GetPtr(&v3), l_WaypointList.GetPtr(&wlh)))
		return ade_set_error(L, "o", l_Waypoint.Set(object_h()));

	int obj_idx = waypoint_create(v3 != NULL ? v3 : &vmd_zero_vector, wlh->IsValid() ? WAYPOINTLIST_INDEX(wlh->wlp) : -1);

	if(obj_idx > -1)
		return ade_set_args(L, "o", l_Waypoint.Set(object_h(&Objects[obj_idx])));
	else
		return ade_set_args(L, "o", l_Waypoint.Set(object_h()));
}

ADE_FUNC(createWeapon, l_Mission, "[weaponclass Class=WeaponClass[1], orientation Orientation=null, world vector Position={0,0,0}, object Parent = nil, number Group = -1",
		 "Creates a weapon and returns a handle to it. 'Group' is used for lighting grouping purposes;"
		 " for example, quad lasers would only need to act as one light source.",
		 "weapon",
		 "Weapon handle, or invalid weapon handle if weapon couldn't be created.")
{
	int wclass = -1;
	object_h *parent = NULL;
	int group = -1;
	matrix_h *orient = NULL;
	vec3d pos = vmd_zero_vector;
	ade_get_args(L, "|ooooi", l_Weaponclass.Get(&wclass), l_Matrix.GetPtr(&orient), l_Vector.Get(&pos), l_Object.GetPtr(&parent), &group);

	matrix *real_orient = &vmd_identity_matrix;
	if(orient != NULL)
	{
		real_orient = orient->GetMatrix();
	}

	int parent_idx = parent->IsValid() ? OBJ_INDEX(parent->objp) : -1;

	int obj_idx = weapon_create(&pos, real_orient, wclass, parent_idx, group);

	if(obj_idx > -1)
		return ade_set_args(L, "o", l_Weapon.Set(object_h(&Objects[obj_idx]), Objects[obj_idx].signature));
	else
		return ade_set_error(L, "o", l_Weapon.Set(object_h()));
}

ADE_FUNC(getMissionFilename, l_Mission, NULL, "Gets mission filename", "string", "Mission filename, or empty string if game is not in a mission")
{
	if(!(Game_mode & GM_IN_MISSION))
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Game_current_mission_filename);
}

ADE_FUNC(startMission, l_Mission, "[Filename or MISSION_* enumeration, Briefing = true]", "Starts the defined mission", "boolean", "True, or false if the function fails")
{
	bool b = true;
	char s[MAX_FILENAME_LEN];
	char *str = s;

	if(lua_isstring(L, 1))
	{
		if (!ade_get_args(L, "s|b", &str, &b))
			return ade_set_args(L, "b", false);

	} else {
		enum_h *e = NULL;

		if (!ade_get_args(L, "o|b", l_Enum.GetPtr(&e), &b))
			return ade_set_args(L, "b", false);

		if (e->index == LE_MISSION_REPEAT) {
			if (Num_recent_missions > 0)  {
				strncpy( s, Recent_missions[0], MAX_FILENAME_LEN );
			} else {
				return ade_set_args(L, "b", false);
			}
		} else {
			return ade_set_args(L, "b", false);
		}
	}

	// no filename... bail
	if (str == NULL)
		return ade_set_args(L, "b", false);

	// if mission name has extension... it needs to be removed...
	char *file_ext;

	file_ext = strrchr(str, '.');
	if (file_ext)
		*file_ext = 0;

	// game is in MP mode... or if the file does not exist... bail
	if ((Game_mode & GM_MULTIPLAYER) || (cf_exists_full(str, CF_TYPE_MISSIONS) != 0))
		return ade_set_args(L, "b", false);

	// mission is already running...
	if (Game_mode & GM_IN_MISSION) {
		// TO DO... All the things needed if this function is called in any state of the game while mission is running.
		//    most likely all require 'stricmp(str, Game_current_mission_filename)' to make sure missions arent mixed
		//    but after that it might be possible to imprement method for jumping directly into already running 
		//    missions.
		return ade_set_args(L, "b", false);
	// if mission is not running
	} else {
		// due safety checks of the game_start_mission() function allow only main menu for now.
		if (gameseq_get_state(gameseq_get_depth()) == GS_STATE_MAIN_MENU) {
			strncpy( Game_current_mission_filename, str, MAX_FILENAME_LEN );
			if (b == true) {
				// start mission - go via briefing screen
				gameseq_post_event(GS_EVENT_START_GAME);
			} else {
				// start mission - enter the game directly
				gameseq_post_event(GS_EVENT_START_GAME_QUICK);
			}
			return ade_set_args(L, "b", true);
		}
	}
	return ade_set_args(L, "b", false);
}

ADE_FUNC(getMissionTime, l_Mission, NULL, "Game time in seconds since the mission was started; is affected by time compression", "number", "Mission time (seconds), or 0 if game is not in a mission")
{
	if(!(Game_mode & GM_IN_MISSION))
		return ade_set_error(L, "f", 0.0f);

	/*
	if(ADE_SETTING_VAR)
	{
		fix newtime=Missiontime;
		ade_get_args(L, "|x", &newtime);
		Missiontime = newtime;
	}*/

	return ade_set_args(L, "x", Missiontime);
}

//WMC - These are in freespace.cpp
ADE_FUNC(loadMission, l_Mission, "Mission name", "Loads a mission", "boolean", "True if mission was loaded, otherwise false")
{
	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "b", false);

	//NOW do the loading stuff
	game_stop_time();
	get_mission_info(s, &The_mission, false);
	game_level_init();

	if(mission_load(s) == -1)
		return ADE_RETURN_FALSE;

	game_post_level_init();

	Game_mode |= GM_IN_MISSION;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(unloadMission, l_Mission, NULL, "Stops the current mission and unloads it", NULL, NULL)
{
	if(Game_mode & GM_IN_MISSION)
	{
		game_level_close();
		Game_mode &= ~GM_IN_MISSION;
		strcpy(Game_current_mission_filename, "");
	}

	return ADE_RETURN_NIL;
}

ADE_FUNC(simulateFrame, l_Mission, NULL, "Simulates mission frame", NULL, NULL)
{
	game_update_missiontime();
	game_simulation_frame();

	return ADE_RETURN_TRUE;
}

ADE_FUNC(renderFrame, l_Mission, NULL, "Renders mission frame, but does not move anything", NULL, NULL)
{
	camid cid = game_render_frame_setup();
	game_render_frame( cid );
	game_render_post_frame();

	return ADE_RETURN_TRUE;
}
/*
ADE_FUNC(getDirectiveByName, l_Mission, "Name, [Whether to include unborn directives]", "event handle",
		 "Gets directive by its name."
		 "Unborn directives are events that have not become available yet.")
{
	bool b = false;
	char *s;
	if(!ade_get_args(L, "s|b", &s, &b))
		return ADE_RETURN_NIL;

	mission_event *mep;
	for(int i = 0; i < Num_mission_events; i++)
	{
		mep = &Mission_events[i];
		if(mep->objective_text != NULL && !stricmp(Mission_events[i].name, s) && (b || mission_get_event_status(i) != EVENT_UNBORN))
			return ade_set_args(L, "o", l_Event.Set(i));
	}

	return ADE_RETURN_FALSE;
}

ADE_FUNC(getNumDirectives, l_Mission, "[Whether to include unborn directives]", "Number of directives in mission",
		 "Gets number of directives in mission. "
		 "Can be slightly slow, so only call it when you need to account for new/changed events. "
		 "Unborn directives are events that have not become available yet.")
{
	bool b = false;
	ade_get_args(L, "|b", &b);

	int count = 0;
	int i;
	mission_event *mep;
	for(i = 0; i < Num_mission_events; i++)
	{
		mep = &Mission_events[i];
		if(mep->objective_text != NULL && (b || mission_get_event_status(i) != EVENT_UNBORN)) {
			count++;
		}
	}

	return ade_set_args(L, "i", count);
}

ADE_FUNC(getDirectiveByIndex, l_Mission, "Index, [Whether to include unborn directives]", "Event handle",
		 "Gets directive. "
		 "Can be slightly slow, so use as little as possible."
		 "Unborn directives are events that have not become available yet.")
{
	int idx;
	bool b = false;
	if(!ade_get_args(L, "i|b", &idx, &b))
		return ADE_RETURN_NIL;

	if(idx < 1 || idx > Num_mission_events)
		return ADE_RETURN_FALSE;

	//Remember, Lua indices start at 0.
	int count=1;

	int i;
	mission_event *mep;
	for(i = 0; i < Num_mission_events; i++)
	{
		mep = &Mission_events[i];
		if(mep->objective_text != NULL && (b || mission_get_event_status(i) != EVENT_UNBORN))
		{
			if(count == idx)
				return ade_set_args(L, "o", l_Event.Set(i));

			count++;
		}
	}

	return ADE_RETURN_FALSE;
}*/

//**********LIBRARY: Keyboard
/*ade_lib l_Keyboard("kb", "Keyboard library");
//WMC - For some reason, this always returns true
ADE_FUNC(isKeyPressed, l_Keyboard, "Letter", "True if key is pressed, false if not", "Determines whether the given ASCII key is pressed. (If a string is given, only the first character is used)")
{
	char *s;
	if(!ade_get_args(L, "s", &s))
		return ADE_RETURN_NIL;

	char c = s[0];

	if(c == key_to_ascii(key_inkey()))
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}*/

//**********LIBRARY: Tables
ade_lib l_Tables("Tables", NULL, "tb", "Tables library");

//*****SUBLIBRARY: Tables/ShipClasses
ade_lib l_Tables_ShipClasses("ShipClasses", &l_Tables, NULL, NULL);

ADE_INDEXER(l_Tables_ShipClasses, "number Index/string Name", "Array of ship classes", "ship", "Ship handle, or invalid ship handle if index is invalid")
{
	if(!ships_inited)
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	int idx = ship_info_lookup(name);
	
	if(idx < 0) {
		idx = atoi(name);
		if(idx < 1 || idx > Num_ship_classes)
			return ade_set_error(L, "o", l_Shipclass.Set(-1));

		idx--;	//Lua->FS2
	}

	return ade_set_args(L, "o", l_Shipclass.Set(idx));
}

ADE_FUNC(__len, l_Tables_ShipClasses, NULL, "Number of ship classes", "number", "Number of ship classes, or 0 if ship classes haven't been loaded yet")
{
	if(!ships_inited)
		return ade_set_args(L, "i", 0);	//No ships loaded...should be 0

	return ade_set_args(L, "i", Num_ship_classes);
}

//*****SUBLIBRARY: Tables/WeaponClasses
ade_lib l_Tables_WeaponClasses("WeaponClasses", &l_Tables, NULL, NULL);

extern int Weapons_inited;

ADE_INDEXER(l_Tables_WeaponClasses, "number Index/string WeaponName", "Array of weapon classes", "weapon", "Weapon class handle, or invalid weaponclass handle if index is invalid")
{
	if(!Weapons_inited)
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	char *name;
	if(!ade_get_args(L, "*s", &name))
		return 0;

	int idx = weapon_info_lookup(name);
	
	if(idx < 0) {
		idx = atoi(name);

		if(idx < 1 || idx > Num_weapon_types) {
			return ade_set_args(L, "o", l_Weaponclass.Set(-1));
		}
	}

	return ade_set_args(L, "o", l_Weaponclass.Set(idx));
}

ADE_FUNC(__len, l_Tables_WeaponClasses, NULL, "Number of weapon classes", "number", "Number of weapon classes, or 0 if weapon classes haven't been loaded yet")
{
	if(!Weapons_inited)
		return ade_set_args(L, "i", 0);

	return ade_set_args(L, "i", Num_weapon_types);
}

//*************************Testing stuff*************************
//This section is for stuff that's considered experimental.
ade_lib l_Testing("Testing", NULL, "ts", "Experimental or testing stuff");

ADE_FUNC(avdTest, l_Testing, NULL, "Test the AVD Physics code", NULL, NULL)
{
	static bool initialized = false;
	static avd_movement avd;

	if(!initialized)
	{
		avd.setAVD(10.0f, 3.0f, 1.0f, 1.0f, 0.0f);
		initialized = true;
	}
	for(int i = 0; i < 3000; i++)
	{
		float Pc, Vc;
		avd.get((float)i/1000.0f, &Pc, &Vc);
		gr_set_color(0, 255, 0);
		gr_pixel(i/10, gr_screen.clip_bottom - (int)(Pc*10.0f), false);
		gr_set_color(255, 0, 0);
		gr_pixel(i/10, gr_screen.clip_bottom - (int)(Vc*10.0f), false);

		avd.get(&Pc, &Vc);
		gr_set_color(255, 255, 255);
		gr_pixel((timestamp()%3000)/10, gr_screen.clip_bottom - (int)(Pc*10.0f), false);
		gr_pixel((timestamp()%3000)/10, gr_screen.clip_bottom - (int)(Vc*10.0f), false);
	}

	return ADE_RETURN_NIL;
}

ADE_FUNC(createParticle, l_Testing, "vector Position, vector Velocity, number Lifetime, number Radius, enumeration Type, [number Tracer length=-1, boolean Reverse=false, texture Texture=Nil, object Attached Object=Nil]",
		 "Creates a particle. Use PARTICLE_* enumerations for type."
		 "Reverse reverse animation, if one is specified"
		 "Attached object specifies object that Position will be (and always be) relative to.",
		 NULL,
		 NULL)
{
	particle_info pi;
	pi.type = PARTICLE_DEBUG;
	pi.optional_data = 0;
	pi.tracer_length = 1.0f;
	pi.attached_objnum = -1;
	pi.attached_sig = -1;
	pi.reverse = 0;

	enum_h *type = NULL;
	bool rev=false;
	object_h *objh=NULL;
	if(!ade_get_args(L, "ooffo|fboo", l_Vector.Get(&pi.pos), l_Vector.Get(&pi.vel), &pi.lifetime, &pi.rad, l_Enum.GetPtr(&type), &pi.tracer_length, &rev, l_Texture.Get((int*)&pi.optional_data), l_Object.GetPtr(&objh)))
		return ADE_RETURN_NIL;

	if(type != NULL)
	{
		switch(type->index)
		{
			case LE_PARTICLE_DEBUG:
				pi.type = PARTICLE_DEBUG;
				break;
			/*case LE_PARTICLE_FIRE:
				pi.type = PARTICLE_FIRE;
				break;
			case LE_PARTICLE_SMOKE:
				pi.type = PARTICLE_SMOKE;
				break;
			case LE_PARTICLE_SMOKE2:
				pi.type = PARTICLE_SMOKE2;
				break;*/
			case LE_PARTICLE_BITMAP:
				pi.type = PARTICLE_BITMAP;
				break;
		}
	}

	if(rev)
		pi.reverse = 0;

	if(objh != NULL && objh->IsValid())
	{
		pi.attached_objnum = (short)OBJ_INDEX(objh->objp);
		pi.attached_sig = objh->objp->signature;
	}

	particle_create(&pi);

	return ADE_RETURN_NIL;
}

ADE_FUNC(getStack, l_Testing, NULL, "Generates an ADE stackdump", "string", "Current Lua stack")
{
	char buf[10240] = {'\0'};
	ade_stackdump(L, buf);
	return ade_set_args(L, "s", buf);
}

ADE_FUNC(isCurrentPlayerMulti, l_Testing, NULL, "Returns whether current player is a multiplayer pilot or not.", "boolean", "Whether current player is a multiplayer pilot or not")
{
	if(Player == NULL)
		return ade_set_error(L, "b", false);

	if(!(Player->flags & PLAYER_FLAGS_IS_MULTI))
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

// Om_tracker_flag should already be set in FreeSpace.cpp, needed to determine if PXO is enabled from the registry
extern int Om_tracker_flag; // needed for FS2OpenPXO config

ADE_FUNC(isPXOEnabled, l_Testing, NULL, "Returns whether PXO is currently enabled in the configuration.", "boolean", "Whether PXO is enabled or not")
{
	if(!(Om_tracker_flag))
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}


// *************************Helper functions*********************
//WMC - This should be used anywhere that an 'object' is set, so
//that scripters can get access to as much relevant data to that
//object as possible.
//It should also be updated as new types are added to Lua.
int ade_set_object_with_breed(lua_State *L, int obj_idx)
{
	if(obj_idx < 0 || obj_idx > MAX_OBJECTS)
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	object *objp = &Objects[obj_idx];

	switch(objp->type)
	{
		case OBJ_SHIP:
			return ade_set_args(L, "o", l_Ship.Set(object_h(objp)));
		case OBJ_ASTEROID:
			return ade_set_args(L, "o", l_Asteroid.Set(object_h(objp)));
		case OBJ_DEBRIS:
			return ade_set_args(L, "o", l_Debris.Set(object_h(objp)));
		case OBJ_WAYPOINT:
			return ade_set_args(L, "o", l_Waypoint.Set(object_h(objp)));
		case OBJ_WEAPON:
			return ade_set_args(L, "o", l_Weapon.Set(object_h(objp)));
		default:
			return ade_set_args(L, "o", l_Object.Set(object_h(objp)));
	}
}

//###########################################################
//########################<IMPORTANT>########################
//###########################################################
//If you are a coder who wants to add libraries, functions,
//or objects to Lua, then you want to be above this point.
//###########################################################
//########################</IMPORTANT>#######################
//###########################################################

// *************************Housekeeping*************************
//WMC - The miraculous lines of code that make Lua debugging worth something.
lua_Debug Ade_debug_info;

void ade_debug_line(lua_State *L, lua_Debug *ar)
{
	Assert(L != NULL);
	Assert(ar != NULL);
	lua_getinfo(L, "nSlu", ar);
	memcpy(&Ade_debug_info, ar, sizeof(lua_Debug));
}

void ade_debug_ret(lua_State *L, lua_Debug *ar)
{
	//WMC - So Lua isn't mean and uses ade_debug_line for returns
}

//WMC - because the behavior of the return keyword
//was changed, I now have to use this in hooks.
static int ade_return_hack(lua_State *L)
{
	int i = 0;
	int num = lua_gettop(L);
	for(i = 0; i < num; i++)
	{
		lua_pushvalue(L, i+1);
	}

	return num;
}
//Inits LUA
//Note that "libraries" must end with a {NULL, NULL}
//element
int script_state::CreateLuaState()
{
	mprintf(("LUA: Opening LUA state...\n"));
	lua_State *L = lua_open();

	if(L == NULL)
	{
		Warning(LOCATION, "Could not initialize Lua");
		return 0;
	}

	//*****INITIALIZE AUXILIARY LIBRARIES
	mprintf(("LUA: Initializing base Lua libraries...\n"));
	luaL_openlibs(L);

	//*****DISABLE DANGEROUS COMMANDS
	lua_pushstring(L, "os");
	lua_rawget(L, LUA_GLOBALSINDEX);
	int os_ldx = lua_gettop(L);
	if(lua_istable(L, os_ldx))
	{
		lua_pushstring(L, "execute");
		lua_pushnil(L);
		lua_rawset(L, os_ldx);
		lua_pushstring(L, "remove");
		lua_pushnil(L);
		lua_rawset(L, os_ldx);
		lua_pushstring(L, "rename");
		lua_pushnil(L);
		lua_rawset(L, os_ldx);
	}
	lua_pop(L, 1);	//os table

	//*****SET DEBUG HOOKS
#ifndef NDEBUG
	lua_sethook(L, ade_debug_line, LUA_MASKLINE, 0);
	lua_sethook(L, ade_debug_ret, LUA_MASKRET, 0);
#endif

	//*****INITIALIZE ADE
	uint i;
	mprintf(("LUA: Beginning ADE initialization\n"));
	for(i = 0; i < Ade_table_entries.size(); i++)
	{
		//WMC - Do only toplevel table entries, doi
		if(Ade_table_entries[i].ParentIdx == UINT_MAX)			//WMC - oh hey, we're done with the meaty point in < 10 lines.
			Ade_table_entries[i].SetTable(L, LUA_GLOBALSINDEX, LUA_GLOBALSINDEX);	//Oh the miracles of OOP.
	}

	//*****INITIALIZE RETURN HACK FUNCTION
	lua_pushstring(L, "ade_return_hack");
	lua_pushboolean(L, 0);
	lua_pushcclosure(L, ade_return_hack, 2);
	lua_setglobal(L, "ade_return_hack");

	//*****INITIALIZE ENUMERATION CONSTANTS
	mprintf(("ADE: Initializing enumeration constants...\n"));
	enum_h eh;
	for(i = 0; i < Num_enumerations; i++)
	{
		eh.index = Enumerations[i].def;
		eh.is_constant = true;

		ade_set_args(L, "o", l_Enum.Set(eh));
		lua_setglobal(L, Enumerations[i].name);
	}

	//*****ASSIGN LUA SESSION
	mprintf(("ADE: Assigning Lua session...\n"));
	SetLuaSession(L);

	return 1;
}

void script_state::EndLuaFrame()
{
	memcpy(NextDrawStringPos, NextDrawStringPosInitial, sizeof(NextDrawStringPos));
}

//*************************Lua functions*************************
//WMC - Spits out the current Lua stack to "stackdump"
//This includes variable values, but not names
void ade_stackdump(lua_State *L, char *stackdump)
{
	char buf[512];
	int stacksize = lua_gettop(L);

	//Lua temps
	double d;
	int b;
	char *s;
	for(int argnum = 1; argnum <= stacksize; argnum++)
	{
		int type = lua_type(L, argnum);
		snprintf(buf, sizeof(buf), "\r\n%d: ", argnum);
		strcat(stackdump, buf);
		switch(type)
		{
			case LUA_TNIL:
				strcat(stackdump, "NIL");
				break;
			case LUA_TNUMBER:
				d = lua_tonumber(L, argnum);
				snprintf(buf, sizeof(buf), "Number [%f]",d);
				strcat(stackdump, buf);
				break;
			case LUA_TBOOLEAN:
				b = lua_toboolean(L, argnum);
				snprintf(buf, sizeof(buf), "Boolean [%d]",b);
				strcat(stackdump, buf);
				break;
			case LUA_TSTRING:
				s = (char *)lua_tostring(L, argnum);
				snprintf(buf, sizeof(buf), "String [%s]",s);
				strcat(stackdump, buf);
				break;
			case LUA_TTABLE:
				{
					if(lua_getmetatable(L, argnum))
					{
						lua_pushstring(L, "__adeid");
						lua_rawget(L, -2);
						if(lua_isnumber(L, -1))
						{
							snprintf(buf, sizeof(buf), "Table [%s]", Ade_table_entries[(uint)lua_tonumber(L, -1)].Name);
							strcat(stackdump, buf);
						}
						else
							strcat(stackdump, "non-default Table");
						lua_pop(L, 2);	//metatable and nil/adeid
					}
					else
						strcat(stackdump, "Table w/ no metatable");

					//Maybe get first key?
					char *firstkey = NULL;
					lua_pushnil(L);
					if(lua_next(L, argnum))
					{
						firstkey = (char *)lua_tostring(L, -2);
						if(firstkey != NULL)
						{
							strcat(stackdump, ", First key: [");
							strcat(stackdump, firstkey);
							strcat(stackdump, "]");
						}
						lua_pop(L, 1);	//Key
					}
					lua_pop(L, 1);	//Nil
				}
				break;
			case LUA_TFUNCTION:
				strcat(stackdump, "Function");
				{
					char *upname = (char*)lua_getupvalue(L, argnum, ADE_FUNCNAME_UPVALUE_INDEX);
					if(upname != NULL)
					{
						strcat(stackdump, " ");
						strcat(stackdump, lua_tostring(L, -1));
						strcat(stackdump, "()");
						lua_pop(L, 1);
					}
				}
				break;
			case LUA_TUSERDATA:
				if(lua_getmetatable(L, argnum))
				{
					lua_pushstring(L, "__adeid");
					lua_rawget(L, -2);
					if(lua_isnumber(L, -1))
					{
						snprintf(buf, sizeof(buf), "Userdata [%s]", Ade_table_entries[(uint)lua_tonumber(L, -1)].Name);
					}
					else
						snprintf(buf, sizeof(buf), "non-default Userdata");

					lua_pop(L, 2);	//metatable and nil/adeid
				}
				else
					snprintf(buf, sizeof(buf), "Userdata w/ no metatable");
				strcat(stackdump, buf);
				break;
			case LUA_TTHREAD:
				//ls = lua_tothread(L, argnum);
				snprintf(buf, sizeof(buf), "Thread");
				strcat(stackdump, buf);
				break;
			case LUA_TLIGHTUSERDATA:
				//v = lua_touserdata(L, argnum);
				snprintf(buf, sizeof(buf), "Light userdata");
				strcat(stackdump, buf);
				break;
			default:
				snprintf(buf, sizeof(buf), "<UNKNOWN>: %s (%f) (%s)", lua_typename(L, type), lua_tonumber(L, argnum), lua_tostring(L, argnum));
				strcat(stackdump, buf);
				break;
		}
	}
}

//WMC - Sometimes this gets out of sync between Lua versions
static const char *Lua_type_names[] = {
	"nil",
	"boolean",
	"light userdata",
	"number",
	"string",
	"table",
	"function",
	"userdata",
	"thread",
};

static int Lua_type_names_num = sizeof(Lua_type_names)/sizeof(char*);

//WMC - Gets type of object
const char *ade_get_type_string(lua_State *L, int argnum)
{
	int type = lua_type(L, argnum);

	if(type < 0 || type >= Lua_type_names_num)
		return "Unknown";

	return Lua_type_names[type];
}

//WMC - hack to skip X number of arguments on the stack
//Lets me use ade_get_args for global hook return values
int Ade_get_args_skip = 0;
bool Ade_get_args_lfunction = false;

//ade_get_args(state, arguments, variables)
//----------------------------------------------
//based on "Programming in Lua"
//
//Parses arguments from string to variables given
//a '|' divides required and optional arguments.
//Returns 0 if a required argument is invalid,
//or there are too few arguments actually passed
//
//NOTE: This function essentially takes objects
//from the stack in series, so it can easily be used
//to get the return values from a chunk of Lua code
//after it has been executed. See RunByteCode()
int ade_get_args(lua_State *L, char *fmt, ...)
{
	//Check that we have all the arguments that we need
	//If we don't, return 0
	int needed_args = strlen(fmt);
	int total_args = lua_gettop(L) - Ade_get_args_skip;

	if(strchr(fmt, '|') != NULL) {
		needed_args = strchr(fmt, '|') - fmt;
	}

	char funcname[128] = "\0";
#ifndef NDEBUG
	lua_Debug ar;
	memset(&ar, 0, sizeof(ar));
	if(lua_getstack(L, 0, &ar))
	{
		lua_getinfo(L, "nl", &ar);
		strcpy(funcname, "");
		if(ar.name != NULL) {
			strcat(funcname, ar.name);
		}
		if(ar.currentline > -1) {
			char buf[33];
			snprintf(buf, sizeof(buf), "%d", ar.currentline);
			strcat(funcname, " (Line ");
			strcat(funcname, buf);
			strcat(funcname, ")");
		}
	}
#endif
	if(!strlen(funcname)) {
		//WMC - This was causing crashes with user-defined functions.
		//WMC - Try and get at function name from upvalue
		if(!Ade_get_args_lfunction && !lua_isnone(L, lua_upvalueindex(ADE_FUNCNAME_UPVALUE_INDEX)))
		{
				if(lua_type(L, lua_upvalueindex(ADE_FUNCNAME_UPVALUE_INDEX)) == LUA_TSTRING)
					strcpy(funcname, lua_tostring(L, lua_upvalueindex(ADE_FUNCNAME_UPVALUE_INDEX)));
		}

		//WMC - Totally unknown function
		if(!strlen(funcname)) {
			strcpy(funcname, "<UNKNOWN>");
		}
	}
	if(total_args < needed_args) {
		LuaError(L, "Not enough arguments for '%s' - need %d, had %d. If you are using objects or handles, make sure that you are using \":\" to access member functions, rather than \".\"", funcname, needed_args, total_args);
		return 0;
	}

	//Start throught
	va_list vl;
	int nargs;
	int counted_args = 0;

	//Are we parsing optional args yet?
	bool optional_args = false;

	va_start(vl, fmt);
	nargs = 1 + Ade_get_args_skip;
	total_args += Ade_get_args_skip;
	while(*fmt && nargs <= total_args)
	{
		//Skip functions; I assume these are being used to return args
		while(lua_type(L, nargs) == LUA_TFUNCTION && nargs <= total_args)
			nargs++;

		if(nargs > total_args)
			break;

		switch(*fmt++)
		{
			case 'b':
				if(lua_isboolean(L, nargs)) {
					*va_arg(vl, bool*) = lua_toboolean(L, nargs) > 0 ? true : false;
				} else {
					LuaError(L, "%s: Argument %d is an invalid type '%s'; boolean expected", funcname, nargs, ade_get_type_string(L, nargs));
					if(!optional_args) return 0;
				}
				break;
			case 'd':
				if(lua_isnumber(L, nargs)) {
					*va_arg(vl, double*) = (double)lua_tonumber(L, nargs);
				} else {
					LuaError(L, "%s: Argument %d is an invalid type '%s'; number expected", funcname, nargs, ade_get_type_string(L, nargs));
					if(!optional_args) return 0;
				}
				break;
			case 'f':
				if(lua_isnumber(L, nargs)) {
					*va_arg(vl, float*) = (float)lua_tonumber(L, nargs);
				} else {
					LuaError(L, "%s: Argument %d is an invalid type '%s'; number expected", funcname, nargs, ade_get_type_string(L, nargs));
					if(!optional_args) return 0;
				}
				break;
			case 'i':
				if(lua_isnumber(L, nargs)) {
					*va_arg(vl, int*) = (int)lua_tonumber(L, nargs);
				} else {
					LuaError(L, "%s: Argument %d is an invalid type '%s'; number expected", funcname, nargs, ade_get_type_string(L, nargs));
					if(!optional_args) return 0;
				}
				break;
			case 's':
				if(lua_isstring(L, nargs)) {
					*va_arg(vl, const char **) = lua_tostring(L, nargs);
				} else {
					LuaError(L, "%s: Argument %d is an invalid type '%s'; string expected", funcname, nargs, ade_get_type_string(L, nargs));
					if(!optional_args) return 0;
				}
				break;
			case 'x':
				if(lua_isnumber(L, nargs)) {
					*va_arg(vl, fix*) = fl2f((float)lua_tonumber(L, nargs));
				} else {
					LuaError(L, "%s: Argument %d is an invalid type '%s'; number expected", funcname, nargs, ade_get_type_string(L, nargs));
					if(!optional_args) return 0;
				}
				break;
			case 'o':
				{
					ade_odata od = va_arg(vl, ade_odata);
					if(lua_isuserdata(L, nargs))
					{
						//WMC - Get metatable
						lua_getmetatable(L, nargs);
						int mtb_ldx = lua_gettop(L);
						Assert(!lua_isnil(L, -1));
	
						//Get ID
						lua_pushstring(L, "__adeid");
						lua_rawget(L, mtb_ldx);
						//ade_id *paid = (ade_id*)lua_touserdata(L, -1);
	
						if(lua_tonumber(L, -1) != od.idx)
						{
							lua_pushstring(L, "__adederivid");
							lua_rawget(L, mtb_ldx);
							//ade_id *paideriv = (ade_id*)lua_touserdata(L, -1);
							if((uint)lua_tonumber(L, -1) != od.idx)
							{
								LuaError(L, "%s: Argument %d is the wrong type of userdata; '%s' given, but '%s' expected", funcname, nargs, Ade_table_entries[(uint)lua_tonumber(L, -2)].Name, Ade_table_entries[od.idx].GetName());
								if(!optional_args) return 0;
							}
							lua_pop(L, 1);
						}
						lua_pop(L, 2);
						if(od.size != ODATA_PTR_SIZE)
						{
							memcpy(od.buf, lua_touserdata(L, nargs), od.size);
							if(od.sig != NULL) {
								//WMC - char must be 1
								Assert(sizeof(char) == 1);
								//WMC - Yuck. Copy sig data.
								//Maybe in the future I'll do a packet userdata thing.
								(*od.sig) = *(ODATA_SIG_TYPE*)(*(char **)od.buf + od.size);
							}
						} else {
							(*(void**)od.buf) = lua_touserdata(L, nargs);
						}
					}
					else if(lua_isnil(L, nargs) && optional_args)
					{
						//WMC - Modder has chosen to ignore this argument
					}
					else
					{
						LuaError(L, "%s: Argument %d is an invalid type '%s'; type '%s' expected", funcname, nargs, ade_get_type_string(L, nargs), Ade_table_entries[od.idx].GetName());
						if(!optional_args) return 0;
					}
				}
				break;
			case '|':
				nargs--;	//cancel out the nargs++ at the end
				counted_args--;
				optional_args = true;
				break;
			case '*':
				//WMC - Ignore one spot
				break;
			default:
				Error(LOCATION, "%s: Bad character passed to ade_get_args; (%c)", funcname, *(fmt-1));
				break;
		}
		nargs++;
		counted_args++;
	}
	va_end(vl);
	return counted_args;
}

//ade_set_args(state, arguments, variables)
//----------------------------------------------
//based on "Programming in Lua"
//
//Takes variables given and pushes them onto the
//Lua stack. Use it to return variables from a
//Lua scripting function.
//
//NOTE: You can also use this to push arguments
//on to the stack in series. See script_state::SetHookVar
int ade_set_args(lua_State *L, char *fmt, ...)
{
	//Start throught
	va_list vl;
	int nargs;
	int setargs;	//args actually set

	va_start(vl, fmt);
	nargs = 0;
	setargs = 0;
	while(*fmt != '\0')
	{
		//lua_set_arg(L, *fmt++, va_arg(vl, void*));
		switch(*fmt++)
		{
			case '*':
				lua_pushnil(L);
				break;
			case 'b':	//WMC - Bool is actually int for GCC (Why...?)
				lua_pushboolean(L, va_arg(vl, int) ? 1 : 0);
				break;
			case 'd':
				lua_pushnumber(L, va_arg(vl, double));
				break;
			case 'f':
				lua_pushnumber(L, va_arg(vl, double));
				break;
			case 'i':
				lua_pushnumber(L, va_arg(vl, int));
				break;
			case 's':
				{
					//WMC - Isn't working with HookVar for some strange reason
					char *s = va_arg(vl, char*);
					lua_pushstring(L, s);
					break;
				}
			/*
			case 'u':
			case 'v':
				//WMC - Default upvalues, to reserve space for real ones
				// - Function name
				// - Whether function is in set mode (for virtvars), default is 0
				//WMC - WARNING!!!
				//WARNING!!! Making changes to any 'u' or 'v' functions must also
				//WARNING!!! be changed in ade_table_entry::SetTable()
				//Note that function pointers do not pass through va_args properly
				//under 64-bit.
				lua_pushstring(L, "<UNNAMED FUNCTION>");
				lua_pushboolean(L, 0);
				lua_pushcclosure(L, va_arg(vl, lua_CFunction), 2);
				break;
			*/
			case 'x':
				lua_pushnumber(L, f2fl(va_arg(vl, fix)));
				break;
			case 'o':
				{
					//WMC - char must be 1 byte, foo.
					Assert(sizeof(char)==1);
					//WMC - step by step
					//Copy over objectdata
					ade_odata od = (ade_odata) va_arg(vl, ade_odata);

					//Create new LUA object and get handle
					char *newod = (char*)lua_newuserdata(L, od.size + sizeof(ODATA_SIG_TYPE));
					//Create or get object metatable
					luaL_getmetatable(L, Ade_table_entries[od.idx].Name);
					//Set the metatable for the object
					lua_setmetatable(L, -2);

					//Copy the actual object data to the Lua object
					memcpy(newod, od.buf, od.size);

					//Also copy in the unique sig
					if(od.sig != NULL)
						memcpy(newod + od.size, od.sig, sizeof(ODATA_SIG_TYPE));
					else
					{
						ODATA_SIG_TYPE tempsig = ODATA_SIG_DEFAULT;
						memcpy(newod + od.size, &tempsig, sizeof(ODATA_SIG_TYPE));
					}
					break;
				}
			//WMC -  Don't forget to update lua_set_arg
			default:
				Error(LOCATION, "Bad character passed to ade_set_args; (%c)", *(fmt-1));
				setargs--;
		}
		nargs++;
		setargs++;
	}
	va_end(vl);
	return setargs;
}

/*
ade_id &ade_id::operator=(const ade_id &n_aid)
{
	Path.resize(n_aid.Path.size());
	memcpy(&Path[0], &n_aid.Path[0], sizeof(uint) * Path.size());

	return (*this);
}

bool ade_id::operator ==(ade_id &n_aid)
{
	if(n_aid.Path.size() != Path.size())
		return false;
	else
		return (memcmp(&n_aid.Path[0], &Path[0], sizeof(uint) * Path.size()) == 0);
}
*/
/*
ade_table_entry &ade_table_entry::operator =(const ade_table_entry &ate)
{
	Name = ate.Name;
	ShortName = ate.ShortName;

	ParentIdx = ate.ParentIdx;
	DerivatorIdx = ate.DerivatorIdx;
	//AdeID = ate.AdeID;
	//DerivatorID = ate.DerivatorID;

	Instanced = ate.Instanced;
	Type = ate.Type;
	memcpy(&Value, &ate.Value, sizeof(Value));
	Size = ate.Size;

	ReturnValues = ate.ReturnValues;
	Arguments = ate.Arguments;
	Description = ate.Description;

	Subentries.resize(ate.Subentries.size());
	for(uint i = 0; i < Subentries.size(); i++)
	{
		Subentries[i] = ate.Subentries[i];
	}

	return (*this);
}*/

//WMC - This function should _always_ return a valid pointer when used.
//a non-null pointer is assumed every time that it is called.
/*ade_table_entry *ade_id::GetATE()
{
	Assert(Path.size());
	Assert(Path[0] < Ade_table_entries.size());
	ade_table_entry *ate = &Ade_table_entries[Path[0]];
	for(uint i = 1; i < Path.size(); i++)
	{
		ate = &ate[i].Subentries[0];
		Assert(Path.size() == i || Path[i] < ate[i].Subentries.size());
	}
	return ate;
}*/


int ade_friendly_error(lua_State *L)
{
	LuaError(L);

	//WMC - According to documentation, this will always be the error
	//if error handler is called
	return LUA_ERRRUN;
}

//WMC - Used to get tostring from object, or just return string pointer
//if object is already a string.
char *ade_concat_helper(lua_State *L, int obj_ldx)
{
	char *rtn = NULL;

	lua_pushcfunction(L, ade_friendly_error);
	int err_ldx = lua_gettop(L);

	if(lua_isstring(L, obj_ldx))
	{
		rtn = (char*)lua_tostring(L, obj_ldx);
	}
	else if(lua_isuserdata(L, obj_ldx))
	{
		if(lua_getmetatable(L, obj_ldx))
		{
			int mtb_ldx = lua_gettop(L);

			lua_pushstring(L, "__tostring");
			lua_rawget(L, mtb_ldx);

			if(lua_iscfunction(L, -1))
			{
				lua_pushvalue(L, obj_ldx);
				if(!lua_pcall(L, 1, 1, err_ldx) && lua_type(L, -1) == LUA_TSTRING)
				{
					rtn = (char*)lua_tostring(L, -1);
				}
			}
		}
	}

	//WMC - Clear out all the extra crap.
	lua_pop(L, lua_gettop(L) - err_ldx + 1);

	return rtn;
}

//WMC - Used to automatically use an object's __tostring function to concatenate
//WMC - CAUSES CRASH
/*
int ade_concat_handler(lua_State *L)
{
	lua_pushcfunction(L, ade_friendly_error);
	int err_ldx = lua_gettop(L);

	char *s1=NULL;
	char *s2=NULL;

	s1 = ade_concat_helper(L, 1);
	s2 = ade_concat_helper(L, 2);

	if(s1 != NULL && s2 != NULL)
	{
		char *sf = (char*)vm_malloc((sizeof(s1) + sizeof(s2) + 1) * sizeof(char));
		strcpy(sf, s1);
		strcat(sf, s2);

		lua_pushstring(L, sf);
		//WMC - Causes crashes. WTF @ vm_ functions
		//vm_free(sf);
		//LuaError(L, "");
		return 1;
	}
	else if(s1 != NULL)
	{
		lua_pushstring(L, s1);
		return 1;
	}
	else if(s2 != NULL)
	{
		lua_pushstring(L, s2);
		return 1;
	}
	else
	{
		lua_pushstring(L, "???");
		return 1;
	}
}
*/

//1: Userspace variables (ie in object table)
//2: Handle-specific values
//3: Entries in metatable (ie defined by ADE)
//4: Virtual variables
//5: Use the indexer, if possible
//6: Set userspace variable
//7: Set handle-specific variables
//X: Mission failed.
//
//On the stack when this is called:
//Index 1 - Object (Can be anything with Lua 5.1; Number to a library)
//Index 2 - String (ie the key we're trying to access; Object.string, Object:string, Object['string'], etc)
//Index 3 - (Optional) Argument we are trying to set Object.String = Argument
static int ade_index_handler(lua_State *L)
{
	Assert(L != NULL);

	const int obj_ldx = 1;
	const int key_ldx = 2;
	const int arg_ldx = 3;
	int last_arg_ldx = lua_gettop(L);
	char *type_name = NULL;
	uint ade_id = UINT_MAX;
	int mtb_ldx = INT_MAX;

	//*****STEP 1: Check for user-defined objects
	if(lua_istable(L, obj_ldx) && !ADE_SETTING_VAR)
	{
		lua_pushvalue(L, key_ldx);
		lua_rawget(L, obj_ldx);
		if(!lua_isnil(L, -1))
			return 1;
		else
			lua_pop(L, 1);	//nil value
	}

	//*****STEP 1.5: Set-up metatable
	if(lua_getmetatable(L, obj_ldx))
	{
		mtb_ldx = lua_gettop(L);
		lua_pushcfunction(L, ade_friendly_error);
		int err_ldx = lua_gettop(L);
		int i;

		//*****WMC - go for the type name
		lua_pushstring(L, "__adeid");
		lua_rawget(L, mtb_ldx);
		if(lua_isnumber(L, -1))
		{
			ade_id = (uint) lua_tonumber(L, -1);
			if(ade_id < Ade_table_entries.size())
				type_name = Ade_table_entries[ade_id].Name;
		}
		lua_pop(L, 1);

		//*****STEP 2: Check for handle signature-specific values
		if(lua_isuserdata(L, obj_ldx) && ade_id != UINT_MAX && !ADE_SETTING_VAR)
		{
			//WMC - I assume char is one byte
			Assert(sizeof(char) == 1);

			//Get userdata sig
			char *ud = (char *)lua_touserdata(L, obj_ldx);
			ODATA_SIG_TYPE sig = *(ODATA_SIG_TYPE*)(ud + Ade_table_entries[ade_id].Value.Object.size);

			//Now use it to index the table with that #
			lua_pushnumber(L, sig);
			lua_rawget(L, mtb_ldx);
			if(lua_istable(L, -1))
			{
				int hvt_ldx = lua_gettop(L);
				lua_pushvalue(L, key_ldx);
				lua_rawget(L, hvt_ldx);
				if(!lua_isnil(L, -1))
					return 1;
				else
					lua_pop(L, 1);	//nil value
			}
			lua_pop(L, 1);	//sig table
		}

		//*****STEP 3: Check for __ademember objects (ie defaults)
		lua_pushstring(L, "__ademembers");
		lua_rawget(L, mtb_ldx);
		if(lua_istable(L, -1))
		{
			int amt_ldx = lua_gettop(L);
			lua_pushvalue(L, key_ldx);
			lua_rawget(L, amt_ldx);
			if(!lua_isnil(L, -1))
				return 1;
			else
				lua_pop(L, 1);	//nil value
		}
		lua_pop(L, 1);	//member table

		//*****STEP 4: Check for virtual variables
		lua_pushstring(L, "__virtvars");
		lua_rawget(L, mtb_ldx);
		if(lua_istable(L, -1))
		{
			//Index virtvar function
			int vvt_ldx = lua_gettop(L);
			lua_pushvalue(L, key_ldx);
			lua_rawget(L, vvt_ldx);
			if(lua_isfunction(L, -1))
			{
				//Set upvalue
				lua_pushvalue(L, lua_upvalueindex(ADE_SETTING_UPVALUE_INDEX));
				if(lua_setupvalue(L, -2, ADE_SETTING_UPVALUE_INDEX) == NULL)
					LuaError(L, "Unable to set upvalue for virtual variable");

				//Set arguments
				//WMC - Skip setting the key
				lua_pushvalue(L, obj_ldx);
				int numargs = 1;
				for(i = arg_ldx; i <= last_arg_ldx; i++)
				{
					lua_pushvalue(L, i);
					numargs++;
				}

				//Execute function
				lua_pcall(L, numargs, LUA_MULTRET, err_ldx);

				//WMC - Return as appropriate
				int rval = lua_gettop(L) - vvt_ldx;

				if(rval)
					return rval;
			}
			else
			{
				lua_pop(L, 1);	//non-function value
			}
		}
		lua_pop(L, 1);	//virtvar table

		//*****STEP 5: Use the indexer
		//NOTE: Requires metatable from step 1.5

		//Get indexer
		lua_pushstring(L, "__indexer");
		lua_rawget(L, mtb_ldx);
		if(lua_isfunction(L, -1))
		{
			//Function already on stack
			//Set upvalue
			lua_pushvalue(L, lua_upvalueindex(ADE_SETTING_UPVALUE_INDEX));
			if(lua_setupvalue(L, -2, ADE_SETTING_UPVALUE_INDEX) == NULL)
				LuaError(L, "Unable to set upvalue for indexer");

			//Set arguments
			for(i = 1; i <= last_arg_ldx; i++)
				lua_pushvalue(L, i);

			//Execute function
			lua_pcall(L, last_arg_ldx, LUA_MULTRET, err_ldx);

			int rval = lua_gettop(L) - err_ldx;

			if(rval)
				return rval;
		}
		lua_pop(L, 2);	//WMC - Don't need __indexer or error handler
	}

	//*****STEP 6: Set a new variable or die.
	if(ADE_SETTING_VAR && lua_istable(L, obj_ldx))
	{
		lua_pushvalue(L, key_ldx);
		lua_pushvalue(L, arg_ldx);
		lua_rawset(L, obj_ldx);

		lua_pushvalue(L, key_ldx);
		lua_rawget(L, obj_ldx);
		return 1;
	}
	//*****STEP 7: Set sig thingie
	else if(ADE_SETTING_VAR && ade_id != UINT_MAX && mtb_ldx != INT_MAX && lua_isuserdata(L, obj_ldx))
	{
		//WMC - I assume char is one byte
		Assert(sizeof(char) == 1);

		//Get userdata sig
		char *ud = (char *)lua_touserdata(L, obj_ldx);
		ODATA_SIG_TYPE sig = *(ODATA_SIG_TYPE*)(ud + Ade_table_entries[ade_id].Value.Object.size);

		//Now use it to index the table with that #
		lua_pushnumber(L, sig);
		lua_rawget(L, mtb_ldx);

		//Create table, if necessary
		if(!lua_istable(L, -1))
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushnumber(L, sig);
			lua_pushvalue(L, -2);
			lua_rawset(L, mtb_ldx);
		}

		//Index the table
		if(lua_istable(L, -1))
		{
			int hvt_ldx = lua_gettop(L);
			lua_pushvalue(L, key_ldx);
			lua_pushvalue(L, arg_ldx);
			lua_rawset(L, hvt_ldx);

			lua_pushvalue(L, key_ldx);
			lua_rawget(L, hvt_ldx);
			return 1;
		}
		lua_pop(L, 1);	//WMC - maybe-sig-table
	}
	lua_pop(L, 1);	//WMC - metatable

	if(type_name != NULL)
		LuaError(L, "Could not find index '%s' in type '%s'", lua_tostring(L, key_ldx), type_name);
	else
		LuaError(L, "Could not find index '%s'", lua_tostring(L, key_ldx));
	return 0;
}

//Think of n_mtb_ldx as the parent metatable
int ade_table_entry::SetTable(lua_State *L, int p_amt_ldx, int p_mtb_ldx)
{
	uint i;
	int cleanup_items = 0;
	int mtb_ldx = INT_MAX;
	int data_ldx = INT_MAX;
	int desttable_ldx = INT_MAX;
	int amt_ldx = INT_MAX;

	if(Instanced)
	{
		//Set any actual data
		int nset = 0;
		switch (Type)
		{
			//WMC - This hack by taylor is a necessary evil.
			//64-bit function pointers do not get passed properly
			//when using va_args for some reason.
			case 'u':
			case 'v':
				lua_pushstring(L, "<UNNAMED FUNCTION>");
				lua_pushboolean(L, 0);
				lua_pushcclosure(L, Value.Function, 2);
				nset++;
				break;

			default:
				char typestr[2] = {Type, '\0'};
				nset = ade_set_args(L, typestr, Value);
				break;
		}
				
		if (nset)
		{
			data_ldx = lua_gettop(L);
		}
		else
		{
			LuaError(L, "ade_table_entry::SetTable - Could not set data for '%s' (%d)", GetName(), ADE_INDEX(this));
		}

		if(data_ldx != INT_MAX)
		{
			//WMC - Cannot delete libs and stuff off here.
			if(p_amt_ldx != LUA_GLOBALSINDEX)
			{
				cleanup_items++;
			}

			//WMC - Handle virtual variables by getting their table
			if(Type == 'v')
			{
				//Get virtvars table
				lua_pushstring(L, "__virtvars");
				lua_rawget(L, p_mtb_ldx);
				if(lua_istable(L, -1))
				{
					cleanup_items++;

					//Virtual variables are stored in virtvar table,
					//rather than the parent table
					desttable_ldx = lua_gettop(L);
				}
				else
				{
					lua_pop(L, 1);
				}
			}
			else
			{
				//WMC - Member objects prefixed with __ are assumed to be metatable objects
				if(strnicmp("__", GetName(), 2) && lua_istable(L, p_amt_ldx))
					desttable_ldx = p_amt_ldx;
				else if(lua_istable(L, p_mtb_ldx))
					desttable_ldx = p_mtb_ldx;
			}

			if(desttable_ldx != INT_MAX)
			{
				//If we are setting a function...
				if(lua_isfunction(L, data_ldx))
				{
					//Set the FIRST upvalue to its name,
					//so we can always find out what it is for debugging
					lua_pushstring(L, GetName());
					if(lua_setupvalue(L, data_ldx, 1) == NULL) {
						LuaError(L, "ade_table_entry::SetTable - Could not set upvalue for '%s' (%d)", GetName(), ADE_INDEX(this));
					}
				}

				//Register name and shortname
				if(Name != NULL)
				{
					lua_pushstring(L, Name);
					lua_pushvalue(L, data_ldx);
					lua_rawset(L, desttable_ldx);
				}
				if(ShortName != NULL)
				{
					lua_pushstring(L, ShortName);
					lua_pushvalue(L, data_ldx);
					lua_rawset(L, desttable_ldx);
				}
			}
			else
			{
				LuaError(L, "ade_table_entry::SetTable - Could not instance '%s' (%d)", GetName(), ADE_INDEX(this));
			}
		}
	}

	//If subentries, create a metatable pointer and set it
	if(Num_subentries || (DerivatorIdx != UINT_MAX && Ade_table_entries[DerivatorIdx].Num_subentries))
	{
		//Create the new metatable
		if(!luaL_newmetatable(L, Name))
		{
			LuaError(L, "ade_table_entry::SetTable - Couldn't create metatable for table entry '%s' - does a Lua object already exist with this name?", Name);
			return 0;
		}
		mtb_ldx = lua_gettop(L);
		cleanup_items++;

		//Push a copy of the metatable and set it for this object
		//WMC - Make sure it's instanced, too. This helps keep crashes from happening...
		if(data_ldx != INT_MAX)
		{
			lua_pushvalue(L, mtb_ldx);
			lua_setmetatable(L, data_ldx);
		}

		//***Create index handler entry
		lua_pushstring(L, "__index");
		lua_pushstring(L, "ade_index_handler(get)");	//upvalue(1) = function name
		lua_pushboolean(L, 0);							//upvalue(2) = setting true/false
		lua_pushcclosure(L, ade_index_handler, 2);
		lua_rawset(L, mtb_ldx);

		//***Create newindex handler entry
		lua_pushstring(L, "__newindex");
		lua_pushstring(L, "ade_index_handler(set)");	//upvalue(1) = function name
		lua_pushboolean(L, 1);							//upvalue(2) = setting true/false
		lua_pushcclosure(L, ade_index_handler, 2);
		lua_rawset(L, mtb_ldx);

		//***Create concat handler entry
		//WMC - default concat handler causes crash.
		/*
		lua_pushstring(L, "__concat");
		lua_pushstring(L, "ade_concat_handler");
		lua_pushboolean(L, 0);
		lua_pushcclosure(L, ade_concat_handler, 2);
		lua_rawset(L, mtb_ldx);
		*/

		//***Create virtvar storage facility
		lua_pushstring(L, "__virtvars");
		lua_newtable(L);
		lua_rawset(L, mtb_ldx);

		//***Create ade members table
		lua_createtable(L, 0, Num_subentries);
		if(lua_istable(L, -1)) {
			//WMC - was lua_gettop(L) - 1 for soem
			amt_ldx = lua_gettop(L);
			cleanup_items++;

			//Set it
			lua_pushstring(L, "__ademembers");
			lua_pushvalue(L, amt_ldx);	//dup
			lua_rawset(L, mtb_ldx);
		}

		//***Create ID entries
		//void *ud;
		lua_pushstring(L, "__adeid");
		lua_pushnumber(L, ADE_INDEX(this));
		//ud = lua_newuserdata(L, AdeID.GetSizeInBytes());
		//AdeID.Copy(ud);
		lua_rawset(L, mtb_ldx);

		if(DerivatorIdx != UINT_MAX)
		{
			lua_pushstring(L, "__adederivid");
			//ud = lua_newuserdata(L, DerivatorID.GetSizeInBytes());
			//DerivatorID.Copy(ud);
			lua_pushnumber(L, DerivatorIdx);
			lua_rawset(L, mtb_ldx);
		}
	}

	if(amt_ldx != INT_MAX)
	{
		//Fill out ze metatable
		if(DerivatorIdx != UINT_MAX)
		{
			for(i = 0; i < Ade_table_entries[DerivatorIdx].Num_subentries; i++)
			{
				Ade_table_entries[Ade_table_entries[DerivatorIdx].Subentries[i]].SetTable(L, amt_ldx, mtb_ldx);
			}
		}
		for(i = 0; i < Num_subentries; i++)
		{
			Ade_table_entries[Subentries[i]].SetTable(L, amt_ldx, mtb_ldx);
		}
	}

	//Pop the metatable and data (cleanup)
	lua_pop(L, cleanup_items);

	return 1;
}

void ade_output_type_link(FILE *fp, char *typestr)
{
	for(int i = 0; i < Lua_type_names_num; i++)
	{
		if(!stricmp(Lua_type_names[i], typestr))
		{
			fputs(typestr, fp);
			return;
		}
	}

	fprintf(fp, "<a href=\"#%s\">%s</a>", typestr, typestr);
}

void ade_table_entry::OutputMeta(FILE *fp)
{
	if(Name == NULL && ShortName == NULL) {
		Warning(LOCATION, "Data entry with no name or shortname");
		return;
	}

	uint i;
	bool skip_this = false;

	//WMC - Hack
	if(ParentIdx != UINT_MAX)
	{
		for(i = 0; i < Num_subentries; i++)
		{
			if(!stricmp(Ade_table_entries[Subentries[i]].Name, "__indexer"))
			{
				Ade_table_entries[Subentries[i]].OutputMeta(fp);
				skip_this = true;
				break;
			}
		}
	}


	//***Begin entry
	if(!skip_this)
	{
		fputs("<dd><dl>", fp);

		switch(Type)
		{
			case 'o':
				{
					//***Name (ShortName)
					if(ParentIdx == UINT_MAX) {
						fprintf(fp, "<dt id=\"%s\">", GetName());
					}
					if(Name == NULL)
					{
						if(DerivatorIdx == UINT_MAX)
							fprintf(fp, "<h2>%s</h2>\n", ShortName);
						else
							fprintf(fp, "<h2>%s:<a href=\"#%s\">%s</a></h2>\n", Ade_table_entries[DerivatorIdx].GetName(), Ade_table_entries[DerivatorIdx].GetName(), Ade_table_entries[DerivatorIdx].GetName());
					}
					else
					{
						fprintf(fp, "<h2>%s", Name);

						if(ShortName != NULL)
							fprintf(fp, " (%s)", ShortName);
						if(DerivatorIdx != UINT_MAX)
							fprintf(fp, ":<a href=\"#%s\">%s</a>", Ade_table_entries[DerivatorIdx].GetName(), Ade_table_entries[DerivatorIdx].GetName());

						fputs("</h2>\n", fp);
					}
					fputs("</dt>\n", fp);

					//***Description
					if(Description != NULL) {
						fprintf(fp, "<dd>%s</dd>\n", this->Description);
					}

					//***Type: ReturnType
					if(ReturnType != NULL) {
						fprintf(fp, "<dd><b>Return Type: </b> %s<br>&nbsp;</dd>\n", ReturnType);
					}

					if(ReturnDescription != NULL) {
						fprintf(fp, "<dd><b>Return Description: </b> %s<br>&nbsp;</dd>\n", ReturnDescription);
					}
				}
				break;
			case 'u':
				{
					//***Name(ShortName)(Arguments)
					fputs("<dt>", fp);
					int ao = -1;

					fputs("<i>", fp);
					if(ReturnType != NULL)
						ade_output_type_link(fp, ReturnType);
					else
						fputs("nil", fp);
					fputs(" </i>", fp);
					if(Name == NULL) {
						fprintf(fp, "<b>%s", ShortName);
					}
					else
					{
						ao = ade_get_operator(Name);

						//WMC - Do we have an operator?
						if(ao < 0)
						{
							fprintf(fp, "<b>%s", Name);
							if(ShortName != NULL)
							{
								int ao2 = ade_get_operator(ShortName);
								if(ao2 < 0)
									fprintf(fp, "(%s)", ShortName);
								else
									fprintf(fp, "(%s)", ade_Operators[ao2].dest);
							}
						}
						else
						{
							//WMC - Hack
							if(ParentIdx != UINT_MAX && Ade_table_entries[ParentIdx].ParentIdx != UINT_MAX && Ade_table_entries[ParentIdx].Name != NULL && !stricmp(Name, "__indexer"))
							{
								fprintf(fp, "<b>%s%s", Ade_table_entries[ParentIdx].Name, ade_Operators[ao].dest);
							}
							else
								fprintf(fp, "<b>%s", ade_Operators[ao].dest);
						}
					}
					if(ao < 0)
					{
						if(Arguments != NULL) {
							fprintf(fp, "(</b><i>%s</i><b>)</b>\n", Arguments);
						} else {
							fprintf(fp, "()</b>\n");
						}
					}
					else
					{
						fputs("</b>", fp);
						if(Arguments != NULL) {
							fprintf(fp, " <i>%s</i>\n", Arguments);
						}
					}
					fputs("</dt>\n", fp);

					//***Description
					if(Description != NULL) {
						fprintf(fp, "<dd>%s</dd>\n", Description);
					}

					//***Result: ReturnDescription
					if(ReturnDescription != NULL) {
						fprintf(fp, "<dd><b>Returns:</b> %s<br>&nbsp;</dd>\n", ReturnDescription);
					} else {
						fputs("<dd><b>Returns:</b> Nothing<br>&nbsp;</dd>\n", fp);
					}
				}
				break;
			default:
				Warning(LOCATION, "Unknown type '%c' passed to ade_table_entry::OutputMeta", Type);
			case 'b':
			case 'd':
			case 'f':
			case 'i':
			case 's':
			case 'x':
			case 'v':
				{
					//***Type Name(ShortName)
					fputs("<dt>\n", fp);
					if(ReturnType != NULL)
					{
						fputs("<i>", fp);
						ade_output_type_link(fp, ReturnType);
						fputs(" </i>", fp);
					}

					if(Name == NULL) {
						fprintf(fp, "<b>%s</b>\n", ShortName);
					}
					else
					{
						fprintf(fp, "<b>%s", Name);
						if(ShortName != NULL)
							fputs(ShortName, fp);
						fputs("</b>\n", fp);
					}
					if(Arguments != NULL)
						fprintf(fp, " = <i>%s</i>", Arguments);
					fputs("</dt>\n", fp);

					//***Description
					if(Description != NULL)
						fprintf(fp, "<dd>%s</dd>\n", Description);

					//***Also settable with: Arguments
					if(Arguments != NULL)
						fprintf(fp, "<dd><b>Value:</b> %s</b></dd>\n", ReturnDescription);
				}
				break;
		}
	}

	fputs("<dd><dl>\n", fp);
	for(i = 0; i < Num_subentries; i++)
	{
		if(ParentIdx == UINT_MAX
			|| stricmp(Ade_table_entries[Subentries[i]].Name, "__indexer"))
			Ade_table_entries[Subentries[i]].OutputMeta(fp);
	}
	fputs("</dl></dd>\n", fp);

	if(!skip_this)
		fputs("<br></dl></dd>\n", fp);
}

void ade_output_toc(FILE *fp, ade_table_entry *ate)
{
	Assert(fp != NULL);
	Assert(ate != NULL);
		
	//WMC - sanity checking
	if(ate->Name == NULL && ate->ShortName == NULL) {
		Warning(LOCATION, "Found ade_table_entry with no name or shortname");
		return;
	}

	fputs("<dd>", fp);

	if(ate->Name == NULL)
	{
		fprintf(fp, "<a href=\"#%s\">%s", ate->ShortName, ate->ShortName);
	}
	else
	{
		fprintf(fp, "<a href=\"#%s\">%s", ate->Name, ate->Name);
		if(ate->ShortName)
			fprintf(fp, " (%s)", ate->ShortName);
	}
	fputs("</a>", fp);

	if(ate->Description)
		fprintf(fp, " - %s\n", ate->Description);

	fputs("</dd>\n", fp);
}

void script_state::OutputLuaMeta(FILE *fp)
{
	uint i;
	ade_table_entry *ate;
	fputs("<dl>\n", fp);

	//***TOC: Libraries
	fputs("<dt><b>Libraries</b></dt>\n", fp);
	for(i = 0; i < Ade_table_entries.size(); i++)
	{
		ate = &Ade_table_entries[i];
		if(ate->ParentIdx == UINT_MAX && ate->Type == 'o' && ate->Instanced) {
			ade_output_toc(fp, ate);
		}
	}

	//***TOC: Objects
	fputs("<dt><b>Types</b></dt>\n", fp);
	for(i = 0; i < Ade_table_entries.size(); i++)
	{
		ate = &Ade_table_entries[i];
		if(ate->ParentIdx == UINT_MAX && ate->Type == 'o' && !ate->Instanced) {
			ade_output_toc(fp, ate);
		}
	}

	//***TOC: Enumerations
	fputs("<dt><b><a href=\"#Enumerations\">Enumerations</a></b></dt>", fp);

	//***End TOC
	fputs("</dl><br/><br/>", fp);

	//***Everything
	fputs("<dl>\n", fp);
	for(i = 0; i < Ade_table_entries.size(); i++)
	{
		ate = &Ade_table_entries[i];
		if(ate->ParentIdx == UINT_MAX)
			ate->OutputMeta(fp);
	}
	//***Enumerations
	fprintf(fp, "<dt id=\"Enumerations\"><h2>Enumerations</h2></dt>");
	for(i = 0; i < Num_enumerations; i++)
	{
		//WMC - This is in case we ever want to add descriptions to enums.
		//fprintf(fp, "<dd><dl><dt><b>%s</b></dt><dd>%s</dd></dl></dd>", Enumerations[i].name, Enumerations[i].desc);

		//WMC - Otherwise, just use this.
		fprintf(fp, "<dd><b>%s</b></dd>", Enumerations[i].name);
	}
	fputs("</dl>\n", fp);

	//***End LUA
	fputs("</dl>\n", fp);
}
