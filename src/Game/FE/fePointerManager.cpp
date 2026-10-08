#include "Game/FE/fePointerManager.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"

FEPointerManager* g_pFEPointerManager;

FEPointerManager::FEPointerManager()
    : m_pad01C(0)
    , mListenerCount(0)
    , m_pad054(-1)
{
    for (int i = 0; i < 4; ++i)
    {
        m_pad00C[i] = 0;
        m_pad020[i][0] = 0.0f;
        m_pad020[i][1] = 0.0f;
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
static TweakFloatBinding sPositionRadiusTweak("Pos Radius", "FE", &sPositionRadius);
static TweakFloatBinding sPositionSensitivityTweak("Pos Sensitivity", "FE", &sPositionSensitivity);
