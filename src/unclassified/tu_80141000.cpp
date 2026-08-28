#include "Game/AI/Powerups.h"
#include "Game/GameTweaks.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsBanana.h"

PhysicsBanana::PhysicsBanana(float radius)
    : PhysicsSphere(g_CollisionSpace, g_PhysicsWorld, radius)
{
    m_pTriggerCallbackFunc = 0;
    m_pCallbackParam = 0;
    m_pPowerupObject = 0;
    m_bIsSupportedByGround = false;
    mUnidentified045[0] = 0;

    SetCollide(0x1F062);
    SetCategory(0x2000);
    m_gravity = -16.0f;
}

void PhysicsBanana::PreCollide()
{
    m_bIsSupportedByGround = false;
}

bool PhysicsBanana::SetContactInfo(dContact* contact, PhysicsObject* other, bool first)
{
    if (first)
    {
        SetDefaultContactInfo(contact);
    }

    if (other->GetObjectType() == 0x12)
    {
        contact->surface.bounce = lbl_8056CF08.m_pGameTweaks->fShellBounceGround;
    }
    else
    {
        contact->surface.bounce = 0.001f;
    }
    contact->surface.bounce_vel = 0.0f;
    contact->surface.mu = 100.0f;
    return true;
}

PhysicsBanana::~PhysicsBanana()
{
}
