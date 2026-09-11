#ifndef GAME_RENDER_NIS_H
#define GAME_RENDER_NIS_H

#include "Game/Character.h"
#include "Game/InterpreterCore.h"
#include "Game/Inventory.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/nlMath.h"
#include "types.h"

class GLView;
class cAnimCamera;
class cCameraData;
class DrawableCharacter;
class ImpostorModel;

enum NisTriggerType
{
    NIS_TRIGGER_TYPE_EFFECT = 0,
    NIS_TRIGGER_TYPE_TIME_DILATION = 1,
    NIS_TRIGGER_TYPE_RAISE_EVENT = 2,
    NIS_TRIGGER_TYPE_PLAY_SOUND = 3,
    NIS_TRIGGER_TYPE_UNIDENTIFIED_4 = 4,
    NIS_TRIGGER_TYPE_UNIDENTIFIED_5 = 5,
    NIS_TRIGGER_TYPE_UNIDENTIFIED_6 = 6,
    NIS_TRIGGER_TYPE_UNIDENTIFIED_7 = 7,
    NIS_TRIGGER_TYPE_UNIDENTIFIED_8 = 8,
    NIS_TRIGGER_TYPE_UNIDENTIFIED_9 = 9,
    NIS_TRIGGER_TYPE_UNIDENTIFIED_10 = 10,
};

enum NisTarget
{
    NIS_TARGET_NONE = 0,
    NIS_TARGET_STADIUM = 1,
    NIS_TARGET_HOME_CAPTAIN = 2,
    NIS_TARGET_AWAY_CAPTAIN = 3,
    NIS_TARGET_HOME_SIDEKICK = 4,
    NIS_TARGET_UNIDENTIFIED_5 = 5,
    NIS_TARGET_UNIDENTIFIED_6 = 6,
    NIS_TARGET_UNIDENTIFIED_7 = 7,
    NIS_TARGET_AWAY_SIDEKICK = 8,
    NIS_TARGET_UNIDENTIFIED_9 = 9,
    NIS_TARGET_UNIDENTIFIED_10 = 10,
    NIS_TARGET_UNIDENTIFIED_11 = 11,
    NIS_TARGET_HOME_GOALIE = 12,
    NIS_TARGET_AWAY_GOALIE = 13,
    NIS_TARGET_UNIDENTIFIED_14 = 14,
    NIS_TARGET_LOSER_CAPTAIN = 15,
    NIS_TARGET_WINNER_CAPTAIN = 16,
    NIS_TARGET_LOSER_SIDEKICK = 17,
    NIS_TARGET_WINNER_SIDEKICK = 18,
    NIS_TARGET_LOSER_GOALIE = 19,
    NIS_TARGET_WINNER_GOALIE = 20,
    NIS_TARGET_UNIDENTIFIED_21 = 21,
    NIS_TARGET_UNIDENTIFIED_22 = 22,
    NIS_NUM_TARGETS = 23,
};

enum NisWinnerType
{
    NIS_GAME_WINNER = 0,
    NIS_GOAL_WINNER = 1,
    NIS_NUM_WINNER_TYPES = 2,
    NIS_DO_NOT_CARE = 3,
};

enum NisUseStadiumOffset
{
    NIS_NO_STADIUM_OFFSET = 0,
    NIS_AWAY_STADIUM_OFFSET = 1,
    NIS_HOME_STADIUM_OFFSET = 2,
};

struct NisHeader
{
    /* 0x000 */ char name[64];
    /* 0x040 */ int size;
    /* 0x044 */ int numBalls;
    /* 0x048 */ int numAnimations;
    /* 0x04C */ int numCameras;
    /* 0x050 */ nlVector3 center;
    /* 0x05C */ nlVector3 minBounds;
    /* 0x068 */ nlVector3 maxBounds;
    /* 0x074 */ nlVector3 beginPositions[4];
    /* 0x0A4 */ int mUnidentified0A4;
    /* 0x0A8 */ char mUnidentified0A8[8][16];
    /* 0x128 */ nlVector2 mUnidentified128[8];
    /* 0x168 */ u16 mUnidentified168[8];
    /* 0x178 */ NisTarget target;
    /* 0x17C */ NisWinnerType winnerType;
    /* 0x180 */ u32 unknown_0x180;
    /* 0x184 */ nlVector3 stadiumOffset;
    /* 0x190 */ float mTime;
    /* 0x194 */ bool mUnidentified194;
    /* 0x195 */ bool mUnidentified195;
    /* 0x196 */ u8 unknown_0x196[0x02];
    /* 0x198 */ char* unknown_0x198;
    /* 0x19C */ u32 unknown_0x19C;
};

class Nis : public InterpreterCore
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
        void FireEffect(Nis& nis) const;
    };

    struct Unidentified864
    {
        /* 0x00 */ const char* mUnidentified00;
        /* 0x04 */ int mUnidentified04;
        /* 0x08 */ bool mUnidentified08;
        /* 0x09 */ u8 unknown_0x09[0x03];
        /* 0x0C */ unsigned int mUnidentified0C;
        /* 0x10 */ void* mUnidentified10;
        /* 0x14 */ unsigned long mUnidentified14;
        /* 0x18 */ void* mUnidentified18;
    };

    Nis(NisHeader& header, char* data, int size);
    virtual ~Nis();
    virtual void DoFunctionCall(unsigned int param1);

    char* Name() const;
    void fn_802815E0();
    void fn_802816CC();
    void Update(float dt);
    void UpdateTriggers(float oldTime, float newTime, float duration);
    void fn_80281C70(cAnimCamera& camera);
    void Render(int param1);
    nlVector3 Offset() const;
    void AddTrigger(NisTriggerType triggerType, float frameNumber,
        const char* name, const char* target, TriggerParams* trigParams);
    bool fn_80282474(nlVector3& param1);
    int fn_80282DD8(NisTarget target, NisWinnerType winnerType, bool param3);
    void fn_802834A0();
    ImpostorModel* fn_8028350C(eCharacterClass param1, const char* param2,
        const char* param3, DrawableCharacter** param4);
    ImpostorModel* fn_80283884(const char* name);
    void fn_80283670(glModel* model, DrawableCharacter* character);
    bool fn_80283930();
    void fn_80283A40(const char* param1, int param2);
    void fn_80283200(const char* param1, const char* param2,
        NisTarget param3, NisWinnerType param4, bool param5);

    /* 0x028 */ NisHeader* mHeader;
    /* 0x02C */ NisTarget mTarget;
    /* 0x030 */ NisWinnerType mWinnerType;
    /* 0x034 */ u32 unknown_0x034;
    /* 0x038 */ char* mData;
    /* 0x03C */ int mSize;
    /* 0x040 */ int mBallId[MAX_NUM_CHARACTERS];
    /* 0x068 */ cPN_SAnimController* mCharacterControllers[MAX_NUM_CHARACTERS];
    /* 0x090 */ int mUnidentified090[MAX_NUM_CHARACTERS];
    /* 0x0B8 */ nlVector2 mUnidentified0B8[8];
    /* 0x0F8 */ u16 mUnidentified0F8[8];
    /* 0x108 */ bool mMirrored;
    /* 0x10C */ ImpostorModel* mUnidentified10C[8];
    /* 0x12C */ const char* mUnidentified12C[8];
    /* 0x14C */ char mUnidentified14C[8];
    /* 0x154 */ cAnimCamera* mCamera;
    /* 0x158 */ cCameraData* mUnidentified158[10];
    /* 0x180 */ int mNumCameras;
    /* 0x184 */ int mNumTriggers;
    /* 0x188 */ Trigger mTriggers[MAX_NUM_TRIGGERS];
    /* 0x848 */ int mMainCharacterIndex;
    /* 0x84C */ int mAudioCharacterIndex;
    /* 0x850 */ u32 mUnidentified850;
    /* 0x854 */ ImpostorModel* mUnidentified854;
    /* 0x858 */ DrawableCharacter* mUnidentified858;
    /* 0x85C */ ImpostorModel* mUnidentified85C;
    /* 0x860 */ DrawableCharacter* mUnidentified860;
    /* 0x864 */ Unidentified864 mUnidentified864[MAX_NUM_CHARACTERS];
    /* 0x97C */ cInventory<cSAnim> mUnidentified97C[20];
    /* 0xBAC */ bool mUnidentifiedBAC;
};


extern GLView* g_pNisRenderView;

#endif // GAME_RENDER_NIS_H
