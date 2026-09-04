#include "Game/Transitions/ScreenTransitionManager.h"

#include "Game/Transitions/TransitionSequence.h"
#include "Game/Transitions/ColourBlendScreenTransition.h"
#include "Game/Transitions/ScriptedTransition.h"
#include "Game/Transitions/ModelTransition.h"

#include "Game/Sys/simpleparser.h"

#include "string.h"

#include "NL/gl/gl.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"

template <>
ScreenTransitionManager* nlSingleton<ScreenTransitionManager>::s_pInstance = 0;

ScreenTransitionManager::ScreenTransitionManager()
    : m_pActiveTransition(0)
    , m_TransitionMap()
    , m_eView(0)
    , m_pCallback(0)
    , m_SelectedTransition(0)
    , m_fCurrentTime(0.0f)
{
    m_Transitions.mData = 0;
    m_Transitions.mSize = 0;
    m_Transitions.mCapacity = 0;
    m_Cut = false;
    m_Transitions.reserve(16);
}

ScreenTransitionManager::~ScreenTransitionManager()
{
    if (m_pActiveTransition != 0)
    {
        m_pActiveTransition->Cancel();

        if (m_pCallback != 0)
        {
            m_pCallback->TransitionFinished();
        }
    }

    m_pActiveTransition = 0;
    m_TransitionMap.DeleteValues();
}

void ScreenTransitionManager::Update(float dt)
{
    if (m_pActiveTransition != 0)
    {
        m_pActiveTransition->Update(dt);

        if (m_pActiveTransition->IsFinished())
        {
            m_pActiveTransition->Cancel();
            m_pActiveTransition = 0;
            m_fCurrentTime = 0.0f;

            if (m_pCallback != 0)
            {
                m_pCallback->TransitionFinished();
            }
        }
        else
        {
            m_fCurrentTime += dt;

            if (m_pCallback != 0)
            {
                float curProgress = m_fCurrentLength;
                float progress = 0.0f;
                if (curProgress > 0.0f)
                {
                    progress = m_fCurrentTime / curProgress;
                }

                m_pCallback->TransitionProgressed(progress);
            }
        }
    }
}

void ScreenTransitionManager::Render()
{
    if (m_eView != 0 && m_pActiveTransition != 0 && !m_pActiveTransition->IsFinished())
    {
        if (m_fCurrentTime == 0.0f)
        {
            m_pActiveTransition->Update(0.0f);
        }

        m_pActiveTransition->Render(m_eView);

        if (m_pCallback != 0 && m_pActiveTransition->UnidentifiedVirtual30())
        {
            m_pCallback->fn_80188764();
        }
    }
}

void ScreenTransitionManager::CancelAllTransitions()
{
    if (m_pActiveTransition != 0)
    {
        m_pActiveTransition->Cancel();

        if (m_pCallback != 0)
        {
            m_pCallback->TransitionFinished();
        }
    }
    m_pActiveTransition = 0;
}

void ScreenTransitionManager::AddTransitionToMap(char* name, ScreenTransition* pTransition)
{
    unsigned long transitionHash = glHash(name);
    m_TransitionMap.Add(transitionHash, pTransition);

    BasicString<char, Detail::TempStringAllocator> nameString(name);
    m_Transitions.push_back(nameString);
}

void ScreenTransitionManager::EnableRandomTransition(const char* filter)
{
    SelectRandomTransition(filter);

    if (m_pActiveTransition != 0)
    {
        m_pActiveTransition->Cancel();
    }

    if (m_SelectedTransition != 0)
    {
        m_SelectedTransition->Reset();
        m_pActiveTransition = m_SelectedTransition;
        m_SelectedTransition = 0;
        m_fCurrentTime = 0.0f;
        m_Cut = false;
        m_fCurrentLength = m_pActiveTransition->GetTransitionLength();
    }
}

void ScreenTransitionManager::SelectRandomTransition(const char* filter)
{
    Vector<BasicString<char, Detail::TempStringAllocator>, DefaultAllocator> candidates;
    candidates.mData = 0;
    candidates.mSize = 0;
    candidates.mCapacity = 0;
    candidates.reserve(8);

    for (int i = 0; i < m_Transitions.mSize; ++i)
    {
        const char* transitionName = m_Transitions.mData[i].c_str();

        if (strstr(transitionName, filter) != 0)
        {
            candidates.push_back(m_Transitions.mData[i]);
        }
    }

    m_SelectedTransition = 0;

    if (candidates.mSize > 0)
    {
        int randomIndex = nlRandom(candidates.mSize, &nlDefaultSeed);
        const char* selectedName = candidates.mData[randomIndex].c_str();

        unsigned long transitionHash = glHash(selectedName);
        ScreenTransition** foundTransition = 0;
        bool found = m_TransitionMap.FindGet(transitionHash, &foundTransition);

        if (found)
        {
            m_SelectedTransition = *foundTransition;
        }
    }
}

float ScreenTransitionManager::GetSelectedTransitionCutTime() const
{
    if (m_SelectedTransition != 0)
    {
        return m_SelectedTransition->CutTime();
    }
    return 0.0f;
}

void ScreenTransitionManager::EnableSelectedTransition()
{
    if (m_pActiveTransition != 0)
    {
        m_pActiveTransition->Cancel();
    }

    if (m_SelectedTransition != 0)
    {
        m_SelectedTransition->Reset();
        m_pActiveTransition = m_SelectedTransition;
        m_SelectedTransition = 0;
        m_fCurrentTime = 0.0f;
        m_Cut = false;
        m_fCurrentLength = m_pActiveTransition->GetTransitionLength();
    }
}

void ScreenTransitionManager::AddTransitions(char* loadedData, unsigned long fileSize)
{
    SimpleParser parser;
    char szNameBuffer[64];

    parser.StartParsing(loadedData, fileSize, " \t\r\n");
    char* pToken = parser.NextToken(true);

    while (pToken != 0)
    {
        if (nlStrCmp<char>(pToken, "colourblend") == 0)
        {
            pToken = parser.NextTokenOnLine(true);
            nlStrNCpy<char>(szNameBuffer, pToken, 0x40);
            AddTransitionToMap(szNameBuffer, ColourBlendScreenTransition::GetFromParser(&parser));
        }
        else if (nlStrCmp<char>(pToken, "sequence") == 0)
        {
            nlStrNCpy<char>(szNameBuffer, parser.NextTokenOnLine(true), 0x40);
            TransitionSequence* transitionSequence = new (8, false) TransitionSequence();
            transitionSequence->Initialize(&parser);
            AddTransitionToMap(szNameBuffer, transitionSequence);
        }
        else if (nlStrCmp<char>(pToken, "transition") == 0)
        {
            nlStrNCpy<char>(szNameBuffer, parser.NextTokenOnLine(true), 0x40);
            ScriptedScreenTransition* scriptedTransition = new (8, false) ScriptedScreenTransition();
            scriptedTransition->InitializeFromParser(&parser);
            AddTransitionToMap(szNameBuffer, scriptedTransition);
        }
        else if (nlStrCmp<char>(pToken, "model") == 0)
        {
            nlStrNCpy<char>(szNameBuffer, parser.NextTokenOnLine(true), 0x40);
            ModeledScreenTransition* modeledTransition = new (8, false) ModeledScreenTransition();
            modeledTransition->LoadFromParser(&parser);
            AddTransitionToMap(szNameBuffer, modeledTransition);
        }
        else if (*pToken == 0x23)
        {
            while (pToken != 0)
            {
                pToken = parser.NextTokenOnLine(true);
            }
        }
        pToken = parser.NextToken(true);
    }
}
