#ifndef GAME_AUDIO_AUDIO_RPC_H
#define GAME_AUDIO_AUDIO_RPC_H

#include "NL/nlDLListContainer.h"

class nlChunk;
class XSoundHandle;

struct AudioRpcCurvePoint
{
    float input;
    float output;
};

struct AudioRpcRuntimeNode;

struct AudioRpcDefinition
{
    u32 field_00;
    u32 field_04;
    u32 sliderIndex;
    s32 kind;
    u32 enabled;
    u32 field_14;
    u32 pointCount;
    AudioRpcCurvePoint* points;
    AudioRpcRuntimeNode* runtimeNode;
};

struct AudioRpcGroup
{
    u32 field_00;
    u32 definitionCount;
    AudioRpcDefinition* definitions;
    u32 staticDefinitionCount;
    AudioRpcDefinition* staticDefinitions;
    u32 dynamicDefinitionCount;
    AudioRpcDefinition* dynamicDefinitions;
};

struct AudioRpcOwner
{
    XSoundHandle* soundHandle;
};

struct AudioRpcRuntimeNode
{
    AudioRpcDefinition* definition;
    float value;
    u8 valid;
    u8 pad_09[3];
    AudioRpcOwner* owner;
};

typedef DLListEntry<AudioRpcRuntimeNode*> AudioRpcListEntry;
typedef BorrowedDLListSlotPool<AudioRpcRuntimeNode*> AudioRpcList;

struct AudioRpcController
{
    u32 groupCount;
    AudioRpcGroup* groups;
    u32 runtimeCount;
    AudioRpcRuntimeNode** runtimeNodes;
    AudioRpcList* dynamicNodes;
};

extern SlotPool<AudioRpcRuntimeNode> sAudioRpcRuntimeNodePool;
extern SlotPool<AudioRpcListEntry> sAudioRpcListEntryPool;

extern "C" AudioRpcController* ParseAudioRpcController(nlChunk* chunk);
extern "C" void InitializeAudioRpcController(AudioRpcController* controller);
extern "C" void UpdateAudioRpcController(
    AudioRpcController* controller, float dt);
extern "C" AudioRpcRuntimeNode* AddAudioRpcRuntimeNode(
    AudioRpcController* controller,
    AudioRpcDefinition* definition,
    AudioRpcOwner* owner);
extern "C" void RemoveAudioRpcRuntimeNodes(
    AudioRpcController* controller, AudioRpcOwner* owner);

#endif // GAME_AUDIO_AUDIO_RPC_H
