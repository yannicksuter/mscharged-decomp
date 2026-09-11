#ifndef GAME_RENDER_TU_8027944C_H
#define GAME_RENDER_TU_8027944C_H

#include "Game/World/WorldPhysicsDescription.h"

class PhysicsObject;

class StadiumPhysicsObject_8027944C
{
public:
    virtual ~StadiumPhysicsObject_8027944C();
    virtual void fn_80279730();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4* transform);
    virtual void V4(void* world);
    virtual void fn_80279700(void* context);

    /* 0x04 */ u8 m_Unknown04[0x1C];
    /* 0x20 */ WorldPhysicsDescription_80341EEC m_Description;
    /* 0x74 */ u8 m_Unknown74[0x0C];
    /* 0x80 */ PhysicsObject* m_pPhysicsObject;
};

class StadiumMarker_802799AC
{
public:
    virtual ~StadiumMarker_802799AC();
    virtual void fn_80279A24();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4* transform);
    virtual void V4(void* world);
    virtual void fn_802799AC(void* context);
};

class StadiumMarker_8027999C
{
public:
    virtual ~StadiumMarker_8027999C();
    virtual void fn_802799A8();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4* transform);
    virtual void V4(void* world);
    virtual void fn_8027999C(void* context);
};

#endif
