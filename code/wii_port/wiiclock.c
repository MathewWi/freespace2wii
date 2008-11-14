#include <gccore.h>
#include "wiiclock.h"

volatile clock_t counter;
syswd_t pluser;

	
clock_t wiiclock()
	__attribute__ ((no_instrument_function));
	
void pluse()
	__attribute__ ((no_instrument_function));
	
void init_wiiclock()
	__attribute__ ((no_instrument_function));

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
