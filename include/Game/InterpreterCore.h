#ifndef GAME_INTERPRETER_CORE_H
#define GAME_INTERPRETER_CORE_H

#include "types.h"

struct FunctionEntryPoint
{
    /* 0x00 */ u32 hash;
    /* 0x04 */ u32 offset;
    /* 0x08 */ u16 unknown_0x08;
    /* 0x0A */ u8 unknown_0x0A;
    /* 0x0B */ u8 unknown_0x0B;

    operator unsigned long() const { return hash; }
};

struct ByteCodeHeader
{
    /* 0x00 */ u32 signature;
    /* 0x04 */ u32 numFunctions;
    /* 0x08 */ u32 unknown_0x08;
    /* 0x0C */ u32 unknown_0x0C;
    /* 0x10 */ u32 unknown_0x10;
    /* 0x14 */ u32 unknown_0x14;
    /* 0x18 */ u32 unknown_0x18;
    /* 0x1C */ u32 unknown_0x1C;
    /* 0x20 */ u32 unknown_0x20;
    /* 0x24 */ u32 unknown_0x24;
    /* 0x28 */ u32 unknown_0x28;
    /* 0x2C */ u32 unknown_0x2C;
    /* 0x30 */ FunctionEntryPoint* m_FunctionTable;
    /* 0x34 */ u8* unknown_0x34;
    /* 0x38 */ u8* unknown_0x38;
    /* 0x3C */ u32* m_DataSegment;
    /* 0x40 */ u16* m_CodeSegment;
    /* 0x44 */ u8* m_StringSegment;
};

struct UnidentifiedInterpreterStorage;
class InterpreterCore;

extern "C" FunctionEntryPoint* fn_802DF3E4(
    InterpreterCore* core, u32* hash);

class InterpreterCore
{
public:
    InterpreterCore(unsigned int size);
    virtual ~InterpreterCore();
    virtual void DoFunctionCall(unsigned int) = 0;
    virtual bool UnidentifiedVirtual2(FunctionEntryPoint*, unsigned int, u32, u32, u32, u32);

    void LoadByteCode(void* data);
    void CallFunction(u32 hash)
    {
        FunctionEntryPoint* fnc_ptr = fn_802DF3E4(this, &hash);
        UnidentifiedVirtual2(fnc_ptr, 0, 0, 0, 0, 0);
    }
    void Run();
    void StopWithoutUndo();
    void StopWithUndo();
    void Step();

protected:
    u32 Pop()
    {
        m_SP--;
        return *m_SP;
    }

public:
    /* 0x04 */ u32* m_SP;
    /* 0x08 */ ByteCodeHeader* m_Header;
    /* 0x0C */ u32* m_StackSegment;
    /* 0x10 */ u32* unknown_0x10;
    /* 0x14 */ UnidentifiedInterpreterStorage* unknown_0x14;
    /* 0x18 */ u16* m_IP;
    /* 0x1C */ u32* m_BP;
    /* 0x20 */ u32* m_SavedSP;
    /* 0x24 */ u32 m_Stop : 1;
    /* 0x24 */ u32 m_RunState : 2;
}; // size: 0x28

#endif // GAME_INTERPRETER_CORE_H
