#ifndef GAME_OVERLAY_MANAGER_H
#define GAME_OVERLAY_MANAGER_H

#include "Game/BaseGameSceneManager.h"

struct UnidentifiedEventData_8006701C;
struct GoalScoredData;
struct MegaStrikeEndData;
class InGameTextOverlay;
class nlVector3;

class OverlayManager : public BaseGameSceneManager
{
public:
    OverlayManager();
    virtual ~OverlayManager();
    virtual BaseSceneHandler* Push(SceneList scene, ScreenMovement movement, bool popfirst);
    virtual void Pop();

    void ShowDemoSlide();
    void Update(float deltaTime);
    void SetVisible(SceneList scene, bool visibility, bool overrideStateSettings);
    void HandleStateTransition(u32 from, u32 to);
    void fn_801E1514();
    void fn_801E2498(float delay);
    void fn_801E258C();
    void fn_801E2590();
    void fn_801E2608();
    void fn_801E2784(UnidentifiedEventData_8006701C* eventData);
    void fn_801E281C();
    void fn_801E28A8(UnidentifiedEventData_8006701C* eventData);
    void fn_801E28E4(UnidentifiedEventData_8006701C* eventData);
    void fn_801E2920();
    void fn_801E2988(MegaStrikeEndData* eventData);
    void fn_801E29C0(nlVector3 position);
    void fn_801E2A28(GoalScoredData* eventData);

    /* 0x108 */ InGameTextOverlay* mInGameTextOverlay;
    /* 0x10C */ bool mIsHUDSlideIn;
    /* 0x10D */ bool mDoHUDSlideIn;
    /* 0x10E */ u8 mUnidentified10E;
    /* 0x10F */ bool mIsDemoSlideVisible;
    /* 0x110 */ float mHUDDelay;
    /* 0x114 */ u32 mUnidentified114;
    /* 0x118 */ u32 mUnidentified118;
    /* 0x11C */ u32 mUnidentified11C;
    /* 0x120 */ u32 mUnidentified120;

private:
    void SlideHUDOut();
}; // size 0x124

#endif // GAME_OVERLAY_MANAGER_H
