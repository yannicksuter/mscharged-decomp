#include "Game/InterpreterOperations.h"

#include "Game/InterpreterCore.h"
#include "NL/nlString.h"

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
