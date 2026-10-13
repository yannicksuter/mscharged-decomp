#ifndef GAME_SH_SH_LOADING_H
#define GAME_SH_SH_LOADING_H

#include "Game/BaseSceneHandler.h"

class TLComponentInstance;
class TLTextInstance;
struct CharacterInfo;

class SuperLoadingScene : public BaseSceneHandler
{
public:
    enum TransitionType
    {
        TT_INVALID = -1,
        TT_IN = 0,
        TT_OUT = 1,
        TT_3D_TRANSITION = 2,
    };

    SuperLoadingScene();
    virtual ~SuperLoadingScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    /* 0x1C */ TransitionType mType;
}; // size 0x20

class BaseLoadingScene : public BaseSceneHandler
{
public:
    BaseLoadingScene();
    virtual ~BaseLoadingScene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void OnHomeButtonPressed();

    /* 0x1C */ TLComponentInstance* mTransitionComponent;
    /* 0x20 */ bool mTransitionActive;
    /* 0x21 */ bool mWidescreen;
    /* 0x22 */ unsigned char mPadding22[2];
}; // size 0x24

class MatchLoadingScene : public BaseLoadingScene
{
public:
    MatchLoadingScene();
    virtual ~MatchLoadingScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void DisplayOnlineInfo();
    void DisplayStadiumName(TLTextInstance* stadiumText);
    void DisplayFriendlyInfo();
    void DisplayCupInfo();
    void DisplayChallengeInfo();
    void SetTeamLogo(int side, CharacterInfo character);

    /* 0x024 */ TLTextInstance* mTextInstances[6];
    /* 0x03C */ unsigned short mTitleBuffer[128];
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    /* 0x13C */ unsigned short mStatsBuffers[4][128];
#else
    /* 0x13C */ unsigned short mStatsBuffers[4][32];
#endif
}; // size 0x53C (USA, Japan, Korea), 0x23C (Europe)

#endif // GAME_SH_SH_LOADING_H
