/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/

/*
* $Logfile: /Freespace2/code/Inetfile/Chttpget.h $
* $Revision: 2.2.2.1 $
* $Date: 2007-10-15 06:43:14 $
* $Author: taylor $
*
* HTTP Client class (get only)
*
* $Log: not supported by cvs2svn $
* Revision 2.2  2005/07/13 03:15:50  Goober5000
* remove PreProcDefine #includes in FS2
* --Goober5000
*
* Revision 2.1  2004/08/11 05:06:25  Kazan
* added preprocdefines.h to prevent what happened with fred -- make sure to make all fred2 headers include this file as the _first_ include -- i have already modified fs2 files to do this
*
* Revision 2.0  2002/06/03 04:02:24  penguin
* Warpcore CVS sync
*
* Revision 1.1  2002/05/02 18:03:08  mharris
* Initial checkin - converted filenames and includes to lower case
*
 * 
 * 3     8/22/99 1:19p Dave
 * Fixed up http proxy code. Cleaned up scoring code. Reverse the order in
 * which d3d cards are detected.
 * 
 * 5     8/20/99 3:01p Kevin
 * Added support for Proxies (I hope!)
 * 
 * 4     7/31/98 12:19p Nate
 * Fixed http abort problem.
 * 
 * 3     7/31/98 11:57a Kevin
 * Added new functions for getting state
 * 
 * 2     6/01/98 10:10a Kevin
 * Added DLL connection interface and auto update DLL
 * 
 * 1     5/27/98 9:54a Kevin
 * 
 * 1     5/25/98 5:31p Kevin
 * Initial version
*
* $NoKeywords: $
*/

#ifndef _CHTTPGET_HEADER_
#define _CHTTPGET_HEADER_

#include "globalincs/pstypes.h"

#define HTTP_STATE_INTERNAL_ERROR		0
#define HTTP_STATE_SOCKET_ERROR			1
#define HTTP_STATE_URL_PARSING_ERROR	2
#define HTTP_STATE_CONNECTING				3
#define HTTP_STATE_HOST_NOT_FOUND		4
#define HTTP_STATE_CANT_CONNECT			5
#define HTTP_STATE_CONNECTED				6
#define HTTP_STATE_FILE_NOT_FOUND		10
#define HTTP_STATE_RECEIVING				11
#define HTTP_STATE_FILE_RECEIVED			12
#define HTTP_STATE_UNKNOWN_ERROR			13
#define HTTP_STATE_RECV_FAILED			14
#define HTTP_STATE_CANT_WRITE_FILE		15
#define HTTP_STATE_STARTUP					16

#define MAX_URL_LEN	300

class ChttpGet
{
public:
	ChttpGet(char *URL,char *localfile);
	ChttpGet(char *URL,char *localfile,char *proxyip,unsigned short proxyport);
	~ChttpGet();
	void GetFile(char *URL,char *localfile);
	int GetStatus();
	uint GetBytesIn();
	uint GetTotalBytes();
	void AbortGet();
	void WorkerThread();
	bool m_Aborted;

protected:
	int ConnectSocket();
	char *GetHTTPLine();
	uint ReadDataChannel();
	uint m_iBytesIn;
	uint m_iBytesTotal;
	uint m_State;
	bool m_ProxyEnabled;
	char *m_ProxyIP;
	char m_URL[MAX_URL_LEN];
	ushort m_ProxyPort;

	char m_szUserName[100];
	char m_szPassword[100];
	char m_szHost[200];
	char m_szDir[200];
	char m_szFilename[100];
	
	bool m_Aborting;


	SOCKET m_DataSock;
	
	FILE *LOCALFILE;
	char recv_buffer[1000];

#ifdef SCP_UNIX
	SDL_Thread *thread_id;
#endif
};










#endif
