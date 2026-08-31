#include "Game/Task/DispatchEventsTask.h"

#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/Physics/PhysicsObject.h"
#include "NL/nlAVLTree.h"
#include "NL/nlBind.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"

extern "C" void fn_8017617C();
extern "C" void fn_801761E0();
extern "C" void fn_80144AB8();

extern "C" void fn_80065D4C(void*, EventDispatcher*, const char*, int);
extern "C" void fn_80062254(void*, int);
extern "C" void fn_80066900(void*, EventDispatcher*, const char*, int);
extern "C" void fn_8006458C(void*, int);

class UnidentifiedExternalEvent00
{
public:
    UnidentifiedExternalEvent00(EventDispatcher* dispatcher, const char* name,
        int length)
    {
        fn_80065D4C(this, dispatcher, name, length);
    }

    ~UnidentifiedExternalEvent00()
    {
        fn_80062254(this, -1);
    }

private:
    unsigned char mStorage[0x2C];
};

class UnidentifiedExternalEvent02
{
public:
    UnidentifiedExternalEvent02(EventDispatcher* dispatcher, const char* name,
        int length)
    {
        fn_80066900(this, dispatcher, name, length);
    }

    ~UnidentifiedExternalEvent02()
    {
        fn_8006458C(this, -1);
    }

private:
    unsigned char mStorage[0x2C];
};

class PhysicsEventQueue
{
public:
    PhysicsEventQueue();
    ~PhysicsEventQueue();

    void Dispatch(bool deliver)
    {
        mDispatcher.Dispatch(deliver);
    }

public:
    EventDispatcher mDispatcher;
    UnidentifiedExternalEvent00 mEvent00;
    UnidentifiedQueuedEvent<UnidentifiedEventData01> mEvent01;
    UnidentifiedExternalEvent02 mEvent02;
    UnidentifiedQueuedEvent<UnidentifiedEventData03> mEvent03;
    UnidentifiedQueuedEvent<UnidentifiedEventData04> mEvent04;
    UnidentifiedQueuedEvent<UnidentifiedEventData05> mEvent05;
    UnidentifiedQueuedEvent<UnidentifiedEventData06> mEvent06;
    UnidentifiedQueuedEvent<UnidentifiedEventData07> mEvent07;
    UnidentifiedQueuedEvent<UnidentifiedEventData08> mEvent08;
    UnidentifiedQueuedEvent<UnidentifiedEventData09> mEvent09;
    UnidentifiedQueuedEvent<UnidentifiedEventData10> mEvent10;
    UnidentifiedQueuedEvent<UnidentifiedEventData11> mEvent11;
    UnidentifiedQueuedEvent<UnidentifiedEventData12> mEvent12;
    UnidentifiedQueuedEvent<UnidentifiedEventData13> mEvent13;
    UnidentifiedQueuedEvent<UnidentifiedEventData14> mEvent14;
    UnidentifiedQueuedEvent<UnidentifiedEventData15> mEvent15;
    UnidentifiedQueuedEvent<UnidentifiedEventData16> mEvent16;
    UnidentifiedQueuedEvent<UnidentifiedEventData17> mEvent17;
    UnidentifiedQueuedEvent<UnidentifiedEventData17> mEvent18;
    UnidentifiedQueuedEvent<UnidentifiedEventData18> mEvent19;
    UnidentifiedQueuedEvent<UnidentifiedEventData19> mEvent20;
    UnidentifiedQueuedEvent<UnidentifiedEventData20> mEvent21;
    UnidentifiedQueuedEvent<UnidentifiedEventData21> mEvent22;
    UnidentifiedQueuedEvent<UnidentifiedEventData22> mEvent23;
    UnidentifiedQueuedEvent<UnidentifiedEventData23> mEvent24;
    UnidentifiedQueuedEvent<UnidentifiedEventData23> mEvent25;
    UnidentifiedQueuedEvent<UnidentifiedEventData23> mEvent26;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mEvent27;
    UnidentifiedExternalEvent00 mEvent28;
    UnidentifiedQueuedEvent<UnidentifiedEventData25> mEvent29;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent30;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent31;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent32;
    UnidentifiedQueuedEvent<UnidentifiedEventData27> mEvent33;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent34;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent35;
    UnidentifiedQueuedEvent<UnidentifiedEventData29> mEvent36;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mEvent37;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mEvent38;
    UnidentifiedQueuedEvent<UnidentifiedEventData30> mEvent39;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent40;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mEvent41;
    UnidentifiedQueuedEvent<UnidentifiedEventData31> mEvent42;
    UnidentifiedQueuedEvent<UnidentifiedEventData31> mEvent43;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent44;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent45;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent46;
    UnidentifiedQueuedEvent<UnidentifiedEventData27> mEvent47;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent48;
    UnidentifiedQueuedEvent<UnidentifiedEventData32> mEvent49;
    UnidentifiedQueuedEvent<UnidentifiedEventData33> mEvent50;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent51;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent52;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent53;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent54;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent55;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent56;
    UnidentifiedQueuedEvent<UnidentifiedEventData27> mEvent57;
    UnidentifiedQueuedEvent<UnidentifiedEventData35> mEvent58;
    UnidentifiedQueuedEvent<UnidentifiedEventData36> mEvent59;
    UnidentifiedQueuedEvent<UnidentifiedEventData37> mEvent60;
    UnidentifiedQueuedEvent<UnidentifiedEventData36> mEvent61;
    UnidentifiedQueuedEvent<UnidentifiedEventData38> mEvent62;
};

extern "C" PhysicsEventQueue* lbl_806E11F0;

PhysicsEventQueue::PhysicsEventQueue()
    : mDispatcher("PhysicsEventQueue")
    , mEvent00(&mDispatcher, "BallFall", -1)
    , mEvent01(&mDispatcher, "CollisionPlayerPlayer", -1)
    , mEvent02(&mDispatcher, "CollisionPlayerWall", -1)
    , mEvent03(&mDispatcher, "CollisionPlayerBall", -1)
    , mEvent04(&mDispatcher, "CollisionBallNetmesh", -1)
    , mEvent05(&mDispatcher, "CollisionBallGround", -1)
    , mEvent06(&mDispatcher, "CollisionBallWall", -1)
    , mEvent07(&mDispatcher, "CollisionBallGoalpost", -1)
    , mEvent08(&mDispatcher, "CollisionBallShell", -1)
    , mEvent09(&mDispatcher, "CollisionBallChain", -1)
    , mEvent10(&mDispatcher, "CollisionKoopaShotBallPlayer", -1)
    , mEvent11(&mDispatcher, "CollisionKoopaShellGoalie", -1)
    , mEvent12(&mDispatcher, "CollisionKoopaShellEnd", -1)
    , mEvent13(&mDispatcher, "CollisionBirdoShotBallPlayer", -1)
    , mEvent14(&mDispatcher, "CollisionBirdoEggGoalie", -1)
    , mEvent15(&mDispatcher, "CollisionBirdoEggEnd", -1)
    , mEvent16(&mDispatcher, "CollisionHammerbroShotBallPlayer", -1)
    , mEvent17(&mDispatcher, "CollisionPowerupGround", -1)
    , mEvent18(&mDispatcher, "CollisionPowerupGoalie", -1)
    , mEvent19(&mDispatcher, "CollisionPowerupWall", -1)
    , mEvent20(&mDispatcher, "PowerupHit", -1)
    , mEvent21(&mDispatcher, "CollisionPlayerBanana", -1)
    , mEvent22(&mDispatcher, "CollisionPlayerShell", -1)
    , mEvent23(&mDispatcher, "CollisionPlayerFreeze", -1)
    , mEvent24(&mDispatcher, "CollisionBulletBillPlayer", -1)
    , mEvent25(&mDispatcher, "CollisionBulletBillFreeze", -1)
    , mEvent26(&mDispatcher, "ExplosionBulletBill", -1)
    , mEvent27(&mDispatcher, "CollisionTongue", -1)
    , mEvent28(&mDispatcher, "CollisionBallTronWall", -1)
    , mEvent29(&mDispatcher, "PowerupUsed", -1)
    , mEvent30(&mDispatcher, "CollisionFireballPlayer", -1)
    , mEvent31(&mDispatcher, "CollisionFireballBall", -1)
    , mEvent32(&mDispatcher, "CollisionFireballGround", -1)
    , mEvent33(&mDispatcher, "CollisionFireballPowerup", -1)
    , mEvent34(&mDispatcher, "CollisionFireballChain", -1)
    , mEvent35(&mDispatcher, "CollisionChainCrowd", -1)
    , mEvent36(&mDispatcher, "CollisionChainPowerup", -1)
    , mEvent37(&mDispatcher, "CollisionPatchPlayer", -1)
    , mEvent38(&mDispatcher, "CollisionPatchGround", -1)
    , mEvent39(&mDispatcher, "CollisionPatchPowerup", -1)
    , mEvent40(&mDispatcher, "CollisionPatchChain", -1)
    , mEvent41(&mDispatcher, "CollisionPatchPatch", -1)
    , mEvent42(&mDispatcher, "CollisionPatchBall", -1)
    , mEvent43(&mDispatcher, "CollisionPatchWall", -1)
    , mEvent44(&mDispatcher, "CollisionHammerPlayer", -1)
    , mEvent45(&mDispatcher, "CollisionHammerBall", -1)
    , mEvent46(&mDispatcher, "CollisionHammerGround", -1)
    , mEvent47(&mDispatcher, "CollisionHammerPowerup", -1)
    , mEvent48(&mDispatcher, "CollisionHammerChain", -1)
    , mEvent49(&mDispatcher, "CollisionThwompPlayer", -1)
    , mEvent50(&mDispatcher, "CollisionThwompBall", -1)
    , mEvent51(&mDispatcher, "CollisionThwompChain", -1)
    , mEvent52(&mDispatcher, "CollisionEggBall", -1)
    , mEvent53(&mDispatcher, "CollisionEggPlayer", -1)
    , mEvent54(&mDispatcher, "CollisionEggPowerup", -1)
    , mEvent55(&mDispatcher, "CollisionEggChain", -1)
    , mEvent56(&mDispatcher, "CollisionCrackEgg", -1)
    , mEvent57(&mDispatcher, "DestroyPowerup", -1)
    , mEvent58(&mDispatcher, "DestroyHammer", -1)
    , mEvent59(&mDispatcher, "KnockYoshiTongue", -1)
    , mEvent60(&mDispatcher, "CollisionDebrisBall", -1)
    , mEvent61(&mDispatcher, "CollisionWaluigiWall", -1)
    , mEvent62(&mDispatcher, "CollisionShockwave", -1)
{
    fn_80144AB8();
}

PhysicsEventQueue::~PhysicsEventQueue()
{
}

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

struct UnidentifiedMemberFunction
{
    long thisDelta;
    long vtableOffset;
    void* function;
};

extern "C" UnidentifiedEventRegistry* lbl_806E1D90;
extern "C" long __ptmf_test(UnidentifiedMemberFunction*);
extern "C" UnidentifiedMemberFunction lbl_8050F58C;
extern "C" unsigned char lbl_804DCC60[];
extern "C" void* g_pBall;

extern "C" void fn_800156F8(void*, void*);
extern "C" void fn_80015B38(void*, int);
extern "C" void fn_8002E5F4(void*, int);
extern "C" void fn_8004AC68(void*, nlVector3*, void*);
extern "C" void fn_8019A434(void*, int);
extern "C" void fn_801A1CFC(void*, int);
extern "C" void fn_801A64A4(void*, int);
extern "C" void fn_80176A60(nlVector3*);

extern "C" void fn_801452F4(void* object)
{
    ((unsigned char*)object)[4] = true;
}

extern "C" void fn_80145300(void* object)
{
    unsigned char* bytes = (unsigned char*)object;
    if (bytes[4] == false)
    {
        bytes[4] = true;
    }
}

extern "C" void fn_80145318(void* object)
{
    nlVector3 direction;
    nlPolarToCartesian(direction.x, direction.y, *(unsigned short*)((unsigned char*)object + 0x62), 1.0f);
    direction.z = 0.0f;
    fn_8004AC68(object, &direction, lbl_804DCC60);
}

extern "C" void fn_80145370(void* object)
{
    if (__ptmf_test(&lbl_8050F58C))
    {
        fn_801A1CFC(object, 0);
    }
}

extern "C" void fn_801453B8(void* object)
{
    unsigned char* bytes = (unsigned char*)object;
    if (*(int*)(bytes + 0xA4) != 2 || *(int*)(bytes + 0x1C) != 2)
    {
        bytes[4] = true;
    }
}

extern "C" void fn_801453DC(void*)
{
}

extern "C" void fn_801453E0(void* data)
{
    unsigned char* object = *(unsigned char**)((unsigned char*)data + 4);
    if (*(int*)(object + 0xF0) == 2)
    {
        fn_8002E5F4(object, 0);
    }
}

extern "C" void fn_801453FC(void* data)
{
    unsigned char* bytes = (unsigned char*)data;
    if (bytes[4] != false)
    {
        fn_800156F8(
            g_pBall, *(void**)(*(unsigned char**)bytes + 0x2C));
    }
    else
    {
        fn_80015B38(g_pBall, 0);
    }
    fn_801A64A4(*(void**)bytes, 0);
}

extern "C" void fn_8014545C(void* data)
{
    unsigned char* bytes = (unsigned char*)data;
    if (bytes[4] != false)
    {
        fn_800156F8(
            g_pBall, *(void**)(*(unsigned char**)bytes + 0x3C));
    }
    else
    {
        fn_80015B38(g_pBall, 0);
    }
    fn_8019A434(*(void**)bytes, 0);
}

extern "C" void fn_801454BC(void*);

extern "C" void fn_80145C3C(void* data)
{
    unsigned char* object = *(unsigned char**)((unsigned char*)data + 0x10);
    if (*(int*)(object + 0x48) == 0 && object[0x65] == false)
    {
        fn_80176A60(&((PhysicsObject*)object)->GetPosition());
        ((PhysicsObject*)object)->Unknown0();
    }
}

static void UnidentifiedRegisterEventCallback(const char* name,
    void (*callback)(void*))
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

extern "C" void fn_80144AB8()
{
    UnidentifiedRegisterEventCallback("CollisionPatchPowerup", fn_801453DC);
    UnidentifiedRegisterEventCallback("CollisionHammerPowerup", fn_801453B8);
    UnidentifiedRegisterEventCallback("CollisionFireballPowerup", fn_801452F4);
    UnidentifiedRegisterEventCallback("CollisionCrackEgg", fn_801453E0);
    UnidentifiedRegisterEventCallback("CollisionShockwave", fn_801454BC);
    UnidentifiedRegisterEventCallback("CollisionKoopaShellEnd", fn_801453FC);
    UnidentifiedRegisterEventCallback("CollisionBirdoEggEnd", fn_8014545C);
    UnidentifiedRegisterEventCallback("CollisionPatchPatch", fn_80145C3C);
    UnidentifiedRegisterEventCallback("DestroyPowerup", fn_80145300);
    UnidentifiedRegisterEventCallback("DestroyHammer", fn_80145370);
    UnidentifiedRegisterEventCallback("CollisionWaluigiWall", fn_80145318);
}

extern "C" SlotPool<UnidentifiedEventData04> lbl_80571780;

extern "C" void fn_800721AC(void*);
extern "C" void fn_8016A658(void*);
extern "C" void fn_8016A670(void*);
extern "C" void fn_8016A688(void*);
extern "C" void fn_8016A6A0(void*);
extern "C" void fn_8016A6B8(void*);
extern "C" void fn_8016A6D0(void*);
extern "C" void fn_8016A6E8(void*);
extern "C" void fn_8016A700(void*);
extern "C" void fn_8016A718(void*);
extern "C" void fn_8016A730(void*);
extern "C" void fn_8016A748(void*);
extern "C" void fn_8016A760(void*);
extern "C" void fn_8016A778(void*);
extern "C" void fn_8016A790(void*);
extern "C" void fn_8016A7A8(void*);
extern "C" void fn_8016A7C0(void*);
extern "C" void fn_8016A7D8(void*);
extern "C" void fn_8016A7F0(void*);
extern "C" void fn_8016A808(void*);
extern "C" void fn_8016A820(void*);
extern "C" void fn_8016A838(void*);
extern "C" void fn_8016A850(void*);
extern "C" void fn_8016A868(void*);
extern "C" void fn_8016A880(void*);
extern "C" void fn_8016A898(void*);
extern "C" void fn_8016A8B0(void*);
extern "C" void fn_8016A8C8(void*);
extern "C" void fn_8016A8E0(void*);
extern "C" void fn_8016A8F8(void*);

extern "C" void fn_80145C9C()
{
    ((UnidentifiedQueuedEvent<UnidentifiedEventData00>*)&lbl_806E11F0->mEvent00)
        ->Queue((UnidentifiedEventData00*)0,
            Function<UnidentifiedEventData00*>());
}

extern "C" void fn_80145DD0(UnidentifiedEventData01* data)
{
    Function<UnidentifiedEventData01*> disposer(
        (void (*)(UnidentifiedEventData01*))fn_8016A658);
    lbl_806E11F0->mEvent01.Queue(data, disposer);
}

extern "C" void fn_80145F18(UnidentifiedEventData02* data)
{
    ((UnidentifiedQueuedEvent<UnidentifiedEventData02>*)&lbl_806E11F0->mEvent02)
        ->Queue(data,
            Function<UnidentifiedEventData02*>(
                (void (*)(UnidentifiedEventData02*))fn_800721AC));
}

extern "C" void fn_80146060(UnidentifiedEventData24* data)
{
    lbl_806E11F0->mEvent27.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))fn_8016A670));
}

extern "C" void fn_801461A8(UnidentifiedEventData00* data)
{
    ((UnidentifiedQueuedEvent<UnidentifiedEventData00>*)&lbl_806E11F0->mEvent00)
        ->Queue(data, Function<UnidentifiedEventData00*>());
}

extern "C" void fn_801462DC(UnidentifiedEventData03* data)
{
    lbl_806E11F0->mEvent03.Queue(
        data, Function<UnidentifiedEventData03*>((void (*)(UnidentifiedEventData03*))fn_8016A688));
}

extern "C" void fn_80146424(UnidentifiedEventData04* data, bool release)
{
    if (!release)
    {
        lbl_806E11F0->mEvent04.Queue(
            data, Function<UnidentifiedEventData04*>((void (*)(UnidentifiedEventData04*))fn_8016A6A0));
    }
    else
    {
        lbl_80571780.Free(data);
    }
}

extern "C" void fn_8014658C(UnidentifiedEventData05* data)
{
    lbl_806E11F0->mEvent05.Queue(
        data, Function<UnidentifiedEventData05*>((void (*)(UnidentifiedEventData05*))fn_8016A6B8));
}

extern "C" void fn_801466D4(UnidentifiedEventData06* data)
{
    lbl_806E11F0->mEvent06.Queue(
        data, Function<UnidentifiedEventData06*>((void (*)(UnidentifiedEventData06*))fn_8016A6D0));
}

extern "C" void fn_8014681C(UnidentifiedEventData07* data)
{
    lbl_806E11F0->mEvent07.Queue(
        data, Function<UnidentifiedEventData07*>((void (*)(UnidentifiedEventData07*))fn_8016A6E8));
}

extern "C" void fn_80146964(UnidentifiedEventData08* data)
{
    lbl_806E11F0->mEvent08.Queue(
        data, Function<UnidentifiedEventData08*>((void (*)(UnidentifiedEventData08*))fn_8016A700));
}

extern "C" void fn_80146AAC(UnidentifiedEventData09* data)
{
    lbl_806E11F0->mEvent09.Queue(
        data, Function<UnidentifiedEventData09*>((void (*)(UnidentifiedEventData09*))fn_8016A718));
}

extern "C" void fn_80146BF4(UnidentifiedEventData10* data)
{
    lbl_806E11F0->mEvent10.Queue(
        data, Function<UnidentifiedEventData10*>((void (*)(UnidentifiedEventData10*))fn_8016A730));
}

extern "C" void fn_80146D3C(UnidentifiedEventData11* data)
{
    lbl_806E11F0->mEvent11.Queue(
        data, Function<UnidentifiedEventData11*>((void (*)(UnidentifiedEventData11*))fn_8016A748));
}

extern "C" void fn_80146E84(UnidentifiedEventData12* data)
{
    lbl_806E11F0->mEvent12.Queue(
        data, Function<UnidentifiedEventData12*>((void (*)(UnidentifiedEventData12*))fn_8016A760));
}

extern "C" void fn_80146FCC(UnidentifiedEventData13* data)
{
    lbl_806E11F0->mEvent13.Queue(
        data, Function<UnidentifiedEventData13*>((void (*)(UnidentifiedEventData13*))fn_8016A778));
}

extern "C" void fn_80147114(UnidentifiedEventData14* data)
{
    lbl_806E11F0->mEvent14.Queue(
        data, Function<UnidentifiedEventData14*>((void (*)(UnidentifiedEventData14*))fn_8016A790));
}

extern "C" void fn_8014725C(UnidentifiedEventData15* data)
{
    lbl_806E11F0->mEvent15.Queue(
        data, Function<UnidentifiedEventData15*>((void (*)(UnidentifiedEventData15*))fn_8016A7A8));
}

extern "C" void fn_801473A4(UnidentifiedEventData17* data)
{
    lbl_806E11F0->mEvent17.Queue(
        data, Function<UnidentifiedEventData17*>((void (*)(UnidentifiedEventData17*))fn_8016A7C0));
}

extern "C" void fn_801474EC(UnidentifiedEventData17* data)
{
    lbl_806E11F0->mEvent18.Queue(
        data, Function<UnidentifiedEventData17*>((void (*)(UnidentifiedEventData17*))fn_8016A7C0));
}

extern "C" void fn_80147634(UnidentifiedEventData18* data)
{
    lbl_806E11F0->mEvent19.Queue(
        data, Function<UnidentifiedEventData18*>((void (*)(UnidentifiedEventData18*))fn_8016A7D8));
}

extern "C" void fn_8014777C(UnidentifiedEventData19* data)
{
    lbl_806E11F0->mEvent20.Queue(
        data, Function<UnidentifiedEventData19*>((void (*)(UnidentifiedEventData19*))fn_8016A7F0));
}

extern "C" void fn_801478C4(UnidentifiedEventData20* data)
{
    lbl_806E11F0->mEvent21.Queue(
        data, Function<UnidentifiedEventData20*>((void (*)(UnidentifiedEventData20*))fn_8016A808));
}

extern "C" void fn_80147A0C(UnidentifiedEventData21* data)
{
    lbl_806E11F0->mEvent22.Queue(
        data, Function<UnidentifiedEventData21*>((void (*)(UnidentifiedEventData21*))fn_8016A820));
}

extern "C" void fn_80147B54(UnidentifiedEventData22* data)
{
    lbl_806E11F0->mEvent23.Queue(
        data, Function<UnidentifiedEventData22*>((void (*)(UnidentifiedEventData22*))fn_8016A838));
}

extern "C" void fn_80147C9C(UnidentifiedEventData23* data)
{
    lbl_806E11F0->mEvent24.Queue(
        data, Function<UnidentifiedEventData23*>((void (*)(UnidentifiedEventData23*))fn_8016A850));
}

extern "C" void fn_80147DE4(UnidentifiedEventData23* data)
{
    lbl_806E11F0->mEvent25.Queue(
        data, Function<UnidentifiedEventData23*>((void (*)(UnidentifiedEventData23*))fn_8016A850));
}

extern "C" void fn_80147F2C(UnidentifiedEventData23* data)
{
    lbl_806E11F0->mEvent26.Queue(
        data, Function<UnidentifiedEventData23*>((void (*)(UnidentifiedEventData23*))fn_8016A850));
}

extern "C" void fn_80148074(UnidentifiedEventData25* data)
{
    lbl_806E11F0->mEvent29.Queue(
        data, Function<UnidentifiedEventData25*>((void (*)(UnidentifiedEventData25*))fn_8016A868));
}

extern "C" void fn_801481BC(UnidentifiedEventData28* data)
{
    lbl_806E11F0->mEvent35.Queue(data, Function<UnidentifiedEventData28*>());
}

extern "C" void fn_801482F8(UnidentifiedEventData29* data)
{
    lbl_806E11F0->mEvent36.Queue(
        data, Function<UnidentifiedEventData29*>((void (*)(UnidentifiedEventData29*))fn_8016A880));
}

extern "C" void fn_80148440(UnidentifiedEventData24* data)
{
    lbl_806E11F0->mEvent37.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))fn_8016A670));
}

extern "C" void fn_80148588(UnidentifiedEventData24* data)
{
    lbl_806E11F0->mEvent38.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))fn_8016A670));
}

extern "C" void fn_801486D0(UnidentifiedEventData30* data)
{
    lbl_806E11F0->mEvent39.Queue(
        data, Function<UnidentifiedEventData30*>((void (*)(UnidentifiedEventData30*))fn_8016A898));
}

extern "C" void fn_80148818(UnidentifiedEventData28* data)
{
    lbl_806E11F0->mEvent40.Queue(data, Function<UnidentifiedEventData28*>());
}

extern "C" void fn_80148954(UnidentifiedEventData24* data)
{
    lbl_806E11F0->mEvent41.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))fn_8016A670));
}

extern "C" void fn_80148A9C(UnidentifiedEventData31* data)
{
    lbl_806E11F0->mEvent42.Queue(data, Function<UnidentifiedEventData31*>());
}

extern "C" void fn_80148BD8(UnidentifiedEventData31* data)
{
    lbl_806E11F0->mEvent43.Queue(data, Function<UnidentifiedEventData31*>());
}

extern "C" void fn_80148D14(UnidentifiedEventData26* data)
{
    lbl_806E11F0->mEvent44.Queue(
        data, Function<UnidentifiedEventData26*>((void (*)(UnidentifiedEventData26*))fn_8016A8B0));
}

extern "C" void fn_80148E5C(UnidentifiedEventData26* data)
{
    lbl_806E11F0->mEvent46.Queue(
        data, Function<UnidentifiedEventData26*>((void (*)(UnidentifiedEventData26*))fn_8016A8B0));
}

extern "C" void fn_80148FA4(UnidentifiedEventData27* data)
{
    lbl_806E11F0->mEvent47.Queue(data, Function<UnidentifiedEventData27*>());
}

extern "C" void fn_801490E0(UnidentifiedEventData27* data)
{
    lbl_806E11F0->mEvent57.Queue(data, Function<UnidentifiedEventData27*>());
}

extern "C" void fn_8014921C(UnidentifiedEventData27* data)
{
    lbl_806E11F0->mEvent57.Queue(data, Function<UnidentifiedEventData27*>());
}

extern "C" void fn_80149358(UnidentifiedEventData35* data)
{
    lbl_806E11F0->mEvent58.Queue(data, Function<UnidentifiedEventData35*>());
}

extern "C" void fn_80149494(UnidentifiedEventData35* data)
{
    lbl_806E11F0->mEvent58.Queue(data, Function<UnidentifiedEventData35*>());
}

extern "C" void fn_801495D0(UnidentifiedEventData36* data)
{
    lbl_806E11F0->mEvent59.Queue(data, Function<UnidentifiedEventData36*>());
}

extern "C" void fn_8014970C(UnidentifiedEventData36* data)
{
    lbl_806E11F0->mEvent59.Queue(data, Function<UnidentifiedEventData36*>());
}

extern "C" void fn_80149848(UnidentifiedEventData28* data)
{
    lbl_806E11F0->mEvent48.Queue(data, Function<UnidentifiedEventData28*>());
}

extern "C" SlotPool<UnidentifiedEventData32> lbl_805712D0;

extern "C" void fn_80149984(void* source, void* target)
{
    UnidentifiedEventData32* data = 0;
    lbl_805712D0.Allocate(data);
    data->source = source;
    data->sourceValue = *(void**)source;
    data->target = target;
    lbl_806E11F0->mEvent49.Queue(
        data, Function<UnidentifiedEventData32*>((void (*)(UnidentifiedEventData32*))fn_8016A8C8));
}

extern "C" void fn_80149B30(UnidentifiedEventData33* data)
{
    lbl_806E11F0->mEvent50.Queue(data, Function<UnidentifiedEventData33*>());
}

extern "C" void fn_80149C6C(UnidentifiedEventData34* data)
{
    lbl_806E11F0->mEvent52.Queue(
        data, Function<UnidentifiedEventData34*>((void (*)(UnidentifiedEventData34*))fn_8016A8E0));
}

extern "C" void fn_80149DB4(UnidentifiedEventData34* data)
{
    lbl_806E11F0->mEvent53.Queue(
        data, Function<UnidentifiedEventData34*>((void (*)(UnidentifiedEventData34*))fn_8016A8E0));
}

extern "C" void fn_80149EFC(UnidentifiedEventData34* data)
{
    lbl_806E11F0->mEvent56.Queue(
        data, Function<UnidentifiedEventData34*>((void (*)(UnidentifiedEventData34*))fn_8016A8E0));
}

extern "C" void fn_8014A044(UnidentifiedEventData37* data)
{
    lbl_806E11F0->mEvent60.Queue(data, Function<UnidentifiedEventData37*>());
}

extern "C" void fn_8014A180(UnidentifiedEventData36* data)
{
    lbl_806E11F0->mEvent61.Queue(data, Function<UnidentifiedEventData36*>());
}

extern "C" void fn_8014A2BC(UnidentifiedEventData38* data)
{
    lbl_806E11F0->mEvent62.Queue(
        data, Function<UnidentifiedEventData38*>((void (*)(UnidentifiedEventData38*))fn_8016A8F8));
}

EventDispatcher::EventDispatcher(const char*)
    : EventDispatcherBase()
{
}

struct UnidentifiedPooledData20
{
    unsigned char data[0x20];
};

struct UnidentifiedPooledData14A
{
    unsigned char data[0x14];
};

struct UnidentifiedPooledData08
{
    unsigned char data[0x08];
};

struct UnidentifiedPooledData14B
{
    unsigned char data[0x14];
};

struct UnidentifiedPooledData0C
{
    unsigned char data[0x0C];
};

static SlotPool<UnidentifiedPooledData20> lbl_80570110(16, 16);
static SlotPool<UnidentifiedPooledData14A> lbl_80570138(16, 16);
static SlotPool<UnidentifiedPooledData08> lbl_80570160(16, 16);
static SlotPool<UnidentifiedPooledData14B> lbl_80570188(16, 16);
static SlotPool<UnidentifiedPooledData0C> lbl_805701B0(16, 16);

PhysicsEventQueue* lbl_806E11F0;

extern "C" void fn_80143FD4()
{
    lbl_80570110.FreeBlocks();
    lbl_80570138.FreeBlocks();
    lbl_80570160.FreeBlocks();
    lbl_80570188.FreeBlocks();
    lbl_805701B0.FreeBlocks();
}

extern "C" void fn_80144070()
{
    if (lbl_806E11F0 == 0)
    {
        lbl_806E11F0 = new (nlMalloc(sizeof(PhysicsEventQueue), 8, false))
            PhysicsEventQueue;
        fn_8017617C();
    }
}

extern "C" void fn_801440BC()
{
    if (lbl_806E11F0 != 0)
    {
        lbl_806E11F0->~PhysicsEventQueue();
        nlFree(lbl_806E11F0);
        lbl_806E11F0 = 0;
        fn_801761E0();
    }
}

extern "C" void fn_80144130(EventDispatcher* dispatcher)
{
    dispatcher->Dispatch(true);
}

extern "C" SlotPool<UnidentifiedEventData01> lbl_80571258;
extern "C" SlotPool<UnidentifiedEventData03> lbl_805714D8;
extern "C" SlotPool<UnidentifiedEventData05> lbl_80571488;
extern "C" SlotPool<UnidentifiedEventData06> lbl_80571410;
extern "C" SlotPool<UnidentifiedEventData07> lbl_80571758;
extern "C" SlotPool<UnidentifiedEventData08> lbl_80571618;
extern "C" SlotPool<UnidentifiedEventData09> lbl_80571640;
extern "C" SlotPool<UnidentifiedEventData10> lbl_80571550;
extern "C" SlotPool<UnidentifiedEventData11> lbl_80571528;
extern "C" SlotPool<UnidentifiedEventData12> lbl_805715C8;
extern "C" SlotPool<UnidentifiedEventData13> lbl_80571578;
extern "C" SlotPool<UnidentifiedEventData14> lbl_805715A0;
extern "C" SlotPool<UnidentifiedEventData15> lbl_805715F0;
extern "C" SlotPool<UnidentifiedEventData17> lbl_80571460;
extern "C" SlotPool<UnidentifiedEventData18> lbl_805714B0;
extern "C" SlotPool<UnidentifiedEventData19> lbl_805719D8;
extern "C" SlotPool<UnidentifiedEventData20> lbl_805716B8;
extern "C" SlotPool<UnidentifiedEventData21> lbl_80571668;
extern "C" SlotPool<UnidentifiedEventData22> lbl_80571690;
extern "C" SlotPool<UnidentifiedEventData23> lbl_805712F8;
extern "C" SlotPool<UnidentifiedEventData25> lbl_805719B0;
extern "C" SlotPool<UnidentifiedEventData29> lbl_80571320;

extern "C" unsigned char fn_8016A650(void* object)
{
    return ((unsigned char*)object)[0x24];
}

extern "C" void fn_8016A658(void* data)
{
    lbl_80571258.Free((UnidentifiedEventData01*)data);
}

extern "C" void fn_8016A670(void* data)
{
    lbl_80570138.Free((UnidentifiedPooledData14A*)data);
}

extern "C" void fn_8016A688(void* data)
{
    lbl_805714D8.Free((UnidentifiedEventData03*)data);
}

extern "C" void fn_8016A6A0(void* data)
{
    lbl_80571780.Free((UnidentifiedEventData04*)data);
}

extern "C" void fn_8016A6B8(void* data)
{
    lbl_80571488.Free((UnidentifiedEventData05*)data);
}

extern "C" void fn_8016A6D0(void* data)
{
    lbl_80571410.Free((UnidentifiedEventData06*)data);
}

extern "C" void fn_8016A6E8(void* data)
{
    lbl_80571758.Free((UnidentifiedEventData07*)data);
}

extern "C" void fn_8016A700(void* data)
{
    lbl_80571618.Free((UnidentifiedEventData08*)data);
}

extern "C" void fn_8016A718(void* data)
{
    lbl_80571640.Free((UnidentifiedEventData09*)data);
}

extern "C" void fn_8016A730(void* data)
{
    lbl_80571550.Free((UnidentifiedEventData10*)data);
}

extern "C" void fn_8016A748(void* data)
{
    lbl_80571528.Free((UnidentifiedEventData11*)data);
}

extern "C" void fn_8016A760(void* data)
{
    lbl_805715C8.Free((UnidentifiedEventData12*)data);
}

extern "C" void fn_8016A778(void* data)
{
    lbl_80571578.Free((UnidentifiedEventData13*)data);
}

extern "C" void fn_8016A790(void* data)
{
    lbl_805715A0.Free((UnidentifiedEventData14*)data);
}

extern "C" void fn_8016A7A8(void* data)
{
    lbl_805715F0.Free((UnidentifiedEventData15*)data);
}

extern "C" void fn_8016A7C0(void* data)
{
    lbl_80571460.Free((UnidentifiedEventData17*)data);
}

extern "C" void fn_8016A7D8(void* data)
{
    lbl_805714B0.Free((UnidentifiedEventData18*)data);
}

extern "C" void fn_8016A7F0(void* data)
{
    lbl_805719D8.Free((UnidentifiedEventData19*)data);
}

extern "C" void fn_8016A808(void* data)
{
    lbl_805716B8.Free((UnidentifiedEventData20*)data);
}

extern "C" void fn_8016A820(void* data)
{
    lbl_80571668.Free((UnidentifiedEventData21*)data);
}

extern "C" void fn_8016A838(void* data)
{
    lbl_80571690.Free((UnidentifiedEventData22*)data);
}

extern "C" void fn_8016A850(void* data)
{
    lbl_805712F8.Free((UnidentifiedEventData23*)data);
}

extern "C" void fn_8016A868(void* data)
{
    lbl_805719B0.Free((UnidentifiedEventData25*)data);
}

extern "C" void fn_8016A880(void* data)
{
    lbl_80571320.Free((UnidentifiedEventData29*)data);
}

extern "C" void fn_8016A898(void* data)
{
    lbl_80570160.Free((UnidentifiedPooledData08*)data);
}

extern "C" void fn_8016A8B0(void* data)
{
    lbl_80570110.Free((UnidentifiedPooledData20*)data);
}

extern "C" void fn_8016A8C8(void* data)
{
    lbl_805712D0.Free((UnidentifiedEventData32*)data);
}

extern "C" void fn_8016A8E0(void* data)
{
    lbl_80570188.Free((UnidentifiedPooledData14B*)data);
}

extern "C" void fn_8016A8F8(void* data)
{
    lbl_805701B0.Free((UnidentifiedPooledData0C*)data);
}
