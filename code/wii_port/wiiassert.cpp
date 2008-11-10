#include "wii_port/wiiassert.h"
#include "wii_port/wiiclock.h"
#undef exit

#include <stdio.h>
#include <stdlib.h>

#include <ogc/video.h>
#include <wiiuse/wpad.h>
#include <GL/GLwii.h>

static void *exception_xfb = (void*)0xC1710000;			//we use a static address above ArenaHi.
extern "C" void __console_init(void *framebuffer,int xstart,int ystart,int xres,int yres,int stride);

extern "C" void WiiAssert(const char * text,const char *filename, int line)
{
	ShowConsole();
	CON_SetStipple(0);
	VIDEO_WaitVSync();	
	
	LWP_Reschedule(LWP_PRIO_IDLE);
	LWP_SetThreadPriority (LWP_GetSelf (), LWP_PRIO_HIGHEST);
	
	while ( 1 ) {
		VIDEO_SetNextFramebuffer(exception_xfb);
		
		__console_init(exception_xfb,20,270,640,250,1280);
		
		kprintf("\n\n");
		kprintf("Assertion failed:\n %s \n", text);
		kprintf("Occured in %s at %d\n\n", filename, line);
		kprintf("Press Home on WiiMote 1 or A on Gamecube Pad 1 to exit\n");

		WPAD_ScanPads();
		PAD_ScanPads();

		int buttonsDown = PAD_ButtonsDown(0);
		int wbuttonsDown = WPAD_ButtonsDown(0);

		if ( (buttonsDown & PAD_BUTTON_A) || (wbuttonsDown & WPAD_BUTTON_HOME) )
		{
			kprintf("Reset\n");
			throw;
			exit(0);
		}
		
		VIDEO_WaitVSync();
	}
}

extern "C" void wiipause()
{
	ShowConsole();
	CON_SetStipple(0);
	VIDEO_WaitVSync();
	
	kprintf("Press Home on WiiMote 1 or A on Gamecube Pad 1 to continue\n");
	
	fflush(stdout);
	fflush(stderr);
	
	while(1) {

		WPAD_ScanPads();
		PAD_ScanPads();

		int buttonsDown = PAD_ButtonsDown(0);
		int wbuttonsDown = WPAD_ButtonsDown(0);

		if ( (buttonsDown & PAD_BUTTON_A) || (wbuttonsDown & WPAD_BUTTON_HOME) )
		{
			return;
		}
		
		if(wbuttonsDown & WPAD_BUTTON_1)
		{
			exit(0);
		}
		
		VIDEO_WaitVSync();
	}
}

extern "C" void pause_exit(int code,const char *filename, int line)
{
	ShowConsole();
	CON_SetStipple(0);
	VIDEO_WaitVSync();
	
	fflush(stderr);
	LWP_Reschedule(LWP_PRIO_IDLE);
	LWP_SetThreadPriority (LWP_GetSelf (), LWP_PRIO_HIGHEST);
	
	while ( 1 ) {
		VIDEO_SetNextFramebuffer(exception_xfb);
		
		__console_init(exception_xfb,20,350,640,100,1280);
		
		kprintf("\n\n");
		kprintf("Exit was called from %s at %d\n\n", filename, line);
		kprintf("Press Home on WiiMote 1 or A on Gamecube Pad 1 to exit\n");

		WPAD_ScanPads();
		PAD_ScanPads();

		int buttonsDown = PAD_ButtonsDown(0);
		int wbuttonsDown = WPAD_ButtonsDown(0);

		if ( (buttonsDown & PAD_BUTTON_A) || (wbuttonsDown & WPAD_BUTTON_HOME) )
		{
			exit(code);
		}
		
		VIDEO_WaitVSync();
	}	
}


long __stack_chk_guard[8] = {0,0,0,0,0,0,0,0};
extern "C" void __stack_chk_fail(void);

extern "C" void __stack_chk_fail(void)
{
	u32 *p = (u32 *) 0x1;
	printf("Stack failure!\n");
	wiipause();
	*p = 0xDEADBEEF;
}

volatile unsigned char power_pressed = 0;
	
extern "C" void fs2_power_off(s32 chan)
{
	power_pressed = 1;
}

static volatile u32 perf_count;

#include "SDL_Timer.h"

extern "C" unsigned int getCount()
{
	return SDL_GetTicks();
}

void incCount(syswd_t)
{
	++perf_count;
}


syswd_t perf_timer;
FILE * perf_log;

static u32 counter;
extern "C" void tic()
{
	counter = getCount();
}

extern "C" void tocInternal(const char *file, int line)
{
	u32 c = getCount() - counter;
	printf("%u ticks at %s - %d\n", c, file, line);
	fprintf(perf_log, "%u ticks at %s - %d\n", c, file, line);
	counter = getCount();
}

extern "C" void tocInternalMsg(const char *file, int line, const char *msg)
{
	u32 c = getCount() - counter;
	printf("%u ticks at %s - %d\n", c, file, line);
	fprintf(perf_log, "%u ticks at %s - %d, %s\n", c, file, line, msg);
	counter = getCount();
}


void WiiInit()
{
	init_wiiclock();
	ShowConsole(); 
	CON_SetStipple(0);
	
	// Listen for power off
	WPAD_SetPowerCallback(&fs2_power_off);
	
	perf_count = 0;
	
	timespec start;
	timespec interval;
	
    int timer_created = !SYS_CreateAlarm(&perf_timer);
	
	start.tv_sec = 0;
	start.tv_nsec = 0;
	
	interval.tv_sec = 0;
	interval.tv_nsec = 1000;
	
	if(timer_created)
	{		
		SYS_SetPeriodicAlarm(perf_timer, &start, &interval, &incCount);
	}
	
	perf_log = NULL;
	
	perf_log = fopen("/perf_log.txt","w");
}

