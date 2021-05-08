#include "RetroEngine.hpp"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

int sinValM7[0x200];
int cosValM7[0x200];

int sinVal512[0x200];
int cosVal512[0x200];

int sinVal256[0x100];
int cosVal256[0x100];

byte atanVal256[0x100 * 0x100];

void CalculateTrigAngles()
{
    for (int i = 0; i < 0x200; ++i) {
        sinValM7[i]   = (sin((i / 256.0) * M_PI) * 4096.0);
        cosValM7[i]   = (cos((i / 256.0) * M_PI) * 4096.0);
    }

    cosValM7[0]   = 0x1000;
    cosValM7[128] = 0;
    cosValM7[256] = -0x1000;
    cosValM7[384] = 0;
    sinValM7[0]   = 0;
    sinValM7[128] = 0x1000;
    sinValM7[256] = 0;
    sinValM7[384] = -0x1000;

    for (int i = 0; i < 0x200; ++i) {
        sinVal512[i] = (sinf((i / 256.0) * M_PI) * 512.0);
        cosVal512[i] = (cosf((i / 256.0) * M_PI) * 512.0);
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
            *ATan       = (angle * 40.743664f);
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
