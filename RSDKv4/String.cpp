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
ushort *strStageList[8];
ushort *strSaveStageList[26];
ushort *strNewBestTime      = NULL;
ushort *strRecords          = NULL;
ushort *strNextAct          = NULL;
ushort *strPlay             = NULL;
ushort *strTotalTime        = NULL;
ushort *strInstructions     = NULL;
ushort *strSettings         = NULL;
ushort *strStaffCredits     = NULL;
ushort *strAbout            = NULL;
ushort *strMusic            = NULL;
ushort *strSoundFX          = NULL;
ushort *strSpindash         = NULL;
ushort *strBoxArt           = NULL;
ushort *strControls         = NULL;
ushort *strOn               = NULL;
ushort *strOff              = NULL;
ushort *strCustomizeDPad    = NULL;
ushort *strDPadSize         = NULL;
ushort *strDPadOpacity      = NULL;
ushort *strHelpText1        = NULL;
ushort *strHelpText2        = NULL;
ushort *strHelpText3        = NULL;
ushort *strHelpText4        = NULL;
ushort *strHelpText5        = NULL;
ushort *strVersionName      = NULL;
ushort *strPrivacy          = NULL;
ushort *strTerms            = NULL;

int stageStrCount = 0;

ushort stringStorage[STRSTORAGE_SIZE][STRING_SIZE];
int stringStorePos = 0;

int creditsListSize = 0;
const ushort *strCreditsList[CREDITS_LIST_SIZE];
byte creditsType[CREDITS_LIST_SIZE];
float creditsAdvanceY[CREDITS_LIST_SIZE];

//From here: https://rosettacode.org/wiki/MD5#C

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
        msg2 = (unsigned char*)malloc(64 * grps);
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

void ConvertIntegerToString(char *text, int value)
{
    sprintf(text, "%d", value); // Vary lazy ik
}

//Buffer is expected to be at least 16 bytes long
void GenerateMD5FromString(const char *string, int len, byte *buffer)
{
    unsigned *d = md5(string, len);
    WBunion u;

    for (int i = 0; i < 4; ++i) {
        u.w = d[i];
        for (int c = 0; c < 4; ++c) buffer[(i << 2) + c] = u.b[c];
    }
}

void InitLocalizedStrings() {
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
    strSoundTest      = ReadLocalizedString("SoundTest", langStr, "Data/Game/StringList.txt");
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
    strExitGame          = ReadLocalizedString("ExitGame", langStr, "Data/Game/StringList.txt");
    strNetworkMessage    = ReadLocalizedString("NetworkMessage", langStr, "Data/Game/StringList.txt");
    strStageList[0]      = ReadLocalizedString("StageName1", "en", "Data/Game/StringList.txt");
    strStageList[1]      = ReadLocalizedString("StageName2", "en", "Data/Game/StringList.txt");
    strStageList[2]      = ReadLocalizedString("StageName3", "en", "Data/Game/StringList.txt");
    strStageList[3]      = ReadLocalizedString("StageName4", "en", "Data/Game/StringList.txt");
    strStageList[4]      = ReadLocalizedString("StageName5", "en", "Data/Game/StringList.txt");
    strStageList[5]      = ReadLocalizedString("StageName6", "en", "Data/Game/StringList.txt");
    strStageList[6]      = ReadLocalizedString("StageName7", "en", "Data/Game/StringList.txt");
    strStageList[7]      = ReadLocalizedString("StageName8", "en", "Data/Game/StringList.txt");
    strSaveStageList[0]  = ReadLocalizedString("SaveStageName1", "en", "Data/Game/StringList.txt");
    strSaveStageList[1]  = ReadLocalizedString("SaveStageName2", "en", "Data/Game/StringList.txt");
    strSaveStageList[2]  = ReadLocalizedString("SaveStageName3", "en", "Data/Game/StringList.txt");
    strSaveStageList[3]  = ReadLocalizedString("SaveStageName4", "en", "Data/Game/StringList.txt");
    strSaveStageList[4]  = ReadLocalizedString("SaveStageName5", "en", "Data/Game/StringList.txt");
    strSaveStageList[5]  = ReadLocalizedString("SaveStageName6", "en", "Data/Game/StringList.txt");
    strSaveStageList[6]  = ReadLocalizedString("SaveStageName7", "en", "Data/Game/StringList.txt");
    strSaveStageList[7]  = ReadLocalizedString("SaveStageName8", "en", "Data/Game/StringList.txt");
    strSaveStageList[8]  = ReadLocalizedString("SaveStageName9", "en", "Data/Game/StringList.txt");
    strSaveStageList[9]  = ReadLocalizedString("SaveStageName10", "en", "Data/Game/StringList.txt");
    strSaveStageList[10] = ReadLocalizedString("SaveStageName11", "en", "Data/Game/StringList.txt");
    strSaveStageList[11] = ReadLocalizedString("SaveStageName12", "en", "Data/Game/StringList.txt");
    strSaveStageList[12] = ReadLocalizedString("SaveStageName13", "en", "Data/Game/StringList.txt");
    strSaveStageList[13] = ReadLocalizedString("SaveStageName14", "en", "Data/Game/StringList.txt");
    strSaveStageList[14] = ReadLocalizedString("SaveStageName15", "en", "Data/Game/StringList.txt");
    strSaveStageList[15] = ReadLocalizedString("SaveStageName16", "en", "Data/Game/StringList.txt");
    strSaveStageList[16] = ReadLocalizedString("SaveStageName17", "en", "Data/Game/StringList.txt");
    strSaveStageList[17] = ReadLocalizedString("SaveStageName18", "en", "Data/Game/StringList.txt");
    strSaveStageList[18] = ReadLocalizedString("SaveStageName19", "en", "Data/Game/StringList.txt");
    strSaveStageList[19] = ReadLocalizedString("SaveStageName20", "en", "Data/Game/StringList.txt");
    strSaveStageList[20] = ReadLocalizedString("SaveStageName21", "en", "Data/Game/StringList.txt");
    strSaveStageList[21] = ReadLocalizedString("SaveStageName22", "en", "Data/Game/StringList.txt");
    strSaveStageList[22] = ReadLocalizedString("SaveStageName23", "en", "Data/Game/StringList.txt");
    strSaveStageList[23] = ReadLocalizedString("SaveStageName24", "en", "Data/Game/StringList.txt");
    strSaveStageList[24] = ReadLocalizedString("SaveStageName25", "en", "Data/Game/StringList.txt");
    strSaveStageList[25] = ReadLocalizedString("SaveStageName26", "en", "Data/Game/StringList.txt");
    stageStrCount        = 26;
    strNewBestTime       = ReadLocalizedString("NewBestTime", langStr, "Data/Game/StringList.txt");
    strRecords           = ReadLocalizedString("Records", langStr, "Data/Game/StringList.txt");
    strNextAct           = ReadLocalizedString("NextAct", langStr, "Data/Game/StringList.txt");
    strPlay              = ReadLocalizedString("Play", langStr, "Data/Game/StringList.txt");
    strTotalTime         = ReadLocalizedString("TotalTime", langStr, "Data/Game/StringList.txt");
    strInstructions      = ReadLocalizedString("Instructions", langStr, "Data/Game/StringList.txt");
    strSettings          = ReadLocalizedString("Settings", langStr, "Data/Game/StringList.txt");
    strStaffCredits      = ReadLocalizedString("StaffCredits", langStr, "Data/Game/StringList.txt");
    strAbout             = ReadLocalizedString("About", langStr, "Data/Game/StringList.txt");
    strMusic             = ReadLocalizedString("Music", langStr, "Data/Game/StringList.txt");
    strSoundFX           = ReadLocalizedString("SoundFX", langStr, "Data/Game/StringList.txt");
    strSpindash          = ReadLocalizedString("SpinDash", langStr, "Data/Game/StringList.txt");
    strBoxArt            = ReadLocalizedString("BoxArt", langStr, "Data/Game/StringList.txt");
    strControls          = ReadLocalizedString("Controls", langStr, "Data/Game/StringList.txt");
    strOn                = ReadLocalizedString("On", langStr, "Data/Game/StringList.txt");
    strOff               = ReadLocalizedString("Off", langStr, "Data/Game/StringList.txt");
    strCustomizeDPad     = ReadLocalizedString("CustomizeDPad", langStr, "Data/Game/StringList.txt");
    strDPadSize          = ReadLocalizedString("DPadSize", langStr, "Data/Game/StringList.txt");
    strDPadOpacity       = ReadLocalizedString("DPadOpacity", langStr, "Data/Game/StringList.txt");
    strHelpText1         = ReadLocalizedString("HelpText1", langStr, "Data/Game/StringList.txt");
    strHelpText2         = ReadLocalizedString("HelpText2", langStr, "Data/Game/StringList.txt");
    strHelpText3         = ReadLocalizedString("HelpText3", langStr, "Data/Game/StringList.txt");
    strHelpText4         = ReadLocalizedString("HelpText4", langStr, "Data/Game/StringList.txt");
    strHelpText5         = ReadLocalizedString("HelpText5", langStr, "Data/Game/StringList.txt");
    strVersionName       = ReadLocalizedString("Version", langStr, "Data/Game/StringList.txt");
    strPrivacy           = ReadLocalizedString("Privacy", langStr, "Data/Game/StringList.txt");
    strTerms             = ReadLocalizedString("Terms", langStr, "Data/Game/StringList.txt");
    ReadCreditsList("Data/Game/CreditsMobile.txt");
}
ushort *ReadLocalizedString(const char *stringName, const char *language, const char *filePath)
{
    FileInfo info;
    ushort strName[0x40];
    ushort langName[0x8];
    ushort textBuffer[0x200];

    memset(strName, 0, 0x40 * sizeof(ushort));
    memset(langName, 0, 0x8 * sizeof(ushort));
    memset(textBuffer, 0, 0x200 * sizeof(ushort));

    char strNameChar = *stringName;
    int strNamePos   = 0;
    if (strNameChar) {
        strNamePos = 0;
        do {
            strName[strNamePos++] = strNameChar;
            strNameChar           = stringName[strNamePos];
        } while (strNameChar);
        ++strNamePos;
    }
    strName[strNamePos++] = ':';
    strName[strNamePos]   = 0;

    char langNameChar = *language;
    int langNamePos   = 0;
    if (langNameChar) {
        langName[0] = langNameChar;
        if (language[1]) {
            langName[1] = language[1];
            if (language[2]) {
                langName[2] = language[2];
                if (language[3]) {
                    langName[3] = language[3];
                    langNamePos = 4;
                }
                else {
                    langNamePos = 3;
                }
            }
            else {
                langNamePos = 2;
            }
        }
        else {
            langNamePos = 1;
        }
        langNamePos++;
    }
    else {
        langNamePos = 0;
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
                    ReadStringLineUnicode(textBuffer);
                    if (!FindStringTokenUnicode(textBuffer, langName, 1u)) {
                        int tPos = FindStringTokenUnicode(textBuffer, strName, 1u);
                        if (tPos == 3)
                            flag = 1;
                        readMode = tPos == 3;
                    }
                    break;
                case 1:
                    FileRead(fileBuffer, 2);
                    curChar = fileBuffer[0] + (fileBuffer[1] << 8);
                    if (curChar != '\t' && curChar != '\r') {
                        stringStorage[stringStorePos][charID++] = 0;
                        CloseFile();

                        endLine = false;
                        //printLog("Loaded String\nLanguage: %s\nStringName: %s\nString: ", language, stringName);
                        //printLog(stringStorage[stringStorePos]);
                        endLine = true;

                        return stringStorage[stringStorePos++];
                    }
                    if (curChar == '\t') {
                        if (flag) {
                            flag     = 1;
                            readMode = 2;
                        }
                        else {
                            readMode                                = 2;
                            stringStorage[stringStorePos][charID++] = '\n';
                        }
                    }
                    break;
                case 2:
                    FileRead(fileBuffer, 2);
                    curChar = fileBuffer[0] + (fileBuffer[1] << 8);
                    if (curChar != '\t') {
                        if (curChar == '\r' || curChar == '\n') {
                            flag     = 0;
                            readMode = 1;
                        }
                        else {
                            stringStorage[stringStorePos][charID++] = curChar;
                        }
                    }
                    break;
            }
        }
        CloseFile();
    }

    printLog("Failed to load string... (%s, %s)", language, stringName);
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
            while (creditsListSize < CREDITS_LIST_SIZE) {
                strCreditsList[creditsListSize] = stringStorage[stringStorePos];
                StrCopy(dest, "NULL");
                ReadCreditsLine(dest);
                
                if (dest[0] != '[' || dest[2] != ']') {
                    advance += 24.0;
                }
                else {
                    int strPos   = 0;
                    char curChar = dest[strPos++ + 3];
                    while (curChar) {
                        stringStorage[stringStorePos][strPos] = curChar;
                        curChar                               = dest[strPos + 3];
                        strPos++;
                    }
                    stringStorage[stringStorePos++][strPos] = 0;

                    switch (dest[1]) {
                        default:
                        case '0': creditsType[creditsListSize] = 0; break;
                        case '1': creditsType[creditsListSize] = 1; break;
                        case '2': creditsType[creditsListSize] = 2; break;
                        case '3': creditsType[creditsListSize] = 3; break;
                    }

                    creditsAdvanceY[creditsListSize++] = advance;
                    advance                          = 24.0;
                }

                if (ReachedEndOfFile())
                    break;
            }
        }

        CloseFile();
    }
}