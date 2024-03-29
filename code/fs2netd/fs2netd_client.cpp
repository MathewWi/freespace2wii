/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/

/*
 * $Logfile: /freespace2/code/fs2netd/fs2netd_client.cpp $
 * $Revision: 1.1.2.3 $
 * $Date: 2007-11-22 05:04:07 $
 * $Author: taylor $
 *
 * FS2NetD client handler
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1.2.2  2007/10/15 08:18:33  taylor
 * Oops, forgot that the v.2 daemon is still on the secondary IP :)
 *
 * Revision 1.1.2.1  2007/10/15 06:43:09  taylor
 * FS2NetD v.2  (still a work in progress, but is ~98% complete)
 *
 *
 * $NoKeywords: $
 */

#include "globalincs/pstypes.h"
#include "fs2netd/fs2netd_client.h"
#include "fs2netd/tcp_client.h"
#include "cfile/cfile.h"
#include "network/multi_log.h"
#include "osapi/osregistry.h"
#include "popup/popup.h"
#include "gamesnd/gamesnd.h"
#include "network/multi.h"
#include "playerman/player.h"
#include "io/timer.h"
#include "network/multiutil.h"
#include "network/multiui.h"
#include "network/stand_gui.h"
#include "network/multi_pxo.h"
#include "bmpman/bmpman.h"
#include "graphics/2d.h"
#include "graphics/font.h"
#include "globalincs/alphacolors.h"
#include "network/multi_options.h"
#include "cmdline/cmdline.h"
#include "cfile/cfilesystem.h"
#include "network/multimsgs.h"

#ifndef WIN32
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <cerrno>
#endif

#include <string>
#include <vector>
#include <limits.h>


#define FS2NETD_DEFAULT_PORT			"12009"
#define FS2NETD_DEFAULT_SERVER			"fs2netd.game-warden.com"
#define FS2NETD_DEFAULT_CHAT_SERVER		"fs2netd.game-warden.com"
#define FS2NETD_DEFAULT_BANNER_URL		"http://fs2netd.game-warden.com/files/banners"

#include "hud/hudmessage.h"
#include "freespace2/freespace.h"
#include "network/multiui.h"

extern int Multi_debrief_stats_accept_code;


static bool PXO_options_loaded = false;

static bool Is_connected = false;
static bool In_process = false;
static bool Logged_in = false;
static bool Duplicate_login_detected = false;

static bool do_full_packet = true;

static int Local_timeout = -1;
static int Next_gameserver_update = -1;
static int Last_pong = -1;
static int Login_retry_time = -1;

static std::vector<file_record> FS2NetD_file_list;
static std::vector<std::string> FS2NetD_ban_list;

std::vector<crc_valid_status> Table_valid_status;

// channel to associate when creating a server
char Multi_fs_tracker_channel[MAX_PATH] = "";

// channel to use when polling the tracker for games
char Multi_fs_tracker_filter[MAX_PATH] = "";

tracker_game_data Multi_tracker_game_data;


static void fs2netd_reset_state()
{
	Multi_tracker_id = -1;
	Is_connected = false;
	In_process = false;
	Logged_in = false;
	do_full_packet = true;
	Local_timeout = -1;
	Next_gameserver_update = -1;
	Last_pong = -1;
	Duplicate_login_detected = false;
	Login_retry_time = -1;
}

void fs2netd_options_config_init()
{
	if (PXO_options_loaded) {
		return;
	}

	if ( !strlen(Multi_options_g.game_tracker_ip) ) {
		ml_printf("NOTICE: Address for game tracker not specified, using default instead (%s).", FS2NETD_DEFAULT_SERVER);
		strncpy( Multi_options_g.game_tracker_ip, FS2NETD_DEFAULT_SERVER, MULTI_OPTIONS_STRING_LEN );
	} else if ( !strcmp("gt.pxo.net", Multi_options_g.game_tracker_ip) ) {
		ml_printf("NOTICE: Incompatible game tracker IP detected (gt.pxo.net), using default instead (%s)!", FS2NETD_DEFAULT_SERVER);
		strncpy( Multi_options_g.game_tracker_ip, FS2NETD_DEFAULT_SERVER, MULTI_OPTIONS_STRING_LEN );
	}

	if ( !strlen(Multi_options_g.user_tracker_ip) ) {
		ml_printf("NOTICE: Address for user tracker not specified, using default instead (%s).", FS2NETD_DEFAULT_SERVER);
		strncpy( Multi_options_g.user_tracker_ip, FS2NETD_DEFAULT_SERVER, MULTI_OPTIONS_STRING_LEN );
	} else if ( !strcmp("ut.pxo.net", Multi_options_g.user_tracker_ip) ) {
		ml_printf("NOTICE: Incompatible user tracker IP detected (ut.pxo.net), using default instead (%s)!", FS2NETD_DEFAULT_SERVER);
		strncpy( Multi_options_g.user_tracker_ip, FS2NETD_DEFAULT_SERVER, MULTI_OPTIONS_STRING_LEN );
	}

	if ( !strlen(Multi_options_g.tracker_port) ) {
		ml_printf("NOTICE: Port for game/user trackers not specified, using default instead (%s).", FS2NETD_DEFAULT_PORT);
		strncpy( Multi_options_g.tracker_port, FS2NETD_DEFAULT_PORT, STD_NAME_LEN );
	} else {
		long port_tmp = strtol(Multi_options_g.tracker_port, (char**)NULL, 10);

		if ( (port_tmp < 1024) || (port_tmp > USHRT_MAX) ) {
			ml_printf("NOTICE: The port specified for game/user trackers, '%i', is outside of the required range, %i through %i!", port_tmp, 1024, USHRT_MAX);
			ml_printf("NOTICE: Port for game/user trackers is invalid, using default instead (%s).", FS2NETD_DEFAULT_PORT);
			strncpy( Multi_options_g.tracker_port, FS2NETD_DEFAULT_PORT, STD_NAME_LEN );
		}
	}

	if ( !strlen(Multi_options_g.pxo_ip) ) {
		ml_printf("NOTICE: Address for chat server not specified, using default instead (%s).", FS2NETD_DEFAULT_CHAT_SERVER);
		strncpy( Multi_options_g.pxo_ip, FS2NETD_DEFAULT_CHAT_SERVER, MULTI_OPTIONS_STRING_LEN );
	} else if ( !strcmp("chat.pxo.net", Multi_options_g.pxo_ip) ) {
		ml_printf("NOTICE: Incompatible chat server IP detected (chat.pxo.net), using default instead (%s)!", FS2NETD_DEFAULT_CHAT_SERVER);
		strncpy( Multi_options_g.pxo_ip, FS2NETD_DEFAULT_CHAT_SERVER, MULTI_OPTIONS_STRING_LEN );
	}

	if ( !strlen(Multi_options_g.pxo_banner_url) ) {
		ml_printf("NOTICE: URL for banners not specified, using default instead (%s).", FS2NETD_DEFAULT_BANNER_URL);
		strncpy( Multi_options_g.pxo_banner_url, FS2NETD_DEFAULT_BANNER_URL, MULTI_OPTIONS_STRING_LEN );
	} else if ( !strcmp("http://www.pxo.net/files/banners", Multi_options_g.pxo_banner_url) ) {
		ml_printf("NOTICE: Incompatible banner URL detected (chat.pxo.net), using default instead (%s)!", FS2NETD_DEFAULT_BANNER_URL);
		strncpy( Multi_options_g.pxo_banner_url, FS2NETD_DEFAULT_BANNER_URL, MULTI_OPTIONS_STRING_LEN );
	}

	PXO_options_loaded = true;
}

bool fs2netd_is_online()
{
	return (Is_connected && Logged_in);
}

void fs2netd_reset_connection()
{
	bool reset_gameserver = false;

	if (Net_player->flags & NETINFO_FLAG_MT_CONNECTED) {
		fs2netd_gameserver_disconnect();
		reset_gameserver = true;
	}

	FS2NetD_Disconnect();

	// wait a little to allow for the port to clear
	Sleep(250);

	fs2netd_reset_state();

	// try to reinit the server connection
	fs2netd_login();

	if (reset_gameserver) {
		fs2netd_gameserver_start();
	}
}

static int fs2netd_connect_do()
{
	int retval = FS2NetD_ConnectToServer(Multi_options_g.game_tracker_ip, Multi_options_g.tracker_port);

	Sleep(5);

	switch (retval) {
		// connection failed
		case -1:
			Is_connected = false;
			return 2;

		// still trying to connect
		case 0:
			return 0;

		// connected!
		case 1:
			Is_connected = true;
			return 1;
	}

	return 0;
}

void fs2netd_connect()
{
	int rc = 0;

	// don't bother with this if we aren't on FS2NetD
	if ( !Om_tracker_flag ) {
		return;
	}

	if ( !(Game_mode & GM_MULTIPLAYER) ) {
		return;
	}

	if (Is_connected) {
		return;
	}

	Assert( strlen(Multi_options_g.game_tracker_ip) );
	Assert( strlen(Multi_options_g.tracker_port) );

	In_process = true;

	if (Is_standalone) {
		do { rc = fs2netd_connect_do(); } while (!rc);
	} else {
		popup_till_condition(fs2netd_connect_do, XSTR("&Cancel", 779), XSTR("Connecting into FS2NetD", -1));
	}

	In_process = false;
	Local_timeout = -1;
}

int fs2netd_login_do()
{
	if (Multi_tracker_id < 0) {
		if ( Is_standalone && std_gen_is_active() ) {
			std_gen_set_text("Verifying username and password", 1);
		} else {
			popup_change_text( XSTR("Verifying username and password", -1) );
		}

		memset(Multi_tracker_id_string, 0, sizeof(Multi_tracker_id_string));

		if (Local_timeout == -1) {
			Local_timeout = timer_get_seconds() + 15;
		}

		// if timeout passes then bail on SID failure
		if ( timer_get_seconds() > Local_timeout ) {
			ml_string("FS2NetD MSG: Login failure due to timeout!");
			Local_timeout = -1;
			return 2;
		}

		const char *user = Multi_tracker_login;
		const char *passwd = Multi_tracker_passwd;

		if (Is_standalone) {
			if ( strlen(Multi_options_g.std_pxo_login) ) {
				user = Multi_options_g.std_pxo_login;
			}

			if ( strlen(Multi_options_g.std_pxo_password) ) {
				passwd = Multi_options_g.std_pxo_password;
			}
		}

		Multi_tracker_id = FS2NetD_Login(user, passwd, do_full_packet);

		// if we have already been through once then only deal with the recieve packet next time
		do_full_packet = false;

		// invalid login
		if (Multi_tracker_id == -2) {
			Multi_tracker_id = -1;
			Local_timeout = -1;
			return 1;
		}

		if (Multi_tracker_id >= 0) {
			ml_printf("FS2NetD MSG: Login '%s' is valid, session ID is %d!", user, Multi_tracker_id);
			do_full_packet = true;
			Local_timeout = -1;
		}
	} else {
		if ( Is_standalone && std_gen_is_active() ) {
			std_gen_set_text("Getting pilot stats", 1);
		} else {
			popup_change_text( XSTR("Getting pilot stats", -1) );
		}

		if (Local_timeout == -1) {
			Local_timeout = timer_get_seconds() + 30;
		}

		// if timeout passes then bail on stats failure
		if ( timer_get_seconds() > Local_timeout ) {
		
			Local_timeout = -1;
			return 2;
		}

		int rescode = FS2NetD_GetPlayerData(Players[Player_num].callsign, &Players[Player_num], true, do_full_packet);

		do_full_packet = false;

		if ( rescode != -1 ) {
			Local_timeout = -1;
			return (rescode + 3);
		}
	}

	return 0;
}

bool fs2netd_login()
{
	bool retval = true;
	int rc;

	// don't bother with this if we aren't on FS2NetD
	if ( !Om_tracker_flag ) {
		return false;
	}

	if ( !(Game_mode & GM_MULTIPLAYER) ) {
		return false;
	}

	if ( Logged_in && (Multi_tracker_id >= 0) ) {
		return true;
	}

	Logged_in = false;

	Multi_tracker_id = -1;
	memset( Multi_tracker_id_string, 0, sizeof(Multi_tracker_id_string) );

	// verify that our connection settings are sane
	fs2netd_options_config_init();

	// if we're a standalone, show a dialog saying "validating tables"
	if (Is_standalone) {
		std_create_gen_dialog("Logging into FS2NetD");
		std_gen_set_text("Connecting...", 1);
	}

	fs2netd_connect();

	if ( !Is_connected ) {
		if ( !Is_standalone ) {
			popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("Failed to connect to FS2NetD server!", -1));
		} else {
			std_gen_set_text("Connect FAILED!", 1);
			Sleep(2000);
			std_destroy_gen_dialog();
		}

		return false;
	}

	char error_str[256];
	char std_error_str[64];

	do_full_packet = true;

	In_process = true;

	if (Is_standalone) {
		do { rc = fs2netd_login_do(); } while (!rc);
	} else {
		rc = popup_till_condition(fs2netd_login_do, XSTR("&Cancel", 779), XSTR("Logging into FS2NetD", -1));
	}

	In_process = false;
	Local_timeout = -1;

	memset( error_str, 0, sizeof(error_str) );
	memset( std_error_str, 0, sizeof(std_error_str) );

	switch (rc) {
		// the action was cancelled
		case 0:
			ml_string("FS2NetD MSG: Login process canceled by user.");
			retval = false;
			break;

		// didn't get a session id
		case 1: {
			const char *user = Multi_tracker_login;
			const char *passwd = Multi_tracker_passwd;

			if (Is_standalone) {
				if ( strlen(Multi_options_g.std_pxo_login) ) {
					user = Multi_options_g.std_pxo_login;
				}

				if ( strlen(Multi_options_g.std_pxo_password) ) {
					passwd = Multi_options_g.std_pxo_password;
				}
			}

			ml_printf("FS2NetD ERROR: Login %s/%s is invalid!", user, passwd);
			strcpy(error_str, "Login failed!");
			strcpy(std_error_str, "Login failed!");
			retval = false;
			break;
		}

		// unknown failure fetching pilot data
		case 2:
			ml_string("FS2NetD ERROR: UNKNOWN ERROR when fetching pilot data");
			strcpy(error_str, "An Unknown Error (probably a timeout) occured when trying to retrieve your pilot data.");
			strcpy(std_error_str, "Unknown Error (timeout?)");
			retval = false;
			break;

		// success!!
		case 3:
			ml_string("FS2NetD MSG: Got Pilot data");
			retval = true;
			break;

		// success!!  pilot was created
		case 4:
			ml_string("FS2NetD MSG: Created New Pilot");
			strcpy(error_str, "New Pilot has been created.");
			strcpy(std_error_str, "New Pilot has been created.");
			retval = true;
			break;

		// invalid pilot name
		case 5:
			ml_string("FS2NetD ERROR: Invalid Pilot!");
			strcpy(error_str, "Invalid pilot name - A serious error has occured, Contact the FS2NetD Administrator!");
			strcpy(std_error_str, "Invalid pilot name!");
			retval = false;
			break;

		// the session id was invalid
		case 6:
			ml_string("FS2NetD ERROR: Invalid SID!");
			strcpy(error_str, "Invalid SID - A serious error has occured, Contact the FS2NetD Administrator!");
			strcpy(std_error_str, "Invalid SID");
			retval = false;
			break;

		default:
			ml_string("FS2NetD ERROR: Unknown return case for GetPlayerData()");
			strcpy(error_str, "Unkown return case from GetPlayerData(). Contact the FS2NetD Administrator!");
			retval = false;
			break;
	}

	if ( !Is_standalone && strlen(error_str) ) {
		popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, error_str);
	} else if ( Is_standalone && std_gen_is_active() && strlen(std_error_str) ) {
		std_gen_set_text(std_error_str, 1);
		Sleep(2000);
	}

	if (retval) {
		Logged_in = true;
		sprintf(Multi_tracker_id_string, "%d", Multi_tracker_id);
	}

	if (Is_standalone) {
		std_destroy_gen_dialog();
	}

	return retval;
}

static void fs2netd_handle_ping()
{
	static int Next_ping = -1;
	bool reset = false;

	// if we didn't get a PONG within 4 minutes the server connection must have dropped
	if ( (Last_pong != -1) && ((Next_ping - Last_pong) > 240) ) {
		ml_string("FS2NetD WARNING: Lost connection to server!");
		fs2netd_reset_connection();

		// make sure that we are good to go
		if ( !Is_connected ) {
			if (!Is_standalone) {
				gamesnd_play_iface(SND_GENERAL_FAIL);
				popup(PF_USE_AFFIRMATIVE_ICON | PF_TITLE_BIG | PF_TITLE_RED, 1, POPUP_OK, "ERROR:\nLost connection to the FS2NetD server!");
			}

			ml_string("FS2NetD ERROR: Lost connection to the FS2NetD server!");
			FS2NetD_Disconnect();
			fs2netd_reset_state();

			ml_string("FS2NetD MSG: Will attempt an automatic reconnect to server in 30 minutes...");
			Login_retry_time = timer_get_seconds() + 1800;

			return;
		} else {
			ml_string("FS2NetD NOTICE: Connection to server has been reestablished!");
		}
	}

	// send out ping every 90 seconds
	if ( (Next_ping == -1) || (timer_get_seconds() >= Next_ping) ) {
		// if we have seen a long period of time between pings then reset the pong time too
		if ( (timer_get_seconds() - Next_ping) > 120 ) {
			reset = true;
		}

		Next_ping = timer_get_seconds() + 90;

		// we go ahead and set the initial Last_pong here, even though we haven't gotten a pong yet
		if ( (Last_pong == -1) || reset ) {
			Last_pong = Next_ping;
			reset = false;
		}

		FS2NetD_Ping();

		// also send out a ID check to keep our login verified
		if ( FS2NetD_CheckValidID() < 0 ) {
			ml_string("FS2NetD WARNING: Unable to validate login!");
			fs2netd_reset_connection();

			// make sure that we are good to go
			if ( !Is_connected ) {
				if (!Is_standalone) {
					gamesnd_play_iface(SND_GENERAL_FAIL);
					popup(PF_USE_AFFIRMATIVE_ICON | PF_TITLE_BIG | PF_TITLE_RED, 1, POPUP_OK, "ERROR:\nLost connection to the FS2NetD server!");
				}

				ml_string("FS2NetD ERROR: Lost connection to the FS2NetD server!");
				FS2NetD_Disconnect();
				fs2netd_reset_state();

				ml_string("FS2NetD MSG: Will attempt an automatic reconnect to server in 30 minutes...");
				Login_retry_time = timer_get_seconds() + 1800;

				return;
			} else {
				ml_string("FS2NetD NOTICE: Connection to server has been reestablished!");
			}
		}

		// verify that we are only logged in once (for stats saving purposes)
		if ( (Netgame.game_state == NETGAME_STATE_BRIEFING) || (Netgame.game_state == NETGAME_STATE_MISSION_SYNC) ) {
			FS2NetD_CheckDuplicateLogin();
		}

		ml_string("FS2NetD sent PING/IDENT");
	}
}

static void fs2netd_handle_messages()
{
	int buffer_size = 0, buffer_offset = 0;
	int bytes_read = 0;
	char tbuf[256];
	char buffer[8192];
	ubyte pid = 0;
	int itemp;

	while ( FS2NetD_DataReady() && (bytes_read < (int)sizeof(buffer)) ) {
		int read_size = FS2NetD_GetData(buffer+bytes_read, sizeof(buffer)-bytes_read);

		if (read_size <= 0) {
			break;
		}

		bytes_read += read_size;

		Sleep(20);
	}

	if ( (bytes_read == 0) || (bytes_read < BASE_PACKET_SIZE) ) {
		return;
	}

	buffer_offset = 0;

	while (buffer_offset+BASE_PACKET_SIZE <= bytes_read) {
		PXO_GET_DATA( pid );
		PXO_GET_INT( buffer_size );

		// packet has more data than our buffer received
		if (buffer_offset+buffer_size-BASE_PACKET_SIZE > bytes_read) {
			break;
		}

		// processing time!
		switch (pid) {
			case PCKT_PING: {
				PXO_GET_INT( itemp );

			//	ml_printf("FS2NetD received PING");

				FS2NetD_Pong(itemp);

				break;
			}

			case PCKT_PONG: {
				PXO_GET_INT( itemp );

				ml_printf("FS2NetD PONG: %d ms", timer_get_milliseconds() - itemp);

				Last_pong = timer_get_seconds();

				break;
			}

			case PCKT_NETOWRK_WALL: {
				PXO_GET_STRING( tbuf );
				ml_printf("FS2NetD WALL received MSG: %s", tbuf);

				switch (Netgame.game_state) {
					case NETGAME_STATE_FORMING:
					case NETGAME_STATE_BRIEFING:
					case NETGAME_STATE_MISSION_SYNC:
					case NETGAME_STATE_DEBRIEF:
						multi_display_chat_msg(tbuf, 0, 0);
						break;

					case NETGAME_STATE_IN_MISSION: // gotta make it paused
						//multi_pause_request(1); 
						//send_game_chat_packet(Net_player, str, MULTI_MSG_ALL, NULL);
						HUD_printf(tbuf);
						break;

					default:
						// do-nothing
						break;
				}

				break;
			}

			case PCKT_CHAT_CHAN_COUNT_REPLY: {
				PXO_GET_STRING( tbuf );
				PXO_GET_INT( itemp );

				if ( (itemp < 0) || (itemp > USHRT_MAX) ) {
					itemp = 0;
				}

				multi_pxo_channel_count_update(tbuf, itemp);

				break;
			}

			case PCKT_VALID_SID_REPLY: {
				ubyte login_status = 0;

				PXO_GET_DATA( login_status );

				if (login_status != 1) {
					ml_printf("FS2NetD IDENT: Got invalid login check!");
					fs2netd_reset_connection();
				}

				break;
			}

			case PCKT_DUP_LOGIN_REPLY: {
				ubyte dupe_status = 0;

				PXO_GET_DATA( dupe_status );

				Duplicate_login_detected = (dupe_status != 0);

				break;
			}

			case PCKT_SLIST_REPLY: {
				int numServers = 0;
				int svr_flags;
				ushort svr_port;
				char svr_ip[16];
				active_game ag;

				PXO_GET_USHORT( numServers );

				if (numServers == 0) {
					break;
				}

				for (int i = 0; i < numServers; i++) {
					PXO_GET_INT( svr_flags );
					PXO_GET_USHORT( svr_port );
					PXO_GET_STRING( svr_ip );

					if ( !psnet_is_valid_ip_string(svr_ip) ) {
						ml_printf("FS2NetD SLIST: Invalid ip string (%s)!", svr_ip);
					} else {
						memset( &ag, 0, sizeof(active_game) );

						ag.server_addr.type = NET_TCP;
						ag.server_addr.port = (short) svr_port;

						if (ag.server_addr.port <= 0) {
							ag.server_addr.port = DEFAULT_GAME_PORT;
						}

						psnet_string_to_addr(&ag.server_addr, svr_ip);

						// query this server
						send_server_query(&ag.server_addr);
					}
				}

				break;
			}

			default: {
				break;
			}
		}
	}
}

void fs2netd_do_frame()
{
	// in a previous processing loop, so don't do a frame until that has completed
	if ( In_process ) {
		return;
	}

	if ( !Logged_in ) {
		// maybe try and reconnect, if we were bumped due to a comm error ...
		if ( (Login_retry_time != -1) && (timer_get_seconds() >= Login_retry_time) ) {
			fs2netd_login();

			if ( !Logged_in ) {
				// bah!  try again in another 30 minutes
				Login_retry_time = timer_get_seconds() + 1800;
			} else {
				Login_retry_time = -1;
			}
		}

		return;
	}

	// do ping/pong and ident
	fs2netd_handle_ping();

	// handle gameserver updates
	fs2netd_gameserver_update();

	// check for server messages, ping replies, etc.
	fs2netd_handle_messages();

	// WTF?!  (TODO: figure out how this happens)
	if (Is_connected && !Logged_in) {
		fs2netd_login();
	}
}

void fs2netd_gameserver_start()
{
	if ( !Logged_in ) {
		return;
	}

	// already been here
	if (Net_player->flags & NETINFO_FLAG_MT_CONNECTED) {
		return;
	}

	memset(&Multi_tracker_game_data, 0, sizeof(tracker_game_data));

	strcpy(Multi_tracker_game_data.name, Netgame.name);
	strcpy(Multi_tracker_game_data.mission_name, Netgame.mission_name);
	strcpy(Multi_tracker_game_data.title, Netgame.title);
	strcpy(Multi_tracker_game_data.campaign_name, Netgame.campaign_name);

	if ( strlen(Multi_fs_tracker_channel) ) {
		strcpy(Multi_tracker_game_data.chat_channel, Multi_fs_tracker_channel);
	}

	Multi_tracker_game_data.campaign_mode = (ubyte)Netgame.campaign_mode;
	Multi_tracker_game_data.flags = Netgame.flags;
	Multi_tracker_game_data.type_flags = Netgame.type_flags;
	Multi_tracker_game_data.players = (short)multi_num_players();
	Multi_tracker_game_data.max_players = Netgame.max_players;
	Multi_tracker_game_data.mode = (ubyte)Netgame.mode;
	Multi_tracker_game_data.rank_base = (ubyte)Netgame.rank_base;
	Multi_tracker_game_data.game_state = (ubyte)Netgame.game_state;
	Multi_tracker_game_data.speed = (ubyte)multi_get_connection_speed();

	FS2NetD_SendServerStart();

	Net_player->flags |= NETINFO_FLAG_MT_CONNECTED;

	// initial update should be about 2 seconds from now
	Next_gameserver_update = timer_get_seconds() + 2;

	ml_string("FS2NetD sent game server start");
}

void fs2netd_gameserver_update(bool force)
{
	if ( !Logged_in ) {
		return;
	}

	// server hasn't started yet?
	if ( !(Net_player->flags & NETINFO_FLAG_MT_CONNECTED) ) {
		return;
	}

	// is it actually time for an update
	if ( !force && (timer_get_seconds() < Next_gameserver_update) ) {
		return;
	}

	strcpy(Multi_tracker_game_data.mission_name, Netgame.mission_name);
	strcpy(Multi_tracker_game_data.title, Netgame.title);
	strcpy(Multi_tracker_game_data.campaign_name, Netgame.campaign_name);

	Multi_tracker_game_data.campaign_mode = (ubyte)Netgame.campaign_mode;
	Multi_tracker_game_data.players = (short)multi_num_players();
	Multi_tracker_game_data.game_state = (ubyte)Netgame.game_state;

	FS2NetD_SendServerUpdate();

	// set timeout for every 2 minutes
	Next_gameserver_update = timer_get_seconds() + 120;

	ml_string("FS2NetD sent game server update");
}

void fs2netd_gameserver_disconnect()
{
	if ( !Logged_in ) {
		return;
	}

	// server hasn't started yet?
	if ( !(Net_player->flags & NETINFO_FLAG_MT_CONNECTED) ) {
		return;
	}

	FS2NetD_SendServerDisconnect();

	Net_player->flags &= ~NETINFO_FLAG_MT_CONNECTED;

	ml_string("FS2NetD sent game server disconnect");
}

void fs2netd_send_game_request()
{
	if ( !Logged_in ) {
		return;
	}

	FS2NetD_RequestServerList();
}

static char Chk_mission_name[NAME_LENGTH+1];
static uint Chk_mission_crc = 0;

int fs2netd_check_mission_do()
{
	if (Local_timeout == -1) {
		Local_timeout = timer_get_seconds() + 15;
	}

	// if timeout passes then bail on stats failure
	if ( timer_get_seconds() > Local_timeout ) {
		Local_timeout = -1;
		return 4;
	}

	int rescode = FS2NetD_CheckSingleMission(Chk_mission_name, Chk_mission_crc, do_full_packet);

	do_full_packet = false;

	if (rescode) {
		Local_timeout = -1;
		return rescode;
	}

	return 0;
}

bool fs2netd_check_mission(char *mission_name)
{
	int rc = 0;
	char popup_string[256];

	if ( !Logged_in ) {
		return 0;
	}

	strcpy(Chk_mission_name, mission_name);
	cf_chksum_long(Chk_mission_name, &Chk_mission_crc);

	do_full_packet = true;

	In_process = true;

	memset(popup_string, 0, sizeof(popup_string));
	sprintf(popup_string, XSTR("Validating mission %s", 1074), mission_name);

	if (Is_standalone) {
		do { rc = fs2netd_check_mission_do(); } while (!rc);
	} else {
		rc = popup_till_condition(fs2netd_check_mission_do, XSTR("&Cancel", 779), popup_string);
	}

	In_process = false;
	Local_timeout = -1;

	switch (rc) {
		// operation canceled, or invalid
		case 0:
			return false;

		// successful, but invalid
		case 1:
			return false;

		// successful and valid
		case 2:
			return true;

		// failed to send request packet
		case 3:
			if ( !Is_standalone ) {
				popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("Server request failed!", -1));
			}

			return false;

		// it timed out
		case 4:
			if ( !Is_standalone ) {
				popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("Server request timed out!", -1));
			}

			return false;
	}

	return false;
}

static int fs2netd_send_player_do()
{
	if (Local_timeout == -1) {
		Local_timeout = timer_get_seconds() + 15;
	}

	// if timeout passes then bail on stats failure
	if ( timer_get_seconds() > Local_timeout ) {
		Local_timeout = -1;
		return 2;
	}

	int rescode = FS2NetD_SendPlayerData(Players[Player_num].callsign, &Players[Player_num], do_full_packet);

	do_full_packet = false;

	if (rescode != -1) {
		Local_timeout = -1;
		return rescode+3;
	}

	return 0;
}

static int fs2netd_send_player()
{
	int rc;

	do_full_packet = true;

	In_process = true;

	if (Is_standalone) {
		do { rc = fs2netd_send_player_do(); } while (!rc);
	} else {
		rc = popup_till_condition(fs2netd_send_player_do, XSTR("&Cancel", 779), XSTR("Sending player stats requests ...", 676));
	}

	In_process = false;
	Local_timeout = -1;

	rc = rc - 3;

	if (rc < -1) {
		rc = -1;
	}

	return rc;
}

static void fs2netd_store_stats_results()
{
	char str[512];

	memset(str, 0, sizeof(str));

	multi_display_chat_msg(XSTR("<PXO stats store process complete>", 1001), 0, 0);
	ml_string( XSTR("<PXO stats store process complete>", 1001) );

	if (Multi_debrief_stats_accept_code != 1) {
		sprintf(str, XSTR("<PXO stats store failed for player %s>", 1002), Net_player->m_player->callsign);
		multi_display_chat_msg(str, 0, 0);
		ml_string(str);
	}
}

void fs2netd_store_stats()
{
	if ( !Logged_in ) {
		return;
	}

	ml_string("Sending stats to server");

	// default to not saving the stats
	Multi_debrief_stats_accept_code = 0;

	if (Duplicate_login_detected) {
		Duplicate_login_detected = false;
		multi_display_chat_msg( XSTR("<Duplicate login detected - stats have been tossed>", -1), 0, 0 );
		ml_string( XSTR("<Duplicate login detected - stats have been tossed>", -1) );
		fs2netd_store_stats_results();
		return;
	}

	if ( game_hacked_data() ) {
		multi_display_chat_msg( XSTR("<Hacked tables detected - stats have been tossed>", -1), 0, 0 );
		popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("You are playing with a hacked tables, your stats will not be saved", -1) );
		fs2netd_store_stats_results();
		return;
	}

	if ( (multi_num_players() <= 1) && (Multi_num_players_at_start <= 1) ) {
		multi_display_chat_msg(XSTR("<Not enough players were present at game start or end, stats will not be saved>", 1048), 0, 0);
		ml_string( XSTR("<Not enough players were present at game start or end, stats will not be saved>", 1048) );
		fs2netd_store_stats_results();
		return;
	}

/*
	// if any players have hacked info
	for(int idx = 0; idx < MAX_PLAYERS; idx++) {
		if ( MULTI_CONNECTED(Net_players[idx]) && !MULTI_STANDALONE(Net_players[idx]) && (Net_players[idx].flags & NETINFO_FLAG_HAXOR) ) {
			multi_display_chat_msg( XSTR("<Connected player has hacked info - tossing invalid stats>", -1), 0, 0 );
			return;
		}
	}
*/
	if ( !fs2netd_check_mission(Netgame.mission_name) ) {
		multi_display_chat_msg(XSTR("<Server detected a non PXO validated mission. Stats will not be saved>", 1049), 0, 0);
		popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("This is not a PXO validated mission, your stats will not be saved", 1050));
		fs2netd_store_stats_results();
		return;
	}

	int spd_ret = fs2netd_send_player();

	switch (spd_ret) { // 0 = pilot updated, 1  = invalid pilot, 2 = invalid (expired?) sid
		case -1:
			ml_string("<stats have been tossed - server error>");
			break;

		case 0:
			ml_string( XSTR("<stats have been accepted>", 850) );
			Multi_debrief_stats_accept_code = 1;
			break;

		case 1:
			ml_string("<stats have been tossed - pilot error>");
			break;

		case 2:
			// we should never get here with the new code
			Int3();
			ml_string("<stats have been tossed - invalid tracker id>");
			break;

		default:
			multi_display_chat_msg( XSTR("Unknown Stats Store Request Reply", -1), 0, 0 );
			break;
	}

	fs2netd_store_stats_results();
}

int fs2netd_update_ban_list_do()
{
	if (Local_timeout == -1) {
		Local_timeout = timer_get_seconds() + 30;
	}

	// if timeout passes then bail on stats failure
	if ( timer_get_seconds() > Local_timeout ) {
		Local_timeout = -1;
		return 2;
	}

	int rc = FS2NetD_GetBanList(FS2NetD_ban_list, do_full_packet);

	do_full_packet = false;

	if (rc) {
		Local_timeout = -1;
		return 1;
	}

	return 0;
}

void fs2netd_update_ban_list()
{
	int rc = 0;

	if ( !Logged_in ) {
		return;
	}

	// destroy the file prior to updating
	cf_delete( "banlist.cfg", CF_TYPE_DATA );

	do_full_packet = true;

	In_process = true;

	if (Is_standalone) {
		do { rc = fs2netd_update_ban_list_do(); } while (!rc);
	} else {
		rc = popup_till_condition(fs2netd_update_ban_list_do, XSTR("&Cancel", 779), XSTR("Requesting IP ban list", -1));
	}

	In_process = false;
	Local_timeout = -1;

	if ( !FS2NetD_ban_list.empty() ) {
		CFILE *banlist_cfg = cfopen("banlist.cfg", "wt", CFILE_NORMAL, CF_TYPE_DATA);

		if (banlist_cfg != NULL) {
			for (uint i = 0; i < FS2NetD_ban_list.size(); i++) {
				cfputs( const_cast<char*>(FS2NetD_ban_list[i].c_str()), banlist_cfg );
			}

			cfclose(banlist_cfg);
		}
	}

	FS2NetD_ban_list.clear();
}

bool fs2netd_player_banned(net_addr *addr)
{
	if ( !Logged_in ) {
		return false;
	}

	char line[32]; // no line should be larger than 16, but let's be safe
	char ip_str[32];
	memset(ip_str, 0, 32);
	memset(line, 0, 32);

	bool retval = false;
	CFILE *banlist_cfg = cfopen("banlist.cfg", "rt", CFILE_NORMAL, CF_TYPE_DATA);

	if (banlist_cfg == NULL) {
		return false;
	}

	psnet_addr_to_string( ip_str, addr );

	while ( !cfeof(banlist_cfg) && !retval ) {
		cfgets(line, 32, banlist_cfg);

		if ( !strnicmp(ip_str, line, strlen(line)) ) {
			retval = true; // BANNINATED!!!
		}
	}

	cfclose(banlist_cfg);

	return retval;
}

int fs2netd_get_valid_missions_do()
{
	if (Local_timeout == -1) {
		Local_timeout = timer_get_seconds() + 30;
	}

	// get the available CRCs from the server if we need to
	if ( FS2NetD_file_list.empty() ) {
		int rc = FS2NetD_GetMissionsList(FS2NetD_file_list, do_full_packet);

		do_full_packet = false;

		// communications error
		if (rc < 0) {
			Local_timeout = -1;
			return 4;
		}

		// no missions
		if ( rc && FS2NetD_file_list.empty() ) {
			Local_timeout = -1;
			return 2;
		}

		// if timeout passes then bail on crc failure
		if ( timer_get_seconds() > Local_timeout ) {
			Local_timeout = -1;
			return 1;
		}
	}
	// we should have the CRCs, or there were no missions, so process them
	else {
		static char **file_names = NULL;
		static int idx = 0, count = 0;

		bool found = false;
		int file_index = 0;
		char valid_status = MVALID_STATUS_UNKNOWN;
		char full_name[MAX_FILENAME_LEN], wild_card[10];
		char val_text[MAX_FILENAME_LEN+15];
		int i;
		uint checksum = 0;

		if (file_names == NULL) {
			// allocate filename space	
			file_names = (char**) vm_malloc_q( sizeof(char*) * 1024 ); // 1024 files should be safe!

			if (file_names == NULL) {
				Local_timeout = -1;
				return 3;
			}

			memset( wild_card, 0, sizeof(wild_card) );
			strcpy( wild_card, NOX("*") );
			strcat( wild_card, FS_MISSION_FILE_EXT );

			idx = count = cf_get_file_list(1024, file_names, CF_TYPE_MISSIONS, wild_card);
		}

		// drop idx first thing
		idx--;

		// we should be done validating, or just not have nothing to validate
		if (idx < 0) {
			for (idx = 0; idx < count; idx++) {
				if (file_names[idx] != NULL) {
					vm_free(file_names[idx]);
					file_names[idx] = NULL;
				}
			}

			vm_free(file_names);
			file_names = NULL;

			idx = count = 0;

			Local_timeout = -1;
			return 4;
		}


		// verify all filenames that we know about with their CRCs
		// NOTE: that this is done for one file per frame, since this is inside of a popup
		memset( full_name, 0, MAX_FILENAME_LEN );
		strncpy( full_name, cf_add_ext(file_names[idx], FS_MISSION_FILE_EXT), sizeof(full_name) - 1 );

		memset( val_text, 0, sizeof(val_text) );
		snprintf( val_text, sizeof(val_text) - 1, "Validating:  %s", full_name );

		if (Is_standalone) {
			if ( std_gen_is_active() ) {
				std_gen_set_text(val_text, 1);
			}
		} else {
			popup_change_text(val_text);
		}

		cf_chksum_long(full_name, &checksum);

		// try and find the file
		file_index = multi_create_lookup_mission(full_name);

		found = false;

		if (file_index >= 0) {
			for (i = 0; (i < (int)FS2NetD_file_list.size()) && (!found); i++) {
				if ( !stricmp(full_name, FS2NetD_file_list[i].name) ) {
					if (FS2NetD_file_list[i].crc32 == checksum) {
						found = true;
						valid_status = MVALID_STATUS_VALID;
					} else {
						valid_status = MVALID_STATUS_INVALID;
					}

					Multi_create_mission_list[file_index].valid_status = valid_status;
				}
			}

			if (found) {
				ml_printf("FS2NetD Mission Validation: %s  =>  Valid!", full_name);
			} else {
				ml_printf("FS2NetD Mission Validation: %s  =>  INVALID! -- 0x%08x", full_name, checksum);
			}
		}
	}

	return 0;
}

bool fs2netd_get_valid_missions()
{
	int rc = 0;

	if ( !Logged_in ) {
		return false;
	}

	FS2NetD_file_list.clear();

	do_full_packet = true;

	In_process = true;

	if (Is_standalone) {
		do { rc = fs2netd_get_valid_missions_do(); } while (!rc);
	} else {
		rc = popup_till_condition(fs2netd_get_valid_missions_do, XSTR("&Cancel", 779), XSTR("Starting mission validation", -1));
	}

	In_process = false;
	Local_timeout = -1;

	FS2NetD_file_list.clear();

	switch (rc) {
		// canceled by popup
		case 0:
			return false;

		// timed out
		case 1:
			if ( !Is_standalone ) {
				popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("Mission validation timed out!", -1));
			}

			return false;

		// no missions
		case 2:
			if ( !Is_standalone ) {
				popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("No missions are available from the server for validation!", -1));
			}

			return false;

		// out of memory
		case 3:
			if ( !Is_standalone ) {
				popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("Memory error during mission validation!", -1));
			}

			return false;
	}

	return true;
}

int fs2netd_update_valid_tables_do()
{
	if (Local_timeout == -1) {
		Local_timeout = timer_get_seconds() + 30;
	}

	int rc = FS2NetD_ValidateTableList(do_full_packet);

	do_full_packet = false;

	// if timeout passes then bail on crc failure
	if ( timer_get_seconds() > Local_timeout ) {
		Local_timeout = -1;
		return 1;
	}

	if ( rc == 0 ) {
		return 0;
	}

	switch (rc) {
		// some error occured, assume that there are no valid table crcs
		case -1:
			Local_timeout = -1;
			return 2;

		// timeout
		case 1:
			Local_timeout = -1;
			return 1;

		// done!
		case 2:
			Local_timeout = -1;
			return 3;
	}

	return 0;
}

int fs2netd_update_valid_tables()
{
	int rc;
	int hacked = 0;

	if ( !Logged_in ) {
		return -1;
	}

	// if there are no tables to check with then bail
	if ( Table_valid_status.empty() ) {
		return -1;
	}

	// if we're a standalone, show a dialog saying "validating tables"
	if (Game_mode & GM_STANDALONE_SERVER) {
		std_create_gen_dialog("Validating tables");
		std_gen_set_text("Querying FS2NetD:", 1);
	}

	do_full_packet = true;

	In_process = true;

	if (Is_standalone) {
		do { rc = fs2netd_update_valid_tables_do(); } while (!rc);
	} else {
		rc = popup_till_condition(fs2netd_update_valid_tables_do, XSTR("&Cancel", 779), XSTR("Starting table validation", -1));
	}

	In_process = false;
	Local_timeout = -1;

	switch (rc) {
		// canceled by popup
		case 0:
			return -1;

		// timed out
		case 1: {
			if ( !Is_standalone ) {
				popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("Table validation timed out!", -1));
			}

			return -1;
		}

		// no tables
		case 2: {
			if ( !Is_standalone ) {
				popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("No tables are available from the server for validation!", -1));
			}

			return -1;
		}
	}

	// output the status of table validity to multi.log
	for (uint i = 0; i < Table_valid_status.size(); i++) {
		if (Table_valid_status[i].valid) {
			ml_printf("FS2NetD Table Check: '%s' -- Valid!", Table_valid_status[i].name);
		} else {
			ml_printf("FS2NetD Table Check: '%s' -- INVALID (0x%x)!", Table_valid_status[i].name, Table_valid_status[i].crc32);
			hacked = 1;
		}
	}

	// if we're a standalone, kill the validate dialog
	if (Game_mode & GM_STANDALONE_SERVER) {
		std_destroy_gen_dialog();
	}

	return hacked;
}

void fs2netd_add_table_validation(char *tblname)
{
	uint chksum = 0;

	// if the tbl name isn't valid then just assume that the tbl is too
	if ( (tblname == NULL) || !strlen(tblname) ) {
		return;
	}

	CFILE *tbl = cfopen(tblname, "rt", CFILE_NORMAL, CF_TYPE_TABLES);

	if (tbl == NULL) {
		return;
	}

	cf_chksum_long(tbl, &chksum);

	cfclose(tbl);

	crc_valid_status tbl_crc;

	strncpy(tbl_crc.name, tblname, NAME_LENGTH);
	tbl_crc.crc32 = chksum;
	tbl_crc.valid = 0;

	Table_valid_status.push_back( tbl_crc );
}

int fs2netd_get_pilot_info(const char *callsign, player *out_plr, bool first_call)
{
	if ( !Logged_in ) {
		return -2;
	}

	if ( (out_plr == NULL) || (callsign == NULL) || !(strlen(callsign)) ) {
		return -2;
	}

	static player new_plr;

	if (first_call) {
		memset( &new_plr, 0, sizeof(player) );
		strncpy( new_plr.callsign, callsign, CALLSIGN_LEN );

		memset( out_plr, 0, sizeof(player) );

		Local_timeout = timer_get_seconds() + 30;

		In_process = true;
	}

	int rc = FS2NetD_GetPlayerData(callsign, &new_plr, false, first_call);

	// some sort of failure
	if (rc > 0) {
		In_process = false;
		Local_timeout = -1;
		return -2;
	}

	// if timeout passes then bail on failure
	if ( timer_get_seconds() > Local_timeout ) {
		In_process = false;
		Local_timeout = -1;
		return -2;
	}

	if (rc == 0) {
		memcpy( out_plr, &new_plr, sizeof(player) );
		In_process = false;
		Local_timeout = -1;
	}

	// we should only be returning -1 (processing) or 0 (got data successfully)
	return rc;
}

void fs2netd_close()
{
	// make sure that a hosted games is de-listed
	fs2netd_gameserver_disconnect();

	FS2NetD_Disconnect();

	fs2netd_reset_state();
	PXO_options_loaded = false;

	Table_valid_status.clear();

	FS2NetD_file_list.clear();
	FS2NetD_ban_list.clear();
}

void fs2netd_update_game_count(char *chan_name)
{
	if ( !Logged_in ) {
		return;
	}

	if ( (chan_name == NULL) || !strlen(chan_name) ) {
		return;
	}

	FS2NetD_GameCountUpdate(chan_name);
}

void fs2netd_spew_table_checksums(char *outfile)
{
	char full_name[MAX_PATH_LEN];
	int count, idx;
	FILE *out = NULL;
	char modname[128];
	time_t my_time = 0;

	if ( Table_valid_status.empty() ) {
		return;
	}

	cf_create_default_path_string(full_name, sizeof(full_name) - 1, CF_TYPE_ROOT, outfile);

	// open the outfile
	out = fopen(full_name, "wt");

	if (out == NULL) {
		return;
	}

	memset( modname, 0, sizeof(modname) );
	strcpy( modname, Cmdline_spew_table_crcs );

	my_time = time(NULL);
	
	fprintf(out, "--  Table CRCs generated on %s \n", ctime(&my_time));

	fprintf(out, "LOCK TABLES `fstables` WRITE;\n");
	fprintf(out, "INSERT INTO `fstables` VALUES ");

	count = (int)Table_valid_status.size();

	// do all the checksums
	for (idx = 0; idx < count; idx++) {
		if (idx == 0) {
			fprintf(out, "('%s',%u,'%s')", Table_valid_status[idx].name, Table_valid_status[idx].crc32, modname);
		} else {
			fprintf(out, ",('%s',%u,'%s')", Table_valid_status[idx].name, Table_valid_status[idx].crc32, modname);
		}
	}

	fprintf(out, ";\n");
	fprintf(out, "UNLOCK TABLES;\n");

	fclose(out);
}
