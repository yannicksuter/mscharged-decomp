#include "Game/InterpreterCore.h"

#include "Game/TweakValue.h"

#include "NL/nlAlgorithm.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <string.h>

#include "Game/UnidentifiedStaticStorage.h"

typedef void (*InterpreterOperation)(InterpreterCore*);

inline TweakIntBinding::TweakIntBinding(int* value)
    : m_pValue(value)
{
}

struct InterpreterTweakStorage
{
    /* 0x00 */ u32 unknown_0x00;
    /* 0x04 */ u32 unknown_0x04;
    /* 0x08 */ u32 unknown_0x08;
    /* 0x0C */ TweakIntBinding* unknown_0x0C;
    /* 0x10 */ TweakFloatBinding* unknown_0x10;
    /* 0x14 */ TweakBoolBinding* unknown_0x14;
};

extern const char sInterpreterEmptyTweakGroup[8];
extern const float sInterpreterDefaultTweakParameter;

static inline float& StackFloat(u32* value)
{
    return *(float*)value;
}

void InterpreterOpStop(InterpreterCore* core)
{
    core->m_SP--;
    core->StopWithoutUndo();
}

void InterpreterOpAverageFloat(InterpreterCore* core)
{
    u32 count = *--core->m_SP;
    float result = 0.0f;

    for (u32 i = 0; i < count; i++)
    {
        result += StackFloat(--core->m_SP);
    }

    float average = result / count;
    *core->m_SP = *(u32*)&average;
    core->m_SP++;
}

void InterpreterOpDotProductFloat(InterpreterCore* core)
{
    u32 count = *--core->m_SP;
    float rhs;
    float lhs;
    float result = 0.0f;

    while (count != 0)
    {
        rhs = StackFloat(--core->m_SP);
        lhs = StackFloat(--core->m_SP);
        result += lhs * rhs;
        count -= 2;
    }

    float value = result;
    *core->m_SP = *(u32*)&value;
    core->m_SP++;
}

void InterpreterOpNegateFloat(InterpreterCore* core)
{
    StackFloat(core->m_SP - 1) = -StackFloat(core->m_SP - 1);
}

void InterpreterOpNegateInt(InterpreterCore* core)
{
    core->m_SP[-1] = -core->m_SP[-1];
}

void InterpreterOpNot(InterpreterCore* core)
{
    core->m_SP[-1] = !core->m_SP[-1];
}

void InterpreterOpMinFloat(InterpreterCore* core)
{
    u32* stack = --core->m_SP;
    float rhs = StackFloat(stack);
    float lhs = StackFloat(stack - 1);
    float result = lhs <= rhs ? lhs : rhs;
    stack[-1] = *(u32*)&result;
}

void InterpreterOpMinInt(InterpreterCore* core)
{
    core->m_SP--;
    s32 result = core->m_SP[0];
    s32 lhs = core->m_SP[-1];
    if (lhs <= result)
    {
        result = lhs;
    }
    core->m_SP[-1] = result;
}

void InterpreterOpMaxFloat(InterpreterCore* core)
{
    u32* stack = --core->m_SP;
    float rhs = StackFloat(stack);
    float lhs = StackFloat(stack - 1);
    float result = lhs >= rhs ? lhs : rhs;
    stack[-1] = *(u32*)&result;
}

void InterpreterOpMaxInt(InterpreterCore* core)
{
    core->m_SP--;
    s32 result = core->m_SP[0];
    s32 lhs = core->m_SP[-1];
    if (lhs >= result)
    {
        result = lhs;
    }
    core->m_SP[-1] = result;
}

void InterpreterOpModuloUnsignedInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] %= core->m_SP[0];
}

void InterpreterOpDivideFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    float result = lhs / rhs;
    core->m_SP[-1] = *(u32*)&result;
}

void InterpreterOpDivideInt(InterpreterCore* core)
{
    core->m_SP--;
    *(s32*)(core->m_SP - 1) /= *(s32*)core->m_SP;
}

void InterpreterOpMultiplyFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    float result = lhs * rhs;
    core->m_SP[-1] = *(u32*)&result;
}

void InterpreterOpMultiplyInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] *= core->m_SP[0];
}

void InterpreterOpSubtractFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    float result = lhs - rhs;
    core->m_SP[-1] = *(u32*)&result;
}

void InterpreterOpSubtractInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] -= core->m_SP[0];
}

void InterpreterOpAddFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    float result = lhs + rhs;
    core->m_SP[-1] = *(u32*)&result;
}

void InterpreterOpAddInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] += core->m_SP[0];
}

void InterpreterOpGreaterEqualString(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) >= 0;
}

void InterpreterOpGreaterEqualFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs >= rhs;
}

void InterpreterOpGreaterEqualInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = *(s32*)(core->m_SP - 1) >= *(s32*)core->m_SP;
}

void InterpreterOpGreaterString(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) > 0;
}

void InterpreterOpGreaterFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs > rhs;
}

void InterpreterOpGreaterInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = *(s32*)(core->m_SP - 1) > *(s32*)core->m_SP;
}

void InterpreterOpLessEqualString(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) <= 0;
}

void InterpreterOpLessEqualFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs <= rhs;
}

void InterpreterOpLessEqualInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = *(s32*)(core->m_SP - 1) <= *(s32*)core->m_SP;
}

void InterpreterOpLessString(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) < 0;
}

void InterpreterOpLessFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs < rhs;
}

void InterpreterOpLessInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = *(s32*)(core->m_SP - 1) < *(s32*)core->m_SP;
}

void InterpreterOpNotEqualString(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];

    if (lhs == rhs)
    {
        core->m_SP[-1] = false;
        return;
    }
    if (lhs == 0 || rhs == 0)
    {
        core->m_SP[-1] = true;
        return;
    }
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) != 0;
}

void InterpreterOpNotEqualFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs != rhs;
}

void InterpreterOpNotEqualInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = core->m_SP[-1] != core->m_SP[0];
}

void InterpreterOpEqualString(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];

    if (lhs == rhs)
    {
        core->m_SP[-1] = true;
        return;
    }
    if (lhs == 0 || rhs == 0)
    {
        core->m_SP[-1] = false;
        return;
    }
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) == 0;
}

void InterpreterOpEqualFloat(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs == rhs;
}

void InterpreterOpEqualInt(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = core->m_SP[-1] == core->m_SP[0];
}

void InterpreterOpAnd(InterpreterCore* core)
{
    u32* stack = --core->m_SP;
    stack[-1] = stack[-1] && stack[0];
}

void InterpreterOpOr(InterpreterCore* core)
{
    u32* stack = --core->m_SP;
    stack[-1] = stack[-1] || stack[0];
}

InterpreterOperation gInterpreterOperations[] = {
    InterpreterOpOr,
    InterpreterOpAnd,
    InterpreterOpEqualInt,
    InterpreterOpEqualFloat,
    InterpreterOpEqualString,
    InterpreterOpNotEqualInt,
    InterpreterOpNotEqualFloat,
    InterpreterOpNotEqualString,
    InterpreterOpLessInt,
    InterpreterOpLessFloat,
    InterpreterOpLessString,
    InterpreterOpLessEqualInt,
    InterpreterOpLessEqualFloat,
    InterpreterOpLessEqualString,
    InterpreterOpGreaterInt,
    InterpreterOpGreaterFloat,
    InterpreterOpGreaterString,
    InterpreterOpGreaterEqualInt,
    InterpreterOpGreaterEqualFloat,
    InterpreterOpGreaterEqualString,
    InterpreterOpAddInt,
    InterpreterOpAddFloat,
    InterpreterOpSubtractInt,
    InterpreterOpSubtractFloat,
    InterpreterOpMultiplyInt,
    InterpreterOpMultiplyFloat,
    InterpreterOpDivideInt,
    InterpreterOpDivideFloat,
    InterpreterOpModuloUnsignedInt,
    InterpreterOpMaxInt,
    InterpreterOpMaxFloat,
    InterpreterOpMinInt,
    InterpreterOpMinFloat,
    0,
    InterpreterOpNot,
    InterpreterOpNegateInt,
    InterpreterOpNegateFloat,
    InterpreterOpDotProductFloat,
    InterpreterOpAverageFloat,
    InterpreterOpStop,
};

InterpreterCore::InterpreterCore(unsigned int size)
{
    m_StackSegment = (u32*)nlMalloc(size * 4, 8, false);
    m_Header = 0;
    unknown_0x10 = 0;
    unknown_0x14 = 0;
}

InterpreterCore::~InterpreterCore()
{
    if (unknown_0x10 != 0)
    {
        nlFree(unknown_0x10);
        unknown_0x10 = 0;
    }

    if (unknown_0x14 != 0)
    {
        InterpreterTweakStorage* storage = unknown_0x14;
        delete[] storage->unknown_0x0C;
        delete[] storage->unknown_0x10;
        delete[] storage->unknown_0x14;
        delete storage;
        unknown_0x14 = 0;
    }
    nlFree(m_StackSegment);
}

void InterpreterCore::Reset()
{
    m_RunState = 2;
    m_SP = m_StackSegment;
    m_SavedSP = m_SP;
    m_BP = 0;
    m_IP = 0;
}

void InterpreterCore::InitializeTweaks()
{
    u32 count = m_Header->unknown_0x28 - m_Header->unknown_0x1C;
    if (count != 0)
    {
        AllocateTweaks(count);

        u8* data = m_Header->unknown_0x34;
        for (u32 i = 0; i < count; i++)
        {
            u32 valueIndex = m_Header->unknown_0x1C + i;
            u32 type;
            if (valueIndex < m_Header->unknown_0x20)
            {
                type = 0;
            }
            else
            {
                type = 2;
                if (valueIndex < m_Header->unknown_0x24)
                {
                    type = 1;
                }
            }

            u8 flags = *data++;
            const char* name = (const char*)data;
            data += nlStrLen<char>(name) + 1;

            u32 value0 = 0;
            u32 value1 = 0;
            u32 value2 = 0;
            u32 value3 = 0;
            if (flags & 1)
            {
                value0 = *(u32*)data;
                data += 4;
            }
            if (flags & 2)
            {
                value1 = *(u32*)data;
                data += 4;
            }
            if (flags & 4)
            {
                value2 = *(u32*)data;
                data += 4;
            }
            if (flags & 8)
            {
                value3 = *(u32*)data;
                data += 4;
            }

            RegisterTweak(i, type, name, flags, value0, value1, value2, value3);
        }
    }
}

static void RelocateStringReferences(InterpreterCore* core)
{
    if (core->m_Header->unknown_0x0C != 0)
    {
        core->unknown_0x10 = (u32*)nlMalloc(core->m_Header->unknown_0x0C, 8, false);
        memcpy(core->unknown_0x10, core->m_Header->unknown_0x38, core->m_Header->unknown_0x0C);

        u32* relocatedValue = core->unknown_0x10;
        for (unsigned int i = 0; i < core->m_Header->unknown_0x2C; i++)
        {
            *relocatedValue += (u32)core->m_Header->m_StringSegment;
        }
    }
}

void InterpreterCore::LoadByteCode(void* data)
{
    if (unknown_0x10 != 0)
    {
        nlFree(unknown_0x10);
        unknown_0x10 = 0;
    }

    if (unknown_0x14 != 0)
    {
        InterpreterTweakStorage* storage = unknown_0x14;
        delete[] storage->unknown_0x0C;
        delete[] storage->unknown_0x10;
        delete[] storage->unknown_0x14;
        delete storage;
        unknown_0x14 = 0;
    }

    m_Header = (ByteCodeHeader*)data;
    if (m_Header->m_CodeSegment == 0)
    {
        ByteCodeHeader* header = m_Header;
        header->m_FunctionTable = (FunctionEntryPoint*)(header + 1);
        header->unknown_0x34 = (u8*)(header->m_FunctionTable + header->numFunctions);
        header->unknown_0x38 = header->unknown_0x34 + header->unknown_0x08;
        header->m_DataSegment = (u32*)(header->unknown_0x38 + header->unknown_0x0C);
        header->m_CodeSegment = (u16*)((u8*)header->m_DataSegment + header->unknown_0x10);
        header->m_StringSegment = (u8*)header->m_CodeSegment + header->unknown_0x14;

        for (unsigned int i = 0; i < m_Header->numFunctions; i++)
        {
            m_Header->m_FunctionTable[i].offset = (u32)((u8*)m_Header->m_CodeSegment + m_Header->m_FunctionTable[i].offset);
        }
    }

    RelocateStringReferences(this);

    InitializeTweaks();
    Reset();
}

void InterpreterCore::RunFunction(FunctionEntryPoint* entry, unsigned int count)
{
    m_SP[0] = 0;
    m_SP[1] = (u32)m_BP;
    m_BP = m_SP - count;

    if (entry->unknown_0x0B == 3)
    {
        m_BP--;
    }

    m_SP += entry->unknown_0x08;
    if (m_RunState != 1)
    {
        m_RunState = 0;
    }

    u16* saved_ip = m_IP;
    m_IP = (u16*)entry->offset;
    Step();

    if (m_RunState != 3)
    {
        m_IP = saved_ip;
        if (entry->unknown_0x0B & 1)
        {
            m_SP--;
        }
    }
}

bool InterpreterCore::ExecuteFunction(FunctionEntryPoint* entry, unsigned int count, unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3)
{
    if (m_RunState == 3)
    {
        Reset();
    }

    if (entry == 0)
    {
        return false;
    }

    if (entry->unknown_0x0B == 3)
    {
        m_SP++;
    }

    switch (count)
    {
    case 4:
        m_SP[3] = value3;
    case 3:
        m_SP[2] = value2;
    case 2:
        m_SP[1] = value1;
    case 1:
        m_SP[0] = value0;
        m_SP += count;
    case 0:
        break;
    default:
        nlBreak();
        break;
    }

    RunFunction(entry, count);
    return true;
}

bool InterpreterCore::ExecuteFunction(FunctionEntryPoint* entry, unsigned int count, const unsigned int* values)
{
    if (m_RunState == 3)
    {
        Reset();
    }

    if (entry == 0)
    {
        return false;
    }

    if (entry->unknown_0x0B == 3)
    {
        m_SP++;
    }

    memcpy(m_SP, values, count << 2);
    m_SP += count;
    RunFunction(entry, count);
    return true;
}

FunctionEntryPoint* InterpreterCore::FindFunctionEntryPoint(const unsigned int& hash)
{
    unsigned long value = hash;
    return nlBSearch<FunctionEntryPoint, unsigned long>(value, m_Header->m_FunctionTable, m_Header->numFunctions);
}

FunctionEntryPoint* InterpreterCore::GetFunctionEntryPoint(unsigned int index)
{
    return &m_Header->m_FunctionTable[index];
}

void InterpreterCore::Run()
{
    Step();
}

void InterpreterCore::Step()
{
    unsigned int wasNotRunning = m_RunState != 1;
    m_RunState = 1;
    m_Stop = 0;

    while (!m_Stop)
    {
        u16* instructionPointer = m_IP;
        u16 instruction = *instructionPointer;
        u16 opcode = instruction >> 11;
        u16 operand = instruction & 0x7FF;

        switch (opcode)
        {
        case 0:
            *m_SP = m_Header->m_DataSegment[operand];
            m_SP++;
            break;

        case 1:
            *m_SP = (u32)(m_Header->m_StringSegment + m_Header->m_DataSegment[operand]);
            m_SP++;
            break;

        case 2:
            *m_SP = operand;
            m_SP++;
            break;

        case 3:
            *m_SP = (u32)(m_Header->m_StringSegment + operand);
            m_SP++;
            break;

        case 4:
            if (Pop() == 0)
            {
                break;
            }
        case 5:
            m_IP += operand;
            continue;

        case 6:
            if (Pop() == 0)
            {
                break;
            }
        case 7:
            m_IP -= operand;
            continue;

        case 8:
            if (wasNotRunning)
            {
                m_SavedSP = m_SP;
            }
            DoFunctionCall(operand);
            break;

        case 9:
        {
            FunctionEntryPoint* entry = &m_Header->m_FunctionTable[operand];
            m_SP[0] = (u32)instructionPointer;
            m_SP[1] = (u32)m_BP;
            m_BP = m_SP - entry->unknown_0x0A;
            m_SP += entry->unknown_0x08;
            m_IP = (u16*)entry->offset;
            continue;
        }

        case 10:
        {
            u32* oldBP = m_BP;
            u32* frame = oldBP + (operand >> 1);
            u32* newSP = oldBP + (operand & 1);
            m_SP = frame;
            m_BP = (u32*)frame[1];
            m_IP = (u16*)frame[0];
            m_SP = newSP;
            if (m_IP == 0)
            {
                m_Stop = 1;
                continue;
            }
            break;
        }

        case 11:
            *m_SP = m_BP[operand];
            m_SP++;
            break;

        case 12:
            m_BP[operand] = Pop();
            break;

        case 13:
            gInterpreterOperations[operand](this);
            break;

        case 14:
        {
            if (operand < m_Header->unknown_0x1C)
            {
                *m_SP = unknown_0x10[operand];
                m_SP++;
            }
            else
            {
                InterpreterTweakStorage* storage = unknown_0x14;
                u32 value;
                u32 index = operand;
                index -= m_Header->unknown_0x1C;
                u32 intTweakCount = storage->unknown_0x00;
                if (index < intTweakCount)
                {
                    value = *storage->unknown_0x0C[index].m_pValue;
                }
                else if (index < m_Header->unknown_0x24)
                {
                    float floatValue = *storage->unknown_0x10[index - intTweakCount].m_pValue;
                    value = *(u32*)&floatValue;
                }
                else
                {
                    value = *storage->unknown_0x14[index - intTweakCount - storage->unknown_0x04].m_pValue;
                }
                *m_SP = value;
                m_SP++;
            }
            break;
        }

        case 15:
        {
            if (operand < m_Header->unknown_0x1C)
            {
                unknown_0x10[operand] = Pop();
            }
            else
            {
                InterpreterTweakStorage* storage = unknown_0x14;
                u32 value = Pop();
                u32 index = operand;
                index -= m_Header->unknown_0x1C;
                u32 intTweakCount = storage->unknown_0x00;
                if (index < intTweakCount)
                {
                    *storage->unknown_0x0C[index].m_pValue = value;
                }
                else if (index < m_Header->unknown_0x24)
                {
                    *storage->unknown_0x10[index - intTweakCount].m_pValue = *(float*)&value;
                }
                else
                {
                    *storage->unknown_0x14[index - intTweakCount - storage->unknown_0x04].m_pValue = value != 0;
                }
            }
            break;
        }

        case 16:
            m_SP += (s8)operand;
            break;

        case 17:
            m_BP[operand] = m_SP[-1];
            break;

        case 18:
            m_BP[operand & 0x1F] = m_BP[operand >> 5];
            break;

        case 19:
            m_BP[operand >> 5] = 16 - (operand & 0x1F);
            break;

        case 20:
        {
            u32 upper = operand >> 5;
            u32 lower = operand & 0x1F;
            m_SP[0] = m_BP[upper];
            m_SP[1] = m_BP[upper + 16 - lower];
            m_SP += 2;
            break;
        }

        case 21:
        {
            int index0;
            int index1;
            int index2;
            index0 = instruction & 0x7FF;
            index0 >>= 6;
            index1 = index0 + 4 - ((operand >> 3) & 7);
            index2 = index1 + 4 - (operand & 7);
            m_SP[0] = m_BP[index0];
            m_SP[1] = m_BP[index1];
            m_SP[2] = m_BP[index2];
            m_SP += 3;
            break;
        }
        }

        m_IP++;
    }

    if (!wasNotRunning)
    {
        m_Stop = 0;
    }
    else if (m_RunState != 3)
    {
        m_RunState = 2;
    }
}

void InterpreterCore::StopWithoutUndo()
{
    m_Stop = 1;
    m_RunState = 3;
}

void InterpreterCore::StopWithUndo()
{
    m_IP--;
    m_SP = m_SavedSP;
    m_Stop = 1;
    m_RunState = 3;
}

int InterpreterCore::GetInstructionOffset()
{
    return (m_IP - m_Header->m_CodeSegment);
}

void InterpreterCore::AllocateTweaks(unsigned int)
{
    InterpreterTweakStorage* storage = (InterpreterTweakStorage*)nlMalloc(sizeof(InterpreterTweakStorage), 8, false);

    storage->unknown_0x00 = m_Header->unknown_0x20 - m_Header->unknown_0x1C;
    if (storage->unknown_0x00 != 0)
    {
        storage->unknown_0x0C = new (8, false) TweakIntBinding[storage->unknown_0x00];
    }
    else
    {
        storage->unknown_0x0C = 0;
    }

    storage->unknown_0x04 = m_Header->unknown_0x24 - m_Header->unknown_0x20;
    if (storage->unknown_0x04 != 0)
    {
        storage->unknown_0x10 = new (8, false) TweakFloatBinding[storage->unknown_0x04];
    }
    else
    {
        storage->unknown_0x10 = 0;
    }

    storage->unknown_0x08 = m_Header->unknown_0x28 - m_Header->unknown_0x24;
    if (storage->unknown_0x08 != 0)
    {
        storage->unknown_0x14 = new (8, false) TweakBoolBinding[storage->unknown_0x08];
    }
    else
    {
        storage->unknown_0x14 = 0;
    }

    unknown_0x14 = storage;
}

extern const float sInterpreterDefaultTweakParameter = 0.0f;

void InterpreterCore::RegisterTweak(unsigned int index, unsigned int type, const char* name, u8 flags, unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3)
{
    float float1 = sInterpreterDefaultTweakParameter;
    float float2 = sInterpreterDefaultTweakParameter;
    float float3 = sInterpreterDefaultTweakParameter;
    InterpreterTweakStorage* storage = unknown_0x14;

    switch (type)
    {
    case 0:
    {
        if (flags & 2)
        {
            float1 = (float)(s32)value1;
        }
        if (flags & 4)
        {
            float2 = (float)(s32)value2;
        }
        if (flags & 8)
        {
            float3 = (float)(s32)value3;
        }

        if (flags & 1)
        {
            TweakIntBinding* intTarget = &storage->unknown_0x0C[index];
            bool result = intTarget->Bind(name, float1,
                sInterpreterEmptyTweakGroup, false, float2, float3);
            if (result == 0)
            {
                *intTarget->m_pValue = value0;
            }
        }
        else
        {
            TweakIntBinding* target = &storage->unknown_0x0C[index];
            target->Bind(name, float1,
                sInterpreterEmptyTweakGroup, false, float2, float3);
        }
        break;
    }

    case 1:
    {
        if (flags & 2)
        {
            float1 = *(float*)&value1;
        }
        if (flags & 4)
        {
            float2 = *(float*)&value2;
        }
        if (flags & 8)
        {
            float3 = *(float*)&value3;
        }

        if (flags & 1)
        {
            TweakFloatBinding& target = storage->unknown_0x10[index - storage->unknown_0x00];
            float defaultValue = *(float*)&value0;
            target.BindWithDefault(name, defaultValue, sInterpreterEmptyTweakGroup, false, float1, float2, float3);
        }
        else
        {
            TweakFloatBinding* target = &storage->unknown_0x10[index - storage->unknown_0x00];
            target->Bind(name, float1, sInterpreterEmptyTweakGroup, false, float2, float3);
        }
        break;
    }

    case 2:
    {
        unsigned int storageIndex = index - storage->unknown_0x00 - storage->unknown_0x04;
        if (flags & 1)
        {
            TweakBoolBinding* target;
            bool defaultValue = value0 != 0;
            target = &storage->unknown_0x14[storageIndex];
            if (target->Bind(name) == 0)
            {
                *target->m_pValue = defaultValue;
            }
        }
        else
        {
            TweakBoolBinding* target = &storage->unknown_0x14[storageIndex];
            if (target->Bind(name) == 0)
            {
                *target->m_pValue = target->GetDefault();
            }
        }
        break;
    }
    }
}
