#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioCalculation.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioRpc.h"
#include "Game/Audio/AudioSequenceEvent.h"
#include "Game/Audio/AudioSlider.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/XSoundCueHandle.h"
#include "NL/nlChunk.h"
#include "NL/nlSlotPool.h"
#include "Game/Audio/SoundInstancePool.h"

bool AudioRpcBundleManager::Initialize()
{
    if (AudioBundleManager::Initialize())
        m_Initialized = true;
    return m_Initialized;
}

void AudioRpcBundleManager::Shutdown()
{
    OnSlotPoolStateRelease(&sSoundCueHandlePool);
    SlotPoolBase::BaseFreeBlocks(&sSoundCueHandlePool, 0x40);
    OnSlotPoolStateRelease(&sSoundInstancePool);
    SlotPoolBase::BaseFreeBlocks(&sSoundInstancePool, 0x7C);
    sAudioSequenceInstancePool.FreeBlocks();
    OnSlotPoolStateRelease(&sSoundPlaybackEventPool);
    SlotPoolBase::BaseFreeBlocks(&sSoundPlaybackEventPool, 0x34);
    OnSlotPoolStateRelease(&sHitMarkerEventPool);
    SlotPoolBase::BaseFreeBlocks(&sHitMarkerEventPool, 0x18);
    OnSlotPoolStateRelease(&sParameterChangeEventPool);
    SlotPoolBase::BaseFreeBlocks(&sParameterChangeEventPool, 0x18);
    OnSlotPoolStateRelease(&sAudioRpcListEntryPool);
    SlotPoolBase::BaseFreeBlocks(
        &sAudioRpcListEntryPool, sizeof(AudioRpcListEntry));
    OnSlotPoolStateRelease(&sAudioRpcRuntimeNodePool);
    SlotPoolBase::BaseFreeBlocks(
        &sAudioRpcRuntimeNodePool, sizeof(AudioRpcRuntimeNode));
    ShutdownAudioResourceRuntime(&m_Runtime);

    m_Backend->Shutdown();
}

void AudioRpcBundleManager::ParseChunk(nlChunk* chunk)
{
    switch (chunk->GetID())
    {
    case 0x80023600:
        m_RpcController = ParseAudioRpcController(chunk);
        break;
    default:
        AudioBundleManager::ParseChunk(chunk);
        break;
    }
}

void AudioRpcBundleManager::Update(float dt)
{
    if (IsLoaded())
    {
        ((AudioSliderTable*)m_SliderTable)->Update(dt);
        UpdateAudioRpcController(m_RpcController, dt);
        UpdateAudioResourceRuntime(&m_Runtime, dt);
        ((AudioCalculationTable*)m_CalculationTable)->Update(dt);
    }
    m_Backend->ServiceReadQueue(dt);
}
