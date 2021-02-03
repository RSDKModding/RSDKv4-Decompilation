#include "3ds.h"
#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if BUILD_SONIC_1
#include "splash_sonic1.h"
#elif BUILD_SONIC_2
#include "splash_sonic2.h"
#endif

//System
void sys_Init()
{
	srand(time(0));
	osSetSpeedupEnable(true);
	cfguInit();
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

void gfx_UpdateScreen(Uint16 *pixels, bool vsync)
{
	u16* out = (u16*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

	for (int y = 0; y < 240; ++y) {
		for (int x = 0; x < 400; ++x) {
			out[((x * 240) + (240-y-1))] = *pixels++;
		}
	}	
	
	//gfxFlushBuffers();
	gfxSwapBuffers();
	if (vsync)
		gspWaitForVBlank();
}

//Input
static u32 kBuffer = 0;

void inp_ScanInput()
{
	hidScanInput();
	u32 kDown = hidKeysHeld();

	kBuffer = 0;
	if (kDown & KEY_UP)    kBuffer |= BTN_UP;
	if (kDown & KEY_DOWN)  kBuffer |= BTN_DOWN;
	if (kDown & KEY_LEFT)  kBuffer |= BTN_LEFT;
	if (kDown & KEY_RIGHT) kBuffer |= BTN_RIGHT;
	if (kDown & KEY_A)     kBuffer |= BTN_A;
	if (kDown & KEY_B)     kBuffer |= BTN_B;
	if (kDown & KEY_Y)     kBuffer |= BTN_C;
	if (kDown & KEY_START) kBuffer |= BTN_START;
	if (kDown & KEY_X) 	   kBuffer |= BTN_START;
}

bool inp_GetButtonDown(int btn)
{
	return (kBuffer & btn);
}

//Audio
