#include "RetroEngine.hpp"

#include <upng.h>

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

struct GifDecoder gifDecoder;
int codeMasks[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095 };

int textureCount = 0;
texInfo textureList[TEXTURE_MAX];

int ReadGifCode();
byte ReadGifByte();
byte TraceGifPrefix(uint *prefix, int code, int clearCode);

void InitGifDecoder()
{
    byte val = 0;
    FileRead(&val, 1);
    gifDecoder.fileState      = LOADING_IMAGE;
    gifDecoder.position       = 0;
    gifDecoder.bufferSize     = 0;
    gifDecoder.buffer[0]      = 0;
    gifDecoder.depth          = val;
    gifDecoder.clearCode      = 1 << val;
    gifDecoder.eofCode        = gifDecoder.clearCode + 1;
    gifDecoder.runningCode    = gifDecoder.eofCode + 1;
    gifDecoder.runningBits    = val + 1;
    gifDecoder.maxCodePlusOne = 1 << gifDecoder.runningBits;
    gifDecoder.stackPtr       = 0;
    gifDecoder.prevCode       = NO_SUCH_CODE;
    gifDecoder.shiftState     = 0;
    gifDecoder.shiftData      = 0u;
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
            if (i >= length) {
                break;
            }
            line[offset++] = gifDecoder.stack[--stackPtr];
            i++;
        }
    }
    while (i < length) {
        int gifCode = ReadGifCode();
        if (gifCode == eofCode) {
            if (i != length - 1 | gifDecoder.pixelCount != 0u) {
                return;
            }
            i++;
        }
        else {
            if (gifCode == clearCode) {
                for (int j = 0; j <= LZ_MAX_CODE; j++) {
                    gifDecoder.prefix[j] = NO_SUCH_CODE;
                }
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
                    if (gifCode<0 | gifCode> LZ_MAX_CODE) {
                        return;
                    }
                    int code;
                    if (gifDecoder.prefix[gifCode] == NO_SUCH_CODE) {
                        if (gifCode != gifDecoder.runningCode - 2) {
                            return;
                        }
                        code = prevCode;
                        gifDecoder.suffix[gifDecoder.runningCode - 2] =
                            (gifDecoder.stack[stackPtr++] = TraceGifPrefix(gifDecoder.prefix, prevCode, clearCode));
                    }
                    else {
                        code = gifCode;
                    }
                    int c = 0;
                    while (c++ <= LZ_MAX_CODE && code > clearCode && code <= LZ_MAX_CODE) {
                        gifDecoder.stack[stackPtr++] = gifDecoder.suffix[code];
                        code                         = gifDecoder.prefix[code];
                    }
                    if (c >= LZ_MAX_CODE | code > LZ_MAX_CODE) {
                        return;
                    }
                    gifDecoder.stack[stackPtr++] = (byte)code;
                    while (stackPtr != 0 && i++ < length) {
                        line[offset++] = gifDecoder.stack[--stackPtr];
                    }
                }
                if (prevCode != NO_SUCH_CODE) {
                    if (gifDecoder.runningCode<2 | gifDecoder.runningCode> FIRST_CODE) {
                        return;
                    }
                    gifDecoder.prefix[gifDecoder.runningCode - 2] = prevCode;
                    if (gifCode == gifDecoder.runningCode - 2) {
                        gifDecoder.suffix[gifDecoder.runningCode - 2] = TraceGifPrefix(gifDecoder.prefix, prevCode, clearCode);
                    }
                    else {
                        gifDecoder.suffix[gifDecoder.runningCode - 2] = TraceGifPrefix(gifDecoder.prefix, gifCode, clearCode);
                    }
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
    int result = (int)((unsigned long)gifDecoder.shiftData & (unsigned long)(codeMasks[gifDecoder.runningBits]));
    gifDecoder.shiftData >>= gifDecoder.runningBits;
    gifDecoder.shiftState -= gifDecoder.runningBits;
    if (++gifDecoder.runningCode > gifDecoder.maxCodePlusOne && gifDecoder.runningBits < LZ_BITS) {
        gifDecoder.maxCodePlusOne <<= 1;
        gifDecoder.runningBits++;
    }
    return result;
}

byte ReadGifByte()
{
    byte c = '\0';
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
    int array[]  = { 0, 4, 2, 1 };
    int array2[] = { 8, 8, 4, 2 };
    InitGifDecoder();
    if (interlaced) {
        for (int i = 0; i < 4; ++i) {
            for (int j = array[i]; j < height; j += array2[i]) {
                ReadGifLine(gfxData, width, j * width + offset);
            }
        }
        return;
    }
    for (int h = 0; h < height; ++h) ReadGifLine(gfxData, width, h * width + offset);
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
        if (++sheetID == SURFACE_MAX) // Max Sheet cnt
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
        for (int i = 0; i < SURFACE_MAX; ++i) {
            if (StrLength(gfxSurface[i].fileName) > 0 && StrComp(gfxSurface[i].fileName, filePath))
                sheetID = i;
        }
    }

    if (sheetID >= 0 && StrLength(gfxSurface[sheetID].fileName)) {
        StrCopy(gfxSurface[sheetID].fileName, "");
        int dataPosStart = gfxSurface[sheetID].dataPosition;
        int dataPosEnd   = gfxSurface[sheetID].dataPosition + gfxSurface[sheetID].height * gfxSurface[sheetID].width;
        for (int i = GFXDATA_MAX - dataPosEnd; i > 0; --i) graphicData[dataPosStart++] = graphicData[dataPosEnd++];
        gfxDataPosition -= gfxSurface[sheetID].height * gfxSurface[sheetID].width;
        for (int i = 0; i < SURFACE_MAX; ++i) {
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
        surface->width += fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        surface->width += fileBuffer << 16;
        FileRead(&fileBuffer, 1);
        surface->width += fileBuffer << 24;

        FileRead(&fileBuffer, 1);
        surface->height = fileBuffer;
        FileRead(&fileBuffer, 1);
        surface->height += fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        surface->height += fileBuffer << 16;
        FileRead(&fileBuffer, 1);
        surface->height += fileBuffer << 24;

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

        surface->widthShift = 0;
        int w               = surface->width;
        while (w > 1) {
            w >>= 1;
            ++surface->widthShift;
        }

        if (gfxDataPosition >= GFXDATA_MAX) {
            gfxDataPosition = 0;
            printLog("WARNING: Exceeded max gfx size!");
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
        surface->width += (fileBuffer << 8);
        FileRead(&fileBuffer, 1);
        surface->height = fileBuffer;
        FileRead(&fileBuffer, 1);
        surface->height += (fileBuffer << 8);

        FileRead(&fileBuffer, 1); // Palette Size (thrown away) :/
        FileRead(&fileBuffer, 1); // BG Colour index (thrown away)
        FileRead(&fileBuffer, 1); // idk actually (still thrown away)

        int c = 0;
        byte clr[3];
        do {
            ++c;
            FileRead(clr, 3);
        } while (c != 0x100);

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
        surface->widthShift   = 0;
        int w                 = surface->width;
        while (w > 1) {
            w >>= 1;
            ++surface->widthShift;
        }

        gfxDataPosition += surface->width * surface->height;
        if (gfxDataPosition < GFXDATA_MAX) {
            ReadGifPictureData(surface->width, surface->height, interlaced, graphicData, surface->dataPosition);
        }
        else {
            gfxDataPosition = 0;
            printLog("WARNING: Exceeded max gfx size!");
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
        width += fileBuffer[0] << 8;
        FileRead(fileBuffer, 1);
        int height = fileBuffer[0];
        FileRead(fileBuffer, 1);
        height = fileBuffer[0] << 8;

        surface->width        = width;
        surface->height       = height;
        surface->dataPosition = gfxDataPosition;
        gfxDataPosition += surface->width * surface->height;

        if (gfxDataPosition >= GFXDATA_MAX) {
            gfxDataPosition = 0;
            printLog("WARNING: Exceeded max gfx size!");
        }

        surface->widthShift = 0;
        int w               = surface->width;
        while (w > 1) {
            w >>= 1;
            ++surface->widthShift;
        }

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

// 0 = data, 1 = menu, 0xFF = no datafile
int LoadTexture(const char *filePath, byte dMode)
{

    texInfo *texture = nullptr;

    for (int i = 0; i < textureCount; ++i) {
        if (StrComp(filePath, textureList[i].fileName))
            return i;
    }

    int texID = textureCount;
    if (texID >= TEXTURE_MAX) {
        return 0;
    }

    if (!textureList[texID].tex) {
        texture = &textureList[texID];
    }
    if (!texture)
        return 0;
    textureCount++;

    int dataStore = Engine.usingDataFile;
    if (dMode == 0xFF)
        Engine.usingDataFile = false;
    else
        snapDataFile(dMode);

#if RETRO_USING_SDL1
        // Returns SDL_Surface*
#endif

#if RETRO_USING_SDL2
    // Returns SDL_Texture*

    FileInfo info;

    if (LoadFile(filePath, &info)) {

        byte *fileData = (byte *)malloc(info.vfileSize);
        FileRead(fileData, info.vfileSize);

        upng_t *upng = upng_new_from_bytes(fileData, info.vfileSize);
        upng_decode(upng);

        if (dMode == 0xFF)
            Engine.usingDataFile = dataStore;

        if (upng_get_error(upng) != UPNG_EOK) {
            char errorText[9][0x30] = { "No error",
                                        "Memory allocation Failed",
                                        "Resource not found",
                                        "Invalid or missing PNG header",
                                        "Invalid image data",
                                        "Critical type not supported",
                                        "Interlacing not supported",
                                        "Colour format not supported",
                                        "Invalid parameter to method call" };
            byte error              = upng_get_error(upng);
            char buf[0x80];
            sprintf(buf, "PNG error: \"%s\" at line: %d", errorText[error], upng_get_error_line(upng));
            printLog(buf);
            free(fileData);
            upng_free(upng);
            snapDataFile(0);
            return 0;
        }

        int width  = upng_get_width(upng);
        int height = upng_get_height(upng);
        int format = upng_get_format(upng);

        Uint32 rmask = 0, gmask = 0, bmask = 0, amask = 0;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        amask = 0x000000FF;
        rmask = 0x0000FF00;
        gmask = 0x00FF0000;
        bmask = 0xFF000000;
#else
        amask = 0xFF000000;
        rmask = 0x00FF0000;
        gmask = 0x0000FF00;
        bmask = 0x000000FF;
#endif

        SDL_Surface *img = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);

        byte *dst = (byte *)img->pixels;
        byte *src = (byte *)upng_get_buffer(upng);

        int dstID  = 0;
        int bufPos = 0;
        for (int p = 0; p < width * height; p++) {
            byte A = 0xFF, R = 0xFF, G = 0, B = 0xFF;
            int buffer    = 0;
            int buffer2   = 0;
            float fBuffer = 0.0f;
            switch (format) {
                case UPNG_RGB8:
                    R = src[bufPos++]; // R
                    G = src[bufPos++]; // G
                    B = src[bufPos++]; // B
                    break;
                case UPNG_RGBA8:
                    R = src[bufPos++]; // R
                    G = src[bufPos++]; // G
                    B = src[bufPos++]; // B
                    A = src[bufPos++]; // A
                    break;
                case UPNG_RGB16:
                    buffer  = src[bufPos++];
                    buffer2 = src[bufPos++];
                    fBuffer = (buffer + (buffer2 << 8)) / 65536.0f;
                    R       = (int)floor(fBuffer * 0xFF);

                    buffer  = src[bufPos++];
                    buffer2 = src[bufPos++];
                    fBuffer = (buffer + (buffer2 << 8)) / 65536.0f;
                    G       = (int)floor(fBuffer * 0xFF);

                    buffer  = src[bufPos++];
                    buffer2 = src[bufPos++];
                    fBuffer = (buffer + (buffer2 << 8)) / 65536.0f;
                    B       = (int)floor(fBuffer * 0xFF);
                    break;
                case UPNG_RGBA16:
                    buffer  = src[bufPos++];
                    buffer2 = src[bufPos++];
                    fBuffer = (buffer + (buffer2 << 8)) / 65536.0f;
                    R       = (int)floor(fBuffer * 0xFF);

                    buffer  = src[bufPos++];
                    buffer2 = src[bufPos++];
                    fBuffer = (buffer + (buffer2 << 8)) / 65536.0f;
                    G       = (int)floor(fBuffer * 0xFF);

                    buffer  = src[bufPos++];
                    buffer2 = src[bufPos++];
                    fBuffer = (buffer + (buffer2 << 8)) / 65536.0f;
                    B       = (int)floor(fBuffer * 0xFF);

                    buffer  = src[bufPos++];
                    buffer2 = src[bufPos++];
                    fBuffer = (buffer + (buffer2 << 8)) / 65536.0f;
                    A       = (int)floor(fBuffer * 0xFF);
                    break;
                case UPNG_LUMINANCE1: printLog("Unsupported format: Luminace1"); break;
                case UPNG_LUMINANCE2: printLog("Unsupported format: Luminace2"); break;
                case UPNG_LUMINANCE4: printLog("Unsupported format: Luminace4"); break;
                case UPNG_LUMINANCE8: printLog("Unsupported format: Luminace8"); break;
                case UPNG_LUMINANCE_ALPHA1: printLog("Unsupported format: Luminace1A"); break;
                case UPNG_LUMINANCE_ALPHA2: printLog("Unsupported format: Luminace2A"); break;
                case UPNG_LUMINANCE_ALPHA4: printLog("Unsupported format: Luminace4A"); break;
                case UPNG_LUMINANCE_ALPHA8: printLog("Unsupported format: Luminace8A"); break;
            }
            *dst++ = B;
            *dst++ = G;
            *dst++ = R;
            *dst++ = A;
        }

        SDL_SetColorKey(img, 1, 0xFF00FF);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(Engine.renderer, img);

        StrCopy(texture->fileName, filePath);
        texture->tex    = tex;
        texture->width  = width;
        texture->height = height;

        SDL_FreeSurface(img);
        free(fileData);
        upng_free(upng);
        snapDataFile(0);
        return texID;
    }

#endif
    if (dMode == 0xFF)
        Engine.usingDataFile = dataStore;
    else
        snapDataFile(0);

    return 0;
}

void ClearTextures()
{
    for (int i = 0; i < TEXTURE_MAX; ++i) {
        if (textureList[i].tex) {

#if RETRO_USING_SDL1
            SDL_FreeSurface((SDL_Surface *)textureList[i].tex);
#endif

#if RETRO_USING_SDL2
            SDL_DestroyTexture((SDL_Texture *)textureList[i].tex);
#endif
            textureList[i].tex = nullptr;
        }
        StrCopy(textureList[i].fileName, "");
    }
    textureCount = 0;
}