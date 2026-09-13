#ifndef GAME_FE_OVERLAY_HANDLER_DEFENSIVE_PLAY_H
#define GAME_FE_OVERLAY_HANDLER_DEFENSIVE_PLAY_H

#include "Game/FE/BaseOverlayHandler.h"

class cPlayer;

class DefensivePlayOverlay : public BaseOverlayHandler
{
public:
    DefensivePlayOverlay();
    virtual ~DefensivePlayOverlay();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void StartCountdown();

    /* 0x25 */ bool mCountdownStarted;
    /* 0x26 */ bool mCountdownComplete;
    /* 0x27 */ u8 mPadding27;
    /* 0x28 */ int mPlayerIndex;
    /* 0x2C */ int mCountdownSoundCount;
    /* 0x30 */ float mCountdownSoundTimer;
    /* 0x34 */ float mCountdownSoundInterval;
    /* 0x38 */ cPlayer* mGoalie;
    /* 0x3C */ int mCountdownSpeed;
}; // size 0x40

#endif // GAME_FE_OVERLAY_HANDLER_DEFENSIVE_PLAY_H
