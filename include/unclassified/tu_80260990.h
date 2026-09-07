#ifndef UNCLASSIFIED_TU_80260990_H
#define UNCLASSIFIED_TU_80260990_H

#include "Game/BaseSceneHandler.h"
#include "Game/NetworkSession.h"
#include "unclassified/tu_8022EF84.h"

class TU80260990Scene : public BaseSceneHandler, public UnidentifiedLoginListener
{
public:
    TU80260990Scene();
    virtual ~TU80260990Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    virtual void OnLoginResult(int result);
    virtual void OnStatsResult(bool success);

    void fn_80261710();

    /* 0x020 */ bool mUnidentified20;
    /* 0x024 */ TU8022EF84Component mUnidentified24;
    /* 0x0FC */ TLComponentInstance* mUnidentifiedFC;
    /* 0x100 */ int mUnidentified100;
    /* 0x104 */ float mUnidentified104;
    /* 0x108 */ float mUnidentified108;
}; // size 0x10C

extern "C" int fn_80261770(int error, bool connected, int value);

#endif // UNCLASSIFIED_TU_80260990_H
