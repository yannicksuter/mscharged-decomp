#include "Game/NisPlayer.h"

#include "Game/Effects/EmissionManager.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

inline void* operator new(unsigned long, void* ptr)
{
    return ptr;
}

NisPlayer* NisPlayer::sInstance;

bool NisPlayer::WorldIsFrozen() const
{
    for (int i = 0; i < 8; i++)
    {
        if (mPlaying[i] != 0 && mPlaying[i]->unknown_0x034 != 0)
        {
            return true;
        }
    }
    return false;
}

void NisPlayer::SetExtraNameFilter(const char* filter)
{
    nlStrNCpy(mExtraNameFilter, filter, 128);
}

void NisPlayer::ResetEffects()
{
    EmissionManager::Instance()->Destroy((unsigned long)this, 0);
    EmissionManager::Instance()->DestroyAll(0, true);
    EmissionManager::Instance()->DestroyAll(3, true);
}

void NisPlayer::Load(char* buffer, unsigned int size, NisHeader& nisHeader)
{
    if (!mActive)
        return;

    for (int i = 0; i < 8; i++)
    {
        if (mLoaded[i] != 0)
            continue;

        if (nisHeader.unknown_0x198 == 0)
        {
            for (int j = 0; j < 8; j++)
            {
                if (&nisHeader == mLoadQueue[j])
                {
                    mLoadQueue[j] = 0;
                    mAsyncStarted[j] = false;
                    break;
                }
            }
        }

        Nis* nis = new (nlMalloc(sizeof(Nis), 8, false))
            Nis(nisHeader, buffer, size);
        mLoaded[i] = nis;
        LoadTriggers(*mLoaded[i]);
        return;
    }
}

NisPlayer* NisPlayer::Instance()
{
    if (sInstance == 0)
    {
        sInstance = new NisPlayer;
    }
    return sInstance;
}
