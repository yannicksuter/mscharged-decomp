#include "Game/Transitions/TransitionSequence.h"

#include "stdlib.h"
#include "string.h"
#include "strtold.h"

#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include "Game/Transitions/ColourBlendScreenTransition.h"
#include "Game/Transitions/ModelTransition.h"
#include "Game/Transitions/ScriptedTransition.h"

TransitionSequence::TransitionSequence()
{
    m_pTransitions = NULL;
    m_nNumTransitions = 0;
    m_pPlaying = NULL;
    m_pSound = NULL;
    m_CutAt = -1.0f;
}

TransitionSequence::~TransitionSequence()
{
    if (m_pTransitions != NULL)
    {
        for (int i = 0; i < m_nNumTransitions; i++)
        {
            delete m_pTransitions[i];
            m_pTransitions[i] = NULL;

            delete[] m_pSound[i].soundStr;
            m_pSound[i].soundStr = NULL;
        }

        delete[] m_pTransitions;
        m_pTransitions = NULL;

        delete[] m_pPlaying;
        m_pPlaying = NULL;

        delete[] m_pEarly;
        m_pEarly = NULL;

        delete[] m_pSound;
        m_pSound = NULL;
    }
}

void TransitionSequence::DoSanityCheck()
{
    for (int i = 0; i < m_nNumTransitions; i++)
    {
        if (m_pTransitions[i] != NULL)
        {
            m_pTransitions[i]->DoSanityCheck();
        }
    }
}

void TransitionSequence::Update(float dt)
{
    for (int i = 0; i < m_nNumTransitions; i++)
    {
        if (m_pPlaying[i] == 1)
        {
            m_pTransitions[i]->Update(dt);

            int next = i + 1;
            bool isFinished = m_pTransitions[i]->IsFinished();
            if (!isFinished)
            {
                if (next >= m_nNumTransitions)
                {
                    continue;
                }

                if (!((1.0f - m_pEarly[next]) < m_pTransitions[i]->Time()))
                {
                    continue;
                }
            }

            isFinished = m_pTransitions[i]->IsFinished();

            if (isFinished)
            {
                m_pTransitions[i]->Cancel();
                m_pPlaying[i] = 2;
            }

            if (next < m_nNumTransitions)
            {
                ScreenTransitionManager* manager = ScreenTransitionManager::s_pInstance;
                if (manager->m_pCallback != NULL)
                {
                    manager->m_pCallback->SequenceSwitch();
                }

                if (m_pPlaying[next] == 0)
                {
                    m_pTransitions[next]->Reset();
                    m_pTransitions[next]->Update(0.0f);
                    m_pPlaying[next] = 1;
                }
            }
        }
    }

    float progress = m_Time / GetTransitionLength();

    m_Time += dt;

    float newProgress = m_Time / GetTransitionLength();

    if ((progress <= m_CutAt) && (newProgress > m_CutAt))
    {
        ScreenTransitionManager::Instance()->m_Cut = true;
    }
}

void TransitionSequence::Reset()
{
    for (int i = 0; i < m_nNumTransitions; i++)
    {
        m_pPlaying[i] = 0;
        m_pTransitions[i]->Reset();
    }

    m_pPlaying[0] = 1;
    m_Time = 0.0f;
}

void TransitionSequence::Render(GLView* glView)
{
    for (int i = 0; i < m_nNumTransitions; i++)
    {
        if (m_pPlaying[i] == 1)
        {
            m_pTransitions[i]->Render(glView);
        }
    }
}

void TransitionSequence::Cancel()
{
    for (int i = 0; i < m_nNumTransitions; i++)
    {
        m_pTransitions[i]->Cancel();
    }
}

float TransitionSequence::GetTransitionLength()
{
    float totalLength = 0.0f;

    for (int i = 0; i < m_nNumTransitions; i++)
    {
        float transitionLength = m_pTransitions[i]->GetTransitionLength();
        totalLength += transitionLength;

        totalLength -= m_pEarly[i];
    }

    return totalLength;
}

float TransitionSequence::Time() const
{
    return 0.0f;
}

bool TransitionSequence::IsFinished()
{
    for (int i = 0; i < m_nNumTransitions; i++)
    {
        if (m_pPlaying[i] != 2)
        {
            return false;
        }
    }

    return true;
}

void TransitionSequence::Initialize(SimpleParser* parser)
{
    ScreenTransition* pTrans[20];
    float pEarly[20] = { };
    TransitionSounds sound[20] = { };

    m_nNumTransitions = 0;

    char* token = parser->NextToken(true);
    while (token != NULL)
    {
        if (nlStrCmp<char>(token, "end") == 0)
            break;

        if (nlStrCmp<char>(token, "colourblend") == 0)
        {
            ColourBlendScreenTransition* transition = ColourBlendScreenTransition::GetFromParser(parser);
            pTrans[m_nNumTransitions] = transition;
            m_nNumTransitions++;
        }
        else if (nlStrCmp<char>(token, "transition") == 0)
        {
            ScriptedScreenTransition* transition = new (8, false) ScriptedScreenTransition();
            transition->InitializeFromParser(parser);
            pTrans[m_nNumTransitions] = transition;
            m_nNumTransitions++;
        }
        else if (nlStrCmp<char>(token, "early") == 0)
        {
            pEarly[m_nNumTransitions] = atof(parser->NextTokenOnLine(true));
        }
        else if (nlStrCmp<char>(token, "sound") == 0 || nlStrCmp<char>(token, "soundstopatend") == 0)
        {
            const char* soundName = parser->NextTokenOnLine(true);
            int soundNameLen = nlStrLen<char>(soundName);

            sound[m_nNumTransitions].soundStr = (char*)nlMalloc(soundNameLen + 1, 8, false);
            nlStrNCpy<char>(sound[m_nNumTransitions].soundStr, soundName, soundNameLen + 1);

            if (nlStrCmp<char>(token, "soundstopatend") == 0)
            {
                sound[m_nNumTransitions].stopAtEnd = true;
            }
            else
            {
                sound[m_nNumTransitions].stopAtEnd = false;
            }
        }
        else if (nlStrCmp<char>(token, "model") == 0)
        {
            ModeledScreenTransition* transition = new (8, false) ModeledScreenTransition();
            transition->LoadFromParser(parser);
            pTrans[m_nNumTransitions] = transition;
            m_nNumTransitions++;
        }
        else if (nlStrCmp<char>(token, "cut_at") == 0)
        {
            m_CutAt = atof(parser->NextTokenOnLine(true));
        }

        token = parser->NextToken(true);
    }

    m_pTransitions = (ScreenTransition**)nlMalloc(m_nNumTransitions * sizeof(ScreenTransition*), 8, false);
    memcpy(m_pTransitions, pTrans, m_nNumTransitions * sizeof(ScreenTransition*));
    m_pEarly = (float*)nlMalloc(m_nNumTransitions * sizeof(float), 8, false);
    memcpy(m_pEarly, pEarly, m_nNumTransitions * sizeof(float));
    m_pSound = (TransitionSounds*)nlMalloc(m_nNumTransitions * sizeof(TransitionSounds), 8, false);

    for (int i = 0; i < m_nNumTransitions; i++)
    {
        m_pSound[i].soundStr = NULL;
        s32 len = nlStrLen<char>(sound[i].soundStr);
        if (len > 0)
        {
            m_pSound[i].soundStr = (char*)nlMalloc(len + 1, 8, false);
            nlStrNCpy<char>(m_pSound[i].soundStr, sound[i].soundStr, len + 1);
            m_pSound[i].stopAtEnd = sound[i].stopAtEnd;
            delete sound[i].soundStr;
        }
    }

    m_pPlaying = (char*)nlMalloc(m_nNumTransitions, 8, false);
    for (int i = 0; i < m_nNumTransitions; i++)
    {
        m_pPlaying[i] = 0;
    }
}
