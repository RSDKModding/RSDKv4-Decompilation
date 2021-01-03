#ifndef TEXTSYSTEM_H
#define TEXTSYSTEM_H

#define TEXTDATA_COUNT  (0x2800)
#define TEXTENTRY_COUNT (0x200)
#define TEXTMENU_COUNT (0x2)

enum TextInfoTypes { TEXTINFO_TEXTDATA = 0, TEXTINFO_TEXTSIZE = 1, TEXTINFO_ROWCOUNT = 2 };

struct TextMenu {
    ushort textData[TEXTDATA_COUNT];
    int entryStart[TEXTENTRY_COUNT];
    int entrySize[TEXTENTRY_COUNT];
    byte entryHighlight[TEXTENTRY_COUNT];
    int textDataPos;
    int selection1;
    int selection2;
    ushort rowCount;
    ushort visibleRowCount;
    ushort visibleRowOffset;
    byte alignment;
    byte selectionCount;
    char timer;
};

extern TextMenu gameMenu[TEXTMENU_COUNT];
extern int textMenuSurfaceNo;

extern char playerListText[0x80][0x20];

void LoadTextFile(TextMenu *menu, const char *filePath);
void LoadConfigListText(TextMenu *menu, int listNo);

void SetupTextMenu(TextMenu *menu, int rowCount);
void AddTextMenuEntry(TextMenu *menu, const char *text);
void AddTextMenuEntryW(TextMenu *menu, const ushort *text);
void SetTextMenuEntry(TextMenu *menu, const char *text, int rowID);
void SetTextMenuEntryW(TextMenu *menu, const ushort *text, int rowID);
void EditTextMenuEntry(TextMenu *menu, const char *text, int rowID);

#endif // !TEXTSYSTEM_H
