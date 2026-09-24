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
    sWarbleConfiguration.values18[0] = 3.0f;
    sWarbleConfiguration.values18[1] = 12.0f;
    sWarbleConfiguration.values18[2] = 32.0f;
    sWarbleConfiguration.values18[3] = 32.0f;
    sWarbleConfiguration.values18[4] = 1.0f;
    sWarbleConfiguration.values18[5] = 0.0f;
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

    bool expired = instance->UnidentifiedQuery();
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
