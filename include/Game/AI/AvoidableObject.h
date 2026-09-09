#ifndef GAME_AI_AVOIDABLE_OBJECT_H
#define GAME_AI_AVOIDABLE_OBJECT_H

#include "Game/AI/AvoidController.h"
#include "NL/nlMath.h"

class cFielder;
class cPlayer;
class PhysicsPatch;
class PowerupBase;
class ChainChomp;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

// Polymorphic "thing to avoid" registered with the AvoidController. The
// retail binary keeps the family in one translation unit.
class AvoidableObject
{
public:
    AvoidableObject(int type);
    virtual ~AvoidableObject();
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
        AvoidableObject* other, float range);
    virtual float UnidentifiedVirtual24(AvoidableObject* other)
    {
        return 1.0f;
    }
    virtual float UnidentifiedVirtual28(AvoidableObject* other)
    {
        return 1.0f;
    }
    virtual bool UnidentifiedVirtual2C()
    {
        return false;
    }

    /* 0x04 */ AvoidableObject* next;
    /* 0x08 */ int mUnidentified008;
    /* 0x0C */ int mType;
    /* 0x10 */ const float* mTweaks;
}; // size: 0x14

class AvoidableFielder : public AvoidableObject
{
public:
    AvoidableFielder(cFielder* pFielder)
        : AvoidableObject(AVOID_FIELDERS)
        , m_pFielder(pFielder)
    {
    }
    virtual ~AvoidableFielder()
    {
    }
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity();
    virtual float GetRadius();
    virtual float UnidentifiedVirtual18();
    virtual bool UnidentifiedVirtual20(
        AvoidableObject* other, float range);
    virtual float UnidentifiedVirtual24(AvoidableObject* other);
    virtual float UnidentifiedVirtual28(AvoidableObject* other);
    virtual bool UnidentifiedVirtual2C()
    {
        return true;
    }

    /* 0x14 */ cFielder* m_pFielder;
}; // size: 0x18

class AvoidableGoalie : public AvoidableObject
{
public:
    AvoidableGoalie(cPlayer* pPlayer)
        : AvoidableObject(AVOID_GOALIES)
        , m_pPlayer(pPlayer)
    {
    }
    virtual ~AvoidableGoalie()
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

class AvoidablePowerup : public AvoidableObject
{
public:
    AvoidablePowerup(PowerupBase* pPowerup)
        : AvoidableObject(AVOID_POWERUPS)
        , m_pPowerup(pPowerup)
        , m_pChainChomp(0)
    {
    }
    AvoidablePowerup(ChainChomp* pChainChomp)
        : AvoidableObject(AVOID_POWERUPS)
        , m_pPowerup(0)
        , m_pChainChomp(pChainChomp)
    {
    }
    virtual ~AvoidablePowerup()
    {
    }
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity();
    virtual float GetRadius();
    virtual bool UnidentifiedVirtual2C();

    /* 0x14 */ PowerupBase* m_pPowerup;
    /* 0x18 */ ChainChomp* m_pChainChomp;
}; // size: 0x1C

class AvoidablePoint : public AvoidableObject
{
public:
    AvoidablePoint(
        int type, const nlVector3& position, float radius)
        : AvoidableObject(type)
        , mPosition(position)
        , mRadius(radius)
    {
    }
    virtual ~AvoidablePoint()
    {
    }
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity();
    virtual float GetRadius();

    /* 0x14 */ nlVector3 mPosition;
    /* 0x20 */ float mRadius;
}; // size: 0x24

class AvoidablePatch : public AvoidableObject
{
public:
    AvoidablePatch(PhysicsPatch* pPatch)
        : AvoidableObject(AVOID_UNIDENTIFIED_20)
        , m_pPatch(pPatch)
    {
    }
    virtual ~AvoidablePatch()
    {
    }
    virtual const nlVector3& GetPosition();
    virtual const nlVector3& GetVelocity();
    virtual float GetRadius();
    virtual bool UnidentifiedVirtual2C();

    /* 0x14 */ PhysicsPatch* m_pPatch;
    /* 0x18 */ nlVector3 mUnidentified018;
}; // size: 0x24

class AvoidablePolygon : public AvoidableObject
{
public:
    AvoidablePolygon(
        int mode, const nlVector3& a, const nlVector3& b, float width);
    AvoidablePolygon(
        int mode, const nlVector3& center, float length, float width);
    virtual ~AvoidablePolygon();
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
        AvoidableObject* other, float range);

    /* 0x14 */ int mUnidentified014;
    /* 0x18 */ nlVector2 mPoints[4];
    /* 0x38 */ nlVector2 mNormals[4];
    /* 0x58 */ nlVector3 mCenter;
    /* 0x64 */ cFielder* mUnidentified064;
}; // size: 0x68

extern nlList<AvoidableObject> gAvoidableObjects;
extern float sUnidentifiedTweaks[7][7];

extern "C" int GetAvoidableIndex(eAvoidableThings avoidable);
extern "C" int GetAvoidableMask(int index);

#endif // GAME_AI_AVOIDABLE_OBJECT_H
