#ifndef GAME_RENDER_NIS_H
#define GAME_RENDER_NIS_H

#include "Game/SAnim/pnSAnimController.h"
#include "NL/nlMath.h"
#include "types.h"

class GLView;
class cAnimCamera;

enum NisTriggerType
{
    NIS_TRIGGER_TYPE_EFFECT = 0,
    NIS_TRIGGER_TYPE_TIME_DILATION = 1,
    NIS_TRIGGER_TYPE_PLAY_SOUND = 2,
    NIS_TRIGGER_TYPE_PLAY_RANDOM_DIALOGUE = 3,
    NIS_TRIGGER_TYPE_STOP_SOUND = 4,
    NIS_TRIGGER_TYPE_PLAY_STREAM = 5,
    NIS_TRIGGER_TYPE_STOP_STREAM = 6,
    NIS_TRIGGER_TYPE_STOP_ALL_STREAMS = 7,
    NIS_TRIGGER_TYPE_SET_ACTIVE_STREAM_LOOPING = 8,
    NIS_TRIGGER_TYPE_REGISTER_GOAL_AUDIO = 9,
    NIS_TRIGGER_TYPE_RAISE_EVENT = 10,
};

enum NisTarget
{
    NIS_TARGET_NONE = 0,
    NIS_TARGET_STADIUM = 1,
    NIS_TARGET_HOME_CAPTAIN = 2,
    NIS_TARGET_AWAY_CAPTAIN = 3,
    NIS_TARGET_HOME_SIDEKICK = 4,
    NIS_TARGET_AWAY_SIDEKICK = 5,
    NIS_TARGET_HOME_GOALIE = 6,
    NIS_TARGET_AWAY_GOALIE = 7,
    NIS_TARGET_LOSER_CAPTAIN = 8,
    NIS_TARGET_WINNER_CAPTAIN = 9,
    NIS_TARGET_LOSER_SIDEKICK = 10,
    NIS_TARGET_WINNER_SIDEKICK = 11,
    NIS_TARGET_LOSER_GOALIE = 12,
    NIS_TARGET_WINNER_GOALIE = 13,
    NIS_NUM_TARGETS = 14,
};

enum NisWinnerType
{
    NIS_GAME_WINNER = 0,
    NIS_GOAL_WINNER = 1,
    NIS_NUM_WINNER_TYPES = 2,
    NIS_DO_NOT_CARE = 3,
};

struct NisHeader
{
    /* 0x000 */ char name[0x178];
    /* 0x178 */ NisTarget target;
    /* 0x17C */ NisWinnerType winnerType;
    /* 0x180 */ u32 unknown_0x180;
    /* 0x184 */ nlVector3 stadiumOffset;
    /* 0x190 */ u8 unknown_0x190[0x08];
    /* 0x198 */ u32 unknown_0x198;
};

class Nis
{
public:
    static const int MAX_NUM_TRIGGERS = 48;
    static const int MAX_NUM_CHARACTERS = 10;

    struct TriggerParams
    {
        /* 0x00 */ float float1;
        /* 0x04 */ unsigned long param1;
        /* 0x08 */ unsigned long param2;
        /* 0x0C */ unsigned long param3;
        /* 0x10 */ unsigned long param4;
    };

    struct Trigger
    {
        /* 0x00 */ NisTriggerType type;
        /* 0x04 */ float frameNumber;
        /* 0x08 */ const char* name;
        /* 0x0C */ const char* target;
        /* 0x10 */ TriggerParams params;

        void Fire(Nis& nis) const;
    };

    Nis(NisHeader& header, char* data, int size);
    virtual ~Nis();

    char* Name() const;
    void Update(float dt);
    void UpdateTriggers(float oldTime, float newTime, float duration);
    nlVector3 Offset() const;
    void AddTrigger(NisTriggerType triggerType, float frameNumber,
        const char* name, const char* target, TriggerParams* trigParams);

    /* 0x004 */ u8 unknown_0x004[0x24];
    /* 0x028 */ NisHeader* mHeader;
    /* 0x02C */ NisTarget mTarget;
    /* 0x030 */ NisWinnerType mWinnerType;
    /* 0x034 */ u32 unknown_0x034;
    /* 0x038 */ char* mData;
    /* 0x03C */ int mSize;
    /* 0x040 */ int mBallId[MAX_NUM_CHARACTERS];
    /* 0x068 */ cPN_SAnimController* mCharacterControllers[MAX_NUM_CHARACTERS];
    /* 0x090 */ u8 unknown_0x090[0x7C];
    /* 0x10C */ cAnimCamera* mCameras[8];
    /* 0x12C */ u8 unknown_0x12C[0x54];
    /* 0x180 */ int mNumCameras;
    /* 0x184 */ int mNumTriggers;
    /* 0x188 */ Trigger mTriggers[MAX_NUM_TRIGGERS];
    /* 0x848 */ u8 unknown_0x848[0x368];
};


extern GLView* g_pNisRenderView;

#endif // GAME_RENDER_NIS_H
