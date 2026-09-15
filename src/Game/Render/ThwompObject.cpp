#include "Game/Sys/audio.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/RumbleActions.h"
#include "Game/Render/ThwompObject.h"

#include "Game/AI/Fielder.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Drawable/RenderObject.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Team.h"
#include "NL/gl/glState.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMath.h"
#include "ode/objects.h"
#include "Game/Physics/PhysicsThwomp.h"
#include "NL/gl/glTexture.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

extern "C"
{
    bool gThwompHideAfterRise = true;
    float gThwompTextureSwitchHeight = 2.0f;
    float gThwompLandingTime = 0.8f;
    float gThwompWarningTime = 0.2f;
    float gThwompFallGravity = -60.0f;
    float gThwompRiseSpeed = 1.25f;
    float gThwompRiseGravity = 4.65f;
    float gThwompScaleTime = 2.0f;
    float gThwompSpawnHeight = 25.0f;
    float gThwompCameraRumbleX = 0.1f;
    float gThwompCameraRumbleY = 0.125f;
    float gThwompCameraRumbleSpring = 4300.0f;
    float gThwompCameraRumbleDamping = 5.8f;

    bool gThwompAutoDrop;

    char gThwompTexture3Name[] = "gameplay/thwomp_3";
    char gThwompTexture2Name[] = "global/thwomp_2";
    char gThwompTexture1Name[] = "global/thwomp_1";
    char gThwompMovementSoundName[] = "ThwompUp";
    char gThwompLandingEffectName[] = "fx_thwompland";

    extern const nlVector3 gThwompHiddenPosition = { 0.0f, 0.0f, -100.0f };
    extern const nlVector3 gWindDebrisZeroVelocity = { 0.0f, 0.0f, 0.0f };
    extern const nlVector3 gWindDebrisHiddenPosition = { 0.0f, 0.0f, -10.0f };
    extern const nlVector3 gYoshiEggZeroDisplacement = { 0.0f, 0.0f, 0.0f };
    extern const nlVector3 gYoshiEggHiddenPosition = { 0.0f, -20.0f, -20.0f };

    void fn_802B5370(
        nlQuaternion&, const nlVector3&, unsigned short);

}

static inline void ApplyTexture(ThwompObject* object,
    unsigned long texture, unsigned long resolvedTexture)
{
    if (object->mDiffusePacket != 0)
    {
        glSetMaterialTextureParameter(object->mDiffusePacket, gDiffuseTextureSemantic, texture);
        unsigned long resolved = resolvedTexture;
        glSetMaterialTextureIndexParameter(
            object->mDiffusePacket, gDiffuseTextureSemantic, &resolved);
    }

    if (object->mGlossPacket != 0)
    {
        glSetMaterialTextureParameter(object->mGlossPacket, gGlossTextureSemantic, texture);
        unsigned long resolved = resolvedTexture;
        glSetMaterialTextureIndexParameter(
            object->mGlossPacket, gGlossTextureSemantic, &resolved);
    }
}

ThwompObject::ThwompObject(int index)
{
    mIndex = index;
    mState = THWOMP_STATE_HIDDEN;
    mVisible = false;
    mDrawable = 0;
    mAvoidable = 0;
    mDelayTimer = 0.0f;
    mLandingTimer = 0.0f;
    mWarningTimer = 0.0f;
    mScaleTimer = 0.0f;
    mUnidentified028 = -1;

    mTexture3 = glGetTexture(gThwompTexture3Name);
    mTexture2 = glGetTexture(gThwompTexture2Name);
    mTexture1 = glGetTexture(gThwompTexture1Name);
    mDrawable = GetRenderObject(8, index);

    PhysicsObject* physics
        = new PhysicsThwomp(this, 3.14f, 2.88f, 3.5f);
    mPhysics = physics;
    physics->SetPosition(
        gThwompHiddenPosition, PhysicsObject::WORLD_COORDINATES);

    SetState(THWOMP_STATE_HIDDEN);

    mDiffusePacket = 0;
    mGlossPacket = 0;
    mTexture3Index
        = glGetTextureManager()->GetTextureIndex(mTexture3);
    mTexture2Index
        = glGetTextureManager()->GetTextureIndex(mTexture2);
    mTexture1Index
        = glGetTextureManager()->GetTextureIndex(mTexture1);

    for (glModelPacket* packet = mDrawable->m_pModel->packets;
         packet < mDrawable->m_pModel->packets
                + mDrawable->m_pModel->numPackets;
         ++packet)
    {
        if (glHasMaterialParameter(packet, gDiffuseTextureSemantic))
        {
            unsigned long texture
                = glGetMaterialUnsignedParameter(packet, gDiffuseTextureSemantic);
            if (texture == mTexture3)
            {
                mDiffusePacket = packet;
            }
        }
        if (glHasMaterialParameter(packet, gGlossTextureSemantic))
        {
            unsigned long texture
                = glGetMaterialUnsignedParameter(packet, gGlossTextureSemantic);
            if (texture == mTexture3)
            {
                mGlossPacket = packet;
            }
        }
    }
}

ThwompObject::~ThwompObject()
{
    delete mAvoidable;
    if (mPhysics != 0)
    {
        delete mPhysics;
        mPhysics = 0;
    }
}

void ThwompObject::Freeze(float)
{
}

void ThwompObject::Update(float dt)
{
    if (mDelayTimer > 0.0f)
    {
        mDelayTimer -= dt;
        return;
    }

    if (mVisible)
    {
        if (mState == THWOMP_STATE_APPEARING
            || mState == THWOMP_STATE_DISAPPEARING)
        {
            mScaleTimer -= dt;
            if (mScaleTimer <= 0.0f)
            {
                mScaleTimer = 0.0f;
                if (mState == THWOMP_STATE_APPEARING)
                {
                    SetState(THWOMP_STATE_IDLE);
                    if (gThwompAutoDrop == true)
                    {
                        SetState(THWOMP_STATE_WARNING);
                    }
                }
                else if (mState == THWOMP_STATE_DISAPPEARING)
                {
                    SetState(THWOMP_STATE_HIDDEN);
                }
            }
        }
        else if (mState == THWOMP_STATE_WARNING)
        {
            mWarningTimer -= dt;
            if (mWarningTimer <= 0.0f)
            {
                mWarningTimer = 0.0f;
                SetState(THWOMP_STATE_FALLING);
            }
        }
        else if (mState == THWOMP_STATE_LANDED)
        {
            mLandingTimer -= dt;
            if (mLandingTimer <= 0.0f)
            {
                mLandingTimer = 0.0f;
                SetState(THWOMP_STATE_RISING);
            }
        }
        else if (mState == THWOMP_STATE_RISING
            || mState == THWOMP_STATE_FORCED_RISING)
        {
            if (mPhysics->GetPosition().z > gThwompSpawnHeight)
            {
                SetState(THWOMP_STATE_IDLE);
                if (gThwompHideAfterRise == true
                    || mState == THWOMP_STATE_FORCED_RISING)
                {
                    SetState(THWOMP_STATE_DISAPPEARING);
                }
            }
        }
    }

    if (mAvoidable != 0
        && mState != THWOMP_STATE_WARNING
        && mState != THWOMP_STATE_FALLING
        && mState != THWOMP_STATE_LANDED
        && mPhysics->GetPosition().z >= 5.0f)
    {
        delete mAvoidable;
        mAvoidable = 0;
    }
}

void ThwompObject::Spawn(float x, float y)
{
    nlVector3 position = { x, y, gThwompSpawnHeight };
    mPhysics->SetPosition(
        position, PhysicsObject::WORLD_COORDINATES);

    nlVector3 velocity = { 0.0f, 0.0f, 0.0f };
    mPhysics->SetLinearVelocity(velocity);
    SetState(THWOMP_STATE_APPEARING);
}

void ThwompObject::Stop(bool immediate)
{
    if (mState == THWOMP_STATE_HIDDEN)
    {
        return;
    }

    mLandingTimer = 0.0f;
    mScaleTimer = 0.0f;
    mWarningTimer = 0.0f;
    if (immediate == true)
    {
        SetState(THWOMP_STATE_HIDDEN);
    }
    else if (mState == THWOMP_STATE_IDLE)
    {
        SetState(THWOMP_STATE_DISAPPEARING);
    }
    else
    {
        SetState(THWOMP_STATE_FORCED_RISING);
    }
}

void ThwompObject::SetState(eThwompState state)
{
    if (state == THWOMP_STATE_APPEARING)
    {
        mVisible = true;
        mScaleTimer = gThwompScaleTime;
        mState = THWOMP_STATE_APPEARING;
    }
    else if (state == THWOMP_STATE_IDLE)
    {
        mPhysics->m_gravity = 0.0f;
        mPhysics->SetLinearVelocity(v3Zero);

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
        dBodySetQuaternion(mPhysics->m_bodyID, bodyOrientation);
        mPhysics->SetAngularVelocity(v3Zero);
        mPhysics->DisableCollisions();
        mState = THWOMP_STATE_IDLE;
    }
    else if (state == THWOMP_STATE_WARNING)
    {
        mWarningTimer = gThwompWarningTime;
        mState = THWOMP_STATE_WARNING;
        mAvoidable = new (nlMalloc(
            sizeof(AvoidablePolygon), 8, false))
            AvoidablePolygon(
                3, mPhysics->GetPosition(), 3.14f, 2.88f);
    }
    else if (state == THWOMP_STATE_FALLING)
    {
        mPhysics->m_gravity = gThwompFallGravity;
        mPhysics->EnableCollisions();
        mState = THWOMP_STATE_FALLING;
        unsigned long soundID = 0x014CC818;
        StopSound(soundID, this);
        PlaySound(11, soundID, gThwompMovementSoundName, this);
    }
    else if (state == THWOMP_STATE_LANDED)
    {
        mLandingTimer = gThwompLandingTime;
        mState = THWOMP_STATE_LANDED;
        EmissionController* controller = fn_802E7DC4(
            EmissionManager::Instance(), gThwompLandingEffectName, 2, true, false);
        controller->SetPosition(mPhysics->GetPosition());
        controller->SetVelocity(v3Zero);
    }
    else if (state == THWOMP_STATE_RISING)
    {
        nlVector3 velocity = { 0.0f, 0.0f, gThwompRiseSpeed };
        mPhysics->SetLinearVelocity(velocity);
        mPhysics->m_gravity = gThwompRiseGravity;
        mState = THWOMP_STATE_RISING;
        unsigned long soundID = 0xCC0C89C5;
        StopSound(soundID, this);
        PlaySound(11, soundID, gThwompMovementSoundName, this);
    }
    else if (state == THWOMP_STATE_HIDDEN)
    {
        mVisible = false;
        mPhysics->m_gravity = 0.0f;
        mPhysics->SetLinearVelocity(v3Zero);
        mPhysics->SetPosition(
            gThwompHiddenPosition, PhysicsObject::WORLD_COORDINATES);
        mPhysics->SetAngularVelocity(v3Zero);
        mPhysics->DisableCollisions();
        mState = THWOMP_STATE_HIDDEN;
    }
    else if (state == THWOMP_STATE_DISAPPEARING)
    {
        mScaleTimer = gThwompScaleTime;
        mState = THWOMP_STATE_DISAPPEARING;
    }
    else if (state == THWOMP_STATE_FORCED_RISING)
    {
        nlVector3 velocity = { 0.0f, 0.0f, gThwompRiseSpeed };
        mPhysics->SetLinearVelocity(velocity);
        mPhysics->m_gravity = gThwompRiseGravity;
        mPhysics->SetAngularVelocity(v3Zero);
        mState = THWOMP_STATE_FORCED_RISING;
    }
}

const nlVector3* ThwompObject::GetPosition() const
{
    return &mPhysics->GetPosition();
}

void ThwompObject::OnLanding()
{
    if (mState == THWOMP_STATE_FALLING)
    {
        SetState(THWOMP_STATE_LANDED);
        for (int team = 0; team < 2; ++team)
        {
            cTeam* pTeam = g_pTeams[team];
            for (int fielder = 0; fielder < 4; ++fielder)
            {
                cFielder* player = pTeam->GetFielder(fielder);
                if (player->GetGlobalPad() != 0)
                {
                    PlayRumbleAction(1, player->GetGlobalPad());
                }
            }
        }
        FireCameraRumbleFilter(gThwompCameraRumbleX, gThwompCameraRumbleY,
            gThwompCameraRumbleSpring, gThwompCameraRumbleDamping);
        unsigned long soundID = 0x9320C77E;
        StopSound(soundID, this);
        PlaySound(11, soundID, gThwompMovementSoundName, this);
    }
}

float ThwompObject::GetScale() const
{
    float scale = 1.0f;
    if (mState != THWOMP_STATE_APPEARING
        && mState != THWOMP_STATE_DISAPPEARING)
    {
        return scale;
    }

    scale = mScaleTimer / gThwompScaleTime;
    if (mState == THWOMP_STATE_APPEARING)
    {
        scale = 1.0f - scale;
    }
    return scale;
}

void ThwompObject::UpdateTexture()
{
    unsigned long texture;
    unsigned long resolvedTexture;

    switch (mState)
    {
    case THWOMP_STATE_HIDDEN:
    case THWOMP_STATE_APPEARING:
    case THWOMP_STATE_IDLE:
    case THWOMP_STATE_WARNING:
    case THWOMP_STATE_FALLING:
    case THWOMP_STATE_LANDED:
        resolvedTexture = mTexture3Index;
        texture = mTexture3;
        ApplyTexture(this, texture, resolvedTexture);
        break;
    case THWOMP_STATE_RISING:
        if (mPhysics->GetPosition().z >= gThwompTextureSwitchHeight)
        {
            resolvedTexture = mTexture1Index;
            texture = mTexture1;
            ApplyTexture(this, texture, resolvedTexture);
        }
        else
        {
            resolvedTexture = mTexture2Index;
            texture = mTexture2;
            ApplyTexture(this, texture, resolvedTexture);
        }
        break;
    case THWOMP_STATE_DISAPPEARING:
    case THWOMP_STATE_FORCED_RISING:
    case THWOMP_STATE_8:
        resolvedTexture = mTexture1Index;
        texture = mTexture1;
        ApplyTexture(this, texture, resolvedTexture);
        break;
    }
}
