#ifndef NATIVE_STAFFCREDITS_H
#define NATIVE_STAFFCREDITS_H

#define StaffCredits_CreditsCount (0x10)

enum StaffCreditsStates { STAFFCREDITS_STATE_ENTER, STAFFCREDITS_STATE_SCROLL, STAFFCREDITS_STATE_EXIT };

struct NativeEntity_StaffCredits : NativeEntityBase {
    StaffCreditsStates state;
    int unused1;
    float timer;
    int unused2;
    float scale;
    NativeEntityBase *optionsMenu;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *meshPanel;
    MatrixF renderMatrix;
    MatrixF matrix2;
    byte textureArrows;
    byte useRenderMatrix;
    int alpha;
    NativeEntity_CreditText *creditText[StaffCredits_CreditsCount];
    int creditsTextID;
    byte latestTextID;
};

void StaffCredits_Create(void *objPtr);
void StaffCredits_Main(void *objPtr);

#endif // !NATIVE_STAFFCREDITS_H
