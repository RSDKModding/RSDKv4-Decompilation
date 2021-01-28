#include "3ds.h"
#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//System
void sys_Init()
{
	srand(time(0));
	osSetSpeedupEnable(true);
}

bool sys_MainLoop()
{
	return aptMainLoop();
}

Uint32 sys_GetTicks()
{
	return osGetTime();
}

//Graphics
void gfx_Init()
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
	gfxSetWide(false);
	gfxSet3D(false);
}

void gfx_UpdateScreen(Uint16 *pixels, bool vsync)
{
	u8* out = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	unsigned int pix = 0;

	for (int y = 0; y < 240; ++y)
	{
		for (int x = 0; x < 400; ++x)
		{
			//B | G | R
			pix = ((*pixels & 0x1F) << 3) | (((*pixels & 0x7E0) >> 3) << 8) | (((*pixels & 0xF800) >> 8) << 16);
			memcpy(&out[((x * 240) + (240-y-1)) * 3], &pix, 3);
			pixels++;
		}
	}	
	
	gfxFlushBuffers();
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