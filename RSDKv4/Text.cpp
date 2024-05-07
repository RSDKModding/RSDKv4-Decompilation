#include "RetroEngine.hpp"

TextMenu gameMenu[TEXTMENU_COUNT];
int textMenuSurfaceNo = 0;

char playerListText[0x80][0x20];

BitmapFont fontList[FONTLIST_COUNT];

#if !RETRO_REV02
FontCharacter fontCharacterList[FONTCHAR_COUNT];

void LoadFontFile(const char *filePath)
{
    byte fileBuffer = 0;
    int cnt         = 0;
    FileInfo info;
    if (LoadFile(filePath, &info)) {
        while (!ReachedEndOfFile()) {
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].id = fileBuffer;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].id += fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].id += fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].id += fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].srcX = fileBuffer;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].srcX += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].srcY = fileBuffer;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].srcY += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].width = fileBuffer;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].width += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].height = fileBuffer;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].height += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].pivotX = fileBuffer;
            FileRead(&fileBuffer, 1);
            if (fileBuffer > 0x80) {
                fontCharacterList[cnt].pivotX += (fileBuffer - 0x80) << 8;
                fontCharacterList[cnt].pivotX += -0x8000;
            }
            else {
                fontCharacterList[cnt].pivotX += fileBuffer << 8;
            }

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].pivotY = fileBuffer;
            FileRead(&fileBuffer, 1);
            if (fileBuffer > 0x80) {
                fontCharacterList[cnt].pivotY += (fileBuffer - 0x80) << 8;
                fontCharacterList[cnt].pivotY += -0x8000;
            }
            else {
                fontCharacterList[cnt].pivotY += fileBuffer << 8;
            }

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].xAdvance = fileBuffer;
            FileRead(&fileBuffer, 1);
            if (fileBuffer > 0x80) {
                fontCharacterList[cnt].xAdvance += (fileBuffer - 0x80) << 8;
                fontCharacterList[cnt].xAdvance += -0x8000;
            }
            else {
                fontCharacterList[cnt].xAdvance += fileBuffer << 8;
            }

            // Unused
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            cnt++;
        }
        CloseFile();
    }
}
#endif

void LoadTextFile(TextMenu *menu, const char *filePath, byte mapCode)
{
    FileInfo info;
    byte fileBuffer = 0;
    if (LoadFile(filePath, &info)) {
        menu->textDataPos                = 0;
        menu->rowCount                   = 0;
        menu->entryStart[menu->rowCount] = menu->textDataPos;
        menu->entrySize[menu->rowCount]  = 0;

#if !RETRO_REV02
        bool flag = false;
        FileRead(&fileBuffer, 1);
        if (fileBuffer == 0xFF) {
            FileRead(&fileBuffer, 1);
            while (!flag) {
                ushort val = 0;
                FileRead(&fileBuffer, 1);
                val = fileBuffer;
                FileRead(&fileBuffer, 1);
                val |= fileBuffer << 8;

                if (val != '\n') {
                    if (val == '\r') {
                        menu->rowCount += 1;
                        if (menu->rowCount > 511) {
                            flag = true;
                        }
                        else {
                            menu->entryStart[menu->rowCount] = menu->textDataPos;
                            menu->entrySize[menu->rowCount]  = 0;
                        }
                    }
                    else {
                        if (mapCode) {
                            int i = 0;
                            while (i < 1024) {
                                if (fontCharacterList[i].id == val) {
                                    val = i;
                                    i   = 1025;
                                }
                                else {
                                    ++i;
                                }
                            }
                            if (i == 1024) {
                                val = 0;
                            }
                        }
                        menu->textData[menu->textDataPos++] = val;
                        menu->entrySize[menu->rowCount]++;
                    }
                }
                if (!flag) {
                    flag = ReachedEndOfFile();
                    if (menu->textDataPos >= TEXTDATA_COUNT)
                        flag = true;
                }
            }
        }
        else {
            ushort val = fileBuffer;
            if (val != '\n') {
                if (val == '\r') {
                    menu->rowCount++;
                    menu->entryStart[menu->rowCount] = menu->textDataPos;
                    menu->entrySize[menu->rowCount]  = 0;
                }
                else {
                    if (mapCode) {
                        int i = 0;
                        while (i < 1024) {
                            if (fontCharacterList[i].id == val) {
                                val = i;
                                i   = 1025;
                            }
                            else {
                                ++i;
                            }
                        }
                        if (i == 1024) {
                            val = 0;
                        }
                    }
                    menu->textData[menu->textDataPos++] = val;
                    menu->entrySize[menu->rowCount]++;
                }
            }

            while (!flag) {
                FileRead(&fileBuffer, 1);
                val = fileBuffer;
                if (val != '\n') {
                    if (val == '\r') {
                        menu->rowCount++;
                        if (menu->rowCount > 511) {
                            flag = true;
                        }
                        else {
                            menu->entryStart[menu->rowCount] = menu->textDataPos;
                            menu->entrySize[menu->rowCount]  = 0;
                        }
                    }
                    else {
                        if (mapCode) {
                            int i = 0;
                            while (i < 1024) {
                                if (fontCharacterList[i].id == val) {
                                    val = i;
                                    i   = 1025;
                                }
                                else {
                                    ++i;
                                }
                            }
                            if (i == 1024)
                                val = 0;
                        }
                        menu->textData[menu->textDataPos++] = val;
                        menu->entrySize[menu->rowCount]++;
                    }
                }
                if (!flag) {
                    flag = ReachedEndOfFile();
                    if (menu->textDataPos >= TEXTDATA_COUNT)
                        flag = true;
                }
            }
        }
#else
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
#endif

        menu->rowCount++;
        CloseFile();
    }
}
void SetupTextMenu(TextMenu *menu, int rowCount)
{
    menu->textDataPos = 0;
    menu->rowCount    = rowCount;
}
void AddTextMenuEntry(TextMenu *menu, const char *text)
{
    menu->entryStart[menu->rowCount]     = menu->textDataPos;
    menu->entrySize[menu->rowCount]      = 0;
    menu->entryHighlight[menu->rowCount] = false;
    int textLength                       = StrLength(text);
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
    menu->entryStart[menu->rowCount]     = menu->textDataPos;
    menu->entrySize[menu->rowCount]      = 0;
    menu->entryHighlight[menu->rowCount] = false;
    int textLength                       = StrLengthW(text);
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
    menu->entryStart[rowID]              = menu->textDataPos;
    menu->entrySize[rowID]               = 0;
    menu->entryHighlight[menu->rowCount] = false;
    int textLength                       = StrLength(text);
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
    menu->entryStart[rowID]              = menu->textDataPos;
    menu->entrySize[rowID]               = 0;
    menu->entryHighlight[menu->rowCount] = false;
    int textLength                       = StrLengthW(text);
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
    int entryPos                         = menu->entryStart[rowID];
    menu->entrySize[rowID]               = 0;
    menu->entryHighlight[menu->rowCount] = false;
    int textLength                       = StrLength(text);
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
            // Var Name
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;

            // Var Value
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
                StrCopy(playerListText[menu->rowCount], strBuf);
            }
        }

        // Categories
        int entryID = 0;
        for (byte c = 1; c <= 4; ++c) {
            byte stageCnt = 0;
            FileRead(&stageCnt, 1);
            for (byte s = 0; s < stageCnt; ++s) {
                // Stage Folder
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = '\0';

                // Stage ID
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = '\0';

                // Stage Name
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = '\0';

                // IsHighlighted
                FileRead(&fileBuffer, 1);
                if (listNo == c) {
                    AddTextMenuEntry(menu, strBuf);
                    menu->entryHighlight[menu->rowCount - 1] = fileBuffer;
                }
            }
        }
        CloseFile();

#if RETRO_USE_MOD_LOADER
        if (listNo == 0)
            Engine.LoadXMLPlayers(menu);
        else
            Engine.LoadXMLStages(menu, listNo);
#endif
    }
}

void LoadBitmapFont(const char *filePath, int index, char textureID)
{
    FileInfo info;

    BitmapFont *entry = &fontList[index];
    if (!entry->count)
        entry->count = 2;
    if (LoadFile(filePath, &info)) {
        char lineBuffer[256];
        char buffer[32];
        int num = 0;
        int pos = 0;

        ReadStringLine(lineBuffer);
        ReadStringLine(lineBuffer);

        int lineHeightPos = FindStringToken(lineBuffer, "lineHeight=", 1);
        int basePos       = FindStringToken(lineBuffer, "base=", 1);
        int scaleWPos     = FindStringToken(lineBuffer, "scaleW=", 1);

        pos = 0;
        if (lineHeightPos + 11 >= basePos) {
            pos = 0;
        }
        else {
            pos = basePos - 11 - lineHeightPos;
            memcpy(buffer, &lineBuffer[lineHeightPos + 11], pos);
        }
        buffer[pos] = 0;
        if (!ConvertStringToInteger(buffer, &num))
            num = 0;
        if (fontList[index].lineHeight < 1.0)
            fontList[index].lineHeight = num;

        if (basePos + 5 >= scaleWPos) {
            pos = 0;
        }
        else {
            pos = scaleWPos - 5 - basePos;
            memcpy(buffer, &lineBuffer[basePos + 5], pos);
        }
        buffer[pos] = 0;
        if (!ConvertStringToInteger(buffer, &num))
            num = 0;
        if (fontList[index].base < 1.0)
            fontList[index].base = num;

        ReadStringLine(lineBuffer);
        ReadStringLine(lineBuffer);

        int countPos = FindStringToken(lineBuffer, "count=", 1);

        pos = 0;
        if (lineBuffer[countPos + 6]) {
            char *str = &lineBuffer[countPos + 6];
            do {
                buffer[pos++] = *str;
                str++;
            } while (*str);
        }
        buffer[pos] = 0;

        ConvertStringToInteger(buffer, &num);

        int start = entry->count;
        entry->count += num;

        for (int c = start; c < entry->count; ++c) {
            BitmapFontCharacter *character = &fontList[index].characters[c];
            ReadStringLine(lineBuffer);

            int idPos   = FindStringToken(lineBuffer, "id=", 1);
            int xPos    = FindStringToken(lineBuffer, "x=", 1);
            int yPos    = FindStringToken(lineBuffer, "y=", 1);
            int wPos    = FindStringToken(lineBuffer, "width=", 1);
            int hPos    = FindStringToken(lineBuffer, "height=", 1);
            int xOffPos = FindStringToken(lineBuffer, "xoffset=", 1);
            int yOffPos = FindStringToken(lineBuffer, "yoffset=", 1);
            int xAdvPos = FindStringToken(lineBuffer, "xadvance=", 1);
            int pagePos = FindStringToken(lineBuffer, "page=", 1);

            character->textureID = textureID;

            // ID
            pos = 0;
            if (idPos + 3 >= xPos) {
                pos = 0;
            }
            else {
                pos = xPos - 3 - idPos;
                memcpy(buffer, &lineBuffer[idPos + 3], pos);
            }
            buffer[pos] = 0;
            if (!ConvertStringToInteger(buffer, &num))
                num = 0;
            character->id = num;

            // X
            pos = 0;
            if (xPos + 2 >= yPos) {
                pos = 0;
            }
            else {
                pos = yPos - 2 - xPos;
                memcpy(buffer, &lineBuffer[xPos + 2], pos);
            }
            buffer[pos] = 0;
            if (!ConvertStringToInteger(buffer, &num))
                num = 0;
            character->x = num;

            // Y
            pos = 0;
            if (yPos + 2 >= wPos) {
                pos = 0;
            }
            else {
                pos = wPos - 2 - yPos;
                memcpy(buffer, &lineBuffer[yPos + 2], pos);
            }
            buffer[pos] = 0;
            if (!ConvertStringToInteger(buffer, &num))
                num = 0;
            character->y = num;

            // Width
            pos = 0;
            if (wPos + 6 >= hPos) {
                pos = 0;
            }
            else {
                pos = hPos - 6 - wPos;
                memcpy(buffer, &lineBuffer[wPos + 6], pos);
            }
            buffer[pos] = 0;
            if (!ConvertStringToInteger(buffer, &num))
                num = 0;
            character->width = num;

            // Height
            pos = 0;
            if (hPos + 7 >= xOffPos) {
                pos = 0;
            }
            else {
                pos = xOffPos - 7 - hPos;
                memcpy(buffer, &lineBuffer[hPos + 7], pos);
            }
            buffer[pos] = 0;
            if (!ConvertStringToInteger(buffer, &num))
                num = 0;
            character->height = num;

            // XOffset
            pos = 0;
            if (xOffPos + 8 >= yOffPos) {
                pos = 0;
            }
            else {
                pos = yOffPos - 8 - xOffPos;
                memcpy(buffer, &lineBuffer[xOffPos + 8], pos);
            }
            buffer[pos] = 0;
            if (!ConvertStringToInteger(buffer, &num))
                num = 0;
            character->xOffset = num;

            // YOffset
            pos = 0;
            if (yOffPos + 8 >= xAdvPos) {
                pos = 0;
            }
            else {
                pos = xAdvPos - 8 - yOffPos;
                memcpy(buffer, &lineBuffer[yOffPos + 8], pos);
            }
            buffer[pos] = 0;
            if (!ConvertStringToInteger(buffer, &num))
                num = 0;
            character->yOffset = num;

            // XAdvance
            pos = 0;
            if (xAdvPos + 9 >= pagePos) {
                pos = 0;
            }
            else {
                pos = pagePos - 9 - xAdvPos;
                memcpy(buffer, &lineBuffer[xAdvPos + 9], pos);
            }
            buffer[pos] = 0;
            if (!ConvertStringToInteger(buffer, &num))
                num = 0;
            character->xAdvance = num;
        }

        CloseFile();
    }
}

void ResetBitmapFonts()
{
    for (int i = 0; i < FONTLIST_COUNT; ++i) {
        fontList[i].count = 2; // none & newline
    }
}

float GetTextWidth(ushort *text, int fontID, float scaleX)
{
    float width      = 0.0;
    ushort character = *text++;
    float lineMax    = 0.0;
    float w          = 0.0;
    while (character) {
        w += fontList[fontID].characters[character].xAdvance;
        if (character == 1) {
            if (w > lineMax)
                lineMax = w;
            w = 0.0;
        }
        character = *text++;
    }

    width = fmaxf(w, lineMax);
    return width * scaleX;
}

float GetTextHeight(ushort *text, int fontID, float scaleY)
{
    float height     = 0.0;
    ushort character = *text++;
    while (character) {
        if (character == 1) {
            height += fontList[fontID].lineHeight;
        }
        character = *text++;
    }
    return height * scaleY;
}

void SetStringToFont(ushort *text, ushort *string, int fontID)
{
    ushort stringChar = *string++;

    int textPos = 0;
    while (stringChar) {
        ushort charID = 0;
        while (!charID && stringChar) {
            if (stringChar != '\n') {
                if (stringChar == '\r') {
                    charID = 1;
                }
                else {
                    for (int i = 2; i < FONTLIST_CHAR_COUNT; ++i) {
                        if (fontList[fontID].characters[i].id == stringChar) {
                            charID = i;
                            break;
                        }
                    }
                }
            }

            stringChar = *string++;
        }
        text[textPos++] = charID;
    }
    text[textPos] = 0;
}

void SetStringToFont8(ushort *text, const char *string, int fontID)
{
    char stringChar = *string++;

    int textPos = 0;
    while (stringChar) {
        ushort charID = 0;
        while (!charID && stringChar) {
            if (stringChar != '\n') {
                if (stringChar == '\r') {
                    charID = 1;
                }
                else {
                    for (int i = 2; i < FONTLIST_CHAR_COUNT; ++i) {
                        if (fontList[fontID].characters[i].id == stringChar) {
                            charID = i;
                            break;
                        }
                    }
                }
            }

            stringChar = *string++;
        }
        text[textPos++] = charID;
    }
    text[textPos] = 0;
}

void AddTimeStringToFont(ushort *text, int time, int fontID)
{
    char textBuffer[0x40];
    char numBuffer[0x20];

    int minutes      = time / 6000;
    int seconds      = time / 100 % 60;
    int milliseconds = time % 100;

    if (time / 6000) {
        StrCopy(textBuffer, " ");
        ConvertIntegerToString(numBuffer, minutes);
        StrAdd(textBuffer, numBuffer);
    }
    else {
        StrCopy(textBuffer, " 0");
    }
    StrAdd(textBuffer, "'");
    if (seconds) {
        if (seconds <= 9)
            StrAdd(textBuffer, "0");
        ConvertIntegerToString(numBuffer, seconds);
        StrAdd(textBuffer, numBuffer);
    }
    else {
        StrAdd(textBuffer, "00");
    }
    StrAdd(textBuffer, "\"");

    if (milliseconds) {
        if (milliseconds <= 9)
            StrAdd(textBuffer, "0");
        ConvertIntegerToString(numBuffer, milliseconds);
        StrAdd(textBuffer, numBuffer);
    }
    else {
        StrAdd(textBuffer, "00");
    }

    while (*text) {
        if (*text == 1)
            *text = 2;
        text++;
    }

    SetStringToFont8(text, textBuffer, fontID);
}