#ifndef UNCLASSIFIED_TU_8020E1B0_H
#define UNCLASSIFIED_TU_8020E1B0_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class TU8020E1B0Scene : public BaseSceneHandler
{
public:
    TU8020E1B0Scene();
    virtual ~TU8020E1B0Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_8020E300();
    void fn_8020E8FC(int index, void* context);
    void fn_8020E9C8(int index, void* context);
    void fn_802102C0(int index, void* context);
    void fn_80210364(int index, void* context);
    void fn_802103F0(int index, void* context);
    void fn_80210490(int index, void* context);

    /* 0x01C */ unsigned char mUnidentified1C[0xB0];
    /* 0x0CC */ bool mUnidentifiedCC;
    /* 0x0CD */ bool mUnidentifiedCD;
    /* 0x0CE */ bool mUnidentifiedCE;
    /* 0x0CF */ bool mUnidentifiedCF;
    /* 0x0D0 */ int mSelectionCounts[4];
    /* 0x0E0 */ TU8022EF84Component mNavigationComponent;
    /* 0x1B8 */ TU80219248Component mMatchupComponents[3];
    /* 0x3D4 */ unsigned char mPadding3D4[4];
    /* 0x3D8 */ TU80219248Component mBracketComponent;
    /* 0x48C */ TLComponentInstance* mBracketInstance;
    /* 0x490 */ TLComponentInstance* mMatchupInstances[3];
    /* 0x49C */ int mState;
}; // size 0x4A0

#endif // UNCLASSIFIED_TU_8020E1B0_H
