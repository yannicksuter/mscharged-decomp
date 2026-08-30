#include "Game/InterpreterCore.h"

#include "Game/TweakValue.h"

#include "NL/nlAlgorithm.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <string.h>

typedef void (*InterpreterOperation)(InterpreterCore*);

inline TweakValueImpl_804F4DC8::TweakValueImpl_804F4DC8(float* value)
    : m_pValue(value)
{
}

inline TweakValueIntImpl_804FD898::TweakValueIntImpl_804FD898(int* value)
    : m_pValue(value)
{
}

inline TweakValueBoolImpl_804F4538::TweakValueBoolImpl_804F4538(bool* value)
    : m_pValue(value)
{
}

struct UnidentifiedInterpreterStorage
{
    /* 0x00 */ u32 unknown_0x00;
    /* 0x04 */ u32 unknown_0x04;
    /* 0x08 */ u32 unknown_0x08;
    /* 0x0C */ TweakValueIntImpl_804FD898* unknown_0x0C;
    /* 0x10 */ TweakValueImpl_804F4DC8* unknown_0x10;
    /* 0x14 */ TweakValueBoolImpl_804F4538* unknown_0x14;

    ~UnidentifiedInterpreterStorage()
    {
        delete[] unknown_0x0C;
        delete[] unknown_0x10;
        delete[] unknown_0x14;
    }
};

extern "C" void fn_802DEE14(InterpreterCore*);
extern "C" void fn_802DFA1C(InterpreterCore*, unsigned int);
extern "C" void fn_802DFC40(InterpreterCore*, unsigned int, unsigned int, const char*, u8, u32, u32, u32, u32);
extern "C" bool fn_802C4FEC(TweakValueBase_8052BF70*, const char*, const void*, bool, float, float, float);
extern "C" bool fn_802C4F94(TweakValueBase_8052BF70*, const char*);
extern "C" u32 lbl_806DF460[2];
extern "C" const float lbl_806E64A0;

static inline float& StackFloat(u32* value)
{
    return *(float*)value;
}

extern "C" void fn_802DE284(InterpreterCore* core)
{
    core->m_SP--;
    core->StopWithoutUndo();
}

extern "C" void fn_802DE294(InterpreterCore* core)
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

extern "C" void fn_802DE3CC(InterpreterCore* core)
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

extern "C" void fn_802DE438(InterpreterCore* core)
{
    StackFloat(core->m_SP - 1) = -StackFloat(core->m_SP - 1);
}

extern "C" void fn_802DE44C(InterpreterCore* core)
{
    core->m_SP[-1] = -core->m_SP[-1];
}

extern "C" void fn_802DE460(InterpreterCore* core)
{
    core->m_SP[-1] = !core->m_SP[-1];
}

extern "C" void fn_802DE478(InterpreterCore* core)
{
    u32* stack = --core->m_SP;
    float rhs = StackFloat(stack);
    float lhs = StackFloat(stack - 1);
    float result = lhs <= rhs ? lhs : rhs;
    stack[-1] = *(u32*)&result;
}

extern "C" void fn_802DE4B8(InterpreterCore* core)
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

extern "C" void fn_802DE4E0(InterpreterCore* core)
{
    u32* stack = --core->m_SP;
    float rhs = StackFloat(stack);
    float lhs = StackFloat(stack - 1);
    float result = lhs >= rhs ? lhs : rhs;
    stack[-1] = *(u32*)&result;
}

extern "C" void fn_802DE520(InterpreterCore* core)
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

extern "C" void fn_802DE548(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] %= core->m_SP[0];
}

extern "C" void fn_802DE570(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    float result = lhs / rhs;
    core->m_SP[-1] = *(u32*)&result;
}

extern "C" void fn_802DE5A0(InterpreterCore* core)
{
    core->m_SP--;
    *(s32*)(core->m_SP - 1) /= *(s32*)core->m_SP;
}

extern "C" void fn_802DE5C0(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    float result = lhs * rhs;
    core->m_SP[-1] = *(u32*)&result;
}

extern "C" void fn_802DE5F0(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] *= core->m_SP[0];
}

extern "C" void fn_802DE610(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    float result = lhs - rhs;
    core->m_SP[-1] = *(u32*)&result;
}

extern "C" void fn_802DE640(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] -= core->m_SP[0];
}

extern "C" void fn_802DE660(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    float result = lhs + rhs;
    core->m_SP[-1] = *(u32*)&result;
}

extern "C" void fn_802DE690(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] += core->m_SP[0];
}

extern "C" void fn_802DE6B0(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) >= 0;
}

extern "C" void fn_802DE738(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs >= rhs;
}

extern "C" void fn_802DE764(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = *(s32*)(core->m_SP - 1) >= *(s32*)core->m_SP;
}

extern "C" void fn_802DE790(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) > 0;
}

extern "C" void fn_802DE81C(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs > rhs;
}

extern "C" void fn_802DE844(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = *(s32*)(core->m_SP - 1) > *(s32*)core->m_SP;
}

extern "C" void fn_802DE874(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) <= 0;
}

extern "C" void fn_802DE900(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs <= rhs;
}

extern "C" void fn_802DE92C(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = *(s32*)(core->m_SP - 1) <= *(s32*)core->m_SP;
}

extern "C" void fn_802DE958(InterpreterCore* core)
{
    core->m_SP--;
    const char* lhs = (const char*)core->m_SP[-1];
    const char* rhs = (const char*)core->m_SP[0];
    core->m_SP[-1] = nlStrICmp<char>(lhs, rhs) < 0;
}

extern "C" void fn_802DE9DC(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs < rhs;
}

extern "C" void fn_802DEA04(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = *(s32*)(core->m_SP - 1) < *(s32*)core->m_SP;
}

extern "C" void fn_802DEA34(InterpreterCore* core)
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

extern "C" void fn_802DEAF0(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs != rhs;
}

extern "C" void fn_802DEB1C(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = core->m_SP[-1] != core->m_SP[0];
}

extern "C" void fn_802DEB48(InterpreterCore* core)
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

extern "C" void fn_802DEC00(InterpreterCore* core)
{
    core->m_SP--;
    float rhs = StackFloat(core->m_SP);
    float lhs = StackFloat(core->m_SP - 1);
    core->m_SP[-1] = lhs == rhs;
}

extern "C" void fn_802DEC28(InterpreterCore* core)
{
    core->m_SP--;
    core->m_SP[-1] = core->m_SP[-1] == core->m_SP[0];
}

extern "C" void fn_802DEC50(InterpreterCore* core)
{
    u32* stack = --core->m_SP;
    stack[-1] = stack[-1] && stack[0];
}

extern "C" void fn_802DEC84(InterpreterCore* core)
{
    u32* stack = --core->m_SP;
    stack[-1] = stack[-1] || stack[0];
}

extern "C" InterpreterOperation lbl_8052EDC8[] = {
    fn_802DEC84,
    fn_802DEC50,
    fn_802DEC28,
    fn_802DEC00,
    fn_802DEB48,
    fn_802DEB1C,
    fn_802DEAF0,
    fn_802DEA34,
    fn_802DEA04,
    fn_802DE9DC,
    fn_802DE958,
    fn_802DE92C,
    fn_802DE900,
    fn_802DE874,
    fn_802DE844,
    fn_802DE81C,
    fn_802DE790,
    fn_802DE764,
    fn_802DE738,
    fn_802DE6B0,
    fn_802DE690,
    fn_802DE660,
    fn_802DE640,
    fn_802DE610,
    fn_802DE5F0,
    fn_802DE5C0,
    fn_802DE5A0,
    fn_802DE570,
    fn_802DE548,
    fn_802DE520,
    fn_802DE4E0,
    fn_802DE4B8,
    fn_802DE478,
    0,
    fn_802DE460,
    fn_802DE44C,
    fn_802DE438,
    fn_802DE3CC,
    fn_802DE294,
    fn_802DE284,
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

    delete unknown_0x14;
    unknown_0x14 = 0;
    nlFree(m_StackSegment);
}

extern "C" void fn_802DEDE8(InterpreterCore* core)
{
    core->m_SP = core->m_StackSegment;
    core->m_RunState = 2;
    core->m_SavedSP = core->m_SP;
    core->m_BP = 0;
    core->m_IP = 0;
}

extern "C" void fn_802DEE14(InterpreterCore* core)
{
    u32 count = core->m_Header->unknown_0x28 - core->m_Header->unknown_0x1C;
    if (count != 0)
    {
        fn_802DFA1C(core, count);

        u8* data = core->m_Header->unknown_0x34;
        for (u32 i = 0; i < count; i++)
        {
            u32 valueIndex = core->m_Header->unknown_0x1C + i;
            u32 type;
            if (valueIndex < core->m_Header->unknown_0x20)
            {
                type = 0;
            }
            else
            {
                type = 2;
                if (valueIndex < core->m_Header->unknown_0x24)
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

            fn_802DFC40(core, i, type, name, flags, value0, value1, value2, value3);
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

    delete unknown_0x14;
    unknown_0x14 = 0;

    ByteCodeHeader* header = (ByteCodeHeader*)data;
    m_Header = header;
    if (header->m_CodeSegment == 0)
    {
        u32 numFunctions = header->numFunctions;
        u32 size08 = header->unknown_0x08;
        u32 size0C = header->unknown_0x0C;
        u32 size10 = header->unknown_0x10;
        u32 size14 = header->unknown_0x14;
        FunctionEntryPoint* functionTable = (FunctionEntryPoint*)(header + 1);
        u8* segment34 = (u8*)(functionTable + numFunctions);
        u8* segment38 = segment34 + size08;
        u32* dataSegment = (u32*)(segment38 + size0C);
        u16* codeSegment = (u16*)((u8*)dataSegment + size10);
        u8* stringSegment = (u8*)codeSegment + size14;

        header->m_FunctionTable = functionTable;
        header->unknown_0x34 = segment34;
        header->unknown_0x38 = segment38;
        header->m_DataSegment = dataSegment;
        header->m_CodeSegment = codeSegment;
        header->m_StringSegment = stringSegment;

        for (unsigned int i = 0; i < m_Header->numFunctions; i++)
        {
            m_Header->m_FunctionTable[i].offset += (u32)m_Header->m_CodeSegment;
        }
    }

    if (header->unknown_0x0C != 0)
    {
        unknown_0x10 = (u32*)nlMalloc(header->unknown_0x0C, 8, false);
        memcpy(unknown_0x10, m_Header->unknown_0x38, m_Header->unknown_0x0C);

        u32* relocatedValue = unknown_0x10;
        for (unsigned int i = 0; i < m_Header->unknown_0x2C; i++)
        {
            *relocatedValue += (u32)m_Header->m_StringSegment;
        }
    }

    fn_802DEE14(this);
    fn_802DEDE8(this);
}

extern "C" void fn_802DF11C(InterpreterCore* core, FunctionEntryPoint* entry, unsigned int count)
{
    core->m_SP[0] = 0;
    core->m_SP[1] = (u32)core->m_BP;
    core->m_BP = core->m_SP - count;

    if (entry->unknown_0x0B == 3)
    {
        core->m_BP--;
    }

    core->m_SP += entry->unknown_0x08;
    if (core->m_RunState != 1)
    {
        core->m_RunState = 0;
    }

    u16* saved_ip = core->m_IP;
    core->m_IP = (u16*)entry->offset;
    core->Step();

    if (core->m_RunState != 3)
    {
        core->m_IP = saved_ip;
        if (entry->unknown_0x0B & 1)
        {
            core->m_SP--;
        }
    }
}

bool InterpreterCore::UnidentifiedVirtual2(FunctionEntryPoint* entry, unsigned int count, u32 value0, u32 value1, u32 value2, u32 value3)
{
    if (m_RunState == 3)
    {
        fn_802DEDE8(this);
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

    fn_802DF11C(this, entry, count);
    return true;
}

extern "C" bool fn_802DF314(InterpreterCore* core, FunctionEntryPoint* entry, unsigned int count, const u32* values)
{
    if (core->m_RunState == 3)
    {
        fn_802DEDE8(core);
    }

    if (entry == 0)
    {
        return false;
    }

    if (entry->unknown_0x0B == 3)
    {
        core->m_SP++;
    }

    memcpy(core->m_SP, values, count << 2);
    core->m_SP += count;
    fn_802DF11C(core, entry, count);
    return true;
}

extern "C" FunctionEntryPoint* fn_802DF3E4(InterpreterCore* core, u32* hash)
{
    unsigned long value = *hash;
    return nlBSearch<FunctionEntryPoint, unsigned long>(value, core->m_Header->m_FunctionTable, core->m_Header->numFunctions);
}

extern "C" FunctionEntryPoint* fn_802DF41C(InterpreterCore* core, unsigned int index)
{
    return &core->m_Header->m_FunctionTable[index];
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
            lbl_8052EDC8[operand](this);
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
                UnidentifiedInterpreterStorage* storage = unknown_0x14;
                u32 value;
                u32 index = operand;
                index -= m_Header->unknown_0x1C;
                if (index < storage->unknown_0x00)
                {
                    value = *storage->unknown_0x0C[index].m_pValue;
                }
                else if (index < m_Header->unknown_0x24)
                {
                    float floatValue = *storage->unknown_0x10[index - storage->unknown_0x00].m_pValue;
                    value = *(u32*)&floatValue;
                }
                else
                {
                    value = *storage->unknown_0x14[index - storage->unknown_0x00 - storage->unknown_0x04].m_pValue;
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
                UnidentifiedInterpreterStorage* storage = unknown_0x14;
                u32 value = Pop();
                u32 index = operand;
                index -= m_Header->unknown_0x1C;
                if (index < storage->unknown_0x00)
                {
                    *storage->unknown_0x0C[index].m_pValue = value;
                }
                else if (index < m_Header->unknown_0x24)
                {
                    *storage->unknown_0x10[index - storage->unknown_0x00].m_pValue = *(float*)&value;
                }
                else
                {
                    *storage->unknown_0x14[index - storage->unknown_0x00 - storage->unknown_0x04].m_pValue = value != 0;
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
            index0 = operand;
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

extern "C" int fn_802DF9FC(InterpreterCore* core)
{
    return (core->m_IP - core->m_Header->m_CodeSegment);
}

extern "C" void fn_802DFA1C(InterpreterCore* core, unsigned int)
{
    UnidentifiedInterpreterStorage* storage = (UnidentifiedInterpreterStorage*)nlMalloc(sizeof(UnidentifiedInterpreterStorage), 8, false);

    storage->unknown_0x00 = core->m_Header->unknown_0x20 - core->m_Header->unknown_0x1C;
    if (storage->unknown_0x00 != 0)
    {
        storage->unknown_0x0C = new (8, false) TweakValueIntImpl_804FD898[storage->unknown_0x00];
    }
    else
    {
        storage->unknown_0x0C = 0;
    }

    storage->unknown_0x04 = core->m_Header->unknown_0x24 - core->m_Header->unknown_0x20;
    if (storage->unknown_0x04 != 0)
    {
        storage->unknown_0x10 = new (8, false) TweakValueImpl_804F4DC8[storage->unknown_0x04];
    }
    else
    {
        storage->unknown_0x10 = 0;
    }

    storage->unknown_0x08 = core->m_Header->unknown_0x28 - core->m_Header->unknown_0x24;
    if (storage->unknown_0x08 != 0)
    {
        storage->unknown_0x14 = new (8, false) TweakValueBoolImpl_804F4538[storage->unknown_0x08];
    }
    else
    {
        storage->unknown_0x14 = 0;
    }

    core->unknown_0x14 = storage;
}

extern "C" const float lbl_806E64A0 = 0.0f;

extern "C" void fn_802DFC40(InterpreterCore* core, unsigned int index, unsigned int type, const char* name, u8 flags, u32 value0, u32 value1, u32 value2, u32 value3)
{
    float float1 = lbl_806E64A0;
    float float2 = lbl_806E64A0;
    float float3 = lbl_806E64A0;
    UnidentifiedInterpreterStorage* storage = core->unknown_0x14;

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
            TweakValueIntImpl_804FD898* intTarget = &storage->unknown_0x0C[index];
            bool result = fn_802C4FEC(intTarget, name, lbl_806DF460, false, float1, float2, float3);
            if (result == 0)
            {
                *intTarget->m_pValue = intTarget->UnidentifiedVirtual3C();
            }
            if (result == 0)
            {
                *intTarget->m_pValue = value0;
            }
        }
        else
        {
            TweakValueIntImpl_804FD898* target = &storage->unknown_0x0C[index];
            if (fn_802C4FEC(target, name, lbl_806DF460, false, float1, float2, float3) == 0)
            {
                *target->m_pValue = target->UnidentifiedVirtual3C();
            }
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
            TweakValueImpl_804F4DC8& target = storage->unknown_0x10[index - storage->unknown_0x00];
            float defaultValue = *(float*)&value0;
            bool result = fn_802C4FEC(&target, name, lbl_806DF460, false, float1, float2, float3);
            if (result == 0)
            {
                *target.m_pValue = target.UnidentifiedVirtual3C();
            }
            if (result == 0)
            {
                *target.m_pValue = defaultValue;
            }
        }
        else
        {
            TweakValueImpl_804F4DC8* target = &storage->unknown_0x10[index - storage->unknown_0x00];
            if (fn_802C4FEC(target, name, lbl_806DF460, false, float1, float2, float3) == 0)
            {
                *target->m_pValue = target->UnidentifiedVirtual3C();
            }
        }
        break;
    }

    case 2:
    {
        unsigned int storageIndex = index - storage->unknown_0x00 - storage->unknown_0x04;
        if (flags & 1)
        {
            TweakValueBoolImpl_804F4538* target;
            bool defaultValue = value0 != 0;
            target = &storage->unknown_0x14[storageIndex];
            if (fn_802C4F94(target, name) == 0)
            {
                *target->m_pValue = defaultValue;
            }
        }
        else
        {
            TweakValueBoolImpl_804F4538* target = &storage->unknown_0x14[storageIndex];
            if (fn_802C4F94(target, name) == 0)
            {
                *target->m_pValue = target->UnidentifiedVirtual3C();
            }
        }
        break;
    }
    }
}
