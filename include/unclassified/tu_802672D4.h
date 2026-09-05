#ifndef UNCLASSIFIED_TU_802672D4_H
#define UNCLASSIFIED_TU_802672D4_H

#include <RVLFaceLib/RFL_DataUtility.h>

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;
struct TU802384AC;

class TU802672D4Scene : public BaseSceneHandler
{
public:
    TU802672D4Scene();
    virtual ~TU802672D4Scene();
    void fn_8026748C();
    void fn_802677C4(int, void*);
    bool fn_80267A84();
    void fn_80267D84();
    void fn_80267DDC();
    void fn_80267DE0();
    void fn_80267E20();
    void fn_80267E40(unsigned short* name);
    void fn_80268ED0(int index, void* context);
    void fn_80268F94(int index, void* context);
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    /* 0x1C */ unsigned char mPadding1C[4];
    /* 0x20 */ int mUnidentified20[4];
    /* 0x30 */ bool mUnidentified30;
    /* 0x31 */ bool mUnidentified31;
    /* 0x32 */ unsigned char mPadding32[2];
    /* 0x34 */ RFLAdditionalInfo mUnidentified34;
    /* 0x70 */ int mUnidentified70;
    /* 0x74 */ int mUnidentified74;
    /* 0x78 */ unsigned long long mUnidentified78;
    /* 0x80 */ int mUnidentified80;
    /* 0x84 */ unsigned short mUnidentified84[0x40];
    /* 0x104 */ unsigned short mUnidentified104[0x40];
    /* 0x184 */ unsigned short mUnidentified184[0x40];
    /* 0x204 */ TU80219248Component mComponents[2];
    /* 0x36C */ TU8022EF84Component mNavigation;
    /* 0x444 */ TU802384AC* mUnidentified444;
    /* 0x448 */ TLComponentInstance* mUnidentified448[2];
}; // size 0x450

#endif // UNCLASSIFIED_TU_802672D4_H
