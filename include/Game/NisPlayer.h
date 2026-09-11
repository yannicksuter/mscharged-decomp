#ifndef GAME_NIS_PLAYER_H
#define GAME_NIS_PLAYER_H

#include "Game/InterpreterCore.h"
#include "Game/Render/Nis.h"
#include "types.h"

class NisPlayer : public InterpreterCore
{
public:
    NisPlayer();
    virtual ~NisPlayer();
    virtual void DoFunctionCall(unsigned int);

    void Load(char* buffer, unsigned int size, NisHeader& nisHeader);
    void LoadTriggers(Nis& nis);
    void ResetEffects();
    void Reset();
    void fn_8027D11C();
    void SetExtraNameFilter(const char* filter);
    bool WorldIsFrozen() const;
    void Render(int pass);
    void fn_8027E5D0();
    void fn_8027E714();
    int fn_8027E284(NisWinnerType winnerType);
    static NisPlayer* Instance();

    static NisPlayer* sInstance;

    /* 0x00028 */ u32 unknown_0x00028;
    /* 0x0002C */ bool mActive;
    /* 0x0002D */ u8 unknown_0x0002D[0x3400B];
    /* 0x34038 */ int mMaxNumBallsVisible;
    /* 0x3403C */ Nis* mPlaying[8];
    /* 0x3405C */ Nis* mLoaded[8];
    /* 0x3407C */ NisHeader* mLoadQueue[8];
    /* 0x3409C */ bool mAsyncStarted[8];
    /* 0x340A4 */ u8 unknown_0x340A4[0x1C];
    /* 0x340C0 */ int mUnidentified340C0;
    /* 0x340C4 */ u8 unknown_0x340C4[0x16C];
    /* 0x34230 */ int mUnidentified34230;
    /* 0x34234 */ u8 unknown_0x34234[0x4];
    /* 0x34238 */ int mUnidentified34238;
    /* 0x3423C */ u8 unknown_0x3423C[0x78];
    /* 0x342B4 */ char mExtraNameFilter[128];
    /* 0x34334 */ void* mUnidentified34334;
    /* 0x34338 */ int mUnidentified34338;
    /* 0x3433C */ u8 unknown_0x3433C[0xA4];
    /* 0x343E0 */ bool mUnidentified343E0;
    /* 0x343E4 */ float mUnidentified343E4;
    /* 0x343E8 */ u8 unknown_0x343E8[0x04];
    /* 0x343EC */ float mUnidentified343EC;
    /* 0x343F0 */ u8 unknown_0x343F0[0x7C];
};

#endif // GAME_NIS_PLAYER_H
