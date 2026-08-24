#ifndef GAME_INTERPRETER_CORE_H
#define GAME_INTERPRETER_CORE_H

#include "types.h"

struct ByteCodeHeader;

class InterpreterCore
{
public:
    InterpreterCore(unsigned int size);
    virtual ~InterpreterCore();
    virtual void DoFunctionCall(unsigned int) = 0;

protected:
    u32 Pop()
    {
        m_SP--;
        return *m_SP;
    }

public:
    /* 0x04 */ u32 m_Return;
    /* 0x08 */ ByteCodeHeader* m_Header;
    /* 0x0C */ u32* m_StackSegment;
    /* 0x10 */ u16* m_IP;
    /* 0x14 */ u32* m_SP;
    /* 0x18 */ u32* m_BP;
    /* 0x1C */ u32* m_SavedSP;
    /* 0x20 */ u32 m_Stop : 1;
    /* 0x20 */ u32 m_RunState : 2;
    /* 0x24 */ u32 unknown_0x24;
    /* 0x28 */ u32 unknown_0x28;
};

#endif // GAME_INTERPRETER_CORE_H
