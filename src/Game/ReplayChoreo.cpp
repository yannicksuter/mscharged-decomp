#include "Game/ReplayChoreo.h"

#include "Game/Camera/CameraMan.h"
#include "Game/Render/NetMesh.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"

namespace
{
char* replayTypeNames[8] = {
    "NORMAL_SHOT",
    "ONE_TIMER",
    "SKILLSHOT",
    "DEFLECTION",
    "LOOSE_BALL",
    "OWN_GOAL",
    "MEGA_STRIKE",
    "WALK_IN",
};
char* zoneDepthNames[3] = { "MID", "CLOSE", "DEEP" };
char* zoneInWidthNames[3] = { "CENTER", "FRONT", "BACK" };
} // namespace

ReplayChoreo& ReplayChoreo::Instance()
{
    static ReplayChoreo instance;
    return instance;
}

void ReplayChoreo::LoadScript()
{
    if (mByteCode != 0)
    {
        nlFree(mByteCode);
    }

    unsigned long fileSize = 0;
    mByteCode = nlLoadEntireFile("art/Scripts/replay_choreo.byte_code", &fileSize,
        0x20, AllocateStart, 0, 0, 0);
    LoadByteCode(mByteCode);

    for (int d = 0; d < 3; d++)
    {
        for (int w = 0; w < 3; w++)
        {
            for (int t = 0; t < 8; t++)
            {
                mNumScripts[d][w][t] = 0;
                for (int j = 0;; j++)
                {
                    char name[0x40];
                    nlSNPrintf(name, sizeof(name), "%s_%s_%s_%d",
                        zoneDepthNames[d], zoneInWidthNames[w], replayTypeNames[t], j);
                    if (!FunctionExists(nlStringHash(name)))
                    {
                        break;
                    }
                    mNumScripts[d][w][t]++;
                }
            }
        }
    }
}

void ReplayChoreo::Reset()
{
    mCamera.m_pFilter[0] = 0;
    mCamera.m_pFilter[1] = 0;
    cCameraManager::Remove(mCamera);
}

void ReplayChoreo::Finish()
{
    NetMesh::spNegativeXNetMesh->Reset(false);
    NetMesh::spPositiveXNetMesh->Reset(false);
}

void ReplayChoreo::FlushHighlights()
{
    mUnidentified3F4 = 0;
    mHighlights[0].mUnidentified000 = -1;
    mHighlights[1].mUnidentified000 = -1;
    mHighlights[2].mUnidentified000 = -1;
    mUnidentified3F0 = 0;
    mUnidentified3EC = 0;
}

bool ReplayChoreo::Done(float param) const
{
    if (IsFinished())
    {
        if (nlTaskManager::m_pInstance->mCurrentState == 8)
        {
            return true;
        }
    }

    if (param > 0.0f)
    {
        float endTime = mReplayManager->mReplay->EndTime();
        float timeRemaining = (1.0f / mReplayManager->mSpeed) *
                              (endTime - mReplayManager->mTime);
        if (timeRemaining < param)
        {
            return true;
        }
    }

    return false;
}

int ReplayChoreo::NumHighlights() const
{
    int count = 0;
    for (Highlight* highlight = mUnidentified3EC; highlight != 0;
         highlight = highlight->mUnidentified030)
    {
        count++;
    }
    return count;
}

int ReplayChoreo::fn_80195CBC() const
{
    Highlight* highlight = mUnidentified3EC;
    int index = 1;
    for (; highlight != 0; highlight = highlight->mUnidentified030)
    {
        if (highlight == mUnidentified3F4)
        {
            return NumHighlights() - index;
        }
        index++;
    }
    return -1;
}
