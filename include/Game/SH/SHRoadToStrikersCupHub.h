#ifndef GAME_SH_SHROADTOSTRIKERSCUPHUB_H
#define GAME_SH_SHROADTOSTRIKERSCUPHUB_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;
class TLImageInstance;
class TLTextInstance;

class RoadToStrikersCupHubScene : public BaseSceneHandler
{
public:
    RoadToStrikersCupHubScene();
    virtual ~RoadToStrikersCupHubScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void SetTeamLogo(TLImageInstance* image, int captain);
    void UpdateCupRecordText(TLTextInstance* text);
    void UpdateCupStatus();
    void UpdateRoundMessage();
    void UpdateCupHeading();
    void InitializePointerButtons();
    void OnButtonPointerEnter(unsigned int index, void* context);
    void OnButtonPointerLeave(unsigned int index, void* context);
    void OnButtonPointerPress(unsigned int index, void* context);

    /* 0x01C */ u16 mCupRecordText[128];
    /* 0x11C */ u16 mCupHeadingText[64];
    /* 0x19C */ u16 mCupStatusText[64];
    /* 0x21C */ bool mPointerButtonsInitialized;
    /* 0x21D */ bool mButtonPressed;
    /* 0x21E */ bool mUpdateDisabled;
    /* 0x21F */ bool mIntroAudioPlayed;
    /* 0x220 */ int mPointerHoverCounts[4];
    /* 0x230 */ int mSelectedButton;
    /* 0x234 */ FEBackButton mBackButton;
    /* 0x30C */ FEPointerButton mPlayButton;
    /* 0x3C0 */ FEPointerButton mScheduleButton;
    /* 0x474 */ FEPointerButton mCupStatsButton;
    /* 0x528 */ FEPointerButton mRulesButton;
    /* 0x5DC */ TLComponentInstance* mPlayButtonInstance;
    /* 0x5E0 */ TLComponentInstance* mScheduleButtonInstance;
    /* 0x5E4 */ TLComponentInstance* mCupStatsButtonInstance;
    /* 0x5E8 */ TLComponentInstance* mRulesButtonInstance;
    /* 0x5EC */ int mTransitionState;
}; // size 0x5F0

#endif // GAME_SH_SHROADTOSTRIKERSCUPHUB_H
