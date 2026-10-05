#ifndef GAME_OVERLAY_MANAGER_H
#define GAME_OVERLAY_MANAGER_H

#include "Game/BaseGameSceneManager.h"
#include "Game/OverlayHandlerInGameText.h"

struct MegaStrikeMeterData;
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
    void SetCurrentTextOverlaySlide(OverlaySlideName slideName);

    void ShowDemoSlide();
    void RestartGoalOverlay();
    void Update(float deltaTime);
    void SetVisible(SceneList scene, bool visibility, bool overrideStateSettings);
    void HandleStateTransition(u32 from, u32 to);
    void RegisterEventHandlers();
    void SlideHUDIn(float delay);
    void OnGetReadyForKickoff();
    void OnKickoff();
    void OnGameOver();
    void OnMegaStrikeMeterStart(MegaStrikeMeterData* eventData);
    void OnMegaStrikeMeterEnd();
    void OnMegaStrikeMeterFirst(MegaStrikeMeterData* eventData);
    void OnMegaStrikeMeterSecond(MegaStrikeMeterData* eventData);
    void OnMegastrikeStart();
    void OnMegastrikeEnd(MegaStrikeEndData* eventData);
    void SetMegaStrikeMeterPosition(nlVector3 position);
    void OnGoalScored(GoalScoredData* eventData);

    void GetStrikerTimesVariants(int* story, int* headline, int* image)
    {
        *story = mStrikerTimesStoryVariant;
        *headline = mStrikerTimesHeadlineVariant;
        *image = mStrikerTimesImageVariant;
    }

    void SetStrikerTimesVariants(int story, int headline, int image)
    {
        mStrikerTimesStoryVariant = story;
        mStrikerTimesHeadlineVariant = headline;
        mStrikerTimesImageVariant = image;
    }

    void ResetStrikerTimesVariants();

    /* 0x108 */ InGameTextOverlay* mInGameTextOverlay;
    /* 0x10C */ bool mIsHUDSlideIn;
    /* 0x10D */ bool mDoHUDSlideIn;
    /* 0x10E */ bool mIsInHighlights;
    /* 0x10F */ bool mIsDemoSlideVisible;
    /* 0x110 */ float mHUDDelay;
    /* 0x114 */ int mStrikerTimesStoryVariant;
    /* 0x118 */ int mStrikerTimesHeadlineVariant;
    /* 0x11C */ int mStrikerTimesImageVariant;
    /* 0x120 */ u32 mUnidentified120;

private:
    void SlideHUDOut();
}; // size 0x124

#endif // GAME_OVERLAY_MANAGER_H
