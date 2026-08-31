#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "Game/GameEventQueue.h"
#include "types.h"

class Clock;
class FuzzyTweaks;
class RunningChecksum;
class UnidentifiedGameRegion;
class cFielder;
class cPlayer;

struct UnidentifiedCircularByteQueue
{
    bool UnidentifiedRemoveStart()
    {
        bool* entry;
        if (mSize == 0)
        {
            entry = mData + (mStart & mCapacity);
        }
        else
        {
            --mSize;
            entry = mData + mStart;
            ++mStart;
            if (mStart % mCapacity == 0)
            {
                mStart = 0;
            }
        }
        return *entry;
    }

    /* 0x00 */ bool* mData;
    /* 0x04 */ u32 mStart;
    /* 0x08 */ int mSize;
    /* 0x0C */ u32 mCapacity;
};

void DestroyPowerups();
void DestroyGame();

class cGame
{
public:
    virtual void UnidentifiedVirtual();
    virtual ~cGame();

    void fn_80057FC0();
    void fn_80058180();
    void fn_8005830C();
    void fn_80058400();
    void fn_8005848C();
    void fn_80058498(bool param1, int param2, int param3);
    void fn_80058528(float timeScale, float transitionTime);
    float GetNormalizedGameTime();
    float GetGameTime();
    void fn_800586C0();
    void fn_80058704();
    void fn_80058A78(float seconds);
    void BlowUpPowerups(
        const UnidentifiedGameRegion& v3ExplosionPosition,
        float fExplosionRadius);
    void ResetPowerups(bool clearPowerUps);
    void fn_80059A1C();
    void fn_80059B70(void* param1);
    void fn_80059D80(u8 param1);
    void fn_80059DEC(int param1, int param2, float param3, float param4);
    void fn_80059E78();
    void fn_80059EDC();
    void fn_80059F40(u8 param1, u8 param2, float param3);
    void fn_80059FC4();
    void PreUpdate(float deltaTime);
    void RandomizePlayerUpdateOrder();
    void fn_8005B508();
    float fn_8005B748(int param1, int param2);
    cPlayer* fn_8005B780(int param1, int param2, int param3);
    void SetPotentialScorer(cPlayer* pPlayer);
    void fn_8005BF50(RunningChecksum* runningChecksum);
    void ChangeGameState(int state);
    void InitGameState(int state);
    void SetDifficulty(int diff0, int diff1, int diff2, bool param4);
    void fn_8005DB44(int param1, bool param2);
    void fn_8005DF38();

    inline bool IsGameplayOrOvertime()
    {
        return (m_eGameState == 5 || m_eGameState == 6);
    }

    /* 0x04 */ FuzzyTweaks* m_pFuzzyTweaks;
    /* 0x08 */ Clock* m_pGameClock;
    /* 0x0C */ Clock* m_pPostResetClock;
    /* 0x10 */ Clock* m_pPostGameDoneClock;

private:
    /* 0x14 */ u8 mUnidentified014[0x04];

public:
    /* 0x18 */ int m_eGameState;
    /* 0x1C */ float m_fGameDuration;
    /* 0x20 */ bool mUnidentified020;

private:
    /* 0x21 */ u8 mUnidentified021[0x03];

public:
    /* 0x24 */ u32 mUnidentified024;

private:
    /* 0x28 */ u32 mUnidentified028;
    /* 0x2C */ u32 mUnidentified02C;

public:
    /* 0x30 */ u32 mUnidentified030;

private:
    /* 0x34 */ int mUnidentified034;
    /* 0x38 */ u32 mUnidentified038;

public:
    /* 0x3C */ cFielder* mUnidentified03C;

private:
public:
    /* 0x40 */ bool mUnidentified040;
    /* 0x41 */ bool mUnidentified041;
    /* 0x42 */ bool mUnidentified042;

private:
    /* 0x43 */ u8 mUnidentified043;

public:
    /* 0x44 */ cPlayer* m_pScorer;
    /* 0x48 */ cPlayer* m_pAssister;
    /* 0x4C */ cPlayer* m_pTeamTouch[2];
    /* 0x54 */ cPlayer* m_pRandomPlayersArray[10];

private:
    /* 0x7C */ u8 mUnidentified07C[0x40];

public:
    /* 0xBC */ bool mUnidentified0BC;
    /* 0xBD */ bool mUnidentified0BD;

private:
    /* 0xBE */ u8 mUnidentified0BE[0x02];

public:
    /* 0xC0 */ UnidentifiedCircularByteQueue mUnidentified0C0;

private:
    /* 0xD0 */ u8 mUnidentified0D0[0x64];

public:
    /* 0x134 */ UnidentifiedCircularByteQueue mUnidentified134;

private:
    /* 0x144 */ u8 mUnidentified144[0x10];

public:
    /* 0x154 */ cPlayer* m_nClosestPlayers[10][2][5];
    /* 0x2E4 */ float m_fCachedPlayerDistances[10][10];
    /* 0x474 */ float m_fCachedBallPlayerDistances[10];

public:
    /* 0x49C */ UnidentifiedGameEventQueue mUnidentified49C;

public:
    /* 0x10D8 */ void* mUnidentified10D8;
    /* 0x10DC */ void* mUnidentified10DC;
    /* 0x10E0 */ void* mUnidentified10E0;
};

extern cGame* lbl_806E0C94;
#define g_pGame lbl_806E0C94

#endif // GAME_GAME_H
