#include "NL/nlDLListContainer.inl"
#include "NL/nlAVLTree.h"
#include "NL/nlDLListContainer.h"
#include "Game/AI/GoalieSave.h"

#include "Game/AI/AiUtil.h"
#include "Game/AnimInventory.h"
#include "Game/CharacterTriggers.h"
#include "Game/CharacterTweaks.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Field.h"
#include "Game/Goalie.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/nlString.h"

#include <NMWException.h>
#include <math.h>
#include "NL/nlstring_tmpl.h"

struct SaveInfo
{
    int mnAnimID;
    int mnFailAnimID;
    int mnRecoverAnimID;
    unsigned int muSaveType;
    int mConnectedSaveID[4];
    char mszName[16];
};

int gPositionAnimID[6] = { 23, 26, 18, 21, 32, 33 };
SaveInfo gSaveInfo[89] = {
    { 117, -1, -1, 0x00080000, { -1, 116, 119, -1 }, "Ctch Lob CtrJmp" },
    { 116, -1, -1, 0x00080000, { 117, -1, 118, -1 }, "Catch Lob Ctr" },
    { 119, -1, -1, 0x00080000, { -1, 118, -1, 117 }, "Catch Lob R Jmp" },
    { 118, -1, -1, 0x00080000, { 119, -1, -1, 116 }, "Catch Lob R" },
    { 121, -1, -1, 0x00080000, { -1, 120, 123, -1 }, "Ctch LobCtrJmpL" },
    { 120, -1, -1, 0x00080000, { 121, -1, 122, -1 }, "Catch Lob Ctr L" },
    { 123, -1, -1, 0x00080000, { -1, 122, -1, 121 }, "Catch Lob L Jmp" },
    { 122, -1, -1, 0x00080000, { 123, -1, -1, 120 }, "Catch Lob L" },
    { 45, 91, -1, 0x00000001, { -1, 46, 55, -1 }, "Ctch Ctr XHJmpR" },
    { 46, 92, -1, 0x00000001, { 45, 47, 56, -1 }, "Ctch Ctr HiJmpR" },
    { 47, 93, -1, 0x00000001, { 46, 48, 57, -1 }, "Catch Ctr Hi R" },
    { 48, 94, -1, 0x00000001, { 47, 49, 58, -1 }, "Catch Ctr MedR" },
    { 49, 95, -1, 0x00000001, { 48, -1, 59, -1 }, "Catch Ctr Lo R" },
    { 55, 101, -1, 0x00000001, { -1, 56, -1, 45 }, "Catch Rt XHiJmp" },
    { 56, 102, -1, 0x00000001, { 55, 57, -1, 46 }, "Catch Rt Hi Jmp" },
    { 57, 103, -1, 0x00000001, { 56, 58, -1, 47 }, "Catch Rt Hi" },
    { 58, 104, -1, 0x00000001, { 57, 59, -1, 48 }, "Catch Rt Med" },
    { 59, 105, -1, 0x00000001, { 58, -1, -1, 49 }, "Catch Rt Lo" },
    { 50, 96, -1, 0x00000001, { -1, 51, -1, 60 }, "Ctch Ctr XHJmpL" },
    { 51, 97, -1, 0x00000001, { 50, 52, -1, 61 }, "Ctch Ctr HiJmpL" },
    { 52, 98, -1, 0x00000001, { 51, 53, -1, 62 }, "Catch Ctr Hi L" },
    { 53, 99, -1, 0x00000001, { 52, 54, -1, 63 }, "Catch Ctr MedL" },
    { 54, 100, -1, 0x00000001, { 53, -1, -1, 64 }, "Catch Ctr Lo L" },
    { 60, 106, -1, 0x00000001, { -1, 61, 50, -1 }, "Catch Lf XHiJmp" },
    { 61, 107, -1, 0x00000001, { 60, 62, 51, -1 }, "Catch Lft HiJmp" },
    { 62, 108, -1, 0x00000001, { 61, 63, 52, -1 }, "Catch Lft Hi" },
    { 63, 109, -1, 0x00000001, { 62, 64, 53, -1 }, "Catch Lft Med" },
    { 64, 109, -1, 0x00000001, { 63, -1, 54, -1 }, "Catch Lft Lo" },
    { 114, 82, -1, 0x00000002, { -1, -1, -1, -1 }, "DiveCatchRHiSpc" },
    { 115, 88, -1, 0x00000002, { -1, -1, -1, -1 }, "DiveCatchLHiSpc" },
    { 65, 79, 141, 0x00000002, { -1, 66, 71, -1 }, "Dive Catch R Hi" },
    { 66, 78, 141, 0x00000002, { 65, 67, 72, -1 }, "Dive Catch R Md" },
    { 67, 77, 141, 0x00000002, { 66, -1, 73, -1 }, "Dive Catch R Lo" },
    { 68, 85, 142, 0x00000002, { -1, 69, -1, 74 }, "Dive Catch L Hi" },
    { 69, 84, 142, 0x00000002, { 68, 70, -1, 75 }, "Dive Catch L Md" },
    { 70, 83, 142, 0x00000002, { 69, -1, -1, 76 }, "Dive Catch L Lo" },
    { 71, 82, 141, 0x00000002, { -1, 72, -1, 65 }, "DiveCtch RHiFar" },
    { 72, 81, 141, 0x00000002, { 71, 73, -1, 66 }, "DiveCtch RMdFar" },
    { 73, 80, 141, 0x00000002, { 72, -1, -1, 67 }, "DiveCtch RLoFar" },
    { 74, 88, 142, 0x00000002, { -1, 75, 68, -1 }, "DiveCtch LHiFar" },
    { 75, 87, 142, 0x00000002, { 74, 76, 69, -1 }, "DiveCtch LMdFar" },
    { 76, 86, 142, 0x00000002, { 75, -1, 70, -1 }, "DiveCtch LLoFar" },
    { 91, -1, -1, 0x00000004, { -1, 92, 101, -1 }, "Dfl Ctr XHiJmpR" },
    { 92, -1, -1, 0x00000004, { 91, 93, 102, -1 }, "Defl Ctr HiJmpR" },
    { 93, -1, -1, 0x00000004, { 92, 94, 103, -1 }, "Defl Ctr Hi R" },
    { 94, -1, -1, 0x00000004, { 93, 95, 104, -1 }, "Defl Ctr Med R" },
    { 95, -1, -1, 0x00000004, { 94, -1, 105, -1 }, "Defl Ctr Lo R" },
    { 101, -1, -1, 0x00000004, { -1, 102, -1, 91 }, "Defl R XHi Jmp" },
    { 102, -1, -1, 0x00000004, { 101, 103, -1, 92 }, "Defl R Hi Jmp" },
    { 103, -1, -1, 0x00000004, { 102, 104, -1, 93 }, "Deflect R Hi" },
    { 104, -1, -1, 0x00000004, { 103, 105, -1, 94 }, "Deflect R Med" },
    { 105, -1, -1, 0x00000004, { 104, -1, -1, 95 }, "Deflect R Lo" },
    { 96, -1, -1, 0x00000004, { -1, 97, -1, 106 }, "Dfl Ctr XHiJmpL" },
    { 97, -1, -1, 0x00000004, { 96, 98, -1, 107 }, "Defl Ctr HiJmpL" },
    { 98, -1, -1, 0x00000004, { 97, 99, -1, 108 }, "Defl Ctr Hi L" },
    { 99, -1, -1, 0x00000004, { 98, 100, -1, 109 }, "Defl Ctr Med L" },
    { 100, -1, -1, 0x00000004, { 99, -1, -1, 110 }, "Defl Ctr Lo L" },
    { 106, -1, -1, 0x00000004, { -1, 107, 96, -1 }, "Defl L XHi Jmp" },
    { 107, -1, -1, 0x00000004, { 106, 108, 97, -1 }, "Defl L Hi Jmp" },
    { 108, -1, -1, 0x00000004, { 107, 109, 98, -1 }, "Deflect L Hi" },
    { 109, -1, -1, 0x00000004, { 108, 110, 99, -1 }, "Deflect L Med" },
    { 110, -1, -1, 0x00000004, { 109, -1, 100, -1 }, "Deflect L Lo" },
    { 79, -1, 143, 0x00000008, { -1, 78, 82, -1 }, "Dive Dfl R Hi" },
    { 78, -1, 143, 0x00000008, { 79, 77, 81, -1 }, "Dive Dfl R Med" },
    { 77, -1, 143, 0x00000008, { 78, -1, 80, -1 }, "Dive Dfl R Lo" },
    { 82, -1, 143, 0x00000008, { -1, 81, -1, 79 }, "DiveDfl R HiFar" },
    { 81, -1, 143, 0x00000008, { 82, 80, -1, 78 }, "DiveDfl R MdFar" },
    { 80, -1, 143, 0x00000008, { 81, -1, -1, 77 }, "DiveDfl R LoFar" },
    { 85, -1, 144, 0x00000008, { -1, 84, -1, 88 }, "Dive Dfl L Hi" },
    { 84, -1, 144, 0x00000008, { 85, 83, -1, 87 }, "Dive Dfl L Med" },
    { 83, -1, 144, 0x00000008, { 84, -1, -1, 86 }, "Dive Dfl L Lo" },
    { 88, -1, 144, 0x00000008, { -1, 87, 85, -1 }, "DiveDfl L HiFar" },
    { 87, -1, 144, 0x00000008, { 88, 86, 84, -1 }, "DiveDfl L MdFar" },
    { 86, -1, 144, 0x00000008, { 87, -1, 83, -1 }, "DiveDfl L LoFar" },
    { 89, -1, -1, 0x00000020, { -1, -1, -1, -1 }, "Leg R Lo" },
    { 90, -1, -1, 0x00000020, { -1, -1, -1, -1 }, "Leg L Lo" },
    { 111, -1, -1, 0x00000010, { -1, -1, 112, 113 }, "Punch Hi" },
    { 112, -1, -1, 0x00000010, { -1, -1, -1, 111 }, "Punch R Hi" },
    { 113, -1, -1, 0x00000010, { -1, -1, 111, -1 }, "Punch L Hi" },
    { 137, -1, 143, 0x00100000, { -1, -1, -1, -1 }, "Miss Chip R" },
    { 138, -1, 144, 0x00100000, { -1, -1, -1, -1 }, "Miss Chip L" },
    { 139, -1, 143, 0x00100000, { -1, -1, -1, -1 }, "MissChipShort R" },
    { 140, -1, 144, 0x00100000, { -1, -1, -1, -1 }, "MissChipShort L" },
    { 168, -1, -1, 0x00200000, { -1, -1, -1, -1 }, "SkillShot R" },
    { 169, -1, -1, 0x00200000, { -1, -1, -1, -1 }, "SkillShot L" },
    { 176, -1, -1, 0x00200000, { -1, -1, -1, -1 }, "Whiteball R" },
    { 177, -1, -1, 0x00200000, { -1, -1, -1, -1 }, "Whiteball L" },
    { 134, -1, -1, 0x00000001, { -1, -1, -1, -1 }, "Grab Above" },
    { -1, 0, 0, 0x00000000, { -1, -1, -1, -1 }, "Empty" },
};

unsigned short gSaveBlendInfoType = 0xFFFF;

float GoalieSave::mfCatchAllowDistSq = 0.25f;

SaveData* GoalieSave::mpSaveTable;
unsigned char GoalieSave::mbInitialized;
unsigned int GoalieSave::muNumSaveEntries;
SavePositionData* GoalieSave::mpPositionTable;
unsigned int GoalieSave::muNumPositionEntries;
unsigned int GoalieSave::muLobCatchIndexStart;
unsigned int GoalieSave::muLobCatchCount;
unsigned int GoalieSave::muMissChipIndexStart;
unsigned int GoalieSave::muMissChipCount;
unsigned int GoalieSave::muSTSMissIndexStart;
unsigned int GoalieSave::muSTSMissCount;
float GoalieSave::mfCrouchDuration;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

static nlAVLTree<int, SaveData*, DefaultKeyCompare<int> > gSaveMap;
nlListContainer<SaveData*> gSaveGrid[6][4];
nlVector3 gLobJumpSavePos;
static float fDefaultMilestoneValues[2] = { 0.4f, 0.7f };

void SavePositionData::Init(Goalie* pGoalie, int animID)
{
    mnAnimID = animID;

    cPN_SAnimController* pController = new cPN_SAnimController(
        pGoalie->GetAnimInventory()->GetAnim(animID), 0, PM_HOLD, 0, 0,
        pGoalie->GetAnimInventory()->GetMirrored(animID));

    pController->SetTime(1.0f);

    nlVector3 v3RootTrans;
    pController->GetRootTrans(&v3RootTrans, 0);

    mfAnimDistance = v3RootTrans.y;
    mfAnimTime = (float)pController->m_pSAnim->m_nNumKeys / 30.0f;
    mfAnimVelocity = mfAnimDistance / mfAnimTime;

    delete pController;
}

void SaveBlendInfo::Reset()
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

float SaveData::LookupFatigueValue(Goalie* pGoalie, const SaveInfo& info)
{
    GoalieTweaks* pTweaks = (GoalieTweaks*)pGoalie->m_pTweaks;
    if (info.muSaveType & 0x1)
    {
        return pTweaks->fShotFatigueStandCatch;
    }
    else if (info.muSaveType & 0x2)
    {
        return pTweaks->fShotFatigueDiveCatch;
    }
    else if (info.muSaveType & 0x4)
    {
        return pTweaks->fShotFatigueStandDeflect;
    }
    else if (info.muSaveType & 0x8)
    {
        return pTweaks->fShotFatigueDiveDeflect;
    }
    else if (info.muSaveType & 0x10)
    {
        return pTweaks->fShotFatigueStandPunch;
    }
    else if (info.muSaveType & 0x20)
    {
        return pTweaks->fShotFatigueLegSave;
    }

    return pTweaks->fShotFatigueDefault;
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

inline void SaveData::PostInit(const SaveInfo& info)
{
    mpFailAnimData = GoalieSave::FindSaveData(info.mnFailAnimID);

    for (int i = 0; i < 4; i++)
    {
        mpConnectedSaveData[i] =
            GoalieSave::FindSaveData(info.mConnectedSaveID[i]);
    }
}

void GoalieSave::ClearData()
{
    if (!mbInitialized)
        return;

    gSaveMap.Clear();
    ClearGrid();

    if (mpSaveTable != 0)
    {
        delete[] ((u8*)mpSaveTable - 0x10);
    }

    if (mpPositionTable != 0)
    {
        delete[] ((u8*)mpPositionTable - 0x10);
    }

    mbInitialized = 0;
}

void GoalieSave::InitData(Goalie* pGoalie)
{
    if (mbInitialized)
    {
        return;
    }

    muNumSaveEntries = 0x58;
    mpSaveTable = (SaveData*)__construct_new_array(
        nlMalloc(0x2C10, 8, false), 0, 0, 0x80, 0x58);

    muLobCatchIndexStart = 0;
    muLobCatchCount = 0;
    gLobJumpSavePos = v3Zero;
    muMissChipIndexStart = 0;
    muMissChipCount = 0;
    muSTSMissIndexStart = 0;
    muSTSMissCount = 0;

    mfCrouchDuration =
        (float)pGoalie->GetAnimInventory()->GetAnim(0x2C)->m_nNumKeys
        / 30.0f;

    for (unsigned int i = 0; i < muNumSaveEntries; i++)
    {
        mpSaveTable[i].Init(pGoalie, gSaveInfo[i], i);

        if (mpSaveTable[i].muSaveType & 0x00100000)
        {
            muMissChipCount++;
            if (muMissChipCount == 1)
            {
                muMissChipIndexStart = i;
            }
        }
        else if (mpSaveTable[i].muSaveType & 0x00200000)
        {
            muSTSMissCount++;
            if (muSTSMissCount == 1)
            {
                muSTSMissIndexStart = i;
            }
        }
        else if (mpSaveTable[i].muSaveType & 0x00080000)
        {
            muLobCatchCount++;
            if (muLobCatchCount == 1)
            {
                muLobCatchIndexStart = i;
            }
        }

        int animKey;
        SaveData* pValue;
        pValue = &mpSaveTable[i];
        animKey = pValue->mnAnimID;
        gSaveMap.Add(animKey, pValue);
    }

    for (unsigned int i = 0; i < muNumSaveEntries; i++)
    {
        mpSaveTable[i].PostInit(gSaveInfo[i]);
    }

    muNumPositionEntries = 6;
    mpPositionTable = (SavePositionData*)__construct_new_array(
        nlMalloc(0x70, 8, false), 0, 0, 0x10, 6);

    for (unsigned int count = 0; count < muNumPositionEntries; count++)
    {
        mpPositionTable[count].Init(pGoalie, gPositionAnimID[count]);
    }

    ClearGrid();

    int nBallJointIndex = pGoalie->GetBallJointIndex();

    for (unsigned int i = 0; i < muNumSaveEntries; i++)
    {
        SaveData* pSaveData = &mpSaveTable[i];
        GetAnimTriggerInfo(
            pGoalie, pSaveData->mnAnimID, TriggerCallback, pSaveData);
        pSaveData->mfMilestonePercent[4] = 1.0f;
        float fGoalTime = pSaveData->mfMilestonePercent[2];
        pGoalie->GetJointPositionFuture(&pSaveData->mv3SavePos,
            pSaveData->mnAnimID, nBallJointIndex,
            fGoalTime, true, true, false, true);
        if (pSaveData->mfMilestonePercent[1] > 0.0f)
        {
            pGoalie->GetJointPositionFuture(&pSaveData->mv3TakeoffPos,
                pSaveData->mnAnimID, -1,
                pSaveData->mfMilestonePercent[1], true, true, false, true);
        }
    }

    int nCount = (int)muNumSaveEntries - 1;
    while (nCount >= 0)
    {
        SaveData* pSaveData = &mpSaveTable[nCount];
        if ((pSaveData->muSaveType & 0xFFFF) != 0)
        {
            AddToGrid(pSaveData);
        }
        nCount--;
    }

    SaveData* pLobJumpSide = &mpSaveTable[muLobCatchIndexStart + 2];
    SaveData* pLobJump = &mpSaveTable[muLobCatchIndexStart];
    gLobJumpSavePos.x = 0.5f * pLobJump->mv3SavePos.x
        + 0.5f * pLobJumpSide->mv3SavePos.x;
    gLobJumpSavePos.y = 0.5f * pLobJump->mv3SavePos.y
        + 0.5f * pLobJumpSide->mv3SavePos.y;
    gLobJumpSavePos.z = 0.5f * pLobJump->mv3SavePos.z
        + 0.5f * pLobJumpSide->mv3SavePos.z;

    mbInitialized = 1;
}

struct MyMiniData
{
    int dist;
    nlListContainer<SaveData*>* list;
};

static inline void InsertSorted(
    nlDLListContainer<MyMiniData*>& list, MyMiniData* data)
{
    nlDLListIterator<MyMiniData*> iterator;
    iterator = list.Begin();
    while (iterator.hasNext())
    {
        if ((*iterator)->dist > data->dist)
        {
            if (iterator.IsStart())
            {
                list.AddStart(data);
                return;
            }

            iterator.Retreat();
            list.AddAfter(iterator, data);
            return;
        }

        iterator.Step();
    }

    list.AddEnd(data);
}

SaveData* GoalieSave::FindBestSave(SaveBlendInfo& blendInfo,
    const nlVector3& v3LocalPos, const nlVector3& v3LocalVelocity,
    float fTime, bool bDoNearSearch, unsigned int uSaveType,
    bool bFromTakeoff)
{
    int i;
    int j;
    SaveData* pSaveData;
    MyMiniData griddata[6][4];
    int across;
    int up;

    float y;
    float z;
    z = v3LocalPos.z;
    y = v3LocalPos.y;

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

    pSaveData = GoalieSave::FindBestInList(blendInfo, gSaveGrid[i][j],
        v3LocalPos, v3LocalVelocity, fTime, uSaveType, bFromTakeoff);

    if (bDoNearSearch && pSaveData == 0)
    {
        nlDLListContainer<MyMiniData*> mylist;

        for (across = 0; across < 6; across++)
        {
            MyMiniData* gridRow = griddata[across];
            nlListContainer<SaveData*>* saveRow = gSaveGrid[across];
            for (up = 0; up < 4; up++)
            {
                int du = j - up;
                int dz = i - across;
                int testDist = dz * dz + du * du;

                if (testDist <= 8)
                {
                    gridRow[up].dist = testDist;
                    gridRow[up].list = &saveRow[up];
                    InsertSorted(mylist, &gridRow[up]);
                }
            }
        }

        nlDLListIterator<MyMiniData*> iterator;
        iterator = mylist.Begin();
        iterator.next();

        while (iterator.hasNext())
        {
            DLListEntry<MyMiniData*>* current = iterator.CurrentEntry();
            MyMiniData* data = current->entry;
            nlListContainer<SaveData*>* cellList = data->list;

            if (cellList != 0)
            {
                pSaveData = GoalieSave::FindBestInList(blendInfo,
                    *cellList, v3LocalPos, v3LocalVelocity, fTime,
                    uSaveType, bFromTakeoff);

                if (pSaveData != 0)
                    break;
            }
            iterator.next();
        }
    }

    return pSaveData;
}

void CalcMilestoneScales(SaveBlendInfo& blendInfo);

SaveData* GetBlendedLobSave(
    SaveBlendInfo& blendInfo, const nlVector3& v3TargetPos)
{
    SaveData* pSaveData;
    if (v3TargetPos.y > 0.0f)
    {
        pSaveData = &GoalieSave::mpSaveTable[GoalieSave::muLobCatchIndexStart];
    }
    else
    {
        pSaveData = &GoalieSave::mpSaveTable[GoalieSave::muLobCatchIndexStart + 4];
    }

    SaveData* pClosest = GoalieSave::GetClosestBlendedPos(
        blendInfo, v3TargetPos, pSaveData);
    if (pClosest != 0)
    {
        CalcMilestoneScales(blendInfo);
    }
    return pClosest;
}

SaveData* GetBlendedSave(SaveData* pSaveData,
    SaveBlendInfo& blendInfo, const nlVector3& v3TargetPos)
{
    SaveData* pClosest = GoalieSave::GetClosestBlendedPos(
        blendInfo, v3TargetPos, pSaveData);
    if (pClosest != 0)
    {
        CalcMilestoneScales(blendInfo);
    }
    return pClosest;
}

SaveData* GoalieSave::FindBestInList(SaveBlendInfo& blendInfo,
    nlListContainer<SaveData*>& SaveList, const nlVector3& v3LocalPos,
    const nlVector3& v3LocalVelocity, float fTime,
    unsigned int uSaveType, bool bFromTakeoff)
{
    float fAbsVelocityX;
    float fClosest = 10000.0f;
    nlVector3 v3AdjLocalPos;
    float fSaveTime;
    SaveData* pConnected;
    float fThisTime;
    float fTimeDelta;
    float fTimeOffset;
    float fTimeOffsetSq;

    int milestone = (bFromTakeoff != 0) ? 1 : 0;
    SaveData* pClosest = 0;
    nlListIterator<SaveData*> iterator = SaveList.Begin();

    while (iterator.IsValid())
    {
        SaveData* pCur = iterator.Current();

        if (uSaveType & pCur->muSaveType)
        {
            SaveBlendInfo candidateBlendInfo;
            candidateBlendInfo.Reset();
            fSaveTime = pCur->mfDuration * pCur->mfMilestonePercent[2];
            {
                float fMilestoneVal = pCur->mfMilestonePercent[milestone];

                if (fMilestoneVal > 0.0f)
                {
                    float fMilDur = pCur->mfDuration * fMilestoneVal;
                    fSaveTime = fSaveTime - fMilDur;
                    if (bFromTakeoff)
                        nlVec3Add(v3AdjLocalPos, v3LocalPos,
                            pCur->mv3TakeoffPos);
                    else
                        v3AdjLocalPos = v3LocalPos;
                }
                else
                {
                    float fScale =
                        1.0f - fDefaultMilestoneValues[milestone];
                    v3AdjLocalPos = v3LocalPos;
                    fSaveTime = fSaveTime * fScale;
                }
            }

            fThisTime = fTime;
            fTimeOffset = 0.0f;
            fAbsVelocityX = (float)fabs(v3LocalVelocity.x);
            if (fAbsVelocityX > 0.001f)
            {
                fTimeOffset = (pCur->mv3SavePos.x - v3AdjLocalPos.x)
                    / v3LocalVelocity.x;
                fThisTime += fTimeOffset;
            }

            if (fSaveTime <= fThisTime)
            {
                {
                    fTimeDelta = fThisTime - fTime;
                    fTimeOffsetSq = fTimeOffset * fTimeOffset;
                    nlVec3ScaleAdd(v3AdjLocalPos, fTimeDelta,
                        v3LocalVelocity, v3AdjLocalPos);
                    v3AdjLocalPos.z -= 10.0f * fTimeOffsetSq;
                }
                if (v3AdjLocalPos.z < 0.18f)
                    v3AdjLocalPos.z = 0.18f;

                pConnected = pCur;
                pCur = GoalieSave::GetClosestBlendedPos(
                    candidateBlendInfo, v3AdjLocalPos, pCur);

                fSaveTime = candidateBlendInfo.mfMilestoneTime[2];
                fThisTime = fTime;
                if (fAbsVelocityX > 0.001f)
                {
                    fThisTime +=
                        (candidateBlendInfo.mv3BlendedSavePos.x
                            - v3LocalPos.x)
                        / v3LocalVelocity.x;
                }

                {
                    nlVec3ScaleAdd(v3AdjLocalPos, fThisTime - fTime,
                        v3LocalVelocity, v3LocalPos);
                    fTimeOffsetSq = (fThisTime - fTime)
                        * (fThisTime - fTime);
                    v3AdjLocalPos.z -=
                        10.0f * fTimeOffsetSq;
                }
                if (v3AdjLocalPos.z < 0.18f)
                    v3AdjLocalPos.z = 0.18f;

                {
                    float fMilestoneVal =
                        candidateBlendInfo.mfMilestoneTime[milestone];

                    if (fMilestoneVal > 0.0f)
                    {
                        fSaveTime = fSaveTime - fMilestoneVal;
                        if (bFromTakeoff && pCur != pConnected)
                            nlVec3Add(v3AdjLocalPos, v3AdjLocalPos,
                                pCur->mv3TakeoffPos);
                    }
                    else
                    {
                        float fScale =
                            1.0f - fDefaultMilestoneValues[milestone];
                        fSaveTime = fSaveTime * fScale;
                    }
                }

                if (fSaveTime <= fThisTime)
                {
                    float fDistY = v3AdjLocalPos.y
                        - candidateBlendInfo.mv3BlendedSavePos.y;
                    float fDistSq = nlGetLengthSquared2D(fDistY,
                        v3AdjLocalPos.z
                            - candidateBlendInfo.mv3BlendedSavePos.z);

                    if (fDistSq < fClosest)
                    {
                        if (fDistSq < mfCatchAllowDistSq
                            || !(pCur->muSaveType & 0x80003))
                        {
                            fClosest = fDistSq;
                            pClosest = pCur;

                            blendInfo = candidateBlendInfo;

                            float fMilestoneTime =
                                blendInfo.mfMilestoneTime[2];
                            blendInfo.mfStartTime =
                                (0.0f >= fMilestoneTime - fThisTime)
                                ? 0.0f
                                : fMilestoneTime - fThisTime;

                            if (bFromTakeoff)
                            {
                                nlVec3Sub(blendInfo.mv3BlendedSavePos,
                                    blendInfo.mv3BlendedSavePos,
                                    pCur->mv3TakeoffPos);
                            }

                            if (fDistSq < 0.05f * 0.05f)
                                break;
                        }
                    }
                }
            }
        }

        iterator.Next();
    }

    if (pClosest != 0)
        CalcMilestoneScales(blendInfo);

    return pClosest;
}

void CalcMilestoneScales(SaveBlendInfo& blendInfo)
{
    SaveData* pConnected;
    int segment;
    int anim;
    float fLastTime;
    float fThisTime;
    unsigned char bEmptySpot;
    float fInvSegTime;

    bEmptySpot = 0;

    {
        for (anim = 0; anim < 4; anim++)
        {
            pConnected = blendInfo.mpSaveData[anim];
            if (pConnected == 0)
                continue;

            {
                fLastTime = 0.0f;

                for (segment = 0; segment < 5; segment++)
                {
                    fThisTime = pConnected->mfMilestonePercent[segment]
                        * pConnected->mfDuration;
                    if (fThisTime > 0.0f)
                    {
                        blendInfo.mfMilestoneScale[anim][segment] =
                            fThisTime - fLastTime;
                        fLastTime = fThisTime;
                    }
                    else
                    {
                        blendInfo.mfMilestoneScale[anim][segment] = -1.0f;
                        bEmptySpot = 1;
                    }
                }
            }
        }
    }

    {
        fLastTime = 0.0f;
        for (segment = 0; segment < 5; segment++)
        {
            fThisTime = blendInfo.mfMilestoneTime[segment];
            if (fThisTime > 0.0f)
            {
                fInvSegTime = 1.0f / (fThisTime - fLastTime);
                fLastTime = fThisTime;

                for (anim = 0; anim < 4; anim++)
                {
                    if (blendInfo.mpSaveData[anim])
                    {
                        blendInfo.mfMilestoneScale[anim][segment] *=
                            fInvSegTime;
                    }
                }
            }
        }
    }

    if ((unsigned char)bEmptySpot)
    {
        for (segment = 3; segment >= 0; segment--)
        {
            if (blendInfo.mfMilestoneTime[segment] <= 0.0f)
            {
                for (anim = 0; anim < 4; anim++)
                {
                    blendInfo.mfMilestoneScale[anim][segment] =
                        blendInfo.mfMilestoneScale[anim][segment + 1];
                }
            }
        }
    }
}

static inline float GetBlendScale(float fValue, float fMin, float fMax)
{
    return (fValue - fMin) / (fMax - fMin);
}

SaveData* GoalieSave::GetClosestBlendedPos(SaveBlendInfo& blendInfo,
    const nlVector3& v3TargetPos, SaveData* pSaveData)
{
    SaveData* pClosest = pSaveData;
    SaveData* pEdge = 0;
    SaveData* pLast;

    SaveData* pLeft;
    SaveData* pRight;
    SaveData* pLeftUp;
    SaveData* pRightUp;

    float fScaleLeft;
    float fScaleRight;

    blendInfo.mfSaveBlendPrimary = 0.0f;
    blendInfo.mfSaveBlendSecondary = 0.0f;
    blendInfo.mfSaveBlendComposite = 0.0f;

    if (pSaveData->mv3GroupMaxCoords.y > v3TargetPos.y)
    {
        if (pSaveData->mv3GroupMinCoords.y < v3TargetPos.y)
        {
            FindVerticalBoundingPoints(
                pSaveData, v3TargetPos, &pLeft, &pLeftUp);

            pRight = pLeft;
            pRightUp = pLeftUp;

            unsigned char done = 0;
            while (!done)
            {
                if (v3TargetPos.y <= pLeft->mv3SavePos.y
                    || v3TargetPos.y <= pLeftUp->mv3SavePos.y)
                {
                    if (v3TargetPos.y >= pLeft->mv3SavePos.y
                        || v3TargetPos.y >= pLeftUp->mv3SavePos.y
                        || pLeft->mpConnectedSaveData[3] == 0)
                    {
                        pEdge = pLeft;
                        break;
                    }
                    else
                    {
                        pRight = pLeft;
                        pRightUp = pLeftUp;
                        pLeft = pLeft->mpConnectedSaveData[3];
                        FindVerticalBoundingPoints(
                            pLeft, v3TargetPos, &pLeft, &pLeftUp);
                    }
                }
                else if (v3TargetPos.y >= pRight->mv3SavePos.y
                    || v3TargetPos.y >= pRightUp->mv3SavePos.y)
                {
                    if (v3TargetPos.y <= pRight->mv3SavePos.y
                        || v3TargetPos.y <= pRightUp->mv3SavePos.y
                        || pRight->mpConnectedSaveData[2] == 0)
                    {
                        pEdge = pRight;
                        break;
                    }
                    else
                    {
                        pLeft = pRight;
                        pLeftUp = pRightUp;
                        pRight = pRight->mpConnectedSaveData[2];
                        FindVerticalBoundingPoints(
                            pRight, v3TargetPos, &pRight, &pRightUp);
                    }
                }
                else
                {
                    fScaleLeft = 0.0f;
                    fScaleRight = 0.0f;

                    if (pLeft != pLeftUp)
                    {
                        fScaleLeft = GetBlendScale(v3TargetPos.z,
                            pLeft->mv3SavePos.z, pLeftUp->mv3SavePos.z);
                    }

                    if (pRight != pRightUp)
                    {
                        fScaleRight = GetBlendScale(v3TargetPos.z,
                            pRight->mv3SavePos.z, pRightUp->mv3SavePos.z);
                    }

                    float fLefty = Interpolate(pLeft->mv3SavePos.y,
                        pLeftUp->mv3SavePos.y, fScaleLeft);
                    float fRighty = Interpolate(pRight->mv3SavePos.y,
                        pRightUp->mv3SavePos.y, fScaleRight);
                    float fComposite =
                        (v3TargetPos.y - fLefty) / (fRighty - fLefty);

                    if (fComposite <= 0.001f)
                    {
                        pEdge = pLeft;
                        break;
                    }

                    if (fComposite >= 0.999f)
                    {
                        pEdge = pRight;
                        break;
                    }

                    done = 1;
                    blendInfo.mfSaveBlendComposite = fComposite;

                    float fTimeLeft[5];
                    float fLeftZ;
                    {
                        int milestone;

                        blendInfo.mpSaveData[1] = 0;
                        if (fScaleLeft <= 0.999f)
                        {
                            blendInfo.mpSaveData[0] = pLeft;
                            if (fScaleLeft >= 0.001f)
                            {
                                blendInfo.mpSaveData[1] = pLeftUp;
                                pClosest = pLeft;
                                fLeftZ = v3TargetPos.z;
                                blendInfo.mfSaveBlendPrimary = fScaleLeft;

                                for (milestone = 0; milestone < 5;
                                     milestone++)
                                {
                                    float fLeftUpTime =
                                        pLeftUp->GetMilestoneTime(milestone);
                                    float fLeftTime =
                                        pLeft->GetMilestoneTime(milestone);

                                    fTimeLeft[milestone] =
                                        (fLeftTime <= 0.001f)
                                        ? 0.0f
                                        : Interpolate(fLeftTime, fLeftUpTime,
                                              fScaleLeft);
                                }
                            }
                            else
                            {
                                fLeftZ = pLeft->mv3SavePos.z;
                                for (milestone = 0; milestone < 5;
                                     milestone++)
                                {
                                    fTimeLeft[milestone] =
                                        pLeft->GetMilestoneTime(milestone);
                                }
                            }
                        }
                        else
                        {
                            blendInfo.mpSaveData[0] = pLeftUp;
                            fLeftZ = pLeftUp->mv3SavePos.z;
                            for (milestone = 0; milestone < 5; milestone++)
                            {
                                fTimeLeft[milestone] =
                                    pLeftUp->GetMilestoneTime(milestone);
                            }
                        }
                    }

                    float fTimeRight[5];
                    float fRightZ;
                    {
                        int milestone;

                        blendInfo.mpSaveData[3] = 0;
                        if (fScaleRight <= 0.999f)
                        {
                            blendInfo.mpSaveData[2] = pRight;
                            if (fScaleRight >= 0.001f)
                            {
                                blendInfo.mpSaveData[3] = pRightUp;
                                fRightZ = v3TargetPos.z;
                                blendInfo.mfSaveBlendSecondary = fScaleRight;

                                for (milestone = 0; milestone < 5;
                                     milestone++)
                                {
                                    float fRightUpTime =
                                        pRightUp->GetMilestoneTime(milestone);
                                    float fRightTime =
                                        pRight->GetMilestoneTime(milestone);

                                    fTimeRight[milestone] =
                                        (fRightTime <= 0.001f)
                                        ? 0.0f
                                        : Interpolate(fRightTime,
                                              fRightUpTime, fScaleRight);
                                }
                            }
                            else
                            {
                                fRightZ = pRight->mv3SavePos.z;
                                for (milestone = 0; milestone < 5;
                                     milestone++)
                                {
                                    fTimeRight[milestone] =
                                        pRight->GetMilestoneTime(milestone);
                                }
                            }
                        }
                        else
                        {
                            blendInfo.mpSaveData[2] = pRightUp;
                            fRightZ = pRightUp->mv3SavePos.z;
                            for (milestone = 0; milestone < 5; milestone++)
                            {
                                fTimeRight[milestone] =
                                    pRightUp->GetMilestoneTime(milestone);
                            }
                        }
                    }

                    blendInfo.mv3BlendedSavePos.y = v3TargetPos.y;
                    blendInfo.mv3BlendedSavePos.z =
                        Interpolate(fLeftZ, fRightZ, fComposite);

                    {
                        int milestone;

                        for (milestone = 0; milestone < 5; milestone++)
                        {
                            float fRightTime = fTimeRight[milestone];
                            float fLeftTime = fTimeLeft[milestone];
                            blendInfo.mfMilestoneTime[milestone] =
                                (fLeftTime <= 0.001f)
                                ? 0.0f
                                : Interpolate(fLeftTime, fRightTime,
                                      fComposite);
                        }
                    }

                    if (fComposite <= 0.5f)
                    {
                        if (fScaleLeft <= 0.5f)
                            pClosest = pLeft;
                        else
                            pClosest = pLeftUp;
                    }
                    else
                    {
                        if (fScaleRight <= 0.5f)
                            pClosest = pRight;
                        else
                            pClosest = pRightUp;
                    }
                }
            }
        }
        else
        {
            SaveData* pCurEdge = pSaveData;
            while (pCurEdge != 0)
            {
                pLast = pCurEdge;
                pCurEdge = pCurEdge->mpConnectedSaveData[3];
            }
            pEdge = pLast;
        }
    }
    else
    {
        SaveData* pCurEdge = pSaveData;
        while (pCurEdge != 0)
        {
            pLast = pCurEdge;
            pCurEdge = pCurEdge->mpConnectedSaveData[2];
        }
        pEdge = pLast;
    }

    if (pEdge != 0)
    {
        SaveData* pDown;
        SaveData* pUp;
        int milestone;

        FindVerticalBoundingPoints(
            pEdge, v3TargetPos, &pDown, &pUp);

        blendInfo.mpSaveData[0] = pDown;
        blendInfo.mpSaveData[1] = 0;
        blendInfo.mpSaveData[3] = 0;
        blendInfo.mpSaveData[2] = 0;

        const nlVector3& v3DownPos = pDown->mv3SavePos;
        const nlVector3& v3UpPos = pUp->mv3SavePos;

        if (pDown != pUp)
        {
            float fPrimary = GetBlendScale(
                v3TargetPos.z, v3DownPos.z, v3UpPos.z);
            if (fPrimary >= 0.999f)
            {
                blendInfo.mv3BlendedSavePos = v3UpPos;
                blendInfo.mpSaveData[0] = pUp;
                for (milestone = 0; milestone < 5; milestone++)
                {
                    blendInfo.mfMilestoneTime[milestone] =
                        pUp->GetMilestoneTime(milestone);
                }
            }
            else if (fPrimary <= 0.001f)
            {
                blendInfo.mv3BlendedSavePos = v3DownPos;
                for (milestone = 0; milestone < 5; milestone++)
                {
                    blendInfo.mfMilestoneTime[milestone] =
                        pDown->GetMilestoneTime(milestone);
                }
            }
            else
            {
                blendInfo.mfSaveBlendPrimary = fPrimary;
                blendInfo.mv3BlendedSavePos.x = v3DownPos.x;
                blendInfo.mv3BlendedSavePos.y =
                    Interpolate(v3DownPos.y, v3UpPos.y, fPrimary);
                blendInfo.mv3BlendedSavePos.z = v3TargetPos.z;
                blendInfo.mpSaveData[1] = pUp;

                for (milestone = 0; milestone < 5; milestone++)
                {
                    float fUpTime = pUp->GetMilestoneTime(milestone);
                    float fDownTime =
                        pDown->GetMilestoneTime(milestone);

                    blendInfo.mfMilestoneTime[milestone] =
                        (fDownTime <= 0.001f)
                        ? 0.0f
                        : Interpolate(fDownTime, fUpTime, fPrimary);
                }
            }
        }
        else
        {
            blendInfo.mv3BlendedSavePos = v3DownPos;
            for (milestone = 0; milestone < 5; milestone++)
            {
                blendInfo.mfMilestoneTime[milestone] =
                    pDown->GetMilestoneTime(milestone);
            }

            if (pDown->mpConnectedSaveData[1] == 0
                && pDown->mpConnectedSaveData[0] == 0)
            {
                const float fNudge = 0.1f;

                if (fabsf(v3DownPos.y - v3TargetPos.y)
                    < fNudge)
                {
                    blendInfo.mv3BlendedSavePos.y = v3TargetPos.y;
                }
                else if (v3DownPos.y > v3TargetPos.y)
                {
                    blendInfo.mv3BlendedSavePos.y -= fNudge;
                }
                else
                {
                    blendInfo.mv3BlendedSavePos.y += fNudge;
                }

                if (fabsf(v3DownPos.z - v3TargetPos.z)
                    < fNudge)
                {
                    blendInfo.mv3BlendedSavePos.z = v3TargetPos.z;
                }
                else if (v3DownPos.z > v3TargetPos.z)
                {
                    blendInfo.mv3BlendedSavePos.z -= fNudge;
                }
                else
                {
                    blendInfo.mv3BlendedSavePos.z += fNudge;
                }
            }
        }

        if (blendInfo.mfSaveBlendPrimary < 0.5f)
            pClosest = blendInfo.mpSaveData[0];
        else
            pClosest = blendInfo.mpSaveData[1];
    }

    blendInfo.mv3BlendedSavePos.x = pClosest->mv3SavePos.x;
    return pClosest;
}

SaveData* GoalieSave::FindSaveData(int animID)
{
    SaveData** ppSaveData;
    if (animID >= 0 && gSaveMap.FindGet(animID, &ppSaveData))
    {
        return *ppSaveData;
    }

    return 0;
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

SaveData* GoalieSave::GetRandomSTSMissData(bool bCatchAnimOnly)
{
    int index = muSTSMissIndexStart + (bCatchAnimOnly ? 2 : 3);
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

    nlListIterator<SaveData*> iterator = cell.Begin();
    if (iterator.IsValid())
    {
        while (iterator.IsValid())
        {
            if (iterator.Current() == pSaveData)
                return;
            iterator.Next();
        }
    }

    {
        ListEntry<SaveData*>* newEntry =
            (ListEntry<SaveData*>*)nlMalloc(
                sizeof(ListEntry<SaveData*>), 8, false);
        if (newEntry != 0)
        {
            newEntry->next = 0;
            newEntry->entry = pSaveData;
        }
        nlListAddStart<ListEntry<SaveData*> >(
            &cell.m_Head, newEntry, &cell.m_Tail);
    }
}

static inline bool IsCellEmpty(const nlListContainer<SaveData*>& cell)
{
    return cell.m_Head == 0;
}

static inline void AddSinglePointToGrid(
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

    if (!IsCellEmpty(cell))
    {
        nlListIterator<SaveData*> iterator = cell.Begin();
        while (iterator.IsValid())
        {
            if (iterator.Current() == pSaveData)
                return;
            iterator.Next();
        }
    }

    {
        ListEntry<SaveData*>* newEntry =
            (ListEntry<SaveData*>*)nlMalloc(
                sizeof(ListEntry<SaveData*>), 8, false);
        if (newEntry != 0)
        {
            newEntry->next = 0;
            newEntry->entry = pSaveData;
        }
        nlListAddStart<ListEntry<SaveData*> >(
            &cell.m_Head, newEntry, &cell.m_Tail);
    }
}

static inline float GetDistSqYZ(const nlVector3& a, const nlVector3& b)
{
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return nlGetLengthSquared2D(dy, dz);
}

void GoalieSave::AddAreaToGrid(SaveData* pSaveData)
{
    SaveData* pCur;
    nlVector3 v3TopRight;
    nlVector3 v3BotLeft;
    float yInc;
    float zInc;
    nlVector3 v3CurColPos;
    nlVector3 v3CurRowPos;
    SaveData* pNextRight;
    SaveData* pCurBot;
    SaveData* pRowBot;
    SaveData* pRightCorner;
    SaveData* pNextNextRight;
    SaveData* pCurLeft;
    SaveData* pCurRight;
    SaveData* pClosest;
    float fCloseDist;
    SaveData* const pRoot = pSaveData;

    {
        SaveData* pCurRightUp;
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
    }

    {
        SaveData* pCurUp;
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
    v3TopRight.y += 0.51f * yInc;
    v3TopRight.z += 0.51f * zInc;

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
    v3BotLeft.y -= 0.51f * yInc;
    v3BotLeft.z -= 0.51f * zInc;

    pNextRight = pRightCorner;
    pRowBot = pNextRight;
    pNextNextRight = pNextRight->mpConnectedSaveData[2];
    v3CurRowPos = v3BotLeft;

    while (v3CurRowPos.y < v3TopRight.y)
    {
        if (v3CurRowPos.y >= pNextRight->mv3SavePos.y
            && pNextNextRight != 0)
        {
            pRowBot = pNextRight;
            pNextRight = pNextNextRight;
            pNextNextRight = pNextNextRight->mpConnectedSaveData[2];
        }
        pCurLeft = pRowBot;
        pCurRight = pNextRight;
        v3CurColPos = v3CurRowPos;

        if (v3CurColPos.z < v3TopRight.z)
        {
            while (v3CurColPos.z < v3TopRight.z)
            {
                SaveData* pCurUp;
                SaveData* pCurRightUp;
                FindVerticalBoundingPoints(
                    pCurLeft, v3CurColPos, &pCurLeft, &pCurUp);
                FindVerticalBoundingPoints(
                    pCurRight, v3CurColPos, &pCurRight, &pCurRightUp);

                {
                    fCloseDist = GetDistSqYZ(pCurLeft->mv3SavePos, v3CurColPos);
                    pClosest = pCurLeft;

                    if (pCurLeft != pCurUp)
                    {
                        float d = GetDistSqYZ(pCurUp->mv3SavePos, v3CurColPos);
                        if (d < fCloseDist)
                        {
                            fCloseDist = d;
                            pClosest = pCurUp;
                        }
                    }

                    if (pCurLeft != pCurRight)
                    {
                        float d = GetDistSqYZ(pCurRight->mv3SavePos, v3CurColPos);
                        if (d < fCloseDist)
                        {
                            fCloseDist = d;
                            pClosest = pCurRight;
                        }
                        if (pCurRight != pCurRightUp)
                        {
                            float fUpRightDistSq = GetDistSqYZ(pCurRightUp->mv3SavePos, v3CurColPos);
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

static inline int AbsInt(int value)
{
    return value < 0 ? -value : value;
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
    divisions = AbsInt(i - m) + AbsInt(j - n);
    nlVec3Sub(v3Delta, pSaveData2->mv3SavePos, pSaveData1->mv3SavePos);
    if (divisions > 0)
    {
        nlVec3Scale(v3Delta, v3Delta, 1.0f / (float)divisions);
    }
    v3CurPos = pSaveData1->mv3SavePos;
    count = 0;
    if (divisions >= 0)
    {
        while (count <= divisions)
        {
            if (GetDistSqYZ(pSaveData1->mv3SavePos, v3CurPos)
                < GetDistSqYZ(pSaveData2->mv3SavePos, v3CurPos))
                pCurSaveData = pSaveData1;
            else
                pCurSaveData = pSaveData2;
            AddPointToGrid(pCurSaveData, v3CurPos);
            nlVec3Add(v3CurPos, v3CurPos, v3Delta);
            count++;
        }
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

    AddSinglePointToGrid(pSaveData, pSaveData->mv3SavePos);
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

void GoalieSave::ClearGrid()
{
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            gSaveGrid[i][j].Clear();
        }
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
