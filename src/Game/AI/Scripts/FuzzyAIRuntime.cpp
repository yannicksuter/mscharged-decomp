#include "Game/Ball.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/Variant.h"
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
    u8 mCollection[0x01];
};

struct UnidentifiedFielderIterator
{
    unsigned int mCurrent;
    unsigned int mEnd;
    cTeam* mTeam;
    cFielder* mSkip;
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
extern "C" void fn_80312754();
extern "C" void fn_80313D84();
extern "C" void fn_80314444();
extern "C" void fn_803140CC();
extern "C" bool fn_8030F030(void*, int);
extern "C" Variant* fn_8030F060(void*, int);
extern "C" void fn_80311CB8();
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

extern "C" void fn_800E30A8()
{
    fn_8002E198();
}

extern "C" void fn_800E30AC()
{
    fn_800A695C();
}

extern "C" void fn_800E31A0()
{
    fn_80311CB8();
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

extern "C" void fn_800E34D0()
{
    fn_80312754();
}

extern "C" void fn_800E34D4()
{
    fn_80313D84();
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

extern "C" float fn_800E7F48(bool value)
{
    return value ? 1.0f : 0.0f;
}

extern "C" float fn_800E7EF4(
    void*, UnidentifiedRuntimeCollection* value)
{
    if (fn_8030F030(value->mCollection, 4))
    {
        return fn_8030F060(value->mCollection, 4)->mData.f;
    }

    return 0.0f;
}

extern "C" void* fn_800E8088(void*, void* value)
{
    return value;
}

extern "C" unsigned long fn_800E8CB0(void*, Variant* value)
{
    return value->mData.u;
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

extern "C" void fn_800E92E4(
    Unidentified8002E1A4Result* state, const char* name)
{
    UnidentifiedStringHash value(name);
    state->mUnidentified00C = value.mHash;
    state->mUnidentified010 = value.mUnidentified004;
}
