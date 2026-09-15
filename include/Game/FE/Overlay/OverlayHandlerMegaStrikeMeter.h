#ifndef GAME_FE_OVERLAY_HANDLER_MEGA_STRIKE_METER_H
#define GAME_FE_OVERLAY_HANDLER_MEGA_STRIKE_METER_H

#include "Game/FE/BaseOverlayHandler.h"

class cFielder;
class nlVector3;
class TLComponentInstance;

class MegaStrikeMeterOverlay : public BaseOverlayHandler
{
public:
    MegaStrikeMeterOverlay();
    virtual ~MegaStrikeMeterOverlay();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void SetFirstResult(float meterValue);
    void SetSecondResult(float meterValue);
    void Start(cFielder* fielder);
    void SetPosition(nlVector3 position);

    /* 0x28 */ TLComponentInstance* mNumbers;
    /* 0x2C */ TLComponentInstance* mBackground;
    /* 0x30 */ cFielder* mFielder;
    /* 0x34 */ bool mRestoreVisibility;
    /* 0x35 */ bool mAlignLeft;
    /* 0x36 */ bool mMegaStrikeStarted;
}; // size 0x38

#endif // GAME_FE_OVERLAY_HANDLER_MEGA_STRIKE_METER_H
