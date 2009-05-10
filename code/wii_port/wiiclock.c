#include <gccore.h>
#include "wiiclock.h"
#include <ogc/lwp_watchdog.h>
	
clock_t wiiclock()
	__attribute__ ((no_instrument_function));

clock_t wiiclock()
{
	return gettime();
}
