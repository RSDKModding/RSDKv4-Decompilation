#include "RetroEngine.hpp"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

int sinValM7[512];
int cosValM7[512];

int sinVal512[512];
int cosVal512[512];

int sinVal256[256];
int cosVal256[256];

byte atanVal256[0x100 * 0x100];

void CalculateTrigAngles()
{
    for (int i = 0; i < 0x200; ++i) {
        float val     = sin(((float)i / 256.0) * M_PI);
        sinValM7[i]   = (val * 4096.0);
        val           = cos(((float)i / 256.0) * M_PI);
        cosValM7[i]   = (val * 4096.0);
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
        float val       = sinf(((float)i / 256) * M_PI);
        sinVal512[i] = (signed int)(val * 512.0);
        val             = cosf(((float)i / 256) * M_PI);
        cosVal512[i]    = (signed int)(val * 512.0);
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
        byte *ATan = (byte *)&atanVal256[Y];
        for (int X = 0; X < 0x100; ++X) {
            float angle = atan2f(Y, X);
            *ATan       = (signed int)(angle * 40.743664f);
            ATan += 0x100;
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
