#include "NL/globalpad.h"
#include "NL/nlMemory.h"

PadManager* g_pPadManager;

PadManager::PadManager()
{
}

void PadManager::Initialize(int padCount, int padSetCount)
{
    mPadCount = padCount;
    mPadSetCount = padSetCount;
    mActivePadSet = 0;
    m_aPads = new (8, false) cGlobalPad*[padSetCount * padCount];
    cGlobalPad** pPads;
    for (int i = 0; i < mPadSetCount; ++i)
    {
        pPads = &m_aPads[i * mPadCount];
        for (int j = 0; j < mPadCount; ++j)
        {
            *pPads++ = new (8, false) cGlobalPad(j);
        }
    }
}

void PadManager::Update(float deltaTime)
{
    for (int i = 0; i < mPadCount; ++i)
    {
        m_aPads[i + mActivePadSet * mPadCount]->Update(deltaTime);
    }
}

cGlobalPad* PadManager::GetPad(int idx)
{
    return m_aPads[idx + mActivePadSet * mPadCount];
}

void PadManager::SetActivePadSet(int padSet)
{
    mActivePadSet = padSet;
}
