#ifndef GAME_SCRIPT_TUNING_H
#define GAME_SCRIPT_TUNING_H

#include "Game/TweaksBase.h"
#include "Game/TweakValue.h"

class FuzzyTweaks : public TweaksBase
{
public:
    FuzzyTweaks(const char* name, const char* category);
    virtual ~FuzzyTweaks();
    virtual void Init();

public:
    void RegisterTweaks(bool registerTweaks);

    /* 0x044 */ TweakFloatBinding fCloseTeammateConfidenceDistanceMin;
    /* 0x054 */ TweakFloatBinding fCloseTeammateConfidenceDistanceMax;
    /* 0x064 */ TweakFloatBinding fNearTeammateConfidenceDistanceMin;
    /* 0x074 */ TweakFloatBinding fNearTeammateConfidenceDistanceMax;
    /* 0x084 */ TweakFloatBinding fFarTeammateConfidenceDistanceMin;
    /* 0x094 */ TweakFloatBinding fFarTeammateConfidenceDistanceMax;
    /* 0x0A4 */ TweakFloatBinding fCloseOpponentConfidenceDistanceMin;
    /* 0x0B4 */ TweakFloatBinding fCloseOpponentConfidenceDistanceMax;
    /* 0x0C4 */ TweakFloatBinding fNearOpponentConfidenceDistanceMin;
    /* 0x0D4 */ TweakFloatBinding fNearOpponentConfidenceDistanceMax;
    /* 0x0E4 */ TweakFloatBinding fFarOpponentConfidenceDistanceMin;
    /* 0x0F4 */ TweakFloatBinding fFarOpponentConfidenceDistanceMax;
    /* 0x104 */ TweakFloatBinding fReallyCloseToBallDistanceConfidenceMin;
    /* 0x114 */ TweakFloatBinding fReallyCloseToBallDistanceConfidenceMax;
    /* 0x124 */ TweakFloatBinding fCloseBallConfidenceDistanceMin;
    /* 0x134 */ TweakFloatBinding fCloseBallConfidenceDistanceMax;
    /* 0x144 */ TweakFloatBinding fNearBallConfidenceDistanceMin;
    /* 0x154 */ TweakFloatBinding fNearBallConfidenceDistanceMax;
    /* 0x164 */ TweakFloatBinding fFarBallConfidenceDistanceMin;
    /* 0x174 */ TweakFloatBinding fFarBallConfidenceDistanceMax;
    /* 0x184 */ TweakFloatBinding fCloseNetConfidenceDistanceMin;
    /* 0x194 */ TweakFloatBinding fCloseNetConfidenceDistanceMax;
    /* 0x1A4 */ TweakFloatBinding fNearNetConfidenceDistanceMin;
    /* 0x1B4 */ TweakFloatBinding fNearNetConfidenceDistanceMax;
    /* 0x1C4 */ TweakFloatBinding fFarNetConfidenceDistanceMin;
    /* 0x1D4 */ TweakFloatBinding fFarNetConfidenceDistanceMax;
    /* 0x1E4 */ TweakFloatBinding fCloseBallNetConfidenceDistanceMin;
    /* 0x1F4 */ TweakFloatBinding fCloseBallNetConfidenceDistanceMax;
    /* 0x204 */ TweakFloatBinding fNearBallNetConfidenceDistanceMin;
    /* 0x214 */ TweakFloatBinding fNearBallNetConfidenceDistanceMax;
    /* 0x224 */ TweakFloatBinding fFarBallNetConfidenceDistanceMin;
    /* 0x234 */ TweakFloatBinding fFarBallNetConfidenceDistanceMax;
    /* 0x244 */ TweakFloatBinding fCloseToFormationPositionDistanceMin;
    /* 0x254 */ TweakFloatBinding fCloseToFormationPositionDistanceMax;
    /* 0x264 */ TweakFloatBinding fNearToFormationPositionDistanceMin;
    /* 0x274 */ TweakFloatBinding fNearToFormationPositionDistanceMax;
    /* 0x284 */ TweakFloatBinding fFarToFormationPositionDistanceMin;
    /* 0x294 */ TweakFloatBinding fFarToFormationPositionDistanceMax;
    /* 0x2A4 */ TweakFloatBinding fCloseGoalieConfidenceDistanceMin;
    /* 0x2B4 */ TweakFloatBinding fCloseGoalieConfidenceDistanceMax;
    /* 0x2C4 */ TweakFloatBinding fNearGoalieConfidenceDistanceMin;
    /* 0x2D4 */ TweakFloatBinding fNearGoalieConfidenceDistanceMax;
    /* 0x2E4 */ TweakFloatBinding fFarGoalieConfidenceDistanceMin;
    /* 0x2F4 */ TweakFloatBinding fFarGoalieConfidenceDistanceMax;
    /* 0x304 */ TweakFloatBinding fCloseToSidelineDistanceConfidenceMin;
    /* 0x314 */ TweakFloatBinding fCloseToSidelineDistanceConfidenceMax;
    /* 0x324 */ TweakFloatBinding fNearToSidelineDistanceConfidenceMin;
    /* 0x334 */ TweakFloatBinding fNearToSidelineDistanceConfidenceMax;
    /* 0x344 */ TweakFloatBinding fFarFromSidelineDistanceConfidenceMin;
    /* 0x354 */ TweakFloatBinding fFarFromSidelineDistanceConfidenceMax;
    /* 0x364 */ TweakFloatBinding fHighBallConfidenceDistanceMin;
    /* 0x374 */ TweakFloatBinding fHighBallConfidenceDistanceMax;
    /* 0x384 */ TweakFloatBinding fReallyHighBallConfidenceDistanceMin;
    /* 0x394 */ TweakFloatBinding fReallyHighBallConfidenceDistanceMax;
    /* 0x3A4 */ TweakIntBinding nFacingFullConfidenceAngle;
    /* 0x3B4 */ TweakIntBinding nFacingNoConfidenceAngle;
    /* 0x3C4 */ TweakFloatBinding fControlConfidenceDistanceMin;
    /* 0x3D4 */ TweakFloatBinding fControlConfidenceDistanceMax;
    /* 0x3E4 */ TweakFloatBinding fPassLaneDistance;
    /* 0x3F4 */ TweakFloatBinding fShotLaneDistance;
    /* 0x404 */ TweakFloatBinding fPassInPlayFullConfidenceDistance;
    /* 0x414 */ TweakFloatBinding fShotInPlayFullConfidenceDistance;
    /* 0x424 */ TweakFloatBinding fOnGroundConfidenceDistanceMin;
    /* 0x434 */ TweakFloatBinding fOnGroundConfidenceDistanceMax;
    /* 0x444 */ TweakFloatBinding fInterceptBallSwapControlerScoreWeight;
    /* 0x454 */ TweakFloatBinding fInterceptBallScoreWeight;
    /* 0x464 */ TweakFloatBinding fInterceptBallConfidenceTimeMin;
    /* 0x474 */ TweakFloatBinding fInterceptBallConfidenceTimeMax;
    /* 0x484 */ TweakFloatBinding fInterceptBallConfidenceDistanceMin;
    /* 0x494 */ TweakFloatBinding fInterceptBallConfidenceDistanceMax;
    /* 0x4A4 */ TweakFloatBinding fPressuredNearWeight;
    /* 0x4B4 */ TweakFloatBinding fAvoidGoalieRepulsionConfidenceMin;
    /* 0x4C4 */ TweakFloatBinding fAvoidGoalieRepulsionConfidenceMax;
    /* 0x4D4 */ TweakFloatBinding fAvoidFieldersRepulsionConfidenceMin;
    /* 0x4E4 */ TweakFloatBinding fAvoidFieldersRepulsionConfidenceMax;
    /* 0x4F4 */ TweakFloatBinding fAvoidPowerupsRepulsionConfidenceMin;
    /* 0x504 */ TweakFloatBinding fAvoidPowerupsRepulsionConfidenceMax;
    /* 0x514 */ TweakFloatBinding fBadShooterDistanceMin;
    /* 0x524 */ TweakFloatBinding fBadShooterDistanceMax;
    /* 0x534 */ TweakFloatBinding fGoodShooterDistanceMin;
    /* 0x544 */ TweakFloatBinding fGoodShooterDistanceMax;
    /* 0x554 */ TweakFloatBinding fGoalieOutOfPositionDistanceMin;
    /* 0x564 */ TweakFloatBinding fGoalieOutOfPositionDistanceMax;
    /* 0x574 */ TweakFloatBinding fOutOfNetConfidenceDistanceMin;
    /* 0x584 */ TweakFloatBinding fOutOfNetConfidenceDistanceMax;
    /* 0x594 */ TweakFloatBinding fUpfieldMaxDistance;
    /* 0x5A4 */ TweakFloatBinding fDownfieldMaxDistance;
    /* 0x5B4 */ TweakFloatBinding fClosingSpeedMax;
    /* 0x5C4 */ TweakFloatBinding fSeparatingSpeedMax;
    /* 0x5D4 */ TweakFloatBinding fFrontOfNetMidAngle;
    /* 0x5E4 */ TweakFloatBinding fFrontOfNetMaxAngle;
    /* 0x5F4 */ TweakFloatBinding fFrontOfNetMidScore;
    /* 0x604 */ TweakFloatBinding fGetOpenPassLaneOffsetMin;
    /* 0x614 */ TweakFloatBinding fGetOpenPassLaneOffsetMax;
    /* 0x624 */ TweakFloatBinding fGetOpenPassLaneDistMin;
    /* 0x634 */ TweakFloatBinding fGetOpenPassLaneDistMax;
    /* 0x644 */ TweakFloatBinding fOpenRadiusMin;
    /* 0x654 */ TweakFloatBinding fOpenRadiusMax;
    /* 0x664 */ TweakFloatBinding fWideOpenRadiusMin;
    /* 0x674 */ TweakFloatBinding fWideOpenRadiusMax;
    /* 0x684 */ TweakFloatBinding fInBetweenInterceptRangeMin;
    /* 0x694 */ TweakFloatBinding fInBetweenInterceptRangeMax;
    /* 0x6A4 */ TweakFloatBinding fInBetweenConeWidthMin;
    /* 0x6B4 */ TweakFloatBinding fInBetweenConeWidthMax;
    /* 0x6C4 */ TweakFloatBinding fPassDeadZone;
    /* 0x6D4 */ TweakFloatBinding fLosingScoreDelta;
    /* 0x6E4 */ TweakFloatBinding fWinningScoreDelta;
    /* 0x6F4 */ TweakFloatBinding fTiedScoreDelta;
    /* 0x704 */ TweakFloatBinding fGameTimeCloseToOver;
    /* 0x714 */ TweakFloatBinding fGameTimeNearlyOver;
    /* 0x724 */ TweakFloatBinding fGameTimeFarFromOver;
    /* 0x734 */ TweakFloatBinding fDefensiveConfidenceDistancesMin;
    /* 0x744 */ TweakFloatBinding fDefensiveConfidenceDistancesMax;
    /* 0x754 */ TweakFloatBinding fOffensiveConfidenceDistancesMin;
    /* 0x764 */ TweakFloatBinding fOffensiveConfidenceDistancesMax;
    /* 0x774 */ TweakFloatBinding fStallingTimeEasyMin;
    /* 0x784 */ TweakFloatBinding fStallingTimeEasyMax;
    /* 0x794 */ TweakFloatBinding fStallingTimeHardMin;
    /* 0x7A4 */ TweakFloatBinding fStallingTimeHardMax;

    /* 0x7B4 */ const char* mCategory;
}; // total size: 0x7B8

#endif // GAME_SCRIPT_TUNING_H
