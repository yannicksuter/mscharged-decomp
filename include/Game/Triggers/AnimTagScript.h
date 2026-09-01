#ifndef _ANIMTAGSCRIPT_H_
#define _ANIMTAGSCRIPT_H_

#include "Game/InterpreterCore.h"
#include "NL/nlSlotPool.h"

class AnimTagScriptInterpreter;
class cSAnim;

struct AnimTagInfo
{
    /* 0x0 */ unsigned long Trigger;
    /* 0x4 */ unsigned long ScriptFuncOffset;
}; // total size: 0x8

struct AnimTagCBInfo
{
    /* 0x0 */ AnimTagScriptInterpreter* pAnimTagScript;
    /* 0x4 */ AnimTagInfo ScriptInfo;
}; // total size: 0xC

class AnimTagScriptInterpreter : public InterpreterCore
{
public:
    AnimTagScriptInterpreter(unsigned int stackSize)
        : InterpreterCore(stackSize)
        , m_AnimTagSlotPool(16, 16)
    {
        m_BytecodeCount = 0;
    }
    virtual ~AnimTagScriptInterpreter()
    {
        SlotPoolBase::BaseFreeBlocks(
            &m_AnimTagSlotPool, sizeof(AnimTagCBInfo));
        for (int i = 0; i < m_BytecodeCount; i++)
        {
            nlFree(m_ppBytecode[i]);
        }
    }
    virtual void DoFunctionCall(unsigned int) = 0;
    virtual void TriggerFired(cSAnim*, unsigned long) { }

protected:
    /* 0x28 */ SlotPool<AnimTagCBInfo> m_AnimTagSlotPool;
    /* 0x40 */ void* m_ppBytecode[32];
    /* 0xC0 */ int m_BytecodeCount;
}; // total size: 0xC4

#endif // _ANIMTAGSCRIPT_H_
