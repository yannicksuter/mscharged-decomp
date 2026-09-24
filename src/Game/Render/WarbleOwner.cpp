#include "Game/Render/Warble.h"
#include "Game/Render/RLView.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

static WarbleConfiguration sWarbleConfiguration;

void InitializeWarble(WarbleOwner* owner)
{
    owner->instance = 0;
    nlZeroMemory(&sWarbleConfiguration, sizeof(sWarbleConfiguration));
    sWarbleConfiguration.view = GetLayerView(eCLV_Warble);
    sWarbleConfiguration.blobScale = 5.0f;
    sWarbleConfiguration.duration = 2.0f;
    sWarbleConfiguration.parameters[0] = 3.0f;
    sWarbleConfiguration.parameters[1] = 12.0f;
    sWarbleConfiguration.parameters[2] = 32.0f;
    sWarbleConfiguration.parameters[3] = 32.0f;
    sWarbleConfiguration.parameters[4] = 1.0f;
    sWarbleConfiguration.parameters[5] = 0.0f;
}

static inline void ClearWarble(WarbleOwner* owner)
{
    if (owner->instance != 0)
    {
        delete owner->instance;
        owner->instance = 0;
    }
}

void ShutdownWarble(WarbleOwner* owner)
{
    ClearWarble(owner);
}

void ResetWarble(WarbleOwner* owner)
{
    ClearWarble(owner);
}

void UpdateWarble(WarbleOwner* owner, float dt)
{
    WarbleInstance* instance = owner->instance;
    if (instance == 0)
        return;

    bool expired = instance->IsExpired();
    if (expired)
    {
        delete instance;
        owner->instance = 0;
        return;
    }

    instance->elapsed += dt;
    if (instance->elapsed >= instance->duration)
        instance->active = true;
}

void RenderWarble(WarbleOwner* owner)
{
}

void SetWarbleInstance(
    WarbleOwner* owner, WarbleInstance* instance)
{
    owner->instance = instance;
}

WarbleInstance::WarbleInstance(const WarbleConfiguration& configuration)
{
    *static_cast<WarbleConfiguration*>(this) = configuration;
    elapsed = 0.0f;
    active = false;
}
