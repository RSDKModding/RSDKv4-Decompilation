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

//Input

//Audio