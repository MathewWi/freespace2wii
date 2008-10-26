#include "wii_port/wiiassert.h"
#undef exit

#include <stdio.h>
#include <stdlib.h>

#include <ogc/video.h>
#include <wiiuse/wpad.h>

static void *exception_xfb = (void*)0xC1710000;			//we use a static address above ArenaHi.
extern "C" void __console_init(void *framebuffer,int xstart,int ystart,int xres,int yres,int stride);


void WiiAssert(const char * text,const char *filename, int line)
{
	LWP_Reschedule(LWP_PRIO_IDLE);
	LWP_SetThreadPriority (LWP_GetSelf (), LWP_PRIO_HIGHEST);
	
	while ( 1 ) {
		VIDEO_SetNextFramebuffer(exception_xfb);
		
		__console_init(exception_xfb,20,20,640,250,1280);
		
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
			exit(0);
		}
		
		VIDEO_WaitVSync();
	}
}

void pause_exit(int code)
{
	LWP_Reschedule(LWP_PRIO_IDLE);
	LWP_SetThreadPriority (LWP_GetSelf (), LWP_PRIO_HIGHEST);
	
	while ( 1 ) {
		VIDEO_SetNextFramebuffer(exception_xfb);
		
		__console_init(exception_xfb,20,20,640,250,1280);
		
		kprintf("\n\n");
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