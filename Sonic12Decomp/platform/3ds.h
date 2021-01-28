#pragma once

#ifdef __3DS__

#include <cstdarg>
#include <stdint.h>

#define Uint32 uint32_t
#define Uint16 uint16_t
#define Uint8  uint8_t
#define Sint32 int32_t
#define Sint16 int16_t
#define Sint8  int8_t

//System
void sys_Init();
bool sys_MainLoop();
Uint32 sys_GetTicks();

//Graphics
void gfx_Init();
void gfx_UpdateScreen(Uint16 *pixels, bool vsync);

//Input

//Audio

#endif