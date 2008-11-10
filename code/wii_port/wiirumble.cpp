#include "wiirumble.h"

#include <sys/time.h>
#include <wiiuse/wpad.h>
#include <math.h>

static bool rumbleinit = false;
static timeval request = (timeval) {0,0};

void doRumble()
{
	timeval now;
	gettimeofday(&now, NULL);
	
	if(!rumbleinit)
	{
		request = now;
	}
	
	bool rumble = false;
	
	if(request.tv_sec > now.tv_sec)
	{
		rumble = true;
	}
	else if(request.tv_sec == now.tv_sec && request.tv_usec > now.tv_usec)
	{
		rumble = true;
	}
	
	WPAD_Rumble(WPAD_CHAN_0, rumble ? 1 : 0);
}

void timedRumble(float len)
{
	timeval now;
	gettimeofday(&now, NULL);

	time_t usec = 100000*floor(len);
	time_t sec = (time_t)len;
	
	if(usec + now.tv_usec > 100000)
	{
		request.tv_sec = sec+1+now.tv_sec;
		request.tv_usec = usec + now.tv_usec - 100000;
	}
	else
	{
		request.tv_sec = sec + now.tv_sec;
		request.tv_usec = usec + now.tv_usec;
	}
}

void stopRumble()
{
	request = (timeval) {0,0};
}
