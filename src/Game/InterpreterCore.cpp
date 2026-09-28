#include "Game/InterpreterCore.h"
#include "Game/InterpreterOperations.h"

#include "Game/TweakValue.h"
#include "Game/TweakValue.inl"

#include "NL/nlAlgorithm.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <string.h>

#include "Game/UnidentifiedStaticStorage.h"

struct InterpreterTweakStorage
{
    /* 0x00 */ u32 numIntTweaks;
    /* 0x04 */ u32 numFloatTweaks;
    /* 0x08 */ u32 numBoolTweaks;
    /* 0x0C */ TweakIntBinding* intBindings;
    /* 0x10 */ TweakFloatBinding* floatBindings;
    /* 0x14 */ TweakBoolBinding* boolBindings;
};

InterpreterCore::InterpreterCore(unsigned int size)
{
    m_StackSegment = (u32*)nlMalloc(size * 4, 8, false);
    m_Header = 0;
    m_Globals = 0;
    m_TweakStorage = 0;
}

InterpreterCore::~InterpreterCore()
{
    if (m_Globals != 0)
    {
        nlFree(m_Globals);
        m_Globals = 0;
    }

    if (m_TweakStorage != 0)
    {
        InterpreterTweakStorage* storage = m_TweakStorage;
        delete[] storage->intBindings;
        delete[] storage->floatBindings;
        delete[] storage->boolBindings;
        delete storage;
        m_TweakStorage = 0;
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
    u8* data;
    u32 count = m_Header->numVariables - m_Header->numGlobals;
    u32 i;
    if (count != 0)
    {
        AllocateTweaks(count);

        data = m_Header->m_TweakData;
        for (i = 0; i < count; i++)
        {
            u32 valueIndex = i + m_Header->numGlobals;
            u32 type;
            if (valueIndex < m_Header->firstFloatTweak)
            {
                type = 0;
            }
            else
            {
                type = 2;
                if (valueIndex < m_Header->firstBoolTweak)
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
    if (core->m_Header->globalDataSize != 0)
    {
        core->m_Globals = (u32*)nlMalloc(core->m_Header->globalDataSize, 8, false);
        memcpy(core->m_Globals, core->m_Header->m_GlobalInitData, core->m_Header->globalDataSize);

        u32* relocatedValue = core->m_Globals;
        for (unsigned int i = 0; i < core->m_Header->numStringRefs; i++)
        {
            *relocatedValue += (u32)core->m_Header->m_StringSegment;
        }
    }
}

void InterpreterCore::LoadByteCode(void* data)
{
    if (m_Globals != 0)
    {
        nlFree(m_Globals);
        m_Globals = 0;
    }

    if (m_TweakStorage != 0)
    {
        InterpreterTweakStorage* storage = m_TweakStorage;
        delete[] storage->intBindings;
        delete[] storage->floatBindings;
        delete[] storage->boolBindings;
        delete storage;
        m_TweakStorage = 0;
    }

    m_Header = (ByteCodeHeader*)data;
    if (m_Header->m_CodeSegment == 0)
    {
        ByteCodeHeader* header = m_Header;
        header->m_FunctionTable = (FunctionEntryPoint*)(header + 1);
        header->m_TweakData = (u8*)(header->m_FunctionTable + header->numFunctions);
        header->m_GlobalInitData = header->m_TweakData + header->tweakDataSize;
        header->m_DataSegment = (u32*)(header->m_GlobalInitData + header->globalDataSize);
        header->m_CodeSegment = (u16*)((u8*)header->m_DataSegment + header->dataSegmentSize);
        header->m_StringSegment = (u8*)header->m_CodeSegment + header->codeSegmentSize;

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

    if (entry->flags == 3)
    {
        m_BP--;
    }

    m_SP += entry->frameSize;
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
        if (entry->flags & 1)
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

    if (entry->flags == 3)
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

    if (entry->flags == 3)
    {
        m_SP++;
    }

    memcpy(m_SP, values, count << 2);
    m_SP += count;
    RunFunction(entry, count);
    return true;
}

void InterpreterCore::RegisterTweak(unsigned int index, unsigned int type, const char* name, u8 flags, unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3)
{
    float float1 = 0.0f;
    float float2 = 0.0f;
    float float3 = 0.0f;
    InterpreterTweakStorage* storage = m_TweakStorage;

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
            storage->intBindings[index].BindWithDefault(
                name, (int)value0, "", false, float1, float2, float3);
        }
        else
        {
            TweakIntBinding* target = &storage->intBindings[index];
            target->Bind(name, float1, "", false, float2, float3);
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
            storage->floatBindings[index - storage->numIntTweaks].BindWithDefault(
                name, *(float*)&value0, "", false, float1, float2, float3);
        }
        else
        {
            TweakFloatBinding* target = &storage->floatBindings[index - storage->numIntTweaks];
            target->Bind(name, float1, "", false, float2, float3);
        }
        break;
    }

    case 2:
    {
        unsigned int storageIndex = index - storage->numIntTweaks - storage->numFloatTweaks;
        if (flags & 1)
        {
            TweakBoolBinding* target;
            bool defaultValue = value0 != 0;
            target = &storage->boolBindings[storageIndex];
            if (target->Bind(name) == 0)
            {
                *target->m_pValue = defaultValue;
            }
        }
        else
        {
            TweakBoolBinding* target = &storage->boolBindings[storageIndex];
            if (target->Bind(name) == 0)
            {
                *target->m_pValue = target->GetDefault();
            }
        }
        break;
    }
    }
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
            m_BP = m_SP - entry->numArgs;
            m_SP += entry->frameSize;
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
            if (operand < m_Header->numGlobals)
            {
                *m_SP = m_Globals[operand];
                m_SP++;
            }
            else
            {
                InterpreterTweakStorage* storage = m_TweakStorage;
                u32 value;
                u32 index = operand;
                index -= m_Header->numGlobals;
                u32 intTweakCount = storage->numIntTweaks;
                if (index < intTweakCount)
                {
                    value = *storage->intBindings[index].m_pValue;
                }
                else if (index < m_Header->firstBoolTweak)
                {
                    float floatValue = *storage->floatBindings[index - intTweakCount].m_pValue;
                    value = *(u32*)&floatValue;
                }
                else
                {
                    value = *storage->boolBindings[index - intTweakCount - storage->numFloatTweaks].m_pValue;
                }
                *m_SP = value;
                m_SP++;
            }
            break;
        }

        case 15:
        {
            if (operand < m_Header->numGlobals)
            {
                m_Globals[operand] = Pop();
            }
            else
            {
                InterpreterTweakStorage* storage = m_TweakStorage;
                u32 value = Pop();
                u32 index = operand;
                index -= m_Header->numGlobals;
                u32 intTweakCount = storage->numIntTweaks;
                if (index < intTweakCount)
                {
                    *storage->intBindings[index].m_pValue = value;
                }
                else if (index < m_Header->firstBoolTweak)
                {
                    *storage->floatBindings[index - intTweakCount].m_pValue = *(float*)&value;
                }
                else
                {
                    *storage->boolBindings[index - intTweakCount - storage->numFloatTweaks].m_pValue = value != 0;
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

    storage->numIntTweaks = m_Header->firstFloatTweak - m_Header->numGlobals;
    if (storage->numIntTweaks != 0)
    {
        storage->intBindings = new (8, false) TweakIntBinding[storage->numIntTweaks];
    }
    else
    {
        storage->intBindings = 0;
    }

    storage->numFloatTweaks = m_Header->firstBoolTweak - m_Header->firstFloatTweak;
    if (storage->numFloatTweaks != 0)
    {
        storage->floatBindings = new (8, false) TweakFloatBinding[storage->numFloatTweaks];
    }
    else
    {
        storage->floatBindings = 0;
    }

    storage->numBoolTweaks = m_Header->numVariables - m_Header->firstBoolTweak;
    if (storage->numBoolTweaks != 0)
    {
        storage->boolBindings = new (8, false) TweakBoolBinding[storage->numBoolTweaks];
    }
    else
    {
        storage->boolBindings = 0;
    }

    m_TweakStorage = storage;
}
