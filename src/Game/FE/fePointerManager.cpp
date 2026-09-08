#include "Game/FE/fePointerManager.h"

#include "Game/TweakValue.h"

FEPointerManager::FEPointerManager()
    : mUnidentified01C(0)
    , mListenerCount(0)
    , mUnidentified054(-1)
{
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified00C[i] = 0;
        mUnidentified020[i][0] = 0.0f;
        mUnidentified020[i][1] = 0.0f;
        mUnidentified040[i] = 0;
        mUnidentified048[i] = false;
        mUnidentified04C[i] = true;
    }
}

FEPointerManager::~FEPointerManager()
{
}

void FEPointerManager::RegisterListener(FEPointerListener* listener)
{
    mListeners.AddEnd(listener);
    ++mListenerCount;
}

void FEPointerManager::UnregisterListener(FEPointerListener* listener)
{
    mListeners.RemoveEntry(listener);
    --mListenerCount;
}

static float sPositionRadius = 0.02f;
static float sPositionSensitivity = 0.95f;
static TweakValueImpl_804F4DC8 sPositionRadiusTweak("Pos Radius", "FE", &sPositionRadius);
static TweakValueImpl_804F4DC8 sPositionSensitivityTweak("Pos Sensitivity", "FE", &sPositionSensitivity);
