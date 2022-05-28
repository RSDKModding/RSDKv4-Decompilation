#include "RetroEngine.hpp"

struct GifDecoder {
    int depth;
    int clearCode;
    int eofCode;
    int runningCode;
    int runningBits;
    int prevCode;
    int currentCode;
    int maxCodePlusOne;
    int stackPtr;
    int shiftState;
    int fileState;
    int position;
    int bufferSize;
    uint shiftData;
    uint pixelCount;
    byte buffer[256];
    byte stack[4096];
    byte suffix[4096];
    uint prefix[4096];
};

const int LOADING_IMAGE = 0;
const int LOAD_COMPLETE = 1;
const int LZ_MAX_CODE   = 4095;
const int LZ_BITS       = 12;
const int FIRST_CODE    = 4097;
const int NO_SUCH_CODE  = 4098;

GifDecoder gifDecoder;
int codeMasks[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095 };

int ReadGifCode(void);
byte ReadGifByte(void);
byte TraceGifPrefix(uint *prefix, int code, int clearCode);

void InitGifDecoder()
{
    byte code = 0;
    FileRead(&code, 1);
    gifDecoder.fileState      = LOADING_IMAGE;
    gifDecoder.position       = 0;
    gifDecoder.bufferSize     = 0;
    gifDecoder.buffer[0]      = 0;
    gifDecoder.depth          = code;
    gifDecoder.clearCode      = 1 << code;
    gifDecoder.eofCode        = gifDecoder.clearCode + 1;
    gifDecoder.runningCode    = gifDecoder.eofCode + 1;
    gifDecoder.runningBits    = code + 1;
    gifDecoder.maxCodePlusOne = 1 << gifDecoder.runningBits;
    gifDecoder.stackPtr       = 0;
    gifDecoder.prevCode       = NO_SUCH_CODE;
    gifDecoder.shiftState     = 0;
    gifDecoder.shiftData      = 0;
    for (int i = 0; i <= LZ_MAX_CODE; ++i) gifDecoder.prefix[i] = (byte)NO_SUCH_CODE;
}
void ReadGifLine(byte *line, int length, int offset)
{
    int i         = 0;
    int stackPtr  = gifDecoder.stackPtr;
    int eofCode   = gifDecoder.eofCode;
    int clearCode = gifDecoder.clearCode;
    int prevCode  = gifDecoder.prevCode;
    if (stackPtr != 0) {
        while (stackPtr != 0) {
            if (i >= length)
                break;
            line[offset++] = gifDecoder.stack[--stackPtr];
            i++;
        }
    }
    while (i < length) {
        int gifCode = ReadGifCode();
        if (gifCode == eofCode) {
            if (i != length - 1 || gifDecoder.pixelCount != 0)
                return;
            i++;
        }
        else {
            if (gifCode == clearCode) {
                for (int p = 0; p <= LZ_MAX_CODE; p++) gifDecoder.prefix[p] = NO_SUCH_CODE;
                gifDecoder.runningCode    = gifDecoder.eofCode + 1;
                gifDecoder.runningBits    = gifDecoder.depth + 1;
                gifDecoder.maxCodePlusOne = 1 << gifDecoder.runningBits;
                prevCode                  = (gifDecoder.prevCode = NO_SUCH_CODE);
            }
            else {
                if (gifCode < clearCode) {
                    line[offset] = (byte)gifCode;
                    offset++;
                    i++;
                }
                else {
                    if (gifCode < 0 || gifCode > LZ_MAX_CODE)
                        return;

                    int code = 0;
                    if (gifDecoder.prefix[gifCode] == NO_SUCH_CODE) {
                        if (gifCode != gifDecoder.runningCode - 2)
                            return;

                        code                                          = prevCode;
                        gifDecoder.suffix[gifDecoder.runningCode - 2] = gifDecoder.stack[stackPtr++] =
                            TraceGifPrefix(gifDecoder.prefix, prevCode, clearCode);
                    }
                    else {
                        code = gifCode;
                    }
                    int c = 0;
                    while (c++ <= LZ_MAX_CODE && code > clearCode && code <= LZ_MAX_CODE) {
                        gifDecoder.stack[stackPtr++] = gifDecoder.suffix[code];
                        code                         = gifDecoder.prefix[code];
                    }
                    if (c >= LZ_MAX_CODE || code > LZ_MAX_CODE)
                        return;

                    gifDecoder.stack[stackPtr++] = (byte)code;
                    while (stackPtr != 0 && i++ < length) line[offset++] = gifDecoder.stack[--stackPtr];
                }
                if (prevCode != NO_SUCH_CODE) {
                    if (gifDecoder.runningCode < 2 || gifDecoder.runningCode > FIRST_CODE)
                        return;

                    gifDecoder.prefix[gifDecoder.runningCode - 2] = prevCode;
                    if (gifCode == gifDecoder.runningCode - 2)
                        gifDecoder.suffix[gifDecoder.runningCode - 2] = TraceGifPrefix(gifDecoder.prefix, prevCode, clearCode);
                    else
                        gifDecoder.suffix[gifDecoder.runningCode - 2] = TraceGifPrefix(gifDecoder.prefix, gifCode, clearCode);
                }
                prevCode = gifCode;
            }
        }
    }
    gifDecoder.prevCode = prevCode;
    gifDecoder.stackPtr = stackPtr;
}

int ReadGifCode()
{
    while (gifDecoder.shiftState < gifDecoder.runningBits) {
        byte b = ReadGifByte();
        gifDecoder.shiftData |= (uint)((uint)b << gifDecoder.shiftState);
        gifDecoder.shiftState += 8;
    }
    int code = (gifDecoder.shiftData & codeMasks[gifDecoder.runningBits]);
    gifDecoder.shiftData >>= gifDecoder.runningBits;
    gifDecoder.shiftState -= gifDecoder.runningBits;
    if (++gifDecoder.runningCode > gifDecoder.maxCodePlusOne && gifDecoder.runningBits < LZ_BITS) {
        gifDecoder.maxCodePlusOne <<= 1;
        gifDecoder.runningBits++;
    }
    return code;
}

byte ReadGifByte()
{
    byte c = 0;
    if (gifDecoder.fileState == LOAD_COMPLETE)
        return c;

    byte b;
    if (gifDecoder.position == gifDecoder.bufferSize) {
        FileRead(&b, 1);
        gifDecoder.bufferSize = (int)b;
        if (gifDecoder.bufferSize == 0) {
            gifDecoder.fileState = LOAD_COMPLETE;
            return c;
        }
        FileRead(gifDecoder.buffer, gifDecoder.bufferSize);
        b                   = gifDecoder.buffer[0];
        gifDecoder.position = 1;
    }
    else {
        b = gifDecoder.buffer[gifDecoder.position++];
    }
    return b;
}

byte TraceGifPrefix(uint *prefix, int code, int clearCode)
{
    int i = 0;
    while (code > clearCode && i++ <= LZ_MAX_CODE) code = prefix[code];

    return code;
}
void ReadGifPictureData(int width, int height, bool interlaced, byte *gfxData, int offset)
{
    int initialRow[] = { 0, 4, 2, 1 };
    int rowInc[]     = { 8, 8, 4, 2 };
    InitGifDecoder();
    if (interlaced) {
        for (int p = 0; p < 4; ++p) {
            for (int y = initialRow[p]; y < height; y += rowInc[p]) ReadGifLine(gfxData, width, y * width + offset);
        }
    }
    else {
        for (int y = 0; y < height; ++y) ReadGifLine(gfxData, width, y * width + offset);
    }
}

int AddGraphicsFile(const char *filePath)
{
    char sheetPath[0x100];

    StrCopy(sheetPath, "Data/Sprites/");
    StrAdd(sheetPath, filePath);
    int sheetID = 0;
    while (StrLength(gfxSurface[sheetID].fileName) > 0) {
        if (StrComp(gfxSurface[sheetID].fileName, sheetPath))
            return sheetID;
        if (++sheetID == SURFACE_COUNT) // Max Sheet cnt
            return 0;
    }
    byte fileExtension = (byte)sheetPath[(StrLength(sheetPath) - 1) & 0xFF];
    switch (fileExtension) {
        case 'f': LoadGIFFile(sheetPath, sheetID); break;
        case 'p': LoadBMPFile(sheetPath, sheetID); break;
        case 'r': LoadPVRFile(sheetPath, sheetID); break;
    }

    return sheetID;
}
void RemoveGraphicsFile(const char *filePath, int sheetID)
{
    if (sheetID < 0) {
        for (int i = 0; i < SURFACE_COUNT; ++i) {
            if (StrLength(gfxSurface[i].fileName) > 0 && StrComp(gfxSurface[i].fileName, filePath))
                sheetID = i;
        }
    }

    if (sheetID >= 0 && StrLength(gfxSurface[sheetID].fileName)) {
        StrCopy(gfxSurface[sheetID].fileName, "");
        int dataPosStart = gfxSurface[sheetID].dataPosition;
        int dataPosEnd   = gfxSurface[sheetID].dataPosition + gfxSurface[sheetID].height * gfxSurface[sheetID].width;
        for (int i = GFXDATA_SIZE - dataPosEnd; i > 0; --i) graphicData[dataPosStart++] = graphicData[dataPosEnd++];
        gfxDataPosition -= gfxSurface[sheetID].height * gfxSurface[sheetID].width;
        for (int i = 0; i < SURFACE_COUNT; ++i) {
            if (gfxSurface[i].dataPosition > gfxSurface[sheetID].dataPosition)
                gfxSurface[i].dataPosition -= gfxSurface[sheetID].height * gfxSurface[sheetID].width;
        }
    }
}

int LoadBMPFile(const char *filePath, byte sheetID)
{
    FileInfo info;
    if (LoadFile(filePath, &info)) {
        GFXSurface *surface = &gfxSurface[sheetID];
        StrCopy(surface->fileName, filePath);

        byte fileBuffer = 0;

        SetFilePosition(18);
        FileRead(&fileBuffer, 1);
        surface->width = fileBuffer;
        FileRead(&fileBuffer, 1);
        surface->width |= fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        surface->width |= fileBuffer << 16;
        FileRead(&fileBuffer, 1);
        surface->width |= fileBuffer << 24;

        FileRead(&fileBuffer, 1);
        surface->height = fileBuffer;
        FileRead(&fileBuffer, 1);
        surface->height |= fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        surface->height |= fileBuffer << 16;
        FileRead(&fileBuffer, 1);
        surface->height |= fileBuffer << 24;

        SetFilePosition(info.vfileSize - surface->height * surface->width);
        surface->dataPosition = gfxDataPosition;
        byte *gfxData         = &graphicData[surface->dataPosition + surface->width * (surface->height - 1)];
        for (int y = 0; y < surface->height; ++y) {
            for (int x = 0; x < surface->width; ++x) {
                FileRead(&fileBuffer, 1);
                *gfxData++ = fileBuffer;
            }
            gfxData -= 2 * surface->width;
        }
        gfxDataPosition += surface->height * surface->width;

#if RETRO_SOFTWARE_RENDER
        surface->widthShift = 0;
        int w               = surface->width;
        while (w > 1) {
            w >>= 1;
            ++surface->widthShift;
        }
#endif

        if (gfxDataPosition >= GFXDATA_SIZE) {
            gfxDataPosition = 0;
            PrintLog("WARNING: Exceeded max gfx size!");
        }

        CloseFile();
        return true;
    }
    return false;
}
int LoadGIFFile(const char *filePath, byte sheetID)
{
    FileInfo info;
    if (LoadFile(filePath, &info)) {
        GFXSurface *surface = &gfxSurface[sheetID];
        StrCopy(surface->fileName, filePath);

        byte fileBuffer = 0;

        SetFilePosition(6); // GIF89a
        FileRead(&fileBuffer, 1);
        surface->width = fileBuffer;
        FileRead(&fileBuffer, 1);
        surface->width |= fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        surface->height = fileBuffer;
        FileRead(&fileBuffer, 1);
        surface->height |= fileBuffer << 8;

        FileRead(&fileBuffer, 1); // Palette Size
        // int has_pallete  = (fileBuffer & 0x80) >> 7;
        // int colors       = ((fileBuffer & 0x70) >> 4) + 1;
        int palette_size = (fileBuffer & 0x7) + 1;
        if (palette_size > 0)
            palette_size = 1 << palette_size;
        FileRead(&fileBuffer, 1); // BG Color index (thrown away)
        FileRead(&fileBuffer, 1); // idk actually (still thrown away)

        int c = 0;
        byte clr[3];
        do {
            ++c;
            FileRead(clr, 3);
        } while (c != palette_size);

        FileRead(&fileBuffer, 1);
        while (fileBuffer != ',') FileRead(&fileBuffer, 1); // gif image start identifier

        ushort fileBuffer2 = 0;
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer, 1);
        bool interlaced = (fileBuffer & 0x40) >> 6;
        if (fileBuffer >> 7 == 1) {
            int c = 0x80;
            do {
                ++c;
                FileRead(clr, 3);
            } while (c != 0x100);
        }

        surface->dataPosition = gfxDataPosition;

#if RETRO_SOFTWARE_RENDER
        surface->widthShift = 0;
        int w               = surface->width;
        while (w > 1) {
            w >>= 1;
            ++surface->widthShift;
        }
#endif

        gfxDataPosition += surface->width * surface->height;
        if (gfxDataPosition < GFXDATA_SIZE) {
            ReadGifPictureData(surface->width, surface->height, interlaced, graphicData, surface->dataPosition);
        }
        else {
            gfxDataPosition = 0;
            PrintLog("WARNING: Exceeded max gfx size!");
        }

        CloseFile();
        return true;
    }
    return false;
}
int LoadPVRFile(const char *filePath, byte sheetID)
{
    // ONLY READS "PVRTC 2bpp RGB" PVR FILES
    FileInfo info;
    if (LoadFile(filePath, &info)) {
        GFXSurface *surface = &gfxSurface[sheetID];
        StrCopy(surface->fileName, filePath);

        byte fileBuffer[2];

        SetFilePosition(28);
        FileRead(fileBuffer, 1);
        int width = fileBuffer[0];
        FileRead(fileBuffer, 1);
        width |= fileBuffer[0] << 8;
        FileRead(fileBuffer, 1);
        int height = fileBuffer[0];
        FileRead(fileBuffer, 1);
        height = fileBuffer[0] << 8;

        surface->width        = width;
        surface->height       = height;
        surface->dataPosition = gfxDataPosition;
        gfxDataPosition += surface->width * surface->height;

        if (gfxDataPosition >= GFXDATA_SIZE) {
            gfxDataPosition = 0;
            PrintLog("WARNING: Exceeded max gfx size!");
        }

#if RETRO_SOFTWARE_RENDER
        surface->widthShift = 0;
        int w               = surface->width;
        while (w > 1) {
            w >>= 1;
            ++surface->widthShift;
        }
#endif

        return false; // yeah I have no clue how to handle this, cd lite has this be loaded every frame on framebuffer update and does it that way

        ushort *buffer = NULL;
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                FileRead(fileBuffer, 2);
                buffer[w] = 2 * (fileBuffer[0] + (fileBuffer[1] << 8)) | 1;
            }
            buffer += width;
        }
        buffer += 0x400 - width;

        CloseFile();
        return true;
    }
    return false;
}
