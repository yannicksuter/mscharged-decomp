#include "NL/nlDLListContainer.inl"
#include "Game/Physics/PhysicsTriggerVolume.h"

#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"

PhysicsPatchInfo gPhysicsPatchInfo[13] = {
    { PATCH_GAS_BALL, "GAS BALL", "gas_cloud", 0x13062, 0, 3.0f, 0.0f, 0.1f },
    { PATCH_FIRE_BALL, "FIRE BALL", "bowser_fire", 0x40, 0, 0.001f, 0.0f, 0.0f },
    { PATCH_HEAVENLY_LIGHT, "HEAVENLY LIGHT", "heavenly_light", 0x40, 0, 0.0f, 0.0f, 0.001f },
    { PATCH_MUCK_BALL, "MUCK BALL", "fx_mud_ball", 0x202, 0, 8.0f, 0.0f, 0.0f },
    { PATCH_MUCK_HOLE, "MUCK HOLE", "fx_mud_hole", 0x60, 0, 0.0f, 0.15f, 0.0f },
    { PATCH_YOSHI_YOKE, "YOSHI YOKE", "yoshi_yoke", 0x60, 0, 0.0f, 0.15f, 0.0f },
    { PATCH_YOSHI_TONGUE, "YOSHI TONGUE", 0, 0x40, 0, 0.0f, 0.0f, 0.0f },
    { PATCH_SHRINKER, "SHRINKER", "shrill_shrinker", 0x40, 0, 0.0f, 0.0f, 0.0f },
    { PATCH_LAVA_BALL, "LAVA BALL", "fx_lava_ball", 0x2, 0, 8.0f, 0.0f, 0.0f },
    { PATCH_LAVA_HOLE, "LAVA HOLE", "fx_lava_hole", 0x60, 0, 0.0f, 0.0f, 0.0f },
    { PATCH_CHAIN_LIGHTNING, "CHAINLIGHTNING", "fx_electric_ground_effect", 0x40, 0, 0.0f, 0.0f, 0.0f },
    { PATCH_SAND, "SANDPATCH", 0, 0x40, 0, 0.0f, 0.6f, 0.0f },
    { PATCH_SPEEDER, "SPEEDER", "yoshi_yoke", 0x40, 0, 0.0f, 1.5f, 0.001f },
};

PhysicsPatchInfo* GetPhysicsPatchInfo(const int& type)
{
    if (type > -1 && type < 13)
    {
        return &gPhysicsPatchInfo[type];
    }
    return 0;
}

PhysicsTriggerVolume::PhysicsTriggerVolume(float radius)
    : PhysicsSphere(g_CollisionSpace, 0, radius)
{
    m_pTriggerCallbackFunc = 0;
    m_pCallbackParam = 0;
    SetCollide(0x60);
    SetCategory(2);
}

PhysicsTriggerVolume::~PhysicsTriggerVolume()
{
}

ContactType PhysicsTriggerVolume::Contact(
    PhysicsObject* other, dContact* info, int numContacts)
{
    nlVector3 position;
    nlVec3Set(position, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);

    switch (other->GetObjectType())
    {
    case PHYSOBJ_NPC:
    {
        bool isChainChomp
            = ((SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC)
                  ->GetSkinAnimatedNPC_Type()
           == SkinAnimatedNPC_CHAIN_CHOMP;
        if (isChainChomp)
        {
            QueueCollisionChainCrowd(
                (ChainChomp*)((PhysicsNPC*)other)->mpAINPC);
        }
        break;
    }
    default:
    {
        if (m_pTriggerCallbackFunc != 0)
        {
            m_pTriggerCallbackFunc(
                this, other, position, m_pCallbackParam);
        }
        break;
    }
    }

    return NO_CONTACT;
}
