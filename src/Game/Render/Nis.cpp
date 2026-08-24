#include "Game/Render/Nis.h"

#include "Game/Camera/animcam.h"

void Nis::AddTrigger(NisTriggerType triggerType, float frameNumber,
    const char* name, const char* target, Nis::TriggerParams* trigParams)
{
    mTriggers[mNumTriggers].type = triggerType;
    mTriggers[mNumTriggers].frameNumber = frameNumber;
    mTriggers[mNumTriggers].name = name;
    mTriggers[mNumTriggers].target = target;

    TriggerParams* pParams = &(mTriggers[mNumTriggers].params);
    pParams->float1 = -1.0f;
    pParams->param1 = -1;
    pParams->param2 = -1;
    pParams->param3 = -1;
    pParams->param4 = -1;

    if (trigParams != 0)
    {
        mTriggers[mNumTriggers].params.float1 = trigParams->float1;
        mTriggers[mNumTriggers].params.param1 = trigParams->param1;
        mTriggers[mNumTriggers].params.param2 = trigParams->param2;
        mTriggers[mNumTriggers].params.param3 = trigParams->param3;
        mTriggers[mNumTriggers].params.param4 = trigParams->param4;
    }

    mNumTriggers++;
}

nlVector3 Nis::Offset() const
{
    return mHeader->stadiumOffset;
}

void Nis::UpdateTriggers(float oldTime, float newTime, float duration)
{
    if (duration != 0.0f)
    {
        for (int i = 0; i < mNumTriggers; ++i)
        {
            float triggerFrame = (mTriggers[i].frameNumber / 30.0f) / duration;
            if ((oldTime <= triggerFrame) && (newTime > triggerFrame))
            {
                mTriggers[i].Fire(*this);
            }
        }
    }
}

void Nis::Update(float dt)
{
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        cPN_SAnimController* pController = mCharacterControllers[i];
        if (pController != 0)
        {
            pController->Update(dt);
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        cAnimCamera* pCamera = mCameras[i];
        if (pCamera != 0)
        {
            pCamera->Update(dt);
        }
    }
}

char* Nis::Name() const
{
    return mHeader->name;
}
