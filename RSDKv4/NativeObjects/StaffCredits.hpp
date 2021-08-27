#ifndef NATIVE_STAFFCREDITS_H
#define NATIVE_STAFFCREDITS_H

struct NativeEntity_StaffCredits : NativeEntityBase {
    int state;
    int field_14;
    float field_18;
    int field_1C;
    float scale;
    NativeEntityBase *optionsMenu;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *meshPanel;
    MatrixF renderMatrix;
    MatrixF matrix2;
    byte textureArrows;
    byte useRenderMatrix;
    int alpha;
    NativeEntity_CreditText *creditText[16];
    int creditsTextID;
    int latestTextID;
};

void StaffCredits_Create(void *objPtr);
void StaffCredits_Main(void *objPtr);

#endif // !NATIVE_STAFFCREDITS_H
