#include "RetroEngine.hpp"
#include <math.h>

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

    for (int X = 0; X < 0x100; ++X) {
        for (int Y = 0; Y < 0x100; ++Y) {
            double angle                     = atan2(Y, X);
            atanVal256[(0x100 * X) + Y] = (int)(float)(angle * 40.743664);
        }
    }
}

byte ArcTanLookup(int X, int Y)
{
    int XVal;
    byte result = 0;
    int YVal;

    if (X >= 0)
        XVal = X;
    else 
        XVal = -X;

    if (Y >= 0)
        YVal = Y;
    else 
        YVal = -Y;

    if (XVal <= YVal) {
        while (YVal > 0xFF) {
            XVal >>= 4;
            YVal >>= 4;
        }
    }
    else {
        while (XVal > 0xFF) {
            XVal >>= 4;
            YVal >>= 4;
        }
    }
    if (X <= 0) {
        if (Y <= 0)
            result = atanVal256[0x100 * XVal + YVal] + -0x80;
        else
            result = -0x80 - atanVal256[0x100 * XVal + YVal];
    }
    else if (Y <= 0)
        result = -atanVal256[0x100 * XVal + YVal];
    else 
        result = atanVal256[0x100 * XVal + YVal];
    return result;
}

int64_t pow(int base, int exponent) {
    if (!exponent) return 1;
    int64_t result = base;
    for (int i = 1; i < exponent; i++) result *= base;
    return result; 
}