#include "unclassified/tu_801B535C.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsSphere_801798A8.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/ReplayManager.h"
#include "Game/Sys/audio.h"
#include "NL/nlMemory.h"

extern "C"
{
    extern const nlVector3 lbl_804DCF98;
    extern const nlVector3 lbl_804DCFA4;
    float lbl_806DD148 = 1.0f;
    float lbl_806DD14C = 0.5f;
    float lbl_806DD150 = 1.4f;
    float lbl_806E16F0;

    void fn_802B5370(nlQuaternion&, const nlVector3&, unsigned short);
}

UnidentifiedObject_801B535C::UnidentifiedObject_801B535C(RenderObject* param)
{
    mUnidentified30 = param;
    mUnidentified1C = 1.0f;
    mUnidentified20 = 1.0f;
    mUnidentified24 = 0.0f;
    mUnidentified28 = false;
    mUnidentified34 = 0;
    mUnidentified38 = 0.0f;
    mUnidentified3C = lbl_804DCF98;
    mUnidentified48 = 0.0f;
    mUnidentified10 = lbl_804DCFA4;
    mUnidentified00.x = mUnidentified00.y = mUnidentified00.z = 0.0f;
    mUnidentified00.w = 1.0f;
    mUnidentified2C = new (8, false) PhysicsSphere_801798A8(this, 1.0f);
    mUnidentified2C->SetPosition(lbl_804DCFA4, PhysicsObject::WORLD_COORDINATES);
    mUnidentified2C->DisableCollisions();
}

UnidentifiedObject_801B535C::~UnidentifiedObject_801B535C()
{
    delete mUnidentified2C;
}

void UnidentifiedObject_801B535C::fn_801B54AC(bool, float param)
{
    if (mUnidentified28)
    {
        mUnidentified38 = param;
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
        EmissionManager::Instance()->Destroy((unsigned long)this, group);
        mUnidentified2C->EnableCollisions();
        if (mUnidentified34 != 0 && mUnidentified34->m_pBall != 0)
        {
            mUnidentified34->ReleaseBall(0);
            g_pBall->m_pPhysicsBall->EnableCollisions();
        }
    }
}

void UnidentifiedObject_801B535C::fn_801B5544(float param)
{
    if (!mUnidentified28)
        return;

    if (mUnidentified38 > 0.0f)
    {
        mUnidentified38 -= param;
        if (mUnidentified38 <= 0.0f)
        {
            EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
            EmissionController* controller = EmissionManager::Instance()->Create(group, 0, true, 0);
            controller->SetPosition(mUnidentified10);
            controller->m_uUserData = (unsigned long)this;
            controller->SetUpdateCallback(fn_801B57D8);
            mUnidentified2C->EnableCollisions();
        }
        else
        {
            return;
        }
    }

    if (mUnidentified24 > 0.0f)
    {
        mUnidentified24 -= param;
        if (mUnidentified24 <= 0.0f)
        {
            mUnidentified24 = 0.0f;
            mUnidentified1C = mUnidentified20;
        }
        else
        {
            float factor = param / mUnidentified24;
            if (factor > 1.0f)
                factor = 1.0f;
            mUnidentified1C = Interpolate(mUnidentified1C, mUnidentified20, factor);
        }
        mUnidentified2C->SetRadius(mUnidentified1C);
    }

    if (mUnidentified48 != 0.0f)
    {
        nlVector3 position = mUnidentified34->m_v3Position;
        nlVector3 offset;
        nlVec3Scale(offset, mUnidentified3C, mUnidentified48);
        if (nlAbs(offset.x) <= 0.01f && nlAbs(offset.y) <= 0.01f)
            offset.y = -0.01f;
        position.y += offset.y;
        position.x += offset.x;
        position.z = 0.0f;
        mUnidentified34->SetPosition(position);
        mUnidentified48 = 0.0f;
    }

    fn_801B5B38(param);
    if (nlAbs(mUnidentified10.y) > cField::GetSidelineY(1))
    {
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
        EmissionManager::Instance()->Kill((unsigned long)this, group);
    }
}

void UnidentifiedObject_801B535C::fn_801B57D8(EmissionController& param)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
        return;
    if (param.m_GlView == 0 && ReplayManager::Instance()->mRender != 0)
    {
        RenderSnapshot* snapshot = ReplayManager::Instance()->mRender;
        if (snapshot->_1BA0.mVisible)
            param.SetPosition(snapshot->_1BA0.mPosition);
    }
}

void UnidentifiedObject_801B535C::fn_801B5858(cFielder* param)
{
    mUnidentified34 = param;
    if (param != 0)
        fn_802B549C(mUnidentified00, param->m_aActualFacingDirection);
    mUnidentified20 = lbl_806DD14C;
    mUnidentified24 = 0.0f;
    mUnidentified1C = mUnidentified20;
    mUnidentified2C->SetRadius(mUnidentified1C);
    mUnidentified20 = lbl_806DD150;
    mUnidentified24 = 0.2f;
    if (mUnidentified28)
    {
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
        EmissionManager::Instance()->Kill((unsigned long)this, group);
    }
    mUnidentified28 = true;
    mUnidentified2C->EnableCollisions();
    fn_801B5B38(0.0f);
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
    EmissionController* controller = EmissionManager::Instance()->Create(group, 0, true, 0);
    controller->SetPosition(mUnidentified10);
    controller->m_uUserData = (unsigned long)this;
    controller->SetUpdateCallback(fn_801B57D8);
    if (param->m_pBall != 0)
    {
        g_pBall->m_pPhysicsBall->DisableCollisions();
        g_pBall->m_bVisible = false;
        param->m_pPhysicsCharacter->DisablePhysicsColumn();
    }
}

void UnidentifiedObject_801B535C::fn_801B59DC(bool param)
{
    if (mUnidentified28)
    {
        if (mUnidentified34 == g_pBall->m_pOwner)
        {
            g_pBall->m_pPhysicsBall->EnableCollisions();
            g_pBall->m_bVisible = true;
            mUnidentified34->m_pPhysicsCharacter->EnablePhysicsColumn();
        }
        if (!param)
        {
            PlaySound(mUnidentified34->mUnidentified318, 0x1B274A7A, 0, 0);
        }
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
        if (!param)
            EmissionManager::Instance()->Kill((unsigned long)this, group);
        else
            EmissionManager::Instance()->Destroy((unsigned long)this, group);
        mUnidentified10 = lbl_804DCFA4;
        mUnidentified2C->SetPosition(lbl_804DCFA4, PhysicsObject::WORLD_COORDINATES);
        if (mUnidentified28)
        {
            EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
            EmissionManager::Instance()->Kill((unsigned long)this, group);
        }
        mUnidentified28 = false;
    }
    mUnidentified2C->DisableCollisions();
    mUnidentified24 = 0.0f;
    mUnidentified38 = 0.0f;
}

float UnidentifiedObject_801B535C::fn_801B5B30() const
{
    return mUnidentified1C;
}

void UnidentifiedObject_801B535C::fn_801B5B38(float param)
{
    cFielder* player = mUnidentified34;
    nlVector3 position = player->m_v3Position;
    position.z += mUnidentified1C + lbl_806E16F0;
    nlVector3 foot = player->GetJointPosition(player->m_nLeftFootJointIndex);
    nlVector3 head = player->GetJointPosition(player->m_nHeadJointIndex);
    float height = 0.5f * (foot.z + head.z);
    if (height > position.z)
        position.z = height;
    mUnidentified10 = position;
    mUnidentified2C->SetPosition(position, PhysicsObject::WORLD_COORDINATES);
    if (param > 0.0f)
    {
        nlQuaternion rotation;
        nlVector3 axis;
        nlVector3 displacement;
        float sine, cosine;
        nlSinCos(&sine, &cosine, player->m_aActualFacingDirection + 0x4000);
        nlVec3Set(axis, cosine, sine, 0.0f);
        nlVec3Scale(displacement, player->m_v3Velocity, param);
        float angle = nlVec3Length(displacement) * lbl_806DD148 / mUnidentified1C;
        fn_802B5370(rotation, axis, (unsigned short)(10430.378f * angle));
        nlMultQuat(mUnidentified00, rotation, mUnidentified00);
    }
    else
    {
        mUnidentified00.x = mUnidentified00.y = mUnidentified00.z = 0.0f;
        mUnidentified00.w = 1.0f;
    }
}

void UnidentifiedObject_801B535C::fn_801B5D14()
{
    fn_801B59DC(true);
    mUnidentified10 = lbl_804DCFA4;
    mUnidentified00.x = mUnidentified00.y = mUnidentified00.z = 0.0f;
    mUnidentified00.w = 1.0f;
    mUnidentified1C = 1.0f;
    mUnidentified20 = 1.0f;
    mUnidentified24 = 0.0f;
    mUnidentified34 = 0;
    mUnidentified38 = 0.0f;
    mUnidentified2C->SetRadius(0.1f);
    mUnidentified2C->SetPosition(mUnidentified10, PhysicsObject::WORLD_COORDINATES);
    mUnidentified48 = 0.0f;
    mUnidentified3C = lbl_804DCF98;
}

void UnidentifiedObject_801B535C::fn_801B5DD0()
{
    fn_801B59DC(false);
}

void UnidentifiedObject_801B535C::fn_801B5DD8(const nlVector3& param, float value)
{
    mUnidentified3C = param;
    mUnidentified48 = value;
}
