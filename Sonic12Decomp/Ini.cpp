#include "RetroEngine.hpp"
#include <stdlib.h>
#include <algorithm>
#include <string>

IniParser::IniParser(const char *filename)
{
    memset(items, 0, 0x80 * sizeof(ConfigItem));
    char buf[0x100];
    char section[0x40];
    bool hasSection = false;
    char key[0x40];
    char value[0x100];

    count = 0;

    char pathBuffer[0x80];

#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(pathBuffer, "%s/%s", getResourcesPath(), filename);
    else
        sprintf(pathBuffer, "%s", filename);
#else
    sprintf(pathBuffer, "%s", filename);
#endif

    FileIO *f;
    if ((f = fOpen(pathBuffer, "r")) == NULL) {
        printLog("ERROR: Couldn't open file '%s'!", filename);
        return;
    }

    while (true) {
        bool flag  = false;
        int ret    = 0;
        int strLen = 0;
        while (true) {
            ret  = (int)fRead(&buf[strLen++], sizeof(byte), 1, f);
            flag = ret == 0;
            if (ret == 0)
                break;
            if (buf[strLen - 1] == '\n')
                break;
        }
        buf[strLen] = 0;
        if (buf[0] == '#')
            continue;

        if (sscanf(buf, "[%[^][]]", section) == 1) {
            hasSection = true;
        }
        else if (sscanf(buf, "%[^ =]= %s", key, value) == 2 || sscanf(buf, "%[^ =]=%s", key, value) == 2
                 || sscanf(buf, "%[^ =] = %s", key, value) == 2 || sscanf(buf, "%[^ =] =%s", key, value) == 2) {
            if (hasSection)
                sprintf(items[count].section, "%s", section);

            sprintf(items[count].key, "%s", key);
            sprintf(items[count].value, "%s", value);
            items[count].hasSection = hasSection;
            count++;
        }
        if (flag)
            break;
    }

    fClose(f);
}

int IniParser::GetString(const char *section, const char *key, char *dest)
{
    if (count == 0)
        return 0;

    for (int x = 0; x < count; x++) {
        if (!strcmp(section, items[x].section)) {
            if (!strcmp(key, items[x].key)) {
                strcpy(dest, items[x].value);
                return 1;
            }
        }
    }

    return 0;
}
int IniParser::GetInteger(const char *section, const char *key, int *dest)
{
    if (count == 0)
        return 0;

    for (int x = 0; x < count; x++) {
        if (!strcmp(section, items[x].section)) {
            if (!strcmp(key, items[x].key)) {
                *dest = atoi(items[x].value);
                return 1;
            }
        }
    }

    return 0;
}
int IniParser::GetFloat(const char *section, const char *key, float *dest)
{
    if (count == 0)
        return 0;

    for (int x = 0; x < count; x++) {
        if (!strcmp(section, items[x].section)) {
            if (!strcmp(key, items[x].key)) {
                *dest = atof(items[x].value);
                return 1;
            }
        }
    }

    return 0.0f;
}
int IniParser::GetBool(const char *section, const char *key, bool *dest)
{
    if (count == 0)
        return 0;

    for (int x = 0; x < count; x++) {
        if (!strcmp(section, items[x].section)) {
            if (!strcmp(key, items[x].key)) {
                *dest = !strcmp(items[x].value, "true") || !strcmp(items[x].value, "1");
                return 1;
            }
        }
    }

    return 0;
}

int IniParser::SetString(const char *section, const char *key, char *value)
{
    int where = -1;
    for (int x = 0; x < count; x++) {
        if (strcmp(section, items[x].section) == 0) {
            if (strcmp(key, items[x].key) == 0) {
                where = x;
                break;
            }
        }
    }
    if (where < 0)
        where = count++;

    strcpy(items[where].section, section);
    strcpy(items[where].key, key);
    strcpy(items[where].value, value);
    items[where].type = INI_ITEM_STRING;
    return 1;
}
int IniParser::SetInteger(const char *section, const char *key, int value)
{
    int where = -1;
    for (int x = 0; x < count; x++) {
        if (strcmp(section, items[x].section) == 0) {
            if (strcmp(key, items[x].key) == 0) {
                where = x;
                break;
            }
        }
    }
    if (where < 0)
        where = count++;

    strcpy(items[where].section, section);
    strcpy(items[where].key, key);
    sprintf(items[where].value, "%d", value);
    items[where].type = INI_ITEM_INT;
    return 1;
}
int IniParser::SetFloat(const char *section, const char *key, float value)
{
    int where = -1;
    for (int x = 0; x < count; x++) {
        if (strcmp(section, items[x].section) == 0) {
            if (strcmp(key, items[x].key) == 0) {
                where = x;
                break;
            }
        }
    }
    if (where < 0)
        where = count++;

    strcpy(items[where].section, section);
    strcpy(items[where].key, key);
    sprintf(items[where].value, "%f", value);
    items[where].type = INI_ITEM_FLOAT;
    return 1;
}
int IniParser::SetBool(const char *section, const char *key, bool value)
{
    int where = -1;
    for (int x = 0; x < count; x++) {
        if (strcmp(section, items[x].section) == 0) {
            if (strcmp(key, items[x].key) == 0) {
                where = x;
                break;
            }
        }
    }
    if (where < 0)
        where = count++;

    strcpy(items[where].section, section);
    strcpy(items[where].key, key);
    sprintf(items[where].value, "%s", value ? "true" : "false");
    items[where].type = INI_ITEM_BOOL;
    return 1;
}
int IniParser::SetComment(const char *section, const char *key, const char *comment)
{
    int where = -1;
    for (int x = 0; x < count; x++) {
        if (strcmp(section, items[x].section) == 0) {
            if (strcmp(key, items[x].key) == 0) {
                where = x;
                break;
            }
        }
    }
    if (where < 0)
        where = count++;

    strcpy(items[where].section, section);
    strcpy(items[where].key, key);
    sprintf(items[where].value, "%s", comment);
    items[where].type = INI_ITEM_COMMENT;
    return 1;
}

void IniParser::Write(const char *filename)
{
    char pathBuffer[0x80];

#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(pathBuffer, "%s/%s", getResourcesPath(), filename);
    else
        sprintf(pathBuffer, "%s", filename);
#else
    sprintf(pathBuffer, "%s", filename);
#endif

    FileIO *f;
    if ((f = fOpen(pathBuffer, "w")) == NULL) {
        printLog("ERROR: Couldn't open file '%s' for writing!", filename);
        return;
    }

    char sections[10][60];
    char past[60];
    int c = 0;
    sprintf(past, "");
    for (int i = 0; i < count; ++i) {
        if (std::find(std::begin(sections), std::end(sections), items[i].section) == std::end(sections) && strcmp(past, items[i].section) != 0) {
            sprintf(past, "%s", items[i].section);
            sprintf(sections[c], "%s", items[i].section);
            c++;
        }
    }

    if (c >= 1) {
        if (strcmp(sections[0], sections[c - 1]) == 0)
            c--;
    }

    char buffer[0x100];
    for (int s = 0; s < c; ++s) {
        sprintf(buffer, "[%s]\n", sections[s]);
        fWrite(&buffer, 1, StrLength(buffer), f);
        for (int i = 0; i < count; ++i) {
            if (strcmp(sections[s], items[i].section) == 0) {
                switch (items[i].type) {
                    default:
                    case INI_ITEM_STRING:
                    case INI_ITEM_INT:
                    case INI_ITEM_FLOAT:
                    case INI_ITEM_BOOL:
                        sprintf(buffer, "%s=%s\n", items[i].key, items[i].value);
                        fWrite(&buffer, 1, StrLength(buffer), f);
                        break;
                    case INI_ITEM_COMMENT:
                        sprintf(buffer, "; %s\n", items[i].value);
                        fWrite(&buffer, 1, StrLength(buffer), f);
                        break;
                }
            }
        }

        if (s + 1 < c) {
            sprintf(buffer, "\n");
            fWrite(&buffer, StrLength(buffer), 1, f);
        }
    }

    fClose(f);
}
