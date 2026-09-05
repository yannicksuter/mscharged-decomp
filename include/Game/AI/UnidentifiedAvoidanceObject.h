#ifndef GAME_AI_UNIDENTIFIED_AVOIDANCE_OBJECT_H
#define GAME_AI_UNIDENTIFIED_AVOIDANCE_OBJECT_H

#include "Game/AI/AvoidController.h"
#include "NL/nlMath.h"

class cFielder;
class cPlayer;
class PhysicsPatch;
class PowerupBase;
struct UnidentifiedObject_801B535C;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

// Polymorphic "thing to avoid" registered with the AvoidController. The
// retail binary keeps the family in one translation unit; every class name
// below is an unidentified placeholder keyed by its vtable address.
class UnidentifiedAvoidanceObject
{
public:
    UnidentifiedAvoidanceObject(int type);
    virtual ~UnidentifiedAvoidanceObject();
    virtual const nlVector3& GetPosition() = 0;
    virtual const nlVector3& GetVelocity() = 0;
    virtual float GetRadius() = 0;
    virtual float UnidentifiedVirtual18()
    {
        return 0.0f;
    }
    virtual bool UnidentifiedVirtual1C(
        const nlVector3& target, nlVector3& point, nlVector3& dir);
    virtual bool UnidentifiedVirtual20(
        UnidentifiedAvoidanceObject* other, float range);
    virtual float UnidentifiedVirtual24(UnidentifiedAvoidanceObject* other)
    {
        return 1.0f;
    }
    virtual float UnidentifiedVirtual28(UnidentifiedAvoidanceObject* other)
    {
        return 1.0f;
    }
    virtual bool UnidentifiedVirtual2C()
    {
        return false;
    }

    /* 0x04 */ UnidentifiedAvoidanceObject* next;
    /* 0x08 */ int mUnidentified008;
    /* 0x0C */ int mType;
    /* 0x10 */ const float* mTweaks;
}; // size: 0x14

class UnidentifiedAvoidanceFielder_804F4840 : public UnidentifiedAvoidanceObject
{
public:
    UnidentifiedAvoidanceFielder_804F4840(cFielder* pFielder)
        : UnidentifiedAvoidanceObject(AVOID_FIELDERS)
        , m_pFielder(pFielder)
    {
    }
    virtual ~UnidentifiedAvoidanceFielder_804F4840()
    {
    }
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity();
    virtual float GetRadius();
    virtual float UnidentifiedVirtual18();
    virtual bool UnidentifiedVirtual20(
        UnidentifiedAvoidanceObject* other, float range);
    virtual float UnidentifiedVirtual24(UnidentifiedAvoidanceObject* other);
    virtual float UnidentifiedVirtual28(UnidentifiedAvoidanceObject* other);
    virtual bool UnidentifiedVirtual2C()
    {
        return true;
    }

    /* 0x14 */ cFielder* m_pFielder;
}; // size: 0x18

class UnidentifiedAvoidancePlayer_804F4810 : public UnidentifiedAvoidanceObject
{
public:
    UnidentifiedAvoidancePlayer_804F4810(cPlayer* pPlayer)
        : UnidentifiedAvoidanceObject(AVOID_GOALIES)
        , m_pPlayer(pPlayer)
    {
    }
    virtual ~UnidentifiedAvoidancePlayer_804F4810()
    {
    }
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity();
    virtual float GetRadius();
    virtual bool UnidentifiedVirtual2C()
    {
        return true;
    }

    /* 0x14 */ cPlayer* m_pPlayer;
}; // size: 0x18

class UnidentifiedAvoidancePowerup_804F47E0 : public UnidentifiedAvoidanceObject
{
public:
    UnidentifiedAvoidancePowerup_804F47E0(PowerupBase* pPowerup)
        : UnidentifiedAvoidanceObject(AVOID_POWERUPS)
        , m_pPowerup(pPowerup)
        , mUnidentified018(0)
    {
    }
    virtual ~UnidentifiedAvoidancePowerup_804F47E0()
    {
    }
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity();
    virtual float GetRadius();
    virtual bool UnidentifiedVirtual2C();

    /* 0x14 */ PowerupBase* m_pPowerup;
    /* 0x18 */ UnidentifiedObject_801B535C* mUnidentified018;
}; // size: 0x1C

class UnidentifiedAvoidancePoint_804F47B0 : public UnidentifiedAvoidanceObject
{
public:
    UnidentifiedAvoidancePoint_804F47B0(
        int type, const nlVector3& position, float radius)
        : UnidentifiedAvoidanceObject(type)
        , mPosition(position)
        , mRadius(radius)
    {
    }
    virtual ~UnidentifiedAvoidancePoint_804F47B0()
    {
    }
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity();
    virtual float GetRadius();

    /* 0x14 */ nlVector3 mPosition;
    /* 0x20 */ float mRadius;
}; // size: 0x24

class UnidentifiedAvoidancePatch_804F4780 : public UnidentifiedAvoidanceObject
{
public:
    UnidentifiedAvoidancePatch_804F4780(PhysicsPatch* pPatch)
        : UnidentifiedAvoidanceObject(AVOID_UNIDENTIFIED_20)
        , m_pPatch(pPatch)
    {
    }
    virtual ~UnidentifiedAvoidancePatch_804F4780()
    {
    }
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity();
    virtual float GetRadius();
    virtual bool UnidentifiedVirtual2C();

    /* 0x14 */ PhysicsPatch* m_pPatch;
    /* 0x18 */ nlVector3 mUnidentified018;
}; // size: 0x24

class UnidentifiedAvoidancePolygon_804F4750 : public UnidentifiedAvoidanceObject
{
public:
    UnidentifiedAvoidancePolygon_804F4750(
        int mode, const nlVector3& a, const nlVector3& b, float width);
    UnidentifiedAvoidancePolygon_804F4750(
        int mode, const nlVector3& center, float length, float width);
    virtual ~UnidentifiedAvoidancePolygon_804F4750();
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity()
    {
        return v3Zero;
    }
    virtual float GetRadius()
    {
        return 0.0f;
    }
    virtual bool UnidentifiedVirtual1C(
        const nlVector3& target, nlVector3& point, nlVector3& dir);
    virtual bool UnidentifiedVirtual20(
        UnidentifiedAvoidanceObject* other, float range);

    /* 0x14 */ int mUnidentified014;
    /* 0x18 */ nlVector2 mPoints[4];
    /* 0x38 */ nlVector2 mNormals[4];
    /* 0x58 */ nlVector3 mCenter;
    /* 0x64 */ cFielder* mUnidentified064;
}; // size: 0x68

extern "C" int fn_8000CEF8(eAvoidableThings avoidable);
extern "C" int fn_8000CFD4(int index);

#endif // GAME_AI_UNIDENTIFIED_AVOIDANCE_OBJECT_H
