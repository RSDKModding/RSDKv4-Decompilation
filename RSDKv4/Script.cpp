#include "RetroEngine.hpp"
#include <cmath>

#if RETRO_USE_COMPILER
#if !RETRO_REV00
#define COMMON_SCRIPT_VAR_COUNT (34)
#else
#define COMMON_SCRIPT_VAR_COUNT (33)
#endif
#endif

#define SCRIPT_VAR_COUNT (COMMON_SCRIPT_VAR_COUNT + 0x1DF)
int lineID = 0;

enum ScriptVarType { VAR_ALIAS, VAR_STATICVALUE, VAR_TABLE };
enum ScriptVarAccessModifier { ACCESS_NONE, ACCESS_PUBLIC, ACCESS_PRIVATE };

struct ScriptVariableInfo {
    ScriptVariableInfo()
    {
        type   = VAR_ALIAS;
        access = ACCESS_NONE;
        StrCopy(name, "");
        StrCopy(value, "");
    }

    ScriptVariableInfo(byte type, byte access, const char *name, const char *value)
    {
        this->type   = type;
        this->access = access;
        StrCopy(this->name, name);
        StrCopy(this->value, value);
    }

    byte type;
    byte access;
    char name[0x20];
    char value[0x20];
};

struct FunctionInfo {
    FunctionInfo()
    {
        StrCopy(name, "");
        opcodeSize = 0;
    }
    FunctionInfo(const char *functionName, int opSize)
    {
        StrCopy(name, functionName);
        opcodeSize = opSize;
    }
    
#if RETRO_REV03
    char name[0x30];
#else
    char name[0x20];
#endif
    int opcodeSize;
};

#if RETRO_USE_COMPILER
const char variableNames[][0x20] = {
    // Internal Script Values
    "temp0",
    "temp1",
    "temp2",
    "temp3",
    "temp4",
    "temp5",
    "temp6",
    "temp7",
    "checkResult",
    "arrayPos0",
    "arrayPos1",
    "arrayPos2",
    "arrayPos3",
    "arrayPos4",
    "arrayPos5",
    "arrayPos6",
    "arrayPos7",
    "global",
    "local",

    // Object Properties
    "object.entityPos",
    "object.groupID",
    "object.type",
    "object.propertyValue",
    "object.xpos",
    "object.ypos",
    "object.ixpos",
    "object.iypos",
    "object.xvel",
    "object.yvel",
    "object.speed",
    "object.state",
    "object.rotation",
    "object.scale",
    "object.priority",
    "object.drawOrder",
    "object.direction",
    "object.inkEffect",
    "object.alpha",
    "object.frame",
    "object.animation",
    "object.prevAnimation",
    "object.animationSpeed",
    "object.animationTimer",
    "object.angle",
    "object.lookPosX",
    "object.lookPosY",
    "object.collisionMode",
    "object.collisionPlane",
    "object.controlMode",
    "object.controlLock",
    "object.pushing",
    "object.visible",
    "object.tileCollisions",
    "object.interaction",
    "object.gravity",
    "object.up",
    "object.down",
    "object.left",
    "object.right",
    "object.jumpPress",
    "object.jumpHold",
    "object.scrollTracking",
    "object.floorSensorL",
    "object.floorSensorC",
    "object.floorSensorR",
#if !RETRO_REV00
    "object.floorSensorLC",
    "object.floorSensorRC",
#endif
    "object.collisionLeft",
    "object.collisionTop",
    "object.collisionRight",
    "object.collisionBottom",
    "object.outOfBounds",
    "object.spriteSheet",

    // Object Values
    "object.value0",
    "object.value1",
    "object.value2",
    "object.value3",
    "object.value4",
    "object.value5",
    "object.value6",
    "object.value7",
    "object.value8",
    "object.value9",
    "object.value10",
    "object.value11",
    "object.value12",
    "object.value13",
    "object.value14",
    "object.value15",
    "object.value16",
    "object.value17",
    "object.value18",
    "object.value19",
    "object.value20",
    "object.value21",
    "object.value22",
    "object.value23",
    "object.value24",
    "object.value25",
    "object.value26",
    "object.value27",
    "object.value28",
    "object.value29",
    "object.value30",
    "object.value31",
    "object.value32",
    "object.value33",
    "object.value34",
    "object.value35",
    "object.value36",
    "object.value37",
    "object.value38",
    "object.value39",
    "object.value40",
    "object.value41",
    "object.value42",
    "object.value43",
    "object.value44",
    "object.value45",
    "object.value46",
    "object.value47",

    // Stage Properties
    "stage.state",
    "stage.activeList",
    "stage.listPos",
    "stage.timeEnabled",
    "stage.milliSeconds",
    "stage.seconds",
    "stage.minutes",
    "stage.actNum",
    "stage.pauseEnabled",
    "stage.listSize",
    "stage.newXBoundary1",
    "stage.newXBoundary2",
    "stage.newYBoundary1",
    "stage.newYBoundary2",
    "stage.curXBoundary1",
    "stage.curXBoundary2",
    "stage.curYBoundary1",
    "stage.curYBoundary2",
    "stage.deformationData0",
    "stage.deformationData1",
    "stage.deformationData2",
    "stage.deformationData3",
    "stage.waterLevel",
    "stage.activeLayer",
    "stage.midPoint",
    "stage.playerListPos",
    "stage.debugMode",
    "stage.entityPos",

    // Screen Properties
    "screen.cameraEnabled",
    "screen.cameraTarget",
    "screen.cameraStyle",
    "screen.cameraX",
    "screen.cameraY",
    "screen.drawListSize",
    "screen.xcenter",
    "screen.ycenter",
    "screen.xsize",
    "screen.ysize",
    "screen.xoffset",
    "screen.yoffset",
    "screen.shakeX",
    "screen.shakeY",
    "screen.adjustCameraY",

    "touchscreen.down",
    "touchscreen.xpos",
    "touchscreen.ypos",

    // Sound Properties
    "music.volume",
    "music.currentTrack",
    "music.position",

    // Input Properties
    "keyDown.up",
    "keyDown.down",
    "keyDown.left",
    "keyDown.right",
    "keyDown.buttonA",
    "keyDown.buttonB",
    "keyDown.buttonC",
    "keyDown.buttonX",
    "keyDown.buttonY",
    "keyDown.buttonZ",
    "keyDown.buttonL",
    "keyDown.buttonR",
    "keyDown.start",
    "keyDown.select",
    "keyPress.up",
    "keyPress.down",
    "keyPress.left",
    "keyPress.right",
    "keyPress.buttonA",
    "keyPress.buttonB",
    "keyPress.buttonC",
    "keyPress.buttonX",
    "keyPress.buttonY",
    "keyPress.buttonZ",
    "keyPress.buttonL",
    "keyPress.buttonR",
    "keyPress.start",
    "keyPress.select",

    // Menu Properties
    "menu1.selection",
    "menu2.selection",

    // Tile Layer Properties
    "tileLayer.xsize",
    "tileLayer.ysize",
    "tileLayer.type",
    "tileLayer.angle",
    "tileLayer.xpos",
    "tileLayer.ypos",
    "tileLayer.zpos",
    "tileLayer.parallaxFactor",
    "tileLayer.scrollSpeed",
    "tileLayer.scrollPos",
    "tileLayer.deformationOffset",
    "tileLayer.deformationOffsetW",
    "hParallax.parallaxFactor",
    "hParallax.scrollSpeed",
    "hParallax.scrollPos",
    "vParallax.parallaxFactor",
    "vParallax.scrollSpeed",
    "vParallax.scrollPos",

    // 3D Scene Properties
    "scene3D.vertexCount",
    "scene3D.faceCount",
    "scene3D.projectionX",
    "scene3D.projectionY",
#if !RETRO_REV00
    "scene3D.fogColor",
    "scene3D.fogStrength",
#endif

    "vertexBuffer.x",
    "vertexBuffer.y",
    "vertexBuffer.z",
    "vertexBuffer.u",
    "vertexBuffer.v",

    "faceBuffer.a",
    "faceBuffer.b",
    "faceBuffer.c",
    "faceBuffer.d",
    "faceBuffer.flag",
    "faceBuffer.color",

    "saveRAM",
    "engine.state",
#if RETRO_REV00
    "engine.message",
#endif
    "engine.language",
    "engine.onlineActive",
    "engine.sfxVolume",
    "engine.bgmVolume",
#if RETRO_REV00
    "engine.platformID",
#endif
    "engine.trialMode",
#if !RETRO_REV00
    "engine.deviceType",
#endif

// Extras
#if RETRO_REV03
    "screen.currentID",
    "camera.enabled",
    "camera.target",
    "camera.style",
    "camera.xpos",
    "camera.ypos",
    "camera.adjustY",
#endif

// Haptics
#if RETRO_USE_HAPTICS
    "engine.hapticsEnabled",
#endif
};
#endif

const FunctionInfo functions[] = {
    FunctionInfo("End", 0),      // End of Script
    FunctionInfo("Equal", 2),    // Equal
    FunctionInfo("Add", 2),      // Add
    FunctionInfo("Sub", 2),      // Subtract
    FunctionInfo("Inc", 1),      // Increment
    FunctionInfo("Dec", 1),      // Decrement
    FunctionInfo("Mul", 2),      // Multiply
    FunctionInfo("Div", 2),      // Divide
    FunctionInfo("ShR", 2),      // Bit Shift Right
    FunctionInfo("ShL", 2),      // Bit Shift Left
    FunctionInfo("And", 2),      // Bitwise And
    FunctionInfo("Or", 2),       // Bitwise Or
    FunctionInfo("Xor", 2),      // Bitwise Xor
    FunctionInfo("Mod", 2),      // Mod
    FunctionInfo("FlipSign", 1), // Flips the Sign of the value

    FunctionInfo("CheckEqual", 2),    // compare a=b, return result in CheckResult Variable
    FunctionInfo("CheckGreater", 2),  // compare a>b, return result in CheckResult Variable
    FunctionInfo("CheckLower", 2),    // compare a<b, return result in CheckResult Variable
    FunctionInfo("CheckNotEqual", 2), // compare a!=b, return result in CheckResult Variable

    FunctionInfo("IfEqual", 3),          // compare a=b, jump if condition met
    FunctionInfo("IfGreater", 3),        // compare a>b, jump if condition met
    FunctionInfo("IfGreaterOrEqual", 3), // compare a>=b, jump if condition met
    FunctionInfo("IfLower", 3),          // compare a<b, jump if condition met
    FunctionInfo("IfLowerOrEqual", 3),   // compare a<=b, jump if condition met
    FunctionInfo("IfNotEqual", 3),       // compare a!=b, jump if condition met
    FunctionInfo("else", 0),             // The else for an if statement
    FunctionInfo("endif", 0),            // The end if

    FunctionInfo("WEqual", 3),          // compare a=b, loop if condition met
    FunctionInfo("WGreater", 3),        // compare a>b, loop if condition met
    FunctionInfo("WGreaterOrEqual", 3), // compare a>=b, loop if condition met
    FunctionInfo("WLower", 3),          // compare a<b, loop if condition met
    FunctionInfo("WLowerOrEqual", 3),   // compare a<=b, loop if condition met
    FunctionInfo("WNotEqual", 3),       // compare a!=b, loop if condition met
    FunctionInfo("loop", 0),            // While Loop marker

    FunctionInfo("ForEachActive", 3), // foreach loop, iterates through object group lists only if they are active and interaction is true
    FunctionInfo("ForEachAll", 3),    // foreach loop, iterates through objects matching type
    FunctionInfo("next", 0),          // foreach loop, next marker

    FunctionInfo("switch", 2),    // Switch Statement
    FunctionInfo("break", 0),     // break
    FunctionInfo("endswitch", 0), // endswitch

    // Math Functions
    FunctionInfo("Rand", 2),
    FunctionInfo("Sin", 2),
    FunctionInfo("Cos", 2),
    FunctionInfo("Sin256", 2),
    FunctionInfo("Cos256", 2),
    FunctionInfo("ATan2", 3),
    FunctionInfo("Interpolate", 4),
    FunctionInfo("InterpolateXY", 7),

    // Graphics Functions
    FunctionInfo("LoadSpriteSheet", 1),
    FunctionInfo("RemoveSpriteSheet", 1),
    FunctionInfo("DrawSprite", 1),
    FunctionInfo("DrawSpriteXY", 3),
    FunctionInfo("DrawSpriteScreenXY", 3),
    FunctionInfo("DrawTintRect", 4),
    FunctionInfo("DrawNumbers", 7),
    FunctionInfo("DrawActName", 7),
    FunctionInfo("DrawMenu", 3),
    FunctionInfo("SpriteFrame", 6),
    FunctionInfo("EditFrame", 7),
    FunctionInfo("LoadPalette", 5),
    FunctionInfo("RotatePalette", 4),
    FunctionInfo("SetScreenFade", 4),
    FunctionInfo("SetActivePalette", 3),
#if RETRO_REV00
    FunctionInfo("SetPaletteFade", 7),
#else
    FunctionInfo("SetPaletteFade", 6),
#endif
    FunctionInfo("SetPaletteEntry", 3),
    FunctionInfo("GetPaletteEntry", 3),
    FunctionInfo("CopyPalette", 5),
    FunctionInfo("ClearScreen", 1),
    FunctionInfo("DrawSpriteFX", 4),
    FunctionInfo("DrawSpriteScreenFX", 4),

    // More Useful Stuff
    FunctionInfo("LoadAnimation", 1),
    FunctionInfo("SetupMenu", 4),
    FunctionInfo("AddMenuEntry", 3),
    FunctionInfo("EditMenuEntry", 4),
    FunctionInfo("LoadStage", 0),
    FunctionInfo("DrawRect", 8),
    FunctionInfo("ResetObjectEntity", 5),
    FunctionInfo("BoxCollisionTest", 11),
    FunctionInfo("CreateTempObject", 4),

    // Player and Animation Functions
    FunctionInfo("ProcessObjectMovement", 0),
    FunctionInfo("ProcessObjectControl", 0),
    FunctionInfo("ProcessAnimation", 0),
    FunctionInfo("DrawObjectAnimation", 0),

    // Music
    FunctionInfo("SetMusicTrack", 3),
    FunctionInfo("PlayMusic", 1),
    FunctionInfo("StopMusic", 0),
    FunctionInfo("PauseMusic", 0),
    FunctionInfo("ResumeMusic", 0),
    FunctionInfo("SwapMusicTrack", 4),

    // Sound FX
    FunctionInfo("PlaySfx", 2),
    FunctionInfo("StopSfx", 1),
    FunctionInfo("SetSfxAttributes", 3),

    // More Collision Stuff
    FunctionInfo("ObjectTileCollision", 4),
    FunctionInfo("ObjectTileGrip", 4),

    // Bitwise Not
    FunctionInfo("Not", 1),

    // 3D Stuff
    FunctionInfo("Draw3DScene", 0),
    FunctionInfo("SetIdentityMatrix", 1),
    FunctionInfo("MatrixMultiply", 2),
    FunctionInfo("MatrixTranslateXYZ", 4),
    FunctionInfo("MatrixScaleXYZ", 4),
    FunctionInfo("MatrixRotateX", 2),
    FunctionInfo("MatrixRotateY", 2),
    FunctionInfo("MatrixRotateZ", 2),
    FunctionInfo("MatrixRotateXYZ", 4),
#if !RETRO_REV00
    FunctionInfo("MatrixInverse", 1),
#endif
    FunctionInfo("TransformVertices", 3),

    FunctionInfo("CallFunction", 1),
    FunctionInfo("return", 0),

    FunctionInfo("SetLayerDeformation", 6),
    FunctionInfo("CheckTouchRect", 4),
    FunctionInfo("GetTileLayerEntry", 4),
    FunctionInfo("SetTileLayerEntry", 4),

    FunctionInfo("GetBit", 3),
    FunctionInfo("SetBit", 3),

    FunctionInfo("ClearDrawList", 1),
    FunctionInfo("AddDrawListEntityRef", 2),
    FunctionInfo("GetDrawListEntityRef", 3),
    FunctionInfo("SetDrawListEntityRef", 3),

    FunctionInfo("Get16x16TileInfo", 4),
    FunctionInfo("Set16x16TileInfo", 4),
    FunctionInfo("Copy16x16Tile", 2),
    FunctionInfo("GetAnimationByName", 2),
    FunctionInfo("ReadSaveRAM", 0),
    FunctionInfo("WriteSaveRAM", 0),

#if !RETRO_REV02
    FunctionInfo("LoadFontFile", 1),
    FunctionInfo("LoadTextFile", 3),
#else
    FunctionInfo("LoadTextFile", 2),
#endif
    FunctionInfo("GetTextInfo", 5),
#if !RETRO_REV02
    FunctionInfo("DrawText", 7),
#endif
    FunctionInfo("GetVersionNumber", 2),

    FunctionInfo("GetTableValue", 3),
    FunctionInfo("SetTableValue", 3),

    FunctionInfo("CheckCurrentStageFolder", 1),
    FunctionInfo("Abs", 1),

    FunctionInfo("CallNativeFunction", 1),
    FunctionInfo("CallNativeFunction2", 3),
    FunctionInfo("CallNativeFunction4", 5),

    FunctionInfo("SetObjectRange", 1),
#if RETRO_REV02
    FunctionInfo("GetObjectValue", 3),
    FunctionInfo("SetObjectValue", 3),
    FunctionInfo("CopyObject", 3),
#endif
    FunctionInfo("Print", 3),

#if RETRO_REV03
    // Extras
    FunctionInfo("CheckCameraProximity", 4),
    FunctionInfo("SetScreenCount", 1),
    FunctionInfo("SetScreenVertices", 5),
    FunctionInfo("GetInputDeviceID", 2),
    FunctionInfo("GetFilteredInputDeviceID", 4),
    FunctionInfo("GetInputDeviceType", 2),
    FunctionInfo("IsInputDeviceAssigned", 1),
    FunctionInfo("AssignInputSlotToDevice", 2),
    FunctionInfo("IsInputSlotAssigned", 1),
    FunctionInfo("ResetInputSlotAssignments", 0),
#endif
};

#if RETRO_USE_COMPILER

int scriptValueListCount = 0;
// clang-format off
ScriptVariableInfo scriptValueList[SCRIPT_VAR_COUNT] = {
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "true", "1"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "false", "0"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "FX_SCALE", "0"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "FX_ROTATE", "1"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "FX_ROTOZOOM", "2"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "FX_INK", "3"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "PRESENTATION_STAGE", "0"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "REGULAR_STAGE", "1"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "BONUS_STAGE", "2"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "SPECIAL_STAGE", "3"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "MENU_1", "0"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "MENU_2", "1"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "C_TOUCH", "0"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "C_SOLID", "1"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "C_SOLID2", "2"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "C_PLATFORM", "3"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "C_BOX", "65536"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "MAT_WORLD", "0"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "MAT_VIEW", "1"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "MAT_TEMP", "2"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "FX_FLIP", "5"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "FACING_LEFT", "1"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "FACING_RIGHT", "0"),
#if !RETRO_REV00
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "STAGE_2P_MODE", "4"),
#endif
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "STAGE_FROZEN", "3"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "STAGE_PAUSED", "2"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "STAGE_RUNNING", "1"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "RESET_GAME", "2"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "STANDARD", "0"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "MOBILE", "1"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "DEVICE_XBOX", "2"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "DEVICE_PSN", "3"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "DEVICE_IOS", "4"),
    ScriptVariableInfo(VAR_ALIAS, ACCESS_PUBLIC, "DEVICE_ANDROID", "5"),
};
// clang-format on

const char scriptEvaluationTokens[][0x4] = { "=",  "+=", "-=", "++", "--", "*=", "/=", ">>=", "<<=", "&=",
                                             "|=", "^=", "%=", "==", ">",  ">=", "<",  "<=",  "!=" };

enum ScriptReadModes { READMODE_NORMAL = 0, READMODE_STRING = 1, READMODE_COMMENTLINE = 2, READMODE_ENDLINE = 3, READMODE_EOF = 4 };
enum ScriptParseModes {
    PARSEMODE_SCOPELESS    = 0,
    PARSEMODE_PLATFORMSKIP = 1,
    PARSEMODE_FUNCTION     = 2,
    PARSEMODE_SWITCHREAD   = 3,
    PARSEMODE_TABLEREAD    = 4,
    PARSEMODE_ERROR        = 0xFF
};
#endif

enum ScriptVarTypes { SCRIPTVAR_VAR = 1, SCRIPTVAR_INTCONST = 2, SCRIPTVAR_STRCONST = 3 };
enum ScriptVarArrTypes { VARARR_NONE = 0, VARARR_ARRAY = 1, VARARR_ENTNOPLUS1 = 2, VARARR_ENTNOMINUS1 = 3 };

enum ScrVar {
    VAR_TEMP0,
    VAR_TEMP1,
    VAR_TEMP2,
    VAR_TEMP3,
    VAR_TEMP4,
    VAR_TEMP5,
    VAR_TEMP6,
    VAR_TEMP7,
    VAR_CHECKRESULT,
    VAR_ARRAYPOS0,
    VAR_ARRAYPOS1,
    VAR_ARRAYPOS2,
    VAR_ARRAYPOS3,
    VAR_ARRAYPOS4,
    VAR_ARRAYPOS5,
    VAR_ARRAYPOS6,
    VAR_ARRAYPOS7,
    VAR_GLOBAL,
    VAR_LOCAL,
    VAR_OBJECTENTITYPOS,
    VAR_OBJECTGROUPID,
    VAR_OBJECTTYPE,
    VAR_OBJECTPROPERTYVALUE,
    VAR_OBJECTXPOS,
    VAR_OBJECTYPOS,
    VAR_OBJECTIXPOS,
    VAR_OBJECTIYPOS,
    VAR_OBJECTXVEL,
    VAR_OBJECTYVEL,
    VAR_OBJECTSPEED,
    VAR_OBJECTSTATE,
    VAR_OBJECTROTATION,
    VAR_OBJECTSCALE,
    VAR_OBJECTPRIORITY,
    VAR_OBJECTDRAWORDER,
    VAR_OBJECTDIRECTION,
    VAR_OBJECTINKEFFECT,
    VAR_OBJECTALPHA,
    VAR_OBJECTFRAME,
    VAR_OBJECTANIMATION,
    VAR_OBJECTPREVANIMATION,
    VAR_OBJECTANIMATIONSPEED,
    VAR_OBJECTANIMATIONTIMER,
    VAR_OBJECTANGLE,
    VAR_OBJECTLOOKPOSX,
    VAR_OBJECTLOOKPOSY,
    VAR_OBJECTCOLLISIONMODE,
    VAR_OBJECTCOLLISIONPLANE,
    VAR_OBJECTCONTROLMODE,
    VAR_OBJECTCONTROLLOCK,
    VAR_OBJECTPUSHING,
    VAR_OBJECTVISIBLE,
    VAR_OBJECTTILECOLLISIONS,
    VAR_OBJECTINTERACTION,
    VAR_OBJECTGRAVITY,
    VAR_OBJECTUP,
    VAR_OBJECTDOWN,
    VAR_OBJECTLEFT,
    VAR_OBJECTRIGHT,
    VAR_OBJECTJUMPPRESS,
    VAR_OBJECTJUMPHOLD,
    VAR_OBJECTSCROLLTRACKING,
    VAR_OBJECTFLOORSENSORL,
    VAR_OBJECTFLOORSENSORC,
    VAR_OBJECTFLOORSENSORR,
#if !RETRO_REV00
    VAR_OBJECTFLOORSENSORLC,
    VAR_OBJECTFLOORSENSORRC,
#endif
    VAR_OBJECTCOLLISIONLEFT,
    VAR_OBJECTCOLLISIONTOP,
    VAR_OBJECTCOLLISIONRIGHT,
    VAR_OBJECTCOLLISIONBOTTOM,
    VAR_OBJECTOUTOFBOUNDS,
    VAR_OBJECTSPRITESHEET,
    VAR_OBJECTVALUE0,
    VAR_OBJECTVALUE1,
    VAR_OBJECTVALUE2,
    VAR_OBJECTVALUE3,
    VAR_OBJECTVALUE4,
    VAR_OBJECTVALUE5,
    VAR_OBJECTVALUE6,
    VAR_OBJECTVALUE7,
    VAR_OBJECTVALUE8,
    VAR_OBJECTVALUE9,
    VAR_OBJECTVALUE10,
    VAR_OBJECTVALUE11,
    VAR_OBJECTVALUE12,
    VAR_OBJECTVALUE13,
    VAR_OBJECTVALUE14,
    VAR_OBJECTVALUE15,
    VAR_OBJECTVALUE16,
    VAR_OBJECTVALUE17,
    VAR_OBJECTVALUE18,
    VAR_OBJECTVALUE19,
    VAR_OBJECTVALUE20,
    VAR_OBJECTVALUE21,
    VAR_OBJECTVALUE22,
    VAR_OBJECTVALUE23,
    VAR_OBJECTVALUE24,
    VAR_OBJECTVALUE25,
    VAR_OBJECTVALUE26,
    VAR_OBJECTVALUE27,
    VAR_OBJECTVALUE28,
    VAR_OBJECTVALUE29,
    VAR_OBJECTVALUE30,
    VAR_OBJECTVALUE31,
    VAR_OBJECTVALUE32,
    VAR_OBJECTVALUE33,
    VAR_OBJECTVALUE34,
    VAR_OBJECTVALUE35,
    VAR_OBJECTVALUE36,
    VAR_OBJECTVALUE37,
    VAR_OBJECTVALUE38,
    VAR_OBJECTVALUE39,
    VAR_OBJECTVALUE40,
    VAR_OBJECTVALUE41,
    VAR_OBJECTVALUE42,
    VAR_OBJECTVALUE43,
    VAR_OBJECTVALUE44,
    VAR_OBJECTVALUE45,
    VAR_OBJECTVALUE46,
    VAR_OBJECTVALUE47,
    VAR_STAGESTATE,
    VAR_STAGEACTIVELIST,
    VAR_STAGELISTPOS,
    VAR_STAGETIMEENABLED,
    VAR_STAGEMILLISECONDS,
    VAR_STAGESECONDS,
    VAR_STAGEMINUTES,
    VAR_STAGEACTNUM,
    VAR_STAGEPAUSEENABLED,
    VAR_STAGELISTSIZE,
    VAR_STAGENEWXBOUNDARY1,
    VAR_STAGENEWXBOUNDARY2,
    VAR_STAGENEWYBOUNDARY1,
    VAR_STAGENEWYBOUNDARY2,
    VAR_STAGECURXBOUNDARY1,
    VAR_STAGECURXBOUNDARY2,
    VAR_STAGECURYBOUNDARY1,
    VAR_STAGECURYBOUNDARY2,
    VAR_STAGEDEFORMATIONDATA0,
    VAR_STAGEDEFORMATIONDATA1,
    VAR_STAGEDEFORMATIONDATA2,
    VAR_STAGEDEFORMATIONDATA3,
    VAR_STAGEWATERLEVEL,
    VAR_STAGEACTIVELAYER,
    VAR_STAGEMIDPOINT,
    VAR_STAGEPLAYERLISTPOS,
    VAR_STAGEDEBUGMODE,
    VAR_STAGEENTITYPOS,
    VAR_SCREENCAMERAENABLED,
    VAR_SCREENCAMERATARGET,
    VAR_SCREENCAMERASTYLE,
    VAR_SCREENCAMERAX,
    VAR_SCREENCAMERAY,
    VAR_SCREENDRAWLISTSIZE,
    VAR_SCREENXCENTER,
    VAR_SCREENYCENTER,
    VAR_SCREENXSIZE,
    VAR_SCREENYSIZE,
    VAR_SCREENXOFFSET,
    VAR_SCREENYOFFSET,
    VAR_SCREENSHAKEX,
    VAR_SCREENSHAKEY,
    VAR_SCREENADJUSTCAMERAY,
    VAR_TOUCHSCREENDOWN,
    VAR_TOUCHSCREENXPOS,
    VAR_TOUCHSCREENYPOS,
    VAR_MUSICVOLUME,
    VAR_MUSICCURRENTTRACK,
    VAR_MUSICPOSITION,
    VAR_KEYDOWNUP,
    VAR_KEYDOWNDOWN,
    VAR_KEYDOWNLEFT,
    VAR_KEYDOWNRIGHT,
    VAR_KEYDOWNBUTTONA,
    VAR_KEYDOWNBUTTONB,
    VAR_KEYDOWNBUTTONC,
    VAR_KEYDOWNBUTTONX,
    VAR_KEYDOWNBUTTONY,
    VAR_KEYDOWNBUTTONZ,
    VAR_KEYDOWNBUTTONL,
    VAR_KEYDOWNBUTTONR,
    VAR_KEYDOWNSTART,
    VAR_KEYDOWNSELECT,
    VAR_KEYPRESSUP,
    VAR_KEYPRESSDOWN,
    VAR_KEYPRESSLEFT,
    VAR_KEYPRESSRIGHT,
    VAR_KEYPRESSBUTTONA,
    VAR_KEYPRESSBUTTONB,
    VAR_KEYPRESSBUTTONC,
    VAR_KEYPRESSBUTTONX,
    VAR_KEYPRESSBUTTONY,
    VAR_KEYPRESSBUTTONZ,
    VAR_KEYPRESSBUTTONL,
    VAR_KEYPRESSBUTTONR,
    VAR_KEYPRESSSTART,
    VAR_KEYPRESSSELECT,
    VAR_MENU1SELECTION,
    VAR_MENU2SELECTION,
    VAR_TILELAYERXSIZE,
    VAR_TILELAYERYSIZE,
    VAR_TILELAYERTYPE,
    VAR_TILELAYERANGLE,
    VAR_TILELAYERXPOS,
    VAR_TILELAYERYPOS,
    VAR_TILELAYERZPOS,
    VAR_TILELAYERPARALLAXFACTOR,
    VAR_TILELAYERSCROLLSPEED,
    VAR_TILELAYERSCROLLPOS,
    VAR_TILELAYERDEFORMATIONOFFSET,
    VAR_TILELAYERDEFORMATIONOFFSETW,
    VAR_HPARALLAXPARALLAXFACTOR,
    VAR_HPARALLAXSCROLLSPEED,
    VAR_HPARALLAXSCROLLPOS,
    VAR_VPARALLAXPARALLAXFACTOR,
    VAR_VPARALLAXSCROLLSPEED,
    VAR_VPARALLAXSCROLLPOS,
    VAR_SCENE3DVERTEXCOUNT,
    VAR_SCENE3DFACECOUNT,
    VAR_SCENE3DPROJECTIONX,
    VAR_SCENE3DPROJECTIONY,
#if !RETRO_REV00
    VAR_SCENE3DFOGCOLOR,
    VAR_SCENE3DFOGSTRENGTH,
#endif
    VAR_VERTEXBUFFERX,
    VAR_VERTEXBUFFERY,
    VAR_VERTEXBUFFERZ,
    VAR_VERTEXBUFFERU,
    VAR_VERTEXBUFFERV,
    VAR_FACEBUFFERA,
    VAR_FACEBUFFERB,
    VAR_FACEBUFFERC,
    VAR_FACEBUFFERD,
    VAR_FACEBUFFERFLAG,
    VAR_FACEBUFFERCOLOR,
    VAR_SAVERAM,
    VAR_ENGINESTATE,
#if RETRO_REV00
    VAR_ENGINEMESSAGE,
#endif
    VAR_ENGINELANGUAGE,
    VAR_ENGINEONLINEACTIVE,
    VAR_ENGINESFXVOLUME,
    VAR_ENGINEBGMVOLUME,
#if RETRO_REV00
    VAR_ENGINEPLATFORMID, // v3-style device type aka Windows/Mac/Android/etc
#endif
    VAR_ENGINETRIALMODE,
#if !RETRO_REV00
    VAR_ENGINEDEVICETYPE, // v4-style device type aka Standard/Mobile/Etc
#endif

#if RETRO_REV03
    // Extras
    VAR_SCREENCURRENTID,
    VAR_CAMERAENABLED,
    VAR_CAMERATARGET,
    VAR_CAMERASTYLE,
    VAR_CAMERAXPOS,
    VAR_CAMERAYPOS,
    VAR_CAMERAADJUSTY,
#endif

#if RETRO_USE_HAPTICS
    VAR_HAPTICSENABLED,
#endif
    VAR_MAX_CNT
};

enum ScrFunc {
    FUNC_END,
    FUNC_EQUAL,
    FUNC_ADD,
    FUNC_SUB,
    FUNC_INC,
    FUNC_DEC,
    FUNC_MUL,
    FUNC_DIV,
    FUNC_SHR,
    FUNC_SHL,
    FUNC_AND,
    FUNC_OR,
    FUNC_XOR,
    FUNC_MOD,
    FUNC_FLIPSIGN,
    FUNC_CHECKEQUAL,
    FUNC_CHECKGREATER,
    FUNC_CHECKLOWER,
    FUNC_CHECKNOTEQUAL,
    FUNC_IFEQUAL,
    FUNC_IFGREATER,
    FUNC_IFGREATEROREQUAL,
    FUNC_IFLOWER,
    FUNC_IFLOWEROREQUAL,
    FUNC_IFNOTEQUAL,
    FUNC_ELSE,
    FUNC_ENDIF,
    FUNC_WEQUAL,
    FUNC_WGREATER,
    FUNC_WGREATEROREQUAL,
    FUNC_WLOWER,
    FUNC_WLOWEROREQUAL,
    FUNC_WNOTEQUAL,
    FUNC_LOOP,
    FUNC_FOREACHACTIVE,
    FUNC_FOREACHALL,
    FUNC_NEXT,
    FUNC_SWITCH,
    FUNC_BREAK,
    FUNC_ENDSWITCH,
    FUNC_RAND,
    FUNC_SIN,
    FUNC_COS,
    FUNC_SIN256,
    FUNC_COS256,
    FUNC_ATAN2,
    FUNC_INTERPOLATE,
    FUNC_INTERPOLATEXY,
    FUNC_LOADSPRITESHEET,
    FUNC_REMOVESPRITESHEET,
    FUNC_DRAWSPRITE,
    FUNC_DRAWSPRITEXY,
    FUNC_DRAWSPRITESCREENXY,
    FUNC_DRAWTINTRECT,
    FUNC_DRAWNUMBERS,
    FUNC_DRAWACTNAME,
    FUNC_DRAWMENU,
    FUNC_SPRITEFRAME,
    FUNC_EDITFRAME,
    FUNC_LOADPALETTE,
    FUNC_ROTATEPALETTE,
    FUNC_SETSCREENFADE,
    FUNC_SETACTIVEPALETTE,
    FUNC_SETPALETTEFADE,
    FUNC_SETPALETTEENTRY,
    FUNC_GETPALETTEENTRY,
    FUNC_COPYPALETTE,
    FUNC_CLEARSCREEN,
    FUNC_DRAWSPRITEFX,
    FUNC_DRAWSPRITESCREENFX,
    FUNC_LOADANIMATION,
    FUNC_SETUPMENU,
    FUNC_ADDMENUENTRY,
    FUNC_EDITMENUENTRY,
    FUNC_LOADSTAGE,
    FUNC_DRAWRECT,
    FUNC_RESETOBJECTENTITY,
    FUNC_BOXCOLLISIONTEST,
    FUNC_CREATETEMPOBJECT,
    FUNC_PROCESSOBJECTMOVEMENT,
    FUNC_PROCESSOBJECTCONTROL,
    FUNC_PROCESSANIMATION,
    FUNC_DRAWOBJECTANIMATION,
    FUNC_SETMUSICTRACK,
    FUNC_PLAYMUSIC,
    FUNC_STOPMUSIC,
    FUNC_PAUSEMUSIC,
    FUNC_RESUMEMUSIC,
    FUNC_SWAPMUSICTRACK,
    FUNC_PLAYSFX,
    FUNC_STOPSFX,
    FUNC_SETSFXATTRIBUTES,
    FUNC_OBJECTTILECOLLISION,
    FUNC_OBJECTTILEGRIP,
    FUNC_NOT,
    FUNC_DRAW3DSCENE,
    FUNC_SETIDENTITYMATRIX,
    FUNC_MATRIXMULTIPLY,
    FUNC_MATRIXTRANSLATEXYZ,
    FUNC_MATRIXSCALEXYZ,
    FUNC_MATRIXROTATEX,
    FUNC_MATRIXROTATEY,
    FUNC_MATRIXROTATEZ,
    FUNC_MATRIXROTATEXYZ,
#if !RETRO_REV00
    FUNC_MATRIXINVERSE,
#endif
    FUNC_TRANSFORMVERTICES,
    FUNC_CALLFUNCTION,
    FUNC_RETURN,
    FUNC_SETLAYERDEFORMATION,
    FUNC_CHECKTOUCHRECT,
    FUNC_GETTILELAYERENTRY,
    FUNC_SETTILELAYERENTRY,
    FUNC_GETBIT,
    FUNC_SETBIT,
    FUNC_CLEARDRAWLIST,
    FUNC_ADDDRAWLISTENTITYREF,
    FUNC_GETDRAWLISTENTITYREF,
    FUNC_SETDRAWLISTENTITYREF,
    FUNC_GET16X16TILEINFO,
    FUNC_SET16X16TILEINFO,
    FUNC_COPY16X16TILE,
    FUNC_GETANIMATIONBYNAME,
    FUNC_READSAVERAM,
    FUNC_WRITESAVERAM,
#if !RETRO_REV02
    FUNC_LOADTEXTFONT,
#endif
    FUNC_LOADTEXTFILE,
    FUNC_GETTEXTINFO,
#if !RETRO_REV02
    FUNC_DRAWTEXT,
#endif
    FUNC_GETVERSIONNUMBER,
    FUNC_GETTABLEVALUE,
    FUNC_SETTABLEVALUE,
    FUNC_CHECKCURRENTSTAGEFOLDER,
    FUNC_ABS,
    FUNC_CALLNATIVEFUNCTION,
    FUNC_CALLNATIVEFUNCTION2,
    FUNC_CALLNATIVEFUNCTION4,
    FUNC_SETOBJECTRANGE,
#if RETRO_REV02
    FUNC_GETOBJECTVALUE,
    FUNC_SETOBJECTVALUE,
    FUNC_COPYOBJECT,
#endif
    FUNC_PRINT,

#if RETRO_REV03
    // Extras
    FUNC_CHECKCAMERAPROXIMITY,
    FUNC_SETSCREENCOUNT,
    FUNC_SETSCREENVERTICES,
    FUNC_GETINPUTDEVICEID,
    FUNC_GETFILTEREDINPUTDEVICEID,
    FUNC_GETINPUTDEVICETYPE,
    FUNC_ISINPUTDEVICEASSIGNED,
    FUNC_ASSIGNINPUTSLOTTODEVICE,
    FUNC_ISSLOTASSIGNED,
    FUNC_RESETINPUTSLOTASSIGNMENTS,
#endif
    FUNC_MAX_CNT
};

ObjectScript objectScriptList[OBJECT_COUNT];
ScriptFunction scriptFunctionList[FUNCTION_COUNT];
#if RETRO_USE_COMPILER
int scriptFunctionCount = 0;
#endif

int scriptCode[SCRIPTCODE_COUNT];
int jumpTable[JUMPTABLE_COUNT];
int jumpTableStack[JUMPSTACK_COUNT];
int functionStack[FUNCSTACK_COUNT];
int foreachStack[FORSTACK_COUNT];

int scriptCodePos     = 0;
int scriptCodeOffset  = 0;
int jumpTablePos      = 0;
int jumpTableOffset   = 0;
int jumpTableStackPos = 0;
int functionStackPos  = 0;
int foreachStackPos   = 0;

ScriptEngine scriptEng = ScriptEngine();
char scriptText[0x4000];

#if RETRO_USE_COMPILER
void CheckAliasText(char *text)
{
    if (FindStringToken(text, "publicalias", 1) == 0) {
#if !RETRO_USE_ORIGINAL_CODE
        if (scriptValueListCount >= SCRIPT_VAR_COUNT) {
            SetupTextMenu(&gameMenu[0], 0);
            AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "TOO MANY ALIASES, STATIC");
            AddTextMenuEntry(&gameMenu[0], "VALUES, AND TABLES");
            Engine.gameMode = ENGINE_SCRIPTERROR;
            return;
        }
#endif

        ScriptVariableInfo *variable = &scriptValueList[scriptValueListCount];
        MEM_ZEROP(variable);

        int textStrPos = 11;
        int varStrPos  = 0;
        int parseMode  = 0;

        while (text[textStrPos]) {
            switch (parseMode) {
                default: break;

                case 0:
                    if (text[textStrPos] == ':') {
                        textStrPos++;
                        variable->value[varStrPos] = 0;
                        varStrPos                  = 0;
                        parseMode                  = 1;
                    }
                    else {
                        variable->value[varStrPos++] = text[textStrPos++];
                    }
                    break;

                case 1: variable->name[varStrPos++] = text[textStrPos++]; break;
            }
        }

        variable->access = ACCESS_PUBLIC;

#if !RETRO_USE_ORIGINAL_CODE
        for (int v = 0; v < scriptValueListCount; ++v) {
            if (StrComp(scriptValueList[v].name, variable->name))
                PrintLog("WARNING: Variable Name '%s' has already been used!", variable->name);
        }
#endif

        ++scriptValueListCount;
    }
    else if (FindStringToken(text, "privatealias", 1) == 0) {
#if !RETRO_USE_ORIGINAL_CODE
        if (scriptValueListCount >= SCRIPT_VAR_COUNT) {
            SetupTextMenu(&gameMenu[0], 0);
            AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "TOO MANY ALIASES, STATIC");
            AddTextMenuEntry(&gameMenu[0], "VALUES, AND TABLES");
            Engine.gameMode = ENGINE_SCRIPTERROR;
            return;
        }
#endif

        ScriptVariableInfo *variable = &scriptValueList[scriptValueListCount];
        MEM_ZEROP(variable);

        int textStrPos = 12;
        int varStrPos  = 0;
        int parseMode  = 0;

        while (text[textStrPos]) {
            switch (parseMode) {
                default: break;

                case 0:
                    if (text[textStrPos] == ':') {
                        textStrPos++;
                        variable->value[varStrPos] = 0;
                        varStrPos                  = 0;
                        parseMode                  = 1;
                    }
                    else {
                        variable->value[varStrPos++] = text[textStrPos++];
                    }
                    break;

                case 1: variable->name[varStrPos++] = text[textStrPos++]; break;
            }
        }

        variable->access = ACCESS_PRIVATE;

#if !RETRO_USE_ORIGINAL_CODE
        for (int v = 0; v < scriptValueListCount; ++v) {
            if (StrComp(scriptValueList[v].name, variable->name))
                PrintLog("WARNING: Variable Name '%s' has already been used!", variable->name);
        }
#endif

        ++scriptValueListCount;
    }
}
void CheckStaticText(char *text)
{
    if (FindStringToken(text, "publicvalue", 1) == 0) {
#if !RETRO_USE_ORIGINAL_CODE
        if (scriptValueListCount >= SCRIPT_VAR_COUNT) {
            SetupTextMenu(&gameMenu[0], 0);
            AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "TOO MANY ALIASES, STATIC");
            AddTextMenuEntry(&gameMenu[0], "VALUES, AND TABLES");
            Engine.gameMode = ENGINE_SCRIPTERROR;
            return;
        }
#endif

        ScriptVariableInfo *variable = &scriptValueList[scriptValueListCount];
        MEM_ZEROP(variable);

        int textStrPos = 11;
        int varStrPos  = 0;
        int parseMode  = 0;

        StrCopy(variable->value, "0"); // default value is 0
        while (text[textStrPos]) {
            switch (parseMode) {
                default: break;

                case 0:
                    if (text[textStrPos] == '=') {
                        textStrPos++;
                        variable->name[varStrPos] = 0;
                        varStrPos                 = 0;
                        parseMode                 = 1;
                    }
                    else {
                        variable->name[varStrPos++] = text[textStrPos++];
                    }
                    break;

                case 1: variable->value[varStrPos++] = text[textStrPos++]; break;
            }
        }

        variable->access = ACCESS_PUBLIC;

        if (!ConvertStringToInteger(variable->value, &scriptCode[scriptCodePos]))
            scriptCode[scriptCodePos] = 0;

        StrCopy(variable->value, "local[");
        AppendIntegerToString(variable->value, scriptCodePos++);
        StrAdd(variable->value, "]");

#if !RETRO_USE_ORIGINAL_CODE
        for (int v = 0; v < scriptValueListCount; ++v) {
            if (StrComp(scriptValueList[v].name, variable->name))
                PrintLog("WARNING: Variable Name '%s' has already been used!", variable->name);
        }
#endif

        ++scriptValueListCount;
    }
    else if (FindStringToken(text, "privatevalue", 1) == 0) {
#if !RETRO_USE_ORIGINAL_CODE
        if (scriptValueListCount >= SCRIPT_VAR_COUNT) {
            SetupTextMenu(&gameMenu[0], 0);
            AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "TOO MANY ALIASES, STATIC");
            AddTextMenuEntry(&gameMenu[0], "VALUES, AND TABLES");
            Engine.gameMode = ENGINE_SCRIPTERROR;
            return;
        }
#endif

        ScriptVariableInfo *variable = &scriptValueList[scriptValueListCount];
        MEM_ZEROP(variable);

        int textStrPos = 12;
        int varStrPos  = 0;
        int parseMode  = 0;

        StrCopy(variable->value, "0"); // default value is 0
        while (text[textStrPos]) {
            switch (parseMode) {
                default: break;

                case 0:
                    if (text[textStrPos] == '=') {
                        textStrPos++;
                        variable->name[varStrPos] = 0;
                        varStrPos                 = 0;
                        parseMode                 = 1;
                    }
                    else {
                        variable->name[varStrPos++] = text[textStrPos++];
                    }
                    break;

                case 1: variable->value[varStrPos++] = text[textStrPos++]; break;
            }
        }

        variable->access = ACCESS_PRIVATE;

        if (!ConvertStringToInteger(variable->value, &scriptCode[scriptCodePos]))
            scriptCode[scriptCodePos] = 0;

        StrCopy(variable->value, "local[");
        AppendIntegerToString(variable->value, scriptCodePos++);
        StrAdd(variable->value, "]");

#if !RETRO_USE_ORIGINAL_CODE
        for (int v = 0; v < scriptValueListCount; ++v) {
            if (StrComp(scriptValueList[v].name, variable->name))
                PrintLog("WARNING: Variable Name '%s' has already been used!", variable->name);
        }
#endif

        ++scriptValueListCount;
    }
}
bool CheckTableText(char *text)
{
    bool hasValues = false;

    if (FindStringToken(text, "publictable", 1) == 0) {
#if !RETRO_USE_ORIGINAL_CODE
        if (scriptValueListCount >= SCRIPT_VAR_COUNT) {
            SetupTextMenu(&gameMenu[0], 0);
            AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "TOO MANY ALIASES, STATIC");
            AddTextMenuEntry(&gameMenu[0], "VALUES, AND TABLES");
            Engine.gameMode = ENGINE_SCRIPTERROR;
            return false;
        }
#endif

        ScriptVariableInfo *variable = &scriptValueList[scriptValueListCount];
        MEM_ZEROP(variable);

        int textStrPos = 11;
        int varStrPos  = 0;

        while (text[textStrPos]) {
            if (text[textStrPos] == '[' || text[textStrPos] == ']') {
                variable->name[varStrPos] = 0;
                textStrPos++;
                break;
            }
            else {
                variable->name[varStrPos++] = text[textStrPos++];
            }
        }

        if (FindStringToken(text, "]", 1) < 1) {
            // has default values, we'll stop here and read stuff in a seperate mode
            scriptCode[scriptCodePos] = 0;
            StrCopy(variable->value, "");
            AppendIntegerToString(variable->value, scriptCodePos);
            scriptCodeOffset = scriptCodePos++;
            hasValues        = true;
        }
        else {
            // no default values, just an array size

            varStrPos = 0;
            while (text[textStrPos]) {
                if (text[textStrPos] == '[' || text[textStrPos] == ']') {
                    variable->value[varStrPos] = 0;
                    textStrPos++;
                    break;
                }
                else {
                    variable->value[varStrPos++] = text[textStrPos++];
                }
            }

            // array size can be an variable (alias), how cool!
            for (int v = 0; v < scriptValueListCount; ++v) {
                if (StrComp(variable->value, scriptValueList[v].name))
                    StrCopy(variable->value, scriptValueList[v].value);
            }

            if (!ConvertStringToInteger(variable->value, &scriptCode[scriptCodePos])) {
                scriptCode[scriptCodePos] = 1;
#if !RETRO_USE_ORIGINAL_CODE
                PrintLog("WARNING: Unable to parse table size!");
#endif
            }

            StrCopy(variable->value, "");
            AppendIntegerToString(variable->value, scriptCodePos);

            int valueCount = scriptCode[scriptCodePos++];
            for (int v = 0; v < valueCount; ++v) scriptCode[scriptCodePos++] = 0;
        }

        variable->access = ACCESS_PUBLIC;
        scriptValueListCount++;
    }
    else if (FindStringToken(text, "privatetable", 1) == 0) {
#if !RETRO_USE_ORIGINAL_CODE
        if (scriptValueListCount >= SCRIPT_VAR_COUNT) {
            SetupTextMenu(&gameMenu[0], 0);
            AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "TOO MANY ALIASES, STATIC");
            AddTextMenuEntry(&gameMenu[0], "VALUES, AND TABLES");
            Engine.gameMode = ENGINE_SCRIPTERROR;
            return false;
        }
#endif

        ScriptVariableInfo *variable = &scriptValueList[scriptValueListCount];
        MEM_ZEROP(variable);

        int textStrPos = 12;
        int varStrPos  = 0;

        while (text[textStrPos]) {
            if (text[textStrPos] == '[' || text[textStrPos] == ']') {
                variable->name[varStrPos] = 0;
                textStrPos++;
                break;
            }
            else {
                variable->name[varStrPos++] = text[textStrPos++];
            }
        }

        if (FindStringToken(text, "]", 1) < 1) {
            // has default values, we'll stop here and read stuff in a seperate mode
            scriptCode[scriptCodePos] = 0;
            StrCopy(variable->value, "");
            AppendIntegerToString(variable->value, scriptCodePos);
            scriptCodeOffset = scriptCodePos++;
            hasValues        = true;
        }
        else {
            // no default values, just an array size

            varStrPos = 0;
            while (text[textStrPos]) {
                if (text[textStrPos] == '[' || text[textStrPos] == ']') {
                    variable->value[varStrPos] = 0;
                    textStrPos++;
                    break;
                }
                else {
                    variable->value[varStrPos++] = text[textStrPos++];
                }
            }

            // array size can be an variable (alias), how cool!
            for (int v = 0; v < scriptValueListCount; ++v) {
                if (StrComp(variable->value, scriptValueList[v].name))
                    StrCopy(variable->value, scriptValueList[v].value);
            }

            if (!ConvertStringToInteger(variable->value, &scriptCode[scriptCodePos])) {
                scriptCode[scriptCodePos] = 1;
#if !RETRO_USE_ORIGINAL_CODE
                PrintLog("WARNING: Unable to parse table size!");
#endif
            }

            StrCopy(variable->value, "");
            AppendIntegerToString(variable->value, scriptCodePos);

            int valueCount = scriptCode[scriptCodePos++];
            for (int v = 0; v < valueCount; ++v) scriptCode[scriptCodePos++] = 0;
        }

        variable->access = ACCESS_PRIVATE;
        scriptValueListCount++;
    }

    return hasValues;
}
void ConvertArithmaticSyntax(char *text)
{
    int token  = 0;
    int offset = 0;
    int findID = 0;
    char dest[260];

    for (int i = FUNC_EQUAL; i <= FUNC_MOD; ++i) {
        findID = FindStringToken(text, scriptEvaluationTokens[i - 1], 1);
        if (findID > -1) {
            offset = findID;
            token  = i;
        }
    }

    if (token > 0) {
        StrCopy(dest, functions[token].name);
        StrAdd(dest, "(");
        findID = StrLength(dest);
        for (int i = 0; i < offset; ++i) dest[findID++] = text[i];
        if (functions[token].opcodeSize > 1) {
            dest[findID] = ',';
            int len      = StrLength(scriptEvaluationTokens[token - 1]);
            offset += len;
            ++findID;
            while (text[offset]) dest[findID++] = text[offset++];
        }
        dest[findID] = 0;
        StrAdd(dest, ")");
        StrCopy(text, dest);
    }
}
void ConvertConditionalStatement(char *text)
{
    char dest[260];
    int compareOp  = -1;
    int strPos     = 0;
    int destStrPos = 0;

    if (FindStringToken(text, "if", 1) == 0) {
        for (int i = 0; i < 6; ++i) {
            destStrPos = FindStringToken(text, scriptEvaluationTokens[i + FUNC_MOD], 1);
            if (destStrPos > -1) {
                strPos    = destStrPos;
                compareOp = i;
            }
        }

        if (compareOp > -1) {
            text[strPos] = ',';
            StrCopy(dest, functions[compareOp + FUNC_IFEQUAL].name);
            StrAdd(dest, "(");
            AppendIntegerToString(dest, jumpTablePos - jumpTableOffset);
            StrAdd(dest, ",");

            destStrPos = StrLength(dest);
            for (int i = 2; text[i]; ++i) {
                if (text[i] != '=' && text[i] != '(' && text[i] != ')')
                    dest[destStrPos++] = text[i];
            }
            dest[destStrPos] = 0;

            StrAdd(dest, ")");
            StrCopy(text, dest);

            jumpTableStack[++jumpTableStackPos] = jumpTablePos;
            jumpTable[jumpTablePos++]       = -1;
            jumpTable[jumpTablePos++]       = 0;
        }
    }
    else if (FindStringToken(text, "while", 1) == 0) {
        for (int i = 0; i < 6; ++i) {
            destStrPos = FindStringToken(text, scriptEvaluationTokens[i + FUNC_MOD], 1);
            if (destStrPos > -1) {
                strPos    = destStrPos;
                compareOp = i;
            }
        }

        if (compareOp > -1) {
            text[strPos] = ',';
            StrCopy(dest, functions[compareOp + FUNC_WEQUAL].name);
            StrAdd(dest, "(");
            AppendIntegerToString(dest, jumpTablePos - jumpTableOffset);
            StrAdd(dest, ",");

            destStrPos = StrLength(dest);
            for (int i = 5; text[i]; ++i) {
                if (text[i] != '=' && text[i] != '(' && text[i] != ')')
                    dest[destStrPos++] = text[i];
            }
            dest[destStrPos] = 0;

            StrAdd(dest, ")");
            StrCopy(text, dest);

            jumpTableStack[++jumpTableStackPos] = jumpTablePos;
            jumpTable[jumpTablePos++]       = scriptCodePos - scriptCodeOffset;
            jumpTable[jumpTablePos++]       = 0;
        }
    }
    else if (FindStringToken(text, "foreach", 1) == 0) {
        int argStrPos = FindStringToken(text, ",", 2);

        if (argStrPos > -1) {
            StrCopy(dest, functions[text[argStrPos + 2] == 'C' ? (int)FUNC_FOREACHACTIVE : (int)FUNC_FOREACHALL].name);
            StrAdd(dest, "(");
            AppendIntegerToString(dest, jumpTablePos - jumpTableOffset);
            StrAdd(dest, ",");

            destStrPos = StrLength(dest);
            for (int i = 7; text[i] && i < argStrPos; ++i) {
                if (text[i] != '(' && text[i] != ')')
                    dest[destStrPos++] = text[i];
            }
            dest[destStrPos] = 0;

            StrAdd(dest, ")");
            StrCopy(text, dest);

            jumpTableStack[++jumpTableStackPos] = jumpTablePos;
            jumpTable[jumpTablePos++]       = scriptCodePos - scriptCodeOffset;
            jumpTable[jumpTablePos++]       = 0;
        }
    }
}
bool ConvertSwitchStatement(char *text)
{
    if (FindStringToken(text, "switch", 1) != 0)
        return false;

    char switchText[260];
    StrCopy(switchText, "switch");
    StrAdd(switchText, "(");
    AppendIntegerToString(switchText, jumpTablePos - jumpTableOffset);
    StrAdd(switchText, ",");
    int pos = StrLength(switchText);
    for (int i = 6; text[i]; ++i) {
        if (text[i] != '=' && text[i] != '(' && text[i] != ')')
            switchText[pos++] = text[i];
    }
    switchText[pos] = 0;
    StrAdd(switchText, ")");
    StrCopy(text, switchText);
    jumpTableStack[++jumpTableStackPos] = jumpTablePos;
    jumpTable[jumpTablePos++]       = 0x10000;
    jumpTable[jumpTablePos++]       = -0x10000;
    jumpTable[jumpTablePos++]       = -1;
    jumpTable[jumpTablePos++]       = 0;

    return true;
}
void ConvertFunctionText(char *text)
{
    char arrayStr[0x80];
    char funcName[132];

    int opcode     = 0;
    int opcodeSize = 0;
    int textPos    = 0;
    int namePos    = 0;

    for (namePos = 0; text[namePos] != '(' && text[namePos]; ++namePos) funcName[namePos] = text[namePos];
    funcName[namePos] = 0;

    for (int i = 0; i < FUNC_MAX_CNT; ++i) {
        if (StrComp(funcName, functions[i].name)) {
            opcode     = i;
            opcodeSize = functions[i].opcodeSize;
            textPos    = StrLength(functions[i].name);
            i          = FUNC_MAX_CNT;
        }
    }

    if (opcode <= 0) {
        SetupTextMenu(&gameMenu[0], 0);
        AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
        AddTextMenuEntry(&gameMenu[0], " ");
        AddTextMenuEntry(&gameMenu[0], "OPCODE NOT FOUND");
        AddTextMenuEntry(&gameMenu[0], funcName);
#if !RETRO_USE_ORIGINAL_CODE
        AddTextMenuEntry(&gameMenu[0], " ");
        AddTextMenuEntry(&gameMenu[0], "LINE NUMBER");
        char buffer[0x10];
        buffer[0] = 0;
        AppendIntegerToString(buffer, lineID);
        AddTextMenuEntry(&gameMenu[0], buffer);
#endif
        Engine.gameMode = ENGINE_SCRIPTERROR;
    }
    else {
        scriptCode[scriptCodePos++] = opcode;
        if (StrComp("else", functions[opcode].name))
            jumpTable[jumpTableStack[jumpTableStackPos]] = scriptCodePos - scriptCodeOffset;

        if (StrComp("endif", functions[opcode].name) == 1) {
            int jPos                = jumpTableStack[jumpTableStackPos];
            jumpTable[jPos + 1] = scriptCodePos - scriptCodeOffset;
            if (jumpTable[jPos] == -1)
                jumpTable[jPos] = (scriptCodePos - scriptCodeOffset) - 1;
            --jumpTableStackPos;
        }

        if (StrComp("endswitch", functions[opcode].name)) {
            int jPos                = jumpTableStack[jumpTableStackPos];
            jumpTable[jPos + 3] = scriptCodePos - scriptCodeOffset;
            if (jumpTable[jPos + 2] == -1) {
                jumpTable[jPos + 2] = (scriptCodePos - scriptCodeOffset) - 1;
                int caseCnt             = abs(jumpTable[jPos + 1] - jumpTable[jPos]) + 1;

                int jOffset = jPos + 4;
                for (int c = 0; c < caseCnt; ++c) {
                    if (jumpTable[jOffset + c] < 0)
                        jumpTable[jOffset + c] = jumpTable[jPos + 2];
                }
            }
            --jumpTableStackPos;
        }

        if (StrComp("loop", functions[opcode].name) || StrComp("next", functions[opcode].name)) {
            jumpTable[jumpTableStack[jumpTableStackPos--] + 1] = scriptCodePos - scriptCodeOffset;
        }

        for (int i = 0; i < opcodeSize; ++i) {
            ++textPos;
            int funcNamePos = 0;
            int mode        = 0;
            int prevMode    = 0;
            int arrayStrPos = 0;
            while (((text[textPos] != ',' && text[textPos] != ')') || mode == 2) && text[textPos]) {
                switch (mode) {
                    case 0: // normal
                        if (text[textPos] == '[')
                            mode = 1;
                        else if (text[textPos] == '"') {
                            prevMode                = mode;
                            mode                    = 2;
                            funcName[funcNamePos++] = '"';
                        }
                        else
                            funcName[funcNamePos++] = text[textPos];
                        ++textPos;
                        break;

                    case 1: // array val
                        if (text[textPos] == ']')
                            mode = 0;
                        else if (text[textPos] == '"') {
                            prevMode = mode;
                            mode     = 2;
                        }
                        else
                            arrayStr[arrayStrPos++] = text[textPos];
                        ++textPos;
                        break;

                    case 2: // string
                        if (text[textPos] == '"') {
                            mode                    = prevMode;
                            funcName[funcNamePos++] = '"';
                        }
                        else
                            funcName[funcNamePos++] = text[textPos];
                        ++textPos;
                        break;
                }
            }
            funcName[funcNamePos] = 0;
            arrayStr[arrayStrPos] = 0;

            for (int v = 0; v < scriptValueListCount; ++v) {
                if (StrComp(funcName, scriptValueList[v].name)) {
                    CopyAliasStr(funcName, scriptValueList[v].value, 0);
                    if (FindStringToken(scriptValueList[v].value, "[", 1) > -1)
                        CopyAliasStr(arrayStr, scriptValueList[v].value, 1);
                }
            }

            if (arrayStr[0]) {
                char arrStrBuf[0x80];
                int arrPos = 0;
                int bufPos = 0;
                if (arrayStr[0] == '+' || arrayStr[0] == '-')
                    ++arrPos;
                while (arrayStr[arrPos]) arrStrBuf[bufPos++] = arrayStr[arrPos++];
                arrStrBuf[bufPos] = 0;

                for (int v = 0; v < scriptValueListCount; ++v) {
                    if (StrComp(arrStrBuf, scriptValueList[v].name)) {
                        char pref = arrayStr[0];
                        CopyAliasStr(arrayStr, scriptValueList[v].value, 0);

                        if (pref == '+' || pref == '-') {
                            int len = StrLength(arrayStr);
                            for (int i = len; i >= 0; --i) arrayStr[i + 1] = arrayStr[i];
                            arrayStr[0] = pref;
                        }
                    }
                }
            }

            // Eg: temp0 = game.variable
            for (int v = 0; v < globalVariablesCount; ++v) {
                if (StrComp(funcName, globalVariableNames[v])) {
                    StrCopy(funcName, "global");
                    arrayStr[0] = 0;
                    AppendIntegerToString(arrayStr, v);
                }
            }

            // Eg: temp0 = Function1
            for (int f = 0; f < scriptFunctionCount; ++f) {
                if (StrComp(funcName, scriptFunctionList[f].name)) {
                    funcName[0] = 0;
                    AppendIntegerToString(funcName, f);
                }
            }

            // Eg: temp0 = TypeName[Player Object]
            if (StrComp(funcName, "TypeName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int o = 0;
                for (; o < OBJECT_COUNT; ++o) {
                    if (StrComp(arrayStr, typeNames[o])) {
                        funcName[0] = 0;
                        AppendIntegerToString(funcName, o);
                        break;
                    }
                }

                if (o == OBJECT_COUNT)
                    PrintLog("WARNING: Unknown typename \"%s\", on line %d", arrayStr, lineID);
            }

#if !RETRO_USE_ORIGINAL_CODE
            // Eg: temp0 = SfxName[Jump]
            if (StrComp(funcName, "SfxName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int s = 0;
                for (; s < SFX_COUNT; ++s) {
                    if (StrComp(arrayStr, sfxNames[s])) {
                        funcName[0] = 0;
                        AppendIntegerToString(funcName, s);
                        break;
                    }
                }

                if (s == SFX_COUNT)
                    PrintLog("WARNING: Unknown sfxName \"%s\", on line %d", arrayStr, lineID);
            }

            // Eg: temp0 = VarName[player.lives]
            if (StrComp(funcName, "VarName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int v = 0;
                for (; v < globalVariablesCount; ++v) {
                    if (StrComp(arrayStr, globalVariableNames[v])) {
                        funcName[0] = 0;
                        AppendIntegerToString(funcName, v);
                        break;
                    }
                }

                if (v == globalVariablesCount)
                    PrintLog("WARNING: Unknown varName \"%s\", on line %d", arrayStr, lineID);
            }

            // Eg: temp0 = AchievementName[Ring King]
            if (StrComp(funcName, "AchievementName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int a = 0;
                for (; a < achievementCount; ++a) {
                    char buf[0x40];
                    char *str = achievements[a].name;
                    int pos   = 0;

                    while (*str) {
                        if (*str != ' ')
                            buf[pos++] = *str;
                        str++;
                    }
                    buf[pos] = 0;

                    if (StrComp(arrayStr, buf)) {
                        funcName[0] = 0;
                        AppendIntegerToString(funcName, a);
                        break;
                    }
                }

                if (a == achievementCount)
                    PrintLog("WARNING: Unknown AchievementName \"%s\", on line %d", arrayStr, lineID);
            }

            // Eg: temp0 = PlayerName[SONIC]
            if (StrComp(funcName, "PlayerName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int p = 0;
                for (; p < PLAYER_COUNT; ++p) {
                    char buf[0x40];
                    char *str = playerNames[p];
                    int pos   = 0;

                    while (*str) {
                        if (*str != ' ')
                            buf[pos++] = *str;
                        str++;
                    }
                    buf[pos] = 0;

                    if (StrComp(arrayStr, buf)) {
                        funcName[0] = 0;
                        AppendIntegerToString(funcName, p);
                        break;
                    }
                }

                if (p == PLAYER_COUNT)
                    PrintLog("WARNING: Unknown PlayerName \"%s\", on line %d", arrayStr, lineID);
            }

            // Eg: temp0 = StageName[R - GREEN HILL ZONE 1]
            if (StrComp(funcName, "StageName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int s = -1;
                if (StrLength(arrayStr) >= 2) {
                    char list = arrayStr[0];
                    switch (list) {
                        case 'P': list = STAGELIST_PRESENTATION; break;
                        case 'R': list = STAGELIST_REGULAR; break;
                        case 'S': list = STAGELIST_SPECIAL; break;
                        case 'B': list = STAGELIST_BONUS; break;
                    }
                    s = GetSceneID(list, &arrayStr[2]);
                }

                if (s == -1) {
                    char buf[0x40];
                    sprintf(buf, "WARNING: Unknown StageName \"%s\", on line %d", arrayStr, lineID);
                    PrintLog(buf);
                    s = 0;
                }
                funcName[0] = 0;
                AppendIntegerToString(funcName, s);
            }
#endif

            // Storing Values
            int constant = 0;
            if (ConvertStringToInteger(funcName, &constant)) {
                scriptCode[scriptCodePos++] = SCRIPTVAR_INTCONST;
                scriptCode[scriptCodePos++] = constant;
            }
            else if (funcName[0] == '"') {
                scriptCode[scriptCodePos++] = SCRIPTVAR_STRCONST;
                scriptCode[scriptCodePos++] = StrLength(funcName) - 2;

                int scriptTextPos = 1;
                arrayStrPos       = 0;
                while (scriptTextPos > -1) {
                    switch (arrayStrPos) {
                        case 0:
                            scriptCode[scriptCodePos] = funcName[scriptTextPos] << 24;
                            ++arrayStrPos;
                            break;

                        case 1:
                            scriptCode[scriptCodePos] += funcName[scriptTextPos] << 16;
                            ++arrayStrPos;
                            break;

                        case 2:
                            scriptCode[scriptCodePos] += funcName[scriptTextPos] << 8;
                            ++arrayStrPos;
                            break;

                        case 3:
                            scriptCode[scriptCodePos++] += funcName[scriptTextPos];
                            arrayStrPos = 0;
                            break;

                        default: break;
                    }

                    if (funcName[scriptTextPos] == '"') {
                        if (arrayStrPos > 0)
                            ++scriptCodePos;
                        scriptTextPos = -1;
                    }
                    else {
                        scriptTextPos++;
                    }
                }
            }
            else {
                scriptCode[scriptCodePos++] = SCRIPTVAR_VAR;

                if (arrayStr[0]) {
                    scriptCode[scriptCodePos] = VARARR_ARRAY;

                    if (arrayStr[0] == '+')
                        scriptCode[scriptCodePos] = VARARR_ENTNOPLUS1;

                    if (arrayStr[0] == '-')
                        scriptCode[scriptCodePos] = VARARR_ENTNOMINUS1;

                    ++scriptCodePos;

                    if (arrayStr[0] == '-' || arrayStr[0] == '+') {
                        for (int i = 0; i < StrLength(arrayStr); ++i) arrayStr[i] = arrayStr[i + 1];
                    }

                    if (ConvertStringToInteger(arrayStr, &constant) == 1) {
                        scriptCode[scriptCodePos++] = 0;
                        scriptCode[scriptCodePos++] = constant;
                    }
                    else {
                        if (StrComp(arrayStr, "arrayPos0"))
                            constant = 0;
                        if (StrComp(arrayStr, "arrayPos1"))
                            constant = 1;
                        if (StrComp(arrayStr, "arrayPos2"))
                            constant = 2;
                        if (StrComp(arrayStr, "arrayPos3"))
                            constant = 3;
                        if (StrComp(arrayStr, "arrayPos4"))
                            constant = 4;
                        if (StrComp(arrayStr, "arrayPos5"))
                            constant = 5;
                        if (StrComp(arrayStr, "arrayPos6"))
                            constant = 6;
                        if (StrComp(arrayStr, "arrayPos7"))
                            constant = 7;
                        if (StrComp(arrayStr, "tempObjectPos"))
                            constant = 8;

                        scriptCode[scriptCodePos++] = 1;
                        scriptCode[scriptCodePos++] = constant;
                    }
                }
                else {
                    scriptCode[scriptCodePos++] = VARARR_NONE;
                }

                constant = -1;
                for (int i = 0; i < VAR_MAX_CNT; ++i) {
                    if (StrComp(funcName, variableNames[i]))
                        constant = i;
                }

                if (constant == -1 && Engine.gameMode != ENGINE_SCRIPTERROR) {
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
                    AddTextMenuEntry(&gameMenu[0], " ");
                    AddTextMenuEntry(&gameMenu[0], "OPERAND NOT FOUND");
                    AddTextMenuEntry(&gameMenu[0], funcName);
                    AddTextMenuEntry(&gameMenu[0], " ");
                    AddTextMenuEntry(&gameMenu[0], "LINE NUMBER");
                    funcName[0] = 0;
                    AppendIntegerToString(funcName, lineID);
                    AddTextMenuEntry(&gameMenu[0], funcName);
                    Engine.gameMode = ENGINE_SCRIPTERROR;
                    constant        = 0;
                }

                scriptCode[scriptCodePos++] = constant;
            }
        }
    }
}
void CheckCaseNumber(char *text)
{
    if (FindStringToken(text, "case", 1) != 0)
        return;

    char caseString[128];
    char caseChar = text[4];

    int textPos    = 5;
    int caseStrPos = 0;
    while (caseChar) {
        if (caseChar != ':')
            caseString[caseStrPos++] = caseChar;
        caseChar = text[textPos++];
    }
    caseString[caseStrPos] = 0;

    bool foundValue = false;

    if (FindStringToken(caseString, "[", 1) >= 0) {
        char caseValue[0x80];
        char arrayStr[0x80];

        int textPos     = 0;
        int funcNamePos = 0;
        int mode        = 0;
        int arrayStrPos = 0;
        while (caseString[textPos] != ':' && caseString[textPos]) {
            if (mode) {
                if (caseString[textPos] == ']')
                    mode = 0;
                else
                    arrayStr[arrayStrPos++] = caseString[textPos];
                ++textPos;
            }
            else {
                if (caseString[textPos] == '[')
                    mode = 1;
                else
                    caseValue[funcNamePos++] = caseString[textPos];
                ++textPos;
            }
        }
        caseValue[funcNamePos] = 0;
        arrayStr[arrayStrPos]  = 0;

        // Eg: temp0 = TypeName[Player Object]
        if (StrComp(caseValue, "TypeName")) {
            caseValue[0] = '0';
            caseValue[1] = 0;

            int o = 0;
            for (; o < OBJECT_COUNT; ++o) {
                if (StrComp(arrayStr, typeNames[o])) {
                    caseValue[0] = 0;
                    AppendIntegerToString(caseValue, o);
                    break;
                }
            }

            if (o == OBJECT_COUNT)
                PrintLog("WARNING: Unknown typename \"%s\", on line %d", arrayStr, lineID);
        }

        // Eg: temp0 = SfxName[Jump]
        if (StrComp(caseValue, "SfxName")) {
            caseValue[0] = '0';
            caseValue[1] = 0;

            int s = 0;
            for (; s < SFX_COUNT; ++s) {
                if (StrComp(arrayStr, sfxNames[s])) {
                    caseValue[0] = 0;
                    AppendIntegerToString(caseValue, s);
                    break;
                }
            }

            if (s == SFX_COUNT)
                PrintLog("WARNING: Unknown sfxName \"%s\", on line %d", arrayStr, lineID);
        }

#if !RETRO_USE_ORIGINAL_CODE
        // Eg: temp0 = VarName[player.lives]
        if (StrComp(caseValue, "VarName")) {
            caseValue[0] = '0';
            caseValue[1] = 0;

            int v = 0;
            for (; v < globalVariablesCount; ++v) {
                if (StrComp(arrayStr, globalVariableNames[v])) {
                    caseValue[0] = 0;
                    AppendIntegerToString(caseValue, v);
                    break;
                }
            }

            if (v == globalVariablesCount) {
                PrintLog("WARNING: Unknown varName \"%s\", on line %d", arrayStr, lineID);
            }
        }

        // Eg: temp0 = AchievementName[Ring King]
        if (StrComp(caseValue, "AchievementName")) {
            caseValue[0] = '0';
            caseValue[1] = 0;

            int a = 0;
            for (; a < achievementCount; ++a) {
                char buf[0x40];
                char *str = achievements[a].name;
                int pos   = 0;

                while (*str) {
                    if (*str != ' ')
                        buf[pos++] = *str;
                    str++;
                }
                buf[pos] = 0;

                if (StrComp(arrayStr, buf)) {
                    caseValue[0] = 0;
                    AppendIntegerToString(caseValue, a);
                    break;
                }
            }

            if (a == achievementCount) {
                PrintLog("WARNING: Unknown AchievementName \"%s\", on line %d", arrayStr, lineID);
            }
        }

        // Eg: temp0 = PlayerName[SONIC]
        if (StrComp(caseValue, "PlayerName")) {
            caseValue[0] = '0';
            caseValue[1] = 0;

            int p = 0;
            for (; p < PLAYER_COUNT; ++p) {
                char buf[0x40];
                char *str = playerNames[p];
                int pos   = 0;

                while (*str) {
                    if (*str != ' ')
                        buf[pos++] = *str;
                    str++;
                }
                buf[pos] = 0;

                if (StrComp(arrayStr, buf)) {
                    caseValue[0] = 0;
                    AppendIntegerToString(caseValue, p);
                    break;
                }
            }

            if (p == PLAYER_COUNT) {
                PrintLog("WARNING: Unknown PlayerName \"%s\", on line %d", arrayStr, lineID);
            }
        }

        // Eg: temp0 = StageName[R - GREEN HILL ZONE 1]
        if (StrComp(caseValue, "StageName")) {
            caseValue[0] = '0';
            caseValue[1] = 0;

            int s = -1;
            if (StrLength(arrayStr) >= 2) {
                char list = arrayStr[0];
                switch (list) {
                    case 'P': list = STAGELIST_PRESENTATION; break;
                    case 'R': list = STAGELIST_REGULAR; break;
                    case 'S': list = STAGELIST_SPECIAL; break;
                    case 'B': list = STAGELIST_BONUS; break;
                }
                s = GetSceneID(list, &arrayStr[2]);
            }

            if (s == -1) {
                PrintLog("WARNING: Unknown StageName \"%s\", on line %d", arrayStr, lineID);
                s = 0;
            }
            caseValue[0] = 0;
            AppendIntegerToString(caseValue, s);
        }
#endif
        StrCopy(caseString, caseValue);
        foundValue = true;
    }

    for (int a = 0; a < scriptValueListCount && !foundValue; ++a) {
        if (StrComp(scriptValueList[a].name, caseString)) {
            StrCopy(caseString, scriptValueList[a].value);
            break;
        }
    }

    int caseID = 0;
    if (ConvertStringToInteger(caseString, &caseID)) {
        int stackValue = jumpTableStack[jumpTableStackPos];
        if (caseID < jumpTable[stackValue])
            jumpTable[stackValue] = caseID;
        stackValue++;
        if (caseID > jumpTable[stackValue])
            jumpTable[stackValue] = caseID;
    }
    else {
        PrintLog("WARNING: unable to convert case string \"%s\" to int, on line %d", caseString, lineID);
    }
}
bool ReadSwitchCase(char *text)
{
    char caseText[0x80];
    if (FindStringToken(text, "case", 1) == 0) {
        int textPos       = 4;
        int caseStringPos = 0;
        while (text[textPos]) {
            if (text[textPos] != ':')
                caseText[caseStringPos++] = text[textPos];
            ++textPos;
        }
        caseText[caseStringPos] = 0;

        bool foundValue = false;
        if (FindStringToken(caseText, "[", 1) >= 0) {
            char caseValue[0x80];
            char arrayStr[0x80];

            int textPos     = 0;
            int funcNamePos = 0;
            int mode        = 0;
            int arrayStrPos = 0;
            while (caseText[textPos] != ':' && caseText[textPos]) {
                if (mode) {
                    if (caseText[textPos] == ']')
                        mode = 0;
                    else
                        arrayStr[arrayStrPos++] = caseText[textPos];
                    ++textPos;
                }
                else {
                    if (caseText[textPos] == '[')
                        mode = 1;
                    else
                        caseValue[funcNamePos++] = caseText[textPos];
                    ++textPos;
                }
            }
            caseValue[funcNamePos] = 0;
            arrayStr[arrayStrPos]  = 0;

            // Eg: temp0 = TypeName[Player Object]
            if (StrComp(caseValue, "TypeName")) {
                caseValue[0] = '0';
                caseValue[1] = 0;

                int o = 0;
                for (; o < OBJECT_COUNT; ++o) {
                    if (StrComp(arrayStr, typeNames[o])) {
                        caseValue[0] = 0;
                        AppendIntegerToString(caseValue, o);
                        break;
                    }
                }

                if (o == OBJECT_COUNT)
                    PrintLog("WARNING: Unknown typename \"%s\", on line %d", arrayStr, lineID);
            }

            // Eg: temp0 = SfxName[Jump]
            if (StrComp(caseValue, "SfxName")) {
                caseValue[0] = '0';
                caseValue[1] = 0;

                int s = 0;
                for (; s < SFX_COUNT; ++s) {
                    if (StrComp(arrayStr, sfxNames[s])) {
                        caseValue[0] = 0;
                        AppendIntegerToString(caseValue, s);
                        break;
                    }
                }

                if (s == SFX_COUNT)
                    PrintLog("WARNING: Unknown sfxName \"%s\", on line %d", arrayStr, lineID);
            }

#if !RETRO_USE_ORIGINAL_CODE
            // Eg: temp0 = VarName[player.lives]
            if (StrComp(caseValue, "VarName")) {
                caseValue[0] = '0';
                caseValue[1] = 0;

                int v = 0;
                for (; v < globalVariablesCount; ++v) {
                    if (StrComp(arrayStr, globalVariableNames[v])) {
                        caseValue[0] = 0;
                        AppendIntegerToString(caseValue, v);
                        break;
                    }
                }

                if (v == globalVariablesCount)
                    PrintLog("WARNING: Unknown varName \"%s\", on line %d", arrayStr, lineID);
            }

            // Eg: temp0 = AchievementName[Ring King]
            if (StrComp(caseValue, "AchievementName")) {
                caseValue[0] = '0';
                caseValue[1] = 0;

                int a = 0;
                for (; a < achievementCount; ++a) {
                    char buf[0x40];
                    char *str = achievements[a].name;
                    int pos   = 0;

                    while (*str) {
                        if (*str != ' ')
                            buf[pos++] = *str;
                        str++;
                    }
                    buf[pos] = 0;

                    if (StrComp(arrayStr, buf)) {
                        caseValue[0] = 0;
                        AppendIntegerToString(caseValue, a);
                        break;
                    }
                }

                if (a == achievementCount)
                    PrintLog("WARNING: Unknown AchievementName \"%s\", on line %d", arrayStr, lineID);
            }

            // Eg: temp0 = PlayerName[SONIC]
            if (StrComp(caseValue, "PlayerName")) {
                caseValue[0] = '0';
                caseValue[1] = 0;

                int p = 0;
                for (; p < PLAYER_COUNT; ++p) {
                    char buf[0x40];
                    char *str = playerNames[p];
                    int pos   = 0;

                    while (*str) {
                        if (*str != ' ')
                            buf[pos++] = *str;
                        str++;
                    }
                    buf[pos] = 0;

                    if (StrComp(arrayStr, buf)) {
                        caseValue[0] = 0;
                        AppendIntegerToString(caseValue, p);
                        break;
                    }
                }

                if (p == PLAYER_COUNT)
                    PrintLog("WARNING: Unknown PlayerName \"%s\", on line %d", arrayStr, lineID);
            }

            // Eg: temp0 = StageName[R - GREEN HILL ZONE 1]
            if (StrComp(caseValue, "StageName")) {
                caseValue[0] = '0';
                caseValue[1] = 0;

                int s = -1;
                if (StrLength(arrayStr) >= 2) {
                    char list = arrayStr[0];
                    switch (list) {
                        case 'P': list = STAGELIST_PRESENTATION; break;
                        case 'R': list = STAGELIST_REGULAR; break;
                        case 'S': list = STAGELIST_SPECIAL; break;
                        case 'B': list = STAGELIST_BONUS; break;
                    }
                    s = GetSceneID(list, &arrayStr[2]);
                }

                if (s == -1) {
                    PrintLog("WARNING: Unknown StageName \"%s\", on line %d", arrayStr, lineID);
                    s = 0;
                }
                caseValue[0] = 0;
                AppendIntegerToString(caseValue, s);
            }
#endif
            StrCopy(caseText, caseValue);
            foundValue = true;
        }

        for (int v = 0; v < scriptValueListCount && !foundValue; ++v) {
            if (StrComp(caseText, scriptValueList[v].name)) {
                StrCopy(caseText, scriptValueList[v].value);
                break;
            }
        }

        int val = 0;

        int jPos    = jumpTableStack[jumpTableStackPos];
        int jOffset = jPos + 4;
        if (ConvertStringToInteger(caseText, &val))
            jumpTable[val - jumpTable[jPos] + jOffset] = scriptCodePos - scriptCodeOffset;
        else
            PrintLog("WARNING: unable to read case string \"%s\" as an int, on line %d", caseText, lineID);

        return true;
    }
    else if (FindStringToken(text, "default", 1) == 0) {
        int jumpTablepos                = jumpTableStack[jumpTableStackPos];
        jumpTable[jumpTablepos + 2] = scriptCodePos - scriptCodeOffset;
        int cnt                         = abs(jumpTable[jumpTablepos + 1] - jumpTable[jumpTablepos]) + 1;

        int jOffset = jumpTablepos + 4;
        for (int i = 0; i < cnt; ++i) {
            if (jumpTable[jOffset + i] < 0)
                jumpTable[jOffset + i] = scriptCodePos - scriptCodeOffset;
        }

        return true;
    }

    return false;
}
void ReadTableValues(char *text)
{
    int textStrPos = 0;

    char valueBuffer[256];
    int valueBufferPos = 0;

    while (text[textStrPos]) {
        valueBuffer[valueBufferPos++] = text[textStrPos++];

        while (text[textStrPos] == ',') {
            valueBuffer[valueBufferPos] = 0;
            ++scriptCode[scriptCodeOffset];
            if (!ConvertStringToInteger(valueBuffer, &scriptCode[scriptCodePos])) {
                scriptCode[scriptCodePos] = 0;
#if !RETRO_USE_ORIGINAL_CODE
                PrintLog("WARNING: unable to parse table value \"%s\" as an int, on line %d", valueBuffer, lineID);
#endif
            }
            scriptCodePos++;
            valueBufferPos = 0;
            textStrPos++;
        }
    }

    if (StrLength(valueBuffer)) {
        valueBuffer[valueBufferPos] = 0;
        ++scriptCode[scriptCodeOffset];
        if (!ConvertStringToInteger(valueBuffer, &scriptCode[scriptCodePos])) {
            scriptCode[scriptCodePos] = 0;
#if !RETRO_USE_ORIGINAL_CODE
            PrintLog("WARNING: unable to parse table value \"%s\" as an int, on line %d", valueBuffer, lineID);
#endif
        }
        scriptCodePos++;
    }
}
void AppendIntegerToString(char *text, int value)
{
    int textPos = 0;
    while (true) {
        if (!text[textPos])
            break;
        ++textPos;
    }

    int cnt = 0;
    int v   = value;
    while (v != 0) {
        v /= 10;
        cnt++;
    }

    v = 0;
    for (int i = cnt - 1; i >= 0; --i) {
        v = value / pow(10, i);
        v %= 10;

        int strValue = v + '0';
        if (strValue < '0' || strValue > '9') {
            // what
        }
        text[textPos++] = strValue;
    }
    if (value == 0)
        text[textPos++] = '0';
    text[textPos] = 0;
}
void AppendIntegerToStringW(ushort *text, int value)
{
    int textPos = 0;
    while (true) {
        if (!text[textPos])
            break;
        ++textPos;
    }

    int cnt = 0;
    int v   = value;
    while (v != 0) {
        v /= 10;
        cnt++;
    }

    v = 0;
    for (int i = cnt - 1; i >= 0; --i) {
        v = value / pow(10, i);
        v %= 10;

        int strValue = v + '0';
        if (strValue < '0' || strValue > '9') {
            // what
        }
        text[textPos++] = strValue;
    }
    if (value == 0)
        text[textPos++] = '0';
    text[textPos] = 0;
}
#endif

bool ConvertStringToInteger(const char *text, int *value)
{
    int charID    = 0;
    bool negative = false;
    int base      = 10;
    *value        = 0;
    if (*text != '+' && !(*text >= '0' && *text <= '9') && *text != '-')
        return false;
    int strLength = StrLength(text) - 1;
    uint charVal  = 0;
    if (*text == '-') {
        negative = true;
        charID   = 1;
        --strLength;
    }
    else if (*text == '+') {
        charID = 1;
        --strLength;
    }

    if (text[charID] == '0') {
        if (text[charID + 1] == 'x' || text[charID + 1] == 'X')
            base = 0x10;
#if !RETRO_USE_ORIGINAL_CODE
        else if (text[charID + 1] == 'b' || text[charID + 1] == 'B')
            base = 0b10;
        else if (text[charID + 1] == 'o' || text[charID + 1] == 'O')
            base = 0010; // base 8
#endif

        if (base != 10) {
            charID += 2;
            strLength -= 2;
        }
    }

    while (strLength > -1) {
        bool flag = text[charID] < '0';
        if (!flag) {
            if (base == 0x10 && text[charID] > 'f')
                flag = true;
#if !RETRO_USE_ORIGINAL_CODE
            if (base == 0010 && text[charID] > '7')
                flag = true;
            if (base == 0b10 && text[charID] > '1')
                flag = true;
#endif
        }

        if (flag) {
            return 0;
        }
        if (strLength <= 0) {
            if (text[charID] >= '0' && text[charID] <= '9') {
                *value = text[charID] + *value - '0';
            }
            else if (text[charID] >= 'a' && text[charID] <= 'f') {
                charVal = text[charID] - 'a';
                charVal += 10;
                *value += charVal;
            }
            else if (text[charID] >= 'A' && text[charID] <= 'F') {
                charVal = text[charID] - 'A';
                charVal += 10;
                *value += charVal;
            }
        }
        else {
            int strlen = strLength + 1;
            charVal    = 0;
            if (text[charID] >= '0' && text[charID] <= '9') {
                charVal = text[charID] - '0';
            }
            else if (text[charID] >= 'a' && text[charID] <= 'f') {
                charVal = text[charID] - 'a';
                charVal += 10;
            }
            else if (text[charID] >= 'A' && text[charID] <= 'F') {
                charVal = text[charID] - 'A';
                charVal += 10;
            }
            for (; --strlen; charVal *= base)
                ;
            *value += charVal;
        }
        --strLength;
        ++charID;
    }

    if (negative)
        *value = -*value;

    return true;
}

#if RETRO_USE_COMPILER
void CopyAliasStr(char *dest, char *text, bool arrayIndex)
{
    int textPos     = 0;
    int destPos     = 0;
    bool arrayValue = false;
    if (arrayIndex) {
        while (text[textPos]) {
            if (arrayValue) {
                if (text[textPos] == ']')
                    arrayValue = false;
                else
                    dest[destPos++] = text[textPos];
                ++textPos;
            }
            else {
                if (text[textPos] == '[')
                    arrayValue = true;
                ++textPos;
            }
        }
    }
    else {
        while (text[textPos]) {
            if (arrayValue) {
                if (text[textPos] == ']')
                    arrayValue = false;
                ++textPos;
            }
            else {
                if (text[textPos] == '[')
                    arrayValue = true;
                else
                    dest[destPos++] = text[textPos];
                ++textPos;
            }
        }
    }
    dest[destPos] = 0;
}
bool CheckOpcodeType(char *text)
{
    while (true) {
        int c = *text;
        if (!*text)
            break;
        ++text;
        if (c == '(')
            return false;
    }
    return true;
}

void ParseScriptFile(char *scriptName, int scriptID)
{
    jumpTableStackPos = 0;
    lineID            = 0;

    for (int f = 0; f < scriptFunctionCount; ++f) {
        if (scriptFunctionList[f].access != ACCESS_PUBLIC)
            StrCopy(scriptFunctionList[f].name, "");
    }

    int newScriptValueCount = COMMON_SCRIPT_VAR_COUNT;
    for (int v = COMMON_SCRIPT_VAR_COUNT; v < scriptValueListCount; ++v) {
        if (scriptValueList[v].access != ACCESS_PUBLIC) {
            StrCopy(scriptValueList[v].name, "");
        }
        else {
            if (newScriptValueCount != v)
                memcpy(&scriptValueList[newScriptValueCount], &scriptValueList[v], sizeof(ScriptVariableInfo));

            newScriptValueCount++;
        }
    }
    scriptValueListCount = newScriptValueCount;

    for (int v = scriptValueListCount; v < SCRIPT_VAR_COUNT; ++v) {
        MEM_ZERO(scriptValueList[v]);
    }

    FileInfo info;
    char scriptPath[0x40];

    // Try the original script folder
    StrCopy(scriptPath, "Data/Scripts/");
    StrAdd(scriptPath, scriptName);
    if (LoadFile(scriptPath, &info)) {
        int readMode   = READMODE_NORMAL;
        int parseMode  = PARSEMODE_SCOPELESS;
        char prevChar  = 0;
        char curChar   = 0;
        int switchDeep = 0;

        while (readMode < READMODE_EOF) {
            int textPos               = 0;
            readMode                  = READMODE_NORMAL;
            bool disableLineIncrement = false;

            while (readMode < READMODE_ENDLINE) {
                prevChar = curChar;
                FileRead(&curChar, 1);
                if (readMode == READMODE_STRING) {
                    if (curChar == '\t' || curChar == '\r' || curChar == '\n' || curChar == ';' || readMode >= READMODE_COMMENTLINE) {
                        if ((curChar == '\n' && prevChar != '\r') || (curChar == '\n' && prevChar == '\r')) {
                            readMode            = READMODE_ENDLINE;
                            scriptText[textPos] = 0;
                            if (curChar == ';')
                                disableLineIncrement = true;
                        }
                    }
                    else if (curChar != '/' || textPos <= 0) {
                        scriptText[textPos++] = curChar;
                        if (curChar == '"')
                            readMode = READMODE_NORMAL;
                    }
                    else if (curChar == '/' && prevChar == '/') {
                        readMode              = READMODE_COMMENTLINE;
                        scriptText[--textPos] = 0;
                    }
                    else {
                        scriptText[textPos++] = curChar;
                    }
                }
                else if (curChar == ' ' || curChar == '\t' || curChar == '\r' || curChar == '\n' || curChar == ';'
                         || readMode >= READMODE_COMMENTLINE) {
                    if ((curChar == '\n' && prevChar != '\r') || (curChar == '\n' && prevChar == '\r') || curChar == ';') {
                        readMode            = READMODE_ENDLINE;
                        scriptText[textPos] = 0;
                        if (curChar == ';')
                            disableLineIncrement = true;
                    }
                }
                else if (curChar != '/' || textPos <= 0) {
                    scriptText[textPos++] = curChar;
                    if (curChar == '"' && !readMode)
                        readMode = READMODE_STRING;
                }
                else if (curChar == '/' && prevChar == '/') {
                    readMode              = READMODE_COMMENTLINE;
                    scriptText[--textPos] = 0;
                }
                else {
                    scriptText[textPos++] = curChar;
                }
                if (ReachedEndOfFile()) {
                    scriptText[textPos] = 0;
                    readMode            = READMODE_EOF;
                }
            }

            switch (parseMode) {
                case PARSEMODE_SCOPELESS:
                    if (!disableLineIncrement)
                        ++lineID;

                    CheckAliasText(scriptText);
                    CheckStaticText(scriptText);

                    if (CheckTableText(scriptText)) {
                        parseMode = PARSEMODE_TABLEREAD;
                        StrCopy(scriptText, "");
                    }

                    if (StrComp(scriptText, "eventObjectUpdate")) {
                        parseMode                                          = PARSEMODE_FUNCTION;
                        objectScriptList[scriptID].eventUpdate.scriptCodePtr = scriptCodePos;
                        objectScriptList[scriptID].eventUpdate.jumpTablePtr  = jumpTablePos;
                        scriptCodeOffset                                   = scriptCodePos;
                        jumpTableOffset                                    = jumpTablePos;
                    }

                    if (StrComp(scriptText, "eventObjectDraw")) {
                        parseMode                                          = PARSEMODE_FUNCTION;
                        objectScriptList[scriptID].eventDraw.scriptCodePtr = scriptCodePos;
                        objectScriptList[scriptID].eventDraw.jumpTablePtr  = jumpTablePos;
                        scriptCodeOffset                                   = scriptCodePos;
                        jumpTableOffset                                    = jumpTablePos;
                    }

                    if (StrComp(scriptText, "eventObjectStartup")) {
                        parseMode                                             = PARSEMODE_FUNCTION;
                        objectScriptList[scriptID].eventStartup.scriptCodePtr = scriptCodePos;
                        objectScriptList[scriptID].eventStartup.jumpTablePtr  = jumpTablePos;
                        scriptCodeOffset                                      = scriptCodePos;
                        jumpTableOffset                                       = jumpTablePos;
                    }

                    if (FindStringToken(scriptText, "reservefunction", 1) == 0) { // forward decl
                        char funcName[0x40];
                        for (textPos = 15; scriptText[textPos]; ++textPos) funcName[textPos - 15] = scriptText[textPos];
                        funcName[textPos - 15] = 0;
                        int funcID             = -1;
                        for (int f = 0; f < scriptFunctionCount; ++f) {
                            if (StrComp(funcName, scriptFunctionList[f].name))
                                funcID = f;
                        }

                        if (scriptFunctionCount < FUNCTION_COUNT && funcID == -1) {
                            StrCopy(scriptFunctionList[scriptFunctionCount++].name, funcName);
                        }
                        else {
                            PrintLog("WARNING: Function %s has already been reserved!", funcName);
                        }

                        parseMode = PARSEMODE_SCOPELESS;
                    }
                    else if (FindStringToken(scriptText, "publicfunction", 1) == 0) { // regular public decl
                        char funcName[0x40];
                        for (textPos = 14; scriptText[textPos]; ++textPos) funcName[textPos - 14] = scriptText[textPos];

                        funcName[textPos - 14] = 0;
                        int funcID             = -1;
                        for (int f = 0; f < scriptFunctionCount; ++f) {
                            if (StrComp(funcName, scriptFunctionList[f].name))
                                funcID = f;
                        }

                        if (funcID <= -1) {
                            if (scriptFunctionCount >= FUNCTION_COUNT) {
                                parseMode = PARSEMODE_SCOPELESS;
                            }
                            else {
                                StrCopy(scriptFunctionList[scriptFunctionCount].name, funcName);
                                scriptFunctionList[scriptFunctionCount].access            = ACCESS_PUBLIC;
                                scriptFunctionList[scriptFunctionCount].ptr.scriptCodePtr = scriptCodePos;
                                scriptFunctionList[scriptFunctionCount].ptr.jumpTablePtr  = jumpTablePos;

                                scriptCodeOffset = scriptCodePos;
                                jumpTableOffset  = jumpTablePos;
                                parseMode        = PARSEMODE_FUNCTION;
                                ++scriptFunctionCount;
                            }
                        }
                        else {
                            StrCopy(scriptFunctionList[funcID].name, funcName);
                            scriptFunctionList[funcID].access            = ACCESS_PUBLIC;
                            scriptFunctionList[funcID].ptr.scriptCodePtr = scriptCodePos;
                            scriptFunctionList[funcID].ptr.jumpTablePtr  = jumpTablePos;

                            scriptCodeOffset = scriptCodePos;
                            jumpTableOffset  = jumpTablePos;
                            parseMode        = PARSEMODE_FUNCTION;
                        }
                    }
                    else if (FindStringToken(scriptText, "privatefunction", 1) == 0) { // regular private decl
                        char funcName[0x40];
                        for (textPos = 15; scriptText[textPos]; ++textPos) funcName[textPos - 15] = scriptText[textPos];

                        funcName[textPos - 15] = 0;
                        int funcID             = -1;
                        for (int f = 0; f < scriptFunctionCount; ++f) {
                            if (StrComp(funcName, scriptFunctionList[f].name))
                                funcID = f;
                        }

                        if (funcID <= -1) {
                            if (scriptFunctionCount >= FUNCTION_COUNT) {
                                parseMode = PARSEMODE_SCOPELESS;
                            }
                            else {
                                StrCopy(scriptFunctionList[scriptFunctionCount].name, funcName);
                                scriptFunctionList[scriptFunctionCount].access            = ACCESS_PRIVATE;
                                scriptFunctionList[scriptFunctionCount].ptr.scriptCodePtr = scriptCodePos;
                                scriptFunctionList[scriptFunctionCount].ptr.jumpTablePtr  = jumpTablePos;

                                scriptCodeOffset = scriptCodePos;
                                jumpTableOffset  = jumpTablePos;
                                parseMode        = PARSEMODE_FUNCTION;
                                ++scriptFunctionCount;
                            }
                        }
                        else {
                            StrCopy(scriptFunctionList[funcID].name, funcName);
                            scriptFunctionList[funcID].access            = ACCESS_PRIVATE;
                            scriptFunctionList[funcID].ptr.scriptCodePtr = scriptCodePos;
                            scriptFunctionList[funcID].ptr.jumpTablePtr  = jumpTablePos;

                            scriptCodeOffset = scriptCodePos;
                            jumpTableOffset  = jumpTablePos;
                            parseMode        = PARSEMODE_FUNCTION;
                        }
                    }
                    break;

                case PARSEMODE_PLATFORMSKIP:
                    if (!disableLineIncrement)
                        ++lineID;

                    if (FindStringToken(scriptText, "#endplatform", 1) == 0)
                        parseMode = PARSEMODE_FUNCTION;
                    break;

                case PARSEMODE_FUNCTION:
                    if (!disableLineIncrement)
                        ++lineID;

                    if (scriptText[0]) {
                        if (StrComp(scriptText, "endevent")) {
                            scriptCode[scriptCodePos++] = FUNC_END;
                            parseMode                   = PARSEMODE_SCOPELESS;
                        }
                        else if (StrComp(scriptText, "endfunction")) {
                            scriptCode[scriptCodePos++] = FUNC_RETURN;
                            parseMode                   = PARSEMODE_SCOPELESS;
                        }
                        else if (FindStringToken(scriptText, "#platform:", 1) == 0) {
                            if (FindStringToken(scriptText, Engine.gamePlatform, 1) == -1
                                && FindStringToken(scriptText, Engine.gameRenderType, 1) == -1
#if RETRO_USE_HAPTICS
                                && FindStringToken(scriptText, Engine.gameHapticSetting, 1) == -1
#endif
#if !RETRO_USE_ORIGINAL_CODE && RETRO_REV03
                                && FindStringToken(scriptText, Engine.releaseType, 1) == -1 // general flag for standalone/origins content switching
#endif
#if !RETRO_USE_ORIGINAL_CODE
                                && FindStringToken(scriptText, "USE_DECOMP", 1) == -1 // general flag for decomp-only stuff
#endif
#if RETRO_USE_NETWORKING
                                && FindStringToken(scriptText, "USE_NETWORKING", 1) == -1
#endif
#if RETRO_USE_MOD_LOADER
                                && FindStringToken(scriptText, "USE_MOD_LOADER", 1) == -1
#endif
                            ) {
                                parseMode = PARSEMODE_PLATFORMSKIP;
                            }
                        }
                        else if (FindStringToken(scriptText, "#endplatform", 1) == -1) {
                            ConvertConditionalStatement(scriptText);
                            if (ConvertSwitchStatement(scriptText)) {
                                parseMode    = PARSEMODE_SWITCHREAD;
                                info.readPos = (int)GetFilePosition();
                                switchDeep   = 0;
                            }
                            ConvertArithmaticSyntax(scriptText);
                            if (!ReadSwitchCase(scriptText)) {
                                ConvertFunctionText(scriptText);
                                if (Engine.gameMode == ENGINE_SCRIPTERROR) {
                                    AddTextMenuEntry(&gameMenu[0], " ");
                                    AddTextMenuEntry(&gameMenu[0], "ERROR IN");
                                    AddTextMenuEntry(&gameMenu[0], scriptName);
                                    parseMode = PARSEMODE_ERROR;
                                }
                            }
                        }
                    }
                    break;

                case PARSEMODE_SWITCHREAD:
                    if (FindStringToken(scriptText, "switch", 1) == 0)
                        ++switchDeep;

                    if (switchDeep) {
                        if (FindStringToken(scriptText, "endswitch", 1) == 0)
                            --switchDeep;
                    }
                    else if (FindStringToken(scriptText, "endswitch", 1) == 0) {
                        SetFilePosition(info.readPos);
                        parseMode  = PARSEMODE_FUNCTION;
                        int jPos   = jumpTableStack[jumpTableStackPos];
                        switchDeep = abs(jumpTable[jPos + 1] - jumpTable[jPos]) + 1;
                        for (textPos = 0; textPos < switchDeep; ++textPos) jumpTable[jumpTablePos++] = -1;
                    }
                    else {
                        CheckCaseNumber(scriptText);
                    }
                    break;

                case PARSEMODE_TABLEREAD:
                    if (!disableLineIncrement)
                        ++lineID;

                    if (FindStringToken(scriptText, "endtable", 1) == 0) {
                        parseMode = PARSEMODE_SCOPELESS;
                    }
                    else {
                        if (StrLength(scriptText) >= 1)
                            ReadTableValues(scriptText);

                        parseMode = PARSEMODE_TABLEREAD;
                    }
                    break;

                default: break;
            }
        }

        CloseFile();
    }
}
#endif

void LoadBytecode(int stageListID, int scriptID)
{
    char scriptPath[0x40];
    switch (stageListID) {
        case STAGELIST_PRESENTATION:
        case STAGELIST_REGULAR:
        case STAGELIST_BONUS:
        case STAGELIST_SPECIAL:
            StrCopy(scriptPath, "Bytecode/");
            StrAdd(scriptPath, stageList[stageListID][stageListPosition].folder);
            StrAdd(scriptPath, ".bin");
            break;
        case 4: StrCopy(scriptPath, "Bytecode/GlobalCode.bin"); break;
        default: break;
    }

    FileInfo info;
    if (LoadFile(scriptPath, &info)) {
        byte fileBuffer    = 0;
        int *scriptCodePtr = &scriptCode[scriptCodePos];
        int *jumpTablePtr  = &jumpTable[jumpTablePos];

        FileRead(&fileBuffer, 1);
        int scriptCodeSize = fileBuffer;
        FileRead(&fileBuffer, 1);
        scriptCodeSize |= fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        scriptCodeSize |= fileBuffer << 16;
        FileRead(&fileBuffer, 1);
        scriptCodeSize |= fileBuffer << 24;

        while (scriptCodeSize > 0) {
            FileRead(&fileBuffer, 1);
            int blockSize = fileBuffer & 0x7F;
            if (fileBuffer >= 0x80) {
                while (blockSize > 0) {
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr = fileBuffer;
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr |= fileBuffer << 8;
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr |= fileBuffer << 16;
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr |= fileBuffer << 24;

                    ++scriptCodePtr;
                    ++scriptCodePos;
                    --scriptCodeSize;
                    --blockSize;
                }
            }
            else {
                while (blockSize > 0) {
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr = fileBuffer;

                    ++scriptCodePtr;
                    ++scriptCodePos;
                    --scriptCodeSize;
                    --blockSize;
                }
            }
        }

        FileRead(&fileBuffer, 1);
        int jumpTableSize = fileBuffer;
        FileRead(&fileBuffer, 1);
        jumpTableSize |= fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        jumpTableSize |= fileBuffer << 16;
        FileRead(&fileBuffer, 1);
        jumpTableSize |= fileBuffer << 24;

        while (jumpTableSize > 0) {
            FileRead(&fileBuffer, 1);
            int blockSize = fileBuffer & 0x7F;

            if (fileBuffer >= 0x80) {
                while (blockSize > 0) {
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr = fileBuffer;
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr |= fileBuffer << 8;
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr |= fileBuffer << 16;
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr |= fileBuffer << 24;

                    ++jumpTablePtr;
                    ++jumpTablePos;
                    --jumpTableSize;
                    --blockSize;
                }
            }
            else {
                while (blockSize > 0) {
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr = fileBuffer;

                    ++jumpTablePtr;
                    ++jumpTablePos;
                    --jumpTableSize;
                    --blockSize;
                }
            }
        }

        FileRead(&fileBuffer, 1);
        int scriptCount = fileBuffer;
        FileRead(&fileBuffer, 1);
        scriptCount |= fileBuffer << 8;

        for (int s = 0; s < scriptCount; ++s) {
            ObjectScript *script = &objectScriptList[scriptID + s];

            FileRead(&fileBuffer, 1);
            script->eventUpdate.scriptCodePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->eventUpdate.scriptCodePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->eventUpdate.scriptCodePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->eventUpdate.scriptCodePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->eventDraw.scriptCodePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->eventDraw.scriptCodePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->eventDraw.scriptCodePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->eventDraw.scriptCodePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->eventStartup.scriptCodePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->eventStartup.scriptCodePtr |= (fileBuffer << 8);
            FileRead(&fileBuffer, 1);
            script->eventStartup.scriptCodePtr |= (fileBuffer << 16);
            FileRead(&fileBuffer, 1);
            script->eventStartup.scriptCodePtr |= fileBuffer << 24;
        }

        for (int s = 0; s < scriptCount; ++s) {
            ObjectScript *script = &objectScriptList[scriptID + s];

            FileRead(&fileBuffer, 1);
            script->eventUpdate.jumpTablePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->eventUpdate.jumpTablePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->eventUpdate.jumpTablePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->eventUpdate.jumpTablePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->eventDraw.jumpTablePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->eventDraw.jumpTablePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->eventDraw.jumpTablePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->eventDraw.jumpTablePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->eventStartup.jumpTablePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->eventStartup.jumpTablePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->eventStartup.jumpTablePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->eventStartup.jumpTablePtr |= fileBuffer << 24;
        }

        FileRead(&fileBuffer, 1);
        int functionCount = fileBuffer;
        FileRead(&fileBuffer, 1);
        functionCount |= fileBuffer << 8;

        for (int f = 0; f < functionCount; ++f) {
            ScriptFunction *function = &scriptFunctionList[f];

            FileRead(&fileBuffer, 1);
            function->ptr.scriptCodePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            function->ptr.scriptCodePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            function->ptr.scriptCodePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            function->ptr.scriptCodePtr |= fileBuffer << 24;
        }

        for (int f = 0; f < functionCount; ++f) {
            ScriptFunction *function = &scriptFunctionList[f];

            FileRead(&fileBuffer, 1);
            function->ptr.jumpTablePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            function->ptr.jumpTablePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            function->ptr.jumpTablePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            function->ptr.jumpTablePtr |= fileBuffer << 24;
        }

        CloseFile();
    }
}

void ClearScriptData()
{
    memset(scriptCode, 0, sizeof(scriptCode));
    memset(jumpTable, 0, sizeof(jumpTable));

    memset(foreachStack, -1, sizeof(foreachStack));
    memset(jumpTableStack, 0, sizeof(jumpTableStack));
    memset(functionStack, 0, sizeof(functionStack));

    scriptFrameCount = 0;

    scriptCodePos     = 0;
    jumpTablePos      = 0;
    jumpTableStackPos = 0;
    functionStackPos  = 0;

    scriptCodePos    = 0;
    scriptCodeOffset = 0;
    jumpTablePos     = 0;
    jumpTableOffset  = 0;

#if RETRO_USE_COMPILER
    scriptFunctionCount = 0;

    lineID = 0;

    scriptValueListCount = COMMON_SCRIPT_VAR_COUNT;
    for (int v = COMMON_SCRIPT_VAR_COUNT; v < SCRIPT_VAR_COUNT; ++v) {
        MEM_ZERO(scriptValueList[v]);
    }
#endif

    ClearAnimationData();

    for (int o = 0; o < OBJECT_COUNT; ++o) {
        ObjectScript *scriptInfo               = &objectScriptList[o];
        scriptInfo->eventUpdate.scriptCodePtr  = SCRIPTCODE_COUNT - 1;
        scriptInfo->eventUpdate.jumpTablePtr   = JUMPTABLE_COUNT - 1;
        scriptInfo->eventDraw.scriptCodePtr    = SCRIPTCODE_COUNT - 1;
        scriptInfo->eventDraw.jumpTablePtr     = JUMPTABLE_COUNT - 1;
        scriptInfo->eventStartup.scriptCodePtr = SCRIPTCODE_COUNT - 1;
        scriptInfo->eventStartup.jumpTablePtr  = JUMPTABLE_COUNT - 1;
        scriptInfo->frameListOffset            = 0;
        scriptInfo->spriteSheetID              = 0;
        scriptInfo->animFile                   = GetDefaultAnimationRef();
        typeNames[o][0]                        = 0;
    }

    for (int s = globalSFXCount; s < globalSFXCount + stageSFXCount; ++s) {
        sfxNames[s][0] = 0;
    }

    for (int f = 0; f < FUNCTION_COUNT; ++f) {
        scriptFunctionList[f].ptr.scriptCodePtr = SCRIPTCODE_COUNT - 1;
        scriptFunctionList[f].ptr.jumpTablePtr  = JUMPTABLE_COUNT - 1;
    }

    SetObjectTypeName("Blank Object", OBJ_TYPE_BLANKOBJECT);
}

void ProcessScript(int scriptCodeStart, int jumpTableStart, byte scriptEvent)
{
    bool running      = true;
    int scriptCodePtr = scriptCodeStart;
    jumpTableStackPos = 0;
    functionStackPos  = 0;
    foreachStackPos   = 0;

    while (running) {
        int opcode           = scriptCode[scriptCodePtr++];
        int opcodeSize       = functions[opcode].opcodeSize;
        int scriptCodeOffset = scriptCodePtr;

        scriptText[0] = '\0';

        // Get Values
        for (int i = 0; i < opcodeSize; ++i) {
            int opcodeType = scriptCode[scriptCodePtr++];

            if (opcodeType == SCRIPTVAR_VAR) {
                int arrayVal = 0;
                switch (scriptCode[scriptCodePtr++]) {
                    case VARARR_NONE: arrayVal = objectEntityPos; break;

                    case VARARR_ARRAY:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = scriptEng.arrayPosition[scriptCode[scriptCodePtr++]];
                        else
                            arrayVal = scriptCode[scriptCodePtr++];
                        break;

                    case VARARR_ENTNOPLUS1:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = scriptEng.arrayPosition[scriptCode[scriptCodePtr++]] + objectEntityPos;
                        else
                            arrayVal = scriptCode[scriptCodePtr++] + objectEntityPos;
                        break;

                    case VARARR_ENTNOMINUS1:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = objectEntityPos - scriptEng.arrayPosition[scriptCode[scriptCodePtr++]];
                        else
                            arrayVal = objectEntityPos - scriptCode[scriptCodePtr++];
                        break;

                    default: break;
                }

#if RETRO_REV03 && !RETRO_USE_ORIGINAL_CODE
                bool inputCheck = true; // Default to true for mobile bytecode
                // If we're using the scripts or an Origins datafile, check the array value
                if (forceUseScripts || Engine.usingOrigins)
                    inputCheck = arrayVal <= 1;
#endif

                // Variables
                switch (scriptCode[scriptCodePtr++]) {
                    default: break;
                    case VAR_TEMP0: scriptEng.operands[i] = scriptEng.temp[0]; break;
                    case VAR_TEMP1: scriptEng.operands[i] = scriptEng.temp[1]; break;
                    case VAR_TEMP2: scriptEng.operands[i] = scriptEng.temp[2]; break;
                    case VAR_TEMP3: scriptEng.operands[i] = scriptEng.temp[3]; break;
                    case VAR_TEMP4: scriptEng.operands[i] = scriptEng.temp[4]; break;
                    case VAR_TEMP5: scriptEng.operands[i] = scriptEng.temp[5]; break;
                    case VAR_TEMP6: scriptEng.operands[i] = scriptEng.temp[6]; break;
                    case VAR_TEMP7: scriptEng.operands[i] = scriptEng.temp[7]; break;
                    case VAR_CHECKRESULT: scriptEng.operands[i] = scriptEng.checkResult; break;
                    case VAR_ARRAYPOS0: scriptEng.operands[i] = scriptEng.arrayPosition[0]; break;
                    case VAR_ARRAYPOS1: scriptEng.operands[i] = scriptEng.arrayPosition[1]; break;
                    case VAR_ARRAYPOS2: scriptEng.operands[i] = scriptEng.arrayPosition[2]; break;
                    case VAR_ARRAYPOS3: scriptEng.operands[i] = scriptEng.arrayPosition[3]; break;
                    case VAR_ARRAYPOS4: scriptEng.operands[i] = scriptEng.arrayPosition[4]; break;
                    case VAR_ARRAYPOS5: scriptEng.operands[i] = scriptEng.arrayPosition[5]; break;
                    case VAR_ARRAYPOS6: scriptEng.operands[i] = scriptEng.arrayPosition[6]; break;
                    case VAR_ARRAYPOS7: scriptEng.operands[i] = scriptEng.arrayPosition[7]; break;
                    case VAR_GLOBAL: scriptEng.operands[i] = globalVariables[arrayVal]; break;
                    case VAR_LOCAL: scriptEng.operands[i] = scriptCode[arrayVal]; break;
                    case VAR_OBJECTENTITYPOS: scriptEng.operands[i] = arrayVal; break;
                    case VAR_OBJECTGROUPID: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].groupID;
                        break;
                    }
                    case VAR_OBJECTTYPE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].type;
                        break;
                    }
                    case VAR_OBJECTPROPERTYVALUE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].propertyValue;
                        break;
                    }
                    case VAR_OBJECTXPOS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].xpos;
                        break;
                    }
                    case VAR_OBJECTYPOS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].ypos;
                        break;
                    }
                    case VAR_OBJECTIXPOS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].xpos >> 16;
                        break;
                    }
                    case VAR_OBJECTIYPOS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].ypos >> 16;
                        break;
                    }
                    case VAR_OBJECTXVEL: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].xvel;
                        break;
                    }
                    case VAR_OBJECTYVEL: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].yvel;
                        break;
                    }
                    case VAR_OBJECTSPEED: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].speed;
                        break;
                    }
                    case VAR_OBJECTSTATE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].state;
                        break;
                    }
                    case VAR_OBJECTROTATION: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].rotation;
                        break;
                    }
                    case VAR_OBJECTSCALE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].scale;
                        break;
                    }
                    case VAR_OBJECTPRIORITY: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].priority;
                        break;
                    }
                    case VAR_OBJECTDRAWORDER: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].drawOrder;
                        break;
                    }
                    case VAR_OBJECTDIRECTION: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].direction;
                        break;
                    }
                    case VAR_OBJECTINKEFFECT: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].inkEffect;
                        break;
                    }
                    case VAR_OBJECTALPHA: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].alpha;
                        break;
                    }
                    case VAR_OBJECTFRAME: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].frame;
                        break;
                    }
                    case VAR_OBJECTANIMATION: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].animation;
                        break;
                    }
                    case VAR_OBJECTPREVANIMATION: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].prevAnimation;
                        break;
                    }
                    case VAR_OBJECTANIMATIONSPEED: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].animationSpeed;
                        break;
                    }
                    case VAR_OBJECTANIMATIONTIMER: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].animationTimer;
                        break;
                    }
                    case VAR_OBJECTANGLE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].angle;
                        break;
                    }
                    case VAR_OBJECTLOOKPOSX: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].lookPosX;
                        break;
                    }
                    case VAR_OBJECTLOOKPOSY: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].lookPosY;
                        break;
                    }
                    case VAR_OBJECTCOLLISIONMODE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].collisionMode;
                        break;
                    }
                    case VAR_OBJECTCOLLISIONPLANE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].collisionPlane;
                        break;
                    }
                    case VAR_OBJECTCONTROLMODE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].controlMode;
                        break;
                    }
                    case VAR_OBJECTCONTROLLOCK: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].controlLock;
                        break;
                    }
                    case VAR_OBJECTPUSHING: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].pushing;
                        break;
                    }
                    case VAR_OBJECTVISIBLE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].visible;
                        break;
                    }
                    case VAR_OBJECTTILECOLLISIONS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].tileCollisions;
                        break;
                    }
                    case VAR_OBJECTINTERACTION: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].objectInteractions;
                        break;
                    }
                    case VAR_OBJECTGRAVITY: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].gravity;
                        break;
                    }
                    case VAR_OBJECTUP: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].up;
                        break;
                    }
                    case VAR_OBJECTDOWN: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].down;
                        break;
                    }
                    case VAR_OBJECTLEFT: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].left;
                        break;
                    }
                    case VAR_OBJECTRIGHT: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].right;
                        break;
                    }
                    case VAR_OBJECTJUMPPRESS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].jumpPress;
                        break;
                    }
                    case VAR_OBJECTJUMPHOLD: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].jumpHold;
                        break;
                    }
                    case VAR_OBJECTSCROLLTRACKING: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].scrollTracking;
                        break;
                    }
                    case VAR_OBJECTFLOORSENSORL: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].floorSensors[0];
                        break;
                    }
                    case VAR_OBJECTFLOORSENSORC: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].floorSensors[1];
                        break;
                    }
                    case VAR_OBJECTFLOORSENSORR: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].floorSensors[2];
                        break;
                    }
#if !RETRO_REV00
                    case VAR_OBJECTFLOORSENSORLC: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].floorSensors[3];
                        break;
                    }
                    case VAR_OBJECTFLOORSENSORRC: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].floorSensors[4];
                        break;
                    }
#endif
                    case VAR_OBJECTCOLLISIONLEFT: {
                        AnimationFile *animFile = objectScriptList[objectEntityList[arrayVal].type].animFile;
                        Entity *ent             = &objectEntityList[arrayVal];
                        if (animFile) {
                            int h = animFrames[animationList[animFile->aniListOffset + ent->animation].frameListOffset + ent->frame].hitboxID;

                            scriptEng.operands[i] = hitboxList[animFile->hitboxListOffset + h].left[0];
                        }
                        else {
                            scriptEng.operands[i] = 0;
                        }
                        break;
                    }
                    case VAR_OBJECTCOLLISIONTOP: {
                        AnimationFile *animFile = objectScriptList[objectEntityList[arrayVal].type].animFile;
                        Entity *ent             = &objectEntityList[arrayVal];
                        if (animFile) {
                            int h = animFrames[animationList[animFile->aniListOffset + ent->animation].frameListOffset + ent->frame].hitboxID;

                            scriptEng.operands[i] = hitboxList[animFile->hitboxListOffset + h].top[0];
                        }
                        else {
                            scriptEng.operands[i] = 0;
                        }
                        break;
                    }
                    case VAR_OBJECTCOLLISIONRIGHT: {
                        AnimationFile *animFile = objectScriptList[objectEntityList[arrayVal].type].animFile;
                        Entity *ent             = &objectEntityList[arrayVal];
                        if (animFile) {
                            int h = animFrames[animationList[animFile->aniListOffset + ent->animation].frameListOffset + ent->frame].hitboxID;

                            scriptEng.operands[i] = hitboxList[animFile->hitboxListOffset + h].right[0];
                        }
                        else {
                            scriptEng.operands[i] = 0;
                        }
                        break;
                    }
                    case VAR_OBJECTCOLLISIONBOTTOM: {
                        AnimationFile *animFile = objectScriptList[objectEntityList[arrayVal].type].animFile;
                        Entity *ent             = &objectEntityList[arrayVal];
                        if (animFile) {
                            int h = animFrames[animationList[animFile->aniListOffset + ent->animation].frameListOffset + ent->frame].hitboxID;

                            scriptEng.operands[i] = hitboxList[animFile->hitboxListOffset + h].bottom[0];
                        }
                        else {
                            scriptEng.operands[i] = 0;
                        }
                        break;
                    }
                    case VAR_OBJECTOUTOFBOUNDS: {
#if !RETRO_REV00
                        int boundX1_2P = -(0x200 << 16);
                        int boundX2_2P = (0x200 << 16);
                        int boundX3_2P = -(0x180 << 16);
                        int boundX4_2P = (0x180 << 16);

                        int boundY1_2P = -(0x180 << 16);
                        int boundY2_2P = (0x180 << 16);
                        int boundY3_2P = -(0x100 << 16);
                        int boundY4_2P = (0x100 << 16);

                        Entity *entPtr = &objectEntityList[arrayVal];
                        int x          = entPtr->xpos >> 16;
                        int y          = entPtr->ypos >> 16;

                        if (entPtr->priority == PRIORITY_BOUNDS_SMALL || entPtr->priority == PRIORITY_ACTIVE_SMALL) {
                            if (stageMode == STAGEMODE_2P) {
                                x = entPtr->xpos;
                                y = entPtr->ypos;

                                int boundL_P1 = objectEntityList[0].xpos + boundX3_2P;
                                int boundR_P1 = objectEntityList[0].xpos + boundX4_2P;
                                int boundT_P1 = objectEntityList[0].ypos + boundY3_2P;
                                int boundB_P1 = objectEntityList[0].ypos + boundY4_2P;

                                int boundL_P2 = objectEntityList[1].xpos + boundX3_2P;
                                int boundR_P2 = objectEntityList[1].xpos + boundX4_2P;
                                int boundT_P2 = objectEntityList[1].ypos + boundY3_2P;
                                int boundB_P2 = objectEntityList[1].ypos + boundY4_2P;

                                bool oobP1 = scriptEng.operands[i] = x <= boundL_P1 || x >= boundR_P1 || y <= boundT_P1 || y >= boundB_P1;
                                bool oobP2 = scriptEng.operands[i] = x <= boundL_P2 || x >= boundR_P2 || y <= boundT_P2 || y >= boundB_P2;

                                scriptEng.operands[i] = oobP1 && oobP2;
                            }
                            else {
                                int boundL = xScrollOffset - OBJECT_BORDER_X3;
                                int boundR = xScrollOffset + OBJECT_BORDER_X4;
                                int boundT = yScrollOffset - OBJECT_BORDER_Y3;
                                int boundB = yScrollOffset + OBJECT_BORDER_Y4;

                                scriptEng.operands[i] = x <= boundL || x >= boundR || y <= boundT || y >= boundB;
                            }
                        }
                        else {
                            if (stageMode == STAGEMODE_2P) {
                                x = entPtr->xpos;
                                y = entPtr->ypos;

                                int boundL_P1 = objectEntityList[0].xpos + boundX1_2P;
                                int boundR_P1 = objectEntityList[0].xpos + boundX2_2P;
                                int boundT_P1 = objectEntityList[0].ypos + boundY1_2P;
                                int boundB_P1 = objectEntityList[0].ypos + boundY2_2P;

                                int boundL_P2 = objectEntityList[1].xpos + boundX1_2P;
                                int boundR_P2 = objectEntityList[1].xpos + boundX2_2P;
                                int boundT_P2 = objectEntityList[1].ypos + boundY1_2P;
                                int boundB_P2 = objectEntityList[1].ypos + boundY2_2P;

                                bool oobP1 = scriptEng.operands[i] = x <= boundL_P1 || x >= boundR_P1 || y <= boundT_P1 || y >= boundB_P1;
                                bool oobP2 = scriptEng.operands[i] = x <= boundL_P2 || x >= boundR_P2 || y <= boundT_P2 || y >= boundB_P2;

                                scriptEng.operands[i] = oobP1 && oobP2;
                            }
                            else {
                                int boundL = xScrollOffset - OBJECT_BORDER_X1;
                                int boundR = xScrollOffset + OBJECT_BORDER_X2;
                                int boundT = yScrollOffset - OBJECT_BORDER_Y1;
                                int boundB = yScrollOffset + OBJECT_BORDER_Y2;

                                scriptEng.operands[i] = x <= boundL || x >= boundR || y <= boundT || y >= boundB;
                            }
                        }
#else
                        int x = objectEntityList[arrayVal].xpos >> 16;
                        int y = objectEntityList[arrayVal].ypos >> 16;

                        int boundL = xScrollOffset - OBJECT_BORDER_X1;
                        int boundR = xScrollOffset + OBJECT_BORDER_X2;
                        int boundT = yScrollOffset - OBJECT_BORDER_Y1;
                        int boundB = yScrollOffset + OBJECT_BORDER_Y2;

                        scriptEng.operands[i] = x <= boundL || x >= boundR || y <= boundT || y >= boundB;
#endif
                        break;
                    }
                    case VAR_OBJECTSPRITESHEET: {
                        scriptEng.operands[i] = objectScriptList[objectEntityList[arrayVal].type].spriteSheetID;
                        break;
                    }
                    case VAR_OBJECTVALUE0: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[0];
                        break;
                    }
                    case VAR_OBJECTVALUE1: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[1];
                        break;
                    }
                    case VAR_OBJECTVALUE2: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[2];
                        break;
                    }
                    case VAR_OBJECTVALUE3: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[3];
                        break;
                    }
                    case VAR_OBJECTVALUE4: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[4];
                        break;
                    }
                    case VAR_OBJECTVALUE5: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[5];
                        break;
                    }
                    case VAR_OBJECTVALUE6: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[6];
                        break;
                    }
                    case VAR_OBJECTVALUE7: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[7];
                        break;
                    }
                    case VAR_OBJECTVALUE8: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[8];
                        break;
                    }
                    case VAR_OBJECTVALUE9: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[9];
                        break;
                    }
                    case VAR_OBJECTVALUE10: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[10];
                        break;
                    }
                    case VAR_OBJECTVALUE11: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[11];
                        break;
                    }
                    case VAR_OBJECTVALUE12: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[12];
                        break;
                    }
                    case VAR_OBJECTVALUE13: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[13];
                        break;
                    }
                    case VAR_OBJECTVALUE14: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[14];
                        break;
                    }
                    case VAR_OBJECTVALUE15: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[15];
                        break;
                    }
                    case VAR_OBJECTVALUE16: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[16];
                        break;
                    }
                    case VAR_OBJECTVALUE17: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[17];
                        break;
                    }
                    case VAR_OBJECTVALUE18: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[18];
                        break;
                    }
                    case VAR_OBJECTVALUE19: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[19];
                        break;
                    }
                    case VAR_OBJECTVALUE20: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[20];
                        break;
                    }
                    case VAR_OBJECTVALUE21: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[21];
                        break;
                    }
                    case VAR_OBJECTVALUE22: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[22];
                        break;
                    }
                    case VAR_OBJECTVALUE23: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[23];
                        break;
                    }
                    case VAR_OBJECTVALUE24: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[24];
                        break;
                    }
                    case VAR_OBJECTVALUE25: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[25];
                        break;
                    }
                    case VAR_OBJECTVALUE26: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[26];
                        break;
                    }
                    case VAR_OBJECTVALUE27: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[27];
                        break;
                    }
                    case VAR_OBJECTVALUE28: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[28];
                        break;
                    }
                    case VAR_OBJECTVALUE29: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[29];
                        break;
                    }
                    case VAR_OBJECTVALUE30: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[30];
                        break;
                    }
                    case VAR_OBJECTVALUE31: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[31];
                        break;
                    }
                    case VAR_OBJECTVALUE32: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[32];
                        break;
                    }
                    case VAR_OBJECTVALUE33: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[33];
                        break;
                    }
                    case VAR_OBJECTVALUE34: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[34];
                        break;
                    }
                    case VAR_OBJECTVALUE35: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[35];
                        break;
                    }
                    case VAR_OBJECTVALUE36: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[36];
                        break;
                    }
                    case VAR_OBJECTVALUE37: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[37];
                        break;
                    }
                    case VAR_OBJECTVALUE38: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[38];
                        break;
                    }
                    case VAR_OBJECTVALUE39: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[39];
                        break;
                    }
                    case VAR_OBJECTVALUE40: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[40];
                        break;
                    }
                    case VAR_OBJECTVALUE41: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[41];
                        break;
                    }
                    case VAR_OBJECTVALUE42: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[42];
                        break;
                    }
                    case VAR_OBJECTVALUE43: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[43];
                        break;
                    }
                    case VAR_OBJECTVALUE44: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[44];
                        break;
                    }
                    case VAR_OBJECTVALUE45: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[45];
                        break;
                    }
                    case VAR_OBJECTVALUE46: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[46];
                        break;
                    }
                    case VAR_OBJECTVALUE47: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[47];
                        break;
                    }
                    case VAR_STAGESTATE: scriptEng.operands[i] = stageMode; break;
                    case VAR_STAGEACTIVELIST: scriptEng.operands[i] = activeStageList; break;
                    case VAR_STAGELISTPOS: scriptEng.operands[i] = stageListPosition; break;
                    case VAR_STAGETIMEENABLED: scriptEng.operands[i] = timeEnabled; break;
                    case VAR_STAGEMILLISECONDS: scriptEng.operands[i] = stageMilliseconds; break;
                    case VAR_STAGESECONDS: scriptEng.operands[i] = stageSeconds; break;
                    case VAR_STAGEMINUTES: scriptEng.operands[i] = stageMinutes; break;
                    case VAR_STAGEACTNUM: scriptEng.operands[i] = actID; break;
                    case VAR_STAGEPAUSEENABLED: scriptEng.operands[i] = pauseEnabled; break;
                    case VAR_STAGELISTSIZE: scriptEng.operands[i] = stageListCount[activeStageList]; break;
                    case VAR_STAGENEWXBOUNDARY1: scriptEng.operands[i] = newXBoundary1; break;
                    case VAR_STAGENEWXBOUNDARY2: scriptEng.operands[i] = newXBoundary2; break;
                    case VAR_STAGENEWYBOUNDARY1: scriptEng.operands[i] = newYBoundary1; break;
                    case VAR_STAGENEWYBOUNDARY2: scriptEng.operands[i] = newYBoundary2; break;
                    case VAR_STAGECURXBOUNDARY1: scriptEng.operands[i] = curXBoundary1; break;
                    case VAR_STAGECURXBOUNDARY2: scriptEng.operands[i] = curXBoundary2; break;
                    case VAR_STAGECURYBOUNDARY1: scriptEng.operands[i] = curYBoundary1; break;
                    case VAR_STAGECURYBOUNDARY2: scriptEng.operands[i] = curYBoundary2; break;
                    case VAR_STAGEDEFORMATIONDATA0: scriptEng.operands[i] = bgDeformationData0[arrayVal]; break;
                    case VAR_STAGEDEFORMATIONDATA1: scriptEng.operands[i] = bgDeformationData1[arrayVal]; break;
                    case VAR_STAGEDEFORMATIONDATA2: scriptEng.operands[i] = bgDeformationData2[arrayVal]; break;
                    case VAR_STAGEDEFORMATIONDATA3: scriptEng.operands[i] = bgDeformationData3[arrayVal]; break;
                    case VAR_STAGEWATERLEVEL: scriptEng.operands[i] = waterLevel; break;
                    case VAR_STAGEACTIVELAYER: scriptEng.operands[i] = activeTileLayers[arrayVal]; break;
                    case VAR_STAGEMIDPOINT: scriptEng.operands[i] = tLayerMidPoint; break;
                    case VAR_STAGEPLAYERLISTPOS: scriptEng.operands[i] = playerListPos; break;
                    case VAR_STAGEDEBUGMODE: scriptEng.operands[i] = debugMode; break;
                    case VAR_STAGEENTITYPOS: scriptEng.operands[i] = objectEntityPos; break;
                    case VAR_SCREENCAMERAENABLED: scriptEng.operands[i] = cameraEnabled; break;
                    case VAR_SCREENCAMERATARGET: scriptEng.operands[i] = cameraTarget; break;
                    case VAR_SCREENCAMERASTYLE: scriptEng.operands[i] = cameraStyle; break;
                    case VAR_SCREENCAMERAX: scriptEng.operands[i] = cameraXPos; break;
                    case VAR_SCREENCAMERAY: scriptEng.operands[i] = cameraYPos; break;
                    case VAR_SCREENDRAWLISTSIZE: scriptEng.operands[i] = drawListEntries[arrayVal].listSize; break;
                    case VAR_SCREENXCENTER: scriptEng.operands[i] = SCREEN_CENTERX; break;
                    case VAR_SCREENYCENTER: scriptEng.operands[i] = SCREEN_CENTERY; break;
                    case VAR_SCREENXSIZE: scriptEng.operands[i] = SCREEN_XSIZE; break;
                    case VAR_SCREENYSIZE: scriptEng.operands[i] = SCREEN_YSIZE; break;
                    case VAR_SCREENXOFFSET: scriptEng.operands[i] = xScrollOffset; break;
                    case VAR_SCREENYOFFSET: scriptEng.operands[i] = yScrollOffset; break;
                    case VAR_SCREENSHAKEX: scriptEng.operands[i] = cameraShakeX; break;
                    case VAR_SCREENSHAKEY: scriptEng.operands[i] = cameraShakeY; break;
                    case VAR_SCREENADJUSTCAMERAY: scriptEng.operands[i] = cameraAdjustY; break;
                    case VAR_TOUCHSCREENDOWN: scriptEng.operands[i] = touchDown[arrayVal]; break;
                    case VAR_TOUCHSCREENXPOS: scriptEng.operands[i] = touchX[arrayVal]; break;
                    case VAR_TOUCHSCREENYPOS: scriptEng.operands[i] = touchY[arrayVal]; break;
                    case VAR_MUSICVOLUME: scriptEng.operands[i] = masterVolume; break;
                    case VAR_MUSICCURRENTTRACK: scriptEng.operands[i] = trackID; break;
                    case VAR_MUSICPOSITION: scriptEng.operands[i] = musicPosition; break;
#if RETRO_REV03 && !RETRO_USE_ORIGINAL_CODE
                    case VAR_KEYDOWNUP: scriptEng.operands[i] = keyDown.up && inputCheck; break;
                    case VAR_KEYDOWNDOWN: scriptEng.operands[i] = keyDown.down && inputCheck; break;
                    case VAR_KEYDOWNLEFT: scriptEng.operands[i] = keyDown.left && inputCheck; break;
                    case VAR_KEYDOWNRIGHT: scriptEng.operands[i] = keyDown.right && inputCheck; break;
                    case VAR_KEYDOWNBUTTONA: scriptEng.operands[i] = keyDown.A && inputCheck; break;
                    case VAR_KEYDOWNBUTTONB: scriptEng.operands[i] = keyDown.B && inputCheck; break;
                    case VAR_KEYDOWNBUTTONC: scriptEng.operands[i] = keyDown.C && inputCheck; break;
                    case VAR_KEYDOWNBUTTONX: scriptEng.operands[i] = keyDown.X && inputCheck; break;
                    case VAR_KEYDOWNBUTTONY: scriptEng.operands[i] = keyDown.Y && inputCheck; break;
                    case VAR_KEYDOWNBUTTONZ: scriptEng.operands[i] = keyDown.Z && inputCheck; break;
                    case VAR_KEYDOWNBUTTONL: scriptEng.operands[i] = keyDown.L && inputCheck; break;
                    case VAR_KEYDOWNBUTTONR: scriptEng.operands[i] = keyDown.R && inputCheck; break;
                    case VAR_KEYDOWNSTART: scriptEng.operands[i] = keyDown.start && inputCheck; break;
                    case VAR_KEYDOWNSELECT: scriptEng.operands[i] = keyDown.select && inputCheck; break;
                    case VAR_KEYPRESSUP: scriptEng.operands[i] = keyPress.up && inputCheck; break;
                    case VAR_KEYPRESSDOWN: scriptEng.operands[i] = keyPress.down && inputCheck; break;
                    case VAR_KEYPRESSLEFT: scriptEng.operands[i] = keyPress.left && inputCheck; break;
                    case VAR_KEYPRESSRIGHT: scriptEng.operands[i] = keyPress.right && inputCheck; break;
                    case VAR_KEYPRESSBUTTONA: scriptEng.operands[i] = keyPress.A && inputCheck; break;
                    case VAR_KEYPRESSBUTTONB: scriptEng.operands[i] = keyPress.B && inputCheck; break;
                    case VAR_KEYPRESSBUTTONC: scriptEng.operands[i] = keyPress.C && inputCheck; break;
                    case VAR_KEYPRESSBUTTONX: scriptEng.operands[i] = keyPress.X && inputCheck; break;
                    case VAR_KEYPRESSBUTTONY: scriptEng.operands[i] = keyPress.Y && inputCheck; break;
                    case VAR_KEYPRESSBUTTONZ: scriptEng.operands[i] = keyPress.Z && inputCheck; break;
                    case VAR_KEYPRESSBUTTONL: scriptEng.operands[i] = keyPress.L && inputCheck; break;
                    case VAR_KEYPRESSBUTTONR: scriptEng.operands[i] = keyPress.R && inputCheck; break;
                    case VAR_KEYPRESSSTART: scriptEng.operands[i] = keyPress.start && inputCheck; break;
                    case VAR_KEYPRESSSELECT: scriptEng.operands[i] = keyPress.select && inputCheck; break;
#else
                    case VAR_KEYDOWNUP: scriptEng.operands[i] = keyDown.up; break;
                    case VAR_KEYDOWNDOWN: scriptEng.operands[i] = keyDown.down; break;
                    case VAR_KEYDOWNLEFT: scriptEng.operands[i] = keyDown.left; break;
                    case VAR_KEYDOWNRIGHT: scriptEng.operands[i] = keyDown.right; break;
                    case VAR_KEYDOWNBUTTONA: scriptEng.operands[i] = keyDown.A; break;
                    case VAR_KEYDOWNBUTTONB: scriptEng.operands[i] = keyDown.B; break;
                    case VAR_KEYDOWNBUTTONC: scriptEng.operands[i] = keyDown.C; break;
                    case VAR_KEYDOWNBUTTONX: scriptEng.operands[i] = keyDown.X; break;
                    case VAR_KEYDOWNBUTTONY: scriptEng.operands[i] = keyDown.Y; break;
                    case VAR_KEYDOWNBUTTONZ: scriptEng.operands[i] = keyDown.Z; break;
                    case VAR_KEYDOWNBUTTONL: scriptEng.operands[i] = keyDown.L; break;
                    case VAR_KEYDOWNBUTTONR: scriptEng.operands[i] = keyDown.R; break;
                    case VAR_KEYDOWNSTART: scriptEng.operands[i] = keyDown.start; break;
                    case VAR_KEYDOWNSELECT: scriptEng.operands[i] = keyDown.select; break;
                    case VAR_KEYPRESSUP: scriptEng.operands[i] = keyPress.up; break;
                    case VAR_KEYPRESSDOWN: scriptEng.operands[i] = keyPress.down; break;
                    case VAR_KEYPRESSLEFT: scriptEng.operands[i] = keyPress.left; break;
                    case VAR_KEYPRESSRIGHT: scriptEng.operands[i] = keyPress.right; break;
                    case VAR_KEYPRESSBUTTONA: scriptEng.operands[i] = keyPress.A; break;
                    case VAR_KEYPRESSBUTTONB: scriptEng.operands[i] = keyPress.B; break;
                    case VAR_KEYPRESSBUTTONC: scriptEng.operands[i] = keyPress.C; break;
                    case VAR_KEYPRESSBUTTONX: scriptEng.operands[i] = keyPress.X; break;
                    case VAR_KEYPRESSBUTTONY: scriptEng.operands[i] = keyPress.Y; break;
                    case VAR_KEYPRESSBUTTONZ: scriptEng.operands[i] = keyPress.Z; break;
                    case VAR_KEYPRESSBUTTONL: scriptEng.operands[i] = keyPress.L; break;
                    case VAR_KEYPRESSBUTTONR: scriptEng.operands[i] = keyPress.R; break;
                    case VAR_KEYPRESSSTART: scriptEng.operands[i] = keyPress.start; break;
                    case VAR_KEYPRESSSELECT: scriptEng.operands[i] = keyPress.select; break;
#endif
                    case VAR_MENU1SELECTION: scriptEng.operands[i] = gameMenu[0].selection1; break;
                    case VAR_MENU2SELECTION: scriptEng.operands[i] = gameMenu[1].selection1; break;
                    case VAR_TILELAYERXSIZE: scriptEng.operands[i] = stageLayouts[arrayVal].xsize; break;
                    case VAR_TILELAYERYSIZE: scriptEng.operands[i] = stageLayouts[arrayVal].ysize; break;
                    case VAR_TILELAYERTYPE: scriptEng.operands[i] = stageLayouts[arrayVal].type; break;
                    case VAR_TILELAYERANGLE: scriptEng.operands[i] = stageLayouts[arrayVal].angle; break;
                    case VAR_TILELAYERXPOS: scriptEng.operands[i] = stageLayouts[arrayVal].xpos; break;
                    case VAR_TILELAYERYPOS: scriptEng.operands[i] = stageLayouts[arrayVal].ypos; break;
                    case VAR_TILELAYERZPOS: scriptEng.operands[i] = stageLayouts[arrayVal].zpos; break;
                    case VAR_TILELAYERPARALLAXFACTOR: scriptEng.operands[i] = stageLayouts[arrayVal].parallaxFactor; break;
                    case VAR_TILELAYERSCROLLSPEED: scriptEng.operands[i] = stageLayouts[arrayVal].scrollSpeed; break;
                    case VAR_TILELAYERSCROLLPOS: scriptEng.operands[i] = stageLayouts[arrayVal].scrollPos; break;
                    case VAR_TILELAYERDEFORMATIONOFFSET: scriptEng.operands[i] = stageLayouts[arrayVal].deformationOffset; break;
                    case VAR_TILELAYERDEFORMATIONOFFSETW: scriptEng.operands[i] = stageLayouts[arrayVal].deformationOffsetW; break;
                    case VAR_HPARALLAXPARALLAXFACTOR: scriptEng.operands[i] = hParallax.parallaxFactor[arrayVal]; break;
                    case VAR_HPARALLAXSCROLLSPEED: scriptEng.operands[i] = hParallax.scrollSpeed[arrayVal]; break;
                    case VAR_HPARALLAXSCROLLPOS: scriptEng.operands[i] = hParallax.scrollPos[arrayVal]; break;
                    case VAR_VPARALLAXPARALLAXFACTOR: scriptEng.operands[i] = vParallax.parallaxFactor[arrayVal]; break;
                    case VAR_VPARALLAXSCROLLSPEED: scriptEng.operands[i] = vParallax.scrollSpeed[arrayVal]; break;
                    case VAR_VPARALLAXSCROLLPOS: scriptEng.operands[i] = vParallax.scrollPos[arrayVal]; break;
                    case VAR_SCENE3DVERTEXCOUNT: scriptEng.operands[i] = vertexCount; break;
                    case VAR_SCENE3DFACECOUNT: scriptEng.operands[i] = faceCount; break;
                    case VAR_SCENE3DPROJECTIONX: scriptEng.operands[i] = projectionX; break;
                    case VAR_SCENE3DPROJECTIONY: scriptEng.operands[i] = projectionY; break;
#if !RETRO_REV00
                    case VAR_SCENE3DFOGCOLOR: scriptEng.operands[i] = fogColor; break;
                    case VAR_SCENE3DFOGSTRENGTH: scriptEng.operands[i] = fogStrength; break;
#endif
                    case VAR_VERTEXBUFFERX: scriptEng.operands[i] = vertexBuffer[arrayVal].x; break;
                    case VAR_VERTEXBUFFERY: scriptEng.operands[i] = vertexBuffer[arrayVal].y; break;
                    case VAR_VERTEXBUFFERZ: scriptEng.operands[i] = vertexBuffer[arrayVal].z; break;
                    case VAR_VERTEXBUFFERU: scriptEng.operands[i] = vertexBuffer[arrayVal].u; break;
                    case VAR_VERTEXBUFFERV: scriptEng.operands[i] = vertexBuffer[arrayVal].v; break;
                    case VAR_FACEBUFFERA: scriptEng.operands[i] = faceBuffer[arrayVal].a; break;
                    case VAR_FACEBUFFERB: scriptEng.operands[i] = faceBuffer[arrayVal].b; break;
                    case VAR_FACEBUFFERC: scriptEng.operands[i] = faceBuffer[arrayVal].c; break;
                    case VAR_FACEBUFFERD: scriptEng.operands[i] = faceBuffer[arrayVal].d; break;
                    case VAR_FACEBUFFERFLAG: scriptEng.operands[i] = faceBuffer[arrayVal].flag; break;
                    case VAR_FACEBUFFERCOLOR: scriptEng.operands[i] = faceBuffer[arrayVal].color; break;
                    case VAR_SAVERAM: scriptEng.operands[i] = saveRAM[arrayVal]; break;
                    case VAR_ENGINESTATE: scriptEng.operands[i] = Engine.gameMode; break;
#if RETRO_REV00
                    case VAR_ENGINEMESSAGE: scriptEng.operands[i] = Engine.message; break;
#endif
                    case VAR_ENGINELANGUAGE: scriptEng.operands[i] = Engine.language; break;
                    case VAR_ENGINEONLINEACTIVE: scriptEng.operands[i] = Engine.onlineActive; break;
                    case VAR_ENGINESFXVOLUME: scriptEng.operands[i] = sfxVolume; break;
                    case VAR_ENGINEBGMVOLUME: scriptEng.operands[i] = bgmVolume; break;
#if RETRO_REV00
                    case VAR_ENGINEPLATFORMID: scriptEng.operands[i] = RETRO_GAMEPLATFORMID; break;
#endif
                    case VAR_ENGINETRIALMODE: scriptEng.operands[i] = Engine.trialMode; break;
#if !RETRO_REV00
                    case VAR_ENGINEDEVICETYPE: scriptEng.operands[i] = RETRO_DEVICETYPE; break;
#endif

#if RETRO_REV03
                    // Origins Extras
                    // Due to using regular v4, these don't support array values like origins expects, so its always screen[0]
                    case VAR_SCREENCURRENTID: scriptEng.operands[i] = 0; break;
                    case VAR_CAMERAENABLED:
                        if (arrayVal == 0)
                            scriptEng.operands[i] = cameraEnabled;
                        else
                            scriptEng.operands[i] = 0;
                        break;
                    case VAR_CAMERATARGET:
                        if (arrayVal == 0)
                            scriptEng.operands[i] = cameraTarget;
                        else
                            scriptEng.operands[i] = 0;
                        break;
                    case VAR_CAMERASTYLE:
                        if (arrayVal == 0)
                            scriptEng.operands[i] = cameraStyle;
                        else
                            scriptEng.operands[i] = 0;
                        break;
                    case VAR_CAMERAXPOS:
                        if (arrayVal == 0)
                            scriptEng.operands[i] = cameraXPos;
                        else
                            scriptEng.operands[i] = 0;
                        break;
                    case VAR_CAMERAYPOS:
                        if (arrayVal == 0)
                            scriptEng.operands[i] = cameraYPos;
                        else
                            scriptEng.operands[i] = 0;
                        break;
                    case VAR_CAMERAADJUSTY:
                        if (arrayVal == 0)
                            scriptEng.operands[i] = cameraAdjustY;
                        else
                            scriptEng.operands[i] = 0;
                        break;
#endif

#if RETRO_USE_HAPTICS
                    case VAR_HAPTICSENABLED: scriptEng.operands[i] = Engine.hapticsEnabled; break;
#endif
                }
            }
            else if (opcodeType == SCRIPTVAR_INTCONST) { // int constant
                scriptEng.operands[i] = scriptCode[scriptCodePtr++];
            }
            else if (opcodeType == SCRIPTVAR_STRCONST) { // string constant
                int strLen         = scriptCode[scriptCodePtr++];
                scriptText[strLen] = 0;
                for (int c = 0; c < strLen; ++c) {
                    switch (c % 4) {
                        case 0: 
                            scriptText[c] = scriptCode[scriptCodePtr] >> 24;
                            break;

                        case 1: 
                            scriptText[c] = (0xFFFFFF & scriptCode[scriptCodePtr]) >> 16;
                            break;

                        case 2: 
                            scriptText[c] = (0xFFFF & scriptCode[scriptCodePtr]) >> 8;
                            break;

                        case 3: 
                            scriptText[c] = scriptCode[scriptCodePtr++];
                            break;

                        default: break;
                    }
                }
                scriptCodePtr++;
            }
        }

        ObjectScript *scriptInfo = &objectScriptList[objectEntityList[objectEntityPos].type];
        Entity *entity           = &objectEntityList[objectEntityPos];
        SpriteFrame *spriteFrame = nullptr;

        // Functions
        switch (opcode) {
            default: break;
            case FUNC_END: running = false; break;
            case FUNC_EQUAL: scriptEng.operands[0] = scriptEng.operands[1]; break;
            case FUNC_ADD: scriptEng.operands[0] += scriptEng.operands[1]; break;
            case FUNC_SUB: scriptEng.operands[0] -= scriptEng.operands[1]; break;
            case FUNC_INC: ++scriptEng.operands[0]; break;
            case FUNC_DEC: --scriptEng.operands[0]; break;
            case FUNC_MUL: scriptEng.operands[0] *= scriptEng.operands[1]; break;
            case FUNC_DIV: scriptEng.operands[0] /= scriptEng.operands[1]; break;
            case FUNC_SHR: scriptEng.operands[0] >>= scriptEng.operands[1]; break;
            case FUNC_SHL: scriptEng.operands[0] <<= scriptEng.operands[1]; break;
            case FUNC_AND: scriptEng.operands[0] &= scriptEng.operands[1]; break;
            case FUNC_OR: scriptEng.operands[0] |= scriptEng.operands[1]; break;
            case FUNC_XOR: scriptEng.operands[0] ^= scriptEng.operands[1]; break;
            case FUNC_MOD: scriptEng.operands[0] %= scriptEng.operands[1]; break;
            case FUNC_FLIPSIGN: scriptEng.operands[0] = -scriptEng.operands[0]; break;
            case FUNC_CHECKEQUAL:
                scriptEng.checkResult = scriptEng.operands[0] == scriptEng.operands[1];
                opcodeSize            = 0;
                break;
            case FUNC_CHECKGREATER:
                scriptEng.checkResult = scriptEng.operands[0] > scriptEng.operands[1];
                opcodeSize            = 0;
                break;
            case FUNC_CHECKLOWER:
                scriptEng.checkResult = scriptEng.operands[0] < scriptEng.operands[1];
                opcodeSize            = 0;
                break;
            case FUNC_CHECKNOTEQUAL:
                scriptEng.checkResult = scriptEng.operands[0] != scriptEng.operands[1];
                opcodeSize            = 0;
                break;
            case FUNC_IFEQUAL:
                if (scriptEng.operands[1] != scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFGREATER:
                if (scriptEng.operands[1] <= scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFGREATEROREQUAL:
                if (scriptEng.operands[1] < scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFLOWER:
                if (scriptEng.operands[1] >= scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFLOWEROREQUAL:
                if (scriptEng.operands[1] > scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFNOTEQUAL:
                if (scriptEng.operands[1] == scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_ELSE:
                opcodeSize    = 0;
                scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + jumpTableStack[jumpTableStackPos--] + 1];
                break;
            case FUNC_ENDIF:
                opcodeSize = 0;
                --jumpTableStackPos;
                break;
            case FUNC_WEQUAL:
                if (scriptEng.operands[1] != scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WGREATER:
                if (scriptEng.operands[1] <= scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WGREATEROREQUAL:
                if (scriptEng.operands[1] < scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WLOWER:
                if (scriptEng.operands[1] >= scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WLOWEROREQUAL:
                if (scriptEng.operands[1] > scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WNOTEQUAL:
                if (scriptEng.operands[1] == scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_LOOP:
                opcodeSize    = 0;
                scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + jumpTableStack[jumpTableStackPos--]];
                break;
            case FUNC_FOREACHACTIVE: {
                int groupID = scriptEng.operands[1];
                if (groupID < TYPEGROUP_COUNT) {
                    int loop                      = foreachStack[++foreachStackPos] + 1;
                    foreachStack[foreachStackPos] = loop;
                    if (loop >= objectTypeGroupList[groupID].listSize) {
                        opcodeSize                      = 0;
                        foreachStack[foreachStackPos--] = -1;
                        scriptCodePtr                   = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                        break;
                    }
                    else {
                        scriptEng.operands[2]               = objectTypeGroupList[groupID].entityRefs[loop];
                        jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                    }
                }
                else {
                    opcodeSize    = 0;
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                }
                break;
            }
            case FUNC_FOREACHALL: {
                int objType = scriptEng.operands[1];
                if (objType < OBJECT_COUNT) {
                    int loop                      = foreachStack[++foreachStackPos] + 1;
                    foreachStack[foreachStackPos] = loop;

                    if (scriptEvent == EVENT_SETUP) {
                        while (true) {
                            if (loop >= TEMPENTITY_START) {
                                opcodeSize                      = 0;
                                foreachStack[foreachStackPos--] = -1;
                                scriptCodePtr                   = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                                break;
                            }
                            else if (objType == objectEntityList[loop].type) {
                                scriptEng.operands[2]               = loop;
                                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                                break;
                            }
                            else {
                                foreachStack[foreachStackPos] = ++loop;
                            }
                        }
                    }
                    else {
                        while (true) {
                            if (loop >= ENTITY_COUNT) {
                                opcodeSize                      = 0;
                                foreachStack[foreachStackPos--] = -1;
                                scriptCodePtr                   = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                                break;
                            }
                            else if (objType == objectEntityList[loop].type) {
                                scriptEng.operands[2]               = loop;
                                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                                break;
                            }
                            else {
                                foreachStack[foreachStackPos] = ++loop;
                            }
                        }
                    }
                }
                else {
                    opcodeSize    = 0;
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                }
                break;
            }
            case FUNC_NEXT:
                opcodeSize    = 0;
                scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + jumpTableStack[jumpTableStackPos--]];
                --foreachStackPos;
                break;
            case FUNC_SWITCH:
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                if (scriptEng.operands[1] < jumpTable[jumpTableStart + scriptEng.operands[0]]
                    || scriptEng.operands[1] > jumpTable[jumpTableStart + scriptEng.operands[0] + 1])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 2];
                else
                    scriptCodePtr = scriptCodeStart
                                    + jumpTable[jumpTableStart + scriptEng.operands[0] + 4
                                                    + (scriptEng.operands[1] - jumpTable[jumpTableStart + scriptEng.operands[0]])];
                opcodeSize = 0;
                break;
            case FUNC_BREAK:
                opcodeSize    = 0;
                scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + jumpTableStack[jumpTableStackPos--] + 3];
                break;
            case FUNC_ENDSWITCH:
                opcodeSize = 0;
                --jumpTableStackPos;
                break;
            case FUNC_RAND: scriptEng.operands[0] = rand() % scriptEng.operands[1]; break;
            case FUNC_SIN: {
                scriptEng.operands[0] = Sin512(scriptEng.operands[1]);
                break;
            }
            case FUNC_COS: {
                scriptEng.operands[0] = Cos512(scriptEng.operands[1]);
                break;
            }
            case FUNC_SIN256: {
                scriptEng.operands[0] = Sin256(scriptEng.operands[1]);
                break;
            }
            case FUNC_COS256: {
                scriptEng.operands[0] = Cos256(scriptEng.operands[1]);
                break;
            }
            case FUNC_ATAN2: {
                scriptEng.operands[0] = ArcTanLookup(scriptEng.operands[1], scriptEng.operands[2]);
                break;
            }
            case FUNC_INTERPOLATE:
                scriptEng.operands[0] =
                    (scriptEng.operands[2] * (0x100 - scriptEng.operands[3]) + scriptEng.operands[3] * scriptEng.operands[1]) >> 8;
                break;
            case FUNC_INTERPOLATEXY:
                scriptEng.operands[0] =
                    (scriptEng.operands[3] * (0x100 - scriptEng.operands[6]) >> 8) + ((scriptEng.operands[6] * scriptEng.operands[2]) >> 8);
                scriptEng.operands[1] =
                    (scriptEng.operands[5] * (0x100 - scriptEng.operands[6]) >> 8) + (scriptEng.operands[6] * scriptEng.operands[4] >> 8);
                break;
            case FUNC_LOADSPRITESHEET:
                opcodeSize                = 0;
                scriptInfo->spriteSheetID = AddGraphicsFile(scriptText);
                break;
            case FUNC_REMOVESPRITESHEET:
                opcodeSize = 0;
                RemoveGraphicsFile(scriptText, -1);
                break;
            case FUNC_DRAWSPRITE:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                DrawSprite((entity->xpos >> 16) - xScrollOffset + spriteFrame->pivotX, (entity->ypos >> 16) - yScrollOffset + spriteFrame->pivotY,
                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                break;
            case FUNC_DRAWSPRITEXY:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                DrawSprite((scriptEng.operands[1] >> 16) - xScrollOffset + spriteFrame->pivotX,
                           (scriptEng.operands[2] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width, spriteFrame->height,
                           spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                break;
            case FUNC_DRAWSPRITESCREENXY:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY, spriteFrame->width,
                           spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                break;
            case FUNC_DRAWTINTRECT:
                opcodeSize = 0;
                DrawTintRectangle(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                break;
            case FUNC_DRAWNUMBERS: {
                opcodeSize = 0;
                int i      = 10;
                if (scriptEng.operands[6]) {
                    while (scriptEng.operands[4] > 0) {
                        int frameID = scriptEng.operands[3] % i / (i / 10) + scriptEng.operands[0];
                        spriteFrame = &scriptFrames[scriptInfo->frameListOffset + frameID];
                        DrawSprite(spriteFrame->pivotX + scriptEng.operands[1], spriteFrame->pivotY + scriptEng.operands[2], spriteFrame->width,
                                   spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                        scriptEng.operands[1] -= scriptEng.operands[5];
                        i *= 10;
                        --scriptEng.operands[4];
                    }
                }
                else {
                    int extra = 10;
                    if (scriptEng.operands[3])
                        extra = 10 * scriptEng.operands[3];
                    while (scriptEng.operands[4] > 0) {
                        if (extra >= i) {
                            int frameID = scriptEng.operands[3] % i / (i / 10) + scriptEng.operands[0];
                            spriteFrame = &scriptFrames[scriptInfo->frameListOffset + frameID];
                            DrawSprite(spriteFrame->pivotX + scriptEng.operands[1], spriteFrame->pivotY + scriptEng.operands[2], spriteFrame->width,
                                       spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                        }
                        scriptEng.operands[1] -= scriptEng.operands[5];
                        i *= 10;
                        --scriptEng.operands[4];
                    }
                }
                break;
            }
            case FUNC_DRAWACTNAME: {
                opcodeSize = 0;
                int charID = 0;
                switch (scriptEng.operands[3]) { // Draw Mode
                    case 0:                      // Draw Word 1 (but aligned from the right instead of left)
                        charID = 0;

                        for (charID = 0;; ++charID) {
                            int nextChar = titleCardText[charID + 1];
                            if (nextChar == '-' || !nextChar)
                                break;
                        }

                        while (charID >= 0) {
                            int character = titleCardText[charID];
                            if (character == ' ')
                                character = -1; // special space char
                            if (character == '-')
                                character = 0;
                            if (character >= '0' && character <= '9')
                                character -= 22;
                            if (character > '9' && character < 'f')
                                character -= 'A';

                            if (character <= -1) {
                                scriptEng.operands[1] -= scriptEng.operands[5] + scriptEng.operands[6]; // spaceWidth + spacing
                            }
                            else {
                                character += scriptEng.operands[0];
                                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + character];

                                scriptEng.operands[1] -= spriteFrame->width + scriptEng.operands[6];

                                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                            }
                            charID--;
                        }
                        break;

                    case 1: // Draw Word 1
                        charID = 0;

                        // Draw the first letter as a capital letter, the rest are lowercase (if scriptEng.operands[4] is true, otherwise they're all
                        // uppercase)
                        if (scriptEng.operands[4] == 1 && titleCardText[charID] != 0) {
                            int character = titleCardText[charID];
                            if (character == ' ')
                                character = -1;
                            if (character == '-')
                                character = 0;
                            if (character >= '0' && character <= '9')
                                character -= 22;
                            if (character > '9' && character < 'f')
                                character -= 'A';

                            if (character <= -1) {
                                scriptEng.operands[1] += scriptEng.operands[5] + scriptEng.operands[6]; // spaceWidth + spacing
                            }
                            else {
                                character += scriptEng.operands[0];
                                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + character];
                                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                scriptEng.operands[1] += spriteFrame->width + scriptEng.operands[6];
                            }

                            scriptEng.operands[0] += 26;
                            charID++;
                        }

                        while (titleCardText[charID] != 0 && titleCardText[charID] != '-') {
                            int character = titleCardText[charID];
                            if (character == ' ')
                                character = -1;
                            if (character == '-')
                                character = 0;
                            if (character > '/' && character < ':')
                                character -= 22;
                            if (character > '9' && character < 'f')
                                character -= 'A';

                            if (character <= -1) {
                                scriptEng.operands[1] += scriptEng.operands[5] + scriptEng.operands[6]; // spaceWidth + spacing
                            }
                            else {
                                character += scriptEng.operands[0];
                                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + character];
                                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                scriptEng.operands[1] += spriteFrame->width + scriptEng.operands[6];
                            }
                            charID++;
                        }
                        break;

                    case 2: // Draw Word 2
                        charID = titleCardWord2;

                        // Draw the first letter as a capital letter, the rest are lowercase (if scriptEng.operands[4] is true, otherwise they're all
                        // uppercase)
                        if (scriptEng.operands[4] == 1 && titleCardText[charID] != 0) {
                            int character = titleCardText[charID];
                            if (character == ' ')
                                character = 0;
                            if (character == '-')
                                character = 0;
                            if (character >= '0' && character <= '9')
                                character -= 22;
                            if (character > '9' && character < 'f')
                                character -= 'A';

                            if (character <= -1) {
                                scriptEng.operands[1] += scriptEng.operands[5] + scriptEng.operands[6]; // spaceWidth + spacing
                            }
                            else {
                                character += scriptEng.operands[0];
                                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + character];
                                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                scriptEng.operands[1] += spriteFrame->width + scriptEng.operands[6];
                            }
                            scriptEng.operands[0] += 26;
                            charID++;
                        }

                        while (titleCardText[charID] != 0) {
                            int character = titleCardText[charID];
                            if (character == ' ')
                                character = 0;
                            if (character == '-')
                                character = 0;
                            if (character >= '0' && character <= '9')
                                character -= 22;
                            if (character > '9' && character < 'f')
                                character -= 'A';

                            if (character <= -1) {
                                scriptEng.operands[1] += scriptEng.operands[5] + scriptEng.operands[6]; // spaceWidth + spacing
                            }
                            else {
                                character += scriptEng.operands[0];
                                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + character];
                                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                scriptEng.operands[1] += spriteFrame->width + scriptEng.operands[6];
                            }
                            charID++;
                        }
                        break;
                }
                break;
            }
            case FUNC_DRAWMENU:
                opcodeSize        = 0;
                textMenuSurfaceNo = scriptInfo->spriteSheetID;
                DrawTextMenu(&gameMenu[scriptEng.operands[0]], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            case FUNC_SPRITEFRAME:
                opcodeSize = 0;
                if (scriptEvent == EVENT_SETUP && scriptFrameCount < SPRITEFRAME_COUNT) {
                    scriptFrames[scriptFrameCount].pivotX = scriptEng.operands[0];
                    scriptFrames[scriptFrameCount].pivotY = scriptEng.operands[1];
                    scriptFrames[scriptFrameCount].width  = scriptEng.operands[2];
                    scriptFrames[scriptFrameCount].height = scriptEng.operands[3];
                    scriptFrames[scriptFrameCount].sprX   = scriptEng.operands[4];
                    scriptFrames[scriptFrameCount].sprY   = scriptEng.operands[5];
                    ++scriptFrameCount;
                }
                break;
            case FUNC_EDITFRAME: {
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];

                spriteFrame->pivotX = scriptEng.operands[1];
                spriteFrame->pivotY = scriptEng.operands[2];
                spriteFrame->width  = scriptEng.operands[3];
                spriteFrame->height = scriptEng.operands[4];
                spriteFrame->sprX   = scriptEng.operands[5];
                spriteFrame->sprY   = scriptEng.operands[6];
            } break;
            case FUNC_LOADPALETTE:
                opcodeSize = 0;
                LoadPalette(scriptText, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4]);
                break;
            case FUNC_ROTATEPALETTE:
                opcodeSize = 0;
                RotatePalette(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                break;
            case FUNC_SETSCREENFADE:
                opcodeSize = 0;
                SetFade(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                break;
            case FUNC_SETACTIVEPALETTE:
                opcodeSize = 0;
                SetActivePalette(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            case FUNC_SETPALETTEFADE:
#if RETRO_REV00
                SetLimitedFade(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                               scriptEng.operands[5], scriptEng.operands[6]);
#else
                SetPaletteFade(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                               scriptEng.operands[5]);
#endif
                break;
            case FUNC_SETPALETTEENTRY: SetPaletteEntryPacked(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2]); break;
            case FUNC_GETPALETTEENTRY: scriptEng.operands[2] = GetPaletteEntryPacked(scriptEng.operands[0], scriptEng.operands[1]); break;
            case FUNC_COPYPALETTE:
                opcodeSize = 0;
                CopyPalette(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4]);
                break;
            case FUNC_CLEARSCREEN:
                opcodeSize = 0;
                ClearScreen(scriptEng.operands[0]);
                break;
            case FUNC_DRAWSPRITEFX:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                switch (scriptEng.operands[1]) {
                    default: break;
                    case FX_SCALE:
                        DrawSpriteScaled(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                         (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY, entity->scale,
                                         entity->scale, spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                         scriptInfo->spriteSheetID);
                        break;
                    case FX_ROTATE:
                        DrawSpriteRotated(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                          (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY,
                                          spriteFrame->sprX, spriteFrame->sprY, spriteFrame->width, spriteFrame->height, entity->rotation,
                                          scriptInfo->spriteSheetID);
                        break;
                    case FX_ROTOZOOM:
                        DrawSpriteRotozoom(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                           (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY,
                                           spriteFrame->sprX, spriteFrame->sprY, spriteFrame->width, spriteFrame->height, entity->rotation,
                                           entity->scale, scriptInfo->spriteSheetID);
                        break;
                    case FX_INK:
                        switch (entity->inkEffect) {
                            case INK_NONE:
                                DrawSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                           (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                           spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                break;
                            case INK_BLEND:
                                DrawBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                break;
                            case INK_ALPHA:
                                DrawAlphaBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                       (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                       spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, entity->alpha,
                                                       scriptInfo->spriteSheetID);
                                break;
                            case INK_ADD:
                                DrawAdditiveBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                          (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                          spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, entity->alpha,
                                                          scriptInfo->spriteSheetID);
                                break;
                            case INK_SUB:
                                DrawSubtractiveBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                             (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                             spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, entity->alpha,
                                                             scriptInfo->spriteSheetID);
                                break;
                        }
                        break;
                    case FX_TINT:
                        if (entity->inkEffect == INK_ALPHA) {
                            DrawScaledTintMask(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                               (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY,
                                               entity->scale, entity->scale, spriteFrame->width, spriteFrame->height, spriteFrame->sprX,
                                               spriteFrame->sprY, scriptInfo->spriteSheetID);
                        }
                        else {
                            DrawSpriteScaled(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                             (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY, entity->scale,
                                             entity->scale, spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                             scriptInfo->spriteSheetID);
                        }
                        break;
                    case FX_FLIP:
                        switch (entity->direction) {
                            default:
                            case FLIP_NONE:
                                DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_NONE, scriptInfo->spriteSheetID);
                                break;
                            case FLIP_X:
                                DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset - spriteFrame->width - spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_X, scriptInfo->spriteSheetID);
                                break;
                            case FLIP_Y:
                                DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset - spriteFrame->height - spriteFrame->pivotY,
                                                  spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_Y,
                                                  scriptInfo->spriteSheetID);
                                break;
                            case FLIP_XY:
                                DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset - spriteFrame->width - spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset - spriteFrame->height - spriteFrame->pivotY,
                                                  spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_XY,
                                                  scriptInfo->spriteSheetID);
                                break;
                        }
                        break;
                }
                break;
            case FUNC_DRAWSPRITESCREENFX:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                switch (scriptEng.operands[1]) {
                    default: break;
                    case FX_SCALE:
                        DrawSpriteScaled(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX, -spriteFrame->pivotY,
                                         entity->scale, entity->scale, spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                         scriptInfo->spriteSheetID);
                        break;
                    case FX_ROTATE:
                        DrawSpriteRotated(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX, -spriteFrame->pivotY,
                                          spriteFrame->sprX, spriteFrame->sprY, spriteFrame->width, spriteFrame->height, entity->rotation,
                                          scriptInfo->spriteSheetID);
                        break;
                    case FX_ROTOZOOM:
                        DrawSpriteRotozoom(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX,
                                           -spriteFrame->pivotY, spriteFrame->sprX, spriteFrame->sprY, spriteFrame->width, spriteFrame->height,
                                           entity->rotation, entity->scale, scriptInfo->spriteSheetID);
                        break;
                    case FX_INK:
                        switch (entity->inkEffect) {
                            case INK_NONE:
                                DrawSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                break;
                            case INK_BLEND:
                                DrawBlendedSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                  spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                                  scriptInfo->spriteSheetID);
                                break;
                            case INK_ALPHA:
                                DrawAlphaBlendedSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                       spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, entity->alpha,
                                                       scriptInfo->spriteSheetID);
                                break;
                            case INK_ADD:
                                DrawAdditiveBlendedSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                          spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                                          entity->alpha, scriptInfo->spriteSheetID);
                                break;
                            case INK_SUB:
                                DrawSubtractiveBlendedSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                             spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                                             entity->alpha, scriptInfo->spriteSheetID);
                                break;
                        }
                        break;
                    case FX_TINT:
                        if (entity->inkEffect == INK_ALPHA) {
                            DrawScaledTintMask(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX,
                                               -spriteFrame->pivotY, entity->scale, entity->scale, spriteFrame->width, spriteFrame->height,
                                               spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                        }
                        else {
                            DrawSpriteScaled(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX,
                                             -spriteFrame->pivotY, entity->scale, entity->scale, spriteFrame->width, spriteFrame->height,
                                             spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                        }
                        break;
                    case FX_FLIP:
                        switch (entity->direction) {
                            default:
                            case FLIP_NONE:
                                DrawSpriteFlipped(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                  spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_NONE,
                                                  scriptInfo->spriteSheetID);
                                break;
                            case FLIP_X:
                                DrawSpriteFlipped(scriptEng.operands[2] - spriteFrame->width - spriteFrame->pivotX,
                                                  scriptEng.operands[3] + spriteFrame->pivotY, spriteFrame->width, spriteFrame->height,
                                                  spriteFrame->sprX, spriteFrame->sprY, FLIP_X, scriptInfo->spriteSheetID);
                                break;
                            case FLIP_Y:
                                DrawSpriteFlipped(scriptEng.operands[2] + spriteFrame->pivotX,
                                                  scriptEng.operands[3] - spriteFrame->height - spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_Y, scriptInfo->spriteSheetID);
                                break;
                            case FLIP_XY:
                                DrawSpriteFlipped(scriptEng.operands[2] - spriteFrame->width - spriteFrame->pivotX,
                                                  scriptEng.operands[3] - spriteFrame->height - spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_XY, scriptInfo->spriteSheetID);
                                break;
                        }
                        break;
                }
                break;
            case FUNC_LOADANIMATION:
                opcodeSize           = 0;
                scriptInfo->animFile = AddAnimationFile(scriptText);
                break;
            case FUNC_SETUPMENU: {
                opcodeSize     = 0;
                TextMenu *menu = &gameMenu[scriptEng.operands[0]];
                SetupTextMenu(menu, scriptEng.operands[1]);
                menu->selectionCount = scriptEng.operands[2];
                menu->alignment      = scriptEng.operands[3];
                break;
            }
            case FUNC_ADDMENUENTRY: {
                opcodeSize                           = 0;
                TextMenu *menu                       = &gameMenu[scriptEng.operands[0]];
                menu->entryHighlight[menu->rowCount] = scriptEng.operands[2];
                AddTextMenuEntry(menu, scriptText);
                break;
            }
            case FUNC_EDITMENUENTRY: {
                opcodeSize     = 0;
                TextMenu *menu = &gameMenu[scriptEng.operands[0]];
                EditTextMenuEntry(menu, scriptText, scriptEng.operands[2]);
                menu->entryHighlight[scriptEng.operands[2]] = scriptEng.operands[3];
                break;
            }
            case FUNC_LOADSTAGE:
                opcodeSize = 0;
                stageMode  = STAGEMODE_LOAD;
                break;
            case FUNC_DRAWRECT:
                opcodeSize = 0;
                DrawRectangle(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                              scriptEng.operands[5], scriptEng.operands[6], scriptEng.operands[7]);
                break;
            case FUNC_RESETOBJECTENTITY: {
                opcodeSize     = 0;
                Entity *newEnt = &objectEntityList[scriptEng.operands[0]];
                memset(newEnt, 0, sizeof(Entity));
                newEnt->type               = scriptEng.operands[1];
                newEnt->propertyValue      = scriptEng.operands[2];
                newEnt->xpos               = scriptEng.operands[3];
                newEnt->ypos               = scriptEng.operands[4];
                newEnt->direction          = FLIP_NONE;
                newEnt->priority           = PRIORITY_BOUNDS;
                newEnt->drawOrder          = 3;
                newEnt->scale              = 512;
                newEnt->inkEffect          = INK_NONE;
                newEnt->objectInteractions = true;
                newEnt->visible            = true;
                newEnt->tileCollisions     = true;
                break;
            }
            case FUNC_BOXCOLLISIONTEST:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    default: break;
                    case C_TOUCH:
                        TouchCollision(&objectEntityList[scriptEng.operands[1]], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                                       scriptEng.operands[5], &objectEntityList[scriptEng.operands[6]], scriptEng.operands[7], scriptEng.operands[8],
                                       scriptEng.operands[9], scriptEng.operands[10]);
                        break;
                    case C_SOLID:
                        BoxCollision(&objectEntityList[scriptEng.operands[1]], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                                     scriptEng.operands[5], &objectEntityList[scriptEng.operands[6]], scriptEng.operands[7], scriptEng.operands[8],
                                     scriptEng.operands[9], scriptEng.operands[10]);
                        break;
                    case C_SOLID2:
                        BoxCollision2(&objectEntityList[scriptEng.operands[1]], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                                      scriptEng.operands[5], &objectEntityList[scriptEng.operands[6]], scriptEng.operands[7], scriptEng.operands[8],
                                      scriptEng.operands[9], scriptEng.operands[10]);
                        break;
                    case C_PLATFORM:
                        PlatformCollision(&objectEntityList[scriptEng.operands[1]], scriptEng.operands[2], scriptEng.operands[3],
                                          scriptEng.operands[4], scriptEng.operands[5], &objectEntityList[scriptEng.operands[6]],
                                          scriptEng.operands[7], scriptEng.operands[8], scriptEng.operands[9], scriptEng.operands[10]);
                        break;
                }
                break;
            case FUNC_CREATETEMPOBJECT: {
                opcodeSize = 0;
                if (objectEntityList[scriptEng.arrayPosition[8]].type > OBJ_TYPE_BLANKOBJECT && ++scriptEng.arrayPosition[8] == ENTITY_COUNT)
                    scriptEng.arrayPosition[8] = TEMPENTITY_START;
                Entity *temp = &objectEntityList[scriptEng.arrayPosition[8]];
                memset(temp, 0, sizeof(Entity));
                temp->type               = scriptEng.operands[0];
                temp->propertyValue      = scriptEng.operands[1];
                temp->xpos               = scriptEng.operands[2];
                temp->ypos               = scriptEng.operands[3];
                temp->direction          = FLIP_NONE;
                temp->priority           = PRIORITY_ACTIVE;
                temp->drawOrder          = 3;
                temp->scale              = 512;
                temp->inkEffect          = INK_NONE;
                temp->objectInteractions = true;
                temp->visible            = true;
                temp->tileCollisions     = true;
                break;
            }
            case FUNC_PROCESSOBJECTMOVEMENT:
                opcodeSize = 0;
                if (entity->tileCollisions) {
                    ProcessTileCollisions(entity);
                }
                else {
                    entity->xpos += entity->xvel;
                    entity->ypos += entity->yvel;
                }
                break;
            case FUNC_PROCESSOBJECTCONTROL:
                opcodeSize = 0;
                ProcessObjectControl(entity);
                break;
            case FUNC_PROCESSANIMATION:
                opcodeSize = 0;
                ProcessObjectAnimation(scriptInfo, entity);
                break;
            case FUNC_DRAWOBJECTANIMATION:
                opcodeSize = 0;
                if (entity->visible)
                    DrawObjectAnimation(scriptInfo, entity, (entity->xpos >> 16) - xScrollOffset, (entity->ypos >> 16) - yScrollOffset);
                break;
            case FUNC_SETMUSICTRACK:
                opcodeSize = 0;
                if (scriptEng.operands[2] <= 1)
                    SetMusicTrack(scriptText, scriptEng.operands[1], scriptEng.operands[2], 0);
                else
                    SetMusicTrack(scriptText, scriptEng.operands[1], true, scriptEng.operands[2]);
                break;
            case FUNC_PLAYMUSIC:
                opcodeSize = 0;
                PlayMusic(scriptEng.operands[0], 0);
                break;
            case FUNC_STOPMUSIC:
                opcodeSize = 0;
                StopMusic(true);
                break;
            case FUNC_PAUSEMUSIC:
                opcodeSize = 0;
                PauseSound();
                break;
            case FUNC_RESUMEMUSIC:
                opcodeSize = 0;
                ResumeSound();
                break;
            case FUNC_SWAPMUSICTRACK:
                opcodeSize = 0;
                if (scriptEng.operands[2] <= 1)
                    SwapMusicTrack(scriptText, scriptEng.operands[1], 0, scriptEng.operands[3]);
                else
                    SwapMusicTrack(scriptText, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                break;
            case FUNC_PLAYSFX:
                opcodeSize = 0;
                PlaySfx(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            case FUNC_STOPSFX:
                opcodeSize = 0;
                StopSfx(scriptEng.operands[0]);
                break;
            case FUNC_SETSFXATTRIBUTES:
                opcodeSize = 0;
                SetSfxAttributes(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            case FUNC_OBJECTTILECOLLISION:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    default: break;
                    case CSIDE_FLOOR: ObjectFloorCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_LWALL: ObjectLWallCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_RWALL: ObjectRWallCollision(scriptEng.operands[1] - 1, scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_ROOF: ObjectRoofCollision(scriptEng.operands[1], scriptEng.operands[2] - 1, scriptEng.operands[3]); break;
#if RETRO_REV03
                    // Yes, the right side also calls for LWall
                    case CSIDE_LENTITY: ObjectLWallCollision(scriptEng.operands[2], 0, objectEntityList[scriptEng.operands[1]].collisionPlane); break;
                    case CSIDE_RENTITY: ObjectLWallCollision(scriptEng.operands[2] - 1, 0, objectEntityList[scriptEng.operands[1]].collisionPlane); break;
#endif
                }
                break;
            case FUNC_OBJECTTILEGRIP:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    default: break;
                    case CSIDE_FLOOR: ObjectFloorGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_LWALL: ObjectLWallGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_RWALL: ObjectRWallGrip(scriptEng.operands[1] - 1, scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_ROOF: ObjectRoofGrip(scriptEng.operands[1], scriptEng.operands[2] - 1, scriptEng.operands[3]); break;
#if RETRO_REV03
                    case CSIDE_LENTITY: ObjectLEntityGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_RENTITY: ObjectREntityGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
#endif
                }
                break;
            case FUNC_NOT: scriptEng.operands[0] = ~scriptEng.operands[0]; break;
            case FUNC_DRAW3DSCENE:
                opcodeSize = 0;
                TransformVertexBuffer();
                Sort3DDrawList();
                Draw3DScene(scriptInfo->spriteSheetID);
                break;
            case FUNC_SETIDENTITYMATRIX:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: SetIdentityMatrix(&matWorld); break;
                    case MAT_VIEW: SetIdentityMatrix(&matView); break;
                    case MAT_TEMP: SetIdentityMatrix(&matTemp); break;
                }
                break;
            case FUNC_MATRIXMULTIPLY:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD:
                        switch (scriptEng.operands[1]) {
                            case MAT_WORLD: MatrixMultiply(&matWorld, &matWorld); break;
                            case MAT_VIEW: MatrixMultiply(&matWorld, &matView); break;
                            case MAT_TEMP: MatrixMultiply(&matWorld, &matTemp); break;
                        }
                        break;
                    case MAT_VIEW:
                        switch (scriptEng.operands[1]) {
                            case MAT_WORLD: MatrixMultiply(&matView, &matWorld); break;
                            case MAT_VIEW: MatrixMultiply(&matView, &matView); break;
                            case MAT_TEMP: MatrixMultiply(&matView, &matTemp); break;
                        }
                        break;
                    case MAT_TEMP:
                        switch (scriptEng.operands[1]) {
                            case MAT_WORLD: MatrixMultiply(&matTemp, &matWorld); break;
                            case MAT_VIEW: MatrixMultiply(&matTemp, &matView); break;
                            case MAT_TEMP: MatrixMultiply(&matTemp, &matTemp); break;
                        }
                        break;
                }
                break;
            case FUNC_MATRIXTRANSLATEXYZ:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixTranslateXYZ(&matWorld, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_VIEW: MatrixTranslateXYZ(&matView, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_TEMP: MatrixTranslateXYZ(&matTemp, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                }
                break;
            case FUNC_MATRIXSCALEXYZ:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixScaleXYZ(&matWorld, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_VIEW: MatrixScaleXYZ(&matView, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_TEMP: MatrixScaleXYZ(&matTemp, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                }
                break;
            case FUNC_MATRIXROTATEX:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixRotateX(&matWorld, scriptEng.operands[1]); break;
                    case MAT_VIEW: MatrixRotateX(&matView, scriptEng.operands[1]); break;
                    case MAT_TEMP: MatrixRotateX(&matTemp, scriptEng.operands[1]); break;
                }
                break;
            case FUNC_MATRIXROTATEY:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixRotateY(&matWorld, scriptEng.operands[1]); break;
                    case MAT_VIEW: MatrixRotateY(&matView, scriptEng.operands[1]); break;
                    case MAT_TEMP: MatrixRotateY(&matTemp, scriptEng.operands[1]); break;
                }
                break;
            case FUNC_MATRIXROTATEZ:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixRotateZ(&matWorld, scriptEng.operands[1]); break;
                    case MAT_VIEW: MatrixRotateZ(&matView, scriptEng.operands[1]); break;
                    case MAT_TEMP: MatrixRotateZ(&matTemp, scriptEng.operands[1]); break;
                }
                break;
            case FUNC_MATRIXROTATEXYZ:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixRotateXYZ(&matWorld, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_VIEW: MatrixRotateXYZ(&matView, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_TEMP: MatrixRotateXYZ(&matTemp, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                }
                break;
#if !RETRO_REV00
            case FUNC_MATRIXINVERSE:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixInverse(&matWorld); break;
                    case MAT_VIEW: MatrixInverse(&matView); break;
                    case MAT_TEMP: MatrixInverse(&matTemp); break;
                }
                break;
#endif
            case FUNC_TRANSFORMVERTICES:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: TransformVertices(&matWorld, scriptEng.operands[1], scriptEng.operands[2]); break;
                    case MAT_VIEW: TransformVertices(&matView, scriptEng.operands[1], scriptEng.operands[2]); break;
                    case MAT_TEMP: TransformVertices(&matTemp, scriptEng.operands[1], scriptEng.operands[2]); break;
                }
                break;
            case FUNC_CALLFUNCTION: {
                opcodeSize                        = 0;
                functionStack[functionStackPos++] = scriptCodePtr;
                functionStack[functionStackPos++] = jumpTableStart;
                functionStack[functionStackPos++] = scriptCodeStart;
                scriptCodeStart                   = scriptFunctionList[scriptEng.operands[0]].ptr.scriptCodePtr;
                jumpTableStart                    = scriptFunctionList[scriptEng.operands[0]].ptr.jumpTablePtr;
                scriptCodePtr                     = scriptCodeStart;
                break;
            }
            case FUNC_RETURN:
                opcodeSize = 0;
                if (!functionStackPos) { // event, stop running
                    running = false;
                }
                else { // function, jump out
                    scriptCodeStart = functionStack[--functionStackPos];
                    jumpTableStart  = functionStack[--functionStackPos];
                    scriptCodePtr   = functionStack[--functionStackPos];
                }
                break;
            case FUNC_SETLAYERDEFORMATION:
                opcodeSize = 0;
                SetLayerDeformation(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                                    scriptEng.operands[5]);
                break;
            case FUNC_CHECKTOUCHRECT: opcodeSize = 0; scriptEng.checkResult = -1;
#if !RETRO_USE_ORIGINAL_CODE
                AddDebugHitbox(H_TYPE_FINGER, NULL, scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
#endif
                for (int f = 0; f < touches; ++f) {
                    if (touchDown[f] && touchX[f] > scriptEng.operands[0] && touchX[f] < scriptEng.operands[2] && touchY[f] > scriptEng.operands[1]
                        && touchY[f] < scriptEng.operands[3]) {
                        scriptEng.checkResult = f;
                    }
                }
                break;
            case FUNC_GETTILELAYERENTRY:
                scriptEng.operands[0] = stageLayouts[scriptEng.operands[1]].tiles[scriptEng.operands[2] + 0x100 * scriptEng.operands[3]];
                break;
            case FUNC_SETTILELAYERENTRY:
                stageLayouts[scriptEng.operands[1]].tiles[scriptEng.operands[2] + 0x100 * scriptEng.operands[3]] = scriptEng.operands[0];
                break;
            case FUNC_GETBIT: scriptEng.operands[0] = (scriptEng.operands[1] & (1 << scriptEng.operands[2])) >> scriptEng.operands[2]; break;
            case FUNC_SETBIT:
                if (scriptEng.operands[2] <= 0)
                    scriptEng.operands[0] &= ~(1 << scriptEng.operands[1]);
                else
                    scriptEng.operands[0] |= 1 << scriptEng.operands[1];
                break;
            case FUNC_CLEARDRAWLIST:
                opcodeSize                                      = 0;
                drawListEntries[scriptEng.operands[0]].listSize = 0;
                break;
            case FUNC_ADDDRAWLISTENTITYREF: {
                opcodeSize                                                                                           = 0;
                drawListEntries[scriptEng.operands[0]].entityRefs[drawListEntries[scriptEng.operands[0]].listSize++] = scriptEng.operands[1];
                break;
            }
            case FUNC_GETDRAWLISTENTITYREF: scriptEng.operands[0] = drawListEntries[scriptEng.operands[1]].entityRefs[scriptEng.operands[2]]; break;
            case FUNC_SETDRAWLISTENTITYREF:
                opcodeSize                                                               = 0;
                drawListEntries[scriptEng.operands[1]].entityRefs[scriptEng.operands[2]] = scriptEng.operands[0];
                break;
            case FUNC_GET16X16TILEINFO: {
                scriptEng.operands[4] = scriptEng.operands[1] >> 7;
                scriptEng.operands[5] = scriptEng.operands[2] >> 7;
                scriptEng.operands[6] = stageLayouts[0].tiles[scriptEng.operands[4] + (scriptEng.operands[5] << 8)] << 6;
                scriptEng.operands[6] += ((scriptEng.operands[1] & 0x7F) >> 4) + 8 * ((scriptEng.operands[2] & 0x7F) >> 4);
                int index = tiles128x128.tileIndex[scriptEng.operands[6]];
                switch (scriptEng.operands[3]) {
                    case TILEINFO_INDEX: scriptEng.operands[0] = tiles128x128.tileIndex[scriptEng.operands[6]]; break;
                    case TILEINFO_DIRECTION: scriptEng.operands[0] = tiles128x128.direction[scriptEng.operands[6]]; break;
                    case TILEINFO_VISUALPLANE: scriptEng.operands[0] = tiles128x128.visualPlane[scriptEng.operands[6]]; break;
                    case TILEINFO_SOLIDITYA: scriptEng.operands[0] = tiles128x128.collisionFlags[0][scriptEng.operands[6]]; break;
                    case TILEINFO_SOLIDITYB: scriptEng.operands[0] = tiles128x128.collisionFlags[1][scriptEng.operands[6]]; break;
                    case TILEINFO_FLAGSA: scriptEng.operands[0] = collisionMasks[0].flags[index]; break;
                    case TILEINFO_ANGLEA: scriptEng.operands[0] = collisionMasks[0].angles[index]; break;
                    case TILEINFO_FLAGSB: scriptEng.operands[0] = collisionMasks[1].flags[index]; break;
                    case TILEINFO_ANGLEB: scriptEng.operands[0] = collisionMasks[1].angles[index]; break;
                    default: break;
                }
                break;
            }
            case FUNC_SET16X16TILEINFO: {
                scriptEng.operands[4] = scriptEng.operands[1] >> 7;
                scriptEng.operands[5] = scriptEng.operands[2] >> 7;
                scriptEng.operands[6] = stageLayouts[0].tiles[scriptEng.operands[4] + (scriptEng.operands[5] << 8)] << 6;
                scriptEng.operands[6] += ((scriptEng.operands[1] & 0x7F) >> 4) + 8 * ((scriptEng.operands[2] & 0x7F) >> 4);
                switch (scriptEng.operands[3]) {
                    case TILEINFO_INDEX:
                        tiles128x128.tileIndex[scriptEng.operands[6]]  = scriptEng.operands[0];
                        tiles128x128.gfxDataPos[scriptEng.operands[6]] = scriptEng.operands[0] << 8;
                        break;
                    case TILEINFO_DIRECTION: tiles128x128.direction[scriptEng.operands[6]] = scriptEng.operands[0]; break;
                    case TILEINFO_VISUALPLANE: tiles128x128.visualPlane[scriptEng.operands[6]] = scriptEng.operands[0]; break;
                    case TILEINFO_SOLIDITYA: tiles128x128.collisionFlags[0][scriptEng.operands[6]] = scriptEng.operands[0]; break;
                    case TILEINFO_SOLIDITYB: tiles128x128.collisionFlags[1][scriptEng.operands[6]] = scriptEng.operands[0]; break;
                    case TILEINFO_FLAGSA: collisionMasks[1].flags[tiles128x128.tileIndex[scriptEng.operands[6]]] = scriptEng.operands[0]; break;
                    case TILEINFO_ANGLEA: collisionMasks[1].angles[tiles128x128.tileIndex[scriptEng.operands[6]]] = scriptEng.operands[0]; break;
                    default: break;
                }
                break;
            }
            case FUNC_COPY16X16TILE:
                opcodeSize = 0;
                Copy16x16Tile(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            case FUNC_GETANIMATIONBYNAME: {
                AnimationFile *animFile = scriptInfo->animFile;
                scriptEng.operands[0]   = -1;
                int id                  = 0;
                while (scriptEng.operands[0] == -1) {
                    SpriteAnimation *anim = &animationList[animFile->aniListOffset + id];
                    if (StrComp(scriptText, anim->name))
                        scriptEng.operands[0] = id;
                    else if (++id == animFile->animCount)
                        scriptEng.operands[0] = 0;
                }
                break;
            }
            case FUNC_READSAVERAM:
                opcodeSize            = 0;
                scriptEng.checkResult = ReadSaveRAMData();
                break;
            case FUNC_WRITESAVERAM:
                opcodeSize            = 0;
                scriptEng.checkResult = WriteSaveRAMData();
                break;
#if !RETRO_REV02
            case FUNC_LOADTEXTFONT: {
                opcodeSize = 0;
                LoadFontFile(scriptText);
                break;
            }
#endif
            case FUNC_LOADTEXTFILE: {
                opcodeSize     = 0;
                TextMenu *menu = &gameMenu[scriptEng.operands[0]];
#if !RETRO_REV02
                LoadTextFile(menu, scriptText, scriptEng.operands[2] != 0);
#else
                LoadTextFile(menu, scriptText, false);
#endif
                break;
            }
            case FUNC_GETTEXTINFO: {
                TextMenu *menu = &gameMenu[scriptEng.operands[1]];
                switch (scriptEng.operands[2]) {
                    case TEXTINFO_TEXTDATA:
                        scriptEng.operands[0] = menu->textData[menu->entryStart[scriptEng.operands[3]] + scriptEng.operands[4]];
                        break;
                    case TEXTINFO_TEXTSIZE: scriptEng.operands[0] = menu->entrySize[scriptEng.operands[3]]; break;
                    case TEXTINFO_ROWCOUNT: scriptEng.operands[0] = menu->rowCount; break;
                }
                break;
            }
#if !RETRO_REV02
            case FUNC_DRAWTEXT: {
                opcodeSize        = 0;
                textMenuSurfaceNo = scriptInfo->spriteSheetID;
                TextMenu *menu    = &gameMenu[scriptEng.operands[0]];
                DrawBitmapText(menu, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                               scriptEng.operands[5], scriptEng.operands[6]);
                break;
            }
#endif
            case FUNC_GETVERSIONNUMBER: {
                opcodeSize                           = 0;
                TextMenu *menu                       = &gameMenu[scriptEng.operands[0]];
                menu->entryHighlight[menu->rowCount] = scriptEng.operands[1];
                AddTextMenuEntry(menu, Engine.gameVersion);
                break;
            }
            case FUNC_GETTABLEVALUE: {
                int arrPos = scriptEng.operands[1];
                if (arrPos >= 0) {
                    int pos     = scriptEng.operands[2];
                    int arrSize = scriptCode[pos];
                    if (arrPos < arrSize)
                        scriptEng.operands[0] = scriptCode[pos + arrPos + 1];
                }
                break;
            }
            case FUNC_SETTABLEVALUE: {
                opcodeSize = 0;
                int arrPos = scriptEng.operands[1];
                if (arrPos >= 0) {
                    int pos     = scriptEng.operands[2];
                    int arrSize = scriptCode[pos];
                    if (arrPos < arrSize)
                        scriptCode[pos + arrPos + 1] = scriptEng.operands[0];
                }
                break;
            }
            case FUNC_CHECKCURRENTSTAGEFOLDER:
                opcodeSize            = 0;
                scriptEng.checkResult = StrComp(stageList[activeStageList][stageListPosition].folder, scriptText);
#if RETRO_REV03
                // Mission Mode stuff
                if (!scriptEng.checkResult) {
                    int targetLength  = strlen(stageList[activeStageList][stageListPosition].folder);
                    int currentLength = strlen(scriptText);
                    if (targetLength > currentLength) {
                        scriptEng.checkResult =
                            StrComp(&stageList[activeStageList][stageListPosition].folder[targetLength - currentLength], scriptText);
                    }
                }
#endif
                break;
            case FUNC_ABS: {
                scriptEng.operands[0] = abs(scriptEng.operands[0]);
                break;
            }
            case FUNC_CALLNATIVEFUNCTION:
                opcodeSize = 0;
                if (scriptEng.operands[0] >= 0 && scriptEng.operands[0] < NATIIVEFUNCTION_COUNT) {
                    void (*func)(void) = (void (*)(void))nativeFunction[scriptEng.operands[0]];
                    if (func)
                        func();
                }
                break;
            case FUNC_CALLNATIVEFUNCTION2:
                if (scriptEng.operands[0] >= 0 && scriptEng.operands[0] < NATIIVEFUNCTION_COUNT) {
                    if (StrLength(scriptText)) {
                        void (*func)(int *, char *) = (void (*)(int *, char *))nativeFunction[scriptEng.operands[0]];
                        if (func)
                            func(&scriptEng.operands[2], scriptText);
                    }
                    else {
                        void (*func)(int *, int *) = (void (*)(int *, int *))nativeFunction[scriptEng.operands[0]];
                        if (func)
                            func(&scriptEng.operands[1], &scriptEng.operands[2]);
                    }
                }
                break;
            case FUNC_CALLNATIVEFUNCTION4:
                if (scriptEng.operands[0] >= 0 && scriptEng.operands[0] < NATIIVEFUNCTION_COUNT) {
                    if (StrLength(scriptText)) {
                        void (*func)(int *, char *, int *, int *) = (void (*)(int *, char *, int *, int *))nativeFunction[scriptEng.operands[0]];
                        if (func)
                            func(&scriptEng.operands[1], scriptText, &scriptEng.operands[3], &scriptEng.operands[4]);
                    }
                    else {
                        void (*func)(int *, int *, int *, int *) = (void (*)(int *, int *, int *, int *))nativeFunction[scriptEng.operands[0]];
                        if (func)
                            func(&scriptEng.operands[1], &scriptEng.operands[2], &scriptEng.operands[3], &scriptEng.operands[4]);
                    }
                }
                break;
            case FUNC_SETOBJECTRANGE: {
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = range

                opcodeSize       = 0;
                int offset       = (scriptEng.operands[0] >> 1) - SCREEN_CENTERX;
                OBJECT_BORDER_X1 = offset + 0x80;
                OBJECT_BORDER_X2 = scriptEng.operands[0] + 0x80 - offset;
                OBJECT_BORDER_X3 = offset + 0x20;
                OBJECT_BORDER_X4 = scriptEng.operands[0] + 0x20 - offset;
                break;
            }
#if RETRO_REV02
            case FUNC_GETOBJECTVALUE: {
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = result
                // scriptEng.operands[1] = valueID
                // scriptEng.operands[2] = entitySlot

                if (scriptEng.operands[1] < 48)
                    scriptEng.operands[0] = objectEntityList[scriptEng.operands[2]].values[scriptEng.operands[1]];
                break;
            }
            case FUNC_SETOBJECTVALUE: {
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = value
                // scriptEng.operands[1] = valueID
                // scriptEng.operands[2] = entitySlot

                opcodeSize = 0;
                if (scriptEng.operands[1] < 48)
                    objectEntityList[scriptEng.operands[2]].values[scriptEng.operands[1]] = scriptEng.operands[0];
                break;
            }
            case FUNC_COPYOBJECT: {
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = destSlot
                // scriptEng.operands[1] = srcSlot
                // scriptEng.operands[2] = count

                Entity *dstList = &objectEntityList[scriptEng.operands[0]];
                Entity *srcList = &objectEntityList[scriptEng.operands[1]];
                for (int i = 0; i < scriptEng.operands[2]; ++i) memcpy(&dstList[i], &srcList[i], sizeof(Entity));
                break;
            }
#endif
            case FUNC_PRINT: {
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = message (can be a regular value or a string depending on scriptEng.operands[1])
                // scriptEng.operands[1] = isInt
                // scriptEng.operands[2] = useEndLine

                endLine = false;
                if (scriptEng.operands[1])
                    PrintLog("%d", scriptEng.operands[0]);
                else
                    PrintLog("%s", scriptText);

                if (scriptEng.operands[2])
                    PrintLog("\n");
                endLine = true;
                break;
            }

#if RETRO_REV03
                // Extras for origins 2PVS,
                // most of these aren't (and won't be) implemented here because they rely on v5 tech that isn't part of the scope of this project
            case FUNC_CHECKCAMERAPROXIMITY:
                scriptEng.checkResult = false;

                // FUNCTION PARAMS:
                // scriptEng.operands[0] = pos.x
                // scriptEng.operands[1] = pos.y
                // scriptEng.operands[2] = range.x
                // scriptEng.operands[3] = range.y
                //
                // FUNCTION NOTES:
                // - Sets scriptEng.checkResult

                if (scriptEng.operands[2] > 0 && scriptEng.operands[3] > 0) {
                    int sx = abs(scriptEng.operands[0] - cameraXPos);
                    int sy = abs(scriptEng.operands[1] - cameraYPos);

                    if (sx < scriptEng.operands[2] && sy < scriptEng.operands[3]) {
                        scriptEng.checkResult = true;
                        break;
                    }
                }
                else {
                    if (scriptEng.operands[2] > 0) {
                        int sx = abs(scriptEng.operands[0] - cameraXPos);

                        if (sx < scriptEng.operands[2]) {
                            scriptEng.checkResult = true;
                            break;
                        }
                    }
                    else if (scriptEng.operands[3] > 0) {
                        int sy = abs(scriptEng.operands[1] - cameraYPos);

                        if (sy < scriptEng.operands[3]) {
                            scriptEng.checkResult = true;
                            break;
                        }
                    }
                }
                break;

            case FUNC_SETSCREENCOUNT:
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = screenCount

                break;

            case FUNC_SETSCREENVERTICES:
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = startVert2P_S1
                // scriptEng.operands[1] = startVert2P_S2
                // scriptEng.operands[2] = startVert3P_S1
                // scriptEng.operands[3] = startVert3P_S2
                // scriptEng.operands[4] = startVert3P_S3

                break;

            case FUNC_GETINPUTDEVICEID:
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = deviceID
                // scriptEng.operands[1] = inputSlot
                //
                // FUNCTION NOTES:
                // - Assigns the device's id to scriptEng.operands[0]

                break;

            case FUNC_GETFILTEREDINPUTDEVICEID:
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = deviceID
                // scriptEng.operands[1] = confirmOnly
                // scriptEng.operands[2] = unassignedOnly
                // scriptEng.operands[3] = maxInactiveTimer
                //
                // FUNCTION NOTES:
                // - Assigns the filtered device's id to scriptEng.operands[0]

                break;

            case FUNC_GETINPUTDEVICETYPE:
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = deviceType
                // scriptEng.operands[1] = deviceID
                //
                // FUNCTION NOTES:
                // - Assigns the device's type to scriptEng.operands[0]

                break;

            case FUNC_ISINPUTDEVICEASSIGNED:
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = deviceID

                break;

            case FUNC_ASSIGNINPUTSLOTTODEVICE:
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = inputSlot
                // scriptEng.operands[1] = deviceID

                break;

            case FUNC_ISSLOTASSIGNED:
                // FUNCTION PARAMS:
                // scriptEng.operands[0] = inputSlot
                //
                // FUNCTION NOTES:
                // - Sets scriptEng.checkResult

                break;

            case FUNC_RESETINPUTSLOTASSIGNMENTS:
                // FUNCTION PARAMS:
                // None

                break;
#endif
        }

        // Set Values
        if (opcodeSize > 0)
            scriptCodePtr -= scriptCodePtr - scriptCodeOffset;
        for (int i = 0; i < opcodeSize; ++i) {
            int opcodeType = scriptCode[scriptCodePtr++];
            if (opcodeType == SCRIPTVAR_VAR) {
                int arrayVal = 0;
                switch (scriptCode[scriptCodePtr++]) { // variable
                    case VARARR_NONE: arrayVal = objectEntityPos; break;

                    case VARARR_ARRAY:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = scriptEng.arrayPosition[scriptCode[scriptCodePtr++]];
                        else
                            arrayVal = scriptCode[scriptCodePtr++];
                        break;

                    case VARARR_ENTNOPLUS1:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = objectEntityPos + scriptEng.arrayPosition[scriptCode[scriptCodePtr++]];
                        else
                            arrayVal = objectEntityPos + scriptCode[scriptCodePtr++];
                        break;

                    case VARARR_ENTNOMINUS1:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = objectEntityPos - scriptEng.arrayPosition[scriptCode[scriptCodePtr++]];
                        else
                            arrayVal = objectEntityPos - scriptCode[scriptCodePtr++];
                        break;

                    default: break;
                }

#if RETRO_REV03 && !RETRO_USE_ORIGINAL_CODE
                bool inputCheck = true; // Default to true for mobile bytecode
                // If we're using the scripts or an Origins datafile, check the array value
                if (forceUseScripts || Engine.usingOrigins)
                    inputCheck = arrayVal <= 1;
#endif

                // Variables
                switch (scriptCode[scriptCodePtr++]) {
                    default: break;
                    case VAR_TEMP0: scriptEng.temp[0] = scriptEng.operands[i]; break;
                    case VAR_TEMP1: scriptEng.temp[1] = scriptEng.operands[i]; break;
                    case VAR_TEMP2: scriptEng.temp[2] = scriptEng.operands[i]; break;
                    case VAR_TEMP3: scriptEng.temp[3] = scriptEng.operands[i]; break;
                    case VAR_TEMP4: scriptEng.temp[4] = scriptEng.operands[i]; break;
                    case VAR_TEMP5: scriptEng.temp[5] = scriptEng.operands[i]; break;
                    case VAR_TEMP6: scriptEng.temp[6] = scriptEng.operands[i]; break;
                    case VAR_TEMP7: scriptEng.temp[7] = scriptEng.operands[i]; break;
                    case VAR_CHECKRESULT: scriptEng.checkResult = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS0: scriptEng.arrayPosition[0] = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS1: scriptEng.arrayPosition[1] = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS2: scriptEng.arrayPosition[2] = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS3: scriptEng.arrayPosition[3] = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS4: scriptEng.arrayPosition[4] = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS5: scriptEng.arrayPosition[5] = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS6: scriptEng.arrayPosition[6] = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS7: scriptEng.arrayPosition[7] = scriptEng.operands[i]; break;
                    case VAR_GLOBAL: globalVariables[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_LOCAL: scriptCode[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_OBJECTENTITYPOS: break;
                    case VAR_OBJECTGROUPID: {
                        objectEntityList[arrayVal].groupID = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTTYPE: {
                        objectEntityList[arrayVal].type = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTPROPERTYVALUE: {
                        objectEntityList[arrayVal].propertyValue = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTXPOS: {
                        objectEntityList[arrayVal].xpos = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTYPOS: {
                        objectEntityList[arrayVal].ypos = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTIXPOS: {
                        objectEntityList[arrayVal].xpos = scriptEng.operands[i] << 16;
                        break;
                    }
                    case VAR_OBJECTIYPOS: {
                        objectEntityList[arrayVal].ypos = scriptEng.operands[i] << 16;
                        break;
                    }
                    case VAR_OBJECTXVEL: {
                        objectEntityList[arrayVal].xvel = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTYVEL: {
                        objectEntityList[arrayVal].yvel = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTSPEED: {
                        objectEntityList[arrayVal].speed = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTSTATE: {
                        objectEntityList[arrayVal].state = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTROTATION: {
                        objectEntityList[arrayVal].rotation = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTSCALE: {
                        objectEntityList[arrayVal].scale = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTPRIORITY: {
                        objectEntityList[arrayVal].priority = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTDRAWORDER: {
                        objectEntityList[arrayVal].drawOrder = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTDIRECTION: {
                        objectEntityList[arrayVal].direction = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTINKEFFECT: {
                        objectEntityList[arrayVal].inkEffect = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTALPHA: {
                        objectEntityList[arrayVal].alpha = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTFRAME: {
                        objectEntityList[arrayVal].frame = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTANIMATION: {
                        objectEntityList[arrayVal].animation = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTPREVANIMATION: {
                        objectEntityList[arrayVal].prevAnimation = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTANIMATIONSPEED: {
                        objectEntityList[arrayVal].animationSpeed = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTANIMATIONTIMER: {
                        objectEntityList[arrayVal].animationTimer = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTANGLE: {
                        objectEntityList[arrayVal].angle = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTLOOKPOSX: {
                        objectEntityList[arrayVal].lookPosX = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTLOOKPOSY: {
                        objectEntityList[arrayVal].lookPosY = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTCOLLISIONMODE: {
                        objectEntityList[arrayVal].collisionMode = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTCOLLISIONPLANE: {
                        objectEntityList[arrayVal].collisionPlane = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTCONTROLMODE: {
                        objectEntityList[arrayVal].controlMode = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTCONTROLLOCK: {
                        objectEntityList[arrayVal].controlLock = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTPUSHING: {
                        objectEntityList[arrayVal].pushing = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVISIBLE: {
                        objectEntityList[arrayVal].visible = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTTILECOLLISIONS: {
                        objectEntityList[arrayVal].tileCollisions = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTINTERACTION: {
                        objectEntityList[arrayVal].objectInteractions = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTGRAVITY: {
                        objectEntityList[arrayVal].gravity = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTUP: {
                        objectEntityList[arrayVal].up = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTDOWN: {
                        objectEntityList[arrayVal].down = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTLEFT: {
                        objectEntityList[arrayVal].left = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTRIGHT: {
                        objectEntityList[arrayVal].right = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTJUMPPRESS: {
                        objectEntityList[arrayVal].jumpPress = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTJUMPHOLD: {
                        objectEntityList[arrayVal].jumpHold = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTSCROLLTRACKING: {
                        objectEntityList[arrayVal].scrollTracking = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTFLOORSENSORL: {
                        objectEntityList[arrayVal].floorSensors[0] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTFLOORSENSORC: {
                        objectEntityList[arrayVal].floorSensors[1] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTFLOORSENSORR: {
                        objectEntityList[arrayVal].floorSensors[2] = scriptEng.operands[i];
                        break;
                    }
#if !RETRO_REV00
                    case VAR_OBJECTFLOORSENSORLC: {
                        objectEntityList[arrayVal].floorSensors[3] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTFLOORSENSORRC: {
                        objectEntityList[arrayVal].floorSensors[4] = scriptEng.operands[i];
                        break;
                    }
#endif
                    case VAR_OBJECTCOLLISIONLEFT: {
                        break;
                    }
                    case VAR_OBJECTCOLLISIONTOP: {
                        break;
                    }
                    case VAR_OBJECTCOLLISIONRIGHT: {
                        break;
                    }
                    case VAR_OBJECTCOLLISIONBOTTOM: {
                        break;
                    }
                    case VAR_OBJECTOUTOFBOUNDS: {
                        break;
                    }
                    case VAR_OBJECTSPRITESHEET: {
                        objectScriptList[objectEntityList[arrayVal].type].spriteSheetID = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE0: {
                        objectEntityList[arrayVal].values[0] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE1: {
                        objectEntityList[arrayVal].values[1] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE2: {
                        objectEntityList[arrayVal].values[2] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE3: {
                        objectEntityList[arrayVal].values[3] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE4: {
                        objectEntityList[arrayVal].values[4] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE5: {
                        objectEntityList[arrayVal].values[5] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE6: {
                        objectEntityList[arrayVal].values[6] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE7: {
                        objectEntityList[arrayVal].values[7] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE8: {
                        objectEntityList[arrayVal].values[8] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE9: {
                        objectEntityList[arrayVal].values[9] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE10: {
                        objectEntityList[arrayVal].values[10] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE11: {
                        objectEntityList[arrayVal].values[11] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE12: {
                        objectEntityList[arrayVal].values[12] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE13: {
                        objectEntityList[arrayVal].values[13] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE14: {
                        objectEntityList[arrayVal].values[14] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE15: {
                        objectEntityList[arrayVal].values[15] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE16: {
                        objectEntityList[arrayVal].values[16] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE17: {
                        objectEntityList[arrayVal].values[17] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE18: {
                        objectEntityList[arrayVal].values[18] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE19: {
                        objectEntityList[arrayVal].values[19] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE20: {
                        objectEntityList[arrayVal].values[20] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE21: {
                        objectEntityList[arrayVal].values[21] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE22: {
                        objectEntityList[arrayVal].values[22] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE23: {
                        objectEntityList[arrayVal].values[23] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE24: {
                        objectEntityList[arrayVal].values[24] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE25: {
                        objectEntityList[arrayVal].values[25] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE26: {
                        objectEntityList[arrayVal].values[26] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE27: {
                        objectEntityList[arrayVal].values[27] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE28: {
                        objectEntityList[arrayVal].values[28] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE29: {
                        objectEntityList[arrayVal].values[29] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE30: {
                        objectEntityList[arrayVal].values[30] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE31: {
                        objectEntityList[arrayVal].values[31] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE32: {
                        objectEntityList[arrayVal].values[32] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE33: {
                        objectEntityList[arrayVal].values[33] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE34: {
                        objectEntityList[arrayVal].values[34] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE35: {
                        objectEntityList[arrayVal].values[35] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE36: {
                        objectEntityList[arrayVal].values[36] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE37: {
                        objectEntityList[arrayVal].values[37] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE38: {
                        objectEntityList[arrayVal].values[38] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE39: {
                        objectEntityList[arrayVal].values[39] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE40: {
                        objectEntityList[arrayVal].values[40] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE41: {
                        objectEntityList[arrayVal].values[41] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE42: {
                        objectEntityList[arrayVal].values[42] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE43: {
                        objectEntityList[arrayVal].values[43] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE44: {
                        objectEntityList[arrayVal].values[44] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE45: {
                        objectEntityList[arrayVal].values[45] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE46: {
                        objectEntityList[arrayVal].values[46] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE47: {
                        objectEntityList[arrayVal].values[47] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_STAGESTATE: stageMode = scriptEng.operands[i]; break;
                    case VAR_STAGEACTIVELIST: activeStageList = scriptEng.operands[i]; break;
                    case VAR_STAGELISTPOS: stageListPosition = scriptEng.operands[i]; break;
                    case VAR_STAGETIMEENABLED: timeEnabled = scriptEng.operands[i]; break;
                    case VAR_STAGEMILLISECONDS: stageMilliseconds = scriptEng.operands[i]; break;
                    case VAR_STAGESECONDS: stageSeconds = scriptEng.operands[i]; break;
                    case VAR_STAGEMINUTES: stageMinutes = scriptEng.operands[i]; break;
                    case VAR_STAGEACTNUM: actID = scriptEng.operands[i]; break;
                    case VAR_STAGEPAUSEENABLED: pauseEnabled = scriptEng.operands[i]; break;
                    case VAR_STAGELISTSIZE: break;
                    case VAR_STAGENEWXBOUNDARY1: newXBoundary1 = scriptEng.operands[i]; break;
                    case VAR_STAGENEWXBOUNDARY2: newXBoundary2 = scriptEng.operands[i]; break;
                    case VAR_STAGENEWYBOUNDARY1: newYBoundary1 = scriptEng.operands[i]; break;
                    case VAR_STAGENEWYBOUNDARY2: newYBoundary2 = scriptEng.operands[i]; break;
                    case VAR_STAGECURXBOUNDARY1:
                        if (curXBoundary1 != scriptEng.operands[i]) {
                            curXBoundary1 = scriptEng.operands[i];
                            newXBoundary1 = scriptEng.operands[i];
                        }
                        break;
                    case VAR_STAGECURXBOUNDARY2:
                        if (curXBoundary2 != scriptEng.operands[i]) {
                            curXBoundary2 = scriptEng.operands[i];
                            newXBoundary2 = scriptEng.operands[i];
                        }
                        break;
                    case VAR_STAGECURYBOUNDARY1:
                        if (curYBoundary1 != scriptEng.operands[i]) {
                            curYBoundary1 = scriptEng.operands[i];
                            newYBoundary1 = scriptEng.operands[i];
                        }
                        break;
                    case VAR_STAGECURYBOUNDARY2:
                        if (curYBoundary2 != scriptEng.operands[i]) {
                            curYBoundary2 = scriptEng.operands[i];
                            newYBoundary2 = scriptEng.operands[i];
                        }
                        break;
                    case VAR_STAGEDEFORMATIONDATA0: bgDeformationData0[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEDEFORMATIONDATA1: bgDeformationData1[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEDEFORMATIONDATA2: bgDeformationData2[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEDEFORMATIONDATA3: bgDeformationData3[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEWATERLEVEL: waterLevel = scriptEng.operands[i]; break;
                    case VAR_STAGEACTIVELAYER: activeTileLayers[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEMIDPOINT: tLayerMidPoint = scriptEng.operands[i]; break;
                    case VAR_STAGEPLAYERLISTPOS: playerListPos = scriptEng.operands[i]; break;
                    case VAR_STAGEDEBUGMODE: debugMode = scriptEng.operands[i]; break;
                    case VAR_STAGEENTITYPOS: objectEntityPos = scriptEng.operands[i]; break;
                    case VAR_SCREENCAMERAENABLED: cameraEnabled = scriptEng.operands[i]; break;
                    case VAR_SCREENCAMERATARGET: cameraTarget = scriptEng.operands[i]; break;
                    case VAR_SCREENCAMERASTYLE: cameraStyle = scriptEng.operands[i]; break;
                    case VAR_SCREENCAMERAX: cameraXPos = scriptEng.operands[i]; break;
                    case VAR_SCREENCAMERAY: cameraYPos = scriptEng.operands[i]; break;
                    case VAR_SCREENDRAWLISTSIZE: drawListEntries[arrayVal].listSize = scriptEng.operands[i]; break;
                    case VAR_SCREENXCENTER: break;
                    case VAR_SCREENYCENTER: break;
                    case VAR_SCREENXSIZE: break;
                    case VAR_SCREENYSIZE: break;
                    case VAR_SCREENXOFFSET: xScrollOffset = scriptEng.operands[i]; break;
                    case VAR_SCREENYOFFSET: yScrollOffset = scriptEng.operands[i]; break;
                    case VAR_SCREENSHAKEX: cameraShakeX = scriptEng.operands[i]; break;
                    case VAR_SCREENSHAKEY: cameraShakeY = scriptEng.operands[i]; break;
                    case VAR_SCREENADJUSTCAMERAY: cameraAdjustY = scriptEng.operands[i]; break;
                    case VAR_TOUCHSCREENDOWN: break;
                    case VAR_TOUCHSCREENXPOS: break;
                    case VAR_TOUCHSCREENYPOS: break;
                    case VAR_MUSICVOLUME: SetMusicVolume(scriptEng.operands[i]); break;
                    case VAR_MUSICCURRENTTRACK: break;
                    case VAR_MUSICPOSITION: break;
#if RETRO_REV03 && !RETRO_USE_ORIGINAL_CODE
                    case VAR_KEYDOWNUP:
                        if (inputCheck)
                            keyDown.up = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNDOWN:
                        if (inputCheck)
                            keyDown.down = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNLEFT:
                        if (inputCheck)
                            keyDown.left = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNRIGHT:
                        if (inputCheck)
                            keyDown.right = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNBUTTONA:
                        if (inputCheck)
                            keyDown.A = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNBUTTONB:
                        if (inputCheck)
                            keyDown.B = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNBUTTONC:
                        if (inputCheck)
                            keyDown.C = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNBUTTONX:
                        if (inputCheck)
                            keyDown.X = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNBUTTONY:
                        if (inputCheck)
                            keyDown.Y = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNBUTTONZ:
                        if (inputCheck)
                            keyDown.Z = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNBUTTONL:
                        if (inputCheck)
                            keyDown.L = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNBUTTONR:
                        if (inputCheck)
                            keyDown.R = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNSTART:
                        if (inputCheck)
                            keyDown.start = scriptEng.operands[i];
                        break;
                    case VAR_KEYDOWNSELECT:
                        if (inputCheck)
                            keyDown.select = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSUP:
                        if (inputCheck)
                            keyPress.up = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSDOWN:
                        if (inputCheck)
                            keyPress.down = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSLEFT:
                        if (inputCheck)
                            keyPress.left = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSRIGHT:
                        if (inputCheck)
                            keyPress.right = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSBUTTONA:
                        if (inputCheck)
                            keyPress.A = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSBUTTONB:
                        if (inputCheck)
                            keyPress.B = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSBUTTONC:
                        if (inputCheck)
                            keyPress.C = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSBUTTONX:
                        if (inputCheck)
                            keyPress.X = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSBUTTONY:
                        if (inputCheck)
                            keyPress.Y = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSBUTTONZ:
                        if (inputCheck)
                            keyPress.Z = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSBUTTONL:
                        if (inputCheck)
                            keyPress.L = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSBUTTONR:
                        if (inputCheck)
                            keyPress.R = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSSTART:
                        if (inputCheck)
                            keyPress.start = scriptEng.operands[i];
                        break;
                    case VAR_KEYPRESSSELECT:
                        if (inputCheck)
                            keyPress.select = scriptEng.operands[i];
                        break;
#else
                    case VAR_KEYDOWNUP: keyDown.up = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNDOWN: keyDown.down = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNLEFT: keyDown.left = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNRIGHT: keyDown.right = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONA: keyDown.A = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONB: keyDown.B = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONC: keyDown.C = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONX: keyDown.X = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONY: keyDown.Y = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONZ: keyDown.Z = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONL: keyDown.L = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONR: keyDown.R = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNSTART: keyDown.start = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNSELECT: keyDown.select = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSUP: keyPress.up = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSDOWN: keyPress.down = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSLEFT: keyPress.left = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSRIGHT: keyPress.right = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONA: keyPress.A = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONB: keyPress.B = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONC: keyPress.C = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONX: keyPress.X = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONY: keyPress.Y = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONZ: keyPress.Z = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONL: keyPress.L = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONR: keyPress.R = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSSTART: keyPress.start = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSSELECT: keyPress.select = scriptEng.operands[i]; break;
#endif
                    case VAR_MENU1SELECTION: gameMenu[0].selection1 = scriptEng.operands[i]; break;
                    case VAR_MENU2SELECTION: gameMenu[1].selection1 = scriptEng.operands[i]; break;
                    case VAR_TILELAYERXSIZE: stageLayouts[arrayVal].xsize = scriptEng.operands[i]; break;
                    case VAR_TILELAYERYSIZE: stageLayouts[arrayVal].ysize = scriptEng.operands[i]; break;
                    case VAR_TILELAYERTYPE: stageLayouts[arrayVal].type = scriptEng.operands[i]; break;
                    case VAR_TILELAYERANGLE: {
                        int angle = scriptEng.operands[i] + 0x200;
                        if (scriptEng.operands[i] >= 0)
                            angle = scriptEng.operands[i];
                        stageLayouts[arrayVal].angle = angle & 0x1FF;
                        break;
                    }
                    case VAR_TILELAYERXPOS: stageLayouts[arrayVal].xpos = scriptEng.operands[i]; break;
                    case VAR_TILELAYERYPOS: stageLayouts[arrayVal].ypos = scriptEng.operands[i]; break;
                    case VAR_TILELAYERZPOS: stageLayouts[arrayVal].zpos = scriptEng.operands[i]; break;
                    case VAR_TILELAYERPARALLAXFACTOR: stageLayouts[arrayVal].parallaxFactor = scriptEng.operands[i]; break;
                    case VAR_TILELAYERSCROLLSPEED: stageLayouts[arrayVal].scrollSpeed = scriptEng.operands[i]; break;
                    case VAR_TILELAYERSCROLLPOS: stageLayouts[arrayVal].scrollPos = scriptEng.operands[i]; break;
                    case VAR_TILELAYERDEFORMATIONOFFSET: stageLayouts[arrayVal].deformationOffset = scriptEng.operands[i]; break;
                    case VAR_TILELAYERDEFORMATIONOFFSETW: stageLayouts[arrayVal].deformationOffsetW = scriptEng.operands[i]; break;
                    case VAR_HPARALLAXPARALLAXFACTOR: hParallax.parallaxFactor[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_HPARALLAXSCROLLSPEED: hParallax.scrollSpeed[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_HPARALLAXSCROLLPOS: hParallax.scrollPos[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_VPARALLAXPARALLAXFACTOR: vParallax.parallaxFactor[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_VPARALLAXSCROLLSPEED: vParallax.scrollSpeed[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_VPARALLAXSCROLLPOS: vParallax.scrollPos[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_SCENE3DVERTEXCOUNT: vertexCount = scriptEng.operands[i]; break;
                    case VAR_SCENE3DFACECOUNT: faceCount = scriptEng.operands[i]; break;
                    case VAR_SCENE3DPROJECTIONX: projectionX = scriptEng.operands[i]; break;
                    case VAR_SCENE3DPROJECTIONY: projectionY = scriptEng.operands[i]; break;
#if !RETRO_REV00
                    case VAR_SCENE3DFOGCOLOR: fogColor = scriptEng.operands[i]; break;
                    case VAR_SCENE3DFOGSTRENGTH: fogStrength = scriptEng.operands[i]; break;
#endif
                    case VAR_VERTEXBUFFERX: vertexBuffer[arrayVal].x = scriptEng.operands[i]; break;
                    case VAR_VERTEXBUFFERY: vertexBuffer[arrayVal].y = scriptEng.operands[i]; break;
                    case VAR_VERTEXBUFFERZ: vertexBuffer[arrayVal].z = scriptEng.operands[i]; break;
                    case VAR_VERTEXBUFFERU: vertexBuffer[arrayVal].u = scriptEng.operands[i]; break;
                    case VAR_VERTEXBUFFERV: vertexBuffer[arrayVal].v = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERA: faceBuffer[arrayVal].a = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERB: faceBuffer[arrayVal].b = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERC: faceBuffer[arrayVal].c = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERD: faceBuffer[arrayVal].d = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERFLAG: faceBuffer[arrayVal].flag = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERCOLOR: faceBuffer[arrayVal].color = scriptEng.operands[i]; break;
                    case VAR_SAVERAM: saveRAM[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_ENGINESTATE: Engine.gameMode = scriptEng.operands[i]; break;
#if RETRO_REV00
                    case VAR_ENGINEMESSAGE: break;
#endif
                    case VAR_ENGINELANGUAGE: Engine.language = scriptEng.operands[i]; break;
                    case VAR_ENGINEONLINEACTIVE: Engine.onlineActive = scriptEng.operands[i]; break;
                    case VAR_ENGINESFXVOLUME:
                        sfxVolume = scriptEng.operands[i];
                        SetGameVolumes(bgmVolume, sfxVolume);
                        break;
                    case VAR_ENGINEBGMVOLUME:
                        bgmVolume = scriptEng.operands[i];
                        SetGameVolumes(bgmVolume, sfxVolume);
                        break;
#if RETRO_REV00
                    case VAR_ENGINEPLATFORMID: break;
#endif
                    case VAR_ENGINETRIALMODE: Engine.trialMode = scriptEng.operands[i]; break;
#if !RETRO_REV00
                    case VAR_ENGINEDEVICETYPE: break;
#endif

#if RETRO_REV03
                    // Origins Extras
                    // Due to using regular v4, these don't support array values like origins expects, so its always screen[0]
                    case VAR_SCREENCURRENTID: break;
                    case VAR_CAMERAENABLED:
                        if (arrayVal == 0)
                            cameraEnabled = scriptEng.operands[i];
                        break;
                    case VAR_CAMERATARGET:
                        if (arrayVal == 0)
                            cameraTarget = scriptEng.operands[i];
                        break;
                    case VAR_CAMERASTYLE:
                        if (arrayVal == 0)
                            cameraStyle = scriptEng.operands[i];
                        break;
                    case VAR_CAMERAXPOS:
                        if (arrayVal == 0)
                            cameraXPos = scriptEng.operands[i];
                        break;
                    case VAR_CAMERAYPOS:
                        if (arrayVal == 0)
                            cameraYPos = scriptEng.operands[i];
                        break;
                    case VAR_CAMERAADJUSTY:
                        if (arrayVal == 0)
                            cameraAdjustY = scriptEng.operands[i];
                        break;
#endif

#if RETRO_USE_HAPTICS
                    case VAR_HAPTICSENABLED: Engine.hapticsEnabled = scriptEng.operands[i]; break;
#endif
                }
            }
            else if (opcodeType == SCRIPTVAR_INTCONST) { // int constant
                scriptCodePtr++;
            }
            else if (opcodeType == SCRIPTVAR_STRCONST) { // string constant
                int strLen = scriptCode[scriptCodePtr++];
                for (int c = 0; c < strLen; ++c) {
                    switch (c % 4) {
                        case 0: break;
                        case 1: break;
                        case 2: break;
                        case 3: ++scriptCodePtr; break;
                        default: break;
                    }
                }
                scriptCodePtr++;
            }
        }
    }
}
