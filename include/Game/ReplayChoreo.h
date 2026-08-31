#ifndef GAME_REPLAY_CHOREO_H
#define GAME_REPLAY_CHOREO_H

#include "Game/Camera/ReplayCamera.h"
#include "Game/Camera/rumblefilter.h"
#include "Game/InterpreterCore.h"
#include "Game/ReplayManager.h"

class cPlayer;

struct GoalScoredData
{
    /* 0x00 */ u32 mUnidentified000;
    /* 0x04 */ nlVector3 v3ShotPosition;
    /* 0x10 */ cPlayer* pScorer;
}; // total size: 0x14

class ReplayChoreo : public InterpreterCore
{
public:
    enum HighlightQuality
    {
        HIGHLIGHT_QUALITY_EMPTY = 0,
        HIGHLIGHT_QUALITY_SAVE = 1,
        HIGHLIGHT_QUALITY_GOAL_DECREASE_DIFF = 2,
        HIGHLIGHT_QUALITY_GOAL_EQUALIZER = 3,
        HIGHLIGHT_QUALITY_GOAL_INCREASE_DIFF = 4,
        NUM_QUALITY_LEVELS = 5,
    };

    struct Highlight
    {
        /* 0x00 */ int mUnidentified000;
        /* 0x04 */ float mTime;
        /* 0x08 */ int mReplayPad;
        /* 0x0C */ GoalScoredData mGoalScoredData;
        /* 0x20 */ int mUnidentified020;
        /* 0x24 */ void* mUnidentified024;
        /* 0x28 */ void* mUnidentified028;
        /* 0x2C */ void* mUnidentified02C;
        /* 0x30 */ Highlight* mUnidentified030;
    }; // total size: 0x34

    ReplayChoreo();

    virtual void DoFunctionCall(unsigned int);

    static ReplayChoreo& Instance();
    void LoadScript();
    void Reset();
    void Finish();
    void FlushHighlights();
    void Update(float deltaTime);
    bool Done(float param) const;
    void SaveHighlight(HighlightQuality quality);
    int NumHighlights() const;
    int fn_80195CBC() const;

    /* 0x028 */ int mNumScripts[3][3][8];
    /* 0x148 */ char scriptName[0x40];
    /* 0x188 */ mutable ReplayManager* mReplayManager;
    /* 0x18C */ mutable Replay* mReplay;
    /* 0x190 */ ReplayCamera mCamera;
    /* 0x290 */ cRumbleFilter mUnidentified290;
    /* 0x2CC */ u8 mUnidentifiedCameraFilter[0x60];
    /* 0x32C */ float mRunForTimeLeft;
    /* 0x330 */ bool mRunningFor;
    /* 0x331 */ u8 mPadding331[3];
    /* 0x334 */ void* mByteCode;
    /* 0x338 */ bool mUnidentified338;
    /* 0x339 */ u8 mPadding339[3];
    /* 0x33C */ GoalScoredData mGoalScoredData;
    /* 0x350 */ Highlight mHighlights[3];
    /* 0x3EC */ Highlight* mUnidentified3EC;
    /* 0x3F0 */ Highlight* mUnidentified3F0;
    /* 0x3F4 */ Highlight* mUnidentified3F4;
}; // total size: 0x3F8

#endif // GAME_REPLAY_CHOREO_H
