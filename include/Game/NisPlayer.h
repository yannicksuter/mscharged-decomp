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
    void SetExtraNameFilter(const char* filter);
    bool WorldIsFrozen() const;
    static NisPlayer* Instance();

    static NisPlayer* sInstance;

    /* 0x00028 */ u32 unknown_0x00028;
    /* 0x0002C */ bool mActive;
    /* 0x0002D */ u8 unknown_0x0002D[0x3400F];
    /* 0x3403C */ Nis* mPlaying[8];
    /* 0x3405C */ Nis* mLoaded[8];
    /* 0x3407C */ NisHeader* mLoadQueue[8];
    /* 0x3409C */ bool mAsyncStarted[8];
    /* 0x340A4 */ u8 unknown_0x340A4[0x210];
    /* 0x342B4 */ char mExtraNameFilter[128];
    /* 0x34334 */ u8 unknown_0x34334[0x138];
};

#endif // GAME_NIS_PLAYER_H
