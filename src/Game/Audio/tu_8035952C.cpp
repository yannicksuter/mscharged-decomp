#include "Game/Audio/AudioEffect_802F98F4.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"
#include "types.h"

class CategoryVolumeParameter_803595A4
    : public AudioEffectParameter_802F69A8
{
public:
    CategoryVolumeParameter_803595A4();
    virtual ~CategoryVolumeParameter_803595A4();

    static void* operator new(unsigned long)
    {
        CategoryVolumeParameter_803595A4* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((CategoryVolumeParameter_803595A4*)pointer);
    }

    float m_Volume;
    u32 m_Category;

    static SlotPool<CategoryVolumeParameter_803595A4> s_Pool;
};

class CategoryVolume : public AudioEffectBase_802F98F4
{
public:
    virtual ~CategoryVolume();
    virtual void fn_802F6930(u32 definition, void* context, bool negate,
        AudioEffectParameter_802F69A8** output);
    virtual void fn_802F9B5C();
    virtual void fn_802F9B60(AudioEffectParameter_802F69A8* destination,
        AudioEffectParameter_802F69A8* source);

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

    CategoryVolumeParameter_803595A4 m_Initial;
    CategoryVolumeParameter_803595A4 m_Final;

    static SlotPool<CategoryVolume> s_Pool;
};

struct CategoryEntry_80359888
{
    CategoryEntry_80359888(u32 value, u16 index)
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

class ConfigNode_803595C8
{
public:
    virtual ~ConfigNode_803595C8();
    virtual unsigned long long fn_803595C8(const u32& key);
};

struct ConfigSystem_803595C8
{
    u8 m_Pad00[0x20];
    ConfigNode_803595C8* m_Root;
};

struct CategoryRuntimeState_803595C8
{
    u8 m_Pad00[8];
    float m_Volume;
};

struct CategoryBackendEntry_803595C8
{
    u8 m_Pad00[0xC];
    float m_Current;
    float m_Transition;
    u8 m_Pad14[4];
    float m_Minimum;
    float m_Maximum;
    u8 m_Pad20[4];
    CategoryRuntimeState_803595C8* m_State;
};

struct CategoryBackendTable_803595C8
{
    u8 m_Pad00[8];
    CategoryBackendEntry_803595C8* m_Entries;
};

struct AudioResources_803595C8
{
    u8 m_Pad00[0x10];
    CategoryBackendTable_803595C8* m_Categories;
};

struct AudioSystem_803595C8
{
    u8 m_Pad00[0xCC];
    AudioResources_803595C8* m_Resources;
};

union ConfigValue_803595C8
{
    unsigned long long m_Raw;
    float m_Float;
    struct
    {
        u32 m_Value;
        u32 m_Type;
    } m_Words;
};

static CategoryEntry_80359888 s_CategoryEntries[5] = {
    CategoryEntry_80359888(0x00016A70, 4),
    CategoryEntry_80359888(0x05A165C0, 2),
    CategoryEntry_80359888(0x1883E244, 1),
    CategoryEntry_80359888(0x52030129, 3),
    CategoryEntry_80359888(0xAB29FE50, 0),
};

SlotPool<CategoryVolumeParameter_803595A4>
    CategoryVolumeParameter_803595A4::s_Pool(16, 16);
SlotPool<CategoryVolume> CategoryVolume::s_Pool(16, 16);

extern ConfigSystem_803595C8* lbl_806E203C;
extern AudioSystem_803595C8* lbl_806E202C;
extern bool lbl_806E21C0;
extern s32 lbl_806E21C4;

AuxEffectMap_8035952C::AuxEffectMap_8035952C()
{
    m_Effects[0] = -1;
    m_Effects[1] = -1;
    m_Indices[0] = -1;
    m_Indices[1] = -1;
}

int AuxEffectMap_8035952C::fn_80359544(const int& effect)
{
    int& effectIndex = m_Indices[effect];
    for (u16 index = 0; index < 2; ++index)
    {
        if (m_Effects[index] == -1)
        {
            m_Effects[index] = effect;
            effectIndex = index;
            break;
        }
    }
    return effectIndex;
}

int AuxEffectMap_8035952C::fn_80359590(const int& effect) const
{
    return m_Indices[effect];
}

CategoryVolumeParameter_803595A4::CategoryVolumeParameter_803595A4()
    : m_Volume(0.0f)
    , m_Category(0)
{
}

void CategoryVolume::fn_802F6930(u32 definition, void*, bool negate,
    AudioEffectParameter_802F69A8** output)
{
    CategoryVolumeParameter_803595A4* parameter
        = new CategoryVolumeParameter_803595A4;
    *output = parameter;

    u32 definitionKey = definition;
    u32 key = 0xD2894EC5;
    ConfigValue_803595C8 value;
    value.m_Raw = lbl_806E203C->m_Root->fn_803595C8(key);
    ConfigNode_803595C8* node
        = (ConfigNode_803595C8*)value.m_Words.m_Value;
    value.m_Raw = node->fn_803595C8(definitionKey);
    node = (ConfigNode_803595C8*)value.m_Words.m_Value;

    key = nlStringLowerHash("category");
    value.m_Raw = node->fn_803595C8(key);
    parameter->m_Category = value.m_Words.m_Value;

    if (lbl_806E21C0)
    {
        parameter->m_Volume = (float)lbl_806E21C4;
    }
    else
    {
        key = nlStringLowerHash("volume");
        value.m_Raw = node->fn_803595C8(key);
        parameter->m_Volume = negate ? -value.m_Float : value.m_Float;
    }

    float volume = parameter->m_Volume;
    volume = volume >= -96.0f ? volume : -96.0f;
    volume = volume <= 6.0f ? volume : 6.0f;
    parameter->m_Volume = volume;

    CategoryEntry_80359888 category(parameter->m_Category, 0);
    CategoryEntry_80359888* entry
        = nlBSearch<CategoryEntry_80359888, CategoryEntry_80359888>(
            category, s_CategoryEntries, 5);
    if (entry != 0)
    {
        CategoryBackendEntry_803595C8* backend
            = &lbl_806E202C->m_Resources->m_Categories
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

void CategoryVolume::fn_802F9B60(
    AudioEffectParameter_802F69A8* destination,
    AudioEffectParameter_802F69A8* source)
{
    CategoryVolumeParameter_803595A4* destinationParameter
        = (CategoryVolumeParameter_803595A4*)destination;
    CategoryVolumeParameter_803595A4* sourceParameter
        = (CategoryVolumeParameter_803595A4*)source;
    destinationParameter->m_Volume = sourceParameter->m_Volume;
    destinationParameter->m_Category = sourceParameter->m_Category;
}

CategoryVolumeParameter_803595A4::~CategoryVolumeParameter_803595A4()
{
}

void CategoryVolume::fn_802F9B5C()
{
    m_Final.m_State = m_Initial.m_State;
    m_Final.m_Volume = m_Initial.m_Volume;
    m_Final.m_Category = m_Initial.m_Category;
}

CategoryVolume::~CategoryVolume()
{
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
