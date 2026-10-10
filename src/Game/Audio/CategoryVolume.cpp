#include "NL/nlDLListContainer.inl"
#include "Game/Audio/AudioConfig.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioCalculation.h"
#include "Game/Audio/CategoryVolume.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/SharedStaticStorage.h"

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

static CategoryEntry s_CategoryEntries[5] = {
    CategoryEntry(0x00016A70, 4),
    CategoryEntry(0x05A165C0, 2),
    CategoryEntry(0x1883E244, 1),
    CategoryEntry(0x52030129, 3),
    CategoryEntry(0xAB29FE50, 0),
};

static inline CategoryEntry* FindCategoryEntry(u32 category);

SlotPool<CategoryVolumeParameter>
    CategoryVolumeParameter::s_Pool(16, 16);
SlotPool<CategoryVolume> CategoryVolume::s_Pool(16, 16);

bool gCategoryVolumeOverrideEnabled;
s32 gCategoryVolumeOverride;

CategoryVolumeParameter::CategoryVolumeParameter()
    : m_Volume(0.0f)
    , m_Category(0)
{
}

void CategoryVolume::CreateParameter(unsigned int definition, const void*, bool negate,
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

    CategoryEntry* entry = FindCategoryEntry(parameter->m_Category);
    if (entry != 0)
    {
        AudioCalculationSlider* backend
            = &((AudioCalculationTable*)g_pAudioSystem->GetBundleManager()->GetCalculationTable())
                   ->sliders[entry->m_Index];
        backend->definition->initialValue = parameter->m_Volume;
        if (parameter->m_Volume < backend->minimum)
            backend->target = backend->minimum;
        else if (parameter->m_Volume > backend->maximum)
            backend->target = backend->maximum;
        else
            backend->target = parameter->m_Volume;
        backend->remainingTime = 0.0f;
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

static inline CategoryEntry* FindCategoryEntry(u32 category)
{
    CategoryEntry key(category, 0);
    return nlBSearch<CategoryEntry, CategoryEntry>(key, s_CategoryEntries, 5);
}

void CategoryVolume::BeginBlend()
{
    m_Final.m_State = m_Initial.m_State;
    m_Final.m_Volume = m_Initial.m_Volume;
    m_Final.m_Category = m_Initial.m_Category;
}
