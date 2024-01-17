#include "RetroEngine.hpp"

ushort *strPressStart       = NULL;
ushort *strTouchToStart     = NULL;
ushort *strStartGame        = NULL;
ushort *strTimeAttack       = NULL;
ushort *strAchievements     = NULL;
ushort *strLeaderboards     = NULL;
ushort *strHelpAndOptions   = NULL;
ushort *strSoundTest        = NULL;
ushort *str2PlayerVS        = NULL;
ushort *strSaveSelect       = NULL;
ushort *strPlayerSelect     = NULL;
ushort *strNoSave           = NULL;
ushort *strNewGame          = NULL;
ushort *strDelete           = NULL;
ushort *strDeleteMessage    = NULL;
ushort *strYes              = NULL;
ushort *strNo               = NULL;
ushort *strSonic            = NULL;
ushort *strTails            = NULL;
ushort *strKnuckles         = NULL;
ushort *strPause            = NULL;
ushort *strContinue         = NULL;
ushort *strRestart          = NULL;
ushort *strExit             = NULL;
ushort *strDevMenu          = NULL;
ushort *strRestartMessage   = NULL;
ushort *strExitMessage      = NULL;
ushort *strNSRestartMessage = NULL;
ushort *strNSExitMessage    = NULL;
ushort *strExitGame         = NULL;
ushort *strNetworkMessage   = NULL;
ushort *strStageList[16];
ushort *strSaveStageList[32];
ushort *strNewBestTime   = NULL;
ushort *strRecords       = NULL;
ushort *strNextAct       = NULL;
ushort *strPlay          = NULL;
ushort *strTotalTime     = NULL;
ushort *strInstructions  = NULL;
ushort *strSettings      = NULL;
ushort *strStaffCredits  = NULL;
ushort *strAbout         = NULL;
ushort *strMusic         = NULL;
ushort *strSoundFX       = NULL;
ushort *strSpindash      = NULL;
ushort *strBoxArt        = NULL;
ushort *strControls      = NULL;
ushort *strOn            = NULL;
ushort *strOff           = NULL;
ushort *strCustomizeDPad = NULL;
ushort *strDPadSize      = NULL;
ushort *strDPadOpacity   = NULL;
ushort *strHelpText1     = NULL;
ushort *strHelpText2     = NULL;
ushort *strHelpText3     = NULL;
ushort *strHelpText4     = NULL;
ushort *strHelpText5     = NULL;
ushort *strVersionName   = NULL;
ushort *strPrivacy       = NULL;
ushort *strTerms         = NULL;

int stageStrCount = 0;

ushort stringStorage[STRSTORAGE_SIZE * STRING_SIZE];
int stringStorePos = 0;

int creditsListSize = 0;
ushort *strCreditsList[CREDITS_LIST_COUNT];
byte creditsType[CREDITS_LIST_COUNT];
float creditsAdvanceY[CREDITS_LIST_COUNT];

// From here: https://rosettacode.org/wiki/MD5#C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef union uwb {
    unsigned w;
    unsigned char b[4];
} WBunion;

typedef unsigned digest[4];

unsigned f0(unsigned abcd[]) { return (abcd[1] & abcd[2]) | (~abcd[1] & abcd[3]); }

unsigned f1(unsigned abcd[]) { return (abcd[3] & abcd[1]) | (~abcd[3] & abcd[2]); }

unsigned f2(unsigned abcd[]) { return abcd[1] ^ abcd[2] ^ abcd[3]; }

unsigned f3(unsigned abcd[]) { return abcd[2] ^ (abcd[1] | ~abcd[3]); }

typedef unsigned (*DgstFctn)(unsigned a[]);

unsigned *calcKs(unsigned *k)
{
    double s, pwr;
    int i;

    pwr = pow(2, 32);
    for (i = 0; i < 64; i++) {
        s    = fabs(sin(1 + i));
        k[i] = (unsigned)(s * pwr);
    }
    return k;
}

// ROtate v Left by amt bits
unsigned rol(unsigned v, short amt)
{
    unsigned msk1 = (1 << amt) - 1;
    return ((v >> (32 - amt)) & msk1) | ((v << amt) & ~msk1);
}

unsigned *md5(const char *msg, int mlen)
{
    static digest h0 = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476 };
    //    static Digest h0 = { 0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210 };
    static DgstFctn ff[] = { &f0, &f1, &f2, &f3 };
    static short M[]     = { 1, 5, 3, 7 };
    static short O[]     = { 0, 1, 5, 0 };
    static short rot0[]  = { 7, 12, 17, 22 };
    static short rot1[]  = { 5, 9, 14, 20 };
    static short rot2[]  = { 4, 11, 16, 23 };
    static short rot3[]  = { 6, 10, 15, 21 };
    static short *rots[] = { rot0, rot1, rot2, rot3 };
    static unsigned kspace[64];
    static unsigned *k;

    static digest h;
    digest abcd;
    DgstFctn fctn;
    short m, o, g;
    unsigned f;
    short *rotn;
    union {
        unsigned w[16];
        char b[64];
    } mm;
    int os = 0;
    int grp, grps, q, p;
    unsigned char *msg2;

    if (k == NULL)
        k = calcKs(kspace);

    for (q = 0; q < 4; q++) h[q] = h0[q]; // initialize

    {
        grps = 1 + (mlen + 8) / 64;
        msg2 = (unsigned char *)malloc(64 * grps);
        memcpy(msg2, msg, mlen);
        msg2[mlen] = (unsigned char)0x80;
        q          = mlen + 1;
        while (q < 64 * grps) {
            msg2[q] = 0;
            q++;
        }
        {
            //            unsigned char t;
            WBunion u;
            u.w = 8 * mlen;
            //            t = u.b[0]; u.b[0] = u.b[3]; u.b[3] = t;
            //            t = u.b[1]; u.b[1] = u.b[2]; u.b[2] = t;
            q -= 8;
            memcpy(msg2 + q, &u.w, 4);
        }
    }

    for (grp = 0; grp < grps; grp++) {
        memcpy(mm.b, msg2 + os, 64);
        for (q = 0; q < 4; q++) abcd[q] = h[q];
        for (p = 0; p < 4; p++) {
            fctn = ff[p];
            rotn = rots[p];
            m    = M[p];
            o    = O[p];
            for (q = 0; q < 16; q++) {
                g = (m * q + o) % 16;
                f = abcd[1] + rol(abcd[0] + fctn(abcd) + k[q + 16 * p] + mm.w[g], rotn[q % 4]);

                abcd[0] = abcd[3];
                abcd[3] = abcd[2];
                abcd[2] = abcd[1];
                abcd[1] = f;
            }
        }
        for (p = 0; p < 4; p++) h[p] += abcd[p];
        os += 64;
    }

    if (msg2)
        free(msg2);

    return h;
}

int FindStringToken(const char *string, const char *token, char stopID)
{
    int tokenCharID  = 0;
    bool tokenMatch  = true;
    int stringCharID = 0;
    int foundTokenID = 0;

    while (string[stringCharID]) {
        tokenCharID = 0;
        tokenMatch  = true;
        while (token[tokenCharID]) {
            if (!string[tokenCharID + stringCharID])
                return -1;

            if (string[tokenCharID + stringCharID] != token[tokenCharID])
                tokenMatch = false;

            ++tokenCharID;
        }
        if (tokenMatch && ++foundTokenID == stopID)
            return stringCharID;

        ++stringCharID;
    }
    return -1;
}

int FindLastStringToken(const char *string, const char *token)
{
    int tokenCharID  = 0;
    bool tokenMatch  = true;
    int stringCharID = 0;
    int foundTokenID = 0;
    int lastResult = -1;

    while (string[stringCharID]) {
        tokenCharID = 0;
        tokenMatch  = true;
        while (token[tokenCharID]) {
            if (!string[tokenCharID + stringCharID])
                return lastResult;

            if (string[tokenCharID + stringCharID] != token[tokenCharID])
                tokenMatch = false;

            ++tokenCharID;
        }
        if (tokenMatch)
            lastResult = stringCharID;

        ++stringCharID;
    }
    return lastResult;
}

int FindStringTokenUnicode(const ushort *string, const ushort *token, char stopID)
{
    int tokenCharID  = 0;
    bool tokenMatch  = true;
    int stringCharID = 0;
    int foundTokenID = 0;

    while (string[stringCharID]) {
        tokenCharID = 0;
        tokenMatch  = true;
        while (token[tokenCharID]) {
            if (!string[tokenCharID + stringCharID])
                return -1;

            if (string[tokenCharID + stringCharID] != token[tokenCharID])
                tokenMatch = false;

            ++tokenCharID;
        }
        if (tokenMatch && ++foundTokenID == stopID)
            return stringCharID;

        ++stringCharID;
    }
    return -1;
}

void ConvertIntegerToString(char *text, int value) { sprintf(text, "%d", value); }

// Buffer is expected to be at least 16 bytes long
void GenerateMD5FromString(const char *string, int len, byte *buffer)
{
    unsigned *d = md5(string, len);
    WBunion u;

    for (int i = 0; i < 4; ++i) {
        u.w = d[i];
        for (int c = 0; c < 4; ++c) buffer[(i << 2) + c] = u.b[c];
    }
}

void InitLocalizedStrings()
{
    memset(stringStorage, 0, STRING_SIZE * STRSTORAGE_SIZE * sizeof(ushort));
    stringStorePos = 0;

    char langStr[0x4];
    switch (Engine.language) {
        case RETRO_EN: StrCopy(langStr, "en"); break;
        case RETRO_FR: StrCopy(langStr, "fr"); break;
        case RETRO_IT: StrCopy(langStr, "it"); break;
        case RETRO_DE: StrCopy(langStr, "de"); break;
        case RETRO_ES: StrCopy(langStr, "es"); break;
        case RETRO_JP: StrCopy(langStr, "ja"); break;
        case RETRO_PT: StrCopy(langStr, "pt"); break;
        case RETRO_RU: StrCopy(langStr, "ru"); break;
        case RETRO_KO: StrCopy(langStr, "ko"); break;
        case RETRO_ZH: StrCopy(langStr, "zh"); break;
        case RETRO_ZS: StrCopy(langStr, "zs"); break;
        default: break;
    }
    strPressStart     = ReadLocalizedString("PressStart", langStr, "Data/Game/StringList.txt");
    strTouchToStart   = ReadLocalizedString("TouchToStart", langStr, "Data/Game/StringList.txt");
    strStartGame      = ReadLocalizedString("StartGame", langStr, "Data/Game/StringList.txt");
    strTimeAttack     = ReadLocalizedString("TimeAttack", langStr, "Data/Game/StringList.txt");
    strAchievements   = ReadLocalizedString("Achievements", langStr, "Data/Game/StringList.txt");
    strLeaderboards   = ReadLocalizedString("Leaderboards", langStr, "Data/Game/StringList.txt");
    strHelpAndOptions = ReadLocalizedString("HelpAndOptions", langStr, "Data/Game/StringList.txt");

    // SoundTest & StageTest, both unused
    strSoundTest = ReadLocalizedString("SoundTest", langStr, "Data/Game/StringList.txt");
    // strStageTest      = ReadLocalizedString("StageTest", langStr, "Data/Game/StringList.txt");

    str2PlayerVS      = ReadLocalizedString("TwoPlayerVS", langStr, "Data/Game/StringList.txt");
    strSaveSelect     = ReadLocalizedString("SaveSelect", langStr, "Data/Game/StringList.txt");
    strPlayerSelect   = ReadLocalizedString("PlayerSelect", langStr, "Data/Game/StringList.txt");
    strNoSave         = ReadLocalizedString("NoSave", langStr, "Data/Game/StringList.txt");
    strNewGame        = ReadLocalizedString("NewGame", langStr, "Data/Game/StringList.txt");
    strDelete         = ReadLocalizedString("Delete", langStr, "Data/Game/StringList.txt");
    strDeleteMessage  = ReadLocalizedString("DeleteSavedGame", langStr, "Data/Game/StringList.txt");
    strYes            = ReadLocalizedString("Yes", langStr, "Data/Game/StringList.txt");
    strNo             = ReadLocalizedString("No", langStr, "Data/Game/StringList.txt");
    strSonic          = ReadLocalizedString("Sonic", langStr, "Data/Game/StringList.txt");
    strTails          = ReadLocalizedString("Tails", langStr, "Data/Game/StringList.txt");
    strKnuckles       = ReadLocalizedString("Knuckles", langStr, "Data/Game/StringList.txt");
    strPause          = ReadLocalizedString("Pause", langStr, "Data/Game/StringList.txt");
    strContinue       = ReadLocalizedString("Continue", langStr, "Data/Game/StringList.txt");
    strRestart        = ReadLocalizedString("Restart", langStr, "Data/Game/StringList.txt");
    strExit           = ReadLocalizedString("Exit", langStr, "Data/Game/StringList.txt");
    strDevMenu        = ReadLocalizedString("DevMenu", "en", "Data/Game/StringList.txt");
    strRestartMessage = ReadLocalizedString("RestartMessage", langStr, "Data/Game/StringList.txt");
    strExitMessage    = ReadLocalizedString("ExitMessage", langStr, "Data/Game/StringList.txt");
    if (Engine.language == RETRO_JP) {
        strNSRestartMessage = ReadLocalizedString("NSRestartMessage", "ja", "Data/Game/StringList.txt");
        strNSExitMessage    = ReadLocalizedString("NSExitMessage", "ja", "Data/Game/StringList.txt");
    }
    else {
        strNSRestartMessage = ReadLocalizedString("RestartMessage", langStr, "Data/Game/StringList.txt");
        strNSExitMessage    = ReadLocalizedString("ExitMessage", langStr, "Data/Game/StringList.txt");
    }
    strExitGame       = ReadLocalizedString("ExitGame", langStr, "Data/Game/StringList.txt");
    strNetworkMessage = ReadLocalizedString("NetworkMessage", langStr, "Data/Game/StringList.txt");
    for (int i = 0; i < 16; ++i) {
        char buffer[0x10];
        sprintf(buffer, "StageName%d", i + 1);

        strStageList[i] = ReadLocalizedString(buffer, "en", "Data/Game/StringList.txt");
    }

    stageStrCount = 0;
    for (int i = 0; i < 32; ++i) {
        char buffer[0x20];
        sprintf(buffer, "SaveStageName%d", i + 1);

        strSaveStageList[i] = ReadLocalizedString(buffer, "en", "Data/Game/StringList.txt");
        if (!strSaveStageList[i])
            break;
        stageStrCount++;
    }
    strNewBestTime   = ReadLocalizedString("NewBestTime", langStr, "Data/Game/StringList.txt");
    strRecords       = ReadLocalizedString("Records", langStr, "Data/Game/StringList.txt");
    strNextAct       = ReadLocalizedString("NextAct", langStr, "Data/Game/StringList.txt");
    strPlay          = ReadLocalizedString("Play", langStr, "Data/Game/StringList.txt");
    strTotalTime     = ReadLocalizedString("TotalTime", langStr, "Data/Game/StringList.txt");
    strInstructions  = ReadLocalizedString("Instructions", langStr, "Data/Game/StringList.txt");
    strSettings      = ReadLocalizedString("Settings", langStr, "Data/Game/StringList.txt");
    strStaffCredits  = ReadLocalizedString("StaffCredits", langStr, "Data/Game/StringList.txt");
    strAbout         = ReadLocalizedString("About", langStr, "Data/Game/StringList.txt");
    strMusic         = ReadLocalizedString("Music", langStr, "Data/Game/StringList.txt");
    strSoundFX       = ReadLocalizedString("SoundFX", langStr, "Data/Game/StringList.txt");
    strSpindash      = ReadLocalizedString("SpinDash", langStr, "Data/Game/StringList.txt");
    strBoxArt        = ReadLocalizedString("BoxArt", langStr, "Data/Game/StringList.txt");
    strControls      = ReadLocalizedString("Controls", langStr, "Data/Game/StringList.txt");
    strOn            = ReadLocalizedString("On", langStr, "Data/Game/StringList.txt");
    strOff           = ReadLocalizedString("Off", langStr, "Data/Game/StringList.txt");
    strCustomizeDPad = ReadLocalizedString("CustomizeDPad", langStr, "Data/Game/StringList.txt");
    strDPadSize      = ReadLocalizedString("DPadSize", langStr, "Data/Game/StringList.txt");
    strDPadOpacity   = ReadLocalizedString("DPadOpacity", langStr, "Data/Game/StringList.txt");
    strHelpText1     = ReadLocalizedString("HelpText1", langStr, "Data/Game/StringList.txt");
    strHelpText2     = ReadLocalizedString("HelpText2", langStr, "Data/Game/StringList.txt");
    strHelpText3     = ReadLocalizedString("HelpText3", langStr, "Data/Game/StringList.txt");
    strHelpText4     = ReadLocalizedString("HelpText4", langStr, "Data/Game/StringList.txt");
    strHelpText5     = ReadLocalizedString("HelpText5", langStr, "Data/Game/StringList.txt");
    strVersionName   = ReadLocalizedString("Version", langStr, "Data/Game/StringList.txt");
    strPrivacy       = ReadLocalizedString("Privacy", langStr, "Data/Game/StringList.txt");
    strTerms         = ReadLocalizedString("Terms", langStr, "Data/Game/StringList.txt");
    // strMoreGames         = ReadLocalizedString("MoreGames", langStr, "Data/Game/StringList.txt");

    // Video Filter options
    // strVideoFilter       = ReadLocalizedString("VideoFilter", langStr, "Data/Game/StringList.txt");
    // strSharp             = ReadLocalizedString("Sharp", langStr, "Data/Game/StringList.txt");
    // strSmooth            = ReadLocalizedString("Smooth", langStr, "Data/Game/StringList.txt");
    // strNostalgic         = ReadLocalizedString("Nostalgic", langStr, "Data/Game/StringList.txt");

    // Login With Facebook
    // strFBLogin = ReadLocalizedString("LoginWithFacebook", langStr, "Data/Game/StringList.txt");

    // Unused Control Modes
    // strControlMethod = ReadLocalizedString("ControlMethod", langStr, "Data/Game/StringList.txt");
    // strSwipeAndTap   = ReadLocalizedString("SwipeAndTap", langStr, "Data/Game/StringList.txt");
    // strVirtualDPad   = ReadLocalizedString("VirtualDPad", langStr, "Data/Game/StringList.txt");

    ReadCreditsList("Data/Game/CreditsMobile.txt");
}
ushort *ReadLocalizedString(const char *stringName, const char *language, const char *filePath)
{
    FileInfo info;
    ushort strName[0x40];
    ushort langName[0x8];
    ushort lineBuffer[0x200];

    memset(strName, 0, 0x40 * sizeof(ushort));
    memset(langName, 0, 0x8 * sizeof(ushort));
    memset(lineBuffer, 0, 0x200 * sizeof(ushort));

    int strNamePos = 0;
    while (stringName[strNamePos]) {
        strName[strNamePos] = stringName[strNamePos];
        strNamePos++;
    }
    strName[strNamePos++] = ':';
    strName[strNamePos]   = 0;

    int langNamePos = 0;
    for (langNamePos = 0; langNamePos < 4; ++langNamePos) {
        if (!language[langNamePos])
            break;
        else
            langName[langNamePos] = language[langNamePos];
    }

    langName[langNamePos++] = ':';
    langName[langNamePos]   = 0;

    if (LoadFile(filePath, &info)) {
        int readMode   = 0;
        ushort curChar = 0;
        int charID     = 0;
        byte flag      = 0;
        while (!ReachedEndOfFile()) {
            switch (readMode) {
                case 0:
                    ReadStringLineUnicode(lineBuffer);
                    if (!FindStringTokenUnicode(lineBuffer, langName, 1)) {
                        int tPos = FindStringTokenUnicode(lineBuffer, strName, 1);
                        if (tPos == 3)
                            flag = true;
                        readMode = tPos == 3;
                    }
                    break;
                case 1:
                    FileRead(fileBuffer, sizeof(ushort));
                    curChar = fileBuffer[0] + (fileBuffer[1] << 8);
                    if (curChar > '\n' && curChar != '\r') {
                        stringStorage[stringStorePos + charID++] = 0;
                        CloseFile();

                        int pos = stringStorePos;
                        stringStorePos += charID;
                        return &stringStorage[pos];
                    }
                    else if (curChar == '\t') {
                        if (flag) {
                            flag     = true;
                            readMode = 2;
                        }
                        else {
                            readMode                                 = 2;
                            stringStorage[stringStorePos + charID++] = '\n';
                        }
                    }
                    break;
                case 2:
                    FileRead(fileBuffer, sizeof(ushort));
                    curChar = fileBuffer[0] + (fileBuffer[1] << 8);
                    if (curChar != '\t') {
                        stringStorage[stringStorePos + charID++] = curChar;
                        if (curChar == '\r' || curChar == '\n') {
                            flag     = false;
                            readMode = 1;
                        }
                    }
                    break;
            }
        }
        CloseFile();
    }

    PrintLog("Failed to load string... (%s, %s)", language, stringName);
    return NULL;
}

void ReadCreditsList(const char *filePath)
{
    FileInfo info;
    if (LoadFile(filePath, &info)) {
        creditsListSize = 0;

        char dest[0x100];
        float advance = 24.0;
        if (!ReachedEndOfFile()) {
            while (creditsListSize < CREDITS_LIST_COUNT) {
                ReadCreditsLine(dest);

                if (dest[0] != '[' || dest[2] != ']') {
                    advance += 24.0;
                }
                else {
                    int strPos   = 0;
                    char curChar = dest[strPos + 3];
                    while (curChar) {
                        stringStorage[stringStorePos + strPos] = curChar;
                        strPos++;
                        curChar = dest[strPos + 3];
                    }
                    stringStorage[stringStorePos + strPos++] = 0;

                    switch (dest[1]) {
                        default:
                        case '0': creditsType[creditsListSize] = 0; break;
                        case '1': creditsType[creditsListSize] = 1; break;
                        case '2': creditsType[creditsListSize] = 2; break;
                        case '3': creditsType[creditsListSize] = 3; break;
                    }

                    creditsAdvanceY[creditsListSize] = advance;

                    strCreditsList[creditsListSize++] = &stringStorage[stringStorePos];
                    stringStorePos += strPos;
                    advance = 24.0;
                }

                if (ReachedEndOfFile())
                    break;
            }
        }

        CloseFile();
    }
}
