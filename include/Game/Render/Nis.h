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
    NIS_TRIGGER_TYPE_RUMBLE = 4,
    NIS_TRIGGER_TYPE_CROWD_EXCITEMENT = 5,
    NIS_TRIGGER_TYPE_STADIUM_EFFECTS = 6,
    NIS_TRIGGER_TYPE_CHARACTER_DIRT = 7,
    NIS_TRIGGER_TYPE_DEPTH_OF_FIELD = 8,
    NIS_TRIGGER_TYPE_SHOW_ELECTRIC_FENCE = 9,
    NIS_TRIGGER_TYPE_HIDE_ELECTRIC_FENCE = 10,
};

enum NisTarget
{
    NIS_TARGET_NONE = 0,
    NIS_TARGET_STADIUM = 1,
    NIS_TARGET_HOME_CAPTAIN = 2,
    NIS_TARGET_AWAY_CAPTAIN = 3,
    NIS_TARGET_HOME_SIDEKICK = 4,
    NIS_TARGET_HOME_SIDEKICK_1 = 5,
    NIS_TARGET_HOME_SIDEKICK_2 = 6,
    NIS_TARGET_HOME_SIDEKICK_3 = 7,
    NIS_TARGET_AWAY_SIDEKICK = 8,
    NIS_TARGET_AWAY_SIDEKICK_1 = 9,
    NIS_TARGET_AWAY_SIDEKICK_2 = 10,
    NIS_TARGET_AWAY_SIDEKICK_3 = 11,
    NIS_TARGET_HOME_GOALIE = 12,
    NIS_TARGET_AWAY_GOALIE = 13,
    NIS_TARGET_SCORER = 14,
    NIS_TARGET_LOSER_CAPTAIN = 15,
    NIS_TARGET_WINNER_CAPTAIN = 16,
    NIS_TARGET_LOSER_SIDEKICK = 17,
    NIS_TARGET_WINNER_SIDEKICK = 18,
    NIS_TARGET_LOSER_GOALIE = 19,
    NIS_TARGET_WINNER_GOALIE = 20,
    NIS_TARGET_MEGASTRIKE_CAPTAIN = 21,
    NIS_TARGET_MEGASTRIKE_DEFENDING_GOALIE = 22,
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
    /* 0x0A4 */ int numAnimProxies;
    /* 0x0A8 */ char animProxyNames[8][16];
    /* 0x128 */ nlVector2 animProxyPositions[8];
    /* 0x168 */ u16 animProxyDirections[8];
    /* 0x178 */ NisTarget target;
    /* 0x17C */ NisWinnerType winnerType;
    /* 0x180 */ u32 renderMode;
    /* 0x184 */ nlVector3 stadiumOffset;
    /* 0x190 */ float mTime;
    /* 0x194 */ bool mUnidentified194;
    /* 0x195 */ bool mirrored;
    /* 0x196 */ u8 unknown_0x196[0x02];
    /* 0x198 */ char* buffer;
    /* 0x19C */ u32 bufferSize;
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
        void FireEffect(const Nis& nis) const;
    };

    struct PendingAnimation
    {
        /* 0x00 */ const char* name;
        /* 0x04 */ int characterIndex;
        /* 0x08 */ bool loaded;
        /* 0x09 */ u8 unknown_0x09[0x03];
        /* 0x0C */ unsigned int loadHandle;
        /* 0x10 */ void* data;
        /* 0x14 */ unsigned long size;
        /* 0x18 */ void* request;
    };

    Nis(NisHeader& header, char* data, int size);
    virtual ~Nis();
    virtual void DoFunctionCall(unsigned int function);

    char* Name() const;
    void StartScript();
    void ApplyLoadedAnimations();
    void Update(float dt);
    void UpdateTriggers(float oldTime, float newTime, float duration);
    void SelectCamera(cAnimCamera& camera, int cameraIndex);
    void SelectRandomCamera(cAnimCamera& camera);
    void Render(int param1);
    nlVector3 Offset() const;
    void AddTrigger(NisTriggerType triggerType, float frameNumber,
        const char* name, const char* target, TriggerParams* trigParams);
    bool GetMainCharacterHeadPosition(nlVector3& position) const;
    int TargetToIndex(NisTarget target, NisWinnerType winnerType, bool findAvailableSidekick) const;
    void AttachHeadImpostors();
    ImpostorModel* AttachImpostorToCharacter(eCharacterClass characterClass, const char* impostorName,
        const char* textureName, DrawableCharacter** outCharacter);
    ImpostorModel* FindImpostor(const char* name);
    void ApplyDamageEffects(glModel* model, DrawableCharacter* character);
    bool IsLoading();
    void LoadCharacterAnimation(const char* animName, int characterIndex);
    void PlayAnimProxy(const char* animName, const char* proxyName,
        NisTarget target, NisWinnerType winnerType, bool force);

    /* 0x028 */ NisHeader* mHeader;
    /* 0x02C */ NisTarget mTarget;
    /* 0x030 */ NisWinnerType mWinnerType;
    /* 0x034 */ int mRenderMode;
    /* 0x038 */ char* mData;
    /* 0x03C */ int mSize;
    /* 0x040 */ int mBallId[MAX_NUM_CHARACTERS];
    /* 0x068 */ cPN_SAnimController* mCharacterControllers[MAX_NUM_CHARACTERS];
    /* 0x090 */ int mCharacterAnimProxy[MAX_NUM_CHARACTERS];
    /* 0x0B8 */ nlVector2 mAnimProxyPositions[8];
    /* 0x0F8 */ u16 mAnimProxyDirections[8];
    /* 0x108 */ bool mMirrored;
    /* 0x10C */ ImpostorModel* mImpostors[8];
    /* 0x12C */ const char* mImpostorNames[8];
    /* 0x14C */ char mImpostorSuffixes[8];
    /* 0x154 */ cAnimCamera* mCamera;
    /* 0x158 */ cCameraData* mCameraData[10];
    /* 0x180 */ int mNumCameras;
    /* 0x184 */ int mNumTriggers;
    /* 0x188 */ Trigger mTriggers[MAX_NUM_TRIGGERS];
    /* 0x848 */ int mMainCharacterIndex;
    /* 0x84C */ int mAudioCharacterIndex;
    /* 0x850 */ u32 mUnidentified850;
    /* 0x854 */ ImpostorModel* mDryBonesHead;
    /* 0x858 */ DrawableCharacter* mDryBonesHeadCharacter;
    /* 0x85C */ ImpostorModel* mShyGuyMask;
    /* 0x860 */ DrawableCharacter* mShyGuyMaskCharacter;
    /* 0x864 */ PendingAnimation mPendingAnimations[MAX_NUM_CHARACTERS];
    /* 0x97C */ cInventory<cSAnim> mAnimInventories[20];
    /* 0xBAC */ bool mScriptStarted;
};

template <typename T>
class SlotPool;

struct PendingAnimationRequest
{
    Nis::PendingAnimation* animation;
    bool active;
};

extern GLView* g_pNisRenderView;
extern SlotPool<PendingAnimationRequest> g_PendingAnimationRequestPool;

void ClearNisAnimatedCharacters();

#endif // GAME_RENDER_NIS_H
