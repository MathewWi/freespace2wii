/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

/*
 * $Logfile: /Freespace2/code/Model/MODEL.H $
 * $Revision: 2.80.2.16 $
 * $Date: 2007-02-21 01:43:32 $
 * $Author: Goober5000 $
 *
 * header file for information about polygon models
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.80.2.15  2007/02/12 00:19:48  taylor
 * IBX version 2 support (includes Bobboau's earlier D3D fixes for it)
 *
 * Revision 2.80.2.14  2006/12/28 22:47:15  Goober5000
 * fix spelling... *twitch*
 *
 * Revision 2.80.2.13  2006/11/28 05:48:21  Goober5000
 * the grammar nazi strikes again!
 * (and sneaks an extra #define in there while he's at it!)
 *
 * Revision 2.80.2.12  2006/11/01 18:35:57  taylor
 * make glow_point array for thrusters and glow_point_banks dynamic (a proper fix for old Mantis bug #43)
 *
 * Revision 2.80.2.11  2006/10/28 04:00:09  taylor
 * ok, so apparently that did actually do something ;)
 *
 * Revision 2.80.2.10  2006/10/27 21:39:27  taylor
 * updated/fixed modelanim code
 *
 * Revision 2.80.2.9  2006/10/27 06:42:29  taylor
 * rename set_warp_globals() to model_set_warp_globals()
 * remove two old/unused MR flags (MR_ALWAYS_REDRAW, used for caching that doesn't work; MR_SHOW_DAMAGE, didn't do anything)
 * add MR_FULL_DETAIL to render an object regardless of render/detail box setting
 * change "model_current_LOD" to a global "Interp_detail_level" and the static "Interp_detail_level" to "Interp_detail_level_locked", a bit more descriptive
 * minor bits of cleanup
 * change a couple of vm_vec_scale_add2() calls to just vm_vec_add2() calls in ship.cpp, since that was the final result anyway
 *
 * Revision 2.80.2.8  2006/10/24 13:24:11  taylor
 * various bits of cleanup (slight reformatting to help readability, remove old/dead code bits, etc.)
 * deal with a index_buffer memory leak that Valgrind has always complained about
 * make HTL model buffers dynamic (get rid of MAX_BUFFERS_PER_SUBMODEL)
 * get rid of MAX_BUFFERS
 * make D3D vertex buffers dynamic, like OGL has already done
 *
 * Revision 2.80.2.7  2006/07/17 01:09:45  taylor
 * make glow point banks dynamic
 *
 * Revision 2.80.2.6  2006/07/17 00:10:05  Goober5000
 * stage 2 of animation fix (add base frame time to each ship)
 * --Goober5000
 *
 * Revision 2.80.2.5  2006/07/13 22:11:36  taylor
 * fix for animated texture map issues (*part one*), this should be faster than before too, and fix inf-loop/div-by-0 issues
 *
 * Revision 2.80.2.4  2006/07/06 21:53:58  taylor
 * rest of the map/glow changes
 *  - put glowmap activity back on a per-ship basis (via a SF2_* flag) rather than per-model
 *  - same for glowpoints, back on a per-ship basis
 *  - put specmaps and bumpmap back on a LOD0 and LOD1 affect (got changed to LOD0 only recently)
 *  - fix glowmaps for shockwaves again
 *  - add support for animated specmaps (mainly for TBP and Starfox mods)
 * some minor code cleanup and compiler warning fixes
 *
 * Revision 2.80.2.3  2006/07/06 04:06:01  Goober5000
 * 1) complete (almost) changeover to reorganized texture mapping system
 * 2) finally fix texture animation; textures now animate at the correct speed
 * --Goober5000
 *
 * Revision 2.80.2.2  2006/07/05 23:36:56  Goober5000
 * cvs comment tweaks
 *
 * Revision 2.80.2.1  2006/07/04 07:42:09  Goober5000
 * --in preparation for fixing an annoying animated texture bug, reorganize the various texture structs and glow point structs and clarify several parts of the texture code :P
 * --this breaks animated glow maps, and animated regular maps still aren't fixed, but these will be remedied shortly
 * --Goober5000
 *
 * Revision 2.80  2006/05/31 03:05:42  Goober5000
 * some cosmetic changes in preparation for bay arrival/departure code
 * --Goober5000
 *
 * Revision 2.79  2006/05/27 16:57:12  taylor
 * comment out the model cache stuff, it's old and not actually used anyway
 * minor cleanup of some modelinterp.cpp code, to make it more readable
 * fix tertiary thruster batcher which had the radius value swapped
 * comment out a couple places where we were doing a D3D or OGL check (it's always true so why bother)
 * add more support for NO_DIRECT3D so that it can be used on Windows to build without D3D
 *
 * Revision 2.78  2006/04/20 06:32:15  Goober5000
 * proper capitalization according to Volition
 *
 * Revision 2.77  2006/04/18 00:56:28  bobboau
 * bugfix for the animation system
 *
 * Revision 2.76  2006/03/24 07:38:35  wmcoolmon
 * New subobject animation stuff and Lua functions.
 *
 * Revision 2.75  2006/03/18 07:12:07  Goober5000
 * add ship-subsys-targetable and ship-subsys-untargetable
 * --Goober5000
 *
 * Revision 2.74  2006/02/27 00:51:03  wmcoolmon
 * Bump MAX_TFP to 10
 *
 * Revision 2.73  2006/02/25 21:47:07  Goober5000
 * spelling
 *
 * Revision 2.72  2006/01/18 16:14:04  taylor
 * allow gr_render_buffer() to take TMAP flags
 * let gr_render_buffer() render untextured polys (OGL only until some D3D people fix it on their side)
 * add MR_SHOW_OUTLINE_HTL flag so we easily render using HTL mode for wireframe views
 * make Interp_verts/Interp_norms/etc. dynamic and get rid of the extra htl_* versions
 *
 * Revision 2.71  2006/01/11 21:23:43  wmcoolmon
 * Made turret handling code more descriptive
 *
 * Revision 2.70  2006/01/04 08:19:22  taylor
 * fixes for regular texture replacement
 *
 * Revision 2.69  2005/12/29 08:08:36  wmcoolmon
 * Codebase commit, most notably including objecttypes.tbl
 *
 * Revision 2.68  2005/12/15 04:31:05  phreak
 * [V] can't spell 'turret'
 *
 * Revision 2.67  2005/12/04 19:07:48  wmcoolmon
 * Final commit of codebase
 *
 * Revision 2.66  2005/10/24 07:13:04  Goober5000
 * merge Bobboau's thruster code back in; hopefully this covers everything
 * --Goober5000
 *
 * Revision 2.65  2005/09/24 07:45:31  Goober5000
 * cleaned up some more thruster stuff; honestly, the thruster code is such a
 * mess that it should probably be reverted to the retail version
 * --Goober5000
 *
 * Revision 2.64  2005/07/13 03:26:00  Goober5000
 * remove PreProcDefine #includes in FS2
 * --Goober5000
 *
 * Revision 2.63  2005/07/12 22:14:40  Goober5000
 * removed DECALS_ENABLED
 * --Goober5000
 *
 * Revision 2.62  2005/06/21 00:20:24  taylor
 * in the model _render functions change "light_ignore_id" to "objnum" since that's what it really is
 *   and this makes it so much easier to realize that
 * properly deal with the fact that objnum can be -1 in  model_really_render()
 * add NULL check to neb2_get_fog_values() so that it can just send back defaults if objp is NULL
 * small compiler warning fix for neb code
 *
 * Revision 2.61  2005/04/25 00:26:53  wmcoolmon
 * Dynamically allocated engine washes; subsystem sounds
 *
 * Revision 2.60  2005/04/21 15:49:20  taylor
 * update of bmpman and model bitmap management, well tested but things may get a bit bumpy
 *  - use VM_* macros for bmpman since it didn't seem to register the memory correctly (temporary)
 *  - a little "stupid" fix for dds bitmap reading
 *  - fix it so that memory is released properly on bitmap read errors
 *  - some cleanup to model texture loading
 *  - allow model textures to get released rather than just unloaded, saves bitmap slots
 *  - bump MAX_BITMAPS to 4750, should be able to decrease after public testing of new code
 *
 * Revision 2.59  2005/04/05 05:53:20  taylor
 * s/vector/vec3d/g, better support for different compilers (Jens Granseuer)
 *
 * Revision 2.58  2005/03/25 06:57:36  wmcoolmon
 * Big, massive, codebase commit. I have not removed the old ai files as the ones I uploaded aren't up-to-date (But should work with the rest of the codebase)
 *
 * Revision 2.57  2005/03/03 06:05:30  wmcoolmon
 * Merge of WMC's codebase. "Features and bugs, making Goober say "Grr!", as release would be stalled now for two months for sure"
 *
 * Revision 2.56  2005/03/01 06:55:41  bobboau
 * oh, hey look I've commited something :D
 * animation system, weapon models detail box alt-tab bug, probly other stuff
 *
 * Revision 2.55  2005/02/23 05:05:38  taylor
 * compiler warning fixes (for MSVC++ 6)
 * have the warp effect only load as many LODs as will get used
 * head off strange bug in release when corrupt soundtrack number gets used
 *    (will still Assert in debug)
 * don't ever try and save a campaign savefile in multi or standalone modes
 * first try at 32bit->16bit color conversion for TGA code (for TGA only ship textures)
 *
 * Revision 2.54  2005/02/15 00:06:27  taylor
 * clean up some model related globals
 * code to disable individual thruster glows
 * fix issue where 1 extra OGL light pass didn't render
 *
 * Revision 2.53  2005/02/08 23:49:59  taylor
 * update/add .cvsignore files for project file changes
 * silence warning about deprecated strings.h stuff for MSVC 2005
 * final model_unload() stuff for WMCoolmon, put in missionweaponchoice.cpp
 * remove really old project files
 *
 * Revision 2.52  2005/02/04 23:29:32  taylor
 * merge with Linux/OSX tree - p0204-3
 *
 * Revision 2.51  2005/01/28 11:57:36  Goober5000
 * fixed Bobboau's spelling of 'relative'
 * --Goober5000
 *
 * Revision 2.50  2005/01/28 09:56:44  taylor
 * add model_page_out_textures() for use in techroom
 * make model_page_in_textures() load more textures
 *
 * Revision 2.49  2005/01/27 11:26:23  Goober5000
 * dock points on rotating submodels is *almost* working
 * --Goober5000
 *
 * Revision 2.48  2005/01/11 21:38:50  Goober5000
 * multiple ship docking :)
 * don't tell anyone yet... check the SCP internal
 * --Goober500
 *
 * Revision 2.47  2005/01/01 19:45:32  taylor
 * add MR_NO_FOGGING flag to easily render models without fog (warp model, targetbox models)
 *
 * Revision 2.46  2004/12/05 22:01:11  bobboau
 * sevral feature additions that WCS wanted,
 * and the foundations of a submodel animation system,
 * the calls to the animation triggering code (exept the procesing code,
 * wich shouldn't do anything without the triggering code)
 * have been commented out.
 *
 * Revision 2.45  2004/10/09 17:45:08  taylor
 * da simple IBX code
 *
 * Revision 2.44  2004/08/11 05:06:28  Kazan
 * added preprocdefines.h to prevent what happened with fred -- make sure to make all fred2 headers include this file as the _first_ include -- i have already modified fs2 files to do this
 *
 * Revision 2.43  2004/07/11 03:22:50  bobboau
 * added the working decal code
 *
 * Revision 2.42  2004/07/01 01:12:32  bobboau
 * implemented index buffered background bitmaps,
 * OGL people you realy should get this implemented
 *
 * Revision 2.41  2004/06/28 02:13:08  bobboau
 * high level index buffer suport and d3d implementation,
 * OGL people need to get this working on your end as it's broke now
 *
 * Revision 2.40  2004/05/10 10:51:52  Goober5000
 * made primary and secondary banks quite a bit more friendly... added error-checking
 * and reorganized a bunch of code
 * --Goober5000
 *
 * Revision 2.39  2004/03/20 21:17:13  bobboau
 * fixed -spec comand line option,
 * probly some other stuf
 *
 * Revision 2.38  2004/03/05 09:02:07  Goober5000
 * Uber pass at reducing #includes
 * --Goober5000
 *
 * Revision 2.37  2004/01/21 17:37:48  phreak
 * bumped MAX_POLYGON_MODELS to 300 if INF_BUILD is defined.
 *
 * Revision 2.36  2003/12/17 23:24:24  phreak
 * added a MAX_BUFFERS_PER_SUBMODEL define so it can be easily changed if we ever want to change the 16 texture limit
 *
 * Revision 2.35  2003/11/17 04:25:57  bobboau
 * made the poly list dynamicly alocated,
 * started work on fixing the node model not rendering,
 * but most of that got commented out so I wouldn't have to deal with it
 * while mucking about with the polylist
 *
 * Revision 2.34  2003/11/11 02:15:45  Goober5000
 * ubercommit - basically spelling and language fixes with some additional
 * warnings disabled
 * --Goober5000
 *
 * Revision 2.33  2003/10/10 03:59:41  matt
 * Added -nohtl command line param to disable HT&L, nothing is IFDEFd
 * out now. -Sticks
 *
 * Revision 2.32  2003/09/26 14:37:15  bobboau
 * commiting Hardware T&L code, everything is ifdefed out with the compile flag HTL
 * still needs a lot of work, ubt the frame rates were getting with it are incredable
 * the biggest problem it still has is a bad lightmanegment system, and the zbuffer
 * doesn't work well with things still getting rendered useing the sofware pipeline, like thrusters,
 * and weapons, I think these should be modifyed to be sent through hardware,
 * it would be slightly faster and it would likely fix the problem
 *
 * also the thruster glow/particle stuff I did is now in.
 *
 * Revision 2.31  2003/09/13 06:02:06  Goober5000
 * clean rollback of all of argv's stuff
 * --Goober5000
 *
 * Revision 2.28  2003/08/28 20:42:18  Goober5000
 * implemented rotating barrels for firing primary weapons
 * --Goober5000
 *
 * Revision 2.27  2003/08/22 07:35:09  bobboau
 * specular code should be bugless now,
 * cell shadeing has been added activated via the comand line '-cell',
 * 3D shockwave models, and a transparency method I'm calling edge and center alpha that could be usefull for other things, ask for details
 *
 * Revision 2.26  2003/08/12 03:18:33  bobboau
 * Specular 'shine' mapping;
 * useing a phong lighting model I have made specular highlights
 * that are mapped to the model,
 * rendering them is still slow, but they look real purdy
 *
 * also 4 new (untested) comand lines, the XX is a floating point value
 * -spec_exp XX
 * the n value, you can set this from 0 to 200 (actualy more than that, but this is the recomended range), it will make the highlights bigger or smaller, defalt is 16.0 so start playing around there
 * -spec_point XX
 * -spec_static XX
 * -spec_tube XX
 * these are factors for the three diferent types of lights that FS uses, defalt is 1.0,
 * static is the local stars,
 * point is weapons/explosions/warp in/outs,
 * tube is beam weapons,
 * for thouse of you who think any of these lights are too bright you can configure them you're self for personal satisfaction
 *
 * Revision 2.25  2003/07/15 02:36:54  phreak
 * changed the model_setup_cloak() function to allow an alpha value
 *
 * Revision 2.24  2003/07/04 02:28:37  phreak
 * changes for cloaking implemented
 *
 * Revision 2.23  2003/04/29 01:03:23  Goober5000
 * implemented the custom hitpoints mod
 * --Goober5000
 *
 * Revision 2.22  2003/03/18 01:44:31  Goober5000
 * fixed some misspellings
 * --Goober5000
 *
 * Revision 2.21  2003/03/02 05:54:23  penguin
 * ANSI C++ - namespace clash on ai_rotation; renamed type to ai_rotation_t
 *  - penguin
 *
 * Revision 2.20  2003/01/20 05:40:49  bobboau
 * added several sExps for turning glow points and glow maps on and off
 *
 * Revision 2.19  2003/01/19 06:44:39  Goober5000
 * got rid of nameplate stuff (superceded by texture replacement)
 * --Goober5000
 *
 * Revision 2.18  2003/01/19 01:07:41  bobboau
 * redid the way glow maps are handled; you now must set a global variable before you render a poly that uses a glow map, then set it to -1 when you're done with it
 * fixed a few other misc bugs too
 *
 * Revision 2.17  2003/01/17 04:57:17  Goober5000
 * Allowed selection of either $Texture Replace, which keeps track of individual
 * replacement textures for a ship, or $Duplicate Model Texture Replace, which
 * duplicates a model and reskins it.  Use $Duplicate Model Texture Replace
 * if you want to substitute an animated texture or a transparent texture.
 * --Goober5000
 *
 * Revision 2.16  2003/01/17 01:48:49  Goober5000
 * added capability to the $Texture replace code to substitute the textures
 * without needing and extra model, however, this way you can't substitute
 * transparent or animated textures
 * --Goober5000
 *
 * Revision 2.15  2003/01/16 06:49:11  Goober5000
 * yay! got texture replacement to work!!!
 * --Goober5000
 *
 * Revision 2.14  2003/01/15 08:57:23  Goober5000
 * assigning duplicate models to ships now works; committing so I have a base
 * to fall back to as I work on texture replacement
 * --Goober5000
 *
 * Revision 2.13  2003/01/15 05:18:13  Goober5000
 * moved texture loading around a bit; added Texture_replace in preparation
 * for some cool stuff
 * --Goober5000
 *
 * Revision 2.12  2003/01/06 19:33:22  Goober5000
 * cleaned up some stuff with model_set_thrust and a commented Assert that
 * shouldn't have been
 * --Goober5000
 *
 * Revision 2.11  2002/12/10 05:43:34  Goober5000
 * Full-fledged ballistic primary support added!  Try it and see! :)
 *
 * Revision 2.10  2002/12/07 01:37:42  bobboau
 * initial decals code, if you are worried a bug is being caused by the decals code it's only references are in,
 * collideshipweapon.cpp line 262, beam.cpp line 2771, and modelinterp.cpp line 2949.
 * it needs a better renderer, but is in prety good shape for now,
 * I also (think) I squashed a bug in the warpmodel code
 *
 * Revision 2.9  2002/12/04 09:44:34  DTP
 * lowered MAX_POLYGON_MODELS FROM 198 to 128 for safety reasons since we bumped back MAX_SHIP_CLASSES to 130
 *
 * Revision 2.8  2002/12/02 23:55:31  Goober5000
 * fixed misspelling
 *
 * Revision 2.7  2002/12/02 23:14:17  Goober5000
 * fixed misspelling of "category" as "catagory"
 *
 * Revision 2.6  2002/11/14 04:18:16  bobboau
 * added warp model and type 1 glow points
 * and well as made the new glow file type,
 * some general improvement to fighter beams,
 *
 * Revision 2.5  2002/10/19 19:29:27  bobboau
 * initial commit, trying to get most of my stuff into FSO, there should be most of my fighter beam, beam rendering, beam shield hit, ABtrails, and ssm stuff. one thing you should be happy to know is the beam texture tileing is now set in the beam section section of the weapon table entry
 *
 * Revision 2.4  2002/08/01 01:41:07  penguin
 * The big include file move
 *
 * Revision 2.3  2002/07/29 08:31:52  DTP
 * Forgot to Bump MAX_MODEL_SUBSYSTEMS from 128 to 200
 *
 * Revision 2.2  2002/07/29 08:28:00  DTP
 * BUMPED MAX_POLYGON_MODELS TO 198 , MAX_SHIP_CLASSES - 2 = 198
 *
 * Revision 2.1  2002/07/10 18:42:14  wmcoolmon
 * Added  Bobboau's glow code; all comments include "-Bobboau"
 *
 * Revision 2.0  2002/06/03 04:02:25  penguin
 * Warpcore CVS sync
 *
 * Revision 1.2  2002/05/03 22:07:09  mharris
 * got some stuff to compile
 *
 * Revision 1.1  2002/05/02 18:03:10  mharris
 * Initial checkin - converted filenames and includes to lower case
 *
 * 
 * 38    9/13/99 10:09a Andsager
 * Add debug console commands to lower model render detail and fireball
 * LOD for big ship explosiosns.
 * 
 * 37    9/01/99 10:09a Dave
 * Pirate bob.
 * 
 * 36    8/24/99 8:55p Dave
 * Make sure nondimming pixels work properly in tech menu.
 * 
 * 35    7/19/99 12:02p Andsager
 * Allow AWACS on any ship subsystem. Fix sexp_set_subsystem_strength to
 * only blow up subsystem if its strength is > 0
 * 
 * 34    7/15/99 2:13p Dave
 * Added 32 bit detection.
 * 
 * 33    7/06/99 10:45a Andsager
 * Modify engine wash to work on any ship that is not small.  Add AWACS
 * ask for help.
 * 
 * 32    6/18/99 5:16p Dave
 * Added real beam weapon lighting. Fixed beam weapon sounds. Added MOTD
 * dialog to PXO screen.
 * 
 * 31    5/26/99 11:46a Dave
 * Added ship-blasting lighting and made the randomization of lighting
 * much more customizable.
 * 
 * 30    5/26/99 9:09a Andsager
 * Increase number of live debris and MAX_MODEL_TEXTURES (for rebel base)
 * 
 * 29    5/24/99 5:45p Dave
 * Added detail levels to the nebula, with a decent speedup. Split nebula
 * lightning into its own section.
 * 
 * 28    5/11/99 10:16p Andsager
 * First pass on engine wash effect.  Rotation (control input), damage,
 * shake.  
 * 
 * 27    4/26/99 8:49p Dave
 * Made all pof based nebula stuff full customizable through fred.
 * 
 * 26    4/23/99 5:53p Dave
 * Started putting in new pof nebula support into Fred.
 * 
 * 25    4/19/99 12:51p Andsager
 * Add function to find the nearest point on extneded bounding box and
 * check if inside bounding box.
 * 
 * 24    4/06/99 9:50a Enricco
 * Bump up max live_debris from 4 to 5
 * 
 * 23    3/31/99 8:24p Dave
 * Beefed up all kinds of stuff, incluging beam weapons, nebula effects
 * and background nebulae. Added per-ship non-dimming pixel colors.
 * 
 * 22    3/23/99 5:17p Dave
 * Changed model file format somewhat to account for LOD's on insignias
 * 
 * 21    3/19/99 9:51a Dave
 * Checkin to repair massive source safe crash. Also added support for
 * pof-style nebulae, and some new weapons code.
 * 
 * 20    3/08/99 7:03p Dave
 * First run of new object update system. Looks very promising.
 * 
 * 19    3/02/99 9:25p Dave
 * Added a bunch of model rendering debug code. Started work on fixing
 * beam weapon wacky firing.
 * 
 * 18    2/19/99 11:42a Dave
 * Put in model rendering autocentering.
 * 
 * 17    2/19/99 11:09a Jasen
 * Upped MAX_MODEL_SUBSYSTEMS to 128 (for the souper cap
 * 
 * 16    1/25/99 5:03a Dave
 * First run of stealth, AWACS and TAG missile support. New mission type
 * :)
 * 
 * 15    1/14/99 6:06p Dave
 * 100% full squad logo support for single player and multiplayer.
 * 
 * 14    1/12/99 12:53a Dave
 * More work on beam weapons - made collision detection very efficient -
 * collide against all object types properly - made 3 movement types
 * smooth. Put in test code to check for possible non-darkening pixels on
 * object textures.
 * 
 * 13    1/11/99 12:56p Andsager
 * stupid merge error
 * 
 * 12    1/11/99 12:42p Andsager
 * Add live debris - debris which is created from a destroyed subsystem,
 * when the ship is still alive
 * 
 * 11    1/08/99 2:08p Dave
 * Fixed software rendering for pofview. Super early support for AWACS and
 * beam weapons.
 * 
 * 10    12/23/98 2:53p Andsager
 * Added stepped rotation support
 * 
 * 9     12/09/98 7:34p Dave
 * Cleanup up nebula effect. Tweaked many values.
 * 
 * 8     12/04/98 3:34p Andsager
 * Handle norotating submodels
 * 
 * 7     12/03/98 3:14p Andsager
 * Check in code that checks rotating submodel actually has ship subsystem
 * 
 * 6     11/19/98 11:07p Andsager
 * Check in of physics and collision detection of rotating submodels
 * 
 * 5     11/11/98 5:37p Dave
 * Checkin for multiplayer testing.
 * 
 * 4     10/23/98 3:03p Andsager
 * Initial support for changing rotation rate.
 * 
 * 3     10/16/98 9:40a Andsager
 * Remove ".h" files from model.h
 * 
 * 2     10/07/98 10:53a Dave
 * Initial checkin.
 * 
 * 1     10/07/98 10:50a Dave
 * 
 * 159   8/28/98 3:29p Dave
 * EMP effect done. AI effects may need some tweaking as required.
 * 
 * 158   5/19/98 8:31p Andsager
 * Added split planes (for big ship explosions)
 * 
 * 157   5/07/98 5:39p Andsager
 * Changes to model to hold cross section info
 * 
 * 156   4/22/98 9:58p John
 * Added code to view invisible faces.
 * 
 * 155   4/22/98 9:43p John
 * Added code to allow checking of invisible faces, flagged by any texture
 * name with invisible in it.
 * 
 * 154   4/01/98 5:34p John
 * Made only the used POFs page in for a level.   Reduced some interp
 * arrays.    Made custom detail level work differently.
 * 
 * 153   3/31/98 5:18p John
 * Removed demo/save/restore.  Made NDEBUG defined compile.  Removed a
 * bunch of debug stuff out of player file.  Made model code be able to
 * unload models and malloc out only however many models are needed.
 *  
 * 
 * 152   3/31/98 11:19a Allender
 * upped MAX_MODEL_SUBSYSTEMS to a resonable value
 * 
 * 151   3/31/98 11:02a Adam
 * upped MAX_MODEL_SUBSYSTEMS to 33
 * 
 * 150   3/25/98 11:23a Mike
 * Fix stack overwrite due to async between MAX_SECONDARY_WEAPONS and
 * MAX_WL_SECONDARY.
 * 
 * 149   3/24/98 10:11p Sandeep
 * 
 * 148   3/24/98 4:03p Lawrance
 * JOHN: Fix up outline drawing code to support different colors
 * 
 * 147   3/22/98 10:19a Adam
 * upped subsystem & secondary weapon constants for Frank's new capital
 * ship.
 * 
 * 146   3/21/98 3:33p Lawrance
 * Allow model outline color to be set directly
 * 
 * 145   3/19/98 5:24p John
 * Added code to find the closest point on a model to another point.  Used
 * this for detail levels and for targetting info.
 * 
 * 144   3/16/98 4:51p John
 * Added low-level code to clip all polygons against an arbritary plane.
 * Took out all old model_interp_zclip and used this new method instead.  
 * 
 * 143   3/13/98 9:06a John
 * Made missile thrusters use a different min dist for scaling.   Made
 * missilies not use lighting.
 * 
 * 142   3/02/98 5:42p John
 * Removed WinAVI stuff from FreeSpace.  Made all HUD gauges wriggle from
 * afterburner.  Made gr_set_clip work good with negative x &y.  Made
 * model_caching be on by default.  Made each cached model have it's own
 * bitmap id.  Made asteroids not rotate when model_caching is on.  
 * 
 * 141   2/24/98 5:04p Allender
 * allow different ship classes to use the same model.  Lot's of subsystem
 * stuff to deal with
 * 
 * 140   2/24/98 1:58p John
 * Made asteroids use model_caching.  Made asteroids darken with distance.
 * 
 * 139   1/29/98 5:50p John
 * Made electrical arcing on debris pieces be persistent from frame to
 * frame
 * 
 * 138   1/27/98 11:02a John
 * Added first rev of sparks.   Made all code that calls model_render call
 * clear_instance first.   Made debris pieces not render by default when
 * clear_instance is called.
 * 
 * 137   1/23/98 5:08p John
 * Took L out of vertex structure used B (blue) instead.   Took all small
 * fireballs out of fireball types and used particles instead.  Fixed some
 * debris explosion things.  Restructured fireball code.   Restructured
 * some lighting code.   Made dynamic lighting on by default. Made groups
 * of lasers only cast one light.  Made fireballs not cast light.
 * 
 * 136   1/15/98 3:28p John
 * Took out the unused ptnorms array.  Saved a measly 8MB of RAM,  so it
 * might not have been worth ripping out.  :-)
 * 
 * 135   1/09/98 11:25a John
 * Took the thruster animation stuff out of the model.
 * 
 * 134   12/31/97 2:35p John
 * Added code for core_radius
 * 
 * 133   12/22/97 8:55a John
 * added parameters for checking models.
 * 
 * 132   12/17/97 5:11p John
 * Added brightening back into fade table.  Added code for doing the fast
 * dynamic gun flashes and thruster flashes.
 * 
 * 131   12/17/97 9:54a John
 * added code for precalculated weapon flash lighting.
 * 
 * 130   12/05/97 3:46p John
 * made ship thruster glow scale instead of being an animation.
 * 
 *
 * $NoKeywords: $
 */

#ifndef _MODEL_H
#define _MODEL_H

#include "globalincs/pstypes.h"
#include "globalincs/globals.h"	// for NAME_LENGTH
#include "graphics/2d.h"
#include "decals/decals.h"

#include <vector>

struct object;


#define MAX_DEBRIS_OBJECTS	32
#define MAX_MODEL_DETAIL_LEVELS	8
#define MAX_PROP_LEN			256
#define MAX_NAME_LEN			32
#define MAX_ARC_EFFECTS		8

#define MOVEMENT_TYPE_NONE				-1
#define MOVEMENT_TYPE_POS				0
#define MOVEMENT_TYPE_ROT				1
#define MOVEMENT_TYPE_ROT_SPECIAL	2		// for turrets only
#define MOVEMENT_TYPE_TRIGGERED			3	//triggered rotation


// DA 11/13/98 Reordered to account for difference between max and game
#define MOVEMENT_AXIS_NONE	-1
#define MOVEMENT_AXIS_X		0
#define MOVEMENT_AXIS_Y		2
#define MOVEMENT_AXIS_Z		1

#define MAX_ROTATING_SUBMODELS 50

// defines for special objects like gun and missile points, docking point, etc
// Hoffoss: Please make sure that subsystem NONE is always index 0, and UNKNOWN is
// always the last subsystem in the list.  Also, make sure that MAX is correct.
// Otherwise, problems will arise in Fred.

#define SUBSYSTEM_NONE				0
#define SUBSYSTEM_ENGINE			1
#define SUBSYSTEM_TURRET			2
#define SUBSYSTEM_RADAR				3
#define SUBSYSTEM_NAVIGATION		4
#define SUBSYSTEM_COMMUNICATION	5
#define SUBSYSTEM_WEAPONS			6
#define SUBSYSTEM_SENSORS			7
#define SUBSYSTEM_SOLAR				8
#define SUBSYSTEM_GAS_COLLECT		9
#define SUBSYSTEM_ACTIVATION		10
#define SUBSYSTEM_UNKNOWN			11

#define SUBSYSTEM_MAX				12				//	maximum value for subsystem_xxx, for error checking

#define MAX_TFP						10				// maximum number of turret firing points

#define MAX_SPLIT_PLANE				3				// number of artist specified split planes (used in big ship explosions)

// Data specific to a particular instance of a submodel.  This gets stuffed/unstuffed using
// the model_clear_instance, model_set_instance, model_get_instance functions.
typedef struct submodel_instance_info {
	int		blown_off;								// If set, this subobject is blown off
	angles	angs;										// The current angle this thing is turned to.
	angles	prev_angs;
	vec3d	pt_on_axis;								// in ship RF
	float		cur_turn_rate;
	float		desired_turn_rate;
	float		turn_accel;
	int		axis_set;
	int		step_zero_timestamp;		// timestamp determines when next step is to begin (for stepped rotation)
	submodel_instance_info();
} submodel_instance_info;

#define MAX_MODEL_SUBSYSTEMS		200				// used in ships.cpp (only place?) for local stack variable DTP; bumped to 200
													// when reading in ships.tbl

#define MSS_FLAG_ROTATES			(1 << 0)		// This means the object rotates automatically with "turn_rate"
#define MSS_FLAG_STEPPED_ROTATE		(1 << 1)		// This means that the rotation occurs in steps
#define MSS_FLAG_AI_ROTATE			(1 << 2)		// This means that the rotation is controlled by ai
#define MSS_FLAG_CREWPOINT			(1 << 3)		// If set, this is a crew point.
#define MSS_FLAG_TURRET_MATRIX		(1 << 4)		// If set, this has it's turret matrix created correctly.
#define MSS_FLAG_AWACS				(1 << 5)		// If set, this subsystem has AWACS capability
#define MSS_FLAG_ARTILLERY			(1 << 6)		// if this rotates when weapons are fired - Goober5000
#define MSS_FLAG_TRIGGERED			(1 << 7)		// rotates when triggered by something
#define MSS_FLAG_UNTARGETABLE		(1 << 8)		// Goober5000
#define MSS_FLAG_CARRY_NO_DAMAGE	(1 << 9)		// WMC
#define MSS_FLAG_USE_MULTIPLE_GUNS	(1 << 10)		// WMC
#define MSS_FLAG_FIRE_ON_NORMAL		(1 << 11)		// forces a turret to fire down its normal vecs
#define MSS_FLAG_TURRET_HULL_CHECK	(1 << 12)		// makes the turret check to see if it's going to shoot through it's own hull before fireing - Bobboau

// definition of stepped rotation struct
typedef struct stepped_rotation {
	int num_steps;				// number of steps in complete revolution
	float fraction;			// fraction of time in step spent in accel
	float t_transit;			// time spent moving from one step to next
	float t_pause;				// time at rest between steps
	float max_turn_rate;		// max turn rate going betweens steps
	float max_turn_accel;	// max accel going between steps
} stepped_rotation_t;

/*typedef struct ai_rotation {
//	void *p_rotation;
	uint type;			//flags for what animation type
	float max;
	float min;
	int time;
} ai_rotation_t;*/

struct queued_animation;

// definition for model subsystems.
typedef struct model_subsystem {					/* contains rotation rate info */

	uint		flags;									// See MSS_FLAG_* defines above
	char		name[MAX_NAME_LEN];					// name of the subsystem.  Probably displayed on HUD
	char		subobj_name[MAX_NAME_LEN];			// Temporary (hopefully) parameter used to match stuff in ships.tbl
	char		alt_sub_name[NAME_LENGTH];					//Karajorma - Name that overrides name of original
	int		subobj_num;								// subobject number (from bspgen) -- used to match subobjects of subsystems to these entries
	int		model_num;								// Which model this is attached to (i.e. the polymodel[] index)
	int		type;										// type. see SUBSYSTEM_* types above.  A generic type thing
	vec3d	pnt;										// center point of this subsystem
	float		radius;									// the extent of the subsystem
	float		max_subsys_strength;					// maximum hits of this subsystem
	int armor_type_idx;								//Armor type on teh subsystem -C

	//	The following items are specific to turrets and will probably be moved to
	//	a separate struct so they don't take up space for all subsystem types.
	char	crewspot[MAX_NAME_LEN];				// unique identifying name for this turret -- used to assign AI class and multiplayer people
	vec3d	turret_norm;						//	direction this turret faces
	matrix	turret_matrix;						// turret_norm converted to a matrix.
	float	turret_fov;							//	dot of turret_norm:vec_to_enemy > this means can see
	int		turret_num_firing_points;			// number of firing points on this turret
	vec3d	turret_firing_point[MAX_TFP];		//	in parent object's reference frame, point from which to fire.
	int		turret_gun_sobj;					// Which subobject in this model the firing points are linked to.
	float	turret_turning_rate;				// How fast the turret turns. Read from ships.tbl
	int		turret_rotation_snd;				// Sound to make when the turret moves

	//Sound stuff
	int		alive_snd;		//Sound to make while the subsystem is not-dead
	int		dead_snd;		//Sound to make when the subsystem is dead.
	int		rotation_snd;	//Sound to make when the subsystem is rotating. (ie turrets)

	// engine wash info
	struct engine_wash_info		*engine_wash_pointer;					// index into Engine_wash_info

	// rotation specific info
	float		turn_rate;							// The turning rate of this subobject, if MSS_FLAG_ROTATES is set.
	int			weapon_rotation_pbank;				// weapon-controlled rotation - Goober5000
	stepped_rotation_t	*stepped_rotation;			// turn rotation struct
//	ai_rotation_t		ai_rotation;				// ai controlled rotation struct - by Bobboau

	// AWACS specific information
	float		awacs_intensity;						// awacs intensity of this subsystem
	float		awacs_radius;							// radius of effect of the AWACS

	int		primary_banks[MAX_SHIP_PRIMARY_BANKS];					// default primary weapons -hoffoss
	int		primary_bank_capacity[MAX_SHIP_PRIMARY_BANKS];		// capacity of a bank - Goober5000
	int		secondary_banks[MAX_SHIP_SECONDARY_BANKS];				// default secondary weapons -hoffoss
	int		secondary_bank_capacity[MAX_SHIP_SECONDARY_BANKS];	// capacity of a bank -hoffoss
	int		path_num;								// path index into polymodel .paths array.  -2 if none exists, -1 if not defined

	decal_system model_decal_system;

	int n_triggers;
	queued_animation *triggers;		//all the triggered animations assosiated with this object
	
	model_subsystem();
} model_subsystem;

typedef struct model_special {
	struct	model_special *next, *prev;		// for using as a linked list
	int		bank;										// used for sequencing gun/missile backs. approach/docking points
	int		slot;										// location for gun or missile in this bank
	vec3d	pnt;										// point where this special submodel thingy is at
	vec3d	norm;										// normal for the special submodel thingy
} model_special;

// model arc types
#define MARC_TYPE_NORMAL					0		// standard freespace 1 blue lightning arcs
#define MARC_TYPE_EMP						1		// EMP blast type arcs

#define MAX_LIVE_DEBRIS	7

struct index_list {
	uint *ibuffer;
	ushort *sbuffer;

	index_list(): ibuffer(NULL), sbuffer(NULL) { }
	// the destuctor body is commented out so that we can use this dynamically without a clone()
	// we kill it off with release() instead
	~index_list() { }

	void allocate(int size, bool large_buf);
	void release();
};

struct buffer_data {
	int texture;		// this is the texture the vertex buffer will use
	int n_prim;
	index_list index_buffer;

	buffer_data(): texture(-1), n_prim(0) {}
};

// IBX stuff
typedef struct IBX {
	CFILE *read;		// reads, if an IBX file already exists
	CFILE *write;		// writes, if new file created
	int size;			// file size used to make sure an IBX contains enough data for the whole model
	int version;		// IBX file version to use: v1 is USHORT only, v2 can mix USHORT and UINT
	char name[MAX_FILENAME_LEN];	// filename of the ibx, this is used in case a safety check fails and we delete the file

	// tangent space data
	CFILE *tsb_read;	// reads tangent space data (TSB), if it already exists
	CFILE *tsb_write;	// writes tangent space data, for new file
	int tsb_size;
	char tsb_name[MAX_FILENAME_LEN];	// filename of the tsb (tangent space model data)
} IBX;



typedef struct bsp_info {
	char		name[MAX_NAME_LEN];	// name of the subsystem.  Probably displayed on HUD
	int		movement_type;			// -1 if no movement, otherwise rotational or positional movement -- subobjects only
	int		movement_axis;			// which axis this subobject moves or rotates on.

	vec3d	offset;					// 3d offset from parent object
	matrix	orientation;			// 3d orientation relative to parent object

	int		bsp_data_size;
	ubyte		*bsp_data;

	vec3d	geometric_center;		// geometric center of this subobject.  In the same Frame Of 
	                              //  Reference as all other vertices in this submodel. (Relative to pivot point)
	float		rad;						// radius for each submodel

	vec3d	min;						// The min point of this object's geometry
	vec3d	max;						// The max point of this object's geometry
	vec3d	bounding_box[8];		// caclulated fron min/max

	int		blown_off;				// If set, this subobject is blown off. Stuffed by model_set_instance
	int		my_replacement;		// If not -1 this subobject is what should get rendered instead of this one
	int		i_replace;				// If this is not -1, then this subobject will replace i_replace when it is damaged
	angles	angs;					// The rotation angles of this subobject (Within its own orientation, NOT relative to parent - KeldorKatarn)

	int		is_live_debris;		// whether current submodel is a live debris model
	int		num_live_debris;		// num live debris models assocaiated with a submodel
	int		live_debris[MAX_LIVE_DEBRIS];	// array of live debris submodels for a submodel

	submodel_instance_info	*sii;	// stuff needed for collision from rotations

	int		is_thruster;
	int		is_damaged;

	// Tree info
	int		parent;					// what is parent for each submodel, -1 if none
	int		num_children;			// How many children this model has
	int		first_child;			// The first_child of this model, -1 if none
	int		next_sibling;			// This submodel's next sibling, -1 if none

	int		num_details;			// How many submodels are lower detail "mirrors" of this submodel
	int		details[MAX_MODEL_DETAIL_LEVELS];		// A list of all the lower detail "mirrors" of this submodel

	// Electrical Arc Effect Info
	// Sets a spark for this submodel between vertex v1 and v2	
	int		num_arcs;												// See model_add_arc for more info	
	vec3d	arc_pts[MAX_ARC_EFFECTS][2];	
	ubyte		arc_type[MAX_ARC_EFFECTS];							// see MARC_TYPE_* defines
	
	// buffers used by HT&L
	int indexed_vertex_buffer;
	std::vector<buffer_data> buffer;
//	int flat_buffer;
//	int flat_line_buffer;

	vec3d	render_box_min;
	vec3d	render_box_max;
	int		use_render_box;	//0==do nothing, 1==only render this object if you are inside the box, -1==only if your out
	bool	gun_rotation;//for animated weapon models
	bool	no_collisions; // for $no_collisions property - kazan

} bsp_info;

void parse_triggersint(int &n_trig, queued_animation **triggers, char *props);

#define MP_TYPE_UNUSED 0
#define MP_TYPE_SUBSYS 1

typedef struct mp_vert {
	vec3d		pos;				// xyz coordinates of vertex in object's frame of reference
	int			nturrets;		// number of turrets guarding this vertex
	int			*turret_ids;	// array of indices into ship_subsys linked list (can't index using [] though)
	float			radius;			// How far the closest obstruction is from this vertex
} mp_vert;

typedef struct model_path {
	char			name[MAX_NAME_LEN];					// name of the subsystem.  Probably displayed on HUD
	char			parent_name[MAX_NAME_LEN];			// parent name of submodel that path is linked to in POF
	int			parent_submodel;
	int			nverts;
	mp_vert		*verts;
	int			goal;			// Which of the verts is the one closest to the goal of this path
	int			type;			// What this path takes you to... See MP_TYPE_??? defines above for details
	int			value;		// This depends on the type.
									// For MP_TYPE_UNUSED, this means nothing.
									// For MP_TYPE_SUBSYS, this is the subsystem number this path takes you to.
} model_path;

typedef struct model_tmap_vert {
	short vertnum;
	short normnum;
	float u,v;
} model_tmap_vert;

// info for gun and missile banks.  Also used for docking points.  There should always
// only be two slots for each docking bay

#define MAX_SLOTS		25

typedef struct w_bank {
	int		num_slots;
	vec3d	pnt[MAX_SLOTS];
	vec3d	norm[MAX_SLOTS];
	float		radius[MAX_SLOTS];
} w_bank;

struct glow_point{
	vec3d	pnt;
	vec3d	norm;
	float	radius;
};

typedef struct thruster_bank {
	int		num_points;
	glow_point *points;

	// Engine wash info
	struct engine_wash_info	*wash_info_pointer;			// index into Engine_wash_info

	int		obj_num;		// what subsystem number this thruster is on
} thruster_bank;

typedef struct glow_point_bank {  // glow bank structure -Bobboau
	int			type;
	int			glow_timestamp; 
	int			on_time; 
	int			off_time; 
	int			disp_time; 
	int			is_on; 
	int			is_active; 
	int			submodel_parent; 
	int			LOD; 
	int			num_points; 
	glow_point	*points;
	int			glow_bitmap; 
	int			glow_neb_bitmap; 
} glow_point_bank;

// defines for docking bay things.  The types are essentially flags since docking bays can probably
// be used for multiple things in some cases (i.e. rearming and general docking)
//WMC - IMPORTANT, update Dock_type_names array if you add a new one of these
extern flag_def_list Dock_type_names[];
extern int Num_dock_type_names;

#define DOCK_TYPE_CARGO				(1<<0)
#define DOCK_TYPE_REARM				(1<<1)
#define DOCK_TYPE_GENERIC			(1<<2)

#define MAX_DOCK_SLOTS	2

typedef struct dock_bay {
	int		num_slots;
	int		type_flags;					// indicates what this docking bay can be used for (i.e. cargo/rearm, etc)
	int		num_spline_paths;			// number of spline paths which lead to this docking bay
	int		*splines;					// array of indices into the Spline_path array
	char		name[MAX_NAME_LEN];		// name of this docking location
	vec3d	pnt[MAX_DOCK_SLOTS];
	vec3d	norm[MAX_DOCK_SLOTS];
} dock_bay;

// struct that holds the indicies into path information associated with a fighter bay on a capital ship
// NOTE: Fighter bay paths are identified by the path_name $bayN (where N is numbered from 1).
//			Capital ships only have ONE fighter bay on the entire ship
#define MAX_SHIP_BAY_PATHS		10
typedef struct ship_bay {
	int	num_paths;							// how many paths are associated with the model's fighter bay
	int	path_indexes[MAX_SHIP_BAY_PATHS];	// index into polymodel->paths[] array
	int	arrive_flags;	// bitfield, set to 1 when that path number is reserved for an arrival
	int	depart_flags;	// bitfield, set to 1 when that path number is reserved for a departure
} ship_bay_t;

// three structures now used for representing shields.
// shield_tri structure stores information concerning each face of the shield.
// verts indexes into the verts array in the higher level structure
// neighbors indexes into the tris array in the higher level structure
typedef struct shield_tri {
  int used;
  int verts[3];			// 3 indices into vertex list of the shield.  list found in shield_info struct
  int neighbors[3];		// indices into array of triangles. neighbor = shares edge.  list found in shield_info struct
  vec3d norm;				// norm of this triangle
} shield_tri;

// a list of these shield_vertex structures comprimises the vertex list of the shield.
// The verts array in the shield_tri structure points to one of these members
typedef struct shield_vertex {
	vec3d	pos;
	float		u,v;
} shield_vertex;

// the high level shield structure.  A ship without any shield has nverts and ntris set to 0.
// The vertex list and the tris list are used by the shield_tri structure
typedef struct shield_info {
	int				nverts;
	int				ntris;
	shield_vertex	*verts;
	shield_tri		*tris;
} shield_info;

#define BSP_LIGHT_TYPE_WEAPON 1
#define BSP_LIGHT_TYPE_THRUSTER 2

typedef struct bsp_light {
	vec3d			pos;
	int				type;		// See BSP_LIGHT_TYPE_?? for values
	float				value;	// How much to light up this light.  0-1.
} bsp_light;

// model_octant - There are 8 of these per model.  They are a handy way to categorize
// a lot of model properties to get some easy 8x optimizations for model stuff.
typedef struct model_octant {
	vec3d		min, max;				// The bounding box that makes up this octant defined as 2 points.
	int			nverts;					// how many vertices are in this octant
	vec3d		**verts;					// The vertices in this octant in the high-res hull.  A vertex can only be in one octant.
	int			nshield_tris;			// how many shield triangles are in the octant
	shield_tri	**shield_tris;			// the shield triangles that make up this octant. A tri could be in multiple octants.
} model_octant;

#define MAX_EYES	10

typedef struct eye {
	int		parent;			// parent's subobject number
	vec3d	pnt;				// the point for the eye
	vec3d	norm;				// direction the eye faces.  Not used with first eye since player orient is used
} eye;

typedef struct cross_section {
	float z;
	float radius;
} cross_section;

#define MAX_MODEL_INSIGNIAS		6
#define MAX_INS_FACE_VECS			3
#define MAX_INS_VECS					20
#define MAX_INS_FACES				10
typedef struct insignia {
	int detail_level;
	int num_faces;					
	int faces[MAX_INS_FACES][MAX_INS_FACE_VECS];		// indices into the vecs array	
	float u[MAX_INS_FACES][MAX_INS_FACE_VECS];		// u tex coords on a per-face-per-vertex basis
	float v[MAX_INS_FACES][MAX_INS_FACE_VECS];		// v tex coords on a per-face-per-vertex bases
	vec3d vecs[MAX_INS_VECS];								// vertex list	
	vec3d offset;	// global position offset for this insignia
	vec3d norm[MAX_INS_VECS]	;					//normal of the insignia-Bobboau
} insignia;

#define PM_FLAG_ALLOW_TILING		(1<<0)					// Allow texture tiling
#define PM_FLAG_AUTOCEN				(1<<1)					// contains autocentering info	

// Goober5000
class texture_info {
private:
	int original_texture;	// what gets read in from file
	int texture;			// what texture you draw with; reset to original_textures by model_set_instance

	//WMC - Removed unneeded struct and is_anim to clean this up.
	//If num_frames is < 2, it doesn't need to be treated like an animation.
	int num_frames;
	float total_time;		// in seconds
public:
	texture_info();
	texture_info(int bm_handle);
	void clear();

	int GetNumFrames();
	int GetOriginalTexture();
	int GetTexture();
	float GetTotalTime();

	int LoadTexture(char *filename, char *dbg_name);

	void PageIn();
	void PageOut(bool release);

	int ResetTexture();
	int SetTexture(int n_tex);
};

#define TM_BASE_TYPE		0		// the standard base map
#define TM_GLOW_TYPE		1		// optional glow map
#define TM_SPECULAR_TYPE	2		// optional specular map
#define TM_NORMAL_TYPE		3		// optional normal map
#define TM_HEIGHT_TYPE		4		// optional height map (for parallax mapping)
#define TM_NUM_TYPES		5		//WMC - Number of texture_info objects in texture_map
									//Used by scripting - if you change this, do a search
									//to update switch() statement in lua.cpp
// taylor
//WMC - OOPified
class texture_map {
public:
	texture_info textures[TM_NUM_TYPES];

	bool is_ambient;
	bool is_transparent;
public:
	int FindTexture(int bm_handle);
	int FindTexture(char *name);

	void PageIn();
	void PageOut(bool release);

	void Reset();
};

#define MAX_REPLACEMENT_TEXTURES MAX_MODEL_TEXTURES * TM_NUM_TYPES

//used to describe a polygon model
typedef struct polymodel {
	int			id;				// what the polygon model number is.  (Index in Polygon_models)
	int			version;
	char			filename[FILESPEC_LENGTH];

	uint			flags;			// 1=allow tiling
	int			n_detail_levels;
	int			detail[MAX_MODEL_DETAIL_LEVELS];
	float			detail_depth[MAX_MODEL_DETAIL_LEVELS];

	int			num_debris_objects;
	int			debris_objects[MAX_DEBRIS_OBJECTS];

	int			n_models;

	vec3d		mins,maxs;							//min,max for whole model
	vec3d		bounding_box[8];

	int			num_lights;							// how many lights there are
	bsp_light *	lights;								// array of light info

	int			n_view_positions;					// number of viewing positions available on this ship
	eye			view_positions[MAX_EYES];		//viewing positions.  Default to {0,0,0}. in location 0

	vec3d		autocenter;							// valid only if PM_FLAG_AUTOCEN is set

	float			rad;									// The radius of everything in the model; shields, thrusters.
	float			core_radius;						// The radius to be used for collision detection in small ship vs big ships.
															// This is equal to 1/2 of the smallest dimension of the hull's bounding box.
	// texture maps for model
	int n_textures;
	texture_map	maps[MAX_MODEL_TEXTURES];
	
	bsp_info		*submodel;							// an array of size n_models of submodel info.

	// linked lists for special polygon types on this model.  Most ships I think will have most
	// of these.  (most ships however, probably won't have approach points).
	int			n_guns;								// number of primary gun points (not counting turrets)
	int			n_missiles;							// number of secondary missile points (not counting turrets)
	int			n_docks;								// number of docking points
	int			n_thrusters;						// number of thrusters on this ship.
	w_bank		*gun_banks;							// array of gun banks
	w_bank		*missile_banks;					// array of missile banks
	dock_bay		*docking_bays;						// array of docking point pairs
	thruster_bank		*thrusters;							// array of thruster objects -- likely to change in the future
	ship_bay_t		*ship_bay;							// contains path indexes for ship bay approach/depart paths

	shield_info	shield;								// new shield information
	ubyte	*shield_collision_tree;
	int		sldc_size;

	int			n_paths;
	model_path	*paths;

	// physics info
	float			mass;
	vec3d		center_of_mass;
	matrix		moment_of_inertia;
	
	model_octant	octants[8];

	int num_xc;				// number of cross sections
	cross_section* xc;	// pointer to array of cross sections (used in big ship explosions)

	int num_split_plane;	// number of split planes
	float split_plane[MAX_SPLIT_PLANE];	// actual split plane z coords (for big ship explosions)

	insignia		ins[MAX_MODEL_INSIGNIAS];
	int			num_ins;

#ifndef NDEBUG
	int			ram_used;		// How much RAM this model uses
	int			debug_info_size;
	char			*debug_info;
#endif

	int used_this_mission;		// used for page-in system, how many times this model has been loaded per mission - taylor

	int n_glow_point_banks;						// number of glow points on this ship. -Bobboau
	glow_point_bank *glow_point_banks;			// array of glow objects -Bobboau

	float gun_submodel_rotation;

} polymodel;

// Call once to initialize the model system
void model_init();

// call at the beginning of a level. after the level has been loaded
void model_level_post_init();

// call to unload a model (works like bm_unload()), "force" SHOULD NEVER BE SET outside of modelread.cpp!!!!
void model_unload(int modelnum, int force = 0);

// Call to free all existing models
void model_free_all();

// Loads a model from disk and returns the model number it loaded into.
int model_load(char *filename, int n_subsystems, model_subsystem *subsystems, int ferror = 1, int duplicate = 0);

// Goober5000
void model_load_texture(polymodel *pm, int i, char *file);

// notify the model system that a ship has died
void model_notify_dead_ship(int objnum);

// Returns a pointer to the polymodel structure for model 'n'
polymodel *model_get(int model_num);

// routine to copy susbsystems.  Must be called when subsystems sets are the same -- see ship.cpp
void model_copy_subsystems(int n_subsystems, model_subsystem *d_sp, model_subsystem *s_sp);

// If MR_FLAG_OUTLINE bit set this color will be used for outlines.
// This defaults to black.
void model_set_outline_color(int r, int g, int b);

void model_set_outline_color_fast(void *outline_color);

// IF MR_LOCK_DETAIL is set, then it will always draw detail level 'n'
// This defaults to 0. (0=highest, larger=lower)
void model_set_detail_level(int n);

// Flags you can pass to model_render
#define MR_NORMAL					(0)			// Draw a normal object
#define MR_SHOW_OUTLINE				(1<<0)		// Draw the object in outline mode. Color specified by model_set_outline_color
#define MR_SHOW_PIVOTS				(1<<1)		// Show the pivot points
#define MR_SHOW_PATHS				(1<<2)		// Show the paths associated with a model
#define MR_SHOW_RADIUS				(1<<3)		// Show the radius around the object
#define MR_SHOW_SHIELDS				(1<<4)		// Show the shield mesh
#define MR_SHOW_THRUSTERS			(1<<5)		// Show the engine thrusters. See model_set_thrust for how long it draws.
#define MR_LOCK_DETAIL				(1<<6)		// Only draw the detail level defined in model_set_detail_level
#define MR_NO_POLYS					(1<<7)		// Don't draw the polygons.
#define MR_NO_LIGHTING				(1<<8)		// Don't perform any lighting on the model.
#define MR_NO_TEXTURING				(1<<9)		// Draw textures as flat-shaded polygons.
#define MR_NO_CORRECT				(1<<10)		// Don't to correct texture mapping
#define MR_NO_SMOOTHING				(1<<11)		// Don't perform smoothing on vertices.
#define MR_IS_ASTEROID				(1<<12)		// When set, treat this as an asteroid.  
#define MR_IS_MISSILE				(1<<13)		// When set, treat this as a missilie.  No lighting, small thrusters.
#define MR_SHOW_OUTLINE_PRESET		(1<<14)		// Draw the object in outline mode. Color assumed to be set already.	
#define MR_SHOW_INVISIBLE_FACES		(1<<15)		// Show invisible faces as green...
#define MR_AUTOCENTER				(1<<16)		// Always use the center of the hull bounding box as the center, instead of the pivot point
#define MR_BAY_PATHS				(1<<17)		// draw bay paths
#define MR_ALL_XPARENT				(1<<18)		// render it fully transparent
#define MR_NO_ZBUFFER				(1<<19)		// switch z-buffering off completely 
#define MR_NO_CULL					(1<<20)		// don't cull backfacing poly's
#define MR_FORCE_TEXTURE			(1<<21)		// force a given texture to always be used
#define MR_FORCE_LOWER_DETAIL		(1<<22)		// force the model to draw 1 LOD lower, if possible
#define MR_EDGE_ALPHA				(1<<23)		// makes norms that are faceing away from you render more transparent -Bobboau
#define MR_CENTER_ALPHA				(1<<24)		// oposite of above -Bobboau
#define MR_NO_FOGGING				(1<<25)		// Don't fog - taylor
#define MR_SHOW_OUTLINE_HTL			(1<<26)		// Show outlines (wireframe view) using HTL method
#define MR_NO_GLOWMAPS				(1<<27)		// disable rendering of glowmaps - taylor
#define MR_FULL_DETAIL				(1<<28)		// render all valid objects, particularly ones that are otherwise in/out of render boxes - taylor

// old/obsolete flags
//#define MR_SHOW_DAMAGE			(1<<4)		// Show the "destroyed" subobjects
//#define MR_ALWAYS_REDRAW			(1<<13)		// Don't do any model caching; redraw this model each frame!

// Renders a model and all it's submodels.
// See MR_? defines for values for flags
void model_render(int model_num, matrix *orient, vec3d * pos, uint flags = MR_NORMAL, int objnum = -1, int lighting_skip = -1, int *replacement_textures = NULL);

// Renders just one particular submodel on a model.
// See MR_? defines for values for flags
void submodel_render(int model_num,int submodel_num, matrix *orient, vec3d * pos, uint flags = MR_NORMAL, int objnum = -1, int *replacement_textures = NULL);

// Returns the radius of a model
float model_get_radius(int modelnum);
float submodel_get_radius(int modelnum, int submodelnum);

// Returns the core radius (smallest dimension of hull's bounding box, used for collision detection with big ships only)
float model_get_core_radius(int modelnum);

// Returns zero is x1,y1,x2,y2 are valid
// returns 1 for invalid model, 2 for point offscreen.
// note that x1,y1,x2,y2 aren't clipped to 2d screen coordinates!
// This function just looks at the radius, and not the orientation, so the
// bounding box won't change depending on the obj's orient.
extern int model_find_2d_bound(int model_num,matrix *orient, vec3d * pos,int *x1, int *y1, int *x2, int *y2);

// Returns zero is x1,y1,x2,y2 are valid
// returns 1 for invalid model, 2 for point offscreen.
// note that x1,y1,x2,y2 aren't clipped to 2d screen coordinates!
// This function looks at the object's bounding box and it's orientation,
// so the bounds will change as the object rotates, to give the minimum bouding
// rect.
extern int model_find_2d_bound_min(int model_num,matrix *orient, vec3d * pos,int *x1, int *y1, int *x2, int *y2);

// Returns zero is x1,y1,x2,y2 are valid
// returns 1 for invalid model, 2 for point offscreen.
// note that x1,y1,x2,y2 aren't clipped to 2d screen coordinates!
// This function looks at the object's bounding box and it's orientation,
// so the bounds will change as the object rotates, to give the minimum bouding
// rect.
int submodel_find_2d_bound_min(int model_num,int submodel, matrix *orient, vec3d * pos,int *x1, int *y1, int *x2, int *y2);


// Returns zero is x1,y1,x2,y2 are valid
// Returns 2 for point offscreen.
// note that x1,y1,x2,y2 aren't clipped to 2d screen coordinates!
// This function just looks at the radius, and not the orientation, so the
// bounding box won't change depending on the obj's orient.
int subobj_find_2d_bound(float radius, matrix *orient, vec3d * pos,int *x1, int *y1, int *x2, int *y2);

// stats variables
#ifndef NDEBUG
extern int modelstats_num_polys;
extern int modelstats_num_polys_drawn;
extern int modelstats_num_verts;
extern int modelstats_num_sortnorms;
#endif

// Tries to move joints so that the turret points to the point dst.
// turret1 is the angles of the turret, turret2 is the angles of the gun from turret
extern int model_rotate_gun(int model_num, model_subsystem *turret, matrix *orient, angles *base_angles, angles *gun_angles, vec3d *pos, vec3d *dst);

// Rotates the angle of a submodel.  Use this so the right unlocked axis
// gets stuffed.
extern void submodel_rotate(model_subsystem *psub, submodel_instance_info * sii);

// Rotates the angle of a submodel.  Use this so the right unlocked axis
// gets stuffed.  Does this for stepped rotations
void submodel_stepped_rotate(model_subsystem *psub, submodel_instance_info *sii);

// Goober5000
// For a submodel, return its overall offset from the main model.
extern void model_find_submodel_offset(vec3d *outpnt, int model_num, int sub_model_num);

// Given a point (pnt) that is in sub_model_num's frame of
// reference, and given the object's orient and position, 
// return the point in 3-space in outpnt.
extern void model_find_world_point(vec3d * outpnt, vec3d *mpnt,int model_num, int sub_model_num, matrix * objorient, vec3d * objpos);

// Given a point in the world RF, find the corresponding point in the model RF.
// This is special purpose code, specific for model collision.
// NOTE - this code ASSUMES submodel is 1 level down from hull (detail[0])
void world_find_model_point(vec3d *out, vec3d *world_pt, polymodel *pm, int submodel_num, matrix *orient, vec3d *pos);

// Given a polygon model index, find a list of rotating submodels to be used for collision
void model_get_rotating_submodel_list(int *submodel_list, int *num_rotating_submodesl, object *objp);

// For a rotating submodel, find a point on the axis
void model_init_submodel_axis_pt(submodel_instance_info *sii, int model_num, int submodel_num);

// Given a direction (pnt) that is in sub_model_num's frame of
// reference, and given the object's orient and position, 
// return the point in 3-space in outpnt.
extern void model_find_world_dir(vec3d * out_dir, vec3d *in_dir,int model_num, int sub_model_num, matrix * objorient, vec3d * objpos);

// Clears all the submodel instances stored in a model to their defaults.
extern void model_clear_instance(int model_num);

// Sets rotating submodel turn info to that stored in model
void model_set_instance_info(submodel_instance_info *sii, float turn_rate, float turn_accel);

// Clears all the values in a particular instance to their defaults.
extern void model_clear_instance_info(submodel_instance_info * sii);

// Sets the submodel instance data in a submodel
extern void model_set_instance(int model_num, int sub_model_num, submodel_instance_info * sii);

// Adds an electrical arcing effect to a submodel
void model_add_arc(int model_num, int sub_model_num, vec3d *v1, vec3d *v2, int arc_type);

// Fills in an array with points from a model.  Only gets up to max_num verts.
// Returns number of verts found
extern int submodel_get_points(int model_num, int submodel_num, int max_num, vec3d **nts);

// Gets two random points on the surface of a submodel
extern void submodel_get_two_random_points(int model_num, int submodel_num, vec3d *v1, vec3d *v2, vec3d *n1 = NULL, vec3d *n2 = NULL);

// gets the index into the docking_bays array of the specified type of docking point
// Returns the index.  second functions returns the index of the docking bay with
// the specified name
extern int model_find_dock_index(int modelnum, int dock_type, int index_to_start_at = 0);
extern int model_find_dock_name_index(int modelnum, char *name);

// returns the actual name of a docking point on a model, needed by Fred.
char *model_get_dock_name(int modelnum, int index);

// returns number of docking points for a model
int model_get_num_dock_points(int modelnum);
int model_get_dock_index_type(int modelnum, int index);

// get all the different docking point types on a model
int model_get_dock_types(int modelnum);

// Goober5000
// returns index in [0, MAX_SHIP_BAY_PATHS)
int model_find_bay_path(int modelnum, char *bay_path_name);

// Returns number of verts in a submodel;
int submodel_get_num_verts(int model_num, int submodel_num);

// Returns number of polygons in a submodel;
int submodel_get_num_polys(int model_num, int submodel_num);

// Given a vector that is in sub_model_num's frame of
// reference, and given the object's orient and position,
// return the vector in the model's frame of reference.
void model_find_obj_dir(vec3d *w_vec, vec3d *m_vec, object *ship_obj, int sub_model_num);


// This is the interface to model_check_collision.  Rather than passing all these
// values and returning values in globals, just fill in a temporary variable with
// the input values and call model_check_collision
typedef struct mc_info {
	// Input values
	int		model_num;			// What model to check
	int		submodel_num;		// What submodel to check if MC_SUBMODEL is set
	matrix	*orient;				// The orient of the model
	vec3d	*pos;					// The pos of the model in world coordinates
	vec3d	*p0;					// The starting point of the ray (sphere) to check
	vec3d	*p1;					// The ending point of the ray (sphere) to check
	int		flags;				// Flags that the model_collide code looks at.  See MC_??? defines
	float		radius;				// If MC_CHECK_THICK is set, checks a sphere moving with the radius.
	
	// Return values
	int		num_hits;			// How many collisions were found
	float		hit_dist;			// The distance from p0 to hitpoint
	vec3d	hit_point;			// Where the collision occurred at in hit_submodel's coordinate system
	vec3d	hit_point_world;	// Where the collision occurred at in world coordinates
	int		hit_submodel;		// Which submodel got hit.
	int		hit_bitmap;			// Which texture got hit.  -1 if not a textured poly
	float		hit_u, hit_v;		// Where on hit_bitmap the ray hit.  Invalid if hit_bitmap < 0
	int		shield_hit_tri;	// Which triangle on the shield got hit or -1 if none
	vec3d	hit_normal;			//	Vector normal of polygon of collision.  (This is in submodel RF)
	int		edge_hit;			// Set if an edge got hit.  Only valid if MC_CHECK_THICK is set.	
	ubyte		*f_poly;				// pointer to flat poly where we intersected
	ubyte		*t_poly;				// pointer to tmap poly where we intersected
		
										// flags can be changed for the case of sphere check finds an edge hit
} mc_info;



//======== MODEL_COLLIDE ============

//	Model Collision flags, used in model_collide()
#define MC_CHECK_MODEL			(1<<0)			// Check the polygons in the model.
#define MC_CHECK_SHIELD			(1<<1)			//	check for collision against shield, if it exists.
#define MC_ONLY_SPHERE			(1<<2)			// Only check bounding sphere. Not accurate, but fast.  
															// NOTE!  This doesn't set hit_point correctly with MC_CHECK_SPHERELINE
#define MC_ONLY_BOUND_BOX		(1<<3)			// Only check bounding boxes.  Pretty accurate and slower than MC_ONLY_SPHERE.
															// Checks the rotatated bounding box of each submodel.  
															// NOTE!  This doesn't set hit_point correctly with MC_CHECK_SPHERELINE
#define MC_CHECK_RAY				(1<<4)			// Checks a ray from p0 *through* p1 on to infinity
#define MC_CHECK_SPHERELINE	(1<<5)			// Checks a moving sphere rather than just a ray.  Radius
#define MC_SUBMODEL				(1<<6)			// If this is set, only check the submodel specified in mc->submodel_num. Use with MC_CHECK_MODEL
#define MC_SUBMODEL_INSTANCE	(1<<7)			// Check submodel and its children (of a rotating submodel)
#define MC_CHECK_INVISIBLE_FACES (1<<8)		// Check the invisible faces.


/*
   Checks to see if a vector from p0 to p0 collides with a model of
   type 'model_num' at 'orient' 'pos'.

   Returns the number of polys that were hit.  Zero is none, obviously.
  	Return true if a collision with hull (or shield, if MC_CHECK_SHIELD set), 
	else return false.

   If it did it one or more, then hitpt is the closest 3d point that the
   vector hit.  See the MC_? defines for flag values.

   Model_collide can test a sphere against either (1) shield or (2) model.

   To check a sphere, set the radius of sphere in mc_info structure and
   set the flag MC_CHECK_SPHERE.

   Here is a sample for how to use:
  
	mc_info mc;

	mc.model_num = ???;			// Fill in the model to check
	mc.orient = &obj->orient;	// The object's orient
	mc.pos = &obj->pos;			// The object's position
	mc.p0 = &p0;					// Point 1 of ray to check
	mc.p1 = &p1;					// Point 2 of ray to check
	mc.flags = MC_CHECK_MODEL;	// flags

** TO COLLIDE AGAINST A LINE SEGMENT

  model_collide(&mc);
	if (mc.num_hits) {		
		// We hit submodel mc.hit_submodel on texture mc.hitbitmap,
		// at point mc.hit_point_world, with uv's of mc.hit_u, mc.hit_v.
	}

** TO COLLIDE AGAINST A SPHERE
	mc.flags |= MC_CHECK_SPHERELINE;
	mc.radius = radius;

	model_collide(&mc, radius);
	if (mc.num_hits) {		
		// We hit submodel mc.hit_submodel on texture mc.hitbitmap,
		// at point mc.hit_point_world, with uv's of mc.hit_u, mc.hit_v.
		// Check (mc.edge_hit) to see if we hit an edge
	}
*/

int model_collide(mc_info * mc_info);

// Sets the submodel instance data in a submodel
// If show_damaged is true it shows only damaged submodels.
// If it is false it shows only undamaged submodels.
void model_show_damaged(int model_num, int show_damaged);


//=========================== MODEL OCTANT STUFF ================================

//  Models are now divided into 8 octants.    Shields too.
//  This made the collision code faster.   Shield is 4x and ship faces
//  are about 2x faster.

//  Before, calling model_collide with flags=0 didn't check the shield
//  but did check the model itself.   Setting the shield flags caused
//  the shield to get check along with the ship.
//  Now, you need to explicitly tell the model_collide code to check
//  the model, so you can check the model or shield or both.

//  If you need to check them both, do it in one call; this saves some
//  time.    If checking the shield is sufficient for determining 
//  something   (like if it is under the hud) then use just shield 
//  check, it is at least 5x faster than checking the model itself.


// Model octant ordering - this is a made up ordering, but it makes sense.
// X Y Z  index description
// - - -  0     left bottom rear
// - - +  1     left bottom front
// - + -  2     left top rear
// - + +  3     left top front
// + - -  4     right bottom rear
// + - +  5     right bottom front
// + + -  6     right top rear
// + + +  7     right top front

// Returns which octant point 'pnt' is closet to. This will always return 
// a valid octant (0-7) since the point doesn't have to be in an octant.
// If model_orient and/or model_pos are NULL, pnt is assumed to already 
// be rotated into the model's local coordinates.  
// If oct is not null, it will be filled in with a pointer to the octant
// data.
int model_which_octant_distant(vec3d *pnt, int model_num,matrix *model_orient, vec3d * model_pos, model_octant **oct);

// Returns which octant point 'pnt' is in. This might return
// -1 if the point isn't in any octant.
// If model_orient and/or model_pos are NULL, pnt is assumed to already 
// be rotated into the model's local coordinates.
// If oct is not null, it will be filled in with a pointer to the octant
// data.  Or NULL if the pnt isn't in the octant.
int model_which_octant(vec3d *pnt, int model_num,matrix *model_orient, vec3d * model_pos, model_octant **oct);

typedef struct mst_info {
	int primary_bitmap;
	int primary_glow_bitmap;
	int secondary_glow_bitmap;
	int tertiary_glow_bitmap;

	bool use_ab;
	float glow_noise;
	const vec3d *rotvel;
	vec3d length;

	float glow_rad_factor;
	float secondary_glow_rad_factor;
	float tertiary_glow_rad_factor;
	float glow_length_factor;

	mst_info() : primary_bitmap(-1), primary_glow_bitmap(-1), secondary_glow_bitmap(-1), tertiary_glow_bitmap(-1),
					use_ab(false), glow_noise(1.0f), rotvel(NULL), length(vmd_zero_vector), glow_rad_factor(1.0f),
					secondary_glow_rad_factor(1.0f), tertiary_glow_rad_factor(1.0f), glow_length_factor(1.0f)
				{}
} mst_info;

// scale the engines thrusters by this much
// Only enabled if MR_SHOW_THRUSTERS is on
void model_set_thrust(int model_num = -1, mst_info *mst = NULL);

//=========================================================
// model caching

// Call once to init the model caching stuff
//void model_cache_init();

// Call before every level to clean up the model caching stuff
//void model_cache_reset();

// If TRUE, then model caching is enabled
//extern int Model_caching;


//=======================================================================================
// Finds the closest point on a model to a point in space.  Actually only finds a point
// on the bounding box of the model.    
// Given:
//   model_num      Which model
//   submodel_num   Which submodel, -1 for hull
//   orient         Orientation of the model
//   pos            Position of the model
//   eye_pos        Point that you want to find the closest point to
// Returns:
//   distance from eye_pos to closest_point.  0 means eye_pos is 
//   on or inside the bounding box.
//   Also fills in outpnt with the actual closest point.
float model_find_closest_point(vec3d *outpnt, int model_num, int submodel_num, matrix *orient, vec3d * pos, vec3d *eye_pos);

// set the insignia bitmap to be used when rendering a ship with an insignia (-1 switches it off altogether)
void model_set_insignia_bitmap(int bmap);

// set model transparency for use with MR_ALL_XPARENT
void model_set_alpha(float alpha);

// set the forces bitmap
void model_set_forced_texture(int bmap);

// see if the given texture is used by the passed model. 0 if not used, 1 if used, -1 on error
int model_find_texture(int model_num, int bitmap);

// find closest point on extended bounding box (the bounding box plus all the planes that make it up)
// returns closest distance to extended box
// positive return value means start_point is outside extended box
// displaces closest point an optional amount delta to the outside of the box
// closest_box_point can be NULL.
float get_world_closest_box_point_with_delta(vec3d *closest_box_point, object *box_obj, vec3d *start_point, int *is_inside, float delta);

// given a newly loaded model, page in all textures
void model_page_in_textures(int modelnum, int ship_info_index = -1);

// given a model, unload all of its textures
void model_page_out_textures(int model_num, bool release = false);

void model_set_warp_globals(float scale_x = 1.0f, float scale_y = 1.0f, float scale_z = 1.0f, int bitmap_id = -1, float alpha = -1.0f);

void model_set_replacement_textures(int *replacement_textures);

int decal_make_model(polymodel * pm);

void model_setup_cloak(vec3d *shift, int full_cloak, int alpha);
void model_finish_cloak(int full_cloak);

#endif // _MODEL_H
