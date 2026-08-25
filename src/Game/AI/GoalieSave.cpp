#include "Game/AI/GoalieSave.h"

#include "Game/Field.h"
#include "NL/nlString.h"

#include <math.h>

struct SaveInfo
{
    int mnAnimID;
    int mnFailAnimID;
    int mnRecoverAnimID;
    unsigned int muSaveType;
    int mConnectedSaveID[4];
    char mszName[16];
};

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

nlListContainer<SaveData*> gSaveGrid[6][4];
static float fDefaultMilestoneValues[2] = { 0.4f, 0.7f };

float GoalieSave::mfCatchAllowDistSq = 0.25f;

SaveBlendInfo::SaveBlendInfo()
{
    mfStartTime = 0.0f;
    for (int milestone = 0; milestone < 5; ++milestone)
    {
        mfMilestoneTime[milestone] = 0.0f;
    }

    for (int anim = 0; anim < 4; ++anim)
    {
        for (int milestone = 0; milestone < 5; ++milestone)
        {
            mfMilestoneScale[anim][milestone] = 1.0f;
        }
        mpSaveData[anim] = 0;
    }

    mfSaveBlendPrimary = 0.0f;
    mfSaveBlendSecondary = 0.0f;
    mfSaveBlendComposite = 0.0f;
    mv3BlendedSavePos = v3Zero;
}

void SaveData::Init(
    Goalie* pGoalie, const SaveInfo& info, unsigned int uIndex)
{
    mnAnimID = info.mnAnimID;
    mnRecoverAnimID = info.mnRecoverAnimID;
    muSaveType = info.muSaveType;
    mfFatigueValue = LookupFatigueValue(pGoalie, info);

    mv3SavePos = v3Zero;

    mfMilestonePercent[0] = 0.0f;
    mfMilestonePercent[1] = 0.0f;
    mfMilestonePercent[2] = 0.0f;
    mfMilestonePercent[3] = 0.0f;
    mfMilestonePercent[4] = 0.0f;

    mv3TakeoffPos = v3Zero;
    mv3GroupMinCoords = v3Zero;
    mv3GroupMaxCoords = v3Zero;

    nlStrNCpy<char>(mszName, info.mszName, 16);
    muIndex = uIndex;
}

SaveData* GoalieSave::GetMissChipSaveData(bool bLeft, bool bFar)
{
    unsigned int farFlag = bFar != 0;
    int index = muMissChipIndexStart + (farFlag ? 0 : 2) + (int)bLeft;
    return &mpSaveTable[index];
}

SaveData* GoalieSave::GetSTSSpinMissData(bool bLeft)
{
    unsigned int index = muSTSMissIndexStart + ((!bLeft) ? 1 : 0);
    return &mpSaveTable[index];
}

bool GoalieSave::TriggerCallback(float fTime, float fDuration,
    unsigned long uEventID, float fIntensity, void* pUserData)
{
    SaveData* pSaveData = (SaveData*)pUserData;

    if ((uEventID + 0x307C0000) == 0xE7CD)
    {
        pSaveData->mfMilestonePercent[2] = fTime;
        pSaveData->mfDuration = fDuration;
    }
    else if ((uEventID - 0x56260000) == 0x4BBE)
    {
        pSaveData->mfMilestonePercent[0] = fTime;
    }
    else if ((uEventID - 0x0F950000) == 0x24BA)
    {
        pSaveData->mfMilestonePercent[1] = fTime;
    }
    else if ((uEventID - 0x04540000) == 0x24B9)
    {
        pSaveData->mfMilestonePercent[3] = fTime;
    }
    return true;
}

static inline void AddPointToGrid(
    SaveData* pSaveData, const nlVector3& v3Point)
{
    float y;
    float z;
    z = v3Point.z;
    y = v3Point.y;

    float netWidth = cField::GetNet(1.0f)->GetNetWidth();
    float netHeight = cField::GetNet(1.0f)->GetNetHeight();

    int i = (int)(6.0f * (0.5f * netWidth + y) / netWidth);
    if (i < 0)
        i = 0;
    else if (i >= 6)
        i = 5;

    int j = (int)(4.0f * z / netHeight);
    if (j < 0)
        j = 0;
    else if (j >= 4)
        j = 3;

    nlListContainer<SaveData*>& cell = gSaveGrid[i][j];

    if (cell.Begin().IsValid())
    {
        nlListIterator<SaveData*> iterator = cell.Begin();
        while (iterator.IsValid())
        {
            if (iterator.Current() == pSaveData)
                return;
            iterator.Next();
        }
    }

    cell.AddEntry(pSaveData);
}

void GoalieSave::AddAreaToGrid(SaveData* pSaveData)
{
    SaveData* const pRoot = pSaveData;
    SaveData* pCur;
    nlVector3 v3TopRight;
    nlVector3 v3BotLeft;
    float yInc;
    float zInc;
    nlVector3 v3CurColPos;
    nlVector3 v3CurRowPos;
    SaveData* pNextRight;
    SaveData* pCurBot;
    SaveData* pRightCorner;
    SaveData* pNextNextRight;
    SaveData* pCurLeft;
    SaveData* pCurRight;
    SaveData* pCurUp;
    SaveData* pCurRightUp;
    SaveData* pClosest;
    float fCloseDist;

    pCur = pRoot;
    while (pCur != 0)
    {
        pCurRightUp = pCur;
        pCur = pCur->mpConnectedSaveData[3];
    }
    while (pCurRightUp != 0)
    {
        pCurRightUp = pCurRightUp->mpConnectedSaveData[0];
    }

    pCur = pRoot;
    while (pCur != 0)
    {
        pCurUp = pCur;
        pCur = pCur->mpConnectedSaveData[2];
    }
    while (pCurUp != 0)
    {
        pCurBot = pCurUp;
        pCurUp = pCurUp->mpConnectedSaveData[0];
    }

    {
        SaveData* end;
        pCur = pRoot;
        while (pCur != 0)
        {
            end = pCur;
            pCur = pCur->mpConnectedSaveData[3];
        }
        while (end != 0)
        {
            pRightCorner = end;
            end = end->mpConnectedSaveData[1];
        }
    }

    {
        SaveData* end;
        pCur = pRoot;
        while (pCur != 0)
        {
            end = pCur;
            pCur = pCur->mpConnectedSaveData[2];
        }
        while (end != 0)
        {
            end = end->mpConnectedSaveData[1];
        }
    }

    yInc = (float)(0.95 * GridSectionWidth());
    zInc = (float)(0.95 * GridSectionHeight());

    pCur = pCurBot;
    v3TopRight = pCurBot->mv3SavePos;
    while (pCur != 0)
    {
        if (pCur->mv3SavePos.y > v3TopRight.y)
            v3TopRight.y = pCur->mv3SavePos.y;
        pCur = pCur->mpConnectedSaveData[1];
    }
    while (pCurBot != 0)
    {
        if (pCurBot->mv3SavePos.z > v3TopRight.z)
            v3TopRight.z = pCurBot->mv3SavePos.z;
        pCurBot = pCurBot->mpConnectedSaveData[3];
    }

    pRoot->mv3GroupMaxCoords = v3TopRight;
    float halfYInc = 0.51f * yInc;
    float halfZInc = 0.51f * zInc;
    v3TopRight.y += halfYInc;
    v3TopRight.z += halfZInc;

    pCur = pRightCorner;
    v3BotLeft = pRightCorner->mv3SavePos;
    while (pCur != 0)
    {
        if (pCur->mv3SavePos.y < v3BotLeft.y)
            v3BotLeft.y = pCur->mv3SavePos.y;
        pCur = pCur->mpConnectedSaveData[0];
    }
    pCur = pRightCorner;
    while (pCur != 0)
    {
        if (pCur->mv3SavePos.z < v3BotLeft.z)
            v3BotLeft.z = pCur->mv3SavePos.z;
        pCur = pCur->mpConnectedSaveData[2];
    }

    pRoot->mv3GroupMinCoords = v3BotLeft;
    v3BotLeft.y -= halfYInc;
    v3BotLeft.z -= halfZInc;

    pNextRight = pRightCorner;
    pCurBot = pNextRight;
    pNextNextRight = pNextRight->mpConnectedSaveData[2];
    v3CurRowPos = v3BotLeft;

    while (v3CurRowPos.y < v3TopRight.y)
    {
        if (v3CurRowPos.y >= pNextRight->mv3SavePos.y
            && pNextNextRight != 0)
        {
            pCurBot = pNextRight;
            pNextRight = pNextNextRight;
            pNextNextRight = pNextNextRight->mpConnectedSaveData[2];
        }
        pCurLeft = pCurBot;
        pCurRight = pNextRight;
        v3CurColPos = v3CurRowPos;

        while (v3CurColPos.z < v3TopRight.z)
        {
            FindVerticalBoundingPoints(
                pCurLeft, v3CurColPos, &pCurLeft, &pCurUp);
            FindVerticalBoundingPoints(
                pCurRight, v3CurColPos, &pCurRight, &pCurRightUp);

            {
                float dy = pCurLeft->mv3SavePos.y - v3CurColPos.y;
                float dz = pCurLeft->mv3SavePos.z - v3CurColPos.z;
                fCloseDist = nlGetLengthSquared2D(dy, dz);
                pClosest = pCurLeft;

                if (pCurLeft != pCurUp)
                {
                    float upDy = pCurUp->mv3SavePos.y - v3CurColPos.y;
                    float upDz = pCurUp->mv3SavePos.z - v3CurColPos.z;
                    float d = nlGetLengthSquared2D(upDy, upDz);
                    if (d < fCloseDist)
                    {
                        fCloseDist = d;
                        pClosest = pCurUp;
                    }
                }

                if (pCurLeft != pCurRight)
                {
                    float rightDy = pCurRight->mv3SavePos.y - v3CurColPos.y;
                    float rightDz = pCurRight->mv3SavePos.z - v3CurColPos.z;
                    float d = nlGetLengthSquared2D(rightDy, rightDz);
                    if (d < fCloseDist)
                    {
                        fCloseDist = d;
                        pClosest = pCurRight;
                    }
                    if (pCurRight != pCurRightUp)
                    {
                        float upRightDy = pCurRightUp->mv3SavePos.y
                                        - v3CurColPos.y;
                        float upRightDz = pCurRightUp->mv3SavePos.z
                                        - v3CurColPos.z;
                        float fUpRightDistSq = nlGetLengthSquared2D(upRightDy, upRightDz);
                        if (fUpRightDistSq < fCloseDist)
                        {
                            fCloseDist = fUpRightDistSq;
                            pClosest = pCurRightUp;
                        }
                    }
                }
            }

            pClosest->mv3GroupMinCoords = pRoot->mv3GroupMinCoords;
            pClosest->mv3GroupMaxCoords = pRoot->mv3GroupMaxCoords;
            AddPointToGrid(pClosest, v3CurColPos);
            v3CurColPos.z += zInc;
        }
        v3CurRowPos.y += yInc;
    }
}

static inline void Local2GridCoords(
    float y, float z, int& i, int& j)
{
    float netWidth = cField::GetNet(1.0f)->GetNetWidth();
    float netHeight = cField::GetNet(1.0f)->GetNetHeight();
    i = (int)(6.0f * (0.5f * netWidth + y) / netWidth);
    if (i < 0)
        i = 0;
    else if (i >= 6)
        i = 5;
    j = (int)(4.0f * z / netHeight);
    if (j < 0)
        j = 0;
    else if (j >= 4)
        j = 3;
}

void GoalieSave::AddSegmentToGrid(
    SaveData* pSaveData1, SaveData* pSaveData2)
{
    int divisions;
    int count;
    int i, j, m, n;
    nlVector3 v3Delta;
    nlVector3 v3CurPos;
    SaveData* pCurSaveData;

    Local2GridCoords(
        pSaveData1->mv3SavePos.y, pSaveData1->mv3SavePos.z, i, j);
    Local2GridCoords(
        pSaveData2->mv3SavePos.y, pSaveData2->mv3SavePos.z, m, n);
    divisions = abs(j - n) + abs(i - m);
    nlVec3Sub(v3Delta, pSaveData2->mv3SavePos, pSaveData1->mv3SavePos);
    if (divisions > 0)
    {
        nlVec3Scale(v3Delta, v3Delta, 1.0f / (float)divisions);
    }
    v3CurPos = pSaveData1->mv3SavePos;
    for (count = 0; count <= divisions; count++)
    {
        if (nlGetLengthSquared2D(
                pSaveData1->mv3SavePos.y - v3CurPos.y,
                pSaveData1->mv3SavePos.z - v3CurPos.z)
            < nlGetLengthSquared2D(
                pSaveData2->mv3SavePos.y - v3CurPos.y,
                pSaveData2->mv3SavePos.z - v3CurPos.z))
            pCurSaveData = pSaveData1;
        else
            pCurSaveData = pSaveData2;
        AddPointToGrid(pCurSaveData, v3CurPos);
        nlVec3Add(v3CurPos, v3CurPos, v3Delta);
    }
}

void GoalieSave::AddChainToGrid(SaveData* pSaveData, bool bVertical)
{
    SaveData* pEnd;
    SaveData* p;
    int dir;
    int oppdir;

    if (bVertical)
    {
        dir = 0;
        oppdir = 1;
    }
    else
    {
        dir = 2;
        oppdir = 3;
    }

    p = pSaveData;
    while (p != 0)
    {
        pEnd = p;
        p = p->mpConnectedSaveData[oppdir];
    }

    pSaveData->mv3GroupMaxCoords = pSaveData->mv3SavePos;
    pSaveData->mv3GroupMinCoords = pSaveData->mv3SavePos;

    SaveData* pCur = pEnd;
    SaveData* pLast;

    while (pCur != 0)
    {
        if (pCur->mv3SavePos.x > pSaveData->mv3GroupMaxCoords.x)
            pSaveData->mv3GroupMaxCoords.x = pCur->mv3SavePos.x;
        if (pCur->mv3SavePos.y > pSaveData->mv3GroupMaxCoords.y)
            pSaveData->mv3GroupMaxCoords.y = pCur->mv3SavePos.y;
        if (pCur->mv3SavePos.z > pSaveData->mv3GroupMaxCoords.z)
            pSaveData->mv3GroupMaxCoords.z = pCur->mv3SavePos.z;

        if (pCur->mv3SavePos.x < pSaveData->mv3GroupMinCoords.x)
            pSaveData->mv3GroupMinCoords.x = pCur->mv3SavePos.x;
        if (pCur->mv3SavePos.y < pSaveData->mv3GroupMinCoords.y)
            pSaveData->mv3GroupMinCoords.y = pCur->mv3SavePos.y;
        if (pCur->mv3SavePos.z < pSaveData->mv3GroupMinCoords.z)
            pSaveData->mv3GroupMinCoords.z = pCur->mv3SavePos.z;

        pLast = pCur;
        pCur = pCur->mpConnectedSaveData[dir];
        if (pCur != 0)
        {
            AddSegmentToGrid(pLast, pCur);
        }
    }

    if (pLast == 0)
    {
        return;
    }

    while (pLast != 0)
    {
        pLast->mv3GroupMaxCoords = pSaveData->mv3GroupMaxCoords;
        pLast->mv3GroupMinCoords = pSaveData->mv3GroupMinCoords;
        pLast = pLast->mpConnectedSaveData[oppdir];
    }
}

void GoalieSave::AddToGrid(SaveData* pSaveData)
{
    if (pSaveData->mpConnectedSaveData[1] != 0
        || pSaveData->mpConnectedSaveData[0] != 0)
    {
        if (pSaveData->mpConnectedSaveData[2] != 0
            || pSaveData->mpConnectedSaveData[3] != 0)
        {
            AddAreaToGrid(pSaveData);
            return;
        }
        AddChainToGrid(pSaveData, true);
        return;
    }

    if (pSaveData->mpConnectedSaveData[2] != 0
        || pSaveData->mpConnectedSaveData[3] != 0)
    {
        AddChainToGrid(pSaveData, false);
        return;
    }

    AddPointToGrid(pSaveData, pSaveData->mv3SavePos);
    pSaveData->mv3GroupMinCoords = pSaveData->mv3SavePos;
    pSaveData->mv3GroupMaxCoords = pSaveData->mv3SavePos;
}

void GoalieSave::FindVerticalBoundingPoints(SaveData* pSaveData,
    const nlVector3& v3TargetPoint, SaveData** pLoPoint,
    SaveData** pHiPoint)
{
    SaveData* pHiSaveData = pSaveData;
    SaveData* pLoSaveData = pSaveData;

    while (pHiSaveData != 0
           && v3TargetPoint.z > pHiSaveData->mv3SavePos.z)
    {
        pLoSaveData = pHiSaveData;
        pHiSaveData = pHiSaveData->mpConnectedSaveData[0];
    }
    while (pLoSaveData != 0
           && v3TargetPoint.z < pLoSaveData->mv3SavePos.z)
    {
        pHiSaveData = pLoSaveData;
        pLoSaveData = pLoSaveData->mpConnectedSaveData[1];
    }
    if (pLoSaveData == 0)
    {
        *pLoPoint = pHiSaveData;
        *pHiPoint = pHiSaveData;
    }
    else if (pHiSaveData == 0)
    {
        *pLoPoint = pLoSaveData;
        *pHiPoint = pLoSaveData;
    }
    else
    {
        *pHiPoint = pHiSaveData;
        *pLoPoint = pLoSaveData;
    }
}

float GoalieSave::GridSectionWidth()
{
    return cField::GetNet(1.0f)->GetNetWidth() / 6.0f;
}

float GoalieSave::GridSectionHeight()
{
    return cField::GetNet(1.0f)->GetNetHeight() / 4.0f;
}
