#include "NL/nlDLListContainer.inl"
#include "Game/ReplayChoreo.h"

#include "Game/Ball.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/NisPlayer.h"
#include "Game/Player.h"
#include "Game/Render/CrowdImpostors.h"
#include "Game/Render/NetMesh.h"
#include "Game/Render/Presentation.h"
#include "Game/Team.h"
#include "Game/main.h"
#include "NL/nlBind.h"
#include "NL/nlFile.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"

#include "Game/UnidentifiedStaticStorage.h"

#include <math.h>
#include <string.h>
#include "NL/nlFunction.inl"

namespace
{
inline int CountHighlights(ReplayChoreo::Highlight* first)
{
    ReplayChoreo::Highlight* highlight = first;
    int count = 0;
    for (; highlight != 0; highlight = highlight->next)
    {
        count++;
    }
    return count;
}

inline int PickRandom(int count)
{
    float pick = ((float)count - 1.0f) * nlRandomf(1.0f, GetPresentationRandomSeed(GetPresentation()));
    pick += pick < 0.0f ? -0.5f : 0.5f;
    return (int)pick;
}
} // namespace

// DoFunctionCall is compiled first, so its jump table and literals lead .data
// and .sdata2. With -sym on its code still follows the main .text.
#include "src/Game/ReplayChoreo_interp.cpp"

namespace
{
int cameraPick = -1;
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
float positionDampingTime = 0.25f;
const char scriptNameFormat[] = "%s_%s_%s_%d";
} // namespace

ReplayChoreo& ReplayChoreo::Instance()
{
    static ReplayChoreo instance;
    return instance;
}

ReplayChoreo::ReplayChoreo()
    : InterpreterCore(100)
    , mReplayManager(0)
    , mRunForTimeLeft(0.0f)
    , mRunningFor(false)
    , mByteCode(0)
    , mIsHighlightReel(false)
    , mHighlightList(0, 0)
    , mCurrentHighlight(0)
{
    LoadScript();
    FlushHighlights();
}

void ReplayChoreo::LoadScript()
{
    if (mByteCode != 0)
    {
        nlFree(mByteCode);
    }

    unsigned long fileSize = 0;
    mByteCode = nlLoadEntireFile("art/Scripts/replay_choreo.byte_code", &fileSize, 0x20, AllocateStart, 0, 0, 0);
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
                    nlSNPrintf(name, sizeof(name), scriptNameFormat, zoneDepthNames[d], zoneInWidthNames[w], replayTypeNames[t], j);
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

void ReplayChoreo::RegisterEventHandlers()
{
    typedef BindExp2<void,
        Detail::MemFunImpl<void, void (ReplayChoreo::*)(GoalScoredData*)>,
        ReplayChoreo*,
        Placeholder<0> >
        GoalScoredBinding;
    typedef BindExp2<void,
        Detail::MemFunImpl<void, void (ReplayChoreo::*)(GoalieSaveData*)>,
        ReplayChoreo*,
        Placeholder<0> >
        GoalieSaveBinding;

    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(GoalScoredBinding(MemFun(&ReplayChoreo::OnGoalScored), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<GoalieSaveData>("GoalieSave", -1)->Add(Function<GoalieSaveData*>(GoalieSaveBinding(MemFun(&ReplayChoreo::OnGoalieSave), this, placeholder0)), 0, -1);
}

void ReplayChoreo::OnGoalScored(GoalScoredData* data)
{
    if (!g_pGame)
    {
        return;
    }

    mGoalScoredData.pScorer = data->pScorer;
    mGoalScoredData.uTeamIndex = data->uTeamIndex;
    mGoalScoredData.uGoalType = data->uGoalType;
    mGoalScoredData.v3ShotPosition = data->v3ShotPosition;
    mCamera.SetSideOfInterest((data->uTeamIndex + 1) % 2);
}

void ReplayChoreo::OnGoalieSave(GoalieSaveData* data)
{
    if (!g_pGame)
    {
        return;
    }

    if (data->isSTS)
    {
        return;
    }

    int side = data->pGoalie->m_pTeam->m_nSide;
    mGoalScoredData.pScorer = data->pShooter;
    mGoalScoredData.uTeamIndex = (side + 1) % 2;
    mGoalScoredData.uGoalType = data->saveType;
    mGoalScoredData.v3ShotPosition = data->pGoalie->GetPosition();
    mCamera.SetSideOfInterest(side);
}

void ReplayChoreo::Reset()
{
    mCamera.m_pFilter[0] = 0;
    mCamera.m_pFilter[1] = 0;
    cCameraManager::Remove(mCamera);
}

// StartScript inlines both zone helpers. Compiling them first, depth before
// width, creates the 0.33/0.66/2.0 literals in retail's .sdata2 order; the
// linker drops the unreferenced out-of-line copies.
int GetZoneDepth(const ReplayChoreo::ReplayShotData& data)
{
    float length;
    float distance;
    int zoneDepth = 0;
    float shotX = data.v3ShotPosition.x;
    float negGoalLineX = -cField::GetGoalLineX((unsigned int)data.uTeamIndex);
    length = (float)fabs(negGoalLineX);
    distance = (float)fabs(negGoalLineX - shotX);
    if (distance < 0.33f * length)
    {
        zoneDepth = 1;
    }
    length = (float)fabs(negGoalLineX);
    distance = (float)fabs(negGoalLineX - shotX);
    if (distance > 0.66f * length)
    {
        zoneDepth = 2;
    }
    return zoneDepth;
}

int GetZoneInWidth(const ReplayChoreo::ReplayShotData& data)
{
    int zoneInWidth = 0;
    float shotY = data.v3ShotPosition.y;
    float fieldDepth = 2.0f * cField::mv3FieldPosition.y;
    float y = 0.5f * fieldDepth + shotY;
    if (y > 0.66f * fieldDepth)
    {
        zoneInWidth = 2;
    }
    else if (y < 0.33f * fieldDepth)
    {
        zoneInWidth = 1;
    }
    return zoneInWidth;
}

void ReplayChoreo::StartScript(const ReplayShotData& data)
{
    if (!mReplay->DidOccurInLastNumSeconds(2, 6.0f))
    {
        strcpy(scriptName, "MID_CENTER_OWN_GOAL_0");
    }
    else
    {
        int zoneInWidth = GetZoneInWidth(data);
        int zoneDepth = GetZoneDepth(data);

        int replayType = data.uGoalType;
        if (replayType == 1 || replayType == 2)
        {
            replayType = 0;
        }

        if (mNumScripts[zoneDepth][zoneInWidth][replayType] == 0
            && mNumScripts[zoneDepth][0][replayType] > 0)
        {
            zoneInWidth = 0;
        }
        if (mNumScripts[zoneDepth][zoneInWidth][replayType] == 0
            && mNumScripts[0][zoneInWidth][replayType] > 0)
        {
            zoneDepth = 0;
        }
        if (mNumScripts[zoneDepth][zoneInWidth][replayType] == 0
            && mNumScripts[0][0][replayType] > 0)
        {
            zoneInWidth = 0;
            zoneDepth = 0;
        }
        if (mNumScripts[zoneDepth][zoneInWidth][replayType] == 0
            && mNumScripts[0][0][0] > 0)
        {
            zoneInWidth = 0;
            zoneDepth = 0;
            replayType = 0;
        }

        int numScripts = mNumScripts[zoneDepth][zoneInWidth][replayType];
        int pick = PickRandom(numScripts);
        if (cameraPick > -1)
        {
            pick = cameraPick % mNumScripts[zoneDepth][zoneInWidth][replayType];
        }

        nlSNPrintf(scriptName, sizeof(scriptName), scriptNameFormat, zoneDepthNames[zoneDepth], zoneInWidthNames[zoneInWidth], replayTypeNames[replayType], pick);
    }

    InterpreterCore::Reset();
    CallFunction(nlStringHash(scriptName));
}

void ReplayChoreo::StartAutoReplay(bool highlight)
{
    mIsHighlightReel = highlight;

    nlVector3 offset = { 0.0f, 0.0f, 0.0f };
    mCamera.SetPositionOffset(offset);
    mCamera.SetPositionDampingTime(positionDampingTime);
    ResetBall(g_pBall, false);

    mReplayManager = ReplayManager::Instance();
    mReplay = mReplayManager->mReplay;

    NetMesh::spNegativeXNetMesh->Reset(true);
    NetMesh::spPositiveXNetMesh->Reset(true);

    if (!cCameraManager::HasCamera(&mCamera))
    {
        mCamera.mNoDampenForOneUpdate = true;
        cCameraManager::PushCamera(&mCamera);

        mRumbleFilter.Reset();
        mNoiseFilter.Reset();
        mCamera.m_pFilter[mRumbleFilter.GetFilterIndex()] = &mRumbleFilter;
        mCamera.m_pFilter[mNoiseFilter.GetFilterIndex()] = &mNoiseFilter;
    }

    StartScript(mGoalScoredData);
}

void ReplayChoreo::LoadNextHighlight()
{
    Highlight* highlight;
    if (mCurrentHighlight == 0)
    {
        mCurrentHighlight = mHighlightList.m_pEnd;
        highlight = mHighlightList.m_pStart;
    }
    else
    {
        highlight = mCurrentHighlight->next;
    }

    for (; highlight != mCurrentHighlight; highlight = highlight->next)
    {
        if (highlight == 0)
        {
            highlight = mHighlightList.m_pStart;
        }
        if (highlight->mSlot != -1)
        {
            mCurrentHighlight = highlight;
            break;
        }
    }

    if (highlight != 0)
    {
        mCurrentHighlight = highlight;
        if (mReplayManager->LoadReplay(highlight->mSlot) == true)
        {
            SetRecordingFrames(mReplayManager->mReplay, mCurrentHighlight->mBegin, mCurrentHighlight->mEnd, mCurrentHighlight->mCurrent);
            mCamera.SetSideOfInterest(mCurrentHighlight->mReplayPad);
            mGoalScoredData = mCurrentHighlight->mGoalScoredData;
        }
    }
}

void ReplayChoreo::Finish()
{
    NetMesh::spNegativeXNetMesh->Reset(false);
    NetMesh::spPositiveXNetMesh->Reset(false);
}

void ReplayChoreo::FlushHighlights()
{
    mCurrentHighlight = 0;
    mHighlights[0].mQuality = -1;
    mHighlights[1].mQuality = -1;
    mHighlights[2].mQuality = -1;
    mHighlightList.m_pEnd = 0;
    mHighlightList.m_pStart = 0;
}

// .sbss follows declaration order: this comes after Instance's guard.
namespace
{
int replayExcitement;
} // namespace

void ReplayChoreo::Update(float deltaT)
{
    if (ReplayManager::Instance()->IsSavingReplay() || ReplayManager::Instance()->IsLoadingReplay())
    {
        return;
    }

    replayExcitement = ReplayManager::Instance()->GetReplayExcitement(-8.0f);
    UpdateCrowdImpostorAnimation(replayExcitement);

    if (nlTaskManager::m_pInstance->mCurrentState == 8)
    {
        if (mRunningFor)
        {
            mRunForTimeLeft -= deltaT;
        }
        if (m_RunState == 3)
        {
            Run();
        }
        mCamera.ManualUpdate(deltaT);
    }
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
        float speed = mReplayManager->mSpeed;
        float currentTime = mReplayManager->mTime;
        float timeRemaining = (1.0f / speed) * (endTime - currentTime);
        if (timeRemaining < param)
        {
            return true;
        }
    }

    return false;
}

void ReplayChoreo::SaveHighlight(int quality)
{
    mReplayManager = ReplayManager::Instance();
    mReplay = mReplayManager->mReplay;

    if (g_e3_Build == true)
    {
        return;
    }

    // One load of the list head feeds both counts and the eviction walk.
    Highlight* first = mHighlightList.m_pStart;
    int slot = -1;
    Highlight* highlight = 0;

    if (CountHighlights(first) < 3U)
    {
        slot = CountHighlights(first);
        for (int i = 0; i < 3; i++)
        {
            if (mHighlights[i].mQuality == -1)
            {
                highlight = &mHighlights[i];
                break;
            }
        }
    }

    if (slot == -1)
    {
        float lowestQuality = 1e10f;
        for (Highlight* h = first; h != 0; h = h->next)
        {
            if ((float)h->mQuality < lowestQuality && h->mQuality < quality)
            {
                highlight = h;
                slot = h->mSlot;
                lowestQuality = (float)h->mQuality;
            }
        }

        if (highlight != 0)
        {
            nlListRemoveElement(&mHighlightList.m_pStart, highlight, &mHighlightList.m_pEnd);
            highlight->mQuality = -1;
            highlight->mSlot = -1;
        }
    }

    if (slot != -1 && highlight != 0)
    {
        if (mReplayManager->SaveReplay(slot) == true)
        {
            highlight->mQuality = quality;
            highlight->mTime = mReplayManager->mTime;
            highlight->mGoalScoredData = mGoalScoredData;
            highlight->mReplayPad = mCamera.mSideOfInterest;
            highlight->mSlot = slot;

            Highlight* current = mHighlightList.m_pStart;
            Highlight* prev = 0;
            if (current == 0)
            {
                nlListAddStart(&mHighlightList.m_pStart, highlight, &mHighlightList.m_pEnd);
            }
            else
            {
                for (; current != 0; prev = current, current = current->next)
                {
                    if (highlight->mQuality < current->mQuality)
                    {
                        if (prev == 0)
                        {
                            nlListAddStart(&mHighlightList.m_pStart, highlight, &mHighlightList.m_pEnd);
                        }
                        else
                        {
                            highlight->next = current;
                            prev->next = highlight;
                        }
                        break;
                    }
                }

                if (current == 0)
                {
                    nlListAddEnd(&mHighlightList.m_pStart, &mHighlightList.m_pEnd, highlight);
                }
            }

            GetRecordingFrames(mReplayManager->mReplay, &highlight->mBegin, &highlight->mEnd, &highlight->mCurrent);
        }
    }
}

int ReplayChoreo::NumHighlights() const
{
    return CountHighlights(mHighlightList.m_pStart);
}

int ReplayChoreo::GetHighlightNumber() const
{
    Highlight* highlight = mHighlightList.m_pStart;
    int index = 1;
    for (; highlight != 0; highlight = highlight->next)
    {
        if (highlight == mCurrentHighlight)
        {
            return NumHighlights() - index;
        }
        index++;
    }
    return -1;
}
