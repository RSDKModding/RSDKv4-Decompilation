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
    ScriptPtr subMain;
    ScriptPtr subDraw;
    ScriptPtr subStartup;
    int frameListOffset;
    AnimationFile* animFile;
};

struct ScriptEngine {
    int operands[0x16];
    int tempValue[8];
    int arrayPosition[9];
    // int playerObjectPos; // ArrayPos6
    // int playerObjectCount; // ArrayPos7
    // int tempObjectPos; // ArrayPos8
    int checkResult;
};

enum ScriptSubs { SUB_MAIN = 0, SUB_DRAW = 1, SUB_SETUP = 2 };

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
extern char scriptText[0x100];

extern int scriptDataPos;
extern int scriptDataOffset;
extern int jumpTableDataPos;
extern int jumpTableDataOffset;

extern int scriptFunctionCount;
extern char scriptFunctionNames[FUNCTION_COUNT][0x20];

extern int aliasCount;
extern int lineID;

void CheckAliasText(char *text);
void CheckStaticText(char *text);
void CheckArrayText(char *text);
void ConvertArithmaticSyntax(char *text);
void ConvertIfWhileStatement(char *text);
void ConvertForeachStatement(char *text);
bool ConvertSwitchStatement(char *text);
void ConvertFunctionText(char *text);
void CheckCaseNumber(char *text);
bool ReadSwitchCase(char *text);
void AppendIntegerToSting(char *text, int value);
void AppendIntegerToStingW(ushort *text, int value);
bool ConvertStringToInteger(char *text, int *value);
void CopyAliasStr(char *dest, char *text, bool arrayIndex);
bool CheckOpcodeType(char *text); // Never actually used

void ParseScriptFile(char *scriptName, int scriptID);
void LoadBytecode(int stageListID, int scriptID);

void ProcessScript(int scriptCodePtr, int jumpTablePtr, byte scriptSub);

void ClearScriptData(void);

#endif // !SCRIPT_H
