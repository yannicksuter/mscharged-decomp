#ifndef GAME_INTERPRETER_CORE_H
#define GAME_INTERPRETER_CORE_H

#include "types.h"

struct UnidentifiedFunctionHash_8027F9CC
{
    UnidentifiedFunctionHash_8027F9CC(unsigned int hash)
        : unknown_0x00(hash)
    {
    }

    operator unsigned int() const { return unknown_0x00; }

    unsigned int unknown_0x00;
};

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

struct InterpreterTweakStorage;

class InterpreterCore
{
public:
    InterpreterCore(unsigned int size);
    virtual ~InterpreterCore();
    virtual void DoFunctionCall(unsigned int) = 0;
    virtual bool ExecuteFunction(FunctionEntryPoint*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

    void LoadByteCode(void* data);
    void Reset();
    FunctionEntryPoint* FindFunctionEntryPoint(const u32& hash);
    FunctionEntryPoint* GetFunctionEntryPoint(unsigned int index);
    void RunFunction(FunctionEntryPoint* entry, unsigned int count);
    bool ExecuteFunction(FunctionEntryPoint* entry, unsigned int count, const unsigned int* values);
    int GetInstructionOffset();
    void InitializeTweaks();
    void AllocateTweaks(unsigned int count);
    void RegisterTweak(unsigned int index, unsigned int type, const char* name, unsigned char flags,
        unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3);
    void CallFunction(UnidentifiedFunctionHash_8027F9CC hash)
    {
        FunctionEntryPoint* fnc_ptr = FindFunctionEntryPoint(hash);
        ExecuteFunction(fnc_ptr, 0, 0, 0, 0, 0);
    }
    void CallFunctionAt(unsigned long offset)
    {
        FunctionEntryPoint* fnc_ptr = GetFunctionEntryPoint(offset);
        ExecuteFunction(fnc_ptr, 0, 0, 0, 0, 0);
    }
    void Run();
    void StopWithoutUndo();
    void StopWithUndo();
    void Step();
    bool FunctionExists(UnidentifiedFunctionHash_8027F9CC hash)
    {
        return FindFunctionEntryPoint(hash) != 0;
    }
    bool IsFinished() const { return m_RunState == 2; }

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
    /* 0x14 */ InterpreterTweakStorage* unknown_0x14;
    /* 0x18 */ u16* m_IP;
    /* 0x1C */ u32* m_BP;
    /* 0x20 */ u32* m_SavedSP;
    /* 0x24 */ u32 m_Stop : 1;
    /* 0x24 */ u32 m_RunState : 2;
}; // size: 0x28

#endif // GAME_INTERPRETER_CORE_H
