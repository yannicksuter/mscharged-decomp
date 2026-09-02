#include "Game/Pad/FlickDetection.h"

#include "Game/AI/AIPad.h"

namespace
{

static const char FLICK_NONE = 0x20;

char historyStorage[16][16] = { };
StaticRing<char> history[16];

} // namespace

extern "C" cAIPad* fn_80007C3C(int);

void FlickDetection::Update()
{
    for (int i = 0; i < 16; i++)
    {
        cAIPad* pad = fn_80007C3C(i);
        if (pad == NULL || pad->m_pGlobalPad == NULL)
        {
            continue;
        }

        StaticRing<char>& ring = history[i];
        float magnitude = pad->GetCStickMovementStickMagnitude();

        if (magnitude > 0.6f)
        {
            u16 direction = pad->GetCStickMovementStickDirection();

            char flickValue = 0x72;
            if (direction >= 0x2000 && direction < 0x6000)
            {
                flickValue = 0x75;
            }
            else if (direction >= 0x6000 && direction < 0xA000)
            {
                flickValue = 0x6c;
            }
            else if (direction >= 0xA000 && direction < 0xE000)
            {
                flickValue = 0x64;
            }

            ring.mStorage[ring.mEnd] = flickValue;
            ring.mEnd = (ring.mEnd + 1) % ring.mCapacity;

            ring.mSize += 1;
            if (ring.mSize > ring.mCapacity)
            {
                ring.mSize = ring.mCapacity;
            }
        }
        else
        {
            ring.mStorage[ring.mEnd] = FLICK_NONE;
            ring.mEnd = (ring.mEnd + 1) % ring.mCapacity;

            ring.mSize += 1;
            if (ring.mSize > ring.mCapacity)
            {
                ring.mSize = ring.mCapacity;
            }
        }
    }
}

void FlickDetection::Initialize()
{
    for (int i = 0; i < 16; i++)
    {
        history[i].mStorage = &historyStorage[i][0];
        history[i].mCapacity = 16;
        history[i].mSize = 0;
        history[i].mEnd = 0;
    }
}
