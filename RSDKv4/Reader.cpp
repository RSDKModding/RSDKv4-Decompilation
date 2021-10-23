#include "RetroEngine.hpp"
#include <string>

RSDKContainer rsdkContainer;

char fileName[0x100];
byte fileBuffer[0x2000];
int fileSize          = 0;
int vFileSize         = 0;
int readPos           = 0;
int readSize          = 0;
int bufferPosition    = 0;
int virtualFileOffset = 0;
bool useEncryption    = false;
byte packID           = 0;
byte eStringPosA;
byte eStringPosB;
byte eStringNo;
byte eNybbleSwap;
byte encryptionStringA[0x10];
byte encryptionStringB[0x10];

FileIO *cFileHandle = nullptr;

bool CheckRSDKFile(const char *filePath)
{
    FileInfo info;

    char filePathBuffer[0x100];
    sprintf(filePathBuffer, "%s", filePath);
#if RETRO_PLATFORM == RETRO_OSX
    char pathBuf[0x100];
    sprintf(pathBuf, "%s/%s", gamePath, filePathBuffer);
    sprintf(filePathBuffer, "%s", pathBuf);
#endif

    cFileHandle = fOpen(filePathBuffer, "rb");
    if (cFileHandle) {
        byte signature[6] = { 'R', 'S', 'D', 'K', 'v', 'B' };
        byte buf          = 0;
        for (int i = 0; i < 6; ++i) {
            fRead(&buf, 1, 1, cFileHandle);
            if (buf != signature[i])
                return false;
        }

        Engine.usingDataFile = true;

        StrCopy(rsdkContainer.packNames[rsdkContainer.packCount], filePathBuffer);

        ushort fileCount = 0;
        fRead(&fileCount, 2, 1, cFileHandle);
        for (int f = 0; f < fileCount; ++f) {
            for (int y = 0; y < 16; y += 4) {
                fRead(&rsdkContainer.files[f].hash[y + 3], 1, 1, cFileHandle);
                fRead(&rsdkContainer.files[f].hash[y + 2], 1, 1, cFileHandle);
                fRead(&rsdkContainer.files[f].hash[y + 1], 1, 1, cFileHandle);
                fRead(&rsdkContainer.files[f].hash[y + 0], 1, 1, cFileHandle);
            }

            fRead(&rsdkContainer.files[f].offset, 4, 1, cFileHandle);
            fRead(&rsdkContainer.files[f].filesize, 4, 1, cFileHandle);

            rsdkContainer.files[f].encrypted = (rsdkContainer.files[f].filesize & 0x80000000);
            rsdkContainer.files[f].filesize &= 0x7FFFFFFF;

            rsdkContainer.files[f].packID = rsdkContainer.packCount;

            rsdkContainer.fileCount++;
        }

        fClose(cFileHandle);
        cFileHandle = NULL;
        if (LoadFile("Bytecode/GlobalCode.bin", &info)) {
            Engine.usingBytecode = true;
            CloseFile();
        }
        printLog("loaded datapack '%s'", filePathBuffer);

        rsdkContainer.packCount++;
        return true;
    }
    else {
        Engine.usingDataFile = false;
        cFileHandle          = NULL;

        if (LoadFile("Bytecode/GlobalCode.bin", &info)) {
            Engine.usingBytecode = true;
            CloseFile();
        }
        printLog("Couldn't load datapack '%s'", filePathBuffer);
        return false;
    }
}

#if !RETRO_USE_ORIGINAL_CODE
int CheckFileInfo(const char *filepath)
{
    char pathBuf[0x100];
    StringLowerCase(pathBuf, filepath);
    byte buffer[0x10];
    int len = StrLength(pathBuf);
    GenerateMD5FromString(pathBuf, len, buffer);

    for (int f = 0; f < rsdkContainer.fileCount; ++f) {
        RSDKFileInfo *file = &rsdkContainer.files[f];

        bool match = true;
        for (int h = 0; h < 0x10; ++h) {
            if (buffer[h] != file->hash[h]) {
                match = false;
                break;
            }
        }
        if (!match)
            continue;

        return f;
    }
    return -1;
}
#endif

bool LoadFile(const char *filePath, FileInfo *fileInfo)
{
    MEM_ZEROP(fileInfo);

    if (cFileHandle)
        fClose(cFileHandle);

    char filePathBuf[0x100];
    StrCopy(filePathBuf, filePath);
    bool forceFolder = false;
#if RETRO_USE_MOD_LOADER
    // Fixes ".ani" ".Ani" bug and any other case differences
    char pathLower[0x100];
    memset(pathLower, 0, sizeof(char) * 0x100);
    for (int c = 0; c < strlen(filePathBuf); ++c) {
        pathLower[c] = tolower(filePathBuf[c]);
    }

    bool addPath = true;
    if (activeMod != -1) {
        char buf[0x100];
        sprintf(buf, "%s", filePathBuf);
        sprintf(filePathBuf, "%smods/%s/%s", modsPath, modList[activeMod].folder.c_str(), buf);
        forceFolder = true;
        addPath     = false;
    }
    else {
        for (int m = 0; m < modList.size(); ++m) {
            if (modList[m].active) {
                std::map<std::string, std::string>::const_iterator iter = modList[m].fileMap.find(pathLower);
                if (iter != modList[m].fileMap.cend()) {
                    StrCopy(filePathBuf, iter->second.c_str());
                    forceFolder = true;
                    addPath     = false;
                    break;
                }
            }
        }
    }
#endif

#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
    if (addPath) {
        char pathBuf[0x100];
        sprintf(pathBuf, "%s/%s", gamePath, filePathBuf);
        sprintf(filePathBuf, "%s", pathBuf);
    }
#endif

    cFileHandle = NULL;
#if !RETRO_USE_ORIGINAL_CODE
    if (CheckFileInfo(filePath) != -1 && !forceFolder) {
#else
    if (Engine.usingDataFile) {
#endif
        StringLowerCase(fileInfo->fileName, filePath);
        StrCopy(fileName, fileInfo->fileName);
        byte buffer[0x10];
        int len = StrLength(fileInfo->fileName);
        GenerateMD5FromString(fileInfo->fileName, len, buffer);

        for (int f = 0; f < rsdkContainer.fileCount; ++f) {
            RSDKFileInfo *file = &rsdkContainer.files[f];

            bool match = true;
            for (int h = 0; h < 0x10; ++h) {
                if (buffer[h] != file->hash[h]) {
                    match = false;
                    break;
                }
            }
            if (!match)
                continue;

            packID      = file->packID;
            cFileHandle = fOpen(rsdkContainer.packNames[file->packID], "rb");
            fSeek(cFileHandle, 0, SEEK_END);
            fileSize = (int)fTell(cFileHandle);

            vFileSize         = file->filesize;
            virtualFileOffset = file->offset;
            readPos           = file->offset;
            readSize          = 0;
            bufferPosition    = 0;
            fSeek(cFileHandle, virtualFileOffset, SEEK_SET);

            useEncryption = file->encrypted;
            memset(fileInfo->encryptionStringA, 0, 0x10 * sizeof(byte));
            memset(fileInfo->encryptionStringB, 0, 0x10 * sizeof(byte));
            if (useEncryption) {
                GenerateELoadKeys(vFileSize, (vFileSize >> 1) + 1);
                eStringNo   = (vFileSize & 0x1FC) >> 2;
                eStringPosA = 0;
                eStringPosB = 8;
                eNybbleSwap = 0;
                memcpy(fileInfo->encryptionStringA, encryptionStringA, 0x10 * sizeof(byte));
                memcpy(fileInfo->encryptionStringB, encryptionStringB, 0x10 * sizeof(byte));
            }

            fileInfo->readPos           = readPos;
            fileInfo->fileSize          = fileSize;
            fileInfo->vfileSize         = vFileSize;
            fileInfo->virtualFileOffset = virtualFileOffset;
            fileInfo->eStringNo         = eStringNo;
            fileInfo->eStringPosB       = eStringPosB;
            fileInfo->eStringPosA       = eStringPosA;
            fileInfo->eNybbleSwap       = eNybbleSwap;
            fileInfo->bufferPosition    = bufferPosition;
            fileInfo->useEncryption     = useEncryption;
            fileInfo->packID            = packID;
            fileInfo->usingDataPack     = true;
            printLog("Loaded Data File '%s'", filePath);

#if !RETRO_USE_ORIGINAL_CODE
            Engine.usingDataFile = true;
#endif

            return true;
        }
        printLog("Couldn't load file '%s'", filePath);
        return false;
    }
    else {
        StrCopy(fileInfo->fileName, filePathBuf);
        StrCopy(fileName, fileInfo->fileName);

        cFileHandle = fOpen(fileInfo->fileName, "rb");
        if (!cFileHandle) {
            printLog("Couldn't load file '%s'", filePath);
            return false;
        }
        virtualFileOffset = 0;
        fSeek(cFileHandle, 0, SEEK_END);
        fileInfo->fileSize = (int)fTell(cFileHandle);
        fileSize = fileInfo->vfileSize = fileInfo->fileSize;
        fSeek(cFileHandle, 0, SEEK_SET);
        readPos           = 0;
        fileInfo->readPos = readPos;
        packID = fileInfo->packID = -1;
        fileInfo->usingDataPack   = false;
        bufferPosition            = 0;
        readSize                  = 0;
        useEncryption             = false;

#if !RETRO_USE_ORIGINAL_CODE
        Engine.usingDataFile = false;
#endif

        printLog("Loaded File '%s'", filePath);
        return true;
    }
}

void GenerateELoadKeys(uint key1, uint key2)
{
    char buffer[0x20];
    byte hash[0x10];

    // StringA
    ConvertIntegerToString(buffer, key1);
    int len = StrLength(buffer);
    GenerateMD5FromString(buffer, len, hash);

    for (int y = 0; y < 0x10; y += 4) {
        encryptionStringA[y + 3] = hash[y + 0];
        encryptionStringA[y + 2] = hash[y + 1];
        encryptionStringA[y + 1] = hash[y + 2];
        encryptionStringA[y + 0] = hash[y + 3];
    }

    // StringB
    ConvertIntegerToString(buffer, key2);
    len = StrLength(buffer);
    GenerateMD5FromString(buffer, len, hash);

    for (int y = 0; y < 0x10; y += 4) {
        encryptionStringB[y + 3] = hash[y + 0];
        encryptionStringB[y + 2] = hash[y + 1];
        encryptionStringB[y + 1] = hash[y + 2];
        encryptionStringB[y + 0] = hash[y + 3];
    }
}

const uint ENC_KEY_2 = 0x24924925;
const uint ENC_KEY_1 = 0xAAAAAAAB;
int mulUnsignedHigh(uint arg1, int arg2) { return (int)(((unsigned long long)arg1 * (unsigned long long)arg2) >> 32); }

void FileRead(void *dest, int size)
{
    byte *data = (byte *)dest;
    memset(data, 0, size);

    if (readPos <= fileSize) {
        if (useEncryption) {
            while (size > 0) {
                if (bufferPosition == readSize)
                    FillFileBuffer();

                *data = encryptionStringB[eStringPosB] ^ eStringNo ^ fileBuffer[bufferPosition++];
                if (eNybbleSwap)
                    *data = ((*data << 4) + (*data >> 4)) & 0xFF;
                *data ^= encryptionStringA[eStringPosA];

                ++eStringPosA;
                ++eStringPosB;
                if (eStringPosA <= 0x0F) {
                    if (eStringPosB > 0x0C) {
                        eStringPosB = 0;
                        eNybbleSwap ^= 0x01;
                    }
                }
                else if (eStringPosB <= 0x08) {
                    eStringPosA = 0;
                    eNybbleSwap ^= 0x01;
                }
                else {
                    eStringNo += 2;
                    eStringNo &= 0x7F;

                    if (eNybbleSwap != 0) {
                        int key1    = mulUnsignedHigh(ENC_KEY_1, eStringNo);
                        int key2    = mulUnsignedHigh(ENC_KEY_2, eStringNo);
                        eNybbleSwap = 0;

                        int temp1 = key2 + (eStringNo - key2) / 2;
                        int temp2 = key1 / 8 * 3;

                        eStringPosA = eStringNo - temp1 / 4 * 7;
                        eStringPosB = eStringNo - temp2 * 4 + 2;
                    }
                    else {
                        int key1    = mulUnsignedHigh(ENC_KEY_1, eStringNo);
                        int key2    = mulUnsignedHigh(ENC_KEY_2, eStringNo);
                        eNybbleSwap = 1;

                        int temp1 = key2 + (eStringNo - key2) / 2;
                        int temp2 = key1 / 8 * 3;

                        eStringPosB = eStringNo - temp1 / 4 * 7;
                        eStringPosA = eStringNo - temp2 * 4 + 3;
                    }
                }

                ++data;
                --size;
            }
        }
        else {
            while (size > 0) {
                if (bufferPosition == readSize)
                    FillFileBuffer();

                *data++ = fileBuffer[bufferPosition++];
                size--;
            }
        }
    }
}

void FileSkip(int count)
{
    if (readPos <= fileSize) {
        if (useEncryption) {
            while (count > 0) {
                if (bufferPosition == readSize)
                    FillFileBuffer();
                bufferPosition++;

                ++eStringPosA;
                ++eStringPosB;
                if (eStringPosA <= 0x0F) {
                    if (eStringPosB > 0x0C) {
                        eStringPosB = 0;
                        eNybbleSwap ^= 0x01;
                    }
                }
                else if (eStringPosB <= 0x08) {
                    eStringPosA = 0;
                    eNybbleSwap ^= 0x01;
                }
                else {
                    eStringNo += 2;
                    eStringNo &= 0x7F;

                    if (eNybbleSwap != 0) {
                        int key1    = mulUnsignedHigh(ENC_KEY_1, eStringNo);
                        int key2    = mulUnsignedHigh(ENC_KEY_2, eStringNo);
                        eNybbleSwap = 0;

                        int temp1 = key2 + (eStringNo - key2) / 2;
                        int temp2 = key1 / 8 * 3;

                        eStringPosA = eStringNo - temp1 / 4 * 7;
                        eStringPosB = eStringNo - temp2 * 4 + 2;
                    }
                    else {
                        int key1    = mulUnsignedHigh(ENC_KEY_1, eStringNo);
                        int key2    = mulUnsignedHigh(ENC_KEY_2, eStringNo);
                        eNybbleSwap = 1;

                        int temp1 = key2 + (eStringNo - key2) / 2;
                        int temp2 = key1 / 8 * 3;

                        eStringPosB = eStringNo - temp1 / 4 * 7;
                        eStringPosA = eStringNo - temp2 * 4 + 3;
                    }
                }

                --count;
            }
        }
        else {
            while (count > 0) {
                if (bufferPosition == readSize)
                    FillFileBuffer();
                bufferPosition++;
                count--;
            }
        }
    }
}

void GetFileInfo(FileInfo *fileInfo)
{
    StrCopy(fileInfo->fileName, fileName);
    fileInfo->bufferPosition    = bufferPosition;
    fileInfo->readPos           = readPos - readSize;
    fileInfo->fileSize          = fileSize;
    fileInfo->vfileSize         = vFileSize;
    fileInfo->virtualFileOffset = virtualFileOffset;
    fileInfo->eStringPosA       = eStringPosA;
    fileInfo->eStringPosB       = eStringPosB;
    fileInfo->eStringNo         = eStringNo;
    fileInfo->eNybbleSwap       = eNybbleSwap;
    fileInfo->useEncryption     = useEncryption;
    fileInfo->packID            = packID;
    fileInfo->usingDataPack     = Engine.usingDataFile;
    memcpy(encryptionStringA, fileInfo->encryptionStringA, 0x10 * sizeof(byte));
    memcpy(encryptionStringB, fileInfo->encryptionStringB, 0x10 * sizeof(byte));
}

void SetFileInfo(FileInfo *fileInfo)
{
#if !RETRO_USE_ORIGINAL_CODE
    if (fileInfo->usingDataPack) {
#else
    if (Engine.usingDataFile) {
#endif
        cFileHandle       = fOpen(rsdkContainer.packNames[fileInfo->packID], "rb");
        virtualFileOffset = fileInfo->virtualFileOffset;
        vFileSize         = fileInfo->vfileSize;
        fSeek(cFileHandle, 0, SEEK_END);
        fileSize = (int)fTell(cFileHandle);
        readPos  = fileInfo->readPos;
        fSeek(cFileHandle, readPos, SEEK_SET);
        FillFileBuffer();
        bufferPosition       = fileInfo->bufferPosition;
        eStringPosA          = fileInfo->eStringPosA;
        eStringPosB          = fileInfo->eStringPosB;
        eStringNo            = fileInfo->eStringNo;
        eNybbleSwap          = fileInfo->eNybbleSwap;
        useEncryption        = fileInfo->useEncryption;
        packID               = fileInfo->packID;
        Engine.usingDataFile = fileInfo->usingDataPack;

        if (useEncryption) {
            GenerateELoadKeys(vFileSize, (vFileSize >> 1) + 1);
        }
    }
    else {
        StrCopy(fileName, fileInfo->fileName);
        cFileHandle       = fOpen(fileInfo->fileName, "rb");
        virtualFileOffset = 0;
        fileSize          = fileInfo->fileSize;
        readPos           = fileInfo->readPos;
        fSeek(cFileHandle, readPos, SEEK_SET);
        FillFileBuffer();
        bufferPosition       = fileInfo->bufferPosition;
        eStringPosA          = 0;
        eStringPosB          = 0;
        eStringNo            = 0;
        eNybbleSwap          = 0;
        useEncryption        = fileInfo->useEncryption;
        packID               = fileInfo->packID;
        Engine.usingDataFile = fileInfo->usingDataPack;
    }
}

size_t GetFilePosition()
{
    if (Engine.usingDataFile)
        return bufferPosition + readPos - readSize - virtualFileOffset;
    else
        return bufferPosition + readPos - readSize;
}

void SetFilePosition(int newPos)
{
    if (useEncryption) {
        readPos     = virtualFileOffset + newPos;
        eStringNo   = (vFileSize & 0x1FC) >> 2;
        eStringPosA = 0;
        eStringPosB = 8;
        eNybbleSwap = false;
        while (newPos) {
            ++eStringPosA;
            ++eStringPosB;
            if (eStringPosA <= 0x0F) {
                if (eStringPosB > 0x0C) {
                    eStringPosB = 0;
                    eNybbleSwap ^= 0x01;
                }
            }
            else if (eStringPosB <= 0x08) {
                eStringPosA = 0;
                eNybbleSwap ^= 0x01;
            }
            else {
                eStringNo += 2;
                eStringNo &= 0x7F;

                if (eNybbleSwap != 0) {
                    int key1    = mulUnsignedHigh(ENC_KEY_1, eStringNo);
                    int key2    = mulUnsignedHigh(ENC_KEY_2, eStringNo);
                    eNybbleSwap = 0;

                    int temp1 = key2 + (eStringNo - key2) / 2;
                    int temp2 = key1 / 8 * 3;

                    eStringPosA = eStringNo - temp1 / 4 * 7;
                    eStringPosB = eStringNo - temp2 * 4 + 2;
                }
                else {
                    int key1    = mulUnsignedHigh(ENC_KEY_1, eStringNo);
                    int key2    = mulUnsignedHigh(ENC_KEY_2, eStringNo);
                    eNybbleSwap = 1;

                    int temp1 = key2 + (eStringNo - key2) / 2;
                    int temp2 = key1 / 8 * 3;

                    eStringPosB = eStringNo - temp1 / 4 * 7;
                    eStringPosA = eStringNo - temp2 * 4 + 3;
                }
            }
            --newPos;
        }
    }
    else {
        if (Engine.usingDataFile)
            readPos = virtualFileOffset + newPos;
        else
            readPos = newPos;
    }
    fSeek(cFileHandle, readPos, SEEK_SET);
    FillFileBuffer();
}

bool ReachedEndOfFile()
{
    if (Engine.usingDataFile)
        return bufferPosition + readPos - readSize - virtualFileOffset >= vFileSize;
    else
        return bufferPosition + readPos - readSize >= fileSize;
}
