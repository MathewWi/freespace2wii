#include "wii_port/wiiassert.h"
#undef exit

#include <stdio.h>
#include <stdlib.h>

#include <ogc/video.h>
#include <wiiuse/wpad.h>

static void *exception_xfb = (void*)0xC1710000;			//we use a static address above ArenaHi.
extern "C" void __console_init(void *framebuffer,int xstart,int ystart,int xres,int yres,int stride);

extern "C" void WiiAssert(const char * text,const char *filename, int line)
{
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
	char *p = 0x0;
	printf("Stack failure!");
	wiipause();
	*p = 1;
	
}