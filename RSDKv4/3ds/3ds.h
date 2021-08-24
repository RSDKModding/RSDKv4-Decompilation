#pragma once

#ifdef __3DS__

#include <cstdarg>
#include <stdint.h>
#include <stdio.h>

#define Uint32 uint32_t
#define Uint16 uint16_t
#define Uint8  uint8_t
#define Sint32 int32_t
#define Sint16 int16_t
#define Sint8  int8_t

//System
void sys_Init(const char* basePath);
void sys_Exit();

bool sys_MainLoop();
Uint32 sys_GetTicks();
int sys_GetLanguage();

void* sys_LinearAlloc(Uint32 size);
void sys_LinearFree(void* data);

void* sys_CreateThread(void (*func)(void*), void* arg = NULL);

//Graphics
void gfx_Init();
void gfx_Exit();

void gfx_SetResolution(int w, int h, bool debug);

void gfx_UpdateScreen(Uint16 *pixels, bool vsync);

//Input
void inp_ScanInput();
bool inp_GetButtonDown(int btn);

//Audio
typedef struct
{
	Uint32 sampleRate;
	Uint32 size;
	Uint16 bitsPerSample;
	Uint16 numChannels;
	Uint8* data;
} AUD_WavData;

int aud_Init(void (*callback)(void*, uint8_t*, int));
void aud_Exit();

AUD_WavData* aud_LoadWAV(FILE* f);
void aud_FreeWAV(AUD_WavData* wav);

#endif