#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioCalculation.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioSequenceEvent.h"
#include "Game/Audio/AudioSlider.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/XSoundCueHandle.h"
#include "NL/nlChunk.h"
#include "NL/nlSlotPool.h"

extern SlotPoolBase lbl_8057F9E8;
extern SlotPoolBase lbl_8057FA10;
extern SlotPoolBase lbl_8057FAA8;

extern "C" void* fn_802EFB70(nlChunk* chunk);
extern "C" void fn_802F00F0(void* controller, float dt);
extern "C" void fn_802F4904(void* controller, float dt);
extern "C" void fn_802F4958(void* controller);

bool UnidentifiedAudioBundleManager_802ECD34::Initialize()
{
    if (AudioBundleManager::Initialize())
        m_Initialized = true;
    return m_Initialized;
}

void UnidentifiedAudioBundleManager_802ECD34::Shutdown()
{
    fn_802B467C(&sSoundCueHandlePool);
    SlotPoolBase::BaseFreeBlocks(&sSoundCueHandlePool, 0x40);
    fn_802B467C(&lbl_8057FAA8);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057FAA8, 0x7C);
    sAudioSequenceInstancePool.FreeBlocks();
    fn_802B467C(&sSoundPlaybackEventPool);
    SlotPoolBase::BaseFreeBlocks(&sSoundPlaybackEventPool, 0x34);
    fn_802B467C(&sHitMarkerEventPool);
    SlotPoolBase::BaseFreeBlocks(&sHitMarkerEventPool, 0x18);
    fn_802B467C(&sParameterChangeEventPool);
    SlotPoolBase::BaseFreeBlocks(&sParameterChangeEventPool, 0x18);
    fn_802B467C(&lbl_8057FA10);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057FA10, 0xC);
    fn_802B467C(&lbl_8057F9E8);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057F9E8, 0x10);
    fn_802F4958(&m_Runtime);

    m_Backend->Shutdown();
}

void UnidentifiedAudioBundleManager_802ECD34::ParseChunk(nlChunk* chunk)
{
    switch (chunk->GetID())
    {
    case 0x80023600:
        m_RpcController = fn_802EFB70(chunk);
        break;
    default:
        AudioBundleManager::ParseChunk(chunk);
        break;
    }
}

void UnidentifiedAudioBundleManager_802ECD34::Update(float dt)
{
    if (IsLoaded())
    {
        ((AudioSliderTable*)m_Chunk13100)->Update(dt);
        fn_802F00F0(m_RpcController, dt);
        fn_802F4904(&m_Runtime, dt);
        ((AudioCalculationTable*)m_Chunk13400)->Update(dt);
    }
    m_Backend->ServiceReadQueue(dt);
}
