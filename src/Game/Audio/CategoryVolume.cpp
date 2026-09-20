#include "Game/Audio/AudioConfig.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioEffect.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/UnidentifiedStaticStorage.h"

class CategoryVolumeParameter
    : public AudioEffectParameter
{
public:
    CategoryVolumeParameter();
    virtual ~CategoryVolumeParameter();

    static void* operator new(unsigned long)
    {
        CategoryVolumeParameter* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((CategoryVolumeParameter*)pointer);
    }

    float m_Volume;
    u32 m_Category;

    static SlotPool<CategoryVolumeParameter> s_Pool;
};

class CategoryVolume : public AudioEffectBase
{
public:
    virtual ~CategoryVolume();
    virtual void CreateParameter(u32 definition, void* context, bool negate,
        AudioEffectParameter** output);
    virtual void BeginBlend();
    virtual void BlendParameter(AudioEffectParameter* destination,
        AudioEffectParameter* source);

    static void* operator new(unsigned long)
    {
        CategoryVolume* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((CategoryVolume*)pointer);
    }

    CategoryVolumeParameter m_Initial;
    CategoryVolumeParameter m_Final;

    static SlotPool<CategoryVolume> s_Pool;
};

struct CategoryEntry
{
    CategoryEntry(u32 value, u16 index)
        : m_Hash(value)
        , m_Index(index)
    {
    }

    operator unsigned long() const
    {
        return m_Hash;
    }

    u32 m_Hash;
    u16 m_Index;
};

struct CategoryRuntimeState
{
    u8 m_Pad00[8];
    float m_Volume;
};

struct CategoryBackendEntry
{
    u8 m_Pad00[0xC];
    float m_Current;
    float m_Transition;
    u8 m_Pad14[4];
    float m_Minimum;
    float m_Maximum;
    u8 m_Pad20[4];
    CategoryRuntimeState* m_State;
};

struct CategoryBackendTable
{
    u8 m_Pad00[8];
    CategoryBackendEntry* m_Entries;
};

static CategoryEntry s_CategoryEntries[5] = {
    CategoryEntry(0x00016A70, 4),
    CategoryEntry(0x05A165C0, 2),
    CategoryEntry(0x1883E244, 1),
    CategoryEntry(0x52030129, 3),
    CategoryEntry(0xAB29FE50, 0),
};

SlotPool<CategoryVolumeParameter>
    CategoryVolumeParameter::s_Pool(16, 16);
SlotPool<CategoryVolume> CategoryVolume::s_Pool(16, 16);

extern bool gCategoryVolumeOverrideEnabled;
extern s32 gCategoryVolumeOverride;

CategoryVolumeParameter::CategoryVolumeParameter()
    : m_Volume(0.0f)
    , m_Category(0)
{
}

void CategoryVolume::CreateParameter(u32 definition, void*, bool negate,
    AudioEffectParameter** output)
{
    CategoryVolumeParameter* parameter
        = new CategoryVolumeParameter;
    *output = parameter;

    AudioConfigNode* node = ConfigFindDefinition(definition);

    parameter->m_Category = node->Get(nlStringLowerHash("category")).m_Words.m_Value;

    if (gCategoryVolumeOverrideEnabled)
    {
        parameter->m_Volume = (float)gCategoryVolumeOverride;
    }
    else
    {
        parameter->m_Volume = node->Get(nlStringLowerHash("volume")).m_Float;
        parameter->m_Volume = negate ? -parameter->m_Volume : parameter->m_Volume;
    }

    float volume = parameter->m_Volume;
    volume = volume >= -96.0f ? volume : -96.0f;
    volume = volume <= 6.0f ? volume : 6.0f;
    parameter->m_Volume = volume;

    CategoryEntry* entry
        = nlBSearch<CategoryEntry, CategoryEntry>(
            CategoryEntry(parameter->m_Category, 0), s_CategoryEntries, 5);
    if (entry != 0)
    {
        CategoryBackendEntry* backend
            = &((CategoryBackendTable*)g_pAudioSystem->GetBundleManager()->GetCalculationTable())
                   ->m_Entries[entry->m_Index];
        backend->m_State->m_Volume = parameter->m_Volume;
        if (parameter->m_Volume < backend->m_Minimum)
            backend->m_Current = backend->m_Minimum;
        else if (parameter->m_Volume > backend->m_Maximum)
            backend->m_Current = backend->m_Maximum;
        else
            backend->m_Current = parameter->m_Volume;
        backend->m_Transition = 0.0f;
    }
}

void CategoryVolume::BlendParameter(
    AudioEffectParameter* destination,
    AudioEffectParameter* source)
{
    CategoryVolumeParameter* destinationParameter
        = (CategoryVolumeParameter*)destination;
    CategoryVolumeParameter* sourceParameter
        = (CategoryVolumeParameter*)source;
    destinationParameter->m_Volume = sourceParameter->m_Volume;
    destinationParameter->m_Category = sourceParameter->m_Category;
}

CategoryVolumeParameter::~CategoryVolumeParameter()
{
}

void CategoryVolume::BeginBlend()
{
    m_Final.m_State = m_Initial.m_State;
    m_Final.m_Volume = m_Initial.m_Volume;
    m_Final.m_Category = m_Initial.m_Category;
}

CategoryVolume::~CategoryVolume()
{
}

