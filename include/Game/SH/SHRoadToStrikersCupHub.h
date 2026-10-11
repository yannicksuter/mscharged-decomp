#ifndef GAME_SH_SHROADTOSTRIKERSCUPHUB_H
#define GAME_SH_SHROADTOSTRIKERSCUPHUB_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;
class TLImageInstance;
class TLTextInstance;

enum eRoadToStrikersCupPhase
{
    ROAD_TO_STRIKERS_CUP_ENTERING = 0,
    ROAD_TO_STRIKERS_CUP_ACTIVE = 1,
    ROAD_TO_STRIKERS_CUP_TRANSITIONING = 2,
};

class RoadToStrikersCupHubScene : public BaseSceneHandler
{
public:
    enum HubButton
    {
        ButtonPlay = 0,
        ButtonSchedule = 1,
        ButtonCupStats = 2,
        ButtonRules = 3,
    };

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

#if defined(VERSION_R4QP01_REV1) || defined(VERSION_R4QP01_REV2)
    /* 0x01C */ u16 mCupRecordText[32];
#else
    /* 0x01C */ u16 mCupRecordText[128];
#endif
    // Member offsets: USA / EU.
    /* 0x11C / 0x05C */ u16 mCupHeadingText[64];
    /* 0x19C / 0x0DC */ u16 mCupStatusText[64];
    /* 0x21C / 0x15C */ bool mPointerButtonsInitialized;
    /* 0x21D / 0x15D */ bool mButtonPressed;
    /* 0x21E / 0x15E */ bool mUpdateDisabled;
    /* 0x21F / 0x15F */ bool mIntroAudioPlayed;
    /* 0x220 / 0x160 */ int mPointerHoverCounts[4];
    /* 0x230 / 0x170 */ int mSelectedButton;
    /* 0x234 / 0x174 */ FEBackButton mBackButton;
    /* 0x30C / 0x24C */ FEPointerButton mPlayButton;
    /* 0x3C0 / 0x300 */ FEPointerButton mScheduleButton;
    /* 0x474 / 0x3B4 */ FEPointerButton mCupStatsButton;
    /* 0x528 / 0x468 */ FEPointerButton mRulesButton;
    /* 0x5DC / 0x51C */ TLComponentInstance* mPlayButtonInstance;
    /* 0x5E0 / 0x520 */ TLComponentInstance* mScheduleButtonInstance;
    /* 0x5E4 / 0x524 */ TLComponentInstance* mCupStatsButtonInstance;
    /* 0x5E8 / 0x528 */ TLComponentInstance* mRulesButtonInstance;
    /* 0x5EC / 0x52C */ int mTransitionState;
}; // size: USA 0x5F0, EU 0x530

#endif // GAME_SH_SHROADTOSTRIKERSCUPHUB_H
