#include <gccore.h>
#include "wiiclock.h"

clock_t counter;
syswd_t pluser;

clock_t wiiclock()
{
	return counter;
}

void pluse()
{
	counter++;
}

void init_wiiclock()
{
	struct timespec start;
	struct timespec interval;
	
	counter = 0;
	
	int pluse_conf = !SYS_CreateAlarm (&pluser);
		
	start.tv_sec = 0;
	start.tv_nsec = 0;
	
	interval.tv_sec = 0;
	interval.tv_nsec = 1000000000/CLOCKS_PER_SEC;
	
	if(pluse_conf)
	{	
		SYS_SetPeriodicAlarm(pluser, &start, &interval, &pluse);
	}
}
