#ifndef UNCLASSIFIED_TU_80245F04_H
#define UNCLASSIFIED_TU_80245F04_H

#include "Game/SH/SHSceneBase.h"

class TU80245F04Scene : public BaseSceneHandler
{
public:
    TU80245F04Scene();
    virtual ~TU80245F04Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_802460D8();
    void fn_802466C0();
    void fn_8024671C();

    /* 0x01C */ u8 mUnidentified1C[0x24];
    /* 0x040 */ bool mUnidentified40;
    /* 0x041 */ u8 mUnidentified41[3];
    /* 0x044 */ int mUnidentified44;
    /* 0x048 */ UnidentifiedScrollWidget mUnidentified48;
    /* 0x1FC */ bool mUnidentified1FC;
    /* 0x1FD */ u8 mUnidentified1FD[0x503];
}; // size 0x700

#endif // UNCLASSIFIED_TU_80245F04_H
