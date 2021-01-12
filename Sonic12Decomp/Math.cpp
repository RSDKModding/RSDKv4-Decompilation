#include "RetroEngine.hpp"
#include <math.h>
#include "ida.h"

int sinValM7[512];
int cosValM7[512];

int sinVal512[512];
int cosVal512[512];

int sinVal256[256];
int cosVal256[256];

int atanVal256[0x100 * 0x100];

void CalculateTrigAngles()
{
    for (int i = 0; i < 0x200; ++i) {
        float Val     = sin(((float)i / 256.0) * M_PI);
        sinValM7[i] = (Val * 4096.0);
        Val           = cos(((float)i / 256.0) * M_PI);
        cosValM7[i] = (Val * 4096.0);
    }

    cosValM7[0]   = 4096;
    cosValM7[128] = 0;
    cosValM7[256] = -4096;
    cosValM7[384] = 0;
    sinValM7[0]   = 0;
    sinValM7[128] = 4096;
    sinValM7[256] = 0;
    sinValM7[384] = -4096;

    for (int i = 0; i < 0x200; ++i) {
        float Val       = sinf(((float)i / 256) * M_PI);
        sinVal512[i] = (signed int)(Val * 512.0);
        Val             = cosf(((float)i / 256) * M_PI);
        cosVal512[i] = (signed int)(Val * 512.0);
    }

    cosVal512[0]   = 0x200;
    cosVal512[128] = 0;
    cosVal512[256] = -0x200;
    cosVal512[384] = 0;
    sinVal512[0]   = 0;
    sinVal512[128] = 0x200;
    sinVal512[256] = 0;
    sinVal512[384] = -0x200;

    for (int i = 0; i < 0x100; i++) {
        sinVal256[i] = (sinVal512[i * 2] >> 1);
        cosVal256[i] = (cosVal512[i * 2] >> 1);
    }

    for (int Y = 0; Y < 0x100; ++Y) {
        for (int X = 0; X < 0x100; ++X) {
            float angle = atan2f(Y, X);
            atanVal256[(64 * X) + Y] = (signed int)(angle * 40.743664f);
        }
    }
}

byte ArcTanLookup(int X, int Y)
{
    int x = 0;
    int y = 0;

    x = abs(X);
    y = abs(Y);

    if (x <= y) {
        while (y > 0xFF) {
            x >>= 4;
            y >>= 4;
        }
    }
    else {
        while (x > 0xFF) {
            x >>= 4;
            y >>= 4;
        }
    }
    if (X <= 0) {
        if (Y <= 0)
            return atanVal256[(x << 8) + y] + -0x80;
        else
            return -0x80 - atanVal256[(x << 8) + y];
    }
    else if (Y <= 0)
        return -atanVal256[(x << 8) + y];
    else
        return atanVal256[(x << 8) + y];
}

int64_t pow(int base, int exponent) {
    if (!exponent) return 1;
    int64_t result = base;
    for (int i = 1; i < exponent; i++) result *= base;
    return result; 
}

/*
void CalculateTrigAngles()
{
  float v0; // xmm1_4
  signed int v1; // ebp
  float i; // xmm0_4
  float x; // ST1C_4
  float v4; // ST14_4
  float v5; // ST14_4
  signed int v6; // ebp
  float v7; // xmm0_4
  float v8; // xmm1_4
  float v9; // ST1C_4
  float v10; // ST14_4
  float v11; // ST14_4
  signed int v12; // edx
  int v13; // ecx
  int v14; // eax
  signed int Y; // edi
  int X; // esi
  unsigned __int8 *ATan; // ebp
  float X2; // xmm0_4
  int result; // eax
  float angle; // ST14_4

  v0 = 0.0;
  v1 = 0;
  for ( i = 1.0; ; i = v5 )
  {
    sinValM7[v1] = (signed int)(float)(v0 * 4096.0);
    cosValM7[v1++] = (signed int)(float)(i * 4096.0);
    if ( v1 == 512 )
      break;
    x = (float)((float)v1 * 0.00390625) * 3.1415927;
    v4 = sinf(x);
    v0 = v4;
    v5 = cosf(x);
  }
  sinValM7[0] = 0;
  v6 = 0;
  cosValM7[0] = 4096;
  sinValM7[128] = 4096;
  cosValM7[128] = 0;
  sinValM7[256] = 0;
  cosValM7[256] = -4096;
  sinValM7[384] = -4096;
  v7 = 1.0;
  cosValM7[384] = 0;
  v8 = 0.0;
  while ( 1 )
  {
    sinVal512[v6] = (signed int)(float)(v8 * 512.0);
    cosVal512[v6++] = (signed int)(float)(v7 * 512.0);
    if ( v6 == 512 )
      break;
    v9 = (float)((float)v6 * 0.00390625) * 3.1415927;
    v10 = sinf(v9);
    v8 = v10;
    v11 = cosf(v9);
    v7 = v11;
  }
  sinVal512[0] = 0;
  v12 = 512;
  cosVal512[0] = 512;
  v13 = 0;
  sinVal512[128] = 512;
  v14 = 0;
  cosVal512[128] = 0;
  sinVal512[256] = 0;
  cosVal512[256] = -512;
  sinVal512[384] = -512;
  cosVal512[384] = 0;
  while ( 1 )
  {
    sinVal256[v14] = v13 >> 1;
    cosVal256[v14++] = v12 >> 1;
    if ( v14 == 256 )
      break;
    v13 = sinVal512[2 * v14];
    v12 = cosVal512[2 * v14];
  }
  Y = 0;
  do
  {
    X = 0;
    ATan = (uchar*)&atanVal256[Y];
    do
    {
      X2 = (float)X++;
      ATan += 256;
      angle = atan2f((float)Y, X2);
      *(ATan - 256) = (signed int)(float)(angle * 40.743664);
    }
    while ( X != 256 );
    ++Y;
  }
  while ( Y != 256 );
}//*/
