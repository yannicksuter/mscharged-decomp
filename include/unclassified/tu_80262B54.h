#ifndef UNCLASSIFIED_TU_80262B54_H
#define UNCLASSIFIED_TU_80262B54_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class UnidentifiedScene_8025AF0C : public BaseSceneHandler
{
public:
    UnidentifiedScene_8025AF0C();
    virtual ~UnidentifiedScene_8025AF0C();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_8026388C();

    /* 0x01C */ int mUnidentified1C;
    /* 0x020 */ float mUnidentified20;
    /* 0x024 */ TU8022EF84Component mUnidentified24;
    /* 0x0FC */ bool mUnidentifiedFC;
    /* 0x0FD */ bool mUnidentifiedFD;
    /* 0x0FE */ u8 mPaddingFE[2];
    /* 0x100 */ float mUnidentified100;
    /* 0x104 */ int mUnidentified104;
    /* 0x108 */ TLComponentInstance* mUnidentified108;
}; // size 0x10C

#endif // UNCLASSIFIED_TU_80262B54_H
