#ifndef GAME_TWEAK_INT_BINDING_INL
#define GAME_TWEAK_INT_BINDING_INL

#include "Game/TweakValue.inl"
#include "Game/TweakValueInt.h"
#include "NL/nlPrint.h"

#include <stdlib.h>

bool TweakIntBinding::BindWithDefault(const char* name, int defaultValue,
    const char* group, bool reload, float value, float min, float max)
{
    bool found = Bind(name, value, group, reload, min, max);
    if (!found)
    {
        *m_pValue = defaultValue;
    }
    return found;
}

int TweakIntBinding::GetDefault()
{
    return 0;
}

int TweakIntBinding::GetValueType()
{
    return 3;
}

int TweakIntBinding::GetStorageKind()
{
    return 2;
}

TweakValueBase* TweakIntBinding::CreateValue(
    const char* name, void* entry)
{
    TweakValueInt* created = new (
        gTweakValueAllocator->Allocate(sizeof(TweakValueInt)))
        TweakValueInt(name, 0);
    AddTweakValue((TweakEntry*)entry, created);
    return created;
}

void TweakIntBinding::CopyValueFrom(
    TweakValueBase* other)
{
    switch (other->GetStorageKind())
    {
    case 1:
        *m_pValue = ((TweakValueInt*)other)->value;
        break;
    case 2:
        *m_pValue = *((TweakIntBinding*)other)->m_pValue;
        break;
    }
}

void* TweakIntBinding::GetValueAddress()
{
    return m_pValue;
}

extern char gTweakIntBindingFormat[];

void TweakIntBinding::FormatValue(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, gTweakIntBindingFormat, *m_pValue);
}

void TweakIntBinding::ParseValue(const char* value)
{
    *m_pValue = atoi(value);
}

int TweakIntBinding::IsBound()
{
    return m_pValue != 0;
}

void TweakIntBinding::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

void TweakIntBinding::BindValueAddress(void* value)
{
    m_pValue = (int*)value;
}

#endif // GAME_TWEAK_INT_BINDING_INL
