#ifndef GAME_AI_GOALIE_SAVE_H
#define GAME_AI_GOALIE_SAVE_H

#include "NL/nlMath.h"
#include "NL/nlList.h"

class Goalie;
struct SaveInfo;

class SavePositionData
{
public:
    /* 0x00 */ int mnAnimID;
    /* 0x04 */ float mfAnimDistance;
    /* 0x08 */ float mfAnimTime;
    /* 0x0C */ float mfAnimVelocity;
}; // total size: 0x10

class SaveData
{
public:
    static float LookupFatigueValue(
        Goalie* pGoalie, const SaveInfo& info);
    void Init(Goalie* pGoalie, const SaveInfo& info, unsigned int uIndex);

    float GetMilestoneTime(int milestone) const
    {
        return mfMilestonePercent[milestone] * mfDuration;
    }

    /* 0x00 */ int mnAnimID;
    /* 0x04 */ SaveData* mpFailAnimData;
    /* 0x08 */ int mnRecoverAnimID;
    /* 0x0C */ unsigned int muSaveType;
    /* 0x10 */ nlVector3 mv3SavePos;
    /* 0x1C */ nlVector3 mv3TakeoffPos;
    /* 0x28 */ float mfDuration;
    /* 0x2C */ float mfMilestonePercent[5];
    /* 0x40 */ float mfFatigueValue;
    /* 0x44 */ SaveData* mpConnectedSaveData[4];
    /* 0x54 */ nlVector3 mv3GroupMinCoords;
    /* 0x60 */ nlVector3 mv3GroupMaxCoords;
    /* 0x6C */ char mszName[16];
    /* 0x7C */ int muIndex;
}; // total size: 0x80

struct SaveBlendInfo
{
    SaveBlendInfo();

    /* 0x00 */ float mfStartTime;
    /* 0x04 */ float mfMilestoneTime[5];
    /* 0x18 */ float mfMilestoneScale[4][5];
    /* 0x68 */ float mfSaveBlendPrimary;
    /* 0x6C */ float mfSaveBlendSecondary;
    /* 0x70 */ float mfSaveBlendComposite;
    /* 0x74 */ SaveData* mpSaveData[4];
    /* 0x84 */ nlVector3 mv3BlendedSavePos;
}; // total size: 0x90

class GoalieSave
{
    static void FindVerticalBoundingPoints(SaveData* pSaveData,
        const nlVector3& v3TargetPoint, SaveData** pLoPoint,
        SaveData** pHiPoint);

public:
    static SaveData* FindSaveData(int animID);
    static void ClearData();
    static void InitData(Goalie* pGoalie);
    static SaveData* FindBestSave(SaveBlendInfo& blendInfo,
        const nlVector3& v3LocalPos, float fTime, bool bDoNearSearch,
        unsigned int uSaveType, bool bFromTakeoff);
    static SaveData* FindBestInList(SaveBlendInfo& blendInfo,
        nlListContainer<SaveData*>& SaveList, const nlVector3& v3LocalPos,
        float fTime, unsigned int uSaveType, bool bFromTakeoff);
    static SaveData* GetClosestBlendedPos(SaveBlendInfo& blendInfo,
        const nlVector3& v3TargetPos, SaveData* pSaveData);
    static SaveData* GetMissChipSaveData(bool bLeft, bool bFar);
    static SaveData* GetSTSSpinMissData(bool bLeft);
    static bool TriggerCallback(float fTime, float fDuration,
        unsigned long uEventID, float fIntensity, void* pUserData);
    static void AddAreaToGrid(SaveData* pSaveData);
    static void AddSegmentToGrid(
        SaveData* pSaveData1, SaveData* pSaveData2);
    static void AddChainToGrid(SaveData* pSaveData, bool bVertical);
    static void AddToGrid(SaveData* pSaveData);
    static float GridSectionWidth();
    static float GridSectionHeight();
    static float mfCatchAllowDistSq;
    static SaveData* mpSaveTable;
    static unsigned char mbInitialized;
    static unsigned int muNumSaveEntries;
    static SavePositionData* mpPositionTable;
    static unsigned int muNumPositionEntries;
    static unsigned int muMissChipIndexStart;
    static unsigned int muSTSMissIndexStart;
    static float mfCrouchDuration;
};

#endif // GAME_AI_GOALIE_SAVE_H
