#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioRpc.h"
#include "Game/Audio/AudioResourceBundle.h"
#include "Game/Audio/AudioSequenceEvent.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/RegistryPools.h"
#include "NL/nlChunk.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "types.h"

struct AudioBundleManagerRpcView
{
    u8 pad_00[0x50];
    AudioRpcController* rpcController;
};

AudioResourceBundle* ParseAudioResourceBundle(nlChunk* outer)
{
    nlChunk* header = (nlChunk*)outer->GetData();
    AudioResourceBundle* bundle = (AudioResourceBundle*)header->GetData();

    s32 voiceOffset = (s32)bundle->voices;
    s32 sequenceOffset = (s32)bundle->sequences;
    s32 soundEventOffset = (s32)bundle->soundEvents;
    s32 parameterEventOffset = (s32)bundle->parameterEvents;
    s32 hitMarkerEventOffset = (s32)bundle->hitMarkerEvents;

    u32 groupIndex;
    AudioCueDefinition* cue;
    nlChunk* cursor = header->GetNextChunk();
    bundle->cues = (AudioCueDefinition*)cursor->GetData();
    cursor = cursor->GetNextChunk();
    bundle->voices = (AudioVoiceDefinition*)cursor->GetData();
    voiceOffset = (s32)bundle->voices - voiceOffset;
    cursor = cursor->GetNextChunk();
    bundle->sequences = (AudioSequenceDefinition*)cursor->GetData();
    sequenceOffset = (s32)bundle->sequences - sequenceOffset;
    cursor = cursor->GetNextChunk();
    bundle->soundEvents = (SoundEventDefinition*)cursor->GetData();
    soundEventOffset = (s32)bundle->soundEvents - soundEventOffset;
    cursor = cursor->GetNextChunk();
    bundle->hitMarkerEvents = (HitMarkerEventDefinition*)cursor->GetData();
    hitMarkerEventOffset = (s32)bundle->hitMarkerEvents - hitMarkerEventOffset;
    cursor = cursor->GetNextChunk();
    bundle->parameterEvents = (ParameterChangeEventDefinition*)cursor->GetData();
    parameterEventOffset = (s32)bundle->parameterEvents - parameterEventOffset;

    cue = bundle->cues;
    for (groupIndex = 0; groupIndex < bundle->cueCount;
        groupIndex++, cue++)
    {
        cursor = cursor->GetNextChunk();
        u32 entryIndex;
        AudioCueEntry* entry = (AudioCueEntry*)cursor->GetData();
        cue->voices = entry;
        for (entryIndex = 0; entryIndex < cue->voiceCount;
            entryIndex++, entry++)
        {
            entry->voice = (AudioVoiceDefinition*)((u8*)entry->voice
                                                  + voiceOffset);
        }
    }

    AudioVoiceDefinition* voice = bundle->voices;
    for (groupIndex = 0; groupIndex < bundle->voiceCount;
        groupIndex++, voice++)
    {
        cursor = cursor->GetNextChunk();
        u32 i;
        AudioSequenceDefinition** sequence =
            (AudioSequenceDefinition**)cursor->GetData();
        voice->sequences = sequence;
        for (i = 0; i < voice->sequenceCount; i++, sequence++)
        {
            *sequence = (AudioSequenceDefinition*)((u8*)*sequence
                                                  + sequenceOffset);
        }
        cursor = cursor->GetNextChunk();
        voice->rpcGroupIndices = (u32*)cursor->GetData();
    }

    AudioSequenceDefinition* sequence = bundle->sequences;
    for (groupIndex = 0; groupIndex < bundle->sequenceCount;
        groupIndex++, sequence++)
    {
        cursor = cursor->GetNextChunk();
        u32 eventIndex;
        AudioSequenceEventDefinition* event =
            (AudioSequenceEventDefinition*)cursor->GetData();
        sequence->eventDefinitions = event;
        for (eventIndex = 0;
            eventIndex < sequence->eventCount;
            eventIndex++, event++)
        {
            if (event->type == 1)
                event->sound = (SoundEventDefinition*)((u8*)event->sound
                                                       + soundEventOffset);
            else if (event->type == 3)
                event->hitMarker = (HitMarkerEventDefinition*)((u8*)event->hitMarker
                                                               + hitMarkerEventOffset);
            else if (event->type == 2)
                event->parameter = (ParameterChangeEventDefinition*)((u8*)event->parameter
                                                                     + parameterEventOffset);
        }
    }

    SoundEventDefinition* soundEvent = bundle->soundEvents;
    for (groupIndex = 0; groupIndex < bundle->soundEventCount;
        groupIndex++, soundEvent++)
    {
        cursor = cursor->GetNextChunk();
        soundEvent->choices = (SoundChoice*)cursor->GetData();
    }

    for (groupIndex = 0; groupIndex < bundle->voiceCount;
        groupIndex++)
        InitializeAudioVoiceRpcModifiers(bundle->voices + groupIndex);
    return bundle;
}

AudioVoiceDefinition* SelectAudioCueVoice(AudioCueDefinition* cue)
{
    AudioCueEntry* selected = 0;
    if (cue->selectionMode == -1)
        return 0;
    if (cue->voiceCount == 0)
        return 0;

    if (cue->voiceCount == 1)
    {
        selected = cue->voices;
    }
    else if (cue->selectionMode == 0
             || (cue->selectionMode == 1 && cue->selectedVoiceIndex != 0xFFFF))
    {
        if (cue->selectedVoiceIndex == 0xFFFF)
            selected = cue->voices;
        else
        {
            u32 next = cue->selectedVoiceIndex + 1;
            if (next >= cue->voiceCount)
                next = 0;
            selected = cue->voices + next;
        }
    }
    else
    {
        float totalWeight = 0.0f;
        if (cue->selectionMode == 3)
        {
            u32 i;
            AudioCueEntry* entry = cue->voices;
            for (i = 0; i < cue->voiceCount; i++, entry++)
            {
                if (i != cue->selectedVoiceIndex)
                {
                    entry->eligible = true;
                    totalWeight += entry->weight;
                }
                else
                {
                    entry->eligible = false;
                }
            }
        }
        else if (cue->selectionMode == 1 || cue->selectionMode == 2)
        {
            AudioCueEntry* entry = cue->voices;
            for (u32 i = 0; i < cue->voiceCount; i++, entry++)
            {
                entry->eligible = true;
                totalWeight += entry->weight;
            }
        }
        else if (cue->selectionMode == 4)
        {
            if (cue->selectedVoiceIndex != 0xFFFF)
            {
                AudioCueEntry* entry = cue->voices;
                u32 i;
                AudioCueEntry* previous =
                    cue->voices + cue->selectedVoiceIndex;
                for (i = 0; i < cue->voiceCount; i++, entry++)
                {
                    if (entry->selectionCount < previous->selectionCount)
                    {
                        entry->eligible = true;
                        totalWeight += entry->weight;
                    }
                    else
                    {
                        entry->eligible = false;
                    }
                }
            }
            if (totalWeight == 0.0f)
            {
                AudioCueEntry* entry = cue->voices;
                for (u32 i = 0; i < cue->voiceCount; i++, entry++)
                {
                    entry->eligible = true;
                    totalWeight += entry->weight;
                }
            }
        }

        float choice = (float)nlRandom((u32)(s32)totalWeight, &nlDefaultSeed);
        AudioCueEntry* entry = cue->voices;
        for (u32 i = 0; i < cue->voiceCount; i++, entry++)
        {
            if (!entry->eligible)
                continue;
            if (choice <= entry->weight)
            {
                selected = entry;
                break;
            }
            choice -= entry->weight;
        }
    }

    if (selected == 0)
        cue->selectedVoiceIndex = 0xFFFF;
    cue->selectedVoiceIndex = selected - cue->voices;
    selected->selectionCount++;
    return selected != 0 ? selected->voice : 0;
}

AudioVoiceDefinition* SelectAudioCueVoiceBySlider(
    AudioCueDefinition* cue, float value)
{
    AudioCueEntry* selected = 0;
    if (cue->voiceCount == 0)
        return 0;
    if (cue->voiceCount == 1)
        selected = cue->voices;
    else
    {
        u32 i;
        u32 randomValue;
        AudioCueEntry* entry = cue->voices;
        randomValue = 0;
        i = 0;
        for (; i < cue->voiceCount; i++, entry++)
        {
            if (value >= entry->minimumValue && value <= entry->weight)
            {
                u32 candidate = nlRandom(100, &nlDefaultSeed);
                if (selected == 0 || candidate >= randomValue)
                {
                    randomValue = candidate;
                    selected = entry;
                }
            }
        }
    }
    return selected != 0 ? selected->voice : 0;
}

void InitializeAudioVoiceRpcModifiers(AudioVoiceDefinition* voice)
{
    voice->modifierCount = 0;
    voice->dynamicRpcCount = 0;

    for (u32 i = 0; i < voice->rpcGroupCount; i++)
    {
        AudioRpcController* controller = ((AudioBundleManagerRpcView*)g_pAudioSystem->GetBundleManager())->rpcController;
        AudioRpcGroup* rpcGroup = controller->groups + voice->rpcGroupIndices[i];
        voice->modifierCount += rpcGroup->staticDefinitionCount;
        voice->dynamicRpcCount += rpcGroup->dynamicDefinitionCount;
    }

    if (voice->modifierCount == 0)
        return;

    voice->modifiers = (AudioRpcRuntimeNode**)nlMalloc(
        voice->modifierCount * sizeof(void*), 8, false);
    u32 outputIndex = 0;
    for (u32 i = 0; i < voice->rpcGroupCount; i++)
    {
        AudioRpcController* controller = ((AudioBundleManagerRpcView*)g_pAudioSystem->GetBundleManager())->rpcController;
        AudioRpcGroup* rpcGroup = controller->groups + voice->rpcGroupIndices[i];
        for (u32 definitionIndex = 0;
            definitionIndex < rpcGroup->staticDefinitionCount;
            definitionIndex++)
        {
            AudioRpcDefinition* definition = rpcGroup->staticDefinitions + definitionIndex;
            if (definition->enabled)
            {
                voice->modifiers[outputIndex] = definition->runtimeNode;
                outputIndex++;
            }
        }
    }
}
