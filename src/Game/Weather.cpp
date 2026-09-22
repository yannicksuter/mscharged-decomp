#include "Game/Render/tu_801B43F8.h"
#include "NL/nlFunction.inl"
#include "NL/utility.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"
#include "Game/Goalie.h"
#include "Game/AI/AiUtil.h"
#include "Game/Render/ThwompObject.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/EventRegistry.h"
#include "Game/MathHelpers.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Physics/PhysicsShockwave.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/tu_801B532C.h"
#include "math.h"
#include "Game/Render/tu_8027AE14.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Camera/GameplayCam.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Team.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/Ball.h"
#include "Game/CharacterTriggers.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/Sys/audio.h"
#include "NL/nlTimer.h"
#include "Game/DebugWriteCache.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlFunctionMemory.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlConfig.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Weather.h"
#include "types.h"

float gWindStrength = 5.0f;
float gWindDuration = 8.0f;
float gWindStartChance = 0.01f;
float gDebrisMaxTravelTime = 3.95f;
float gDebrisMinTravelTime = 3.95f;
float gDebrisChance = 1.0f;
float gWindAngleRange = 100.0f;
float gDebrisGoalieClearance = 5.0f;
float gDebrisGoalClearance = 0.75f;
float gDebrisCornerClearance = 0.75f;
float gDebrisSpeed0 = 7.5f;
float gDebrisSpeed1 = 13.5f;
float gDebrisSpeed2 = 12.5f;
float gLightningDarkness = 0.4f;
float gLightningFadeTime = 0.45f;
float gLightningStrikeDelay = 1.0f;
float gLightningStrikeRadius = 5.33f;
int gLightningStrikeCount = 3;
float gLightningStartChance = 0.033f;
float gSolarFlareDelay = 0.9f;
float gSolarFlareInterval = 0.5f;
float gSolarVaporizeDelay = 1.1f;
int gSolarMaxVaporizeTargets = 2;
float gSolarCameraZoom = 1.25f;
float gLavaStartChance = 50.0f;
float gLavaVolleyDelay = 2.5f;
int gLavaVolleyCount = 3;
float gLavaBallRadius = 1.25f;
float gLavaGravity = 1.45f;
float gLavaMinZSpeed = 4.0f;
float gLavaMaxZSpeed = 6.5f;
float gLavaMinXSpeed = 4.0f;
float gLavaMaxXSpeed = 8.0f;
float gLavaMinYSpeed = 4.0f;
float gLavaMaxYSpeed = 8.0f;
float gLavaPoolRadius = 3.0f;
float gLavaPoolWidth = 3.0f;
float gLavaPoolGrowthTime = 0.5f;
float gLavaPoolLifetime = 7.5f;
float gLavaGoalClearance = 11.0f;
float gLavaSoundLeadTime = 1.5f;
float gStormStrikeRadius = 5.0f;
float gStormStartDelay = 15.0f;
float gStormFirstStrikeDelay = 1.1f;
int gStormLoopCount = 3;
float gStormChainRadius = 0.45f;
float gStormChainSpeed = 12.0f;
float gStormElectrocuteDelay = 0.4f;
int gStormChainCount = 3;
float gStormChainDelay = 0.15f;
float gStormDarkness = 0.4f;
float gStormFadeTime = 0.8f;
float gStormDarkenDuration = 0.7f;
float gThwompMinSpawnDelay = 12.5f;
float gThwompSpawnDelayRange = 12.5f;
int gThwompMinCount = 1;
int gThwompMaxCount = 8;
float gThwompMinDropDelay = 12.7f;
float gThwompMaxDropDelay = 12.7f;
float gThwompTriggerRadius = 1.4f;
float gWindVariation;
float gWeakLightningRadius;
float gLavaYOverride;
bool gDisableWeather;
static u16 sWeatherSyncLogType = 0xFFFF;
static u16 sLightningSyncLogType = 0xFFFF;
static u16 sWindySyncLogType = 0xFFFF;
static u16 sSolarFlareSyncLogType = 0xFFFF;
static u16 sBubblingLavaSyncLogType = 0xFFFF;
static u16 sStormShipWeatherSyncLogType = 0xFFFF;
static u16 sSandTombWeatherSyncLogType = 0xFFFF;

extern "C" void fn_801BDC1C(const nlVector3&, const nlVector3&, const nlVector3&);
extern "C" void fn_80031A30(cFielder*, int, float);
extern "C" void fn_80097358(cPlayer*, float);
void OnLavaCollisionPatchGround(UnidentifiedEventData24*);
extern "C" void fn_800B0358();
extern "C" nlVector3* fn_800B0464(int);
extern "C" int fn_800B0478(int);
extern "C" nlVector3* fn_800B048C(int);
extern "C" int fn_800B04A0(int);
extern "C" int fn_800B045C();
extern "C" int fn_800B04B4(SandTombWeather*);
extern "C" nlVector4 fn_800B04BC(SandTombWeather*, int, bool);

inline bool SolarFlare::IsTargetSelected(int target, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (m_TargetIndicies[i] == target)
            return true;
    }
    return false;
}

inline bool SandTombWeather::IsPlayerInDropZone(nlVector3 position)
{
    float radiusSquared = gThwompTriggerRadius * gThwompTriggerRadius;
    for (int team = 0; team < 2; team++)
    {
        for (int player = 0; player < 4; player++)
        {
            nlVector3 delta;
            nlVec3Sub(delta, g_pTeams[team]->GetFielder(player)->mUnidentified024.m_v3Position, position);
            if (nlVec3LengthSquared(delta) < radiusSquared)
                return true;
        }
    }
    return false;
}

WeatherManager::WeatherManager()
{
}

WeatherManager::~WeatherManager()
{
    Clear();
    mWeather.m_Allocator.FreeBlocks();
}

void WeatherManager::Initialize(unsigned int type)
{
    Clear();
    switch (type)
    {
    case 3:
    {
        Weather* state = new (8, false) Lightning();
        mWeather.AddEnd(state);
        state = new (8, false) Windy();
        mWeather.AddEnd(state);
        break;
    }
    case 1:
    {
        Weather* state = new (8, false) Lightning();
        mWeather.AddEnd(state);
        break;
    }
    case 2:
    {
        Weather* state = new (8, false) Windy();
        mWeather.AddEnd(state);
        break;
    }
    case 4:
    {
        Weather* state = new (8, false) SolarFlare();
        mWeather.AddEnd(state);
        break;
    }
    case 5:
    {
        Weather* state = new (8, false) BubblingLava();
        mWeather.AddEnd(state);
        break;
    }
    case 6:
    {
        Weather* state = new (8, false) StormShipWeather();
        mWeather.AddEnd(state);
        break;
    }
    case 7:
    {
        Weather* state = new (8, false) SandTombWeather();
        mWeather.AddEnd(state);
        break;
    }
    default:
    {
        Weather* state = new (8, false) Weather();
        mWeather.AddEnd(state);
        break;
    }
    }
    gDisableWeather = Config::Global().Get<bool>("no_weather", false);
}

Weather* WeatherManager::GetWeather(int value)
{
    if (!mWeather.IsEmpty())
    {
        nlDLListIterator<Weather*> it = mWeather.Begin();
        while (it.hasNext())
        {
            Weather* state = *it;
            if (value == state->meWeather)
            {
                return state;
            }
            it.Step();
        }
    }
    return 0;
}

void WeatherManager::SyncLog(void* context, DebugWriteCache* cache)
{
    if (!mWeather.IsEmpty())
    {
        nlDLListIterator<Weather*> it = mWeather.Begin();
        while (it.hasNext())
        {
            (*it)->SyncLog(context, cache);
            it.next();
        }
    }
}

void WeatherManager::Update(float value)
{
    if (!mWeather.IsEmpty())
    {
        nlDLListIterator<Weather*> it = mWeather.Begin();
        while (it.hasNext())
        {
            (*it)->Update(value);
            it.next();
        }
    }
}

void WeatherManager::Clear()
{
    if (!mWeather.IsEmpty())
    {
        nlDLListIterator<Weather*> it = mWeather.Begin();
        while (it.hasNext())
        {
            Weather* state = *it;
            state->Stop(true);
            delete state;
            it.next();
        }
        mWeather.Clear();
    }
}

void WeatherManager::Stop(bool value)
{
    if (!mWeather.IsEmpty())
    {
        nlDLListIterator<Weather*> it = mWeather.Begin();
        while (it.hasNext())
        {
            (*it)->Stop(value);
            it.next();
        }
    }
}

void WeatherManager::Reset()
{
    if (!mWeather.IsEmpty())
    {
        nlDLListIterator<Weather*> it = mWeather.Begin();
        while (it.hasNext())
        {
            (*it)->Reset();
            it.next();
        }
    }
}

void WeatherManager::Pause()
{
    if (!mWeather.IsEmpty())
    {
        nlDLListIterator<Weather*> it = mWeather.Begin();
        while (it.hasNext())
        {
            (*it)->mbPaused = 1;
            it.next();
        }
    }
}

void WeatherManager::Resume()
{
    if (!mWeather.IsEmpty())
    {
        nlDLListIterator<Weather*> it = mWeather.Begin();
        while (it.hasNext())
        {
            (*it)->mbPaused = 0;
            it.next();
        }
    }
}

Weather::Weather()
    : meWeather(0)
    , meState(0)
    , mbPaused(0)
{
}

void Weather::Start()
{
    meState = 1;
}

void Weather::Update(float)
{
    if (gDisableWeather || GameInfoManager::Instance()->IsRule0x4Equal1())
    {
        return;
    }
    if (!g_pGame->IsGameplayOrOvertime() && g_pGame->m_eGameState != 1 && meState == 1)
    {
        Stop(false);
    }
    else if (meState != 1 && !mbPaused && meWeather != 0)
    {
        float value = nlRandomf(100.0f);
        if (value < GetStartChance())
        {
            Start();
        }
    }
}

int RandomWeatherIndex(int maximum)
{
    float value = maximum * nlRandomf(1.0f);
    value += value < 0.0f ? -0.5f : 0.5f;
    return (int)value;
}

void Weather::Stop(bool)
{
    meState = 0;
}

inline void Weather::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("Weather");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&meWeather - (u8*)this, "meWeather");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&meState - (u8*)this, "meState");
    cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&mbPaused - (u8*)this, "mbPaused");
    cache->EndType();
}

void Weather::SyncLog(void* context, DebugWriteCache* cache)
{
    if (sWeatherSyncLogType == 0xFFFF)
    {
        RegisterDebugFields(&sWeatherSyncLogType, cache);
    }
    cache->ChecksumData(sWeatherSyncLogType, this, context);
    cache->WriteData(sWeatherSyncLogType, this, sizeof(Weather));
}

void Weather::Reset()
{
    meState = 0;
    mbPaused = 0;
}

Lightning::Lightning()
{
    meWeather = 1;
    Reset();
}

float Lightning::GetStartChance()
{
    return gLightningStartChance;
}

void Lightning::Reset()
{
    Weather::Reset();
    mtLightingTimer.UnidentifiedClear();
    mnAmountOfStrikes = gLightningStrikeCount;
}

inline void Lightning::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("Lightning");
    cache->AddField(20, gDebugFieldTypes[20].size, 0, "mtLightingTimer");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&mnAmountOfStrikes - (u8*)&mtLightingTimer, "mnAmountOfStrikes");
    cache->EndType();
}

void Lightning::SyncLog(void* context, DebugWriteCache* cache)
{
    Weather::SyncLog(context, cache);
    if (sLightningSyncLogType == 0xFFFF)
    {
        RegisterDebugFields(&sLightningSyncLogType, cache);
    }
    cache->ChecksumData(sLightningSyncLogType, &mtLightingTimer, context);
    cache->WriteData(sLightningSyncLogType, &mtLightingTimer, sizeof(Lightning) - sizeof(Weather));
}

void Lightning::Start()
{
    Weather::Start();
    mnAmountOfStrikes = gLightningStrikeCount;
    mtLightingTimer.SetSeconds(0.9f * gLightningStrikeDelay + gLightningStrikeDelay * nlRandomf(0.1f));
    PlaySound(12, 0xD172BFEB, 0, 0);
    WorldDarkening::Instance().Fade(gLightningFadeTime, gLightningDarkness);
}

void Lightning::Update(float value)
{
    Weather::Update(value);
    if (mtLightingTimer.m_uPackedTime != 0 && mtLightingTimer.Countdown(value, 0.0f))
    {
        if (--mnAmountOfStrikes > 0)
        {
            mtLightingTimer.SetSeconds(0.9f * gLightningStrikeDelay + gLightningStrikeDelay * nlRandomf(0.1f));
            nlVector3 position = g_pBall->m_v3Position;
            position.x += nlRandomf(10.0f) - 5.0f;
            position.y += nlRandomf(20.0f) - 10.0f;
            position.z = 0.0f;
            cField::FixOutOfBoundsPosition(position, gWeakLightningRadius, true);
            LightningStrikeData* data = 0;
            g_LightningStrikeDataPool.Allocate(data);
            data->position = position;
            data->radius = gWeakLightningRadius;
            fn_8005D210(g_pGame, data);
            if (nlRandomf(1.0f) < 0.5f)
            {
                fn_801BDD24("weather_lightning_weak", position, false);
            }
            else
            {
                fn_801BDD24("weather_lightning_weak_left", position, false);
            }
            PlaySound(12, 0xC46BD58A, 0, 0);
        }
        else
        {
            LightningStrikeData* data = 0;
            g_LightningStrikeDataPool.Allocate(data);
            data->position = g_pBall->m_v3Position;
            data->position.z = 0.0f;
            cField::FixOutOfBoundsPosition(data->position, 0.5f, true);
            data->radius = gLightningStrikeRadius;
            fn_8005D210(g_pGame, data);
            if (nlRandomf(1.0f) < 0.5f)
            {
                fn_801BDD24("weather_lightning", data->position, true);
            }
            else
            {
                fn_801BDD24("weather_lightning_left", data->position, true);
            }
            WorldDarkening::Instance().Fade(0.5f * gLightningFadeTime, 0.0f);
            PlaySound(12, 0x900C09E3, 0, 0);
            Stop(false);
        }
    }
}

void Lightning::Stop(bool value)
{
    if (value)
    {
        WorldDarkening::Instance().fn_801AF550();
    }
    else
    {
        WorldDarkening::Instance().Fade(2.0f * gLightningFadeTime, 0.0f);
    }
    fn_801BDDE0(value);
    mtLightingTimer.UnidentifiedClear();
    mnAmountOfStrikes = 0;
    Weather::Stop(value);
}

Windy::Windy()
{
    meWeather = 2;
    Reset();
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GetReadyForKickoff", -1)->Add(Function<FnVoidVoid>(BindExp1<void, Detail::MemFunImpl<void, void (Windy::*)()>, Windy*>(MemFun(&Windy::OnGetReadyForKickoff), this)), 0, -1);
}

void Windy::OnGetReadyForKickoff()
{
    for (int i = 0; i < 3; i++)
    {
        gNPCManager->fn_801A9DE0(i)->fn_801B4B24(false);
    }
}

float Windy::GetStartChance()
{
    return gWindStartChance;
}

void Windy::Reset()
{
    Weather::Reset();
    mtWindTimer.UnidentifiedClear();
    aWindDirection = 0;
    fWindStrength = 0.0f;
    eDebrisType = -1;
    eLastDebrisType = -1;
}

inline void Windy::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("Windy");
    cache->AddField(20, gDebugFieldTypes[20].size, 0, "mtWindTimer");
    cache->AddField(19, gDebugFieldTypes[19].size, (u8*)&aWindDirection - (u8*)&mtWindTimer, "aWindDirection");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&fWindStrength - (u8*)&mtWindTimer, "fWindStrength");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&eDebrisType - (u8*)&mtWindTimer, "eDebrisType");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&eLastDebrisType - (u8*)&mtWindTimer, "eLastDebrisType");
    cache->EndType();
}

void Windy::SyncLog(void* context, DebugWriteCache* cache)
{
    Weather::SyncLog(context, cache);
    if (sWindySyncLogType == 0xFFFF)
    {
        RegisterDebugFields(&sWindySyncLogType, cache);
    }
    cache->ChecksumData(sWindySyncLogType, &mtWindTimer, context);
    cache->WriteData(sWindySyncLogType, &mtWindTimer, sizeof(Windy) - sizeof(Weather));
}

inline nlVector3 Windy::GetDebrisVelocity(const nlVector3& direction)
{
    float speed = 0.0f;
    switch (eDebrisType)
    {
    case 0:
        speed = gDebrisSpeed0;
        break;
    case 1:
        speed = gDebrisSpeed1;
        break;
    case 2:
        speed = gDebrisSpeed2;
        break;
    }
    nlVector3 velocity;
    nlVec3Scale(velocity, direction, nlRecipSqrt(nlVec3LengthSquared(direction), false));
    nlVec3Scale(velocity, speed);
    return velocity;
}

inline void Windy::ApplyWindForce(nlVector3* output)
{
    nlVector3 force;
    force.z = 0.0f;
    nlPolarToCartesian(force.x, force.y, aWindDirection, fWindStrength);
    nlVector3 windForce = { force.x, force.y, 0.0f };
    g_pBall->m_pPhysicsBall->mv3WindForce = windForce;
    if (output)
        *output = windForce;
}

void Windy::Start()
{
    Reset();
    float range = 65536.0f * (gWindAngleRange / 360.0f);
    aWindDirection = (u16)(s32)(nlRandomf(range) - range / 2.0f - 16384.0f);
    if (nlRandomf(65536.0f) < 32768.0f)
    {
        aWindDirection += 32768.0f;
    }
    fWindStrength = nlRandomf(gWindStrength * gWindVariation) + gWindStrength * (1.0f - gWindVariation);
    g_pBall->m_pPhysicsBall->mbUseWindForce = true;
    mtWindTimer.SetSeconds(nlRandomf(gWindDuration * gWindVariation) + gWindDuration * (1.0f - gWindVariation));
    nlVector3 direction;
    nlVector3 position1;
    nlVector3 position2;
    nlVector3 velocity;
    nlVec3Set(position1, 8.0f, 0.0f, 0.0f);
    nlVec3Set(position2, -8.0f, 0.0f, 0.0f);
    nlVec3Set(velocity, 0.0f, 0.0f, 0.0f);
    float effectSpeed = 20.0f;
    ApplyWindForce(&direction);
    nlVec3Normalize(direction, direction);
    nlVec3Scale(direction, direction, effectSpeed);
    nlVec3Sub(position1, position1, direction);
    nlVec3Sub(position2, position2, direction);
    fn_801BDCB4(true);
    fn_801BDC1C(position1, direction, velocity);
    fn_801BDC1C(position2, direction, velocity);
    Weather::Start();
    StopSound(0x04EE0B75, this);
    PlaySound(11, 0x04EE0B75, "Wind Event", this);
    float angle = 360.0f * ((float)aWindDirection / 65536.0f);
    float value = nlRandomf(1.0f);
    if (!gDisableWeather && !GameInfoManager::Instance()->IsRule0x4Equal1()
        && value <= gDebrisChance && GameInfoManager::Instance()->GetStadium() == 11)
    {
        if (fabsf(angle - 90.0f) < gWindAngleRange / 2.0f || fabsf(angle - 270.0f) < gWindAngleRange / 2.0f)
        {
            eLastDebrisType = eDebrisType;
            float index = 2.0f * nlRandomf(1.0f);
            index += index < 0.0f ? -0.5f : 0.5f;
            eDebrisType = (int)index;
            UnidentifiedNPC_801B43F8* npc = gNPCManager->fn_801A9DE0(eDebrisType);
            int i = 0;
            while ((npc->mbIsVisible == 1 || eDebrisType == eLastDebrisType) && i < 3 && i > -1)
            {
                eDebrisType = (eDebrisType + 1) % 3;
                npc = gNPCManager->fn_801A9DE0(eDebrisType);
                i++;
            }
            UnidentifiedNPCConfig_801B532C* config = fn_801B532C(eDebrisType);
            if (!npc->mbIsVisible)
            {
                nlVector3 debrisVelocity = GetDebrisVelocity(direction);
                nlVector3 debrisPosition = position1;
                nlVec3Normalize(direction, direction);
                float distance = gDebrisMinTravelTime + nlRandomf(gDebrisMaxTravelTime - gDebrisMinTravelTime);
                nlVector3 offset;
                nlVec3Scale(offset, debrisVelocity, distance);
                nlVec3Sub(debrisPosition, v3Zero, offset);
                nlVector3 displacement = CalculateDebrisDisplacement(debrisPosition, debrisVelocity, distance, config->mUnidentified008);
                nlVec3Set(debrisPosition, debrisPosition.x + displacement.x, debrisPosition.y + displacement.y, 0.0f);
                debrisPosition.x = nlMinEquals(nlMaxEquals(debrisPosition.x, -127.0f), 127.0f);
                debrisPosition.y = nlMinEquals(nlMaxEquals(debrisPosition.y, -127.0f), 127.0f);
                npc->SetPosition(debrisPosition);
                npc->mv3Velocity = debrisVelocity;
                npc->maFacingDirection = aWindDirection;
                npc->fn_801B4AD0();
            }
        }
    }
}

nlVector3 Windy::CalculateDebrisDisplacement(const nlVector3& position, const nlVector3& velocity, float value, float radius)
{
    nlVector3 ballPosition = g_pBall->m_v3Position;
    ballPosition.z = 0.0f;
    float goalLine = cField::GetGoalLineX(1U);
    float halfWidth = 0.5f * (2.0f * cField::mv3FieldPosition.y);
    ballPosition.x = nlMinEquals(nlMaxEquals(ballPosition.x, -goalLine), goalLine);
    ballPosition.y = nlMinEquals(nlMaxEquals(ballPosition.y, -halfWidth), halfWidth);
    nlVector3 start = position;
    nlVector3 end;
    nlVec3ScaleAdd(end, 1000.0f, velocity, start);
    nlVector3 direction;
    nlVec3Scale(direction, velocity, nlRecipSqrt(nlVec3LengthSquared(velocity), false));
    nlVector3 closest = GetClosestPointOnLineABFromPointC(start, end, ballPosition);
    nlVector3 displacement;
    nlVec3Sub(displacement, ballPosition, closest);
    nlVec3Add(start, start, displacement);
    nlVec3Add(end, end, displacement);
    if (gDebrisGoalieClearance > 0.0f)
    {
        AvoidDebrisObstacle(start, end, displacement, g_pTeams[0]->GetGoalie()->mUnidentified024.m_v3Position, gDebrisGoalieClearance + radius);
        AvoidDebrisObstacle(start, end, displacement, g_pTeams[1]->GetGoalie()->mUnidentified024.m_v3Position, gDebrisGoalieClearance + radius);
    }
    if (gDebrisGoalClearance > 0.0f)
    {
        nlVector3 goal0 = { 0.0f, 0.0f, 0.0f };
        goal0.x = cField::GetGoalLineX(0U);
        nlVector3 goal1 = { 0.0f, 0.0f, 0.0f };
        goal1.x = cField::GetGoalLineX(1U);
        AvoidDebrisObstacle(start, end, displacement, goal0, gDebrisGoalClearance + radius);
        AvoidDebrisObstacle(start, end, displacement, goal1, gDebrisGoalClearance + radius);
    }
    if (gDebrisCornerClearance > 0.0f)
    {
        nlVector3 corner0 = { 21.34f, 12.73f, 0.0f };
        nlVector3 corner1 = { -21.34f, 12.73f, 0.0f };
        nlVector3 corner2 = { -21.34f, -12.73f, 0.0f };
        nlVector3 corner3 = { 21.34f, -12.73f, 0.0f };
        AvoidDebrisObstacle(start, end, displacement, corner0, gDebrisCornerClearance + radius);
        AvoidDebrisObstacle(start, end, displacement, corner1, gDebrisCornerClearance + radius);
        AvoidDebrisObstacle(start, end, displacement, corner2, gDebrisCornerClearance + radius);
        AvoidDebrisObstacle(start, end, displacement, corner3, gDebrisCornerClearance + radius);
    }
    return displacement;
}

void Windy::AvoidDebrisObstacle(nlVector3& start, nlVector3& end, nlVector3& displacement, const nlVector3& position, float radius)
{
    nlVector3 offset = { 0.0f, 0.0f, 0.0f };
    nlVector3 direction;
    nlVector3 closest = GetClosestPointOnLineABFromPointC(start, end, position);
    nlVec3Sub(direction, closest, position);
    float distance = nlSqrt(nlVec3LengthSquared(direction), true);
    if (distance < radius)
    {
        if ((g_pBall->m_v3Position.x > 0.0f && direction.x > 0.0f)
            || (g_pBall->m_v3Position.x < 0.0f && direction.x < 0.0f))
        {
            distance *= -1.0f;
            direction.x *= -1.0f;
            direction.y *= -1.0f;
        }
        float inverseLength = nlRecipSqrt(nlVec3LengthSquared(direction), false);
        nlVec3Set(direction,
            inverseLength * direction.x,
            inverseLength * direction.y,
            inverseLength * direction.z);
        nlVec3Scale(offset, direction, radius - distance);
        nlVec3Add(displacement, displacement, offset);
        nlVec3Add(start, start, offset);
        nlVec3Add(end, end, offset);
    }
}

void Windy::Update(float value)
{
    Weather::Update(value);
    if (!mbPaused && mtWindTimer.m_uPackedTime != 0)
    {
        if (!g_pBall->m_pPhysicsBall->mbUseWindForce)
        {
            g_pBall->m_pPhysicsBall->mbUseWindForce = true;
            ApplyWindForce();
        }
        if (mtWindTimer.Countdown(value, 0.0f))
        {
            Stop(false);
        }
    }
}

void Windy::Stop(bool value)
{
    Weather::Stop(value);
    Reset();
    if (g_pBall != 0)
    {
        g_pBall->m_pPhysicsBall->mbUseWindForce = false;
    }
    fn_801BDCB4(value);
    StopSound(0x04EE0B75, this);
}

SolarFlare::SolarFlare()
{
    meWeather = 4;
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GetReadyForKickoff", -1)->Add(Function<FnVoidVoid>(BindExp1<void, Detail::MemFunImpl<void, void (SolarFlare::*)()>, SolarFlare*>(MemFun(&SolarFlare::OnGetReadyForKickoff), this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("Kickoff", -1)->Add(Function<FnVoidVoid>(BindExp1<void, Detail::MemFunImpl<void, void (SolarFlare::*)()>, SolarFlare*>(MemFun(&SolarFlare::OnKickoff), this)), 0, -1);
    m_StartCount = 0;
    for (int i = 0; i < 3; i++)
        m_TargetIndicies[i] = -1;
    ResetFlares(true);
}

float SolarFlare::GetStartChance()
{
    return m_StartChance;
}

void SolarFlare::Reset()
{
    Weather::Reset();
}

void SolarFlare::OnGetReadyForKickoff()
{
    if (gDisableWeather || GameInfoManager::Instance()->IsRule0x4Equal1())
    {
        return;
    }
    Stop(false);
    m_StartCount++;
    m_TargetIndicies[0] = -1;
    m_TargetIndicies[1] = -1;
    m_TargetIndicies[2] = -1;
    m_StartChance = 100.0f;
    m_NumToVaporizePerTeam = SelectVaporizeTargets();
    m_VaporizeTimer = 0.0f;
    m_NextFlare = 0;
    m_NextVaporize = 0;
    if (m_NumToVaporizePerTeam > 0)
    {
        m_FlareTimer = gSolarFlareInterval;
    }
    if (m_NumToVaporizePerTeam > 0)
    {
        float duration = (2.0f * m_NumToVaporizePerTeam) * gSolarFlareDelay + gSolarFlareInterval;
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                cFielder* fielder = g_pTeams[i]->GetFielder(j);
                fn_8002E1A4(fielder)->mUnidentified004->mUnidentified078 = duration;
                fielder->m_pPhysicsCharacter->m_CanCollideWithBall = false;
            }
        }
        g_pGame->fn_80058A78(duration);
        GameplayCamera* camera = cCameraManager::GetCamera<GameplayCamera>(eCameraType_Gameplay);
        if (camera)
        {
            camera->m_fZoomOverride = gSolarCameraZoom;
        }
    }
}

void SolarFlare::OnKickoff()
{
    if (gDisableWeather || GameInfoManager::Instance()->IsRule0x4Equal1())
    {
        return;
    }
    GameplayCamera* camera = cCameraManager::GetCamera<GameplayCamera>(eCameraType_Gameplay);
    if (camera)
    {
        camera->m_fZoomOverride = 0.0f;
    }
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            cFielder* fielder = g_pTeams[i]->GetFielder(j);
            if (!fielder->IsShattered())
            {
                fielder->m_pPhysicsCharacter->m_CanCollideWithBall = true;
            }
        }
    }
}

inline void SolarFlare::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("SolarFlare");
    cache->AddField(17, gDebugFieldTypes[17].size, 0, "m_FlareTimer");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_VaporizeTimer - (u8*)&m_FlareTimer, "m_VaporizeTimer");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_StartChance - (u8*)&m_FlareTimer, "m_StartChance");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_StartCount - (u8*)&m_FlareTimer, "m_StartCount");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_NumToVaporizePerTeam - (u8*)&m_FlareTimer, "m_NumToVaporizePerTeam");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_NextFlare - (u8*)&m_FlareTimer, "m_NextFlare");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_NextVaporize - (u8*)&m_FlareTimer, "m_NextVaporize");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_TargetIndicies[0] - (u8*)&m_FlareTimer, "m_TargetIndicies[0]");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_TargetIndicies[1] - (u8*)&m_FlareTimer, "m_TargetIndicies[1]");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_TargetIndicies[2] - (u8*)&m_FlareTimer, "m_TargetIndicies[2]");
    cache->EndType();
}

void SolarFlare::SyncLog(void* context, DebugWriteCache* cache)
{
    Weather::SyncLog(context, cache);
    if (sSolarFlareSyncLogType == 0xFFFF)
    {
        RegisterDebugFields(&sSolarFlareSyncLogType, cache);
    }
    cache->ChecksumData(sSolarFlareSyncLogType, &m_FlareTimer, context);
    cache->WriteData(sSolarFlareSyncLogType, &m_FlareTimer, sizeof(SolarFlare) - sizeof(Weather));
}

void SolarFlare::Start()
{
    Weather::Start();
}

void SolarFlare::Update(float value)
{
    Weather::Update(value);
    bool flare = false;
    bool vaporize = false;
    if (meState == 1 && !mbPaused && m_FlareTimer != 0.0f)
    {
        m_FlareTimer -= value;
        if (m_FlareTimer <= 0.0f)
        {
            m_FlareTimer = 0.0f;
            flare = true;
        }
    }
    if (meState == 1 && !mbPaused && m_VaporizeTimer != 0.0f)
    {
        m_VaporizeTimer -= value;
        if (m_VaporizeTimer <= 0.0f)
        {
            m_VaporizeTimer = 0.0f;
            vaporize = true;
        }
    }
    if (vaporize)
    {
        int team = m_NextVaporize % 2;
        int index = (int)floorf(m_NextVaporize / 2.0f);
        cFielder* fielder = g_pTeams[team]->GetFielder(m_TargetIndicies[index]);
        fn_80031A30(fielder, 4, 99999.0f);
        PlaySound(16, 0x5FCB9348, 0, 0);
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("crystal_canyon_player_explode");
        EmissionManager::Instance()->Create(group, 2, true, 0)->SetPosition(fielder->mUnidentified024.m_v3Position);
        ++m_NextVaporize;
        if (m_NextVaporize < 2.0f * m_NumToVaporizePerTeam)
            m_VaporizeTimer = gSolarFlareInterval;
        else
            m_VaporizeTimer = 0.0f;
    }
    if (flare)
    {
        int team = m_NextFlare % 2;
        int index = (int)floorf(m_NextFlare / 2.0f);
        cFielder* fielder = g_pTeams[team]->GetFielder(m_TargetIndicies[index]);
        new (8, false) UnidentifiedObject_8027AE14(fielder->mUnidentified024.m_v3Position);
        PlaySound(11, 0xE853C469, 0, 0);
        if (m_NextFlare == 0)
            PlaySound(13, 0xF68B3F0F, 0, 0);
        fn_80097358(fielder, 10.0f);
        fielder->EndAction();
        if (fielder->m_pBall != 0)
        {
            fielder->ReleaseBall(0);
            g_pBall->SetVelocity(v3Zero, (eSpinType)0, 0);
        }
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("crystal_canyon_zap");
        EmissionManager::Instance()->Create(group, 2, true, 0)->SetPosition(fielder->mUnidentified024.m_v3Position);
        bool controlled = fielder->GetGlobalPad() != 0;
        if (controlled == true)
            fielder->SwapController(false);
        ++m_NextFlare;
        if (m_NextFlare < 2.0f * m_NumToVaporizePerTeam)
            m_FlareTimer = gSolarFlareInterval;
        else
            m_FlareTimer = 0.0f;
        if (m_VaporizeTimer == 0.0f && !vaporize)
            m_VaporizeTimer = gSolarVaporizeDelay;
    }
}

int SolarFlare::SelectVaporizeTargets()
{
    int count = GetMaxVaporizeTargets();
    if (count > 0)
    {
        if (count > 1)
        {
            float value = (count - 1) * nlRandomf(1.0f);
            value += value < 0.0f ? -0.5f : 0.5f;
            count = (int)value + 1;
        }
        bool allowCaptain = !g_pTeams[0]->fn_800A6764() && !g_pTeams[1]->fn_800A6764();
        if (GameInfoManager::Instance()->GetRule0x0() == 9)
        {
            allowCaptain = false;
        }
        for (int i = 0; i < count; i++)
        {
            int index = RandomWeatherIndex(3);
            while (IsTargetSelected(index, i) == true || (!allowCaptain && index == 0)
                   || g_pTeams[0]->GetFielder(index)->IsShattered())
            {
                index = RandomWeatherIndex(3);
            }
            m_TargetIndicies[i] = index;
        }
    }
    return count;
}

int SolarFlare::GetMaxVaporizeTargets()
{
    int count = 4;
    for (int i = 0; i < 2; i++)
    {
        int available = 4;
        cTeam* team = g_pTeams[i];
        if (team != 0)
        {
            for (int j = 0; j < 4; j++)
            {
                bool controlled = team->GetFielder(j)->GetGlobalPad() != 0;
                if (controlled == true || team->GetFielder(j)->IsShattered() == true)
                {
                    available--;
                }
            }
        }
        count = nlMin(count, available);
    }
    return nlMin(count, gSolarMaxVaporizeTargets);
}

void SolarFlare::Stop(bool initialize)
{
    Weather::Stop(initialize);
    if (initialize)
    {
        ResetFlares(initialize);
    }
}

void SolarFlare::ResetFlares(bool initialize)
{
    if (initialize == true)
    {
        m_NumToVaporizePerTeam = 0;
        m_FlareTimer = 0.0f;
        m_VaporizeTimer = 0.0f;
        m_StartChance = 0.0f;
        m_NextFlare = 0;
        m_NextVaporize = 0;
        if (g_pGame == 0 || g_pGame->m_eGameState == 0)
        {
            m_StartCount = 0;
            for (int i = 0; i < 3; ++i)
            {
                m_TargetIndicies[i] = -1;
            }
        }
    }
}

BubblingLava::BubblingLava()
{
    meWeather = 5;
    UnidentifiedFindEvent<UnidentifiedEventData24>("CollisionPatchGround", -1)->Add(Function<UnidentifiedEventData24*>(OnLavaCollisionPatchGround), 0, -1);
    Reset();
}

float BubblingLava::GetStartChance()
{
    if (m_VolleyCountdown <= 0.0f)
    {
        int i;
        int count = 0;
        for (i = 0; i < 60; i++)
        {
            PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
            if (patch)
            {
                if (patch->m_Type != 8 && patch->m_Type != 9)
                    continue;
                count++;
            }
        }
        if (count == 0)
        {
            m_VolleyCountdown = gLavaVolleyDelay;
            return m_StartChance;
        }
    }
    return 0.0f;
}

void OnLavaCollisionPatchGround(UnidentifiedEventData24* event)
{
    PhysicsPatch* patch = event->mUnidentified10;
    if (patch->m_Type == 8 && patch->m_Velocity.z < 0.0f)
    {
        Weather* state = g_pGame->mpWeatherManager->GetWeather(5);
        if (state == 0 || state->meState != 1)
            return;
        nlVector3 position = patch->GetPosition();
        position.z = 0.0f;
        lbl_806E12C8->fn_801743A8(9, 0, position, v3Zero, gLavaPoolRadius, gLavaPoolWidth, gLavaPoolLifetime)->fn_80173B10(gLavaPoolGrowthTime);
        event->mUnidentified10->Unknown0();
        PlaySound(11, 0xC15AA25B, 0, 0);
    }
}

void BubblingLava::Reset()
{
    m_StartChance = gLavaStartChance;
    m_VolleyCountdown = gLavaVolleyDelay;
    m_FlySoundCountdown = 0.0f;
    Weather::Reset();
}

inline void BubblingLava::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("BubblingLava");
    cache->AddField(17, gDebugFieldTypes[17].size, 0, "m_StartChance");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_VolleyCountdown - (u8*)&m_StartChance, "m_VolleyCountdown");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_FlySoundCountdown - (u8*)&m_StartChance, "m_FlySoundCountdown");
    cache->EndType();
}

void BubblingLava::SyncLog(void* context, DebugWriteCache* cache)
{
    Weather::SyncLog(context, cache);
    if (sBubblingLavaSyncLogType == 0xFFFF)
    {
        RegisterDebugFields(&sBubblingLavaSyncLogType, cache);
    }
    cache->ChecksumData(sBubblingLavaSyncLogType, &m_StartChance, context);
    cache->WriteData(sBubblingLavaSyncLogType, &m_StartChance, sizeof(BubblingLava) - sizeof(Weather));
}

void BubblingLava::Start()
{
    Weather::Start();
    float random = nlRandomf(1.0f);
    int count = gLavaVolleyCount;
    if (random < 0.5f)
        count -= 2;
    else if (random < 0.855f)
        count--;
    float maximum = 0.0f;
    float minimum = 999999.0f;
    for (int i = 0; i < count; i++)
    {
        nlVector3 position = { 0.0f, 0.0f, 0.0f };
        nlVector3 velocity = { 0.0f, 0.0f, 28.0f };
        GetPhysicsPatchInfo(8);
        float time = CalculateLavaTrajectory(position, velocity, gLavaGravity);
        PhysicsPatch* patch = lbl_806E12C8->fn_801743A8(8, 0, position, velocity, gLavaBallRadius, gLavaBallRadius, 999.0f);
        patch->m_Gravity = gLavaGravity;
        maximum = nlMaxEquals(maximum, time);
        minimum = nlMinEquals(minimum, time);
    }
    m_FlySoundCountdown = minimum - gLavaSoundLeadTime;
    m_VolleyCountdown = maximum + (gLavaVolleyDelay + gLavaPoolLifetime);
}

float BubblingLava::CalculateLavaTrajectory(nlVector3& outputPosition, nlVector3& outputVelocity, float gravity)
{
    nlVector3 position;
    nlVector3 velocity;
    float coefficient;
    float length = cField::GetGoalLineX(0U);
    float width = 2.0f * cField::mv3FieldPosition.y;
    float halfWidth = 0.5f * width;
    float xRange = length - gLavaPoolRadius;
    float yRange = halfWidth - gLavaPoolRadius;
    float minX, maxX, minY, maxY, minZ, maxZ, margin;
    minX = gLavaMinXSpeed;
    maxX = gLavaMaxXSpeed;
    minY = gLavaMinYSpeed;
    maxY = gLavaMaxYSpeed;
    minZ = gLavaMinZSpeed;
    maxZ = gLavaMaxZSpeed;
    nlVector3 rightGoal = { 0.0f, 0.0f, 0.0f };
    rightGoal.x = length;
    nlVector3 leftGoal = { 0.0f, 0.0f, 0.0f };
    leftGoal.x = -1.0f * length;
    margin = 3.0f * gLavaPoolRadius;
    float goalDistance = gLavaGoalClearance * gLavaGoalClearance;
    position.x = nlRandomf(2.0f * xRange) - xRange;
    position.y = nlRandomf(2.0f * yRange) - yRange;
    position.z = 0.0f;
    if (gLavaYOverride > 0.0f)
    {
        bool negative = position.y < 0.0f;
        position.y = gLavaYOverride;
        if (negative)
            position.y *= -1.0f;
    }
    nlVector3 rightDistance, leftDistance;
    nlVec3Sub(rightDistance, position, rightGoal);
    nlVec3Sub(leftDistance, position, leftGoal);
    if (nlVec3LengthSquared(rightDistance) < goalDistance || nlVec3LengthSquared(leftDistance) < goalDistance)
    {
        float range = length / 2.5f;
        position.x = nlRandomf(2.0f * range) - range;
    }
    float time;
    coefficient = -0.5f * gravity;
    while (true)
    {
        velocity.x = minX + nlRandomf(maxX - minX);
        if (RandomWeatherIndex(1) == 1)
            velocity.x *= -1.0f;
        velocity.y = minY + nlRandomf(maxY - minY);
        if (RandomWeatherIndex(1) == 1)
            velocity.y *= -1.0f;
        velocity.z = minZ + nlRandomf(maxZ - minZ);
        if (RandomWeatherIndex(1) == 1)
            velocity.z *= -1.0f;
        int count;
        float roots[2];
        SolveQuadratic(coefficient, velocity.z, position.z, count, roots[0], roots[1]);
        if (count == 2)
            time = nlMaxEquals(roots[0], roots[1]);
        else if (count == 1)
            time = roots[0];
        else
            time = -9999.9f;
        position.x += velocity.x * time;
        position.y += velocity.y * time;
        if (position.x > length + margin || position.x < -1.0f * (length + margin))
        {
            if (position.y > halfWidth + margin || position.y < -1.0f * (halfWidth + margin))
                break;
        }
    }
    velocity.x *= -1.0f;
    velocity.y *= -1.0f;
    position.z = gLavaBallRadius;
    outputPosition = position;
    outputVelocity = velocity;
    return time;
}

void BubblingLava::Update(float value)
{
    Weather::Update(value);
    if (!mbPaused)
    {
        if (m_VolleyCountdown > 0.0f)
        {
            m_VolleyCountdown -= value;
            if (m_VolleyCountdown <= 0.0f && meState == 1)
                Stop(false);
        }
        if (m_FlySoundCountdown > 0.0f)
        {
            m_FlySoundCountdown -= value;
            if (m_FlySoundCountdown <= 0.0f)
                PlaySound(11, 0x6AA9237C, 0, 0);
        }
    }
}

void BubblingLava::Stop(bool initialize)
{
    m_StartChance = gLavaStartChance;
    m_VolleyCountdown = gLavaVolleyDelay;
    m_FlySoundCountdown = 0.0f;
    if (lbl_806E12C8 && initialize)
    {
        for (int i = 0; i < 60; i++)
        {
            PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
            if (patch)
            {
                if (patch->m_Type != 8 && patch->m_Type != 9)
                    continue;
                patch->Unknown0();
            }
        }
    }
    Weather::Stop(initialize);
}

StormShipWeather::StormShipWeather()
{
    fn_800B0358();
    meWeather = 6;
    Reset();
    m_StartWeatherTimer = gStormStartDelay;
}

float StormShipWeather::GetStartChance()
{
    if (m_StartWeatherTimer <= 0.0f)
    {
        return 100.0f;
    }
    return 0.0f;
}

void StormShipWeather::Reset()
{
    ResetLightning(false);
    Weather::Reset();
}

inline void StormShipWeather::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("StormShipWeather");
    cache->AddField(17, gDebugFieldTypes[17].size, 0, "m_StartWeatherTimer");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_FirstStrikeTimer - (u8*)&m_StartWeatherTimer, "m_FirstStrikeTimer");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_FirstStrikeElectrocuteTimer - (u8*)&m_StartWeatherTimer, "m_FirstStrikeElectrocuteTimer");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_ChainTrainTimer - (u8*)&m_StartWeatherTimer, "m_ChainTrainTimer");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_StartChainTrainTimer - (u8*)&m_StartWeatherTimer, "m_StartChainTrainTimer");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_DarkenTimer - (u8*)&m_StartWeatherTimer, "m_DarkenTimer");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_StopChainLightningSound - (u8*)&m_StartWeatherTimer, "m_StopChainLightningSound");
    cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&m_bRightSide - (u8*)&m_StartWeatherTimer, "m_bRightSide");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_PathIndex1 - (u8*)&m_StartWeatherTimer, "m_PathIndex1");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_PathIndex2 - (u8*)&m_StartWeatherTimer, "m_PathIndex2");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_LoopCount - (u8*)&m_StartWeatherTimer, "m_LoopCount");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&m_ChainCount - (u8*)&m_StartWeatherTimer, "m_ChainCount");
    cache->EndType();
}

void StormShipWeather::SyncLog(void* context, DebugWriteCache* cache)
{
    Weather::SyncLog(context, cache);
    if (sStormShipWeatherSyncLogType == 0xFFFF)
    {
        RegisterDebugFields(&sStormShipWeatherSyncLogType, cache);
    }
    cache->ChecksumData(sStormShipWeatherSyncLogType, &m_StartWeatherTimer, context);
    cache->WriteData(sStormShipWeatherSyncLogType, &m_StartWeatherTimer, sizeof(StormShipWeather) - sizeof(Weather));
}

void StormShipWeather::CreateChainLightning(int index)
{
    PhysicsPatch* patch = lbl_806E12C8->fn_801743A8(10, 0, v3Zero, v3Zero, gStormChainRadius, gStormChainRadius, 10.0f);
    nlVector3* points;
    int count;
    if (m_bRightSide == true)
    {
        points = fn_800B048C(index);
        count = fn_800B04A0(index);
    }
    else
    {
        points = fn_800B0464(index);
        count = fn_800B0478(index);
    }
    patch->fn_801739A4(*points);
    patch->fn_80173C9C(points, count, gStormChainSpeed);
    float length = 0.0f;
    for (int i = 1; i < count; i++)
    {
        nlVector3 delta;
        nlVec3Set(delta, points[i].x - points[i - 1].x, points[i].y - points[i - 1].y, points[i].z - points[i - 1].z);
        length += nlVec3Length(delta);
    }
    float lifetime = length / gStormChainSpeed;
    lifetime += 0.02f;
    patch->m_fCurtime = 0.0f;
    patch->m_fLifetime = lifetime;
    if (m_LoopCount > 0)
        m_StartChainTrainTimer = nlMaxEquals(lifetime, m_StartChainTrainTimer);
    m_StopChainLightningSound = nlMaxEquals(lifetime, m_StopChainLightningSound);
}

void StormShipWeather::Start()
{
    Stop(false);
    Weather::Start();
    m_bRightSide = g_pBall->m_v3Position.x > 0.0f;
    m_FirstStrikeTimer = gStormFirstStrikeDelay;
    m_StartWeatherTimer = gStormStartDelay;
    m_PathIndex1 = RandomWeatherIndex(fn_800B045C() - 1);
    m_PathIndex2 = m_PathIndex1;
    while (m_PathIndex2 == m_PathIndex1)
    {
        m_PathIndex2 = RandomWeatherIndex(fn_800B045C() - 1);
    }
    m_ChainCount = 0;
    WorldDarkening::Instance().Fade(gStormFadeTime, gStormDarkness);
    m_DarkenTimer = gStormDarkenDuration;
    PlaySound(12, 0xD172BFEB, 0, 0);
}

void StormShipWeather::Update(float value)
{
    Weather::Update(value);
    if (!mbPaused)
    {
        if (m_StartWeatherTimer > 0.0f)
        {
            m_StartWeatherTimer -= value;
            if (m_StartWeatherTimer <= 0.0f)
                Stop(false);
        }
        if (m_FirstStrikeTimer > 0.0f)
        {
            m_FirstStrikeTimer -= value;
            if (m_FirstStrikeTimer <= 0.0f)
            {
                EmissionController* controller;
                if (m_bRightSide == false)
                    controller = fn_802E7DC4(EmissionManager::Instance(), "weather_lightning_left", 2, true, 0);
                else
                    controller = fn_802E7DC4(EmissionManager::Instance(), "weather_lightning", 2, true, 0);
                controller->SetPosition(v3Zero);
                controller->SetVelocity(v3Zero);
                m_StartChainTrainTimer = gStormElectrocuteDelay;
                m_FirstStrikeElectrocuteTimer = gStormElectrocuteDelay;
                m_LoopCount = gStormLoopCount;
                m_StopChainLightningSound = 0.0f;
                PlaySound(12, 0x900C09E3, 0, 0);
                float radiusSquared = gStormStrikeRadius * gStormStrikeRadius;
                if (nlVec3DistanceSquared2D(g_pBall->m_v3Position, v3Zero) < radiusSquared
                    && g_pBall->m_pOwner == 0 && g_pBall->meBallState != 10 && g_pBall->meBallState != 9)
                {
                    fn_80015C38(g_pBall, 9);
                }
            }
        }
        if (m_FirstStrikeElectrocuteTimer > 0.0f)
        {
            m_FirstStrikeElectrocuteTimer -= value;
            if (m_FirstStrikeElectrocuteTimer <= 0.0f)
            {
                m_FirstStrikeElectrocuteTimer = 0.0f;
                CreateLightningShockwave(&v3Zero, gStormStrikeRadius);
            }
        }
        if (m_StopChainLightningSound > 0.0f)
        {
            m_StopChainLightningSound -= value;
            if (m_StopChainLightningSound <= 0.0f)
            {
                StopSound(0xF8D24195, this);
                m_StopChainLightningSound = 0.0f;
            }
        }
        if (m_StartChainTrainTimer > 0.0f)
        {
            m_StartChainTrainTimer -= value;
            if (m_StartChainTrainTimer <= 0.0f)
            {
                if (m_LoopCount > 0)
                    m_ChainTrainTimer = value;
                m_ChainCount = 0;
                m_LoopCount--;
                StopSound(0xF8D24195, this);
                PlaySound(11, 0xF8D24195, "Chain Lightning", this);
            }
        }
        if (m_ChainTrainTimer > 0.0f && m_PathIndex1 != -1 && m_PathIndex2 != -1)
        {
            m_ChainTrainTimer -= value;
            if (m_ChainTrainTimer <= 0.0f)
            {
                CreateChainLightning(m_PathIndex1);
                CreateChainLightning(m_PathIndex2);
                g_pGame->mUnidentified0A0 = 0.0f;
                m_ChainCount++;
                if (m_ChainCount < gStormChainCount)
                    m_ChainTrainTimer = gStormChainDelay;
            }
        }
        if (m_DarkenTimer > 0.0f)
        {
            m_DarkenTimer -= value;
            if (m_DarkenTimer <= 0.0f)
                WorldDarkening::Instance().Fade(0.5f * gStormFadeTime, 0.0f);
        }
    }
}

void StormShipWeather::Stop(bool value)
{
    ResetLightning(value);
    Weather::Stop(value);
    StopSound(0xF8D24195, this);
    if (value)
    {
        WorldDarkening::Instance().fn_801AF550();
    }
    else
    {
        WorldDarkening::Instance().Fade(0.5f * gStormFadeTime, 0.0f);
    }
}

void StormShipWeather::ResetLightning(bool initialize)
{
    if (initialize)
    {
        m_StartWeatherTimer = gStormStartDelay;
    }
    m_PathIndex1 = -1;
    m_PathIndex2 = -1;
    m_ChainCount = -1;
    m_LoopCount = 0;
    m_FirstStrikeTimer = 0.0f;
    m_FirstStrikeElectrocuteTimer = 0.0f;
    m_ChainTrainTimer = 0.0f;
    m_StartChainTrainTimer = 0.0f;
    m_DarkenTimer = 0.0f;
    m_StopChainLightningSound = 0.0f;
    m_bRightSide = false;
    if (lbl_806E12C8)
    {
        for (int i = 0; i < 60; i++)
        {
            PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
            if (patch && patch->m_Type == 10)
            {
                patch->Unknown0();
            }
        }
    }
}

SandTombWeather::SandTombWeather()
    : m_Thwomps()
{
    meWeather = 7;
    Reset();
    UnidentifiedFindEvent<UnidentifiedEventNoData>("Kickoff", -1)->Add(Function<FnVoidVoid>(BindExp1<void, Detail::MemFunImpl<void, void (SandTombWeather::*)()>, SandTombWeather*>(MemFun(&SandTombWeather::OnKickoff), this)), 0, -1);
}

float SandTombWeather::GetStartChance()
{
    if (m_ThwompSpawnTimer <= 0.0f)
    {
        bool available = true;
        for (int i = 0; i < 8; i++)
        {
            if (gNPCManager->GetThwomp(i)->mState != THWOMP_STATE_HIDDEN)
                available = false;
            if (gNPCManager->GetThwomp(i)->mState == THWOMP_STATE_IDLE)
                gNPCManager->GetThwomp(i)->Stop(false);
        }
        if (available == true)
            return 100.0f;
    }
    return 0.0f;
}

void SandTombWeather::Reset()
{
    SandTombWeather::Stop(false);
    Weather::Reset();
}

inline void SandTombWeather::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("SandTombWeather");
    cache->AddField(8, gDebugFieldTypes[8].size, 0, "m_NumActiveThwomps");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_ThwompDropTimer - (u8*)&m_NumActiveThwomps, "m_ThwompDropTimer");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_ThwompSpawnTimer - (u8*)&m_NumActiveThwomps, "m_ThwompSpawnTimer");
    cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&m_bSandPatchesCreated - (u8*)&m_NumActiveThwomps, "m_bSandPatchesCreated");
    cache->EndType();
}

void SandTombWeather::SyncLog(void* context, DebugWriteCache* cache)
{
    Weather::SyncLog(context, cache);
    if (sSandTombWeatherSyncLogType == 0xFFFF)
    {
        RegisterDebugFields(&sSandTombWeatherSyncLogType, cache);
    }
    cache->ChecksumData(sSandTombWeatherSyncLogType, &m_NumActiveThwomps, context);
    cache->WriteData(sSandTombWeatherSyncLogType, &m_NumActiveThwomps, sizeof(SandTombWeather) - sizeof(Weather) - sizeof(m_Thwomps));
}

void SandTombWeather::Start()
{
    Stop(false);
    Weather::Start();
    SpawnThwomps();
    m_bSandPatchesCreated = false;
    m_ThwompSpawnTimer = gThwompMinSpawnDelay + nlRandomf(gThwompSpawnDelayRange);
    m_ThwompDropTimer = gThwompMinDropDelay + nlRandomf(gThwompMaxDropDelay - gThwompMinDropDelay);
}

void SandTombWeather::OnKickoff()
{
    if (gDisableWeather || GameInfoManager::Instance()->IsRule0x4Equal1())
        return;
    CreateSandPatches();
}

void SandTombWeather::InvalidateSandPatches()
{
    if (gDisableWeather || GameInfoManager::Instance()->IsRule0x4Equal1())
        return;
    m_bSandPatchesCreated = false;
}

void SandTombWeather::Update(float dt)
{
    Weather::Update(dt);
    if (!mbPaused)
    {
        if (!m_bSandPatchesCreated)
            CreateSandPatches();
        if (m_ThwompSpawnTimer > 0.0f)
            m_ThwompSpawnTimer -= dt;
        else if (GetStartChance() == 100.0f)
            Stop(false);
        if (m_ThwompDropTimer > 0.0f)
        {
            m_ThwompDropTimer -= dt;
            if (m_ThwompDropTimer < 0.0f)
            {
                DropThwomp(-1);
                m_ThwompDropTimer = gThwompMinDropDelay + nlRandomf(gThwompMaxDropDelay - gThwompMinDropDelay);
            }
        }
        if (m_ThwompSpawnTimer > 0.0f && DropThwompNearPlayer() == true)
            m_ThwompDropTimer = gThwompMinDropDelay + nlRandomf(gThwompMaxDropDelay - gThwompMinDropDelay);
    }
}

void SandTombWeather::Stop(bool initialize)
{
    m_NumActiveThwomps = 0;
    m_ThwompSpawnTimer = 0.0f;
    m_ThwompDropTimer = 0.0f;
    m_bSandPatchesCreated = false;
    for (int i = 0; i < 8; i++)
    {
        if (m_Thwomps[i])
            m_Thwomps[i]->Stop(initialize);
        m_Thwomps[i] = 0;
    }
    Weather::Stop(initialize);
}

void SandTombWeather::CreateSandPatches()
{
    if (lbl_806E12C8)
    {
        for (int i = 0; i < 60; i++)
        {
            PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
            if (patch && patch->m_Type == 11)
                patch->Unknown0();
        }
    }
    for (int side = 0; side < 2; side++)
    {
        for (int i = 0; i < fn_800B04B4(this); i++)
        {
            nlVector4 patch = fn_800B04BC(this, i, side == 1);
            nlVector3 position = { patch.x, patch.y, patch.z };
            lbl_806E12C8->fn_801743A8(11, 0, position, v3Zero, patch.w, patch.w, 99999.0f);
        }
    }
    m_bSandPatchesCreated = true;
}

static const nlVector3 sThwompOrigin = { -9.0f, 3.0f, 0.0f };

void SandTombWeather::SpawnThwomps()
{
    int count = gThwompMinCount + RandomWeatherIndex(gThwompMaxCount - gThwompMinCount);
    count = nlMin(count, 8);
    nlVector3 position;
    ThwompObject* thwomp;
    const nlVector3 origin = sThwompOrigin;
    for (int i = 0; i < count; i++)
    {
        int index = -1;
        while (index == -1 || m_Thwomps[index] != 0)
        {
            index = RandomWeatherIndex(8);
        }
        thwomp = gNPCManager->GetThwomp(-1);
        m_Thwomps[index] = thwomp;
        if (thwomp)
        {
            position = origin;
            int side = (int)floorf((index + 1) / 4.0f);
            position.x += 6.0f * ((index + 1) % 4);
            if (side == 1)
                position.y *= -1.0f;
            thwomp->Spawn(position.x, position.y);
            m_NumActiveThwomps++;
        }
    }
}

void SandTombWeather::DropThwomp(int index)
{
    if (index == -1)
    {
        float distance = 999999.9f;
        for (int i = 0; i < 8; i++)
        {
            if (m_Thwomps[i] && m_Thwomps[i]->mState == THWOMP_STATE_IDLE)
            {
                nlVector3 position = *m_Thwomps[i]->GetPosition();
                position.z = 0.0f;
                nlVec3Sub(position, position, g_pBall->m_v3Position);
                float candidateDistance = nlVec3LengthSquared(position);
                if (candidateDistance < distance)
                {
                    index = i;
                    distance = candidateDistance;
                }
            }
        }
    }
    if (index != -1 && m_Thwomps[index]
        && m_Thwomps[index]->mState == THWOMP_STATE_IDLE)
        m_Thwomps[index]->SetState(THWOMP_STATE_WARNING);
}

bool SandTombWeather::DropThwompNearPlayer()
{
    for (int i = 0; i < 8; i++)
    {
        if (m_Thwomps[i] && m_Thwomps[i]->mState == THWOMP_STATE_IDLE)
        {
            int side = (int)floorf((i + 1) / 4.0f);
            const nlVector3 origin = sThwompOrigin;
            nlVector3 position = origin;
            position.x += 6.0f * ((i + 1) % 4);
            if (side == 1)
                position.y *= -1.0f;
            if (IsPlayerInDropZone(position) == true)
            {
                DropThwomp(i);
                return true;
            }
        }
    }
    return false;
}
