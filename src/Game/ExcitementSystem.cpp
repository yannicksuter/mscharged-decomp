#include "Game/ExcitementSystem.h"

#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/GameEventQueue.h"
#include "NL/nlAVLTree.h"
#include "NL/nlBind.h"
#include "NL/nlDebug.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

struct UnidentifiedEventData_80066748
{
    u8 mUnidentified000[0x10];
    u8 mUnidentified010;
};

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

extern "C" UnidentifiedEventRegistry* lbl_806E1D90;

namespace
{
char lbl_80511AD8[] = "art/Scripts/ExcitementSystem.byte_code";
const char lbl_806E4EA0[] = "Main";
} // namespace

inline ExcitementSystem::ExcitementSystem()
    : InterpreterCore(100)
    , mUnidentified028(200.0f)
    , mUnidentified02C(0)
    , mUnidentified02E(0)
    , mByteCode(0)
{
    fn_801967DC();

    if (mByteCode != 0)
    {
        nlFree(mByteCode);
        mByteCode = 0;
    }

    unsigned long fileSize = 0;
    mByteCode = nlLoadEntireFile(lbl_80511AD8,
        &fileSize,
        0x20,
        AllocateStart,
        0,
        0,
        0);
    LoadByteCode(mByteCode);
    CallFunction(nlStringHash(lbl_806E4EA0));
}

ExcitementSystem& ExcitementSystem::fn_80196644()
{
    static ExcitementSystem instance;
    return instance;
}

void ExcitementSystem::fn_801967DC()
{
    for (int i = 0; i < 130; i++)
    {
        mUnidentified030[i] = 0;
    }
    for (int i = 0; i < 178; i++)
    {
        mUnidentified0B2[i] = 0;
    }
    mUnidentified164[0] = 0;
    mUnidentified164[1] = 0;
    mUnidentified164[2] = 0;
    mUnidentified164[3] = 0;
}

void ExcitementSystem::fn_80196924()
{
    {
        Function<UnidentifiedEventData_80066748*> callback(
            Bind<void>(MemFun(&ExcitementSystem::fn_80196D30), this, placeholder0));
        unsigned int hash = fn_802B289C("AttackSuccess", -1);
        UnidentifiedEventBase** foundEvent = 0;
        lbl_806E1D90->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventData_80066748>*)event)
            ->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_80067110*> callback(
            Bind<void>(MemFun(&ExcitementSystem::fn_80196D64), this, placeholder0));
        unsigned int hash = fn_802B289C("LightningStrike", -1);
        UnidentifiedEventBase** foundEvent = 0;
        lbl_806E1D90->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventData_80067110>*)event)
            ->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData07*> callback(
            Bind<void>(MemFun(&ExcitementSystem::fn_80196D8C), this, placeholder0));
        unsigned int hash = fn_802B289C("CollisionBallGoalpost", -1);
        UnidentifiedEventBase** foundEvent = 0;
        lbl_806E1D90->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventData07>*)event)
            ->Add(callback, 0, -1);
    }
}

void ExcitementSystem::fn_80196D30(
    UnidentifiedEventData_80066748* event)
{
    if (event->mUnidentified010 == 1 && mUnidentified164[1] != 0)
    {
        mUnidentified02C += mUnidentified164[1];
        mUnidentified02E++;
    }
}

void ExcitementSystem::fn_80196D64(UnidentifiedEventData_80067110*)
{
    if (mUnidentified164[3] != 0)
    {
        mUnidentified02C += mUnidentified164[3];
        mUnidentified02E++;
    }
}

void ExcitementSystem::fn_80196D8C(UnidentifiedEventData07*)
{
    if (mUnidentified164[0] != 0)
    {
        mUnidentified02C += mUnidentified164[0];
        mUnidentified02E++;
    }
}

void ExcitementSystem::DoFunctionCall(unsigned int function)
{
    switch (function)
    {
    case 0:
    {
        float value = *(float*)(m_SP - 1);
        m_SP--;
        mUnidentified028 = value * value;
        break;
    }
    case 1:
    {
        unsigned int value = m_SP[-1];
        unsigned int index = m_SP[-2];
        m_SP -= 2;
        mUnidentified030[index] = value;
        break;
    }
    case 2:
    {
        unsigned int value = m_SP[-1];
        unsigned int index = m_SP[-2];
        m_SP -= 2;
        mUnidentified0B2[index] = value;
        break;
    }
    case 3:
    {
        unsigned int value = m_SP[-1];
        unsigned int index = m_SP[-2];
        m_SP -= 2;
        mUnidentified164[index] = value;
        break;
    }
    default:
        nlBreak();
        break;
    }
}
