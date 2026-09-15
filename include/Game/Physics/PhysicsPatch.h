#ifndef GAME_PHYSICS_PHYSICS_PATCH_H
#define GAME_PHYSICS_PHYSICS_PATCH_H

#include "Game/DebugWriteCache.h"
#include "Game/EventConnection.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlFunction.h"
#include "NL/nlSlotPool.h"

class DebugWriteCache;
class PhysicsPatch;
class cPlayer;
class AvoidableObject;

struct PhysicsPatchInfo
{
    /* 0x00 */ int mType;
    /* 0x04 */ const char* mName;
    /* 0x08 */ const char* mEffectName;
    /* 0x0C */ unsigned int mCollisionMask;
    /* 0x10 */ unsigned long mSoundID;
    /* 0x14 */ float mGravity;
    /* 0x18 */ float mFriction;
    /* 0x1C */ float mBounce;
}; // total size: 0x20

PhysicsPatchInfo* GetPhysicsPatchInfo(const int& type);

class PhysicsPatch : public PhysicsSphere
{
public:
    PhysicsPatch();
    virtual ~PhysicsPatch();
    static void* operator new(unsigned long)
    {
        PhysicsPatch* patch = 0;
        lbl_805705D0.Allocate(patch);
        return patch;
    }
    static void operator delete(void* ptr)
    {
        lbl_805705D0.Free((PhysicsPatch*)ptr);
    }

    virtual void Unknown0();
    virtual int GetObjectType() const { return 0x1C; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual ContactType Contact(PhysicsObject*, dContact*, int);
    virtual void RegisterDebugFields(unsigned short* type, DebugWriteCache* cache);

    int GetType() const { return m_Type; }
    int GetCurrentPathPoint() const { return m_CurrentPathPoint; }
    inline void UnidentifiedKillEffect();
    inline void UnidentifiedDestroyEffect();

    void fn_80172EE0(const int* type);
    void Update(float dt);
    void fn_801739A4(const nlVector3& position);
    void fn_80173A10(float duration);
    void fn_80173AF4();
    void fn_80173B08(float time);
    void fn_80173B10(float time);
    void fn_80173B18();
    void fn_80173C9C(nlVector3* points, int pointCount, float speed);
    nlVector3 fn_80173CCC() const;
    void fn_80173DA4(float dt);

    static SlotPool<PhysicsPatch> lbl_805705D0;

    /* 0x38 */ Function<PhysicsPatch*> mUnidentified38;
    /* 0x40 */ nlVector3* mUnidentified40;
    /* 0x44 */ AvoidableObject* mUnidentified44;
    /* 0x48 */ int m_Type;
    /* 0x4C */ cPlayer* m_pOwner;
    /* 0x50 */ float m_fStartRadius;
    /* 0x54 */ float m_fEndRadius;
    /* 0x58 */ float m_fLifetime;
    /* 0x5C */ float m_fCurtime;
    /* 0x60 */ int m_Index;
    /* 0x64 */ bool m_bVisible;
    /* 0x65 */ bool m_bKillMe;
    /* 0x66 */ unsigned char mPadding66[2];
    /* 0x68 */ nlVector3 m_Velocity;
    /* 0x74 */ float m_Gravity;
    /* 0x78 */ cPlayer* m_pTarget;
    /* 0x7C */ float m_TargetSeekSpeed;
    /* 0x80 */ float m_fStartRadiusTime;
    /* 0x84 */ float m_fEndRadiusTime;
    /* 0x88 */ bool m_bFrozen;
    /* 0x89 */ unsigned char mPadding89[3];
    /* 0x8C */ float m_FreezeTimer;
    /* 0x90 */ float m_PathSpeed;
    /* 0x94 */ int m_CurrentPathPoint;
    /* 0x98 */ int m_PathPointCount;
    /* 0x9C */ nlVector3 mUnidentified9C;
}; // total size: 0xA8

class PhysicsPatchManager_801740D0
{
public:
    PhysicsPatchManager_801740D0();
    ~PhysicsPatchManager_801740D0();

    PhysicsPatch* fn_801743A8(int type, cPlayer* owner,
        const nlVector3& position, const nlVector3& velocity,
        float startRadius, float endRadius, float lifetime);
    PhysicsPatch* fn_801745B8(int index);
    void ResetEffects();
    void Update(float dt);
    void fn_801748A0(void* context, DebugWriteCache* cache);

    /* 0x00 */ PhysicsPatch* mUnidentified000[60];
    /* 0xF0 */ UnidentifiedEventConnectionOwner mUnidentified0F0;
    /* 0xF4 */ unsigned int mUnidentified0F4;
}; // total size: 0xF8

extern PhysicsPatchManager_801740D0* lbl_806E12C8;

inline void PhysicsPatch::RegisterDebugFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("PhysicsPatch");

#define REGISTER_FIELD(kind, field) \
    cache->AddField(kind, gDebugFieldTypes[kind].size, (unsigned char*)&field - (unsigned char*)&m_Type, #field)

    REGISTER_FIELD(14, m_Type);
    REGISTER_FIELD(15, m_pOwner);
    REGISTER_FIELD(17, m_fStartRadius);
    REGISTER_FIELD(17, m_fEndRadius);
    REGISTER_FIELD(17, m_fLifetime);
    REGISTER_FIELD(17, m_fCurtime);
    REGISTER_FIELD(8, m_Index);
    REGISTER_FIELD(16, m_bVisible);
    REGISTER_FIELD(16, m_bKillMe);
    REGISTER_FIELD(22, m_Velocity);
    REGISTER_FIELD(17, m_Gravity);
    REGISTER_FIELD(15, m_pTarget);
    REGISTER_FIELD(17, m_TargetSeekSpeed);
    REGISTER_FIELD(17, m_fStartRadiusTime);
    REGISTER_FIELD(17, m_fEndRadiusTime);
    REGISTER_FIELD(16, m_bFrozen);
    REGISTER_FIELD(17, m_FreezeTimer);
    REGISTER_FIELD(17, m_PathSpeed);
    REGISTER_FIELD(8, m_CurrentPathPoint);
    REGISTER_FIELD(8, m_PathPointCount);

#undef REGISTER_FIELD

    cache->EndType();
}

#endif // GAME_PHYSICS_PHYSICS_PATCH_H
