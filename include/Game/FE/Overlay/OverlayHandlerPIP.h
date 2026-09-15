#ifndef GAME_FE_OVERLAY_HANDLER_PIP_H
#define GAME_FE_OVERLAY_HANDLER_PIP_H

#include "Game/FE/BaseOverlayHandler.h"

class PIPOverlay : public BaseOverlayHandler
{
public:
    PIPOverlay();
    virtual ~PIPOverlay();
    virtual void Update(float dt);
    virtual void SceneCreated();

    /* 0x25 */ bool mInactive;
    /* 0x28 */ float mTimeRemaining;
}; // size 0x2C

#endif // GAME_FE_OVERLAY_HANDLER_PIP_H
