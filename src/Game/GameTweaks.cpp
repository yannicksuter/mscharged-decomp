#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/nlPrint.h"

#include <stdlib.h>

TweakValueImpl_804F4DC8::TweakValueImpl_804F4DC8(float* value)
    : m_pValue(value)
{
}

TweakValueIntImpl_804FD898::TweakValueIntImpl_804FD898(int* value)
    : m_pValue(value)
{
}

bool TweakValueIntImpl_804FD898::fn_800757B4(const char* name, int defaultValue,
    const char* group, bool reload, float value, float min, float max)
{
    bool found = fn_802C4FEC(name, value, group, reload, min, max);
    if (!found)
    {
        *m_pValue = UnidentifiedVirtual3C();
    }
    if (!found)
    {
        *m_pValue = defaultValue;
    }
    return found;
}

int TweakValueIntImpl_804FD898::UnidentifiedVirtual3C()
{
    return 0;
}

int TweakValueIntImpl_804FD898::UnidentifiedVirtual0C()
{
    return 3;
}

int TweakValueIntImpl_804FD898::UnidentifiedVirtual10()
{
    return 2;
}

void TweakValueIntImpl_804FD898::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

TweakValueBase_8052BF70* TweakValueIntImpl_804FD898::UnidentifiedVirtual34(
    const char* name, void* entry)
{
    GXMaterialColourTweak_804FC520* created = new (
        lbl_806E1E58->Allocate(sizeof(GXMaterialColourTweak_804FC520)))
        GXMaterialColourTweak_804FC520(name, 0);
    fn_802C5780((TweakEntry_8052BF00*)entry, created);
    return created;
}

void* TweakValueIntImpl_804FD898::UnidentifiedVirtual20()
{
    return m_pValue;
}

void TweakValueIntImpl_804FD898::UnidentifiedVirtual24(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *m_pValue);
}

void TweakValueIntImpl_804FD898::UnidentifiedVirtual28(const char* value)
{
    *m_pValue = atoi(value);
}

void TweakValueIntImpl_804FD898::UnidentifiedVirtual2C(
    TweakValueBase_8052BF70* other)
{
    switch (other->UnidentifiedVirtual10())
    {
    case 1:
        *m_pValue = ((GXMaterialColourTweak_804FC520*)other)->value;
        break;
    case 2:
        *m_pValue = *((TweakValueIntImpl_804FD898*)other)->m_pValue;
        break;
    }
}

int TweakValueIntImpl_804FD898::UnidentifiedVirtual30()
{
    return m_pValue != 0;
}

void TweakValueIntImpl_804FD898::UnidentifiedVirtual38(void* value)
{
    m_pValue = (int*)value;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
