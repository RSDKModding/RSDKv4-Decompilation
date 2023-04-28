#ifndef STRING_H
#define STRING_H

#define STRSTORAGE_SIZE (1000)
#define STRING_SIZE     (0x400)

#define CREDITS_LIST_COUNT (0x200)

enum TextAlignments { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT };

#define USE_STDLIB

extern ushort *strPressStart;
extern ushort *strTouchToStart;
extern ushort *strStartGame;
extern ushort *strTimeAttack;
extern ushort *strAchievements;
extern ushort *strLeaderboards;
extern ushort *strHelpAndOptions;
extern ushort *strSoundTest;
extern ushort *str2PlayerVS;
extern ushort *strSaveSelect;
extern ushort *strPlayerSelect;
extern ushort *strNoSave;
extern ushort *strNewGame;
extern ushort *strDelete;
extern ushort *strDeleteMessage;
extern ushort *strYes;
extern ushort *strNo;
extern ushort *strSonic;
extern ushort *strTails;
extern ushort *strKnuckles;
extern ushort *strPause;
extern ushort *strContinue;
extern ushort *strRestart;
extern ushort *strExit;
extern ushort *strDevMenu;
extern ushort *strRestartMessage;
extern ushort *strExitMessage;
extern ushort *strNSRestartMessage;
extern ushort *strNSExitMessage;
extern ushort *strExitGame;
extern ushort *strNetworkMessage;
extern ushort *strStageList[16];
extern ushort *strSaveStageList[32];
extern ushort *strNewBestTime;
extern ushort *strRecords;
extern ushort *strNextAct;
extern ushort *strPlay;
extern ushort *strTotalTime;
extern ushort *strInstructions;
extern ushort *strSettings;
extern ushort *strStaffCredits;
extern ushort *strAbout;
extern ushort *strMusic;
extern ushort *strSoundFX;
extern ushort *strSpindash;
extern ushort *strBoxArt;
extern ushort *strControls;
extern ushort *strOn;
extern ushort *strOff;
extern ushort *strCustomizeDPad;
extern ushort *strDPadSize;
extern ushort *strDPadOpacity;
extern ushort *strHelpText1;
extern ushort *strHelpText2;
extern ushort *strHelpText3;
extern ushort *strHelpText4;
extern ushort *strHelpText5;
extern ushort *strVersionName;
extern ushort *strPrivacy;
extern ushort *strTerms;

extern int stageStrCount;

extern ushort stringStorage[STRSTORAGE_SIZE * STRING_SIZE];
extern int stringStorePos;

extern int creditsListSize;
extern ushort *strCreditsList[CREDITS_LIST_COUNT];
extern byte creditsType[CREDITS_LIST_COUNT];
extern float creditsAdvanceY[CREDITS_LIST_COUNT];

inline void StrCopy(char *dest, const char *src)
{
#ifdef USE_STDLIB
    strcpy(dest, src);
#else
    int i = 0;
    for (; src[i]; ++i) dest[i] = src[i];
    dest[i] = 0;
#endif
}

inline void StrAdd(char *dest, const char *src)
{
    int destStrPos = 0;
    int srcStrPos  = 0;
    while (dest[destStrPos]) ++destStrPos;
    while (true) {
        if (!src[srcStrPos]) {
            break;
        }
        dest[destStrPos++] = src[srcStrPos++];
    }
    dest[destStrPos] = 0;
}

inline bool StrComp(const char *stringA, const char *stringB)
{
    bool match    = true;
    bool finished = false;
    while (!finished) {
        if (*stringA == *stringB || *stringA == *stringB + ' ' || *stringA == *stringB - ' ') {
            if (*stringA) {
                ++stringA;
                ++stringB;
            }
            else {
                finished = true;
            }
        }
        else {
            match    = false;
            finished = true;
        }
    }
    return match;
}

inline int StrLength(const char *string)
{
#ifdef USE_STDLIB
    return (int)strlen(string);
#else
    int len = 0;
    for (len = 0; string[len]; len++)
        ;
    return len;
#endif
}
int FindStringToken(const char *string, const char *token, char stopID);
int FindLastStringToken(const char *string, const char *token);

inline void StrCopyW(ushort *dest, const ushort *src)
{
    int i = 0;
    for (; src[i]; ++i) dest[i] = src[i];
    dest[i] = 0;
}

inline void StrAddW(ushort *dest, const ushort *src)
{
    int destStrPos = 0;
    int srcStrPos  = 0;
    while (dest[destStrPos]) ++destStrPos;
    while (true) {
        if (!src[srcStrPos]) {
            break;
        }
        dest[destStrPos++] = src[srcStrPos++];
    }
    dest[destStrPos] = 0;
}
inline void StrCopyW(ushort *dest, const char *src)
{
    int i = 0;
    for (; src[i]; ++i) dest[i] = src[i];
    dest[i] = 0;
}

inline void StrAddW(ushort *dest, const char *src)
{
    int destStrPos = 0;
    int srcStrPos  = 0;
    while (dest[destStrPos]) ++destStrPos;
    while (true) {
        if (!src[srcStrPos]) {
            break;
        }
        dest[destStrPos++] = src[srcStrPos++];
    }
    dest[destStrPos] = 0;
}

inline bool StrCompW(const ushort *stringA, const ushort *stringB)
{
    bool match    = true;
    bool finished = false;
    while (!finished) {
        if (*stringA == *stringB || *stringA == *stringB + ' ' || *stringA == *stringB - ' ') {
            if (*stringA) {
                ++stringA;
                ++stringB;
            }
            else {
                finished = true;
            }
        }
        else {
            match    = false;
            finished = true;
        }
    }
    return match;
}

inline bool StrCompW(const ushort *stringA, const char *stringB)
{
    bool match    = true;
    bool finished = false;
    while (!finished) {
        if (*stringA == *stringB || *stringA == *stringB + ' ' || *stringA == *stringB - ' ') {
            if (*stringA) {
                ++stringA;
                ++stringB;
            }
            else {
                finished = true;
            }
        }
        else {
            match    = false;
            finished = true;
        }
    }
    return match;
}

inline int StrLengthW(const ushort *string)
{
    int len = 0;
    for (len = 0; string[len]; len++)
        ;
    return len;
}

int FindStringTokenUnicode(const ushort *string, const ushort *token, char stopID);

inline void StringLowerCase(char *dest, const char *src)
{
    int destPos = 0;
    int curChar = *src;
    if (*src) {
        int srcPos = 0;
        do {
            while (curChar - 'A' <= 0x19u) {
                destPos       = srcPos;
                dest[destPos] = curChar + ' ';
                curChar       = src[++srcPos];
                if (!curChar) {
                    dest[++destPos] = 0;
                    return;
                }
            }
            destPos       = srcPos;
            dest[destPos] = curChar;
            curChar       = src[++srcPos];
        } while (curChar);
    }
    dest[++destPos] = 0;
}

inline void StringUpperCase(char *dest, const char *src)
{
    int destPos = 0;
    int curChar = *src;
    if (*src) {
        int srcPos = 0;
        do {
            while (curChar - 'a' <= 0x19u) {
                destPos       = srcPos;
                dest[destPos] = curChar - ' ';
                curChar       = src[++srcPos];
                if (!curChar) {
                    dest[++destPos] = 0;
                    return;
                }
            }
            destPos       = srcPos;
            dest[destPos] = curChar;
            curChar       = src[++srcPos];
        } while (curChar);
    }
    dest[++destPos] = 0;
}

void ConvertIntegerToString(char *text, int value);

void GenerateMD5FromString(const char *string, int len, byte *buffer);

void InitLocalizedStrings();
ushort *ReadLocalizedString(const char *stringName, const char *language, const char *filePath);

inline void ReadStringLine(char *text)
{
    char curChar = 0;

    int textPos = 0;
    while (true) {
        FileRead(&curChar, 1);

        if (curChar == '\r' || curChar == '\n')
            break;
        if (curChar != ';' && curChar != '\t' && curChar != ' ')
            text[textPos++] = curChar;

        if (ReachedEndOfFile()) {
            text[textPos] = 0;
            return;
        }
    }
    if (curChar != '\n' && curChar != '\r') {
        if (ReachedEndOfFile()) {
            text[textPos] = 0;
            return;
        }
    }

    text[textPos] = 0;
    if (ReachedEndOfFile())
        text[textPos] = 0;
}

inline void ReadStringLineUnicode(ushort *text)
{
    int curChar = 0;
    byte fileBuffer[2];

    int textPos = 0;
    while (true) {
        FileRead(fileBuffer, 2);
        curChar = fileBuffer[0] + (fileBuffer[1] << 8);
        if (curChar != ' ' && curChar != '\t') {
            if (curChar == '\r') {
                int pos = (int)GetFilePosition();
                FileRead(fileBuffer, 2);
                curChar = fileBuffer[0] + (fileBuffer[1] << 8);
                if (curChar == '\n')
                    break;
                SetFilePosition(pos);
            }
            if (curChar != ';')
                text[textPos++] = curChar;
        }
        else if (curChar == '\n' || curChar == '\r')
            break;

        if (ReachedEndOfFile()) {
            text[textPos] = 0;
            return;
        }
    }
    text[textPos] = 0;
    if (ReachedEndOfFile())
        text[textPos] = 0;
}

void ReadCreditsList(const char *filePath);
inline void ReadCreditsLine(char *line)
{
    byte fileBuffer = 0;

    int strPos = 0;
    while (true) {
        FileRead(&fileBuffer, 1);
        if (fileBuffer == '\r')
            break;
        line[strPos++] = fileBuffer;
        if (ReachedEndOfFile()) {
            line[strPos] = 0;
            return;
        }
    }
    FileRead(&fileBuffer, 1);
    line[strPos] = 0;
    if (ReachedEndOfFile()) {
        line[strPos] = 0;
        return;
    }
}

#endif // !STRING_H
