#ifndef GAME_AI_GOALIE_SAVE_H
#define GAME_AI_GOALIE_SAVE_H

#include "NL/nlMath.h"

class Goalie;

class SaveData
{
public:
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
    /* 0x00 */ float mfStartTime;
    /* 0x04 */ float mfMilestoneTime[5];
    /* 0x18 */ float mfMilestoneScale[4][5];
    /* 0x68 */ float mfSaveBlendPrimary;
    /* 0x6C */ float mfSaveBlendSecondary;
    /* 0x70 */ float mfSaveBlendComposite;
    /* 0x74 */ SaveData* mpSaveData[4];
    /* 0x84 */ nlVector3 mv3BlendedSavePos;
}; // total size: 0x90

#endif // GAME_AI_GOALIE_SAVE_H
