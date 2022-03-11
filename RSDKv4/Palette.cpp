#include "RetroEngine.hpp"

// Palettes (as RGB888 Colours)
PaletteEntry fullPalette32[PALETTE_COUNT][PALETTE_SIZE];
PaletteEntry *activePalette32 = fullPalette32[0];

// Palettes (as RGB565 Colours)
ushort fullPalette[PALETTE_COUNT][PALETTE_SIZE];
ushort *activePalette = fullPalette[0]; // Ptr to the 256 colour set thats active

byte gfxLineBuffer[SCREEN_YSIZE]; // Pointers to active palette
int GFX_LINESIZE;
int GFX_LINESIZE_MINUSONE;
int GFX_LINESIZE_DOUBLE;
int GFX_FRAMEBUFFERSIZE;
int GFX_FBUFFERMINUSONE;

uint gfxPalette16to32[0x10000];

int fadeMode = 0;
byte fadeA   = 0;
byte fadeR   = 0;
byte fadeG   = 0;
byte fadeB   = 0;

int paletteMode = 1;

#if RETRO_HARDWARE_RENDER
int texPaletteNum = 0;
#endif

void LoadPalette(const char *filePath, int paletteID, int startPaletteIndex, int startIndex, int endIndex)
{
    FileInfo info;
    char fullPath[0x80];

    StrCopy(fullPath, "Data/Palettes/");
    StrAdd(fullPath, filePath);

    if (LoadFile(fullPath, &info)) {
        SetFilePosition(3 * startIndex);
        if (paletteID >= PALETTE_COUNT || paletteID < 0)
            paletteID = 0;

        byte colour[3];
        if (paletteID) {
            for (int i = startIndex; i < endIndex; ++i) {
                FileRead(&colour, 3);
                SetPaletteEntry(paletteID, startPaletteIndex++, colour[0], colour[1], colour[2]);
            }
        }
        else {
            for (int i = startIndex; i < endIndex; ++i) {
                FileRead(&colour, 3);
                SetPaletteEntry(-1, startPaletteIndex++, colour[0], colour[1], colour[2]);
            }
        }
        CloseFile();
    }
}

#if RETRO_REV00
void SetLimitedFade(byte paletteID, byte R, byte G, byte B, ushort alpha, int startIndex, int endIndex)
{
    if (paletteID >= PALETTE_COUNT)
        return;
    paletteMode     = 1;
    activePalette   = fullPalette[paletteID];
    activePalette32 = fullPalette32[paletteID];

    if (alpha >= PALETTE_SIZE) {
        alpha = PALETTE_SIZE - 1;
    }

    if (startIndex >= endIndex)
        return;

    uint alpha2 = 0xFF - alpha;
    for (int i = startIndex; i < endIndex; ++i) {
        PACK_RGB888(activePalette[i], (byte)((ushort)(R * alpha + alpha2 * activePalette32[i].r) >> 8),
                    (byte)((ushort)(G * alpha + alpha2 * activePalette32[i].g) >> 8),
                    (byte)((ushort)(B * alpha + alpha2 * activePalette32[i].b) >> 8));
        activePalette32[i].r = (byte)((ushort)(R * alpha + alpha2 * activePalette32[i].r) >> 8);
        activePalette32[i].g = (byte)((ushort)(G * alpha + alpha2 * activePalette32[i].g) >> 8);
        activePalette32[i].b = (byte)((ushort)(B * alpha + alpha2 * activePalette32[i].b) >> 8);
#if RETRO_HARDWARE_RENDER
        activePalette[i] |= 1;
#endif
    }
}
#else
void SetPaletteFade(byte destPaletteID, byte srcPaletteA, byte srcPaletteB, ushort blendAmount, int startIndex, int endIndex)
{
    if (destPaletteID >= PALETTE_COUNT || srcPaletteA >= PALETTE_COUNT || srcPaletteB >= PALETTE_COUNT)
        return;

    if (blendAmount >= 0x100) 
        blendAmount = 0xFF;

    if (startIndex >= endIndex)
        return;

    uint blendA         = 0xFF - blendAmount;
    ushort *dst         = &fullPalette[destPaletteID][startIndex];
    PaletteEntry *dst32 = &fullPalette32[destPaletteID][startIndex];
    for (int l = startIndex; l < endIndex; ++l) {
        *dst     = PACK_RGB888((byte)((ushort)(fullPalette32[srcPaletteB][l].r * blendAmount + blendA * fullPalette32[srcPaletteA][l].r) >> 8),
                           (byte)((ushort)(fullPalette32[srcPaletteB][l].g * blendAmount + blendA * fullPalette32[srcPaletteA][l].g) >> 8),
                           (byte)((ushort)(fullPalette32[srcPaletteB][l].b * blendAmount + blendA * fullPalette32[srcPaletteA][l].b) >> 8));
        dst32->r = (byte)((ushort)(fullPalette32[srcPaletteB][l].r * blendAmount + blendA * fullPalette32[srcPaletteA][l].r) >> 8);
        dst32->g = (byte)((ushort)(fullPalette32[srcPaletteB][l].g * blendAmount + blendA * fullPalette32[srcPaletteA][l].g) >> 8);
        dst32->b = (byte)((ushort)(fullPalette32[srcPaletteB][l].b * blendAmount + blendA * fullPalette32[srcPaletteA][l].b) >> 8);
#if RETRO_HARDWARE_RENDER
        *dst |= 1;
#endif

        ++dst;
        ++dst32;
    }
}
#endif