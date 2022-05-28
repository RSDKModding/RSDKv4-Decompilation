#ifndef MATH_H
#define MATH_H

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#undef M_PI_2
#define M_PI_2 (M_PI * 2.0)

#undef M_PI_H
#define M_PI_H (M_PI * 0.5)

#define MEM_ZERO(x)  memset(&(x), 0, sizeof((x)))
#define MEM_ZEROP(x) memset((x), 0, sizeof(*(x)))

extern int sinM7LookupTable[0x200];
extern int cosM7LookupTable[0x200];

extern int sin512LookupTable[0x200];
extern int cos512LookupTable[0x200];

extern int sin256LookupTable[0x100];
extern int cos256LookupTable[0x100];

extern byte arcTan256LookupTable[0x100 * 0x100];

// Setup Angles
void CalculateTrigAngles();

inline int Sin512(int angle)
{
    if (angle < 0)
        angle = 0x200 - angle;

    angle &= 0x1FF;
    return sin512LookupTable[angle];
}

inline int Cos512(int angle)
{
    if (angle < 0)
        angle = 0x200 - angle;
    angle &= 0x1FF;
    return cos512LookupTable[angle];
}

inline int Sin256(int angle)
{
    if (angle < 0)
        angle = 0x100 - angle;
    angle &= 0xFF;
    return sin256LookupTable[angle];
}

inline int Cos256(int angle)
{
    if (angle < 0)
        angle = 0x100 - angle;
    angle &= 0xFF;
    return cos256LookupTable[angle];
}

// Get Arc Tan value
byte ArcTanLookup(int X, int Y);

inline double DegreesToRad(float degrees) { return (M_PI / 180) * degrees; }

#endif // !MATH_H
