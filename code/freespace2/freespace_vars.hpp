#include <algorithm>

Freespace_context::Freespace_context()
{
	_this = this;
	Env_cubemap_drawn = false;
	Game_no_clear = 0;

	#define DEFAULT_SKILL_LEVEL	1
	Game_skill_level = DEFAULT_SKILL_LEVEL;

	// JAS: Code for warphole camera.
	// Needs to be cleaned up.
	Warpout_time = 0.0f;
	Warpout_forced = 0;		// Set if this is a forced warpout that cannot be cancelled.
	Warpout_sound = -1;
	Use_joy_mouse = 0;
	Use_palette_flash = 1;
	#ifndef NDEBUG
	Use_fullscreen_at_startup = 0;
	#endif
	Show_area_effect = 0;
	object	*Last_view_target = NULL;

	dogfight_blown = 0;

	frame_int = -1;
	frametimes[FRAME_FILTER];
	frametotal = 0.0f;
	flRealframetime;
	flFrametime;
	FrametimeOverall = 0;

	#ifndef NDEBUG
	Show_framerate = 1;
	Show_mem = 1;
	#else 
	Show_framerate = 0;
	Show_mem = 0;
	#endif

	Framerate_cap = 120;

	// to determine if networking should be disabled, needs to be done first thing
	Networking_disabled = 0;

	Show_cpu = 0;
	Show_target_debug_info = 0;
	Show_target_weapons = 0;
	Game_font = -1;
	#ifndef NDEBUG
	Show_player_pos = 0;		// debug console command to show player world pos on HUD
	#endif

	Debug_octant = -1;

	Game_time_compression = F1_0;
	Desired_time_compression = Game_time_compression;
	Time_compression_change_rate = 0;
	Time_compression_locked = false; //Can the user change time with shift- controls?

	// auto-lang stuff
	detect_lang();

	// table checksums that will be used for pilot files
	uint Weapon_tbl_checksum = 0;
	uint Ships_tbl_checksum = 0;

	// if the ships.tbl the player has is valid
	Game_ships_tbl_valid = 0;

	// if the weapons.tbl the player has is valid
	Game_weapons_tbl_valid = 0;

	//#ifndef NDEBUG
	Test_begin = 0;
	Show_net_stats;
	//#endif

	Pre_player_entry;

	Fred_running = 0;

	Game_current_mission_filename[MAX_FILENAME_LEN];
	game_single_step = 0;
	last_single_step=0;

	game_zbuffer = 1;
	//  Game_music_paused;
	Game_paused;

	Game_level_seed;

	#define EXPIRE_BAD_CHECKSUM			1
	#define EXPIRE_BAD_TIME					2

	// defines and variables used for dumping frame for making trailers.
	#ifndef NDEBUG
	Debug_dump_frames = 0;			// Set to 0 to not dump frames, else equal hz to dump. (15 or 30 probably)
	Debug_dump_trigger = 0;
	Debug_dump_frame_count;
	Debug_dump_frame_num = 0;
	#define DUMP_BUFFER_NUM_FRAMES	1			// store every 15 frames
	#endif

	// amount of time to wait after the player has died before we display the death died popup
	#define PLAYER_DIED_POPUP_WAIT		2500
	Player_died_popup_wait = -1;
	// Player_multi_died_check = -1;

	Multi_ping_timestamp = -1;

	Default_env_map = -1;

	// builtin mission list stuff
	#ifdef FS2_DEMO
	Game_builtin_mission_count = 6;
	fs_builtin_mission Game_builtin_mission_list_init[MAX_BUILTIN_MISSIONS] = {
	{ "SPDemo-01.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							""		},
	{ "SPDemo-02.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							""		},
	{ "DemoTrain.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							""		},
	{ "Demo.fc2",						(FSB_FROM_VOLITION | FSB_CAMPAIGN_FILE),					""		},
	{ "MPDemo-01.fs2",				(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "Demo-DOG-01.fs2",				(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	};
	#elif defined(PD_BUILD)
	Game_builtin_mission_count = 4;
	fs_builtin_mission Game_builtin_mission_list_init[MAX_BUILTIN_MISSIONS] = {
	{ "sm1-01.fs2",					(FSB_FROM_VOLITION),												""		},
	{ "sm1-05.fs2",					(FSB_FROM_VOLITION),												""		},		
	{ "sm1-01",							(FSB_FROM_VOLITION),												""		},
	{ "sm1-05",							(FSB_FROM_VOLITION),												""		},		
	};
	#elif defined(MULTIPLAYER_BETA)
	Game_builtin_mission_count = 17;
	fs_builtin_mission Game_builtin_mission_list_init[MAX_BUILTIN_MISSIONS] = {
	// multiplayer beta
	{ "md-01.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "md-02.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "md-03.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "md-04.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "md-05.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "md-06.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "md-07.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "mt-02.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "mt-03.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "m-03.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "m-04.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "m-05.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""		},
	{ "templar-01.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""		},
	{ "templar-02.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""		},
	{ "templar-03a.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""		},
	{ "templar-04a.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""		},
	{ "templar.fc2",					(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN_FILE),	""		},	
	};
	#elif defined(OEM_BUILD)
	Game_builtin_mission_count = 17;
	fs_builtin_mission Game_builtin_mission_list_init[MAX_BUILTIN_MISSIONS] = {
	// oem version - act 1 only
	{ "freespace2oem.fc2",			(FSB_FROM_VOLITION | FSB_CAMPAIGN_FILE),					"" },

	// act 1
	{ "sm1-01.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "sm1-02.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "sm1-03.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "sm1-04.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "sm1-05.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "sm1-06.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "sm1-07.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "sm1-08.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "sm1-09.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "sm1-10.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "training-1.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "training-2.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "training-3.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "tsm-104.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "tsm-105.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	},
	{ "tsm-106.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_1	}
	};
	#else 
	Game_builtin_mission_count = 92;
	fs_builtin_mission Game_builtin_mission_list_init[MAX_BUILTIN_MISSIONS] = {
	// single player campaign
	{ "freespace2.fc2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN_FILE),					"" },

	// act 1
	{ "sm1-01.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-02.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-03.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-04.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-05.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-06.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-07.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-08.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-09.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-10.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "loop1-1.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "loop1-2.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "loop1-3.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "training-1.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "training-2.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "training-3.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "tsm-104.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "tsm-105.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "tsm-106.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},

	// act 2
	{ "sm2-01.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-02.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-03.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-04.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-05.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-06.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-07.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-08.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-09.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-10.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},

	// act 3
	{ "sm3-01.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-02.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-03.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-04.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-05.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-06.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-07.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-08.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-09.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-10.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "loop2-1.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},		
	{ "loop2-2.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},

	// multiplayer missions

	// gauntlet
	{ "g-shi.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "g-ter.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "g-vas.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},

	// coop
	{ "m-01.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "m-02.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "m-03.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "m-04.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},		

	// dogfight
	{ "mdh-01.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-02.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-03.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-04.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-05.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-06.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-07.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-08.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-09.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-01.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-02.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-03.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-04.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-05.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-06.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-07.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-08.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-09.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-01.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-02.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-03.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-04.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-05.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-06.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-07.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-08.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-09.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "osdog.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},

	// TvT		
	{ "mt-01.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-02.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-03.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-04.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-05.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-06.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-07.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-08.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-09.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-10.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},				

	// campaign
	{ "templar.fc2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN_FILE),					"" },
	{ "templar-01.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""						},				
	{ "templar-02.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""						},				
	{ "templar-03.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""						},				
	{ "templar-04.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""						},				
	};
	#endif
	std::copy(Game_builtin_mission_list_init,Game_builtin_mission_list_init+Game_builtin_mission_count,Game_builtin_mission_list);

	// loading background filenames
	std::string Game_loading_bground_fname_init[GR_NUM_RESOLUTIONS] = {
	"LoadingBG",		// GR_640
	"2_LoadingBG"		// GR_1024
	};

	std::string Game_loading_ani_fname_init[GR_NUM_RESOLUTIONS] = {
	"Loading.ani",		// GR_640
	"2_Loading.ani"		// GR_1024
	};

	#if defined(OEM_BUILD)
	std::string Game_title_screen_fname_init[GR_NUM_RESOLUTIONS] = {
	"OEMPreLoad",
	"2_OEMPreLoad"
	};
	#else
	std::string Game_title_screen_fname_init[GR_NUM_RESOLUTIONS] = {
	"PreLoad",
	"2_PreLoad"
	};
	#endif
	std::string Game_logo_screen_fname_init[GR_NUM_RESOLUTIONS] = {
	"PreLoadLogo",
	"2_PreLoadLogo"
	};
	std::copy(Game_loading_bground_fname_init, Game_loading_bground_fname_init+GR_NUM_RESOLUTIONS, Game_loading_bground_fname);
	std::copy(Game_loading_ani_fname_init, Game_loading_ani_fname_init+GR_NUM_RESOLUTIONS, Game_loading_ani_fname);
	std::copy(Game_title_screen_fname_init, Game_title_screen_fname_init+GR_NUM_RESOLUTIONS, Game_title_screen_fname);
	std::copy(Game_logo_screen_fname_init, Game_logo_screen_fname_init+GR_NUM_RESOLUTIONS, Game_logo_screen_fname);

	// for title screens
	Game_title_bitmap = -1;
	Game_title_logo = -1;

	// How much RAM is on this machine. Set in WinMain
	uint FreeSpace_total_ram = 0;

	// game flash stuff
	Game_flash_red = 0.0f;
	Game_flash_green = 0.0f;
	Game_flash_blue = 0.0f;
	Sun_spot = 0.0f;
	big_expl_flash Big_expl_flash = {0.0f, 0.0f, 0};

	// game shudder stuff (in ms)
	Game_shudder_time = -1;
	Game_shudder_total = 0;
	Game_shudder_intensity = 0.0f;			// should be between 0.0 and 100.0

	// EAX stuff
	sound_env Game_sound_env;
	sound_env Game_default_sound_env = {SND_ENV_BATHROOM, 0.2F,0.2F,1.0F};
	Game_sound_env_update_timestamp;

	Gf_critical = -1.0f;					// framerate we should be above on the average for this mission
	Gf_critical_time = 0.0f;			// how much time we've been at the critical framerate

	//	Amount to diminish palette towards normal, per second.
	#define	DIMINISH_RATE	0.75f
	#define	SUN_DIMINISH_RATE	6.00f

	Sun_drew = 0;

	sn_glare_scale = 1.7f;

	Supernova_last_glare = 0.0f;

	uint load_gl_init;
	uint load_mission_load;
	uint load_post_level_init;
	//uint load_mission_stuff;

	// An estimate as to how high the count passed to game_loading_callback will go.
	// This is just a guess, it seems to always be about the same.   The count is
	// proportional to the code being executed, not the time, so this works good
	// for a bar, assuming the code does about the same thing each time you
	// load a level.   You can find this value by looking at the return value
	// of game_busy_callback(NULL), which I conveniently print out to the
	// debug output window with the '=== ENDING LOAD ==' stuff.   
	//#define COUNT_ESTIMATE 3706
	//#define COUNT_ESTIMATE 1111
	//#define COUNT_ESTIMATE 2311
	//#define COUNT_ESTIMATE 1250
	#define COUNT_ESTIMATE 425

	Game_loading_callback_inited = 0;

	Game_loading_background = -1;
	Game_loading_ani = NULL;
	Game_loading_frame=-1;

	int Game_loading_ani_coords_init[GR_NUM_RESOLUTIONS][2] = {
	{
	63, 316  // GR_640
	},
	{
	101, 505	// GR_1024
	}};

	for(size_t j = 0; j < GR_NUM_RESOLUTIONS; ++j)
	{
	for(size_t i = 0; i < 2; ++i)
	{
	Game_loading_ani_coords[j][i] = Game_loading_ani_coords_init[j][i];
	}
	}

	#ifndef NDEBUG
	busy_shader_created = 0;
	busy_shader;
	#endif

	Interface_framerate = 0;

	Game_init_seed;

	Use_low_mem = 0;

	Framerate_delay = 0;

	FreeSpace_gamma = 1.0f;

	#ifdef APPLE_APP
	full_path[1024];
	#endif


	transfer_text[128];

	Start_time = 0.0f;

	Framerate = 0.0f;

	#ifndef NDEBUG
	Timing_total = 0.0f;
	Timing_render2 = 0.0f;
	Timing_render3 = 0.0f;
	Timing_flip = 0.0f;
	Timing_clear = 0.0f;
	#endif

	MONITOR(NumPolysDrawn)
	MONITOR(NumPolys)
	MONITOR(NumVerts)
	MONITOR(BmpUsed)
	MONITOR(BmpNew)


	uint Mem_starttime_phys;
	uint Mem_starttime_pagefile;
	uint Mem_starttime_virtual;

	Player_died_time;

	View_percent = 100;

	tst = 0;
	tst_time = 0;
	tst_big = 0;
	tst_bitmap = -1;

	#define	MIN_DIST_TO_DEAD_CAMERA		50.0f
	Show_viewing_from_self = 0;


	Game_hit_x = 0.0f;
	Game_hit_y = 0.0f;

	#define FF_SCALE	10000

	//	Player's velocity just before he blew up.  Used to keep camera target moving.
	Dead_player_last_vel = (vec3d) { { { 1.0f, 1.0f, 1.0f } } };

	Scripting_didnt_draw_hud = 1;

	#ifndef NDEBUG
	extern void ai_debug_render_stuff();
	#endif

	Game_subspace_effect = 0;

	#define	MAX_FRAMETIME	(F1_0/4)		// Frametime gets saturated at this.  Changed by MK on 11/1/97.
									//	Some bug was causing Frametime to always get saturated at 2.0 seconds after the player
									//	died.  This resulted in screwed up death sequences.

	Last_time = 0;						// The absolute time of game at end of last frame (beginning of this frame)
	Last_delta_time = 0;				// While game is paused, this keeps track of how much elapsed in the frame before paused.
	timer_paused=0;
	#if defined(TIMER_TEST) && !defined(NDEBUG)
	stop_count,start_count;
	time_stopped,time_started;
	#endif
	saved_timestamp_ticker = -1;


	Joymouse_button_status = 0;

	// variable used for automatic netgame starting/joining
	Main_hall_netgame_started = 0;


	// Called once a frame.
	// You should never try to change the state
	// in here... if you think you need to, you probably really
	// need to post an event, not change the state.
	Game_do_state_should_skip = 0;

	//////////////////////////////////////////////////////////////////////////
	//
	// Code for supporting an animating mouse pointer
	//
	//
	//////////////////////////////////////////////////////////////////////////


	/////////// Following is for event debug view screen

	#ifndef NDEBUG

	#define EVENT_DEBUG_MAX	5000
	#define EVENT_DEBUG_EVENT 0x8000

	Event_debug_index[EVENT_DEBUG_MAX];
	ED_count;

	#endif // NDEBUG

	#ifndef NDEBUG

	Tmap_npixels=0;
	Tmap_num_too_big = 0;
	Num_models_needing_splitting = 0;

	Time_models = 0;

	#endif

	// ----------------------------------------------------------------
	//
	// OEM UPSELL SCREENS BEGIN
	//
	// ----------------------------------------------------------------
	#if defined(OEM_BUILD)

	#define NUM_OEM_UPSELL_SCREENS				3
	#define OEM_UPSELL_SCREEN_DELAY				10000

	Oem_upsell_bitmaps_loaded = 0;
	Oem_upsell_bitmaps[GR_NUM_RESOLUTIONS][NUM_OEM_UPSELL_SCREENS];
	Oem_upsell_screen_number = 0;
	Oem_upsell_show_next_bitmap_time;

	//XSTR:OFF
	*Oem_upsell_bitmap_filenames[GR_NUM_RESOLUTIONS][NUM_OEM_UPSELL_SCREENS] = 
	{
	{	"OEMUpSell02",
	"OEMUpSell01",
	"OEMUpSell03",
	},
	{	"2_OEMUpSell02",
	"2_OEMUpSell01",
	"2_OEMUpSell03",
	},
	};
	//XSTR:ON

	Oem_normal_cursor = -1;
	Oem_web_cursor = -1;
	//#define OEM_UPSELL_URL		"http://www.interplay-store.com/"
	#define OEM_UPSELL_URL		"http://www.interplay.com/cgi-bin/oemlinks.pl/pid=483421&cid=18384"

	// clickable hotspot on 3rd OEM upsell screen
	Oem_upsell3_button_coords[GR_NUM_RESOLUTIONS][4] = {
	{	// GR_640
	28, 350, 287, 96					// x, y, w, h
	},
	{	// GR_1024
	45, 561, 460, 152					// x, y, w, h
	}
	};


	#endif // defined(OEM_BUILD)
	// ----------------------------------------------------------------
	//
	// OEM UPSELL SCREENS END
	//
	// ----------------------------------------------------------------



	// ----------------------------------------------------------------
	//
	// DEMO UPSELL SCREENS BEGIN
	//
	// ----------------------------------------------------------------

	#ifdef FS2_DEMO

	//#define NUM_DEMO_UPSELL_SCREENS				4

	#define NUM_DEMO_UPSELL_SCREENS				2
	#define DEMO_UPSELL_SCREEN_DELAY				3000

	Demo_upsell_bitmaps_loaded = 0;
	Demo_upsell_bitmaps[GR_NUM_RESOLUTIONS][NUM_DEMO_UPSELL_SCREENS];
	Demo_upsell_screen_number = 0;
	Demo_upsell_show_next_bitmap_time;

	//XSTR:OFF
	*Demo_upsell_bitmap_filenames[GR_NUM_RESOLUTIONS][NUM_DEMO_UPSELL_SCREENS] = 
	{
	{	"UpSell02",
	"UpSell01",
	},
	{	"2_UpSell02",
	"2_UpSell01",
	},
	// "DemoUpsell3",
	// "DemoUpsell4",
	};
	//XSTR:ON


	#endif // DEMO

	// ----------------------------------------------------------------
	//
	// DEMO UPSELL SCREENS END
	//
	// ----------------------------------------------------------------


	// ----------------------------------------------------------------
	//
	// Subspace Ambient Sound START
	//
	// ----------------------------------------------------------------

	Subspace_ambient_left_channel = -1;
	Subspace_ambient_right_channel = -1;

	// ----------------------------------------------------------------
	//
	// Subspace Ambient Sound END
	//
	// ----------------------------------------------------------------


	// ----------------------------------------------------------------
	//
	// CDROM detection code START
	//
	// ----------------------------------------------------------------

	#define CD_SIZE_72_MINUTE_MAX			(697000000)

	Last_cd_label_found = 0;
	Last_cd_label[256];

	// ----------------------------------------------------------------
	//
	// CDROM detection code END
	//
	// ----------------------------------------------------------------


	// ----------------------------------------------------------------
	// Language autodetection stuff
	//

	// this layout *must* match Lcl_languages in localize.cpp in order for the
	// correct language to be detected
	int Lang_auto_detect_checksums_init[] = {
	589986744,						// English
	-1132430286,					// German
	0								// French
	};
	std::copy(Lang_auto_detect_checksums_init, Lang_auto_detect_checksums_init+3, Lang_auto_detect_checksums);

	//
	// Eng Auto Lang stuff
	// ----------------------------------------------------------------


	// ----------------------------------------------------------------
	// SHIPS TBL VERIFICATION STUFF
	//

	// checksums, just keep a list of all valid ones, if it matches any of them, keep it
	#define NUM_SHIPS_TBL_CHECKSUMS		1

	#ifdef FS2_DEMO
	int Game_ships_tbl_checksums_init[NUM_SHIPS_TBL_CHECKSUMS] = {
	//-463907578,						// US - beta 1
	1696074201,						// FS2 demo
	};
	#else
	int Game_ships_tbl_checksums_init[NUM_SHIPS_TBL_CHECKSUMS] = {
	//	-1022810006,					// 1.0 FULL
	-1254285366						// 1.2 FULL (German)
	};
	#endif

	std::copy(Game_ships_tbl_checksums_init, Game_ships_tbl_checksums_init+NUM_SHIPS_TBL_CHECKSUMS, Game_ships_tbl_checksums);


	// ----------------------------------------------------------------
	// WEAPONS TBL VERIFICATION STUFF
	//

	// checksums, just keep a list of all valid ones, if it matches any of them, keep it
	#define NUM_WEAPONS_TBL_CHECKSUMS		1

	#ifdef FS2_DEMO
	int Game_weapons_tbl_checksums_init[NUM_WEAPONS_TBL_CHECKSUMS] = {
	//141718090,				// US - beta 1
	-266420030,				// demo 1
	};
	#else
	int Game_weapons_tbl_checksums_init[NUM_WEAPONS_TBL_CHECKSUMS] = {
	//	399297860,				// 1.0 FULL	
	-553984927				// 1.2 FULL (german)
	};
	#endif

	std::copy(Game_weapons_tbl_checksums_init,Game_weapons_tbl_checksums_init+NUM_WEAPONS_TBL_CHECKSUMS,Game_weapons_tbl_checksums);

	// if the game is running using hacked data
	Hacked_data_check_ready = false;
	Hacked_data = false;
}

