#include "Game/Render/tu_801B43F8.h"
#include "NL/nlFunction.inl"
#include "NL/utility.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"
#include "Game/Goalie.h"
#include "Game/AI/AiUtil.h"
#include "unclassified/tu_801B298C.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/EventRegistry.h"
#include "Game/MathHelpers.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Effects/EmissionController.h"
#include "unclassified/tu_80175F8C.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/tu_801B43F8.h"
#include "Game/Render/tu_801B532C.h"
#include "math.h"
#include "Game/Render/tu_8027AE14.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
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
#include "unclassified/tu_800A9B78.h"

#include "Game/Physics/PhysicsPatch.h"
#include "types.h"

class DebugWriteCache;

struct UnidentifiedWeatherState
{
    UnidentifiedWeatherState();
    ~UnidentifiedWeatherState() { fn_800AA7C4(true); }

    virtual void fn_800AA6B4(float);
    virtual void fn_800AA6A8();
    virtual void fn_800AA7C4(bool);
    virtual float fn_800AFFD8();
    virtual void fn_800AA7D0(void*, DebugWriteCache*);
    virtual void fn_800AA8C8();

    s32 field04;
    s32 field08;
    u8 field0C;
    u8 padding0D[3];
};

struct UnidentifiedWeatherFloatState : UnidentifiedWeatherState
{
    float field10;
    float field14;
    float field18;
};

TU800A9B78::TU800A9B78()
{
}


void TU800A9B78::fn_800AA29C()
{
    if (!field00.IsEmpty())
    {
        nlDLListIterator<UnidentifiedWeatherState*> it = field00.Begin();
        while (it.hasNext())
        {
            UnidentifiedWeatherState* state = *it;
            state->fn_800AA7C4(true);
            delete state;
            it.next();
        }
        field00.Clear();
    }
}

TU800A9B78::~TU800A9B78()
{
    fn_800AA29C();
    field00.m_Allocator.FreeBlocks();
}

UnidentifiedWeatherState* TU800A9B78::fn_800AA060(int value)
{
    if (!field00.IsEmpty())
    {
        nlDLListIterator<UnidentifiedWeatherState*> it = field00.Begin();
        while (it.hasNext())
        {
            UnidentifiedWeatherState* state = *it;
            if (value == state->field04)
            {
                return state;
            }
            it.Step();
        }
    }
    return 0;
}

void TU800A9B78::fn_800AA0FC(void* context, DebugWriteCache* cache)
{
    if (!field00.IsEmpty())
    {
        nlDLListIterator<UnidentifiedWeatherState*> it = field00.Begin();
        while (it.hasNext())
        {
            (*it)->fn_800AA7D0(context, cache);
            it.next();
        }
    }
}

void TU800A9B78::fn_800AA1D4(float value)
{
    if (!field00.IsEmpty())
    {
        nlDLListIterator<UnidentifiedWeatherState*> it = field00.Begin();
        while (it.hasNext())
        {
            (*it)->fn_800AA6B4(value);
            it.next();
        }
    }
}

void TU800A9B78::fn_800AA3E8(bool value)
{
    if (!field00.IsEmpty())
    {
        nlDLListIterator<UnidentifiedWeatherState*> it = field00.Begin();
        while (it.hasNext())
        {
            (*it)->fn_800AA7C4(value);
            it.next();
        }
    }
}

void TU800A9B78::fn_800AA4B0()
{
    if (!field00.IsEmpty())
    {
        nlDLListIterator<UnidentifiedWeatherState*> it = field00.Begin();
        while (it.hasNext())
        {
            (*it)->fn_800AA8C8();
            it.next();
        }
    }
}

void TU800A9B78::fn_800AA568()
{
    if (!field00.IsEmpty())
    {
        nlDLListIterator<UnidentifiedWeatherState*> it = field00.Begin();
        while (it.hasNext())
        {
            (*it)->field0C = 1;
            it.next();
        }
    }
}

void TU800A9B78::fn_800AA5F8()
{
    if (!field00.IsEmpty())
    {
        nlDLListIterator<UnidentifiedWeatherState*> it = field00.Begin();
        while (it.hasNext())
        {
            (*it)->field0C = 0;
            it.next();
        }
    }
}

struct UnidentifiedWeatherExtendedStateA : UnidentifiedWeatherState
{
    UnidentifiedWeatherExtendedStateA();
    virtual void fn_800AA6B4(float);
    virtual void fn_800AA6A8();
    virtual void fn_800AA7C4(bool);
    virtual float fn_800AFFD8();
    virtual void fn_800AA7D0(void*, DebugWriteCache*);
    virtual void fn_800AA8C8();

    Timer field10;
    s32 field18;
};

struct UnidentifiedWeatherExtendedStateB : UnidentifiedWeatherState
{
    UnidentifiedWeatherExtendedStateB();
    nlVector3 fn_800ABB38(const nlVector3&, const nlVector3&, float, float);
    void fn_800ABF68(nlVector3&, nlVector3&, nlVector3&, const nlVector3&, float);
    virtual void fn_800AA6B4(float);
    virtual void fn_800AA6A8();
    virtual void fn_800AA7C4(bool);
    virtual float fn_800AFFD8();
    virtual void fn_800AA7D0(void*, DebugWriteCache*);
    virtual void fn_800AA8C8();

    void fn_800AB1F4();

    Timer field10;
    u16 field18;
    u8 padding1A[2];
    float field1C;
    s32 field20;
    s32 field24;
};

struct UnidentifiedWeatherExtendedStateC : UnidentifiedWeatherState
{
    UnidentifiedWeatherExtendedStateC();
    virtual void fn_800AA6B4(float);
    virtual void fn_800AA6A8();
    virtual void fn_800AA7C4(bool);
    virtual float fn_800AFFD8();
    virtual void fn_800AA7D0(void*, DebugWriteCache*);
    virtual void fn_800AA8C8();
    void fn_800AC6D8();
    void fn_800AC8A4();
    int fn_800AD12C();
    int fn_800AD324();
    void fn_800AD400(bool);

    float field10;
    float field14;
    float field18;
    s32 field1C;
    s32 field20;
    s32 field24;
    s32 field28;
    s32 field2C[3];
};

struct UnidentifiedWeatherExtendedStateD : UnidentifiedWeatherState
{
    UnidentifiedWeatherExtendedStateD();
    virtual void fn_800AA6B4(float);
    virtual void fn_800AA6A8();
    virtual void fn_800AA7C4(bool);
    virtual float fn_800AFFD8();
    virtual void fn_800AA7D0(void*, DebugWriteCache*);
    virtual void fn_800AA8C8();

    float fn_800ADAF8(nlVector3&, nlVector3&, float);

    float field10;
    float field14;
    float field18;
};

struct UnidentifiedWeatherExtendedStateE : UnidentifiedWeatherState
{
    UnidentifiedWeatherExtendedStateE();
    virtual void fn_800AA6B4(float);
    virtual void fn_800AA6A8();
    virtual void fn_800AA7C4(bool);
    virtual float fn_800AFFD8();
    virtual void fn_800AA7D0(void*, DebugWriteCache*);
    virtual void fn_800AA8C8();

    void fn_800AEDAC(bool);
    void fn_800AE5B0(int);

    float field10;
    float field14;
    float field18;
    float field1C;
    float field20;
    float field24;
    float field28;
    bool field2C;
    u8 padding2D[3];
    s32 field30;
    s32 field34;
    s32 field38;
    s32 field3C;
};

struct UnidentifiedWeatherExtendedStateF : UnidentifiedWeatherState
{
    UnidentifiedWeatherExtendedStateF();
    void fn_800AFA7C(int);
    virtual void fn_800AA6B4(float);
    virtual void fn_800AA6A8();
    virtual void fn_800AA7C4(bool);
    virtual float fn_800AFFD8();
    virtual void fn_800AA7D0(void*, DebugWriteCache*);
    virtual void fn_800AA8C8();
    void fn_800AF3B4();
    void fn_800AF80C();
    void fn_800AF6F0();
    void fn_800AF404();
    bool fn_800AFBB8();

    s32 field10;
    float field14;
    float field18;
    bool field1C;
    u8 padding1D[3];
    ThwompObject* field20[8];
};

extern "C" float lbl_806DBF28;
extern "C" s32 lbl_806DBF64;
extern "C" float lbl_806DBF68;
extern "C" float lbl_806DBF80;
extern "C" float lbl_806DBF84;


UnidentifiedWeatherState::UnidentifiedWeatherState()
    : field04(0)
    , field08(0)
    , field0C(0)
{
}

void UnidentifiedWeatherState::fn_800AA6A8()
{
    field08 = 1;
}

extern "C" bool lbl_806E0E1C;

void UnidentifiedWeatherState::fn_800AA6B4(float)
{
    if (lbl_806E0E1C || GameInfoManager::Instance()->IsRule0x4Equal1())
    {
        return;
    }
    if (!g_pGame->IsGameplayOrOvertime() && g_pGame->m_eGameState != 1 && field08 == 1)
    {
        fn_800AA7C4(false);
    }
    else if (field08 != 1 && !field0C && field04 != 0)
    {
        float value = nlRandomf(100.0f);
        if (value < fn_800AFFD8())
        {
            fn_800AA6A8();
        }
    }
}

void UnidentifiedWeatherState::fn_800AA7C4(bool)
{
    field08 = 0;
}

static u16 lbl_806DC010 = 0xFFFF;

void UnidentifiedWeatherState::fn_800AA7D0(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC010 == 0xFFFF)
    {
        lbl_806DC010 = cache->BeginType("Weather");
        cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&field04 - (u8*)this, "meWeather");
        cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&field08 - (u8*)this, "meState");
        cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&field0C - (u8*)this, "mbPaused");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DC010, this, context);
    cache->WriteData(lbl_806DC010, this, sizeof(UnidentifiedWeatherState));
}

void UnidentifiedWeatherState::fn_800AA8C8()
{
    field08 = 0;
    field0C = 0;
}

extern "C" float lbl_806DBF5C;
extern "C" float lbl_806DBF58;
extern "C" float lbl_806DBF54;

static u16 lbl_806DC012 = 0xFFFF;

extern "C" float lbl_806DBF60;
extern "C" float lbl_806E0E14;

UnidentifiedWeatherExtendedStateA::UnidentifiedWeatherExtendedStateA()
{
    field04 = 1;
    fn_800AA8C8();
}

float UnidentifiedWeatherExtendedStateA::fn_800AFFD8()
{
    return lbl_806DBF68;
}

void UnidentifiedWeatherExtendedStateA::fn_800AA8C8()
{
    UnidentifiedWeatherState::fn_800AA8C8();
    field10.UnidentifiedClear();
    field18 = lbl_806DBF64;
}

void UnidentifiedWeatherExtendedStateA::fn_800AA7D0(void* context, DebugWriteCache* cache)
{
    UnidentifiedWeatherState::fn_800AA7D0(context, cache);
    if (lbl_806DC012 == 0xFFFF)
    {
        lbl_806DC012 = cache->BeginType("Lightning");
        cache->AddField(20, gDebugFieldTypes[20].size, 0, "mtLightingTimer");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field18 - (u8*)&field10, "mnAmountOfStrikes");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DC012, &field10, context);
    cache->WriteData(lbl_806DC012, &field10, sizeof(UnidentifiedWeatherExtendedStateA) - sizeof(UnidentifiedWeatherState));
}

void UnidentifiedWeatherExtendedStateA::fn_800AA6A8()
{
    UnidentifiedWeatherState::fn_800AA6A8();
    field18 = lbl_806DBF64;
    field10.SetSeconds(0.9f * lbl_806DBF5C + lbl_806DBF5C * nlRandomf(0.1f));
    PlaySound(12, 0xD172BFEB, 0, 0);
    WorldDarkening::Instance().Fade(lbl_806DBF58, lbl_806DBF54);
}


void UnidentifiedWeatherExtendedStateA::fn_800AA6B4(float value)
{
    UnidentifiedWeatherState::fn_800AA6B4(value);
    if (field10.m_uPackedTime != 0 && field10.Countdown(value, 0.0f))
    {
        if (--field18 > 0)
        {
            field10.SetSeconds(0.9f * lbl_806DBF5C + lbl_806DBF5C * nlRandomf(0.1f));
            nlVector3 position = g_pBall->m_v3Position;
            position.x += nlRandomf(10.0f) - 5.0f;
            position.y += nlRandomf(20.0f) - 10.0f;
            position.z = 0.0f;
            cField::FixOutOfBoundsPosition(position, lbl_806E0E14, true);
            LightningStrikeData* data = 0;
            g_LightningStrikeDataPool.Allocate(data);
            data->position = position;
            data->radius = lbl_806E0E14;
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
            data->radius = lbl_806DBF60;
            fn_8005D210(g_pGame, data);
            if (nlRandomf(1.0f) < 0.5f)
            {
                fn_801BDD24("weather_lightning", data->position, true);
            }
            else
            {
                fn_801BDD24("weather_lightning_left", data->position, true);
            }
            WorldDarkening::Instance().Fade(0.5f * lbl_806DBF58, 0.0f);
            PlaySound(12, 0x900C09E3, 0, 0);
            fn_800AA7C4(false);
        }
    }
}

void UnidentifiedWeatherExtendedStateA::fn_800AA7C4(bool value)
{
    if (value)
    {
        WorldDarkening::Instance().fn_801AF550();
    }
    else
    {
        WorldDarkening::Instance().Fade(2.0f * lbl_806DBF58, 0.0f);
    }
    fn_801BDDE0(value);
    field10.UnidentifiedClear();
    field18 = 0;
    UnidentifiedWeatherState::fn_800AA7C4(value);
}

UnidentifiedWeatherExtendedStateB::UnidentifiedWeatherExtendedStateB()
{
    typedef Detail::MemFunImpl<void, void (UnidentifiedWeatherExtendedStateB::*)()> MemFunImpl_UnidentifiedWeatherExtendedStateB_v;
    typedef BindExp1<void, MemFunImpl_UnidentifiedWeatherExtendedStateB_v, UnidentifiedWeatherExtendedStateB*>
        BindExp1_UnidentifiedWeatherExtendedStateB_v;
    field04 = 2;
    fn_800AA8C8();
    Function<FnVoidVoid> callback(BindExp1_UnidentifiedWeatherExtendedStateB_v(
        MemFun(&UnidentifiedWeatherExtendedStateB::fn_800AB1F4), this));
    EventRegistryValue* foundEvent;
    unsigned int hash = HashEventName("GetReadyForKickoff", -1);
    foundEvent = 0;
    g_pEventRegistry->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? foundEvent->event : 0;
    ((UnidentifiedTypedEvent<UnidentifiedEventNoData>*)event)->Add(callback, 0, -1);
}

float UnidentifiedWeatherExtendedStateB::fn_800AFFD8()
{
    return lbl_806DBF28;
}

void UnidentifiedWeatherExtendedStateB::fn_800AA8C8()
{
    UnidentifiedWeatherState::fn_800AA8C8();
    field10.UnidentifiedClear();
    field18 = 0;
    field1C = 0.0f;
    field20 = -1;
    field24 = -1;
}

static u16 lbl_806DC014 = 0xFFFF;

void UnidentifiedWeatherExtendedStateB::fn_800AA7D0(void* context, DebugWriteCache* cache)
{
    UnidentifiedWeatherState::fn_800AA7D0(context, cache);
    if (lbl_806DC014 == 0xFFFF)
    {
        lbl_806DC014 = cache->BeginType("Windy");
        cache->AddField(20, gDebugFieldTypes[20].size, 0, "mtWindTimer");
        cache->AddField(19, gDebugFieldTypes[19].size, (u8*)&field18 - (u8*)&field10, "aWindDirection");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field1C - (u8*)&field10, "fWindStrength");
        cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&field20 - (u8*)&field10, "eDebrisType");
        cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&field24 - (u8*)&field10, "eLastDebrisType");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DC014, &field10, context);
    cache->WriteData(lbl_806DC014, &field10, sizeof(UnidentifiedWeatherExtendedStateB) - sizeof(UnidentifiedWeatherState));
}

void UnidentifiedWeatherExtendedStateB::fn_800AB1F4()
{
    for (int i = 0; i < 3; i++)
    {
        gNPCManager->fn_801A9DE0(i)->fn_801B4B24(false);
    }
}

void UnidentifiedWeatherExtendedStateB::fn_800AA6B4(float value)
{
    UnidentifiedWeatherState::fn_800AA6B4(value);
    if (!field0C && field10.m_uPackedTime != 0)
    {
        if (!g_pBall->m_pPhysicsBall->mbUseWindForce)
        {
            g_pBall->m_pPhysicsBall->mbUseWindForce = true;
            nlVector3 force;
            force.z = 0.0f;
            nlPolarToCartesian(force.x, force.y, field18, field1C);
            g_pBall->m_pPhysicsBall->mv3WindForce = force;
        }
        if (field10.Countdown(value, 0.0f))
        {
            fn_800AA7C4(false);
        }
    }
}

void UnidentifiedWeatherExtendedStateB::fn_800AA7C4(bool value)
{
    UnidentifiedWeatherState::fn_800AA7C4(value);
    fn_800AA8C8();
    if (g_pBall != 0)
    {
        g_pBall->m_pPhysicsBall->mbUseWindForce = false;
    }
    fn_801BDCB4(value);
    StopSound(0x04EE0B75, this);
}

extern "C" float lbl_806DBF6C;
extern "C" float lbl_806DBF70;
extern "C" float lbl_806DBF7C;

extern "C" float lbl_806DBF38;
extern "C" float lbl_806DBF20;
extern "C" float lbl_806DBF24;
extern "C" float lbl_806E0E10;
extern "C" float lbl_806DBF34;
extern "C" float lbl_806DBF48;
extern "C" float lbl_806DBF4C;
extern "C" float lbl_806DBF50;
extern "C" float lbl_806DBF2C;
extern "C" float lbl_806DBF30;
extern "C" void fn_801BDC1C(const nlVector3&, const nlVector3&, const nlVector3&);

void UnidentifiedWeatherExtendedStateB::fn_800AA6A8()
{
    fn_800AA8C8();
    float range = 65536.0f * (lbl_806DBF38 / 360.0f);
    field18 = (u16)(s32)(nlRandomf(range) - 0.5f * range - 16384.0f);
    if (nlRandomf(65536.0f) < 32768.0f)
    {
        field18 += 32768.0f;
    }
    field1C = nlRandomf(lbl_806DBF20 * lbl_806E0E10) + lbl_806DBF20 * (1.0f - lbl_806E0E10);
    g_pBall->m_pPhysicsBall->mbUseWindForce = true;
    field10.SetSeconds(nlRandomf(lbl_806DBF24 * lbl_806E0E10) + lbl_806DBF24 * (1.0f - lbl_806E0E10));
    nlVector3 direction;
    nlVector3 position1;
    nlVector3 position2;
    nlVector3 velocity;
    nlVec3Set(position1, 8.0f, 0.0f, 0.0f);
    nlVec3Set(position2, -8.0f, 0.0f, 0.0f);
    nlVec3Set(velocity, 0.0f, 0.0f, 0.0f);
    float effectSpeed = 20.0f;
    nlVector3 force;
    force.z = 0.0f;
    nlPolarToCartesian(force.x, force.y, field18, field1C);
    direction = g_pBall->m_pPhysicsBall->mv3WindForce = force;
    nlVec3Scale(direction, direction, nlRecipSqrt(nlVec3LengthSquared(direction), true));
    nlVec3Scale(direction, direction, effectSpeed);
    nlVec3Sub(position1, position1, direction);
    nlVec3Sub(position2, position2, direction);
    fn_801BDCB4(true);
    fn_801BDC1C(position1, direction, velocity);
    fn_801BDC1C(position2, direction, velocity);
    UnidentifiedWeatherState::fn_800AA6A8();
    StopSound(0x04EE0B75, this);
    PlaySound(11, 0x04EE0B75, "Wind Event", this);
    float angle = 360.0f * ((float)field18 / 65536.0f);
    float value = nlRandomf(1.0f);
    if (!lbl_806E0E1C && !GameInfoManager::Instance()->IsRule0x4Equal1()
        && value <= lbl_806DBF34 && GameInfoManager::Instance()->GetStadium() == 11)
    {
        if (nlAbs(angle - 90.0f) < 0.5f * lbl_806DBF38 || nlAbs(angle - 270.0f) < 0.5f * lbl_806DBF38)
        {
            field24 = field20;
            float index = 2.0f * nlRandomf(1.0f);
            index += index < 0.0f ? -0.5f : 0.5f;
            field20 = (int)index;
            UnidentifiedNPC_801B43F8* npc = gNPCManager->fn_801A9DE0(field20);
            int i = 0;
            while ((npc->mbIsVisible == 1 || field20 == field24) && i < 3)
            {
                field20 = (field20 + 1) % 3;
                npc = gNPCManager->fn_801A9DE0(field20);
                i++;
            }
            UnidentifiedNPCConfig_801B532C* config = fn_801B532C(field20);
            if (!npc->mbIsVisible)
            {
                float speed = 0.0f;
                switch (field20)
                {
                case 0: speed = lbl_806DBF48; break;
                case 1: speed = lbl_806DBF4C; break;
                case 2: speed = lbl_806DBF50; break;
                }
                nlVector3 debrisPosition;
                nlVector3 debrisVelocity;
                nlVector3 normalized;
                nlVec3Scale(normalized, direction, nlRecipSqrt(nlVec3LengthSquared(direction), false));
                nlVec3Scale(debrisVelocity, normalized, speed);
                debrisPosition = position1;
                nlVec3Scale(direction, direction, nlRecipSqrt(nlVec3LengthSquared(direction), true));
                float distance = lbl_806DBF30 + nlRandomf(lbl_806DBF2C - lbl_806DBF30);
                nlVector3 offset;
                nlVec3Scale(offset, debrisVelocity, distance);
                nlVec3Sub(debrisPosition, v3Zero, offset);
                nlVector3 displacement = fn_800ABB38(debrisPosition, debrisVelocity, distance, config->mUnidentified008);
                nlVec3Set(debrisPosition, debrisPosition.x + displacement.x, debrisPosition.y + displacement.y, 0.0f);
                debrisPosition.x = nlMinEquals(nlMaxEquals(debrisPosition.x, -127.0f), 127.0f);
                debrisPosition.y = nlMinEquals(nlMaxEquals(debrisPosition.y, -127.0f), 127.0f);
                npc->SetPosition(debrisPosition);
                npc->mv3Velocity = debrisVelocity;
                npc->maFacingDirection = field18;
                npc->fn_801B4AD0();
            }
        }
    }
}

UnidentifiedWeatherExtendedStateC::UnidentifiedWeatherExtendedStateC()
{
    typedef Detail::MemFunImpl<void, void (UnidentifiedWeatherExtendedStateC::*)()> MemFunImpl_UnidentifiedWeatherExtendedStateC_v;
    typedef BindExp1<void, MemFunImpl_UnidentifiedWeatherExtendedStateC_v, UnidentifiedWeatherExtendedStateC*>
        BindExp1_UnidentifiedWeatherExtendedStateC_v;
    field04 = 4;
    {
        Function<FnVoidVoid> callback(BindExp1_UnidentifiedWeatherExtendedStateC_v(
            MemFun(&UnidentifiedWeatherExtendedStateC::fn_800AC6D8), this));
        EventRegistryValue* foundEvent;
        unsigned int hash = HashEventName("GetReadyForKickoff", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? foundEvent->event : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventNoData>*)event)->Add(callback, 0, -1);
    }
    {
        Function<FnVoidVoid> callback(BindExp1_UnidentifiedWeatherExtendedStateC_v(
            MemFun(&UnidentifiedWeatherExtendedStateC::fn_800AC8A4), this));
        EventRegistryValue* foundEvent;
        unsigned int hash = HashEventName("Kickoff", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? foundEvent->event : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventNoData>*)event)->Add(callback, 0, -1);
    }
    field1C = 0;
    for (int i = 0; i < 3; i++)
        field2C[i] = -1;
    fn_800AD400(true);
}

void UnidentifiedWeatherExtendedStateC::fn_800AC6D8()
{
    if (lbl_806E0E1C || GameInfoManager::Instance()->IsRule0x4Equal1())
    {
        return;
    }
    fn_800AA7C4(false);
    field1C++;
    field2C[0] = -1;
    field2C[1] = -1;
    field2C[2] = -1;
    field18 = 100.0f;
    field20 = fn_800AD12C();
    field14 = 0.0f;
    field24 = 0;
    field28 = 0;
    if (field20 > 0)
    {
        field10 = lbl_806DBF70;
    }
    if (field20 > 0)
    {
        float duration = (2.0f * field20) * lbl_806DBF6C + lbl_806DBF70;
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
            camera->m_fZoomOverride = lbl_806DBF7C;
        }
    }
}

void UnidentifiedWeatherExtendedStateC::fn_800AC8A4()
{
    if (lbl_806E0E1C || GameInfoManager::Instance()->IsRule0x4Equal1())
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

float UnidentifiedWeatherExtendedStateC::fn_800AFFD8()
{
    return field18;
}

void UnidentifiedWeatherExtendedStateC::fn_800AA8C8()
{
    UnidentifiedWeatherState::fn_800AA8C8();
}

static u16 lbl_806DC016 = 0xFFFF;

void UnidentifiedWeatherExtendedStateC::fn_800AA7D0(void* context, DebugWriteCache* cache)
{
    UnidentifiedWeatherState::fn_800AA7D0(context, cache);
    if (lbl_806DC016 == 0xFFFF)
    {
        lbl_806DC016 = cache->BeginType("SolarFlare");
        cache->AddField(17, gDebugFieldTypes[17].size, 0, "m_FlareTimer");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field14 - (u8*)&field10, "m_VaporizeTimer");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field18 - (u8*)&field10, "m_StartChance");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field1C - (u8*)&field10, "m_StartCount");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field20 - (u8*)&field10, "m_NumToVaporizePerTeam");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field24 - (u8*)&field10, "m_NextFlare");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field28 - (u8*)&field10, "m_NextVaporize");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field2C[0] - (u8*)&field10, "m_TargetIndicies[0]");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field2C[1] - (u8*)&field10, "m_TargetIndicies[1]");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field2C[2] - (u8*)&field10, "m_TargetIndicies[2]");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DC016, &field10, context);
    cache->WriteData(lbl_806DC016, &field10, sizeof(UnidentifiedWeatherExtendedStateC) - sizeof(UnidentifiedWeatherState));
}

void UnidentifiedWeatherExtendedStateC::fn_800AA6A8()
{
    UnidentifiedWeatherState::fn_800AA6A8();
}

extern "C" int lbl_806DBF78;

int UnidentifiedWeatherExtendedStateC::fn_800AD324()
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
    return nlMin(count, lbl_806DBF78);
}

extern "C" void fn_80031A30(cFielder*, int, float);
extern "C" void fn_80097358(cPlayer*, float);
extern "C" float lbl_806DBF74;

void UnidentifiedWeatherExtendedStateC::fn_800AA6B4(float value)
{
    UnidentifiedWeatherState::fn_800AA6B4(value);
    bool flare = false;
    bool vaporize = false;
    if (field08 == 1 && !field0C && field10 != 0.0f)
    {
        field10 -= value;
        if (field10 <= 0.0f)
        {
            field10 = 0.0f;
            flare = true;
        }
    }
    if (field08 == 1 && !field0C && field14 != 0.0f)
    {
        field14 -= value;
        if (field14 <= 0.0f)
        {
            field14 = 0.0f;
            vaporize = true;
        }
    }
    if (vaporize)
    {
        int team = field28 % 2;
        int index = (int)floorf(field28 / 2.0f);
        cFielder* fielder = g_pTeams[team]->GetFielder(field2C[index]);
        fn_80031A30(fielder, 4, 99999.0f);
        PlaySound(16, 0x5FCB9348, 0, 0);
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("crystal_canyon_player_explode");
        EmissionManager::Instance()->Create(group, 2, true, 0)->SetPosition(fielder->mUnidentified024.m_v3Position);
        ++field28;
        if (field28 < 2.0f * field20)
            field14 = lbl_806DBF70;
        else
            field14 = 0.0f;
    }
    if (flare)
    {
        int team = field24 % 2;
        int index = (int)floorf(field24 / 2.0f);
        cFielder* fielder = g_pTeams[team]->GetFielder(field2C[index]);
        new (8, false) UnidentifiedObject_8027AE14(fielder->mUnidentified024.m_v3Position);
        PlaySound(11, 0xE853C469, 0, 0);
        if (field24 == 0)
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
        ++field24;
        if (field24 < 2.0f * field20)
            field10 = lbl_806DBF70;
        else
            field10 = 0.0f;
        if (field14 == 0.0f && !vaporize)
            field14 = lbl_806DBF74;
    }
}

void UnidentifiedWeatherExtendedStateC::fn_800AA7C4(bool initialize)
{
    UnidentifiedWeatherState::fn_800AA7C4(initialize);
    if (initialize)
    {
        fn_800AD400(initialize);
    }
}

void UnidentifiedWeatherExtendedStateC::fn_800AD400(bool initialize)
{
    if (initialize == true)
    {
        field20 = 0;
        field10 = 0.0f;
        field14 = 0.0f;
        field18 = 0.0f;
        field24 = 0;
        field28 = 0;
        if (g_pGame == 0 || g_pGame->m_eGameState == 0)
        {
            field1C = 0;
            field2C[0] = -1;
            field2C[1] = -1;
            field2C[2] = -1;
        }
    }
}

float UnidentifiedWeatherExtendedStateD::fn_800AFFD8()
{
    if (field14 <= 0.0f)
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
            field14 = lbl_806DBF84;
            return field10;
        }
    }
    return 0.0f;
}

void UnidentifiedWeatherExtendedStateD::fn_800AA6B4(float value)
{
    UnidentifiedWeatherState::fn_800AA6B4(value);
    if (!field0C)
    {
        if (field14 > 0.0f)
        {
            field14 -= value;
            if (field14 <= 0.0f && field08 == 1)
                fn_800AA7C4(false);
        }
        if (field18 > 0.0f)
        {
            field18 -= value;
            if (field18 <= 0.0f)
                PlaySound(11, 0x6AA9237C, 0, 0);
        }
    }
}
extern "C" void fn_800AD628(UnidentifiedEventData24*);

int UnidentifiedWeatherExtendedStateC::fn_800AD12C()
{
    int count = fn_800AD324();
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
            int index;
            bool used;
            do
            {
                float value = 3.0f * nlRandomf(1.0f);
                index = (int)(value + (value < 0.0f ? -0.5f : 0.5f));
                used = false;
                for (int j = 0; j < i; j++)
                {
                    if (index == field2C[j])
                    {
                        used = true;
                        break;
                    }
                }
            } while (used == true || (!allowCaptain && index == 0)
                || g_pTeams[0]->GetFielder(index)->IsShattered());
            field2C[i] = index;
        }
    }
    return count;
}

UnidentifiedWeatherExtendedStateD::UnidentifiedWeatherExtendedStateD()
{
    field04 = 5;
    {
        Function<UnidentifiedEventData24*> callback(fn_800AD628);
        EventRegistryValue* foundEvent;
        unsigned int hash = HashEventName("CollisionPatchGround", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? foundEvent->event : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventData24>*)event)->Add(callback, 0, -1);
    }
    fn_800AA8C8();
}


extern "C" int lbl_806DBF88;
extern "C" float lbl_806DBF8C;
extern "C" float lbl_806DBF90;
extern "C" float lbl_806DBFB8;
extern "C" float lbl_806DBFC0;
extern "C" int lbl_806DC028;

void UnidentifiedWeatherExtendedStateD::fn_800AA6A8()
{
    UnidentifiedWeatherState::fn_800AA6A8();
    float random = nlRandomf(1.0f);
    int count = lbl_806DBF88;
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
        fn_80174ED4(&lbl_806DC028);
        float time = fn_800ADAF8(position, velocity, lbl_806DBF90);
        PhysicsPatch* patch = lbl_806E12C8->fn_801743A8(8, 0, position, velocity,
            lbl_806DBF8C, lbl_806DBF8C, 999.0f);
        patch->m_Gravity = lbl_806DBF90;
        maximum = nlMaxEquals(maximum, time);
        minimum = nlMinEquals(minimum, time);
    }
    field18 = minimum - lbl_806DBFC0;
    field14 = maximum + (lbl_806DBF84 + lbl_806DBFB8);
}



void UnidentifiedWeatherExtendedStateD::fn_800AA7C4(bool initialize)
{
    field10 = lbl_806DBF80;
    field14 = lbl_806DBF84;
    field18 = 0.0f;
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
    UnidentifiedWeatherState::fn_800AA7C4(initialize);
}

void UnidentifiedWeatherExtendedStateD::fn_800AA8C8()
{
    field10 = lbl_806DBF80;
    field14 = lbl_806DBF84;
    field18 = 0.0f;
    UnidentifiedWeatherState::fn_800AA8C8();
}

static u16 lbl_806DC018 = 0xFFFF;

void UnidentifiedWeatherExtendedStateD::fn_800AA7D0(void* context, DebugWriteCache* cache)
{
    UnidentifiedWeatherState::fn_800AA7D0(context, cache);
    if (lbl_806DC018 == 0xFFFF)
    {
        lbl_806DC018 = cache->BeginType("BubblingLava");
        cache->AddField(17, gDebugFieldTypes[17].size, 0, "m_StartChance");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field14 - (u8*)&field10, "m_VolleyCountdown");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field18 - (u8*)&field10, "m_FlySoundCountdown");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DC018, &field10, context);
    cache->WriteData(lbl_806DC018, &field10, sizeof(UnidentifiedWeatherExtendedStateD) - sizeof(UnidentifiedWeatherState));
}

extern "C" void fn_800B0358();
extern "C" float lbl_806DBFC8;

extern "C" float lbl_806DBF94;
extern "C" float lbl_806DBF98;
extern "C" float lbl_806DBF9C;
extern "C" float lbl_806DBFA0;
extern "C" float lbl_806DBFA4;
extern "C" float lbl_806DBFA8;
extern "C" float lbl_806DBFAC;
extern "C" float lbl_806DBFBC;
extern "C" float lbl_806E0E18;

float UnidentifiedWeatherExtendedStateD::fn_800ADAF8(nlVector3& outputPosition, nlVector3& outputVelocity, float gravity)
{
    nlVector3 position;
    nlVector3 velocity;
    float length = cField::GetGoalLineX(0U);
    float width = 2.0f * cField::mv3FieldPosition.y;
    float halfWidth = 0.5f * width;
    float radius = lbl_806DBFAC;
    float xRange = length - radius;
    float yRange = halfWidth - radius;
    float margin = 3.0f * radius;
    float minX = lbl_806DBF9C;
    float maxX = lbl_806DBFA0;
    float minY = lbl_806DBFA4;
    float maxY = lbl_806DBFA8;
    float minZ = lbl_806DBF94;
    float maxZ = lbl_806DBF98;
    float goalDistance = lbl_806DBFBC * lbl_806DBFBC;
    nlVector3 rightGoal = { 0.0f, 0.0f, 0.0f };
    rightGoal.x = length;
    nlVector3 leftGoal = { 0.0f, 0.0f, 0.0f };
    leftGoal.x = -1.0f * length;
    position.x = nlRandomf(2.0f * xRange) - xRange;
    position.y = nlRandomf(2.0f * yRange) - yRange;
    position.z = 0.0f;
    if (lbl_806E0E18 > 0.0f)
    {
        bool negative = position.y < 0.0f;
        position.y = lbl_806E0E18;
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
    float coefficient = -0.5f * gravity;
    while (true)
    {
        velocity.x = minX + nlRandomf(maxX - minX);
        float randomX = nlRandomf(1.0f);
        if ((int)(randomX + (randomX < 0.0f ? -0.5f : 0.5f)) == 1)
            velocity.x *= -1.0f;
        velocity.y = minY + nlRandomf(maxY - minY);
        float randomY = nlRandomf(1.0f);
        if ((int)(randomY + (randomY < 0.0f ? -0.5f : 0.5f)) == 1)
            velocity.y *= -1.0f;
        velocity.z = minZ + nlRandomf(maxZ - minZ);
        float randomZ = nlRandomf(1.0f);
        if ((int)(randomZ + (randomZ < 0.0f ? -0.5f : 0.5f)) == 1)
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
    position.z = lbl_806DBF8C;
    outputPosition = position;
    outputVelocity = velocity;
    return time;
}

UnidentifiedWeatherExtendedStateE::UnidentifiedWeatherExtendedStateE()
{
    fn_800B0358();
    field04 = 6;
    fn_800AA8C8();
    field10 = lbl_806DBFC8;
}

extern "C" nlVector3* fn_800B0464(int);
extern "C" int fn_800B0478(int);
extern "C" nlVector3* fn_800B048C(int);
extern "C" int fn_800B04A0(int);
extern "C" float lbl_806DBFD4;
extern "C" float lbl_806DBFD8;

void UnidentifiedWeatherExtendedStateE::fn_800AE5B0(int index)
{
    PhysicsPatch* patch = lbl_806E12C8->fn_801743A8(10, 0, v3Zero, v3Zero,
        lbl_806DBFD4, lbl_806DBFD4, 999.0f);
    nlVector3* points;
    int count;
    if (field2C == true)
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
    patch->fn_80173C9C(points, count, lbl_806DBFD8);
    float length = 0.0f;
    for (int i = 1; i < count; i++)
    {
        nlVector3 delta;
        nlVec3Sub(delta, points[i], points[i - 1]);
        length += nlVec3Length(delta);
    }
    float lifetime = length / lbl_806DBFD8;
    lifetime += 0.02f;
    patch->m_fCurtime = 0.0f;
    patch->m_fLifetime = lifetime;
    if (field38 > 0)
        field20 = nlMaxEquals(lifetime, field20);
    field28 = nlMaxEquals(lifetime, field28);
}


extern "C" float lbl_806DBFEC;

void UnidentifiedWeatherExtendedStateE::fn_800AA7C4(bool value)
{
    fn_800AEDAC(value);
    UnidentifiedWeatherState::fn_800AA7C4(value);
    StopSound(0xF8D24195, this);
    if (value)
    {
        WorldDarkening::Instance().fn_801AF550();
    }
    else
    {
        WorldDarkening::Instance().Fade(0.5f * lbl_806DBFEC, 0.0f);
    }
}

float UnidentifiedWeatherExtendedStateE::fn_800AFFD8()
{
    if (field10 <= 0.0f)
    {
        return 1.0f;
    }
    return 0.0f;
}

void UnidentifiedWeatherExtendedStateE::fn_800AA8C8()
{
    fn_800AEDAC(false);
    UnidentifiedWeatherState::fn_800AA8C8();
}

static u16 lbl_806DC01A = 0xFFFF;

void UnidentifiedWeatherExtendedStateE::fn_800AA7D0(void* context, DebugWriteCache* cache)
{
    UnidentifiedWeatherState::fn_800AA7D0(context, cache);
    if (lbl_806DC01A == 0xFFFF)
    {
        lbl_806DC01A = cache->BeginType("StormShipWeather");
        cache->AddField(17, gDebugFieldTypes[17].size, 0, "m_StartWeatherTimer");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field14 - (u8*)&field10, "m_FirstStrikeTimer");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field18 - (u8*)&field10, "m_FirstStrikeElectrocuteTimer");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field1C - (u8*)&field10, "m_ChainTrainTimer");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field20 - (u8*)&field10, "m_StartChainTrainTimer");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field24 - (u8*)&field10, "m_DarkenTimer");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field28 - (u8*)&field10, "m_StopChainLightningSound");
        cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&field2C - (u8*)&field10, "m_bRightSide");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field30 - (u8*)&field10, "m_PathIndex1");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field34 - (u8*)&field10, "m_PathIndex2");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field38 - (u8*)&field10, "m_LoopCount");
        cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&field3C - (u8*)&field10, "m_ChainCount");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DC01A, &field10, context);
    cache->WriteData(lbl_806DC01A, &field10, sizeof(UnidentifiedWeatherExtendedStateE) - sizeof(UnidentifiedWeatherState));
}

extern "C" int fn_800B045C();
extern "C" float lbl_806DBFCC;
extern "C" float lbl_806DBFE8;
extern "C" float lbl_806DBFF0;

void UnidentifiedWeatherExtendedStateE::fn_800AA6A8()
{
    fn_800AA7C4(false);
    UnidentifiedWeatherState::fn_800AA6A8();
    field2C = g_pBall->m_v3Position.x > 0.0f;
    field14 = lbl_806DBFCC;
    field10 = lbl_806DBFC8;
    int maximum = fn_800B045C() - 1;
    float random = maximum * nlRandomf(1.0f);
    field30 = (int)(random + (random < 0.0f ? -0.5f : 0.5f));
    field34 = field30;
    while (field34 == field30)
    {
        int maximum = fn_800B045C() - 1;
        float random = maximum * nlRandomf(1.0f);
        field34 = (int)(random + (random < 0.0f ? -0.5f : 0.5f));
    }
    field3C = 0;
    WorldDarkening::Instance().Fade(lbl_806DBFEC, lbl_806DBFE8);
    field24 = lbl_806DBFF0;
    PlaySound(12, 0xD172BFEB, 0, 0);
}

extern "C" float lbl_806DBFC4;
extern "C" int lbl_806DBFD0;
extern "C" float lbl_806DBFDC;
extern "C" int lbl_806DBFE0;
extern "C" float lbl_806DBFE4;
extern "C" float lbl_806DBFEC;

void UnidentifiedWeatherExtendedStateE::fn_800AA6B4(float value)
{
    UnidentifiedWeatherState::fn_800AA6B4(value);
    if (!field0C)
    {
        if (field10 > 0.0f)
        {
            field10 -= value;
            if (field10 <= 0.0f)
                fn_800AA7C4(false);
        }
        if (field14 > 0.0f)
        {
            field14 -= value;
            if (field14 <= 0.0f)
            {
                EmissionController* controller;
                if (field2C == false)
                    controller = fn_802E7DC4(EmissionManager::Instance(), "weather_lightning_left", 2, true, 0);
                else
                    controller = fn_802E7DC4(EmissionManager::Instance(), "weather_lightning", 2, true, 0);
                controller->SetPosition(v3Zero);
                controller->SetVelocity(v3Zero);
                field20 = lbl_806DBFDC;
                field18 = lbl_806DBFDC;
                field38 = lbl_806DBFD0;
                field28 = 0.0f;
                PlaySound(12, 0x900C09E3, 0, 0);
                float radiusSquared = lbl_806DBFC4 * lbl_806DBFC4;
                if (nlVec3DistanceSquared2D(g_pBall->m_v3Position, v3Zero) < radiusSquared
                    && g_pBall->m_pOwner == 0 && g_pBall->meBallState != 10 && g_pBall->meBallState != 9)
                {
                    fn_80015C38(g_pBall, 9);
                }
            }
        }
        if (field18 > 0.0f)
        {
            field18 -= value;
            if (field18 <= 0.0f)
            {
                field18 = 0.0f;
                fn_801768E0(&v3Zero, lbl_806DBFC4);
            }
        }
        if (field28 > 0.0f)
        {
            field28 -= value;
            if (field28 <= 0.0f)
            {
                StopSound(0xF8D24195, this);
                field28 = 0.0f;
            }
        }
        if (field20 > 0.0f)
        {
            field20 -= value;
            if (field20 <= 0.0f)
            {
                if (field38 > 0)
                    field1C = value;
                field3C = 0;
                field38--;
                StopSound(0xF8D24195, this);
                PlaySound(11, 0xF8D24195, "Chain Lightning", this);
            }
        }
        if (field1C > 0.0f && field30 != -1 && field34 != -1)
        {
            field1C -= value;
            if (field1C <= 0.0f)
            {
                fn_800AE5B0(field30);
                fn_800AE5B0(field34);
                g_pGame->mUnidentified0A0 = 0.0f;
                field3C++;
                if (field3C < lbl_806DBFE0)
                    field1C = lbl_806DBFE4;
            }
        }
        if (field24 > 0.0f)
        {
            field24 -= value;
            if (field24 <= 0.0f)
                WorldDarkening::Instance().Fade(0.5f * lbl_806DBFEC, 0.0f);
        }
    }
}

UnidentifiedWeatherExtendedStateF::UnidentifiedWeatherExtendedStateF()
    : field20()
{
    typedef Detail::MemFunImpl<void, void (UnidentifiedWeatherExtendedStateF::*)()> MemFunImpl_UnidentifiedWeatherExtendedStateF_v;
    typedef BindExp1<void, MemFunImpl_UnidentifiedWeatherExtendedStateF_v, UnidentifiedWeatherExtendedStateF*>
        BindExp1_UnidentifiedWeatherExtendedStateF_v;
    field04 = 7;
    fn_800AA8C8();
    Function<FnVoidVoid> callback(BindExp1_UnidentifiedWeatherExtendedStateF_v(
        MemFun(&UnidentifiedWeatherExtendedStateF::fn_800AF3B4), this));
    EventRegistryValue* foundEvent;
    unsigned int hash = HashEventName("Kickoff", -1);
    foundEvent = 0;
    g_pEventRegistry->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? foundEvent->event : 0;
    ((UnidentifiedTypedEvent<UnidentifiedEventNoData>*)event)->Add(callback, 0, -1);
}

float UnidentifiedWeatherExtendedStateF::fn_800AFFD8()
{
    if (field18 <= 0.0f)
    {
        bool available = true;
        for (int i = 0; i < 8; i++)
        {
            if (gNPCManager->fn_801AA528(i)->mState != -1)
                available = false;
            if (gNPCManager->fn_801AA528(i)->mState == 1)
                fn_801B2E64(gNPCManager->fn_801AA528(i), false);
        }
        if (available == true)
            return 1.0f;
    }
    return 0.0f;
}

extern "C" float lbl_806DC004;
extern "C" float lbl_806DC008;

void UnidentifiedWeatherExtendedStateF::fn_800AA6B4(float dt)
{
    UnidentifiedWeatherState::fn_800AA6B4(dt);
    if (!field0C)
    {
        if (!field1C)
            fn_800AF6F0();
        if (field18 > 0.0f)
            field18 -= dt;
        else if (fn_800AFFD8() == 1.0f)
            fn_800AA7C4(false);
        if (field14 > 0.0f)
        {
            field14 -= dt;
            if (field14 < 0.0f)
            {
                fn_800AFA7C(-1);
                field14 = lbl_806DC004 + nlRandomf(lbl_806DC008 - lbl_806DC004);
            }
        }
        if (field18 > 0.0f && fn_800AFBB8() == true)
            field14 = lbl_806DC004 + nlRandomf(lbl_806DC008 - lbl_806DC004);
    }
}

extern "C" float lbl_806DC00C;

bool UnidentifiedWeatherExtendedStateF::fn_800AFBB8()
{
    nlVector3 origin = {-9.0f, 3.0f, 0.0f};
    for (int i = 0; i < 8; i++)
    {
        if (field20[i] && field20[i]->mState == 1)
        {
            int side = (int)floorf((i + 1) / 4.0f);
            nlVector3 position = origin;
            position.x += 6.0f * ((i + 1) % 4);
            if (side == 1)
                position.y *= -1.0f;
            float radiusSquared = lbl_806DC00C * lbl_806DC00C;
            bool nearby = false;
            for (int team = 0; team < 2 && !nearby; team++)
            {
                for (int player = 0; player < 4; player++)
                {
                    nlVector3 delta;
                    nlVec3Sub(delta, g_pTeams[team]->GetFielder(player)->mUnidentified024.m_v3Position, position);
                    if (nlVec3LengthSquared(delta) < radiusSquared)
                    {
                        nearby = true;
                        break;
                    }
                }
            }
            if (nearby == true)
            {
                fn_800AFA7C(i);
                return true;
            }
        }
    }
    return false;
}

static u16 lbl_806DC01C = 0xFFFF;

void UnidentifiedWeatherExtendedStateF::fn_800AF404()
{
    if (lbl_806E0E1C || GameInfoManager::Instance()->IsRule0x4Equal1())
        return;
    field1C = false;
}

extern "C" int fn_800B04B4(UnidentifiedWeatherExtendedStateF*);
extern "C" nlVector4 fn_800B04BC(UnidentifiedWeatherExtendedStateF*, int, bool);

void UnidentifiedWeatherExtendedStateF::fn_800AF6F0()
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
            nlVector3 position = {patch.x, patch.y, patch.z};
            lbl_806E12C8->fn_801743A8(11, 0, position, v3Zero,
                patch.w, patch.w, 99999.0f);
        }
    }
    field1C = true;
}

void UnidentifiedWeatherExtendedStateF::fn_800AF3B4()
{
    if (lbl_806E0E1C || GameInfoManager::Instance()->IsRule0x4Equal1())
        return;
    fn_800AF6F0();
}

void UnidentifiedWeatherExtendedStateF::fn_800AA7D0(void* context, DebugWriteCache* cache)
{
    UnidentifiedWeatherState::fn_800AA7D0(context, cache);
    if (lbl_806DC01C == 0xFFFF)
    {
        lbl_806DC01C = cache->BeginType("SandTombWeather");
        cache->AddField(8, gDebugFieldTypes[8].size, 0, "m_NumActiveThwomps");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field14 - (u8*)&field10, "m_ThwompDropTimer");
        cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&field18 - (u8*)&field10, "m_ThwompSpawnTimer");
        cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&field1C - (u8*)&field10, "m_bSandPatchesCreated");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DC01C, &field10, context);
    cache->WriteData(lbl_806DC01C, &field10,
        sizeof(UnidentifiedWeatherExtendedStateF) - sizeof(UnidentifiedWeatherState) - sizeof(field20));
}

extern "C" void* fn_800AFEF4(unsigned long size, unsigned int alignment, bool fromEnd)
{
    return nlMalloc(size, alignment, fromEnd);
}

float UnidentifiedWeatherState::fn_800AFFD8()
{
    return 0.0f;
}





extern "C" float lbl_806DBFAC;
extern "C" float lbl_806DBFB0;
extern "C" float lbl_806DBFB4;
extern "C" float lbl_806DBFB8;

extern "C" void fn_800AD628(UnidentifiedEventData24* event)
{
    PhysicsPatch* patch = event->mUnidentified10;
    if (patch->m_Type == 8 && patch->m_Velocity.z < 0.0f)
    {
        UnidentifiedWeatherState* state = g_pGame->mUnidentified10DC->fn_800AA060(5);
        if (state == 0 || state->field08 != 1)
            return;
        nlVector3 position = patch->GetPosition();
        position.z = 0.0f;
        lbl_806E12C8->fn_801743A8(9, 0, position, v3Zero,
            lbl_806DBFAC, lbl_806DBFB0, lbl_806DBFB8)->fn_80173B10(lbl_806DBFB4);
        event->mUnidentified10->Unknown0();
        PlaySound(11, 0xC15AA25B, 0, 0);
    }
}

extern "C" float lbl_806DBFC8;

void UnidentifiedWeatherExtendedStateE::fn_800AEDAC(bool initialize)
{
    if (initialize)
    {
        field10 = lbl_806DBFC8;
    }
    field30 = -1;
    field34 = -1;
    field3C = -1;
    field38 = 0;
    field14 = 0.0f;
    field18 = 0.0f;
    field1C = 0.0f;
    field20 = 0.0f;
    field24 = 0.0f;
    field28 = 0.0f;
    field2C = false;
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

void UnidentifiedWeatherExtendedStateF::fn_800AFA7C(int index)
{
    if (index == -1)
    {
        float distance = 999999.9f;
        for (int i = 0; i < 8; i++)
        {
            if (field20[i] && field20[i]->mState == 1)
            {
                nlVector3 position = *fn_801B327C(field20[i]);
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
    if (index != -1 && field20[index] && field20[index]->mState == 1)
        fn_801B2EAC(field20[index], 2);
}

void UnidentifiedWeatherExtendedStateF::fn_800AA7C4(bool initialize)
{
    field10 = 0;
    field18 = 0.0f;
    field14 = 0.0f;
    field1C = false;
    for (int i = 0; i < 8; i++)
    {
        if (field20[i])
            fn_801B2E64(field20[i], initialize);
        field20[i] = 0;
    }
    UnidentifiedWeatherState::fn_800AA7C4(initialize);
}

void UnidentifiedWeatherExtendedStateF::fn_800AA8C8()
{
    UnidentifiedWeatherExtendedStateF::fn_800AA7C4(false);
    UnidentifiedWeatherState::fn_800AA8C8();
}

extern "C" float lbl_806DBFF4;
extern "C" float lbl_806DBFF8;
extern "C" float lbl_806DC004;
extern "C" float lbl_806DC008;

void UnidentifiedWeatherExtendedStateF::fn_800AA6A8()
{
    fn_800AA7C4(false);
    UnidentifiedWeatherState::fn_800AA6A8();
    fn_800AF80C();
    field1C = false;
    field18 = lbl_806DBFF4 + nlRandomf(lbl_806DBFF8);
    field14 = lbl_806DC004 + nlRandomf(lbl_806DC008 - lbl_806DC004);
}

extern "C" int lbl_806DBFFC;
extern "C" int lbl_806DC000;

void UnidentifiedWeatherExtendedStateF::fn_800AF80C()
{
    int range = lbl_806DC000 - lbl_806DBFFC;
    float random = range * nlRandomf(1.0f);
    int count = lbl_806DBFFC + (int)(random + (random < 0.0f ? -0.5f : 0.5f));
    count = nlMin(count, 8);
    nlVector3 position;
    ThwompObject* thwomp;
    nlVector3 origin = {-9.0f, 3.0f, 0.0f};
    for (int i = 0; i < count; i++)
    {
        int index = -1;
        while (index == -1 || field20[index] != 0)
        {
            float random = 8.0f * nlRandomf(1.0f);
            index = (int)(random + (random < 0.0f ? -0.5f : 0.5f));
        }
        thwomp = gNPCManager->fn_801AA528(-1);
        field20[index] = thwomp;
        if (thwomp)
        {
            position = origin;
            int side = (int)floorf((index + 1) / 4.0f);
            position.x += 6.0f * ((index + 1) % 4);
            if (side == 1)
                position.y *= -1.0f;
            fn_801B2DF4(thwomp, position.x, position.y);
            field10++;
        }
    }
}

extern "C" float lbl_806DBF3C;
extern "C" float lbl_806DBF40;
extern "C" float lbl_806DBF44;

nlVector3 UnidentifiedWeatherExtendedStateB::fn_800ABB38(const nlVector3& position, const nlVector3& velocity, float value, float radius)
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
    if (lbl_806DBF3C > 0.0f)
    {
        fn_800ABF68(start, end, displacement, g_pTeams[0]->GetGoalie()->mUnidentified024.m_v3Position, lbl_806DBF3C + radius);
        fn_800ABF68(start, end, displacement, g_pTeams[1]->GetGoalie()->mUnidentified024.m_v3Position, lbl_806DBF3C + radius);
    }
    if (lbl_806DBF40 > 0.0f)
    {
        nlVector3 goal0 = { 0.0f, 0.0f, 0.0f };
        goal0.x = cField::GetGoalLineX(0U);
        nlVector3 goal1 = { 0.0f, 0.0f, 0.0f };
        goal1.x = cField::GetGoalLineX(1U);
        fn_800ABF68(start, end, displacement, goal0, lbl_806DBF40 + radius);
        fn_800ABF68(start, end, displacement, goal1, lbl_806DBF40 + radius);
    }
    if (lbl_806DBF44 > 0.0f)
    {
        nlVector3 corner0 = { 21.34f, 12.73f, 0.0f };
        nlVector3 corner1 = { -21.34f, 12.73f, 0.0f };
        nlVector3 corner2 = { -21.34f, -12.73f, 0.0f };
        nlVector3 corner3 = { 21.34f, -12.73f, 0.0f };
        fn_800ABF68(start, end, displacement, corner0, lbl_806DBF44 + radius);
        fn_800ABF68(start, end, displacement, corner1, lbl_806DBF44 + radius);
        fn_800ABF68(start, end, displacement, corner2, lbl_806DBF44 + radius);
        fn_800ABF68(start, end, displacement, corner3, lbl_806DBF44 + radius);
    }
    return displacement;
}

void UnidentifiedWeatherExtendedStateB::fn_800ABF68(nlVector3& start, nlVector3& end, nlVector3& displacement, const nlVector3& position, float radius)
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
        nlVec3Scale(direction, direction, nlRecipSqrt(nlVec3LengthSquared(direction), false));
        nlVec3Scale(offset, direction, radius - distance);
        nlVec3Add(displacement, displacement, offset);
        nlVec3Add(start, start, offset);
        nlVec3Add(end, end, offset);
    }
}
