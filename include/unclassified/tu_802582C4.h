#ifndef UNCLASSIFIED_TU_802582C4_H
#define UNCLASSIFIED_TU_802582C4_H

#include "Game/BaseSceneHandler.h"
#include "Game/SH/SHSceneBase.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

struct TU802582C4SelectionState
{
    /* 0x0 */ int mUnidentified00;
    /* 0x4 */ int mUnidentified04;
    /* 0x8 */ int mUnidentified08;
};

class TU802582C4Scene : public BaseSceneHandler
{
public:
    TU802582C4Scene();
    virtual ~TU802582C4Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_802584CC(int index, void* context);
    void fn_80258604(int index, void* context);
    void fn_80258730(int item);
    void fn_80259330();
    void fn_802597E8(int index, void* context);
    void fn_80259924(int index, void* context);

    /* 0x01C */ TU80219248Component mComponents[5];
    /* 0x3A0 */ UnidentifiedScrollWidget mScrollWidget;
    /* 0x554 */ TLComponentInstance* mUnidentified554[5];
    /* 0x568 */ bool mUnidentified568;
    /* 0x569 */ unsigned char mPadding569[3];
    /* 0x56C */ TU8022EF84Component mNavigation;
    /* 0x644 */ int mUnidentified644[4];
    /* 0x654 */ unsigned char mUnidentified654;
    /* 0x655 */ unsigned char mPadding655[3];
    /* 0x658 */ int mUnidentified658;
    /* 0x65C */ int mUnidentified65C;
    /* 0x660 */ TU802582C4SelectionState* mUnidentified660;
    /* 0x664 */ int mUnidentified664;
}; // size 0x668

#endif // UNCLASSIFIED_TU_802582C4_H
