#include "NL/nlDLListContainer.inl"
#include "Game/Effects/EffectsGroup.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Sys/simpleparser.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlChunk.h"
#include "NL/nlDebugString.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

class UserEffectFactory
{
public:
    virtual ~UserEffectFactory();
    virtual UserEffectSpec* ParseSpec(SimpleParser* parser);
    virtual const char* GetName();
};

static UserEffectFactory* gUserEffectTypes[3];
static int gnUserEffectTypes;

bool EffectsGroup::IsPersistent() const
{
    unsigned long count = m_numSpecs;
    for (unsigned long i = 0; i < count; ++i)
    {
        if (m_specs[i].m_pTemplate->m_fFountainLife >= 1.0e10f)
        {
            return true;
        }
    }

    for (unsigned long i = 0; i < count; ++i)
    {
        if (m_specs[i].m_pTemplate->m_rParticleLife.base >= 1000.0f)
        {
            return true;
        }
    }

    return false;
}

EffectsGroup* EffectsGroup::LoadFromChunk(nlChunk* chunk)
{
    nlChunk* groupChunk = chunk->GetFirstChunk();
    EffectsGroup* pGroup = static_cast<EffectsGroup*>(groupChunk->GetData());

    groupChunk = groupChunk->GetNextChunk();
    pGroup->m_specs = static_cast<EffectsSpec*>(groupChunk->GetData());
    groupChunk = groupChunk->GetNextChunk();
    pGroup->mUserSpecSources = static_cast<UserEffectSource*>(groupChunk->GetData());

    for (unsigned long i = 0; i < pGroup->m_userSpecs; ++i)
    {
        groupChunk = groupChunk->GetNextChunk();
        pGroup->mUserSpecSources[i].mData = static_cast<char*>(groupChunk->GetData());
    }

    pGroup->ParseUserSpecs();
    return pGroup;
}

void EffectsGroup::ParseUserSpecs()
{
    if (m_userSpecs == 0)
    {
        m_userSpecsPtr = 0;
        return;
    }

    m_userSpecsPtr = new (8, false) UserEffectSpec*[m_userSpecs];
    for (unsigned long specIndex = 0; specIndex < m_userSpecs; ++specIndex)
    {
        SimpleParser parser;
        parser.StartParsing(mUserSpecSources[specIndex].mData,
            mUserSpecSources[specIndex].mSize, " \t\r\n");
        char* token = parser.NextToken(true);

        int i;
        for (i = 0; i < gnUserEffectTypes; ++i)
        {
            if (nlStrCmp<char>(gUserEffectTypes[i]->GetName(), token) == 0)
            {
                m_userSpecsPtr[specIndex] = gUserEffectTypes[i]->ParseSpec(&parser);
                break;
            }
        }

        if (i == gnUserEffectTypes)
        {
            EmissionManager::Instance()->AddError("Unknown usereffect used: '%s' (in effect '%s')\n",
                token, nlLookupDebugString(g_pDebugStringTable, m_hashID));
            m_userSpecsPtr[specIndex] = 0;
        }
    }
}

void EffectsGroup::DestroyUserSpecs()
{
    if (m_userSpecs != 0)
    {
        for (unsigned long i = 0; i < m_userSpecs; ++i)
        {
            if (m_userSpecsPtr[i] != 0)
            {
                delete m_userSpecsPtr[i];
            }
        }
        delete[] m_userSpecsPtr;
    }
}

void EffectsGroup::ResolveTemplates(EffectsTemplate** table)
{
    for (unsigned long i = 0; i < m_numSpecs; ++i)
    {
        m_specs[i].m_pTemplate = table[m_specs[i].m_uTemplateIndex];
    }
}
