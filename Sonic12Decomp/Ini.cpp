#include "RetroEngine.hpp"
#include <stdlib.h>
#include <algorithm>

IniParser::IniParser(const char *filename)
{
    char buf[120];
    char section[60];
    bool hasSection = false;
    char key[60];
    char value[60];

    count = 0;

    FILE *f;
    if ((f = fopen(filename, "rb")) == NULL) {
#if RSDK_DEBUG
        printf("ERROR: Couldn't open file '%s'!\n", filename);
#endif
        return;
    }

    while (fgets(buf, sizeof(buf), f) != NULL) {
        if (buf[0] == '#')
            continue;

        if (sscanf(buf, "[%[^][]]", section) == 1) {
            hasSection = true;
        }
        else if (sscanf(buf, "%[^ =]= %s", key, value) == 2 || sscanf(buf, "%[^ =]=%s", key, value) == 2
                 || sscanf(buf, "%[^ =] = %s", key, value) == 2 || sscanf(buf, "%[^ =] =%s", key, value) == 2) {
            if (hasSection) {
                strcpy(item[count].section, section);
            }
            strcpy(item[count].key, key);
            strcpy(item[count].value, value);
            item[count].hasSection = hasSection;
            count++;
        }
    }

    fclose(f);
}

int IniParser::GetString(const char *section, const char *key, char *dest)
{
    if (count == 0)
        return 0;

    for (int x = 0; x < count; x++) {
        if (!strcmp(section, item[x].section)) {
            if (!strcmp(key, item[x].key)) {
                strcpy(dest, item[x].value);
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
        if (!strcmp(section, item[x].section)) {
            if (!strcmp(key, item[x].key)) {
                *dest = atoi(item[x].value);
                return 1;
            }
        }
    }

    return 0;
}
int IniParser::GetBool(const char *section, const char *key, bool *dest)
{
    if (count == 0)
        return 0;

    for (int x = 0; x < count; x++) {
        if (!strcmp(section, item[x].section)) {
            if (!strcmp(key, item[x].key)) {
                *dest = !strcmp(item[x].value, "true") || !strcmp(item[x].value, "1");
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
        if (strcmp(section, item[x].section) == 0) {
            if (strcmp(key, item[x].key) == 0) {
                where = x;
                break;
            }
        }
    }
    if (where < 0)
        where = count++;

    strcpy(item[where].section, section);
    strcpy(item[where].key, key);
    strcpy(item[where].value, value);
    return 1;
}
int IniParser::SetInteger(const char *section, const char *key, int value)
{
    int where = -1;
    for (int x = 0; x < count; x++) {
        if (strcmp(section, item[x].section) == 0) {
            if (strcmp(key, item[x].key) == 0) {
                where = x;
                break;
            }
        }
    }
    if (where < 0)
        where = count++;

    strcpy(item[where].section, section);
    strcpy(item[where].key, key);
    sprintf(item[where].value, "%d", value);
    return 1;
}
int IniParser::SetBool(const char *section, const char *key, bool value)
{
    int where = -1;
    for (int x = 0; x < count; x++) {
        if (strcmp(section, item[x].section) == 0) {
            if (strcmp(key, item[x].key) == 0) {
                where = x;
                break;
            }
        }
    }
    if (where < 0)
        where = count++;

    strcpy(item[where].section, section);
    strcpy(item[where].key, key);
    sprintf(item[where].value, "%d", value);
    return 1;
}

void IniParser::Write(const char *filename)
{
    FILE *f;
    if ((f = fopen(filename, "w")) == NULL) {
#if RSDK_DEBUG
        printf("ERROR: Couldn't open file '%s' for writing!\n", filename);
#endif
        return;
    }

    char sections[10][60];
    char past[60];
    int k = 0;
    for (int i = 0; i < count; i++) {
        if (std::find(std::begin(sections), std::end(sections), item[i].section) == std::end(sections) && strcmp(past, item[i].section) != 0) {
            strcpy(past, item[i].section);
            // past = item[i].section;
            strcpy(sections[k], item[i].section);
#if RSDK_DEBUG
            printf("%s %s %s\n", sections[k], item[i].section, past);
#endif
            k++;
        }
    }

    if (strcmp(sections[0], sections[k - 1]) == 0)
        k--;

    for (int i = 0; i < k; i++) {
        fprintf(f, "[%s]\n", sections[i]);
        for (int j = 0; j < count; j++) {
            if (strcmp(sections[i], item[j].section) == 0)
                fprintf(f, "%s = %s\n", item[j].key, item[j].value);
        }
    }

    fclose(f);
}
