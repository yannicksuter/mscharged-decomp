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

class NisPlayer : public InterpreterCore
{
public:
    NisPlayer();
    virtual ~NisPlayer();
    virtual void DoFunctionCall(unsigned int);

    void Load(char* buffer, unsigned int size, NisHeader& nisHeader);
    void LoadTriggers(Nis& nis);
    void fn_802805B4(NisHeader& nisHeader, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisWinnerType winnerType, int param5, bool param6);
    void fn_8027B880(char* data);
    void fn_8027B630(char* data, unsigned long size);
    void fn_8027B758(char* data, unsigned long size);
    void ResetEffects();
    void Reset();
    void fn_8027CCEC();
    void fn_8027D11C();
    void fn_8027D994();
    void fn_8027D1EC();
    void fn_8027DFE4(cPlayer* param1);
    void fn_8027E5D4();
    void SetExtraNameFilter(const char* filter);
    bool WorldIsFrozen() const;
    cAnimCamera* fn_8027E708();
    void Render(int pass);
    void fn_8027E5D0();
    void fn_8027E714();
    int fn_8027E284(NisWinnerType winnerType) const;
    bool IsMirrored(NisTarget target, const char* name, NisWinnerType winnerType) const;
    bool fn_8027E0AC();
    bool fn_8027E0AC(const char* name) const;
    static NisPlayer* Instance();
    static void AsyncLoad(nlFile* file, void* buffer, unsigned int size, unsigned long param);

    static NisPlayer* sInstance;

    /* 0x00028 */ u32 unknown_0x00028;
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
    /* 0x340C0 */ int mUnidentified340C0;
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
    /* 0x3435B */ u8 unknown_0x3435B[0x41];
    /* 0x3439C */ bool mUnidentified3439C;
    /* 0x3439D */ u8 unknown_0x3439D[0x43];
    /* 0x343E0 */ bool mUnidentified343E0;
    /* 0x343E4 */ float mUnidentified343E4;
    /* 0x343E8 */ float mUnidentified343E8;
    /* 0x343EC */ float mUnidentified343EC;
    /* 0x343F0 */ float mUnidentified343F0;
    /* 0x343F4 */ int mUnidentified343F4;
    /* 0x343F8 */ bool mUnidentified343F8;
    /* 0x343F9 */ u8 unknown_0x343F9[0x47];
    /* 0x34440 */ glModel* mUnidentified34440[10];
    /* 0x34468 */ float mUnidentified34468;
};

#endif // GAME_NIS_PLAYER_H
