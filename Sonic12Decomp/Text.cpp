#include "RetroEngine.hpp"

TextMenu gameMenu[TEXTMENU_COUNT];
int textMenuSurfaceNo = 0;

char playerListText[0x80][0x20];

void LoadTextFile(TextMenu *menu, const char *filePath)
{
    FileInfo info;
    byte fileBuffer = 0;
    if (LoadFile(filePath, &info)) {
        menu->textDataPos                = 0;
        menu->rowCount                   = 0;
        menu->entryStart[menu->rowCount] = menu->textDataPos;
        menu->entrySize[menu->rowCount]  = 0;

        while (menu->textDataPos < TEXTDATA_COUNT && !ReachedEndOfFile()) {
            FileRead(&fileBuffer, 1);
            if (fileBuffer != '\n') {
                if (fileBuffer == '\r') {
                    menu->rowCount++;
                    menu->entryStart[menu->rowCount] = menu->textDataPos;
                    menu->entrySize[menu->rowCount]  = 0;
                }
                else {
                    menu->textData[menu->textDataPos++] = fileBuffer;
                    menu->entrySize[menu->rowCount]++;
                }
            }
        }

        menu->rowCount++;
        CloseFile();
    }
}
void SetupTextMenu(TextMenu *menu, int rowCount)
{
    menu->textDataPos = 0;
    menu->rowCount     = rowCount;
}
void AddTextMenuEntry(TextMenu *menu, const char *text)
{
    menu->entryStart[menu->rowCount] = menu->textDataPos;
    menu->entrySize[menu->rowCount]  = 0;
    int textLength                   = StrLength(text);
    for (int i = 0; i < textLength;) {
        if (text[i] != '\0') {
            menu->textData[menu->textDataPos++] = text[i];
            menu->entrySize[menu->rowCount]++;
            ++i;
        }
        else {
            break;
        }
    }
    menu->rowCount++;
}
void AddTextMenuEntryW(TextMenu *menu, const ushort *text)
{
    menu->entryStart[menu->rowCount] = menu->textDataPos;
    menu->entrySize[menu->rowCount]  = 0;
    int textLength                   = StrLengthW(text);
    for (int i = 0; i < textLength;) {
        if (text[i] != '\0') {
            menu->textData[menu->textDataPos++] = text[i];
            menu->entrySize[menu->rowCount]++;
            ++i;
        }
        else {
            break;
        }
    }
    menu->rowCount++;
}
void SetTextMenuEntry(TextMenu *menu, const char *text, int rowID)
{
    menu->entryStart[rowID] = menu->textDataPos;
    menu->entrySize[rowID]  = 0;
    int textLength          = StrLength(text);
    for (int i = 0; i < textLength;) {
        if (text[i] != '\0') {
            menu->textData[menu->textDataPos++] = text[i];
            menu->entrySize[rowID]++;
            ++i;
        }
        else {
            break;
        }
    }
}
void SetTextMenuEntryW(TextMenu *menu, const ushort *text, int rowID)
{
    menu->entryStart[rowID] = menu->textDataPos;
    menu->entrySize[rowID]  = 0;
    int textLength          = StrLengthW(text);
    for (int i = 0; i < textLength;) {
        if (text[i] != '\0') {
            menu->textData[menu->textDataPos++] = text[i];
            menu->entrySize[rowID]++;
            ++i;
        }
        else {
            break;
        }
    }
}
void EditTextMenuEntry(TextMenu *menu, const char *text, int rowID)
{
    int entryPos             = menu->entryStart[rowID];
    menu->entrySize[rowID] = 0;
    int textLength         = StrLength(text);
    for (int i = 0; i < textLength;) {
        if (text[i] != '\0') {
            menu->textData[entryPos++] = text[i];
            menu->entrySize[rowID]++;
            ++i;
        }
        else {
            break;
        }
    }
}
void LoadConfigListText(TextMenu *menu, int listNo)
{
    FileInfo info;
    char strBuf[0x100];
    byte fileBuffer = 0;
    byte count      = 0;
    byte strLen     = 0;
    if (LoadFile("Data/Game/GameConfig.bin", &info)) {
        // Name
        FileRead(&strLen, 1);
        FileRead(&strBuf, strLen);
        strBuf[strLen] = 0;

        // About
        FileRead(&strLen, 1);
        FileRead(&strBuf, strLen);
        strBuf[strLen] = 0;

        byte buf[3];
        for (int c = 0; c < 0x60; ++c) FileRead(buf, 3);

        // Object Names
        FileRead(&count, 1);
        for (byte o = 0; o < count; ++o) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
        }

        // Script Paths
        for (byte s = 0; s < count; ++s) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
        }

        // Variables
        FileRead(&count, 1);
        for (byte v = 0; v < count; ++v) {
            //Var Name
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;

            //Var Value
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
        }

        // SFX Names
        FileRead(&count, 1);
        for (byte s = 0; s < count; ++s) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
        }
        // SFX Paths
        for (byte s = 0; s < count; ++s) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
        }

        // Players
        FileRead(&count, 1);
        for (byte p = 0; p < count; ++p) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = '\0';

            if (listNo == 0) { // Player List
                AddTextMenuEntry(menu, strBuf);
                StrCopy(playerListText[p], strBuf);
            }
        }

        // Categories
        for (byte c = 1; c <= 4; ++c) {
            int stageCnt = 0;
            FileRead(&stageCnt, 1);
            for (byte s = 0; s < stageCnt; ++s) {
                //Stage Folder
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = 0;

                //Stage ID
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = 0;

                //Stage Name
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = '\0';

                //IsHighlighted
                FileRead(&fileBuffer, 1);
                if (listNo == c) {
                    menu->entryHighlight[s] = fileBuffer;
                    AddTextMenuEntry(menu, strBuf);
                }
            }
        }
        CloseFile();
    }
}