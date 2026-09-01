#ifndef _SEBRINGANIMSCRIPT_H_
#define _SEBRINGANIMSCRIPT_H_

#include "Game/Triggers/AnimTagScript.h"

class SebringAnimTagScriptInterpreter : public AnimTagScriptInterpreter
{
public:
    SebringAnimTagScriptInterpreter()
        : AnimTagScriptInterpreter(100)
    {
        m_FireTriggers = true;
    }

    virtual void DoFunctionCall(unsigned int func);
    virtual void TriggerFired(cSAnim* arg0, unsigned long triggerId);

    /* 0xC4 */ bool m_FireTriggers;
}; // total size: 0xC8

#endif // _SEBRINGANIMSCRIPT_H_
