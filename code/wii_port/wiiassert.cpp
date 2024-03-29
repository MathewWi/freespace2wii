#include "wii_port/wiiassert.h"
#include "wii_port/wiiclock.h"
#undef exit

#include <stdio.h>
#include <stdlib.h>

#include <ogc/video.h>
#include <wiiuse/wpad.h>
#include <GL/GLwii.h>

#include <ogc/audio.h>

#ifdef SCP_WII

extern "C" void printTextureStats();

#include <ogc/system.h>

#define AssertLoc(x,a,b) do { if (!(x)){ WiiAssert(#x,a,b); } } while (0)
#include <memtracer.h>
#include <wiitrace.h>

#endif

static void *exception_xfb = (void*)0xC1710000;			//we use a static address above ArenaHi.
extern "C" void __console_init(void *framebuffer,int xstart,int ystart,int xres,int yres,int stride);

extern "C" void WiiAssert(const char * text,const char *filename, int line)
{
	fflush(stdout);
	fflush(stderr);
	printTextureStats();
	closeProfiler();
	closeMemtrace();
	
	ShowConsole();
	CON_SetStipple(0);
	AUDIO_StopDMA();
	AUDIO_RegisterDMACallback(NULL);
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
			// Produce a stack trace
			u32 *p = (u32 *) 0x1;
			*p = 0xDEADBEEF;
			
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
extern "C" void __stack_chk_fail(void)
	__attribute__ ((no_instrument_function));

extern "C" void __stack_chk_fail(void)
{
	u32 *p = (u32 *) 0x1;
	printf("Stack failure!\n");
	wiipause();
	*p = 0xDEADBEEF;
}

volatile unsigned char power_pressed = 0;


extern "C" void fs2_power_off(void)
	__attribute__ ((no_instrument_function));
	
extern "C" void fs2_power_off()
{
	power_pressed = 1;
}

static volatile u32 perf_count;

#include "SDL_Timer.h"

extern "C" unsigned int getCount()
	__attribute__ ((no_instrument_function));
extern "C" unsigned int getCount()
{
	return SDL_GetTicks();
}

syswd_t perf_timer;
FILE * perf_log;

static u32 perf_counter;

extern "C" void tic()
	__attribute__ ((no_instrument_function));
extern "C" void tic()
{
	perf_counter = getCount();
}

extern "C" void tocInternal(const char *file, int line)
	__attribute__ ((no_instrument_function));
extern "C" void tocInternal(const char *file, int line)
{
	u32 c = getCount() - perf_counter;
	printf("%u ticks at %s - %d\n", c, file, line);
	fprintf(perf_log, "%u ticks at %s - %d\n", c, file, line);
	perf_counter = getCount();
}

extern "C" void tocInternalMsg(const char *file, int line, const char *msg)
	__attribute__ ((no_instrument_function));
extern "C" void tocInternalMsg(const char *file, int line, const char *msg)
{
	u32 c = getCount() - perf_counter;
	printf("%u ticks at %s - %d\n", c, file, line);
	fprintf(perf_log, "%u ticks at %s - %d, %s\n", c, file, line, msg);
	perf_counter = getCount();
}

#include <wiitrace.h>
#include <memtracer.h>
#include <GL/GLwii.h>
//#include <custom_assert.h>

static size_t prealloc_size[] = { 736, 1440, 2784, 2848, 5536, 5664, 10976, 11040, 11296, 21920, 22048, 43744, 43808, 87456, 88064, 174816, 174880, 349600, 1398144};
static size_t prealloc_count[] = {124,   14,  116,    6,   12,    4,   346,    12,    15,     7,     2,   149,     9,    10,     2,     45,      1,      4,       1};

void WiiTexMemInit()
{
	for(size_t i = 0; i < sizeof(prealloc_size)/sizeof(prealloc_size[0]); ++i)
	{
		for(size_t j = 0; j < prealloc_count[i]; ++j)
		{
			PreallocTexMem(prealloc_size[i]);
		}
	}
}


void WiiInit()
	__attribute__ ((no_instrument_function));
void WiiInit()
{
	ShowConsole(); 
	CON_SetStipple(0);
	
	// Listen for power off
	//WPAD_SetPowerButtonCallback(fs2_power_off);
	SYS_SetPowerCallback(fs2_power_off);
	
	perf_counter = getCount();
	
	perf_log = NULL;
	
	perf_log = fopen("/perf_log.txt","w");
	
	//initProfiler("/trace_log.txt",0);
	//initMemtrace("/mem_trace_log.mtr",1);
	
	//set_custom_assert(&WiiAssert);
}

void WiiClose()
	__attribute__ ((no_instrument_function));
void WiiClose()
{
	//closeMemtrace();
	closeProfiler();
	
	fclose(perf_log);
	
	if(power_pressed)
	{
		SYS_ResetSystem(SYS_POWEROFF,0,0);
	}
	else
	{
		SYS_ResetSystem(SYS_SHUTDOWN,0,0);
	}
}
