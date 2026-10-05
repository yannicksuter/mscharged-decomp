#ifndef GAME_REPLAY_CHOREO_H
#define GAME_REPLAY_CHOREO_H

#include "Game/EventDataTypes.h"
#include "Game/Camera/ReplayCamera.h"
#include "Game/Camera/noisefilter.h"
#include "Game/InterpreterCore.h"
#include "Game/ReplayManager.h"
#include "NL/nlList.h"

class cPlayer;
struct GoalScoredData;
struct GoalieSaveData;

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

    struct ReplayShotData
    {
        /* 0x00 */ unsigned int uTeamIndex : 8;
        /* 0x00 */ unsigned int uGoalType : 16;
        /* 0x00 */ unsigned int mUnidentified003 : 8;
        /* 0x04 */ nlVector3 v3ShotPosition;
        /* 0x10 */ cPlayer* pScorer;
    }; // size: 0x14

    struct Highlight
    {
        Highlight()
            : mQuality(HIGHLIGHT_QUALITY_EMPTY)
            , mTime(0.0f)
            , mReplayPad(-1)
            , mSlot(-1)
            , mBegin(0)
            , mEnd(0)
            , mCurrent(0)
        {
        }

        /* 0x00 */ int mQuality;
        /* 0x04 */ float mTime;
        /* 0x08 */ int mReplayPad;
        /* 0x0C */ ReplayShotData mGoalScoredData;
        /* 0x20 */ int mSlot;
        /* 0x24 */ Replay::Frame* mBegin;
        /* 0x28 */ Replay::Frame* mEnd;
        /* 0x2C */ Replay::Frame* mCurrent;
        /* 0x30 */ Highlight* next;
    }; // total size: 0x34

    ReplayChoreo();

    virtual void DoFunctionCall(unsigned int);

    static ReplayChoreo& Instance();
    void LoadScript();
    void RegisterEventHandlers();
    void OnGoalScored(GoalScoredData* data);
    void OnGoalieSave(GoalieSaveData* data);
    void Reset();
    void StartScript(const ReplayShotData& data);
    void Finish();
    void FlushHighlights();
    void Update(float deltaT);
    bool Done(float param) const;
    void SaveHighlight(int quality);
    int NumHighlights() const;
    int GetHighlightNumber() const;
    void StartAutoReplay(bool highlight);
    void LoadNextHighlight();

    /* 0x028 */ int mNumScripts[3][3][8];
    /* 0x148 */ char scriptName[0x40];
    /* 0x188 */ mutable ReplayManager* mReplayManager;
    /* 0x18C */ mutable Replay* mReplay;
    /* 0x190 */ ReplayCamera mCamera;
    /* 0x290 */ cRumbleFilter mRumbleFilter;
    /* 0x2CC */ cNoiseFilter mNoiseFilter;
    /* 0x32C */ float mRunForTimeLeft;
    /* 0x330 */ bool mRunningFor;
    /* 0x331 */ u8 mPadding331[3];
    /* 0x334 */ void* mByteCode;
    /* 0x338 */ bool mIsHighlightReel;
    /* 0x339 */ u8 mPadding339[3];
    /* 0x33C */ mutable ReplayShotData mGoalScoredData;
    /* 0x350 */ Highlight mHighlights[3];
    /* 0x3EC */ nlList<Highlight> mHighlightList;
    /* 0x3F4 */ Highlight* mCurrentHighlight;
}; // total size: 0x3F8

#endif // GAME_REPLAY_CHOREO_H
