#ifndef GAME_RENDER_TU_802D88F4_H
#define GAME_RENDER_TU_802D88F4_H

#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorCharacter.h"
#include "Game/TweakValue.h"
#include "NL/nlMath.h"
#include "types.h"

// Crowd-layout render object used by the stadium crowd setup. The retail
// binary does not retain the original class name.
class UnidentifiedRenderObject_801A4188
{
public:
    virtual ~UnidentifiedRenderObject_801A4188();
    virtual void UnidentifiedVirtual0C();
    virtual nlMatrix4* UnidentifiedVirtual10();
    virtual void SetTransform(const nlMatrix4& transform);
    virtual void UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();

    /* 0x04 */ u8 mUnidentified004[0x1C];
    /* 0x20 */ nlMatrix4 mTransform;
    /* 0x60 */ float mUnidentified060;
    /* 0x64 */ float mUnidentified064;
    /* 0x68 */ float mUnidentified068;
    /* 0x6C */ float mUnidentified06C;
    /* 0x70 */ void* mUnidentified070;
    /* 0x74 */ u8 mUnidentified074[0x0C];
}; // size: 0x80

class CrowdTweak_801A4188
{
public:
    virtual bool UnidentifiedVirtual08(const Impostor* impostor);

    /* 0x04 */ TweakValueImpl_804F4DC8 value;
}; // size: 0x14

class UnidentifiedCrowdManager_802D88F4;

extern "C" void fn_802D8930(UnidentifiedRenderObject_801A4188* object);
UnidentifiedCrowdManager_802D88F4* GetCrowdImpostorManager();
extern "C" void fn_802D911C(UnidentifiedCrowdManager_802D88F4* manager,
    UnidentifiedRenderObject_801A4188* object, bool enabled);
extern "C" void fn_802D932C(UnidentifiedCrowdManager_802D88F4* manager,
    ImpostorCharacter* character);
extern "C" void fn_802D93E8(
    UnidentifiedCrowdManager_802D88F4* manager, int reload);
extern "C" void fn_802D9708(UnidentifiedCrowdManager_802D88F4* manager);
extern "C" void fn_802D9CD8(UnidentifiedCrowdManager_802D88F4* manager,
    CrowdTweak_801A4188* tweak);
void UpdateCrowdVisibility(
    UnidentifiedCrowdManager_802D88F4* manager, void* view);
void ReleaseCrowdImpostors(UnidentifiedCrowdManager_802D88F4* manager);

#endif // GAME_RENDER_TU_802D88F4_H
