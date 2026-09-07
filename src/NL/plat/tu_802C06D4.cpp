#include "NL/globalpad.h"
#include "NL/nlMemory.h"

PadManager_802C06D4* lbl_806E1E28;

PadManager_802C06D4::PadManager_802C06D4()
{
}

void PadManager_802C06D4::fn_802C06D8(int padCount, int padSetCount)
{
    mUnidentified000 = padCount;
    mUnidentified004 = padSetCount;
    mUnidentified008 = 0;
    m_aPads = new (8, false) cGlobalPad*[padSetCount * padCount];
    cGlobalPad** pPads;
    for (int i = 0; i < mUnidentified004; ++i)
    {
        pPads = &m_aPads[i * mUnidentified000];
        for (int j = 0; j < mUnidentified000; ++j)
        {
            *pPads++ = new (8, false) cGlobalPad(j);
        }
    }
}

void PadManager_802C06D4::Update(float deltaTime)
{
    for (int i = 0; i < mUnidentified000; ++i)
    {
        m_aPads[i + mUnidentified008 * mUnidentified000]->Update(deltaTime);
    }
}

cGlobalPad* PadManager_802C06D4::GetPad(int idx)
{
    return m_aPads[idx + mUnidentified008 * mUnidentified000];
}

void PadManager_802C06D4::fn_802C084C(int padSet)
{
    mUnidentified008 = padSet;
}
