#ifndef MATH_H
#define MATH_H

//#define M_PI (3.1415927)

#define MEM_ZERO(x)  memset(&(x), 0, sizeof((x)))
#define MEM_ZEROP(x) memset((x), 0, sizeof(*(x)))

extern int sinValM7[0x200];
extern int cosValM7[0x200];

extern int sinVal512[0x200];
extern int cosVal512[0x200];

extern int sinVal256[0x100];
extern int cosVal256[0x100];

extern byte atanVal256[0x100 * 0x100];

// Setup Angles
void CalculateTrigAngles();

inline int sin512(int angle) {
    if (angle < 0)
        angle = 0x200 - angle;
    angle &= 0x1FF;
    return sinVal512[angle];
}

inline int cos512(int angle)
{
    if (angle < 0)
        angle = 0x200 - angle;
    angle &= 0x1FF;
    return cosVal512[angle];
}

inline int sin256(int angle)
{
    if (angle < 0)
        angle = 0x100 - angle;
    angle &= 0xFF;
    return sinVal256[angle];
}

inline int cos256(int angle)
{
    if (angle < 0)
        angle = 0x100 - angle;
    angle &= 0xFF;
    return cosVal256[angle];
}

// Get Arc Tan value
byte ArcTanLookup(int X, int Y);

#endif // !MATH_H
