#include "Game/ScriptTuning.h"

#include "Game/tu_802C6224.h"
#include "types.h"
#include "unclassified/tu_80073898.h"

FuzzyTweaks::FuzzyTweaks(const char* name, const char* category)
    : TweaksBase(name)
    , mUnidentified7B4(category)
{
    Init();
    fn_800E1C98(true);
}

FuzzyTweaks::~FuzzyTweaks()
{
}

void FuzzyTweaks::fn_800E1C98(bool registerTweaks)
{
    if (registerTweaks)
    {
        fn_80073A48(&lbl_8056BA00, mszFileName, mUnidentified7B4);
    }
    else
    {
        fn_802C6CAC(mszFileName, mUnidentified7B4, true);
    }
}

void FuzzyTweaks::Init()
{
    mUnidentified044.BindWithDefault("Close 2 Teammate Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified054.BindWithDefault("Close 2 Teammate Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified064.BindWithDefault("Near 2 Teammate Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified074.BindWithDefault("Near 2 Teammate Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified084.BindWithDefault("Far 2 Teammate Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified094.BindWithDefault("Far 2 Teammate Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0A4.BindWithDefault("Close 2 Opponent Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0B4.BindWithDefault("Close 2 Opponent Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0C4.BindWithDefault("Near 2 Opponent Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0D4.BindWithDefault("Near 2 Opponent Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0E4.BindWithDefault("Far 2 Opponent Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0F4.BindWithDefault("Far 2 Opponent Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified104.BindWithDefault("ReallyClose 2 Ball Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified114.BindWithDefault("ReallyClose 2 Ball Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified124.BindWithDefault("Close 2 Ball Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified134.BindWithDefault("Close 2 Ball Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified144.BindWithDefault("Near 2 Ball Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified154.BindWithDefault("Near 2 Ball Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified164.BindWithDefault("Far 2 Ball Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified174.BindWithDefault("Far 2 Ball Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified184.BindWithDefault("Close 2 Net Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified194.BindWithDefault("Close 2 Net Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1A4.BindWithDefault("Near 2 Net Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1B4.BindWithDefault("Near 2 Net Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1C4.BindWithDefault("Far 2 Net Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1D4.BindWithDefault("Far 2 Net Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1E4.BindWithDefault("Ball 2 Net Close Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1F4.BindWithDefault("Ball 2 Net Close Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified204.BindWithDefault("Ball 2 Net Near Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified214.BindWithDefault("Ball 2 Net Near Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified224.BindWithDefault("Ball 2 Net Far Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified234.BindWithDefault("Ball 2 Net Far Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified244.BindWithDefault("Close 2 FormationPos Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified254.BindWithDefault("Close 2 FormationPos Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified264.BindWithDefault("Near 2 FormationPos Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified274.BindWithDefault("Near 2 FormationPos Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified284.BindWithDefault("Far 2 FormationPos Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified294.BindWithDefault("Far 2 FormationPos Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2A4.BindWithDefault("Close 2 Goalie Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2B4.BindWithDefault("Close 2 Goalie Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2C4.BindWithDefault("Near 2 Goalie Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2D4.BindWithDefault("Near 2 Goalie Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2E4.BindWithDefault("Far 2 Goalie Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2F4.BindWithDefault("Far 2 Goalie Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified304.BindWithDefault("Close 2 Sideline Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified314.BindWithDefault("Close 2 Sideline Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified324.BindWithDefault("Near 2 Sideline Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified334.BindWithDefault("Near 2 Sideline Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified344.BindWithDefault("Far 2 Sideline Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified354.BindWithDefault("Far 2 Sideline Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified364.BindWithDefault("Ball Height High Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified374.BindWithDefault("Ball Height High Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified384.BindWithDefault("Ball Height Really High Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified394.BindWithDefault("Ball Height Really High Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    nFacingFullConfidenceAngle.BindWithDefault("Facing Angle Max", -0x270F, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    nFacingNoConfidenceAngle.BindWithDefault("Facing Angle Min", -0x270F, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3C4.BindWithDefault("Ball Control Distance From Owner Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3D4.BindWithDefault("Ball Control Distance From Owner Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3E4.BindWithDefault("Pass Lane Max Width", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3F4.BindWithDefault("Shot Lane Max Width", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified404.BindWithDefault("Pass In Play Dist 2 Target Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified414.BindWithDefault("Shot In Play Dist 2 Target Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified424.BindWithDefault("Player OnGround Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified434.BindWithDefault("Player OnGround Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified444.BindWithDefault("InterceptBall SwapController Time Weight", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified454.BindWithDefault("InterceptBall Time Weight", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified464.BindWithDefault("InterceptBall Min Time", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified474.BindWithDefault("InterceptBall Max Time", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified484.BindWithDefault("InterceptBall Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified494.BindWithDefault("InterceptBall Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4A4.BindWithDefault("Pressured Near Weight", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4B4.BindWithDefault("Avoid Goalie Min Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4C4.BindWithDefault("Avoid Goalie Max Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4D4.BindWithDefault("Avoid Fielders Min Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4E4.BindWithDefault("Avoid Fielders Max Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4F4.BindWithDefault("Avoid Powerups Min Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified504.BindWithDefault("Avoid Powerups Max Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified514.BindWithDefault("Bad Shooter Distance Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified524.BindWithDefault("Bad Shooter Distance Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified534.BindWithDefault("Good Shooter Distance Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified544.BindWithDefault("Good Shooter Distance Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified554.BindWithDefault("Goalie Out Of Position Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified564.BindWithDefault("Goalie Out Of Position Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified574.BindWithDefault("OutOfNet Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified584.BindWithDefault("OutOfNet Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified594.BindWithDefault("Upfield Max Distance", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5A4.BindWithDefault("Downfield Max Distance", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5B4.BindWithDefault("Closing Max Speed", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5C4.BindWithDefault("Separating Max Speed", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5D4.BindWithDefault("InFrontOfNet Mid Angle", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5E4.BindWithDefault("InFrontOfNet Max Angle", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5F4.BindWithDefault("InFrontOfNet Mid Score", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified604.BindWithDefault("OpenToPosition Pass Lane Offset Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified614.BindWithDefault("OpenToPosition Pass Lane Offset Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified624.BindWithDefault("OpenToPosition Pass Lane Dist Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified634.BindWithDefault("OpenToPosition Pass Lane Dist Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified644.BindWithDefault("OpenPosition Radius Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified654.BindWithDefault("OpenPosition Radius Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified664.BindWithDefault("WideOpen Radius Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified674.BindWithDefault("WideOpen Radius Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified684.BindWithDefault("InBetween Intercept Range Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified694.BindWithDefault("InBetween Intercept Range Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6A4.BindWithDefault("InBetween Cone Width Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6B4.BindWithDefault("InBetween Cone Width Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6C4.BindWithDefault("Pass Dead Thought Zone", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6D4.BindWithDefault("Losing Score Delta", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6E4.BindWithDefault("Winning Score Delta", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6F4.BindWithDefault("Tied Score Delta", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified704.BindWithDefault("GameTime CloseTo Over", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified714.BindWithDefault("GameTime Nearly Over", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified724.BindWithDefault("GameTime FarFrom Over", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified734.BindWithDefault("Defensive Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified744.BindWithDefault("Defensive Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified754.BindWithDefault("Offensive Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified764.BindWithDefault("Offensive Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified774.BindWithDefault("Stalling Time Easy Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified784.BindWithDefault("Stalling Time Easy Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified794.BindWithDefault("Stalling Time Hard Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified7A4.BindWithDefault("Stalling Time Hard Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
}
