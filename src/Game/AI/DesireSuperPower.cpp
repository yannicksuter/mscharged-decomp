#include "Game/AI/DesireSuperPower.h"

#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Fielder.h"
#include "Game/Event.h"
#include "Game/Game.h"
#include "NL/nlAVLTree.h"
#include "NL/nlMath.h"
#include <stdlib.h>

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

extern "C" DebugFieldType lbl_80533C98[];
extern "C" UnidentifiedEventRegistry* lbl_806E1D90;
extern "C" cGame* lbl_806E0C94;
extern "C" void fn_800C2C18(UnidentifiedDesireUpdate*, int);
extern "C" void fn_800C93A4(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CA07C(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CA57C(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CADBC(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CB2BC(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CB7A8(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CBF64(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CC720(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CCC0C(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CD61C(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800D01A0(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800D0EAC(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800D1140(void*);
extern "C" void fn_800D12E8(void*);
extern "C" eCharacterClass fn_800D1440(const cCharacter*);
extern "C" unsigned short fn_800D1448(const cCharacter*);
extern "C" const nlVector3* fn_800D1450(const cCharacter*);
extern "C" bool fn_800D1458(const cGame*);
extern "C" unsigned short fn_80338EBC(
    DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);

static unsigned short sDesireSuperPowerType = 0xFFFF;

static inline void UnidentifiedRegisterEventCallback(
    const char* name, void (*callback)(void*))
{
    Function<void*> function(callback);
    unsigned int hash = fn_802B289C(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event;
    if (foundEvent != 0)
    {
        event = *foundEvent;
    }
    else
    {
        event = 0;
    }
    ((UnidentifiedTypedEvent<void>*)event)->Add(function, 0, -1);
}

/**
 * Offset/Address/Size: 0x0 | 0x800C86FC | size: 0x60
 */
DesireSuperPower::DesireSuperPower()
    : Desire(23, UnidentifiedStateTransition(lbl_806E20B8))
    , mpDKShockAvoidable(0)
    , mpTarget(0)
{
}

/**
 * Offset/Address/Size: 0x60 | 0x800C875C | size: 0x274
 */
void DesireSuperPower::UnidentifiedSetContext(
    UnidentifiedDesireContext* context)
{
    Desire::UnidentifiedSetContext(context);

    if (mUnidentifiedFielder->m_eCharacterClass == YOSHI)
    {
        UnidentifiedRegisterEventCallback(
            "CollisionPatchGround", fn_800D1140);
        UnidentifiedRegisterEventCallback(
            "CollisionPatchPlayer", fn_800D1140);
        UnidentifiedRegisterEventCallback(
            "CollisionPatchWall", fn_800D12E8);
    }
}

/**
 * Offset/Address/Size: 0x87C | 0x800C8F78 | size: 0x174
 */
void DesireSuperPower::UnidentifiedUpdate(
    UnidentifiedDesireUpdate* update, float fDeltaT)
{
    if (!fn_800D1458(lbl_806E0C94))
    {
        fn_800C2C18(update, 1);
        return;
    }

    switch (fn_800D1440(mUnidentifiedFielder))
    {
    case BIRDO:
        fn_800CBF64(this, update, fDeltaT);
        break;
    case DAISY:
        fn_800C93A4(this, update, fDeltaT);
        break;
    case DONKEYKONG:
        fn_800CA57C(this, update, fDeltaT);
        break;
    case HAMMERBROS:
        fn_800CB2BC(this, update, fDeltaT);
        break;
    case KOOPA:
        fn_800CB7A8(this, update, fDeltaT);
        break;
    case LUIGI:
        fn_800CC720(this, update, fDeltaT);
        break;
    case MARIO:
        fn_800CD61C(this, update, fDeltaT);
        break;
    case PEACH:
        fn_800D01A0(this, update, fDeltaT);
        break;
    case TOAD:
        fn_800D0EAC(this, update, fDeltaT);
        break;
    case WALUIGI:
        fn_800CA07C(this, update, fDeltaT);
        break;
    case WARIO:
        fn_800CADBC(this, update, fDeltaT);
        break;
    case YOSHI:
        fn_800CCC0C(this, update, fDeltaT);
        break;
    }
}

/**
 * Offset/Address/Size: 0x8D44 | 0x800D1440 | size: 0x8
 */
extern "C" eCharacterClass fn_800D1440(const cCharacter* character)
{
    return character->m_eCharacterClass;
}

/**
 * Offset/Address/Size: 0x8D4C | 0x800D1448 | size: 0x8
 */
extern "C" unsigned short fn_800D1448(const cCharacter* character)
{
    return character->m_aActualFacingDirection;
}

/**
 * Offset/Address/Size: 0x8D54 | 0x800D1450 | size: 0x8
 */
extern "C" const nlVector3* fn_800D1450(const cCharacter* character)
{
    return &character->m_v3Position;
}

/**
 * Offset/Address/Size: 0x8D5C | 0x800D1458 | size: 0x20
 */
extern "C" bool fn_800D1458(const cGame* game)
{
    return game->m_eGameState == 5 || game->m_eGameState == 6;
}

/**
 * Offset/Address/Size: 0x9540 | 0x800D1C3C | size: 0x10
 */
void nlVector3::Set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

/**
 * Offset/Address/Size: 0x9550 | 0x800D1C4C | size: 0x34
 */
extern "C" nlVector3* fn_800D1C4C(
    nlVector3* result, const nlVector3* first,
    const nlVector3* second)
{
    nlVec3Sub(*result, *first, *second);
    return result;
}

/**
 * Offset/Address/Size: 0x9584 | 0x800D1C80 | size: 0x4C
 */
extern "C" float fn_800D1C80(
    const nlVector2* first, const nlVector2* second)
{
    nlVector2 delta = {
        first->x - second->x,
        first->y - second->y,
    };
    return nlVec2Length(delta);
}

/**
 * Offset/Address/Size: 0x95D0 | 0x800D1CCC | size: 0x38
 */
extern "C" unsigned short fn_800D1CCC(float y, float x)
{
    return (unsigned short)(int)(10430.378f * nlATan2f(y, x));
}

/**
 * Offset/Address/Size: 0x9608 | 0x800D1D04 | size: 0xC
 */
extern "C" short fn_800D1D04(
    unsigned short first, unsigned short second)
{
    return nlAngleDiff(first, second);
}

/**
 * Offset/Address/Size: 0x9614 | 0x800D1D10 | size: 0x14
 */
extern "C" unsigned short fn_800D1D10(short angle)
{
    return angle < 0 ? -angle : angle;
}

/**
 * Offset/Address/Size: 0x9628 | 0x800D1D24 | size: 0x10
 */
extern "C" int fn_800D1D24(int value)
{
    return value < 0 ? -value : value;
}

/**
 * Offset/Address/Size: 0x9638 | 0x800D1D34 | size: 0x8
 */
extern "C" eVariantType fn_800D1D34(const Variant* value)
{
    return value->mType;
}

/**
 * Offset/Address/Size: 0x9658 | 0x800D1D54 | size: 0x110
 */
void DesireSuperPower::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field
        = fn_80338EBC(cache, "DesireSuperPower");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    fn_80338F88(cache, 15, lbl_80533C98[15].size,
        (u8*)&mpDKShockAvoidable - (u8*)&mvDesiredPosition,
        "mpDKShockAvoidable");
    fn_80338F88(cache, 15, lbl_80533C98[15].size,
        (u8*)&mpTarget - (u8*)&mvDesiredPosition, "mpTarget");
    fn_80338F78(cache);
}

/**
 * Offset/Address/Size: 0x9768 | 0x800D1E64 | size: 0xC4
 */
void DesireSuperPower::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireSuperPowerType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireSuperPowerType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = fn_8033930C(cache, sDesireSuperPowerType,
        (u8*)this + offset, sizeof(DesireSuperPower) - offset);
    if (data != NULL)
    {
        DesireSuperPower* copy
            = (DesireSuperPower*)((u8*)data - offset);
        *(int*)&copy->mpDKShockAvoidable = -1;
        cFielder* target = mpTarget;
        *(int*)&copy->mpTarget
            = target == NULL ? -1 : target->mUnidentified120;
        fn_80339450(cache, sDesireSuperPowerType, data, context);
    }
}

/**
 * Offset/Address/Size: 0x982C | 0x800D1F28 | size: 0x5C
 */
DesireSuperPower::~DesireSuperPower()
{
}
