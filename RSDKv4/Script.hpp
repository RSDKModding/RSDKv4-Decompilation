#ifndef SCRIPT_H
#define SCRIPT_H

#define SCRIPTCODE_COUNT (0x40000)
#define JUMPTABLE_COUNT  (0x4000)
#define FUNCTION_COUNT   (0x200)

#define JUMPSTACK_COUNT (0x400)
#define FUNCSTACK_COUNT (0x400)
#define FORSTACK_COUNT  (0x400)

#define RETRO_USE_COMPILER (1)

struct ScriptPtr {
    int scriptCodePtr;
    int jumpTablePtr;
};

struct ScriptFunction {

    byte access;
#if RETRO_USE_COMPILER
    char name[0x20];
#endif
    ScriptPtr ptr;
};

struct ObjectScript {
    int frameCount;
    int spriteSheetID;
    ScriptPtr eventUpdate;
    ScriptPtr eventDraw;
    ScriptPtr eventStartup;
    int frameListOffset;
    AnimationFile *animFile;
};

struct ScriptEngine {
    int operands[0x10];
    int temp[8];
    int arrayPosition[9];
    int checkResult;
};

enum ScriptSubs { EVENT_MAIN = 0, EVENT_DRAW = 1, EVENT_SETUP = 2 };

extern ObjectScript objectScriptList[OBJECT_COUNT];
extern ScriptFunction scriptFunctionList[FUNCTION_COUNT];

extern int scriptCode[SCRIPTCODE_COUNT];
extern int jumpTable[JUMPTABLE_COUNT];

extern int jumpTableStack[JUMPSTACK_COUNT];
extern int functionStack[FUNCSTACK_COUNT];
extern int foreachStack[FORSTACK_COUNT];

extern int scriptCodePos;
extern int scriptCodeOffset;
extern int jumpTablePos;
extern int jumpTableOffset;
extern int jumpTableStackPos;
extern int functionStackPos;
extern int foreachStackPos;

extern ScriptEngine scriptEng;
extern char scriptText[0x4000];

bool ConvertStringToInteger(const char *text, int *value);

#if RETRO_USE_COMPILER
extern int scriptFunctionCount;
extern char scriptFunctionNames[FUNCTION_COUNT][0x40];

extern int lineID;

void CheckAliasText(char *text);
void CheckStaticText(char *text);
bool CheckTableText(char *text);
void ConvertArithmaticSyntax(char *text);
void ConvertConditionalStatement(char *text);
bool ConvertSwitchStatement(char *text);
void ConvertFunctionText(char *text);
void CheckCaseNumber(char *text);
bool ReadSwitchCase(char *text);
void ReadTableValues(char *text);
void AppendIntegerToString(char *text, int value);
void AppendIntegerToStringW(ushort *text, int value);
void CopyAliasStr(char *dest, char *text, bool arrayIndex);
bool CheckOpcodeType(char *text); // Never actually used

void ParseScriptFile(char *scriptName, int scriptID);
#endif
void LoadBytecode(int stageListID, int scriptID);

void ProcessScript(int scriptCodeStart, int jumpTableStart, byte scriptEvent);

void ClearScriptData(void);

#endif // !SCRIPT_H