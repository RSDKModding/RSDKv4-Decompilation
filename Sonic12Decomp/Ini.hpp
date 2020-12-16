#ifndef INI_H
#define INI_H

class IniParser
{
public:
    struct ConfigItems {
        char section[60];
        bool hasSection;
        char key[60];
        char value[60];
    };

    IniParser() {}
    IniParser(const char *filename);

    int GetString(const char *section, const char *key, char *dest);
    int GetInteger(const char *section, const char *key, int *dest);
    int GetBool(const char *section, const char *key, bool *dest);
    int SetString(const char *section, const char *key, char *value);
    int SetInteger(const char *section, const char *key, int value);
    int SetBool(const char *section, const char *key, bool value);
    void Write(const char *filename);


    ConfigItems item[80];

    int count = 0;
};
#endif // !INI_H
