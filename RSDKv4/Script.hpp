#ifndef SCRIPT_H
#define SCRIPT_H

#define SCRIPTDATA_COUNT (0x100000)
#define JUMPTABLE_COUNT  (0x10000)
#define FUNCTION_COUNT (0x200)

#define JUMPSTACK_COUNT (0x400)
#define FUNCSTACK_COUNT (0x400)
#define FORSTACK_COUNT (0x400)

struct ScriptPtr {
    int scriptCodePtr;
    int jumpTablePtr;
};

struct ObjectScript {
    int frameCount;
    int spriteSheetID;
    ScriptPtr eventMain;
    ScriptPtr eventDraw;
    ScriptPtr eventStartup;
    int frameListOffset;
    AnimationFile* animFile;
};

struct ScriptEngine {
    int operands[0x10];
    int tempValue[8];
    int arrayPosition[9];
    // int currentPlayer;   // ArrayPos[6]
    // int playerCount;     // ArrayPos[7]
    // int tempObjectPos;   // ArrayPos[8]
    int checkResult;
};

#define TABLE_COUNT       (0x200)
#define TABLE_ENTRY_COUNT (0x400)

struct StaticInfo {
    StaticInfo()
    {
        StrCopy(name, "");
        value   = 0;
        dataPos = SCRIPTDATA_COUNT - 1;
    }
    StaticInfo(const char *aliasName, int val)
    {
        StrCopy(name, aliasName);
        value   = val;
        dataPos = SCRIPTDATA_COUNT - 1;
    }

    char name[0x20];
    int value;
    int dataPos;
};

struct TableInfo {
    TableInfo()
    {
        StrCopy(name, "");
        valueCount = 0;
        dataPos    = SCRIPTDATA_COUNT - 1;
    }
    TableInfo(const char *aliasName, int valCnt)
    {
        StrCopy(name, aliasName);
        valueCount = valCnt;
        dataPos    = SCRIPTDATA_COUNT - 1;
    }

    char name[0x20];
    int valueCount;
    StaticInfo values[TABLE_ENTRY_COUNT];
    int dataPos;
};

enum ScriptSubs { EVENT_MAIN = 0, EVENT_DRAW = 1, EVENT_SETUP = 2 };

extern ObjectScript objectScriptList[OBJECT_COUNT];
extern ScriptPtr functionScriptList[FUNCTION_COUNT];

extern int scriptData[SCRIPTDATA_COUNT];
extern int jumpTableData[JUMPTABLE_COUNT];

extern int jumpTableStack[JUMPSTACK_COUNT];
extern int functionStack[FUNCSTACK_COUNT];
extern int foreachStack[FORSTACK_COUNT];

extern int scriptCodePos; //Bytecode reading offset
extern int jumpTablePos;  //Bytecode reading offset
extern int jumpTableStackPos;
extern int functionStackPos;
extern int foreachStackPos;

extern ScriptEngine scriptEng;
extern char scriptText[0x4000];

extern int scriptDataPos;
extern int scriptDataOffset;
extern int jumpTableDataPos;
extern int jumpTableDataOffset;

extern int scriptFunctionCount;
extern char scriptFunctionNames[FUNCTION_COUNT][0x40];

extern int lineID;

void CheckAliasText(char *text);
void CheckStaticText(char *text);
TableInfo *CheckTableText(char *text);
void ConvertArithmaticSyntax(char *text);
void ConvertIfWhileStatement(char *text);
void ConvertForeachStatement(char *text);
bool ConvertSwitchStatement(char *text);
void ConvertFunctionText(char *text);
void CheckCaseNumber(char *text);
bool ReadSwitchCase(char *text);
void ReadTableValues(char *text);
void AppendIntegerToString(char *text, int value);
void AppendIntegerToStringW(ushort *text, int value);
bool ConvertStringToInteger(const char *text, int *value);
void CopyAliasStr(char *dest, char *text, bool arrayIndex);
bool CheckOpcodeType(char *text); // Never actually used

void ParseScriptFile(char *scriptName, int scriptID);
void LoadBytecode(int stageListID, int scriptID);

void ProcessScript(int scriptCodePtr, int jumpTablePtr, byte scriptSub);

void ClearScriptData(void);

#endif // !SCRIPT_H
