#include "Game/ScriptTuning.h"

#include "types.h"

extern "C" void fn_80073A48(void*, const char*, const char*);
extern "C" void fn_802C6CAC(const char*, const char*, bool);
extern "C" u8 lbl_8056BA00[];

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
        fn_80073A48(lbl_8056BA00, mszFileName, mUnidentified7B4);
    }
    else
    {
        fn_802C6CAC(mszFileName, mUnidentified7B4, true);
    }
}

void FuzzyTweaks::Init()
{
    mUnidentified044.fn_8002D078("Close 2 Teammate Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified054.fn_8002D078("Close 2 Teammate Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified064.fn_8002D078("Near 2 Teammate Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified074.fn_8002D078("Near 2 Teammate Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified084.fn_8002D078("Far 2 Teammate Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified094.fn_8002D078("Far 2 Teammate Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0A4.fn_8002D078("Close 2 Opponent Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0B4.fn_8002D078("Close 2 Opponent Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0C4.fn_8002D078("Near 2 Opponent Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0D4.fn_8002D078("Near 2 Opponent Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0E4.fn_8002D078("Far 2 Opponent Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0F4.fn_8002D078("Far 2 Opponent Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified104.fn_8002D078("ReallyClose 2 Ball Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified114.fn_8002D078("ReallyClose 2 Ball Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified124.fn_8002D078("Close 2 Ball Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified134.fn_8002D078("Close 2 Ball Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified144.fn_8002D078("Near 2 Ball Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified154.fn_8002D078("Near 2 Ball Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified164.fn_8002D078("Far 2 Ball Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified174.fn_8002D078("Far 2 Ball Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified184.fn_8002D078("Close 2 Net Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified194.fn_8002D078("Close 2 Net Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1A4.fn_8002D078("Near 2 Net Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1B4.fn_8002D078("Near 2 Net Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1C4.fn_8002D078("Far 2 Net Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1D4.fn_8002D078("Far 2 Net Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1E4.fn_8002D078("Ball 2 Net Close Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1F4.fn_8002D078("Ball 2 Net Close Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified204.fn_8002D078("Ball 2 Net Near Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified214.fn_8002D078("Ball 2 Net Near Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified224.fn_8002D078("Ball 2 Net Far Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified234.fn_8002D078("Ball 2 Net Far Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified244.fn_8002D078("Close 2 FormationPos Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified254.fn_8002D078("Close 2 FormationPos Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified264.fn_8002D078("Near 2 FormationPos Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified274.fn_8002D078("Near 2 FormationPos Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified284.fn_8002D078("Far 2 FormationPos Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified294.fn_8002D078("Far 2 FormationPos Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2A4.fn_8002D078("Close 2 Goalie Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2B4.fn_8002D078("Close 2 Goalie Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2C4.fn_8002D078("Near 2 Goalie Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2D4.fn_8002D078("Near 2 Goalie Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2E4.fn_8002D078("Far 2 Goalie Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2F4.fn_8002D078("Far 2 Goalie Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified304.fn_8002D078("Close 2 Sideline Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified314.fn_8002D078("Close 2 Sideline Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified324.fn_8002D078("Near 2 Sideline Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified334.fn_8002D078("Near 2 Sideline Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified344.fn_8002D078("Far 2 Sideline Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified354.fn_8002D078("Far 2 Sideline Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified364.fn_8002D078("Ball Height High Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified374.fn_8002D078("Ball Height High Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified384.fn_8002D078("Ball Height Really High Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified394.fn_8002D078("Ball Height Really High Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    nFacingFullConfidenceAngle.fn_800757B4("Facing Angle Max", -0x270F, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    nFacingNoConfidenceAngle.fn_800757B4("Facing Angle Min", -0x270F, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3C4.fn_8002D078("Ball Control Distance From Owner Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3D4.fn_8002D078("Ball Control Distance From Owner Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3E4.fn_8002D078("Pass Lane Max Width", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3F4.fn_8002D078("Shot Lane Max Width", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified404.fn_8002D078("Pass In Play Dist 2 Target Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified414.fn_8002D078("Shot In Play Dist 2 Target Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified424.fn_8002D078("Player OnGround Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified434.fn_8002D078("Player OnGround Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified444.fn_8002D078("InterceptBall SwapController Time Weight", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified454.fn_8002D078("InterceptBall Time Weight", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified464.fn_8002D078("InterceptBall Min Time", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified474.fn_8002D078("InterceptBall Max Time", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified484.fn_8002D078("InterceptBall Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified494.fn_8002D078("InterceptBall Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4A4.fn_8002D078("Pressured Near Weight", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4B4.fn_8002D078("Avoid Goalie Min Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4C4.fn_8002D078("Avoid Goalie Max Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4D4.fn_8002D078("Avoid Fielders Min Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4E4.fn_8002D078("Avoid Fielders Max Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified4F4.fn_8002D078("Avoid Powerups Min Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified504.fn_8002D078("Avoid Powerups Max Repulsion", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified514.fn_8002D078("Bad Shooter Distance Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified524.fn_8002D078("Bad Shooter Distance Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified534.fn_8002D078("Good Shooter Distance Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified544.fn_8002D078("Good Shooter Distance Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified554.fn_8002D078("Goalie Out Of Position Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified564.fn_8002D078("Goalie Out Of Position Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified574.fn_8002D078("OutOfNet Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified584.fn_8002D078("OutOfNet Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified594.fn_8002D078("Upfield Max Distance", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5A4.fn_8002D078("Downfield Max Distance", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5B4.fn_8002D078("Closing Max Speed", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5C4.fn_8002D078("Separating Max Speed", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5D4.fn_8002D078("InFrontOfNet Mid Angle", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5E4.fn_8002D078("InFrontOfNet Max Angle", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified5F4.fn_8002D078("InFrontOfNet Mid Score", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified604.fn_8002D078("OpenToPosition Pass Lane Offset Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified614.fn_8002D078("OpenToPosition Pass Lane Offset Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified624.fn_8002D078("OpenToPosition Pass Lane Dist Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified634.fn_8002D078("OpenToPosition Pass Lane Dist Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified644.fn_8002D078("OpenPosition Radius Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified654.fn_8002D078("OpenPosition Radius Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified664.fn_8002D078("WideOpen Radius Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified674.fn_8002D078("WideOpen Radius Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified684.fn_8002D078("InBetween Intercept Range Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified694.fn_8002D078("InBetween Intercept Range Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6A4.fn_8002D078("InBetween Cone Width Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6B4.fn_8002D078("InBetween Cone Width Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6C4.fn_8002D078("Pass Dead Thought Zone", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6D4.fn_8002D078("Losing Score Delta", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6E4.fn_8002D078("Winning Score Delta", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified6F4.fn_8002D078("Tied Score Delta", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified704.fn_8002D078("GameTime CloseTo Over", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified714.fn_8002D078("GameTime Nearly Over", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified724.fn_8002D078("GameTime FarFrom Over", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified734.fn_8002D078("Defensive Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified744.fn_8002D078("Defensive Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified754.fn_8002D078("Offensive Min Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified764.fn_8002D078("Offensive Max Dist", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified774.fn_8002D078("Stalling Time Easy Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified784.fn_8002D078("Stalling Time Easy Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified794.fn_8002D078("Stalling Time Hard Min", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
    mUnidentified7A4.fn_8002D078("Stalling Time Hard Max", -9999.9f, mUnidentified7B4, false, 0.0f, 0.0f, 0.0f);
}
