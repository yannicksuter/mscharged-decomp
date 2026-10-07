#include "Game/Triggers/SebringAnimScript.h"

#include "Game/CharacterTriggers.h"
#include "Game/Triggers/AnimTrigger.h"
#include "NL/nlDebug.h"

void SebringAnimTagScriptInterpreter::TriggerFired(
    cSAnim* animation, unsigned long triggerId)
{
    if (m_FireTriggers)
    {
        AnimTriggerCallbackInfo data;
        data.m_uEventID = triggerId;
        data.m_fIntensity = 100.0f;
        CharacterTriggerHandler(animation, (unsigned int)&data);
    }
}

void SebringAnimTagScriptInterpreter::DoFunctionCall(unsigned int func)
{
    switch (func)
    {
    case 0:
    {
        m_SP--;
        const char* script = (const char*)*m_SP;
        m_SP--;
        unsigned int charClass = *m_SP;
        m_SP--;
        const char* name = (const char*)*m_SP;
        break;
    }
    case 1:
    {
        m_SP--;
        const char* script = (const char*)*m_SP;
        m_SP--;
        float fVol = *(float*)m_SP;
        m_SP--;
        unsigned int charClass = *m_SP;
        m_SP--;
        const char* name = (const char*)*m_SP;
        break;
    }
    case 2:
    {
        m_SP--;
        const char* script = (const char*)*m_SP;
        m_SP--;
        const char* name = (const char*)*m_SP;
        break;
    }
    case 3:
    {
        m_SP--;
        const char* script = (const char*)*m_SP;
        m_SP--;
        float fVol = *(float*)m_SP;
        m_SP--;
        const char* name = (const char*)*m_SP;
        break;
    }
    case 4:
    {
        m_SP--;
        const char* script = (const char*)*m_SP;
        m_SP--;
        unsigned int charClass = *m_SP;
        m_SP--;
        const char* name = (const char*)*m_SP;
        break;
    }
    case 5:
    {
        m_SP--;
        const char* script = (const char*)*m_SP;
        m_SP--;
        const char* name = (const char*)*m_SP;
        break;
    }
    default:
        nlBreak();
        break;
    }
}
