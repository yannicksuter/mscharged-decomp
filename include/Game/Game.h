#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "Game/GameEventQueue.h"
#include "Game/NetworkMessage.h"
#include "types.h"
#include "NL/nlMath.h"

class Clock;
class FuzzyTweaks;
class DebugWriteCache;
class WeatherManager;
class RunningChecksum;
class nlPolygonRegion;
class Terrain;
class CrowdRiot;
class AvoidablePolygon;
class UnidentifiedFielderInput;
class cFielder;
class cPlayer;

struct UnidentifiedCircularByteQueue
{
    UnidentifiedCircularByteQueue(bool* data, u32 start, int size, u32 capacity)
    {
        mStart = start;
        mSize = size;
        mCapacity = capacity;
        mData = data;
    }

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

extern "C" void fn_80061B1C(int relative, float xTilt, float yTilt);

class cGame : public NetworkMessageReceiver
{
    friend void fn_80061B1C(int relative, float xTilt, float yTilt);
public:
    virtual int ProcessMessage(NetworkMessage* message);
    virtual ~cGame();

    cGame(void* param1, int param2, bool param3);
    void fn_80061AF0();
    void fn_80061AF4();

    void fn_80057FC0();
    void fn_80057FD8(bool param1);
    void fn_80058180();
    void fn_8005830C();
    void fn_80058400();
    void fn_8005848C();
    void BeginGame(bool bRematch, bool bStraightToKickoff);
    void OnSuddenDeath();
    void OnGameOver();
    void SendPauseGameEvent();
    void SendResumingGameEvent();
    void fn_80058498(bool param1, int param2, int param3);
    void fn_80058528(float timeScale, float transitionTime);
    float GetNormalizedGameTime();
    float GetGameTime();
    float GetGameDuration() const { return m_fGameDuration; }
    void fn_800586C0();
    void fn_80058704();
    void fn_80058748();
    void fn_80058A78(float seconds);
    void BlowUpPowerups(
        const nlPolygonRegion& region,
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
    void fn_8005A8FC(float fDeltaT);
    void fn_8005B840(void* checksum, DebugWriteCache* cache);
    void ChangeGameState(int state);
    void InitGameState(int state);
    void LoadTerrain(int terrain);
    void SetDifficulty(int diff0, int diff1, int diff2, bool param4);
    void fn_8005DB44(int param1, bool param2);
    void fn_8005DF38();
    void fn_8005E130(NISData* pData);

    inline bool IsGameplayOrOvertime()
    {
        return (m_eGameState == 5 || m_eGameState == 6);
    }

    inline int GetGameState() const { return m_eGameState; }
    inline u32 GetMegaStrikeSaveMask() const { return mUnidentified038; }

    /* 0x04 */ FuzzyTweaks* m_pFuzzyTweaks;
    /* 0x08 */ Clock* m_pGameClock;
    /* 0x0C */ Clock* m_pPostResetClock;
    /* 0x10 */ Clock* m_pPostGameDoneClock;

private:
    /* 0x14 */ UnidentifiedFielderInput* mUnidentified014;

public:
    /* 0x18 */ int m_eGameState;
    /* 0x1C */ float m_fGameDuration;
    /* 0x20 */ bool mUnidentified020;

private:
    /* 0x21 */ u8 mUnidentified021[0x03];

public:
    /* 0x24 */ int m_nLastTeamToScore;

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
    /* 0x40 */ bool mbCaptainShotToScoreOn;
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
    /* 0x7C */ float mUnidentified07C;

public:
    /* 0x80 */ float mUnidentified080;
    /* 0x84 */ float mUnidentified084;

private:
    /* 0x88 */ float mUnidentified088;
    /* 0x8C */ float mUnidentified08C;
    /* 0x90 */ float mUnidentified090;
    /* 0x94 */ float mUnidentified094;
    /* 0x98 */ float mUnidentified098;
    /* 0x9C */ float mUnidentified09C;

public:
    /* 0xA0 */ float mUnidentified0A0;

private:
    /* 0xA4 */ u16 mUnidentified0A4;
    /* 0xA6 */ u16 mUnidentified0A6;
    /* 0xA8 */ u32 mUnidentified0A8;
    /* 0xAC */ nlVector3 mTiltDirection;
    /* 0xB8 */ u32 mUnidentified0B8;

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
    void QueueChainNisEnd(ShotAtGoalData* data);
    void fn_80060BFC(CollisionBulletBillData& data);

    /* 0x49C */ UnidentifiedGameEventQueue mUnidentified49C;

public:
    /* 0x10D8 */ Terrain* mpTerrain;
    /* 0x10DC */ WeatherManager* mpWeatherManager;
    /* 0x10E0 */ CrowdRiot* mUnidentified10E0;
    /* 0x10E4 */ AvoidablePolygon* mUnidentified10E4[4];
};

extern cGame* g_pGame;

extern "C" void fn_8005D210(cGame*, LightningStrikeData*);

#endif // GAME_GAME_H
