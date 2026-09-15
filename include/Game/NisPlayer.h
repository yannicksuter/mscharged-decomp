#ifndef GAME_NIS_PLAYER_H
#define GAME_NIS_PLAYER_H

#include "Game/InterpreterCore.h"
#include "Game/Camera/animcam.h"
#include "Game/Render/Nis.h"
#include "types.h"

class NisPlayerOverlay;
class nlFile;
class cPlayer;
struct glModel;
struct glModelPacket;
struct GoalScoredData;
struct GoalieSaveData;

enum NisUseFilter
{
    NIS_NO_FILTER = 0,
    NIS_FILTER = 1,
};

class NisPlayer : public InterpreterCore
{
public:
    NisPlayer();
    virtual ~NisPlayer();
    virtual void DoFunctionCall(unsigned int);

    void Load(char* buffer, unsigned int size, NisHeader& nisHeader);
    void Load(const char* nisType, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisUseFilter useFilter, NisWinnerType winnerType, int param5, int param6);
    void fn_8028041C(const char* param1, const char* param2, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisWinnerType winnerType, bool param5, int param6);
    void LoadTriggers(Nis& nis);
    const char* GetTargetFilter(NisTarget target, NisWinnerType winnerType) const;
    void fn_8027F4B0(NisTarget target, NisWinnerType winnerType);

    bool HasUnidentifiedPacket(const glModelPacket* packet) const
    {
        for (int i = 0; i < 10; i++)
        {
            if (mUnidentified34440[i] == packet)
            {
                return true;
            }
        }
        return false;
    }

    void AddUnidentifiedPacket(glModelPacket* packet)
    {
        for (int i = 0; i < 10; i++)
        {
            if (mUnidentified34440[i] == NULL)
            {
                mUnidentified34440[i] = packet;
                return;
            }
        }
    }

    void HandleAsyncs();
    void Update(float deltaT);
    void fn_8027CA44();
    bool fn_8027CB44();
    void fn_802805B4(NisHeader& nisHeader, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisWinnerType winnerType, int param5, bool param6);
    void fn_8027B880(char* data);
    void fn_8027B630(char* data, unsigned long size);
    void fn_8027B758(char* data, unsigned long size);
    void ResetEffects();
    void fn_8027BD60();
    void fn_8027BD64();
    void fn_8027ED70();
    void fn_8027EDCC(unsigned long param1);
    void Reset();
    void Play();
    void fn_8027DA28();
    void fn_8027E054();
    void fn_8027ED08();
    void fn_8027CCEC();
    void fn_8027D11C();
    void fn_8027DF70(GoalScoredData* goalScoredData);
    void fn_8027D994();
    void fn_8027DFE0(GoalieSaveData*);
    void fn_8027D1EC();
    void fn_8027DFE4(cPlayer* param1);
    void fn_8027E5D4();
    void SetExtraNameFilter(const char* filter);
    bool WorldIsFrozen() const;
    bool fn_8027E64C() const;
    unsigned int IsPIPOverlayMode() const
    {
        return mUnidentified34338 == 0;
    }
    float TimeLeft() const;
    float fn_8027C064(int param1) const;
    cAnimCamera* fn_8027E708();
    void fn_8027EE38();
    void Render(int pass) const;
    void HideAllActors() const;
    void fn_8027EEA0(float param1);
    void fn_8027EE60(bool param1);
    void fn_8027EEF0();
    void fn_8027EF8C();
    void fn_8027E5D0();
    void fn_8027E714();
    void fn_8027ED18();
    int fn_8027E284(NisWinnerType winnerType) const;
    bool IsMirrored(NisTarget target, const char* name, NisWinnerType winnerType) const;
    bool fn_8027E0AC();
    bool fn_8027E0AC(const char* name) const;
    static NisPlayer* Instance();
    static void AsyncLoad(nlFile* file, void* buffer, unsigned int size, unsigned long param);

    static NisPlayer* sInstance;

    /* 0x00028 */ int unknown_0x00028;
    /* 0x0002C */ bool mActive;
    /* 0x00030 */ int mDictSize;
    /* 0x00034 */ NisHeader mDict[512];
    /* 0x34034 */ char* mMemory;
    /* 0x34038 */ int mMaxNumBallsVisible;
    /* 0x3403C */ Nis* mPlaying[8];
    /* 0x3405C */ Nis* mLoaded[8];
    /* 0x3407C */ NisHeader* mLoadQueue[8];
    /* 0x3409C */ bool mAsyncStarted[8];
    /* 0x340A4 */ char** mUnidentified340A4;
    /* 0x340A8 */ int mUnidentified340A8;
    /* 0x340AC */ char** mUnidentified340AC;
    /* 0x340B0 */ int mUnidentified340B0;
    /* 0x340B4 */ bool mLoadingFromBack;
    /* 0x340B8 */ int mUsedFromFront;
    /* 0x340BC */ int mUsedFromBack;
    /* 0x340C0 */ int mGoalScorerCharIndex;
    /* 0x340C4 */ cAnimCamera mCamera[2];
    /* 0x3422C */ Nis* mNisForTriggerLoading;
    /* 0x34230 */ int mWinnerSide[NIS_NUM_WINNER_TYPES];
    /* 0x34238 */ int mUnidentified34238;
    /* 0x3423C */ nlVector3 mBeginPositions[10];
    /* 0x342B4 */ char mExtraNameFilter[128];
    /* 0x34334 */ void* mUnidentified34334;
    /* 0x34338 */ int mUnidentified34338;
    /* 0x3433C */ NisPlayerOverlay* mUnidentified3433C[5];
    /* 0x34350 */ unsigned long mUnidentified34350;
    /* 0x34354 */ unsigned long mUnidentified34354;
    /* 0x34358 */ bool mUnidentified34358;
    /* 0x34359 */ bool mUnidentified34359;
    /* 0x3435A */ bool mUnidentified3435A;
    /* 0x3435C */ nlMatrix4 mUnidentified3435C;
    /* 0x3439C */ bool mUnidentified3439C;
    /* 0x3439D */ u8 unknown_0x3439D[0x43];
    /* 0x343E0 */ bool mUnidentified343E0;
    /* 0x343E4 */ float mUnidentified343E4;
    /* 0x343E8 */ float mUnidentified343E8;
    /* 0x343EC */ float mUnidentified343EC;
    /* 0x343F0 */ float mUnidentified343F0;
    /* 0x343F4 */ int mUnidentified343F4;
    /* 0x343F8 */ char mUnidentified343F8[64];
    /* 0x34438 */ unsigned long mUnidentified34438;
    /* 0x3443C */ unsigned long mUnidentified3443C;
    /* 0x34440 */ glModelPacket* mUnidentified34440[10];
    /* 0x34468 */ float mUnidentified34468;
};

#endif // GAME_NIS_PLAYER_H
