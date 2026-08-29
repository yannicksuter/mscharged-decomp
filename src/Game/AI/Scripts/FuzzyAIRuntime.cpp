#include "Game/Ball.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/Variant.h"
#include "Game/InterpreterCore.h"
#include "Game/Team.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"
#include "types.h"

struct UnidentifiedRuntimeFlags
{
    u8 mUnidentified000[0x24];
    u32 mFlags;
};

struct UnidentifiedRuntimeCollection
{
    u8 mUnidentified000[0x18];
    UnidentifiedVariantCollection mCollection;
};

class UnidentifiedFuzzyRuntimeBase : public InterpreterCore
{
public:
    UnidentifiedFuzzyRuntimeBase(void*);
    virtual ~UnidentifiedFuzzyRuntimeBase();
    virtual void DoFunctionCall(unsigned int) = 0;
    virtual bool UnidentifiedVirtual2(FunctionEntryPoint*, unsigned int, u32, u32, u32, u32);
    virtual float UnidentifiedVirtual3(float, float);
    virtual float UnidentifiedVirtual4(float, float);
    virtual float UnidentifiedVirtual5(float);
    virtual float UnidentifiedVirtual6(float);
    virtual float UnidentifiedVirtual7(float, float, float);
    virtual float UnidentifiedVirtual8();
    virtual void UnidentifiedVirtual9();
    virtual float UnidentifiedVirtual10(float);
    virtual float UnidentifiedVirtual11();
    virtual void UnidentifiedVirtual12();
    virtual UnidentifiedVariant_80054AB8* UnidentifiedReturn(
        UnidentifiedVariant_80054AB8*, float);
    virtual void UnidentifiedVirtual14(void*, void*, void*);
    virtual void UnidentifiedVirtual15();

    Variant* mValue;
    void** mCollection;
    u8 mUnidentified034[0x24];
    int mUnidentified058;
    u8 mUnidentified05C[0x0C];
};

class UnidentifiedFuzzyRuntime : public UnidentifiedFuzzyRuntimeBase
{
public:
    UnidentifiedFuzzyRuntime();
    virtual ~UnidentifiedFuzzyRuntime();
    virtual void DoFunctionCall(unsigned int);
    virtual float UnidentifiedVirtual8();
    virtual void UnidentifiedVirtual9();
    virtual void UnidentifiedVirtual12();
    virtual void UnidentifiedVirtual15();
};

struct UnidentifiedFielderIterator
{
    unsigned int mCurrent;
    unsigned int mEnd;
    cTeam* mTeam;
    cFielder* mSkip;
};

struct UnidentifiedRuntimeFielderReference
{
    int mIndex;
    u8 mUnidentified004[0x04];
    cTeam* mTeam;
};

struct UnidentifiedRuntimeTypeEntry
{
    UnidentifiedRuntimeTypeEntry(const char* name, int type)
        : mType(type)
        , mHash(nlStringLowerHash(name))
    {
    }

    int mType;
    unsigned long mHash;
    UnidentifiedRuntimeTypeEntry* next;
};

struct UnidentifiedRuntimeTypeList
{
    UnidentifiedRuntimeTypeEntry* mHead;
    UnidentifiedRuntimeTypeEntry* mTail;

    void AddEnd(UnidentifiedRuntimeTypeEntry* entry)
    {
        nlListAddEnd(&mHead, &mTail, entry);
    }
};

extern "C" void fn_8002E198();
extern "C" void fn_800A695C();
extern "C" void fn_80314444();
extern "C" void fn_803140CC();
extern "C" bool fn_800A3350(
    void*, unsigned long, float*, cFielder*);
extern "C" void* fn_800A636C(cTeam*);
extern "C" void* fn_800A3404(void*);
extern "C" void fn_80310034(void*, void*, int);
extern "C" int fn_80312208(void*);
extern "C" void* fn_80312E0C(void*, const Variant&);
extern "C" int fn_802DF9FC(InterpreterCore*);
extern "C" void fn_800B6A1C(
    UnidentifiedVariant_80054AB8*, int, const Variant&);
extern "C" void* lbl_806E0C94;
extern "C" UnidentifiedRuntimeTypeList lbl_806E20B0;

char lbl_80503EEC[] = "art/Scripts/FuzzyAI.byte_code";
char lbl_80503F0C[] = "Direction";
char lbl_80503F18[] = "Distance";
char* lbl_806DC3F0 = lbl_80503EEC;
char lbl_806DC3F4[] = "Speed";
char lbl_806DC3FC[] = "Target";
char lbl_806DC408[] = "Powerup";
char lbl_806DC410[] = "Lob";

SlotPool<UnidentifiedFielderIterator> lbl_8056DBB0(16, 16);

UnidentifiedFuzzyRuntime::UnidentifiedFuzzyRuntime()
    : UnidentifiedFuzzyRuntimeBase(0)
{
    if (lbl_806E20B0.mHead == 0)
    {
        UnidentifiedVirtual15();
    }
}

UnidentifiedFuzzyRuntime::~UnidentifiedFuzzyRuntime()
{
    lbl_8056DBB0.FreeBlocks();
}

extern "C" void fn_800E30A8()
{
    fn_8002E198();
}

extern "C" void fn_800E30AC()
{
    fn_800A695C();
}

void UnidentifiedFuzzyRuntime::UnidentifiedVirtual15()
{
    UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual15();
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DC3F4, 13));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DC3FC, 14));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DC408, 15));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DC410, 16));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_80503F0C, 17));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_80503F18, 18));
}

extern "C" const char* fn_800E3198()
{
    return lbl_806DC3F0;
}

float UnidentifiedFuzzyRuntime::UnidentifiedVirtual8()
{
    float result = UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual8();
    void* value = 0;

    switch (mValue->GetType())
    {
    case FT_PLAYER:
        value = fn_800A636C(mValue->mData.pPlayer->m_pTeam);
        break;
    case FT_TEAM:
        value = fn_800A636C(mValue->mData.pTeam);
        break;
    case FT_GAME:
        value = 0;
        break;
    }

    if (value != 0)
    {
        void* collection = *mCollection;
        fn_80310034(collection, fn_800A3404(value), 4);
    }

    return result;
}

void UnidentifiedFuzzyRuntime::UnidentifiedVirtual9()
{
    UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual9();
}

void UnidentifiedFuzzyRuntime::UnidentifiedVirtual12()
{
    UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual12();
}

extern "C" cPlayer* fn_800E34D8()
{
    return g_pBall->m_pOwner;
}

extern "C" cBall* fn_800E34E4()
{
    return g_pBall;
}

extern "C" void* fn_800E34EC()
{
    return lbl_806E0C94;
}

extern "C" float fn_800E34F4(
    UnidentifiedFuzzyRuntime* runtime, unsigned long hash)
{
    cFielder* fielder = 0;
    void* teamContext = 0;

    switch (runtime->mValue->GetType())
    {
    case FT_PLAYER:
    {
        cPlayer* player = runtime->mValue->mData.pPlayer;
        teamContext = fn_800A636C(player->m_pTeam);
        if (player->m_eClassType == FIELDER)
        {
            fielder = static_cast<cFielder*>(player);
        }
        break;
    }
    case FT_TEAM:
    {
        cTeam* team = runtime->mValue->mData.pTeam;
        teamContext = fn_800A636C(team);
        fielder = team->GetCaptain();
        break;
    }
    }

    float result = 1.0f;
    fn_800A3350(teamContext, hash, &result, fielder);
    return result;
}

extern "C" void fn_800E35D4(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E3700(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E382C(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E3958(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E3A84(
    void*, cPlayer* value, void* descriptor,
    UnidentifiedRuntimeCollection* runtime)
{
    int index = fn_80312208(descriptor);
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    runtime->mCollection.Set(index, variant);
}

extern "C" void fn_800E3B34(
    void*, cBall* value, void* descriptor,
    UnidentifiedRuntimeCollection* runtime)
{
    int index = fn_80312208(descriptor);
    FuzzyVariant variant;
    variant.mType = FT_BALL;
    variant.mData.pointer = value;
    runtime->mCollection.Set(index, variant);
}

extern "C" void* fn_800E3BE4(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3BEC(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3BF4(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3BFC(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3C04(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3C0C(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3C14(void*, Variant* value)
{
    return value != 0 ? value->mData.pointer : 0;
}

extern "C" void* fn_800E3C2C(void*, Variant* value)
{
    return value != 0 ? value->mData.pointer : 0;
}

extern "C" void* fn_800E3C44(void*, Variant* value)
{
    return value != 0 ? value->mData.pointer : 0;
}

extern "C" void* fn_800E3C5C(void*, Variant* value)
{
    return value != 0 ? value->mData.pointer : 0;
}

extern "C" unsigned long fn_800E3FDC(const char* value)
{
    return nlStringHash(value);
}

extern "C" void fn_800DFF1C();
extern "C" void fn_800DFF60();
extern "C" void fn_800E00F8();
extern "C" void fn_800E0034();

extern "C" void fn_800E3FE0()
{
    fn_800DFF1C();
}

extern "C" void fn_800E3FE4()
{
    fn_800DFF60();
}

extern "C" void fn_800E3FE8()
{
    fn_800E00F8();
}

extern "C" void fn_800E3FEC()
{
    fn_800E0034();
}

extern "C" void fn_800E3EF8(
    void*, UnidentifiedFielderIterator* entry)
{
    lbl_8056DBB0.Free(entry);
}

extern "C" bool fn_800E3EDC(
    void*, UnidentifiedFielderIterator* iterator)
{
    return iterator->mCurrent < iterator->mEnd;
}

extern "C" UnidentifiedFielderIterator* fn_800E3C74(
    void*, cTeam* team)
{
    UnidentifiedFielderIterator* iterator = 0;
    lbl_8056DBB0.Allocate(iterator);
    iterator->mCurrent = 0;
    iterator->mEnd = 4;
    iterator->mTeam = team;
    iterator->mSkip = 0;
    return iterator;
}

extern "C" UnidentifiedFielderIterator* fn_800E3D00(
    void*, cFielder* fielder)
{
    cTeam* team = fielder->m_pTeam->GetOtherTeam();
    UnidentifiedFielderIterator* iterator = 0;
    lbl_8056DBB0.Allocate(iterator);
    iterator->mCurrent = 0;
    iterator->mEnd = 4;
    iterator->mTeam = team;
    iterator->mSkip = 0;
    return iterator;
}

extern "C" UnidentifiedFielderIterator* fn_800E3D98(
    void*, cFielder* fielder)
{
    UnidentifiedFielderIterator* iterator = 0;
    cTeam* team = fielder->m_pTeam;
    lbl_8056DBB0.Allocate(iterator);
    iterator->mCurrent = 0;
    iterator->mEnd = 4;
    iterator->mTeam = team;
    iterator->mSkip = fielder;

    if (iterator->mCurrent < iterator->mEnd && iterator->mSkip != 0
        && iterator->mTeam->GetFielder(iterator->mCurrent)
               == iterator->mSkip)
    {
        ++iterator->mCurrent;
    }

    return iterator;
}

extern "C" UnidentifiedFielderIterator* fn_800E3E68(
    void*, UnidentifiedFielderIterator* iterator)
{
    ++iterator->mCurrent;
    if (iterator->mCurrent < iterator->mEnd && iterator->mSkip != 0
        && iterator->mTeam->GetFielder(iterator->mCurrent)
               == iterator->mSkip)
    {
        ++iterator->mCurrent;
    }
    return iterator;
}

extern "C" cFielder* fn_800E3F10(
    void*, UnidentifiedFielderIterator* iterator)
{
    return iterator->mTeam->GetFielder(iterator->mCurrent);
}

extern "C" cFielder* fn_800E3F1C(
    void*, UnidentifiedFielderIterator* iterator)
{
    return iterator->mTeam->GetFielder(iterator->mCurrent);
}

extern "C" UnidentifiedFielderInput* fn_800E3F28(
    void*, UnidentifiedFielderIterator* iterator)
{
    cFielder* fielder = iterator->mTeam->GetFielder(iterator->mCurrent);
    return fielder->mUnidentified428;
}

extern "C" bool fn_800E7EB4(UnidentifiedRuntimeFlags* value)
{
    return ((value->mFlags >> 29) & 3) == 3;
}

extern "C" float fn_800E7ECC(void*, Variant* value)
{
    return value->mData.f;
}

extern "C" unsigned long fn_800E7ED4(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" unsigned long fn_800E7EDC(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" unsigned long fn_800E7EE4(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" float fn_800E7EEC(void*, Variant* value)
{
    return value->mData.f;
}

extern "C" void fn_800E7F60(
    UnidentifiedFuzzyRuntime* runtime, bool value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_BOOL, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" float fn_800E7F48(bool value)
{
    return value ? 1.0f : 0.0f;
}

extern "C" float fn_800E7EF4(
    void*, UnidentifiedRuntimeCollection* value)
{
    if (value->mCollection.IsSet(4))
    {
        return value->mCollection.Get(4)->mData.f;
    }

    return 0.0f;
}

UnidentifiedVariant_80054AB8* UnidentifiedFuzzyRuntimeBase::UnidentifiedReturn(
    UnidentifiedVariant_80054AB8* value, float)
{
    return value;
}

extern "C" void fn_800E8090(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E81BC(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E82E8(
    UnidentifiedFuzzyRuntime* runtime, float value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_FLOAT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E8414(
    UnidentifiedFuzzyRuntime* runtime, float value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_FLOAT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E8540(
    UnidentifiedFuzzyRuntime* runtime,
    UnidentifiedVariant_80054AB8* value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E8B80(
    UnidentifiedFuzzyRuntime* runtime, unsigned long value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(FT_U32, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" unsigned long fn_800E8CB0(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" void fn_800E8CB8(
    void*, cPlayer* value, void* descriptor,
    UnidentifiedRuntimeCollection* runtime)
{
    int index = fn_80312208(descriptor);
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    runtime->mCollection.Set(index, variant);
}

extern "C" void fn_800E8CAC()
{
    fn_80314444();
}

extern "C" void fn_800E8D68()
{
    fn_803140CC();
}

extern "C" void fn_800E8D6C()
{
    fn_803140CC();
}

extern "C" void fn_800E8D70()
{
    fn_803140CC();
}

extern "C" void fn_800E8D74()
{
    fn_803140CC();
}

extern "C" void fn_800E8D78(
    void*, UnidentifiedRuntimeFielderReference* value,
    void* descriptor, UnidentifiedRuntimeCollection* runtime)
{
    cFielder* fielder = value->mTeam->GetFielder(value->mIndex);
    int index = fn_80312208(descriptor);
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = fielder;
    runtime->mCollection.Set(index, variant);
}

extern "C" void fn_800E8E38(
    UnidentifiedFuzzyRuntime* runtime, cPlayer* value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E8F8C(
    UnidentifiedFuzzyRuntime* runtime,
    UnidentifiedRuntimeFielderReference* value, float confidence)
{
    cFielder* fielder = value->mTeam->GetFielder(value->mIndex);
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(fielder);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void* fn_800E90EC(void* runtime, cPlayer* value)
{
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    return fn_80312E0C(runtime, variant);
}

extern "C" void* fn_800E9194(void* runtime, cPlayer* value)
{
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    return fn_80312E0C(runtime, variant);
}

extern "C" void* fn_800E923C(void* runtime, cTeam* value)
{
    FuzzyVariant variant;
    variant.mType = FT_TEAM;
    variant.mData.pTeam = value;
    return fn_80312E0C(runtime, variant);
}

extern "C" void fn_800E92E4(
    Unidentified8002E1A4Result* state, const char* name)
{
    UnidentifiedStringHash value(name);
    state->mUnidentified00C = value.mHash;
    state->mUnidentified010 = value.mUnidentified004;
}
