#ifndef UNCLASSIFIED_TU_802639C4_H
#define UNCLASSIFIED_TU_802639C4_H

#include "Game/BaseSceneHandler.h"

class TLComponentInstance;
class TLImageInstance;

class TU802639C4Scene : public BaseSceneHandler
{
public:
    TU802639C4Scene();
    virtual ~TU802639C4Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    virtual void fn_80264060();

    void fn_80263E1C();
    bool fn_80264120();

    /* 0x1C */ float mUnidentified1C;
    /* 0x20 */ float mUnidentified20;
    /* 0x24 */ bool mUnidentified24;
    /* 0x25 */ u8 mPadding25[3];
    /* 0x28 */ int mUnidentified28;
    /* 0x2C */ TLImageInstance* mUnidentified2C;
    /* 0x30 */ TLComponentInstance* mUnidentified30;
    /* 0x34 */ bool mUnidentified34;
    /* 0x35 */ bool mUnidentified35;
    /* 0x36 */ u8 mPadding36[2];
}; // size 0x38

#endif // UNCLASSIFIED_TU_802639C4_H
