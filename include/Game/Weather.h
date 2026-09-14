#ifndef GAME_WEATHER_H
#define GAME_WEATHER_H

#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"
#include "NL/nlTimer.h"

class DebugWriteCache;
class ThwompObject;

struct Weather
{
    Weather();
    ~Weather() { Stop(true); }

    virtual void Update(float);
    virtual void Start();
    virtual void Stop(bool);
    virtual float GetStartChance() { return 0.0f; }
    void RegisterDebugFields(u16* type, DebugWriteCache* cache);
    virtual void SyncLog(void*, DebugWriteCache*);
    virtual void Reset();

    s32 meWeather;
    s32 meState;
    bool mbPaused;
    u8 padding0D[3];
};

struct Lightning : Weather
{
    Lightning();
    virtual void Update(float);
    virtual void Start();
    virtual void Stop(bool);
    virtual float GetStartChance();
    void RegisterDebugFields(u16* type, DebugWriteCache* cache);
    virtual void SyncLog(void*, DebugWriteCache*);
    virtual void Reset();

    Timer mtLightingTimer;
    s32 mnAmountOfStrikes;
};

struct Windy : Weather
{
    Windy();
    nlVector3 GetDebrisVelocity(const nlVector3& direction);
    void ApplyWindForce(nlVector3* output = 0);
    nlVector3 CalculateDebrisDisplacement(const nlVector3&, const nlVector3&, float, float);
    void AvoidDebrisObstacle(nlVector3&, nlVector3&, nlVector3&, const nlVector3&, float);
    virtual void Update(float);
    virtual void Start();
    virtual void Stop(bool);
    virtual float GetStartChance();
    void RegisterDebugFields(u16* type, DebugWriteCache* cache);
    virtual void SyncLog(void*, DebugWriteCache*);
    virtual void Reset();

    void OnGetReadyForKickoff();

    Timer mtWindTimer;
    u16 aWindDirection;
    u8 padding1A[2];
    float fWindStrength;
    s32 eDebrisType;
    s32 eLastDebrisType;
};

struct SolarFlare : Weather
{
    SolarFlare();
    virtual void Update(float);
    virtual void Start();
    virtual void Stop(bool);
    virtual float GetStartChance();
    void RegisterDebugFields(u16* type, DebugWriteCache* cache);
    virtual void SyncLog(void*, DebugWriteCache*);
    virtual void Reset();
    void OnGetReadyForKickoff();
    void OnKickoff();
    int SelectVaporizeTargets();
    bool IsTargetSelected(int target, int count);
    int GetMaxVaporizeTargets();
    void ResetFlares(bool);

    float m_FlareTimer;
    float m_VaporizeTimer;
    float m_StartChance;
    s32 m_StartCount;
    s32 m_NumToVaporizePerTeam;
    s32 m_NextFlare;
    s32 m_NextVaporize;
    s32 m_TargetIndicies[3];
};

struct BubblingLava : Weather
{
    BubblingLava();
    virtual void Update(float);
    virtual void Start();
    virtual void Stop(bool);
    virtual float GetStartChance();
    void RegisterDebugFields(u16* type, DebugWriteCache* cache);
    virtual void SyncLog(void*, DebugWriteCache*);
    virtual void Reset();

    float CalculateLavaTrajectory(nlVector3&, nlVector3&, float);

    float m_StartChance;
    float m_VolleyCountdown;
    float m_FlySoundCountdown;
};

struct StormShipWeather : Weather
{
    StormShipWeather();
    virtual void Update(float);
    virtual void Start();
    virtual void Stop(bool);
    virtual float GetStartChance();
    void RegisterDebugFields(u16* type, DebugWriteCache* cache);
    virtual void SyncLog(void*, DebugWriteCache*);
    virtual void Reset();

    void ResetLightning(bool);
    void CreateChainLightning(int);

    float m_StartWeatherTimer;
    float m_FirstStrikeTimer;
    float m_FirstStrikeElectrocuteTimer;
    float m_ChainTrainTimer;
    float m_StartChainTrainTimer;
    float m_DarkenTimer;
    float m_StopChainLightningSound;
    bool m_bRightSide;
    u8 padding2D[3];
    s32 m_PathIndex1;
    s32 m_PathIndex2;
    s32 m_LoopCount;
    s32 m_ChainCount;
};

struct SandTombWeather : Weather
{
    SandTombWeather();
    void DropThwomp(int);
    virtual void Update(float);
    virtual void Start();
    virtual void Stop(bool);
    virtual float GetStartChance();
    void RegisterDebugFields(u16* type, DebugWriteCache* cache);
    virtual void SyncLog(void*, DebugWriteCache*);
    virtual void Reset();
    void OnKickoff();
    void SpawnThwomps();
    void CreateSandPatches();
    void InvalidateSandPatches();
    bool DropThwompNearPlayer();
    bool IsPlayerInDropZone(nlVector3 position);

    s32 m_NumActiveThwomps;
    float m_ThwompDropTimer;
    float m_ThwompSpawnTimer;
    bool m_bSandPatchesCreated;
    u8 padding1D[3];
    ThwompObject* m_Thwomps[8];
};

class WeatherManager
{
public:
    WeatherManager();
    ~WeatherManager();
    void Initialize(unsigned int type);
    void Clear();
    Weather* GetWeather(int value);
    void SyncLog(void* context, DebugWriteCache* cache);
    void Update(float value);
    void Stop(bool value);
    void Reset();
    void Pause();
    void Resume();

    nlDLListSlotPool<Weather*> mWeather;
};

#endif // GAME_WEATHER_H
