#include "unclassified/tu_801B298C.h"

#include "Game/AI/Fielder.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Drawable/RenderObject.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Team.h"
#include "NL/gl/glState.h"
#include "NL/nlMath.h"
#include "ode/objects.h"
#include "unclassified/tu_80176EF4.h"

extern "C"
{
    extern unsigned long lbl_806E1F0C;
    extern unsigned long lbl_806E1F28;

    bool lbl_806DD0F8 = true;
    float lbl_806DD0FC = 2.0f;
    float lbl_806DD100 = 0.8f;
    float lbl_806DD104 = 0.2f;
    float lbl_806DD108 = -60.0f;
    float lbl_806DD10C = 1.25f;
    float lbl_806DD110 = 4.65f;
    float lbl_806DD114 = 2.0f;
    float lbl_806DD118 = 25.0f;
    float lbl_806DD11C = 0.1f;
    float lbl_806DD120 = 0.125f;
    float lbl_806DD124 = 4300.0f;
    float lbl_806DD128 = 5.8f;

    bool lbl_806E16B8;

    char lbl_805146F8[] = "gameplay/thwomp_3";
    char lbl_80514710[] = "global/thwomp_2";
    char lbl_80514720[] = "global/thwomp_1";
    char lbl_80514730[] = "ThwompUp";
    char lbl_8051473C[] = "fx_thwompland";

    extern const nlVector3 lbl_804DCF68 = { 0.0f, 0.0f, 0.0f };
    extern const nlVector3 lbl_804DCF74 = { 0.0f, 0.0f, -100.0f };
    extern const nlVector3 lbl_804DCF80 = { 0.0f, 0.0f, 0.0f };
    extern const nlVector3 lbl_804DCF8C = { 0.0f, 0.0f, -10.0f };
    extern const nlVector3 lbl_804DCF98 = { 0.0f, 0.0f, 0.0f };
    extern const nlVector3 lbl_804DCFA4 = { 0.0f, -20.0f, -20.0f };
    extern const nlVector3 lbl_804DCFB0 = { 0.0f, 0.0f, 0.0f };
    extern const nlVector3 lbl_804DCFBC = { 0.0f, 0.0f, 1.0f };
    extern const nlVector4 lbl_804DCFC8 = { 0.1f, 0.08f, 0.0f, 0.0f };

    RenderObject* fn_80276360(int, int);
    void* fn_802CDF0C();
    unsigned long fn_802CE1B8(void*, unsigned long);
    bool fn_802CC8FC(glModelPacket*, unsigned long);
    unsigned long fn_802CC7E4(glModelPacket*, unsigned long);
    void fn_802CC458(
        glModelPacket*, unsigned long, unsigned long);
    void fn_802CC4FC(
        glModelPacket*, unsigned long, const unsigned long*);
    UnidentifiedThwompMember_801B2EAC* fn_8000E110(
        void*, int, const nlVector3*, float, float);
    EmissionController* fn_802E7DC4(
        EmissionManager*, const char*, int, bool, bool);
    void fn_802B5370(
        nlQuaternion&, const nlVector3&, unsigned short);
    void fn_80139D1C(int, void*);
    void fn_800EC12C(unsigned long, void*);
    bool fn_800EBBFC(
        int, unsigned long, const void*, void*);
}

static inline void ApplyTexture(ThwompObject* object,
    unsigned long texture, unsigned long resolvedTexture)
{
    if (object->mPacket48 != 0)
    {
        fn_802CC458(object->mPacket48, lbl_806E1F0C, texture);
        unsigned long resolved = resolvedTexture;
        fn_802CC4FC(
            object->mPacket48, lbl_806E1F0C, &resolved);
    }

    if (object->mPacket4C != 0)
    {
        fn_802CC458(object->mPacket4C, lbl_806E1F28, texture);
        unsigned long resolved = resolvedTexture;
        fn_802CC4FC(
            object->mPacket4C, lbl_806E1F28, &resolved);
    }
}

extern "C" ThwompObject* fn_801B298C(
    ThwompObject* object, int index)
{
    object->mIndex = index;
    object->mState = -1;
    object->mVisible = false;
    object->mDrawable = 0;
    object->mUnidentified014 = 0;
    object->mUnidentified018 = 0.0f;
    object->mUnidentified01C = 0.0f;
    object->mUnidentified020 = 0.0f;
    object->mUnidentified024 = 0.0f;
    object->mUnidentified028 = -1;

    object->mTexture50 = glGetTexture(lbl_805146F8);
    object->mTexture58 = glGetTexture(lbl_80514710);
    object->mTexture60 = glGetTexture(lbl_80514720);
    object->mDrawable = fn_80276360(8, index);

    PhysicsObject* physics
        = new PhysicsBox_80176EF4(object, 3.14f, 2.88f, 3.5f);
    object->mPhysics = physics;
    physics->SetPosition(
        lbl_804DCF74, PhysicsObject::WORLD_COORDINATES);

    fn_801B2EAC(object, -1);

    object->mPacket48 = 0;
    object->mPacket4C = 0;
    object->mResolvedTexture54
        = fn_802CE1B8(fn_802CDF0C(), object->mTexture50);
    object->mResolvedTexture5C
        = fn_802CE1B8(fn_802CDF0C(), object->mTexture58);
    object->mResolvedTexture64
        = fn_802CE1B8(fn_802CDF0C(), object->mTexture60);

    for (glModelPacket* packet = object->mDrawable->m_pModel->packets;
         packet < object->mDrawable->m_pModel->packets
                + object->mDrawable->m_pModel->numPackets;
         ++packet)
    {
        if (fn_802CC8FC(packet, lbl_806E1F0C))
        {
            unsigned long texture
                = fn_802CC7E4(packet, lbl_806E1F0C);
            if (texture == object->mTexture50)
            {
                object->mPacket48 = packet;
            }
        }
        if (fn_802CC8FC(packet, lbl_806E1F28))
        {
            unsigned long texture
                = fn_802CC7E4(packet, lbl_806E1F28);
            if (texture == object->mTexture50)
            {
                object->mPacket4C = packet;
            }
        }
    }
    return object;
}

extern "C" ThwompObject* fn_801B2B60(
    ThwompObject* object, int shouldDelete)
{
    if (object != 0)
    {
        delete object->mUnidentified014;
        if (object->mPhysics != 0)
        {
            delete object->mPhysics;
            object->mPhysics = 0;
        }
        if (shouldDelete > 0)
        {
            ::operator delete(object);
        }
    }
    return object;
}

extern "C" void fn_801B2BFC(ThwompObject*)
{
}

extern "C" void fn_801B2C00(
    ThwompObject* object, float dt)
{
    if (object->mUnidentified018 > 0.0f)
    {
        object->mUnidentified018 -= dt;
        return;
    }

    if (object->mVisible)
    {
        if (object->mState == 0 || object->mState == 6)
        {
            object->mUnidentified024 -= dt;
            if (object->mUnidentified024 <= 0.0f)
            {
                object->mUnidentified024 = 0.0f;
                if (object->mState == 0)
                {
                    fn_801B2EAC(object, 1);
                    if (lbl_806E16B8 == true)
                    {
                        fn_801B2EAC(object, 2);
                    }
                }
                else if (object->mState == 6)
                {
                    fn_801B2EAC(object, -1);
                }
            }
        }
        else if (object->mState == 2)
        {
            object->mUnidentified020 -= dt;
            if (object->mUnidentified020 <= 0.0f)
            {
                object->mUnidentified020 = 0.0f;
                fn_801B2EAC(object, 3);
            }
        }
        else if (object->mState == 4)
        {
            object->mUnidentified01C -= dt;
            if (object->mUnidentified01C <= 0.0f)
            {
                object->mUnidentified01C = 0.0f;
                fn_801B2EAC(object, 5);
            }
        }
        else if (object->mState == 5 || object->mState == 7)
        {
            if (object->mPhysics->GetPosition().z > lbl_806DD118)
            {
                fn_801B2EAC(object, 1);
                if (lbl_806DD0F8 == true || object->mState == 7)
                {
                    fn_801B2EAC(object, 6);
                }
            }
        }
    }

    if (object->mUnidentified014 != 0
        && object->mState != 2 && object->mState != 3
        && object->mState != 4
        && object->mPhysics->GetPosition().z >= 5.0f)
    {
        delete object->mUnidentified014;
        object->mUnidentified014 = 0;
    }
}

extern "C" void fn_801B2DF4(
    ThwompObject* object, float x, float y)
{
    nlVector3 position = { x, y, lbl_806DD118 };
    object->mPhysics->SetPosition(
        position, PhysicsObject::WORLD_COORDINATES);

    nlVector3 velocity = { 0.0f, 0.0f, 0.0f };
    object->mPhysics->SetLinearVelocity(velocity);
    fn_801B2EAC(object, 0);
}

extern "C" void fn_801B2E64(
    ThwompObject* object, bool reset)
{
    if (object->mState == -1)
    {
        return;
    }

    object->mUnidentified01C = 0.0f;
    object->mUnidentified024 = 0.0f;
    object->mUnidentified020 = 0.0f;
    if (reset == true)
    {
        fn_801B2EAC(object, -1);
    }
    else if (object->mState == 1)
    {
        fn_801B2EAC(object, 6);
    }
    else
    {
        fn_801B2EAC(object, 7);
    }
}

extern "C" void fn_801B2EAC(
    ThwompObject* object, int state)
{
    if (state == 0)
    {
        object->mVisible = true;
        object->mUnidentified024 = lbl_806DD114;
        object->mState = 0;
    }
    else if (state == 1)
    {
        object->mPhysics->m_gravity = 0.0f;
        object->mPhysics->SetLinearVelocity(lbl_804DCF68);

        nlQuaternion orientation;
        nlVector3 axis;
        axis.x = nlRandomf(2.0f) - 1.0f;
        axis.y = nlRandomf(2.0f) - 1.0f;
        axis.z = 0.0f;
        float lengthSquared = axis.GetLengthSq3D();
        if (lengthSquared > 0.01f)
        {
            float inverseLength = nlRecipSqrt(lengthSquared, false);
            nlVec3Scale(axis, axis, inverseLength);
        }
        else
        {
            nlVec3Set(axis, 1.0f, 0.0f, 0.0f);
        }

        float angleRadians = nlRandomf(0.04f) - 0.02f;
        unsigned short angle
            = (unsigned short)(int)(10430.378f * angleRadians);
        fn_802B5370(orientation, axis, angle);
        dQuaternion bodyOrientation;
        bodyOrientation[0] = orientation.x;
        bodyOrientation[1] = orientation.y;
        bodyOrientation[2] = orientation.z;
        bodyOrientation[3] = orientation.w;
        dBodySetQuaternion(object->mPhysics->m_bodyID, bodyOrientation);
        object->mPhysics->SetAngularVelocity(lbl_804DCF68);
        object->mPhysics->DisableCollisions();
        object->mState = 1;
    }
    else if (state == 2)
    {
        object->mUnidentified020 = lbl_806DD104;
        object->mState = 2;
        void* memory = nlMalloc(0x68, 8, false);
        UnidentifiedThwompMember_801B2EAC* member
            = (UnidentifiedThwompMember_801B2EAC*)memory;
        if (memory != 0)
        {
            member = fn_8000E110(memory, 3,
                &object->mPhysics->GetPosition(), 3.14f, 2.88f);
        }
        object->mUnidentified014 = member;
    }
    else if (state == 3)
    {
        object->mPhysics->m_gravity = lbl_806DD108;
        object->mPhysics->EnableCollisions();
        object->mState = 3;
        unsigned long soundID = 0x014CC818;
        fn_800EC12C(soundID, object);
        fn_800EBBFC(11, soundID, lbl_80514730, object);
    }
    else if (state == 4)
    {
        object->mUnidentified01C = lbl_806DD100;
        object->mState = 4;
        EmissionController* controller = fn_802E7DC4(
            EmissionManager::Instance(), lbl_8051473C, 2, true, false);
        controller->SetPosition(object->mPhysics->GetPosition());
        controller->SetVelocity(lbl_804DCF68);
    }
    else if (state == 5)
    {
        nlVector3 velocity = { 0.0f, 0.0f, lbl_806DD10C };
        object->mPhysics->SetLinearVelocity(velocity);
        object->mPhysics->m_gravity = lbl_806DD110;
        object->mState = 5;
        unsigned long soundID = 0xCC0C89C5;
        fn_800EC12C(soundID, object);
        fn_800EBBFC(11, soundID, lbl_80514730, object);
    }
    else if (state == -1)
    {
        object->mVisible = false;
        object->mPhysics->m_gravity = 0.0f;
        object->mPhysics->SetLinearVelocity(lbl_804DCF68);
        object->mPhysics->SetPosition(
            lbl_804DCF74, PhysicsObject::WORLD_COORDINATES);
        object->mPhysics->SetAngularVelocity(lbl_804DCF68);
        object->mPhysics->DisableCollisions();
        object->mState = -1;
    }
    else if (state == 6)
    {
        object->mUnidentified024 = lbl_806DD114;
        object->mState = 6;
    }
    else if (state == 7)
    {
        nlVector3 velocity = { 0.0f, 0.0f, lbl_806DD10C };
        object->mPhysics->SetLinearVelocity(velocity);
        object->mPhysics->m_gravity = lbl_806DD110;
        object->mPhysics->SetAngularVelocity(lbl_804DCF68);
        object->mState = 7;
    }
}

extern "C" const nlVector3* fn_801B327C(
    const ThwompObject* object)
{
    return &object->mPhysics->GetPosition();
}

extern "C" void fn_801B3284(ThwompObject* object)
{
    if (object->mState == 3)
    {
        fn_801B2EAC(object, 4);
        for (int team = 0; team < 2; ++team)
        {
            cTeam* pTeam = g_pTeams[team];
            for (int fielder = 0; fielder < 4; ++fielder)
            {
                cFielder* player = pTeam->GetFielder(fielder);
                if (player->GetGlobalPad() != 0)
                {
                    fn_80139D1C(1, player->GetGlobalPad());
                }
            }
        }
        FireCameraRumbleFilter(lbl_806DD11C, lbl_806DD120,
            lbl_806DD124, lbl_806DD128);
        unsigned long soundID = 0x9320C77E;
        fn_800EC12C(soundID, object);
        fn_800EBBFC(11, soundID, lbl_80514730, object);
    }
}

extern "C" float fn_801B3364(
    const ThwompObject* object)
{
    float scale = 1.0f;
    if (object->mState != 0 && object->mState != 6)
    {
        return scale;
    }

    scale = object->mUnidentified024 / lbl_806DD114;
    if (object->mState == 0)
    {
        scale = 1.0f - scale;
    }
    return scale;
}

extern "C" void fn_801B339C(ThwompObject* object)
{
    unsigned long texture;
    unsigned long resolvedTexture;

    switch (object->mState)
    {
    case -1:
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        resolvedTexture = object->mResolvedTexture54;
        texture = object->mTexture50;
        ApplyTexture(object, texture, resolvedTexture);
        break;
    case 5:
        if (object->mPhysics->GetPosition().z >= lbl_806DD0FC)
        {
            resolvedTexture = object->mResolvedTexture64;
            texture = object->mTexture60;
            ApplyTexture(object, texture, resolvedTexture);
        }
        else
        {
            resolvedTexture = object->mResolvedTexture5C;
            texture = object->mTexture58;
            ApplyTexture(object, texture, resolvedTexture);
        }
        break;
    case 6:
    case 7:
    case 8:
        resolvedTexture = object->mResolvedTexture64;
        texture = object->mTexture60;
        ApplyTexture(object, texture, resolvedTexture);
        break;
    }
}
