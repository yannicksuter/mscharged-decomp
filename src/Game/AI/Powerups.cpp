#include "Game/AI/Powerups.h"

#include "Game/Physics/PhysicsSphere.h"

namespace
{
struct Pair
{
    /* 0x0 */ unsigned long hashId;
    /* 0x4 */ const PowerupBase* powerup;
}; // total size: 0x8

struct PowerupRegistry
{
    PowerupRegistry()
    {
        for (int i = 0; i < NUM_POWERUP_REGISTRY_ENTRIES; i++)
        {
            registry[i].hashId = 0;
        }
    }

    enum
    {
        NUM_POWERUP_REGISTRY_ENTRIES = 25
    };

    /* 0x0 */ Pair registry[NUM_POWERUP_REGISTRY_ENTRIES];
}; // total size: 0xC8

PowerupRegistry powerupRegistry;
}

/**
 * Offset/Address/Size: 0x2214 | 0x8009B874 | size: 0x8
 */
float PowerupBase::GetRadius() const
{
    return ((PhysicsSphere*)m_pPhysicsObject)->GetRadius();
}

static inline PowerupBase* FindPowerUpImpl(unsigned long hashOfDrawable)
{
    const Pair* entry = powerupRegistry.registry;
    for (int i = 0; i < 25; ++i)
    {
        if (hashOfDrawable == entry->hashId)
        {
            return const_cast<PowerupBase*>(powerupRegistry.registry[i].powerup);
        }
        ++entry;
    }
    return 0;
}

/**
 * Offset/Address/Size: 0x15FC | 0x8009AC5C | size: 0x4C
 */
PowerupBase* FindPowerUp(unsigned long hashOfDrawable)
{
    return FindPowerUpImpl(hashOfDrawable);
}
