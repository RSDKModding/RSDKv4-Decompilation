#include "3ds.h"
#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if BUILD_SONIC_1
#include "splash_sonic1.h"
#elif BUILD_SONIC_2
#include "splash_sonic2.h"
#endif

//System
void sys_Init(const char* basePath)
{
	srand(time(0));
	osSetSpeedupEnable(true);
	cfguInit();
	chdir(basePath);
}

bool sys_MainLoop()
{
	return aptMainLoop();
}

Uint32 sys_GetTicks()
{
	return osGetTime();
}

int sys_GetLanguage()
{
	u8 l;
	CFGU_GetSystemLanguage(&l);

	int ret = 0;

	switch (l)
	{
		case CFG_LANGUAGE_EN: ret = 0; break;
		case CFG_LANGUAGE_FR: ret = 1; break;
		case CFG_LANGUAGE_IT: ret = 2; break;
		case CFG_LANGUAGE_DE: ret = 3; break;
		case CFG_LANGUAGE_ES: ret = 4; break;
		//case CFG_LANGUAGE_JP: ret = 5; break;
		case CFG_LANGUAGE_PT: ret = 6; break;
		//case CFG_LANGUAGE_RU: ret = 7; break;
		//case CFG_LANGUAGE_KO: ret = 8; break;
		//case CFG_LANGUAGE_ZH: ret = 9; break;
		//case CFG_LANGUAGE_TW: ret = 10; break;
		default: ret = 0;
	};

	return ret;
}

void* sys_LinearAlloc(Uint32 size)
{
	return linearAlloc(size);
}

void sys_LinearFree(void* data)
{
	linearFree(data);
}

//Graphics
static bool topScreen = true;

void gfx_Init()
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
	gfxSetWide(false);
	gfxSet3D(false);

	//display splash screen
	u8* splash = nullptr;
	#ifdef BUILD_SONIC_1
	splash = SPLASH_SONIC1;
	#elif BUILD_SONIC_2
	splash = SPLASH_SONIC2;
	#endif

	if (splash != nullptr)
	{
		u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
		memcpy(fb, splash, 400*240*3);

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}
	
	//set screen format to 565 *after* splash screen
	gfxSetScreenFormat(GFX_TOP, GSP_RGB565_OES);
}

void gfx_Exit()
{
	gfxExit();
}

void gfx_SetResolution(int w, int h, bool debug)
{
	//use bottom screen
	if (w == 320) 
	{
		topScreen = false;
		
		gfxSetScreenFormat(GFX_BOTTOM, GSP_RGB565_OES);
		gfxSetDoubleBuffering(GFX_BOTTOM, true);
		
		if (debug) {
			consoleInit(GFX_TOP, NULL);
		} else {
			gfxSetScreenFormat(GFX_TOP, GSP_BGR8_OES);
			gfxSetDoubleBuffering(GFX_TOP, false);
		}		
	}
}

void gfx_UpdateScreen(Uint16 *pixels, bool vsync)
{
	u16* out = (u16*)gfxGetFramebuffer(((topScreen)? GFX_TOP: GFX_BOTTOM), GFX_LEFT, NULL, NULL);

	for (int y = 0; y < 240; ++y) {
		for (int x = 0; x < ((topScreen)? 400: 320); ++x) {
			out[((x * 240) + (240-y-1))] = *pixels++;
		}
	}
	
	//gfxFlushBuffers();
	gfxSwapBuffers();
	if (vsync)
		gspWaitForVBlank();
}

//Input
static u32 buttons[] = {
	KEY_A, 		//0
	KEY_B,		//1
	KEY_Y,		//2
	KEY_X,		//3
	KEY_SELECT,	//4
	KEY_TOUCH,	//5
	KEY_START,	//6
	0,			//7
	0,			//8
	KEY_L,		//9
	KEY_R,		//10
	KEY_UP,		//11
	KEY_DOWN,	//12
	KEY_LEFT,	//13
	KEY_RIGHT,	//14
	0,			//15
	KEY_ZL,		//16
	KEY_ZR		//17
};

static u32 kDown = 0;

void inp_ScanInput()
{
	hidScanInput();
	kDown = hidKeysHeld();
}

bool inp_GetButtonDown(int btn)
{	
	if (btn < 0 || btn > 17)
		return false;
		
	return (kDown & buttons[btn]);
}