#include "RetroEngine.hpp"
#include <string>

RSDKContainer rsdkContainer;
char rsdkName[0x400];

char fileName[0x100];
byte fileBuffer[0x2000];
int fileSize;
int vFileSize;
int readPos;
int readSize;
int bufferPosition;
int virtualFileOffset;
bool useEncryption;
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

    Engine.usingDataFile = false;
    Engine.usingBytecode = false;

    // CopyFilePath(filename, &rsdkName);
    cFileHandle = fOpen(filePath, "rb");
    if (cFileHandle) {
        byte signature[6] = { 'R', 'S', 'D', 'K', 'v', 'B' };
        byte buf          = 0;
        for (int i = 0; i < 6; ++i) {
            fRead(&buf, 1, 1, cFileHandle);
            if (buf != signature[i])
                return false;
        }
        Engine.usingDataFile = true;
        StrCopy(rsdkName, filePath);

        rsdkContainer.fileCount = 0;
        fRead(&rsdkContainer.fileCount, 2, 1, cFileHandle);
        for (int f = 0; f < rsdkContainer.fileCount; ++f) {
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

            rsdkContainer.files[f].fileID = f;
        }

        fClose(cFileHandle);
        cFileHandle = NULL;
        if (LoadFile("ByteCode/GlobalCode.bin", &info)) {
            Engine.usingBytecode = true;
            CloseFile();
        }
        return true;
    }
    else {
        Engine.usingDataFile = false;
        cFileHandle = NULL;
        if (LoadFile("ByteCode/GlobalCode.bin", &info)) {
            Engine.usingBytecode = true;
            CloseFile();
        }
        return false;
    }

    return false;
}

bool LoadFile(const char *filePath, FileInfo *fileInfo)
{
    MEM_ZEROP(fileInfo);
    StringLowerCase(fileInfo->fileName, filePath);
    StrCopy(fileName, fileInfo->fileName);

    if (cFileHandle)
        fClose(cFileHandle);

    cFileHandle = NULL;
    if (Engine.usingDataFile) {
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

            cFileHandle = fOpen(rsdkName, "rb");
            fSeek(cFileHandle, 0, SEEK_END);
            fileSize       = (int)fTell(cFileHandle);

            vFileSize = file->filesize;
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
#if RSDK_DEBUG
            printLog("Loaded File '%s'", filePath);
#endif
            return true;
        }
#if RSDK_DEBUG
        printLog("Couldn't load file '%s'", filePath);
#endif
        return false;
    }
    else {
        cFileHandle = fOpen(fileInfo->fileName, "rb");
        if (!cFileHandle) {
#if RSDK_DEBUG
            printLog("Couldn't load file '%s'", filePath);
#endif
            return false;
        }
        virtualFileOffset = 0;
        fSeek(cFileHandle, 0, SEEK_END);
        fileInfo->fileSize = (int)fTell(cFileHandle);
        fileSize = fileInfo->vfileSize = fileInfo->fileSize;
        fSeek(cFileHandle, 0, SEEK_SET);
        readPos = 0;
        fileInfo->readPos           = readPos;
        bufferPosition              = 0;
        readSize                    = 0;

#if RSDK_DEBUG
        printLog("Loaded File '%s'", filePath);
#endif

        return true;
    }
}

void GenerateELoadKeys(uint key1, uint key2)
{
    char buffer[0x20];
    byte hash[0x10];

    //StringA
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
    memcpy(encryptionStringA, fileInfo->encryptionStringA, 0x10 * sizeof(byte));
    memcpy(encryptionStringB, fileInfo->encryptionStringB, 0x10 * sizeof(byte));
}

void SetFileInfo(FileInfo *fileInfo)
{
    if (Engine.usingDataFile) {
        cFileHandle       = fOpen(rsdkName, "rb");
        virtualFileOffset = fileInfo->virtualFileOffset;
        vFileSize         = fileInfo->vfileSize;
        fSeek(cFileHandle, 0, SEEK_END);
        fileSize = (int)fTell(cFileHandle);
        readPos  = fileInfo->readPos;
        fSeek(cFileHandle, readPos, SEEK_SET);
        FillFileBuffer();
        bufferPosition = fileInfo->bufferPosition;
        eStringPosA    = fileInfo->eStringPosA;
        eStringPosB    = fileInfo->eStringPosB;
        eStringNo      = fileInfo->eStringNo;
        eNybbleSwap    = fileInfo->eNybbleSwap;
        useEncryption  = fileInfo->useEncryption;

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
        bufferPosition = fileInfo->bufferPosition;
        eStringPosA    = 0;
        eStringPosB    = 0;
        eStringNo      = 0;
        eNybbleSwap    = 0;
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

size_t FileRead2(FileInfo *info, void *dest, int size)
{
    byte *data = (byte *)dest;
    int rPos   = (int)GetFilePosition2(info);
    memset(data, 0, size);

    if (rPos <= info->vfileSize) {
        if (info->useEncryption) {
            int rSize = 0;
            if (rPos + size <= info->vfileSize)
                rSize = size;
            else
                rSize = info->vfileSize - rPos;

            size_t result = fRead(data, 1u, rSize, info->cFileHandle);
            info->readPos += rSize;
            info->bufferPosition = 0;

            while (size > 0) {
                *data = info->encryptionStringB[info->eStringPosB] ^ info->eStringNo ^ *data;
                if (info->eNybbleSwap)
                    *data = 16 * (*data & 0xF) + (*data >> 4);
                *data ^= info->encryptionStringA[info->eStringPosA];

                ++info->eStringPosA;
                ++info->eStringPosB;
                if (info->eStringPosA <= 0x0F) {
                    if (info->eStringPosB > 0x0C) {
                        info->eStringPosB = 0;
                        info->eNybbleSwap ^= 0x01;
                    }
                }
                else if (info->eStringPosB <= 0x08) {
                    info->eStringPosA = 0;
                    info->eNybbleSwap ^= 0x01;
                }
                else {
                    info->eStringNo += 2;
                    info->eStringNo &= 0x7F;

                    if (info->eNybbleSwap != 0) {
                        int key1    = mulUnsignedHigh(ENC_KEY_1, info->eStringNo);
                        int key2    = mulUnsignedHigh(ENC_KEY_2, info->eStringNo);
                        info->eNybbleSwap = 0;

                        int temp1 = key2 + (info->eStringNo - key2) / 2;
                        int temp2 = key1 / 8 * 3;

                        info->eStringPosA = info->eStringNo - temp1 / 4 * 7;
                        info->eStringPosB = info->eStringNo - temp2 * 4 + 2;
                    }
                    else {
                        int key1    = mulUnsignedHigh(ENC_KEY_1, info->eStringNo);
                        int key2    = mulUnsignedHigh(ENC_KEY_2, info->eStringNo);
                        info->eNybbleSwap = 1;

                        int temp1 = key2 + (info->eStringNo - key2) / 2;
                        int temp2 = key1 / 8 * 3;

                        info->eStringPosB = info->eStringNo - temp1 / 4 * 7;
                        info->eStringPosA = info->eStringNo - temp2 * 4 + 3;
                    }
                }
                ++data;
                --size;
            }
            return result;
        }
        else {
            int rSize = 0;
            if (rPos + size <= info->vfileSize)
                rSize = size;
            else
                rSize = info->vfileSize - rPos;

            size_t result = fRead(data, 1u, rSize, info->cFileHandle);
            info->readPos += rSize;
            info->bufferPosition = 0;
            return result;
        }
    }
    return 0;
}

size_t GetFilePosition2(FileInfo* info)
{
    if (Engine.usingDataFile)
        return info->bufferPosition + info->readPos - info->virtualFileOffset;
    else
        return info->bufferPosition + info->readPos;
}

void SetFilePosition2(FileInfo *info, int newPos)
{
    if (info->useEncryption) {
        info->readPos     = info->virtualFileOffset + newPos;
        info->eStringNo   = (info->vfileSize & 0x1FCu) >> 2;
        info->eStringPosB = (info->eStringNo % 9) + 1;
        info->eStringPosA = (info->eStringNo % info->eStringPosB) + 1;
        info->eNybbleSwap = false;
        while (newPos) {
            ++info->eStringPosA;
            ++info->eStringPosB;
            if (info->eStringPosA <= 0x0F) {
                if (info->eStringPosB > 0x0C) {
                    info->eStringPosB = 0;
                    info->eNybbleSwap ^= 0x01;
                }
            }
            else if (info->eStringPosB <= 0x08) {
                info->eStringPosA = 0;
                info->eNybbleSwap ^= 0x01;
            }
            else {
                info->eStringNo += 2;
                info->eStringNo &= 0x7F;

                if (info->eNybbleSwap != 0) {
                    int key1          = mulUnsignedHigh(ENC_KEY_1, info->eStringNo);
                    int key2          = mulUnsignedHigh(ENC_KEY_2, info->eStringNo);
                    info->eNybbleSwap = false;

                    int temp1 = key2 + (info->eStringNo - key2) / 2;
                    int temp2 = key1 / 8 * 3;

                    info->eStringPosA = info->eStringNo - temp1 / 4 * 7;
                    info->eStringPosB = info->eStringNo - temp2 * 4 + 2;
                }
                else {
                    int key1          = mulUnsignedHigh(ENC_KEY_1, info->eStringNo);
                    int key2          = mulUnsignedHigh(ENC_KEY_2, info->eStringNo);
                    info->eNybbleSwap = true;

                    int temp1 = key2 + (info->eStringNo - key2) / 2;
                    int temp2 = key1 / 8 * 3;

                    info->eStringPosB = info->eStringNo - temp1 / 4 * 7;
                    info->eStringPosA = info->eStringNo - temp2 * 4 + 3;
                }
            }
            --newPos;
        }
    }
    else {
        if (Engine.usingDataFile)
            info->readPos = info->virtualFileOffset + newPos;
        else
            info->readPos = newPos;
    }
    fSeek(info->cFileHandle, info->readPos, SEEK_SET);
}
