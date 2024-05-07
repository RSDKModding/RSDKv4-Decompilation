#ifndef TEXTSYSTEM_H
#define TEXTSYSTEM_H

#define TEXTDATA_COUNT  (0x2800)
#define TEXTENTRY_COUNT (0x200)
#define TEXTMENU_COUNT  (0x2)

#define FONTLIST_CHAR_COUNT (0x1000)
#define FONTLIST_COUNT      (0x4)

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
    sbyte timer;
};

#if !RETRO_REV02
#define FONTCHAR_COUNT (0x400)

struct FontCharacter {
    int id;
    short srcX;
    short srcY;
    short width;
    short height;
    short pivotX;
    short pivotY;
    short xAdvance;
};
#endif

struct BitmapFontCharacter {
    ushort id;
    float x;
    float y;
    float width;
    float height;
    float xOffset;
    float yOffset;
    float xAdvance;
    ushort textureID;
};

struct BitmapFont {
    BitmapFontCharacter characters[FONTLIST_CHAR_COUNT];
    ushort count;
    float lineHeight;
    float base;
};

enum TextMenuAlignments {
    MENU_ALIGN_LEFT,
    MENU_ALIGN_RIGHT,
    MENU_ALIGN_CENTER,
};

extern TextMenu gameMenu[TEXTMENU_COUNT];
extern int textMenuSurfaceNo;

extern char playerListText[0x80][0x20];

extern BitmapFont fontList[FONTLIST_COUNT];

#if !RETRO_REV02
extern FontCharacter fontCharacterList[FONTCHAR_COUNT];

void LoadFontFile(const char *filePath);
#endif

void LoadTextFile(TextMenu *menu, const char *filePath, byte mapCode);
void LoadConfigListText(TextMenu *menu, int listNo);

void SetupTextMenu(TextMenu *menu, int rowCount);
void AddTextMenuEntry(TextMenu *menu, const char *text);
void AddTextMenuEntryW(TextMenu *menu, const ushort *text);
void SetTextMenuEntry(TextMenu *menu, const char *text, int rowID);
void SetTextMenuEntryW(TextMenu *menu, const ushort *text, int rowID);
void EditTextMenuEntry(TextMenu *menu, const char *text, int rowID);

void LoadBitmapFont(const char *filePath, int index, char textureID);
void ResetBitmapFonts();
float GetTextWidth(ushort *text, int fontID, float scaleX);
float GetTextHeight(ushort *text, int fontID, float scaleY);
void SetStringToFont(ushort *text, ushort *string, int fontID);
void SetStringToFont8(ushort *text, const char *string, int fontID);
void AddTimeStringToFont(ushort *text, int time, int fontID);

#endif // !TEXTSYSTEM_H
