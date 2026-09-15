#ifndef GAME_FE_OVERLAY_HANDLER_SUPER_ABILITY_H
#define GAME_FE_OVERLAY_HANDLER_SUPER_ABILITY_H

#include "Game/DB/StatsTracker.h"
#include "Game/FE/BaseOverlayHandler.h"
#include "Game/FE/feAsyncImage.h"

class TLComponentInstance;

extern eTeamID gSuperAbilityTeam;

class SuperAbilityOverlay : public BaseOverlayHandler
{
public:
    SuperAbilityOverlay();
    virtual ~SuperAbilityOverlay();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SetVisible(bool visible);

    void Start();

    /* 0x028 */ TLComponentInstance* mText;
    /* 0x02C */ AsyncImage mImage;
    /* 0x0CC */ bool mImageLoaded;
    /* 0x0D0 */ float mTimeRemaining;
}; // size 0xD4

#endif // GAME_FE_OVERLAY_HANDLER_SUPER_ABILITY_H
