#ifndef UNCLASSIFIED_TU_80210600_H
#define UNCLASSIFIED_TU_80210600_H

#include "Game/BaseSceneHandler.h"
#include "Game/SH/SHSceneBase.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class TU80210600Scene : public BaseSceneHandler
{
public:
    TU80210600Scene();
    virtual ~TU80210600Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_802107AC(int index, void* context);
    void fn_802108B0(int index, void* context);
    void fn_80210984(int index, void* context);
    void fn_80210B00(int index, void* context);
    void fn_80210B1C(int index, void* context);
    void fn_802119C8();
    bool fn_80211B18(int value0, int value1);
    void fn_80211FA4();

    /* 0x01C */ TLComponentInstance* mUnidentified1C[8];
    /* 0x03C */ int mUnidentified3C;
    /* 0x040 */ int mUnidentified40;
    /* 0x044 */ unsigned char mUnidentified44[0x1C0];
    /* 0x204 */ bool mUnidentified204;
    /* 0x205 */ bool mUnidentified205;
    /* 0x206 */ bool mUnidentified206;
    /* 0x207 */ bool mUnidentified207;
    /* 0x208 */ int mUnidentified208[4];
    /* 0x218 */ UnidentifiedScrollWidget mUnidentified218;
    /* 0x3CC */ TU8022EF84Component mUnidentified3CC;
    /* 0x4A4 */ TLComponentInstance* mUnidentified4A4;
    /* 0x4A8 */ TU80219248Component mUnidentified4A8;
    /* 0x55C */ TU80219248Component mUnidentified55C;
    /* 0x610 */ TU80219248Component mUnidentified610[5];
    /* 0x994 */ unsigned char mUnidentified994[0x14];
    /* 0x9A8 */ TLComponentInstance* mUnidentified9A8;
    /* 0x9AC */ TLComponentInstance* mUnidentified9AC;
    /* 0x9B0 */ int mUnidentified9B0;
}; // size 0x9B4

#endif // UNCLASSIFIED_TU_80210600_H
