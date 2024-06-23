#ifndef READER_H
#define READER_H

#ifdef FORCE_CASE_INSENSITIVE

#include "fcaseopen.h"
#define FileIO                                          FILE
#define fOpen(path, mode)                               fcaseopen(path, mode)
#define fRead(buffer, elementSize, elementCount, file)  fread(buffer, elementSize, elementCount, file)
#define fSeek(file, offset, whence)                     fseek(file, offset, whence)
#define fTell(file)                                     ftell(file)
#define fClose(file)                                    fclose(file)
#define fWrite(buffer, elementSize, elementCount, file) fwrite(buffer, elementSize, elementCount, file)

#else

#if RETRO_USING_SDL1 || RETRO_USING_SDL2
#define FileIO                                          SDL_RWops
#define fOpen(path, mode)                               SDL_RWFromFile(path, mode)
#define fRead(buffer, elementSize, elementCount, file)  SDL_RWread(file, buffer, elementSize, elementCount)
#define fSeek(file, offset, whence)                     SDL_RWseek(file, offset, whence)
#define fTell(file)                                     SDL_RWtell(file)
#define fClose(file)                                    SDL_RWclose(file)
#define fWrite(buffer, elementSize, elementCount, file) SDL_RWwrite(file, buffer, elementSize, elementCount)
#else
#define FileIO                                          FILE
#define fOpen(path, mode)                               fopen(path, mode)
#define fRead(buffer, elementSize, elementCount, file)  fread(buffer, elementSize, elementCount, file)
#define fSeek(file, offset, whence)                     fseek(file, offset, whence)
#define fTell(file)                                     ftell(file)
#define fClose(file)                                    fclose(file)
#define fWrite(buffer, elementSize, elementCount, file) fwrite(buffer, elementSize, elementCount, file)
#endif

#endif

#define RETRO_PACKFILE_COUNT (0x1000)
#define RETRO_PACK_COUNT     (0x4)

struct FileInfo {
    char fileName[0x100];
    int fileSize;
    int vfileSize;
    int readPos;
    int bufferPosition;
    int virtualFileOffset;
    byte eStringPosA;
    byte eStringPosB;
    byte eStringNo;
    byte eNybbleSwap;
    bool useEncryption;
    byte packID;
    byte encryptionStringA[0x10];
    byte encryptionStringB[0x10];
#if !RETRO_USE_ORIGINAL_CODE
    FileIO *cFileHandle;
    bool usingDataPack;
#endif
};

struct RSDKFileInfo {
    uint hash[4];
    int offset;
    int filesize;
    bool encrypted;
    byte packID;
};

struct RSDKContainer {
    RSDKFileInfo files[RETRO_PACKFILE_COUNT];
    char packNames[RETRO_PACK_COUNT][0x400];
    int fileCount;
    int packCount;
};

extern RSDKContainer rsdkContainer;

extern char fileName[0x100];
extern byte fileBuffer[0x2000];
extern int fileSize;
extern int vFileSize;
extern int readPos;
extern int readSize;
extern int bufferPosition;
extern int virtualFileOffset;
extern bool useEncryption;
extern byte packID;
extern byte eStringPosA;
extern byte eStringPosB;
extern byte eStringNo;
extern byte eNybbleSwap;
extern byte encryptionStringA[0x10];
extern byte encryptionStringB[0x10];

extern FileIO *cFileHandle;

inline void CopyFilePath(char *dest, const char *src)
{
    strcpy(dest, src);
    for (int i = 0;; ++i) {
        if (i >= strlen(dest)) {
            break;
        }

        if (dest[i] == '/')
            dest[i] = '\\';
    }
}
bool CheckRSDKFile(const char *filePath);
inline void CloseRSDKContainers()
{
    for (int i = 0; i < 4; ++i) {
        strcpy(rsdkContainer.packNames[i], "");
    }
    rsdkContainer.packCount = 0;
    rsdkContainer.fileCount = 0;
}

#if !RETRO_USE_ORIGINAL_CODE
int CheckFileInfo(const char *filepath);
#endif

bool LoadFile(const char *filePath, FileInfo *fileInfo);
inline bool CloseFile()
{
    int result = 0;
    if (cFileHandle)
        result = fClose(cFileHandle);

    cFileHandle = NULL;
    return result;
}

void GenerateELoadKeys(uint key1, uint key2);

void FileRead(void *dest, int size);
void FileSkip(int count);

inline size_t FillFileBuffer()
{
    if (readPos + 0x2000 <= fileSize)
        readSize = 0x2000;
    else
        readSize = fileSize - readPos;

    size_t result = fRead(fileBuffer, 1u, readSize, cFileHandle);
    readPos += readSize;
    bufferPosition = 0;
    return result;
}

void GetFileInfo(FileInfo *fileInfo);
void SetFileInfo(FileInfo *fileInfo);
size_t GetFilePosition();
void SetFilePosition(int newPos);
bool ReachedEndOfFile();

#endif // !READER_H
