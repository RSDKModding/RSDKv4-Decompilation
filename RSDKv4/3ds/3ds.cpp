#include "3ds.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <3ds.h>

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

void sys_Exit()
{

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

void* sys_CreateThread(void (*func)(void*), void* arg)
{
	s32 prio = 0;
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	return (void*)threadCreate(func, arg, 4*1024, prio-1, -2, false);
}

//Graphics
static bool topScreen = true;

void gfx_Init()
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);

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
	
	//set screen format to 565 after splash screen
	gfxSetScreenFormat(GFX_TOP, GSP_RGB565_OES);
}

void gfx_Exit()
{
	gfxExit();
}

void gfx_SetResolution(int w, int h, bool debug)
{
	//make screen wide for 800px
	if (w == 800)
	{
		gfxSetWide(true);
	}
	//use bottom screen
	else if (w == 320) 
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
		for (int x = 0; x < ((topScreen)? (gfxIsWide())? 800: 400: 320); ++x) {
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

//Audio
#define BUF_SIZE (4096)
#define NDSP_NUM_CHANNELS 2
static ndspWaveBuf waveBuf[NDSP_NUM_CHANNELS];
static int fillBlock = 0;
static void (*processAudio) (void*, uint8_t*, int) = NULL;

static void audioCallback(void* userdata)
{	
	if (waveBuf[fillBlock].status != NDSP_WBUF_DONE)
		return;

	processAudio(NULL, (uint8_t*)waveBuf[fillBlock].data_vaddr, BUF_SIZE);

	ndspChnWaveBufAdd(0, &waveBuf[fillBlock]);
	DSP_FlushDataCache(waveBuf[fillBlock].data_vaddr, BUF_SIZE);

	fillBlock = !fillBlock;
}

int aud_Init(void (*callback)(void*, uint8_t*, int))
{
	int ret = ndspInit();

	if (ret) {
		printf("ndspInit failed.\nDid you dump your dspfirm.cdc?\n");
		return ret;
	}

	//channel setup
	ndspChnReset(0);			
	ndspChnSetInterp(0, NDSP_INTERP_NONE);
	ndspChnSetRate(0, 44100);
	ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

	if (!(waveBuf[0].data_vaddr = (uint8_t*)linearAlloc(BUF_SIZE)))
		printf("aud_Init: waveBuf[0].data_vaddr alloc failed\n");

	if (!(waveBuf[1].data_vaddr = (uint8_t*)linearAlloc(BUF_SIZE)))
		printf("aud_Init: waveBuf[1].data_vaddr alloc failed\n");

	waveBuf[0].nsamples = 
	waveBuf[1].nsamples = BUF_SIZE / 2 / 2;
	waveBuf[0].looping = 
	waveBuf[1].looping = false;
	waveBuf[0].status = 
	waveBuf[1].status = NDSP_WBUF_DONE;

	if (callback) {
		processAudio = callback;
		ndspSetCallback(audioCallback, NULL);
	}

	return ret;
}

void aud_Exit()
{
	ndspChnReset(0);

	if (waveBuf[0].data_vaddr) {
		linearFree((char*)waveBuf[0].data_vaddr);
		waveBuf[0].data_vaddr = NULL;
	}	

	if (waveBuf[0].data_vaddr) {
		linearFree((char*)waveBuf[1].data_vaddr);
		waveBuf[1].data_vaddr = NULL;
	}	

	ndspExit();
}

#pragma pack(push,1)
typedef struct
{
	unsigned int ChunkID;
	unsigned int ChunkSize;
	unsigned int Format;
	unsigned int Subchunk1ID;
	unsigned int Subchunk1Size;
	unsigned short AudioFormat;
	unsigned short NumChannels;
	unsigned int SampleRate;
	unsigned int ByteRate;
	unsigned short BlockAlign;
	unsigned short BitsPerSample;
	unsigned int Subchunk2ID;
	unsigned int Subchunk2Size;
} WAVHeader;
#pragma pack(push,0)

AUD_WavData* aud_LoadWAV(FILE* f)
{
	AUD_WavData* wav = nullptr;
	
	if (!f) {
		printf("aud_LoadWAV failed\n");
	}
	else {
		WAVHeader wh;
		fread(&wh, 1, sizeof(WAVHeader), f);

		wav = (AUD_WavData*)malloc(sizeof(AUD_WavData));
		if (!wav) {
			printf("malloc failed\n");
			return wav;
		}

		wav->sampleRate = (float)wh.SampleRate;
		wav->size = wh.Subchunk2Size;
		wav->bitsPerSample = wh.BitsPerSample;
		wav->numChannels = wh.NumChannels;

		//if (wav->bitsPerSample != 16) printf("format: %d\n", wav->bitsPerSample);
		if (wav->sampleRate != 44100) printf("rate: %d\n", (int)wav->sampleRate);		
		if (wav->numChannels != 1) printf("channels: %d\n", wav->numChannels);

		if (wav->bitsPerSample == 8) {
			wav->size = wh.Subchunk2Size * 2;
			wav->bitsPerSample = 16;
			wav->data = (u8*)linearAlloc(wav->size);

			if (!wav->data) {
				printf("linearAlloc failed\n");
				free(wav);
				wav = nullptr;
				return wav;
			}

			unsigned char c;
			Sint16* ptr = (Sint16*)wav->data;
			for (size_t i = 0; i < wh.Subchunk2Size; i++) {
				fread(&c, 1, 1, f);
				*ptr++ = (Sint16)(c - 0x80) << 8;
			}
		}
		else {
			wav->data = (u8*)linearAlloc(wav->size);
			if (!wav->data) {
				printf("linearAlloc failed\n");
				free(wav);
				wav = nullptr;
				return wav;
			}

			fread(wav->data, 1, wav->size, f);
		}		
	}

	return wav;
}

void aud_FreeWAV(AUD_WavData* wav)
{
	if (wav) {
		//if (wav->data)
			//linearFree(wav->data);
		//wav->data = nullptr;
		free(wav);
	}
}