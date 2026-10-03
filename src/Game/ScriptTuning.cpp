#include "Game/ScriptTuning.h"

#include "Game/TweakConfig.h"
#include "types.h"
#include "Game/TweakFileLoader.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/TweakValue.inl"

FuzzyTweaks::FuzzyTweaks(const char* name, const char* category)
    : TweaksBase(name)
    , mCategory(category)
{
    Init();
    RegisterTweaks(true);
}

FuzzyTweaks::~FuzzyTweaks()
{
}

void FuzzyTweaks::RegisterTweaks(bool registerTweaks)
{
    if (registerTweaks)
    {
        gTweakFileLoader.LoadFileAsync(mszFileName, mCategory);
    }
    else
    {
        LoadTweakConfigFile(mszFileName, mCategory, true);
    }
}

void FuzzyTweaks::Init()
{
    fCloseTeammateConfidenceDistanceMin.BindWithDefault("Close 2 Teammate Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseTeammateConfidenceDistanceMax.BindWithDefault("Close 2 Teammate Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearTeammateConfidenceDistanceMin.BindWithDefault("Near 2 Teammate Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearTeammateConfidenceDistanceMax.BindWithDefault("Near 2 Teammate Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarTeammateConfidenceDistanceMin.BindWithDefault("Far 2 Teammate Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarTeammateConfidenceDistanceMax.BindWithDefault("Far 2 Teammate Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseOpponentConfidenceDistanceMin.BindWithDefault("Close 2 Opponent Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseOpponentConfidenceDistanceMax.BindWithDefault("Close 2 Opponent Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearOpponentConfidenceDistanceMin.BindWithDefault("Near 2 Opponent Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearOpponentConfidenceDistanceMax.BindWithDefault("Near 2 Opponent Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarOpponentConfidenceDistanceMin.BindWithDefault("Far 2 Opponent Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarOpponentConfidenceDistanceMax.BindWithDefault("Far 2 Opponent Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fReallyCloseToBallDistanceConfidenceMin.BindWithDefault("ReallyClose 2 Ball Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fReallyCloseToBallDistanceConfidenceMax.BindWithDefault("ReallyClose 2 Ball Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseBallConfidenceDistanceMin.BindWithDefault("Close 2 Ball Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseBallConfidenceDistanceMax.BindWithDefault("Close 2 Ball Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearBallConfidenceDistanceMin.BindWithDefault("Near 2 Ball Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearBallConfidenceDistanceMax.BindWithDefault("Near 2 Ball Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarBallConfidenceDistanceMin.BindWithDefault("Far 2 Ball Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarBallConfidenceDistanceMax.BindWithDefault("Far 2 Ball Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseNetConfidenceDistanceMin.BindWithDefault("Close 2 Net Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseNetConfidenceDistanceMax.BindWithDefault("Close 2 Net Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearNetConfidenceDistanceMin.BindWithDefault("Near 2 Net Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearNetConfidenceDistanceMax.BindWithDefault("Near 2 Net Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarNetConfidenceDistanceMin.BindWithDefault("Far 2 Net Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarNetConfidenceDistanceMax.BindWithDefault("Far 2 Net Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseBallNetConfidenceDistanceMin.BindWithDefault("Ball 2 Net Close Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseBallNetConfidenceDistanceMax.BindWithDefault("Ball 2 Net Close Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearBallNetConfidenceDistanceMin.BindWithDefault("Ball 2 Net Near Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearBallNetConfidenceDistanceMax.BindWithDefault("Ball 2 Net Near Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarBallNetConfidenceDistanceMin.BindWithDefault("Ball 2 Net Far Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarBallNetConfidenceDistanceMax.BindWithDefault("Ball 2 Net Far Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseToFormationPositionDistanceMin.BindWithDefault("Close 2 FormationPos Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseToFormationPositionDistanceMax.BindWithDefault("Close 2 FormationPos Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearToFormationPositionDistanceMin.BindWithDefault("Near 2 FormationPos Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearToFormationPositionDistanceMax.BindWithDefault("Near 2 FormationPos Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarToFormationPositionDistanceMin.BindWithDefault("Far 2 FormationPos Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarToFormationPositionDistanceMax.BindWithDefault("Far 2 FormationPos Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseGoalieConfidenceDistanceMin.BindWithDefault("Close 2 Goalie Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseGoalieConfidenceDistanceMax.BindWithDefault("Close 2 Goalie Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearGoalieConfidenceDistanceMin.BindWithDefault("Near 2 Goalie Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearGoalieConfidenceDistanceMax.BindWithDefault("Near 2 Goalie Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarGoalieConfidenceDistanceMin.BindWithDefault("Far 2 Goalie Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarGoalieConfidenceDistanceMax.BindWithDefault("Far 2 Goalie Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseToSidelineDistanceConfidenceMin.BindWithDefault("Close 2 Sideline Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fCloseToSidelineDistanceConfidenceMax.BindWithDefault("Close 2 Sideline Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearToSidelineDistanceConfidenceMin.BindWithDefault("Near 2 Sideline Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearToSidelineDistanceConfidenceMax.BindWithDefault("Near 2 Sideline Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarFromSidelineDistanceConfidenceMin.BindWithDefault("Far 2 Sideline Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFarFromSidelineDistanceConfidenceMax.BindWithDefault("Far 2 Sideline Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fHighBallConfidenceDistanceMin.BindWithDefault("Ball Height High Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fHighBallConfidenceDistanceMax.BindWithDefault("Ball Height High Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fReallyHighBallConfidenceDistanceMin.BindWithDefault("Ball Height Really High Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fReallyHighBallConfidenceDistanceMax.BindWithDefault("Ball Height Really High Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nFacingFullConfidenceAngle.BindWithDefault("Facing Angle Max", -0x270F, mCategory, false, 0.0f, 0.0f, 0.0f);
    nFacingNoConfidenceAngle.BindWithDefault("Facing Angle Min", -0x270F, mCategory, false, 0.0f, 0.0f, 0.0f);
    fControlConfidenceDistanceMin.BindWithDefault("Ball Control Distance From Owner Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fControlConfidenceDistanceMax.BindWithDefault("Ball Control Distance From Owner Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPassLaneDistance.BindWithDefault("Pass Lane Max Width", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotLaneDistance.BindWithDefault("Shot Lane Max Width", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPassInPlayFullConfidenceDistance.BindWithDefault("Pass In Play Dist 2 Target Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotInPlayFullConfidenceDistance.BindWithDefault("Shot In Play Dist 2 Target Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOnGroundConfidenceDistanceMin.BindWithDefault("Player OnGround Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOnGroundConfidenceDistanceMax.BindWithDefault("Player OnGround Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInterceptBallSwapControlerScoreWeight.BindWithDefault("InterceptBall SwapController Time Weight", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInterceptBallScoreWeight.BindWithDefault("InterceptBall Time Weight", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInterceptBallConfidenceTimeMin.BindWithDefault("InterceptBall Min Time", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInterceptBallConfidenceTimeMax.BindWithDefault("InterceptBall Max Time", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInterceptBallConfidenceDistanceMin.BindWithDefault("InterceptBall Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInterceptBallConfidenceDistanceMax.BindWithDefault("InterceptBall Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPressuredNearWeight.BindWithDefault("Pressured Near Weight", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fAvoidGoalieRepulsionConfidenceMin.BindWithDefault("Avoid Goalie Min Repulsion", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fAvoidGoalieRepulsionConfidenceMax.BindWithDefault("Avoid Goalie Max Repulsion", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fAvoidFieldersRepulsionConfidenceMin.BindWithDefault("Avoid Fielders Min Repulsion", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fAvoidFieldersRepulsionConfidenceMax.BindWithDefault("Avoid Fielders Max Repulsion", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fAvoidPowerupsRepulsionConfidenceMin.BindWithDefault("Avoid Powerups Min Repulsion", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fAvoidPowerupsRepulsionConfidenceMax.BindWithDefault("Avoid Powerups Max Repulsion", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBadShooterDistanceMin.BindWithDefault("Bad Shooter Distance Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBadShooterDistanceMax.BindWithDefault("Bad Shooter Distance Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGoodShooterDistanceMin.BindWithDefault("Good Shooter Distance Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGoodShooterDistanceMax.BindWithDefault("Good Shooter Distance Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGoalieOutOfPositionDistanceMin.BindWithDefault("Goalie Out Of Position Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGoalieOutOfPositionDistanceMax.BindWithDefault("Goalie Out Of Position Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOutOfNetConfidenceDistanceMin.BindWithDefault("OutOfNet Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOutOfNetConfidenceDistanceMax.BindWithDefault("OutOfNet Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fUpfieldMaxDistance.BindWithDefault("Upfield Max Distance", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fDownfieldMaxDistance.BindWithDefault("Downfield Max Distance", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fClosingSpeedMax.BindWithDefault("Closing Max Speed", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSeparatingSpeedMax.BindWithDefault("Separating Max Speed", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFrontOfNetMidAngle.BindWithDefault("InFrontOfNet Mid Angle", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFrontOfNetMaxAngle.BindWithDefault("InFrontOfNet Max Angle", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFrontOfNetMidScore.BindWithDefault("InFrontOfNet Mid Score", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGetOpenPassLaneOffsetMin.BindWithDefault("OpenToPosition Pass Lane Offset Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGetOpenPassLaneOffsetMax.BindWithDefault("OpenToPosition Pass Lane Offset Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGetOpenPassLaneDistMin.BindWithDefault("OpenToPosition Pass Lane Dist Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGetOpenPassLaneDistMax.BindWithDefault("OpenToPosition Pass Lane Dist Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOpenRadiusMin.BindWithDefault("OpenPosition Radius Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOpenRadiusMax.BindWithDefault("OpenPosition Radius Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fWideOpenRadiusMin.BindWithDefault("WideOpen Radius Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fWideOpenRadiusMax.BindWithDefault("WideOpen Radius Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInBetweenInterceptRangeMin.BindWithDefault("InBetween Intercept Range Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInBetweenInterceptRangeMax.BindWithDefault("InBetween Intercept Range Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInBetweenConeWidthMin.BindWithDefault("InBetween Cone Width Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fInBetweenConeWidthMax.BindWithDefault("InBetween Cone Width Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPassDeadZone.BindWithDefault("Pass Dead Thought Zone", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fLosingScoreDelta.BindWithDefault("Losing Score Delta", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fWinningScoreDelta.BindWithDefault("Winning Score Delta", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTiedScoreDelta.BindWithDefault("Tied Score Delta", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGameTimeCloseToOver.BindWithDefault("GameTime CloseTo Over", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGameTimeNearlyOver.BindWithDefault("GameTime Nearly Over", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGameTimeFarFromOver.BindWithDefault("GameTime FarFrom Over", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fDefensiveConfidenceDistancesMin.BindWithDefault("Defensive Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fDefensiveConfidenceDistancesMax.BindWithDefault("Defensive Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOffensiveConfidenceDistancesMin.BindWithDefault("Offensive Min Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOffensiveConfidenceDistancesMax.BindWithDefault("Offensive Max Dist", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fStallingTimeEasyMin.BindWithDefault("Stalling Time Easy Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fStallingTimeEasyMax.BindWithDefault("Stalling Time Easy Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fStallingTimeHardMin.BindWithDefault("Stalling Time Hard Min", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fStallingTimeHardMax.BindWithDefault("Stalling Time Hard Max", -9999.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
}
