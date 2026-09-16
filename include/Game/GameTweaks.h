#ifndef GAME_GAME_TWEAKS_H
#define GAME_GAME_TWEAKS_H

#include "Game/CharacterTweaks.h"
#include "Game/AI/SkillTweaks.h"

struct TerrainTweaks;

class GameTweaks : public TweaksBase
{
public:
    GameTweaks(const char* name, const char* category);
    virtual ~GameTweaks();
    virtual void Init();
    void RegisterTweaks(bool registerTweaks);

public:
    /* 0x044 */ TweakFloatBinding fGameDuration;
    /* 0x054 */ TweakFloatBinding fFielderAttributeWeight;
    /* 0x064 */ TweakFloatBinding vGetInPositionKeyFielderDistX;
    /* 0x074 */ TweakFloatBinding vGetInPositionKeyFielderDistY;
    /* 0x084 */ TweakFloatBinding vGetInPositionInRadiusX;
    /* 0x094 */ TweakFloatBinding vGetInPositionInRadiusY;
    /* 0x0A4 */ TweakFloatBinding vGetInPositionOutRadiusX;
    /* 0x0B4 */ TweakFloatBinding vGetInPositionOutRadiusY;
    /* 0x0C4 */ TweakFloatBinding nStrafeToRunInDirectionDelta;
    /* 0x0D4 */ TweakFloatBinding nBackwardsToStrafeRunInDirectionDelta;
    /* 0x0E4 */ TweakFloatBinding nStrafeToRunOutDirectionDelta;
    /* 0x0F4 */ TweakFloatBinding nBackwardsToStrafeRunOutDirectionDelta;
    /* 0x104 */ TweakFloatBinding fSlideAttackRadius;
    /* 0x114 */ TweakFloatBinding fArrivalInRadius;
    /* 0x124 */ TweakFloatBinding fArrivalOutRadius;
    /* 0x134 */ TweakFloatBinding fNearSeekInRadius;
    /* 0x144 */ TweakFloatBinding fNearSeekOutRadius;
    /* 0x154 */ TweakFloatBinding fSwapControllerTime;
    /* 0x164 */ TweakFloatBinding fSwapFacingTime;
    /* 0x174 */ TweakFloatBinding fFreezeShellFrozenTime;
    /* 0x184 */ TweakFloatBinding fBananaActiveTime;
    /* 0x194 */ TweakFloatBinding fBobombActiveTime;
    /* 0x1A4 */ TweakFloatBinding fChainChompActiveTime;
    /* 0x1B4 */ TweakFloatBinding fChainChompFallTime;
    /* 0x1C4 */ TweakFloatBinding fChainChompSpeed;
    /* 0x1D4 */ TweakIntBinding nScoreDifferenceMaximum;
    /* 0x1E4 */ TweakIntBinding nScoreDifferenceMinimum;
    /* 0x1F4 */ TweakIntBinding nPowerupsNumForPenalty;
    /* 0x204 */ TweakFloatBinding fBananaResistance;
    /* 0x214 */ TweakFloatBinding fShellBounceGround;
    /* 0x224 */ TweakFloatBinding fShellBounce;
    /* 0x234 */ TweakFloatBinding fShellSmallRadius;
    /* 0x244 */ TweakFloatBinding fShellMediumRadius;
    /* 0x254 */ TweakFloatBinding fShellBigRadius;
    /* 0x264 */ TweakFloatBinding fShellMediumChance;
    /* 0x274 */ TweakFloatBinding fShellBigChance;
    /* 0x284 */ TweakFloatBinding fShellFiveChance;
    /* 0x294 */ TweakFloatBinding fShellThreeChance;
    /* 0x2A4 */ TweakFloatBinding fShellExplodeChance;
    /* 0x2B4 */ TweakFloatBinding fBananaSmallRadius;
    /* 0x2C4 */ TweakFloatBinding fBananaMediumRadius;
    /* 0x2D4 */ TweakFloatBinding fBananaBigRadius;
    /* 0x2E4 */ TweakFloatBinding fBananaMediumChance;
    /* 0x2F4 */ TweakFloatBinding fBananaBigChance;
    /* 0x304 */ TweakFloatBinding fBananaFiveChance;
    /* 0x314 */ TweakFloatBinding fBananaThreeChance;
    /* 0x324 */ TweakFloatBinding fBananaExplodeChance;
    /* 0x334 */ TweakFloatBinding fBobombSmallRadius;
    /* 0x344 */ TweakFloatBinding fBobombMediumRadius;
    /* 0x354 */ TweakFloatBinding fBobombBigRadius;
    /* 0x364 */ TweakFloatBinding fBobombMediumChance;
    /* 0x374 */ TweakFloatBinding fBobombBigChance;
    /* 0x384 */ TweakFloatBinding fBobombFiveChance;
    /* 0x394 */ TweakFloatBinding fBobombThreeChance;
    /* 0x3A4 */ TweakFloatBinding fBobombMineChance;
    /* 0x3B4 */ TweakFloatBinding fPowerupExplosionRadiusMin;
    /* 0x3C4 */ TweakFloatBinding fPowerupExplosionRadiusMax;
    /* 0x3D4 */ TweakFloatBinding fPowerupArrowThrowChance;
    /* 0x3E4 */ TweakFloatBinding fPowerupSpreadThrowChance;
    /* 0x3F4 */ TweakFloatBinding fPowerupSurroundThrowChance;
    /* 0x404 */ TweakFloatBinding fPowerupHorizontalLineThrowChance;
    /* 0x414 */ TweakFloatBinding fPowerupHitWithBallMinAmount;
    /* 0x424 */ TweakFloatBinding fPowerupHitWithBallMaxAmount;
    /* 0x434 */ TweakFloatBinding fPowerupHitNoBallMinAmount;
    /* 0x444 */ TweakFloatBinding fPowerupHitNoBallMaxAmount;
    /* 0x454 */ TweakFloatBinding fPowerupSlideWithBallMinAmount;
    /* 0x464 */ TweakFloatBinding fPowerupSlideWithBallMaxAmount;
    /* 0x474 */ TweakFloatBinding fPowerupSlideNoBallMinAmount;
    /* 0x484 */ TweakFloatBinding fPowerupSlideNoBallMaxAmount;
    /* 0x494 */ TweakFloatBinding fPowerupPowerShotMinAmount;
    /* 0x4A4 */ TweakFloatBinding fPowerupPowerShotMaxAmount;
    /* 0x4B4 */ TweakFloatBinding fPowerupInterceptPassMinAmount;
    /* 0x4C4 */ TweakFloatBinding fPowerupInterceptPassMaxAmount;
    /* 0x4D4 */ TweakFloatBinding fPowerupPerfectPassMinAmount;
    /* 0x4E4 */ TweakFloatBinding fPowerupPerfectPassMaxAmount;
    /* 0x4F4 */ TweakFloatBinding fPowerupContextDekeMinAmount;
    /* 0x504 */ TweakFloatBinding fPowerupContextDekeMaxAmount;
    /* 0x514 */ TweakFloatBinding fPowerupIconSpeed;
    /* 0x524 */ TweakIntBinding nPowerupBoxMaxActiveOnField;
    /* 0x534 */ TweakFloatBinding fPowerupBoxDelayDropTime;
    /* 0x544 */ TweakFloatBinding fChainChompRadius;
    /* 0x554 */ TweakFloatBinding fDrawPowerupIconTime;
    /* 0x564 */ TweakFloatBinding fHitAngleWeighting;
    /* 0x574 */ TweakFloatBinding fAngleWeighting;
    /* 0x584 */ TweakFloatBinding fDekeAngleWeighting;
    /* 0x594 */ TweakFloatBinding fPassDistanceWeight;
    /* 0x5A4 */ TweakFloatBinding fPassIdealMinDistance;
    /* 0x5B4 */ TweakFloatBinding fPassIdealMinRange;
    /* 0x5C4 */ TweakFloatBinding fPassIdealMaxDistance;
    /* 0x5D4 */ TweakFloatBinding fPassIdealMaxRange;
    /* 0x5E4 */ TweakIntBinding nPassMinimumAngle;
    /* 0x5F4 */ TweakIntBinding nPassMaximumAngle;
    /* 0x604 */ TweakFloatBinding fVolleyPassDistanceWeight;
    /* 0x614 */ TweakFloatBinding fVolleyPassIdealMinDistance;
    /* 0x624 */ TweakFloatBinding fVolleyPassIdealMinRange;
    /* 0x634 */ TweakFloatBinding fVolleyPassIdealMaxDistance;
    /* 0x644 */ TweakFloatBinding fVolleyPassIdealMaxRange;
    /* 0x654 */ TweakIntBinding nVolleyPassMinimumAngle;
    /* 0x664 */ TweakIntBinding nVolleyPassMaximumAngle;
    /* 0x674 */ TweakFloatBinding fPerfectPassSlowMo;
    /* 0x684 */ TweakFloatBinding fSkillshotSlowMoRate;
    /* 0x694 */ TweakFloatBinding fSkillshotSlowMoTime;
    /* 0x6A4 */ TweakFloatBinding fSkillshotSlowMoDuration;
    /* 0x6B4 */ TweakFloatBinding fSkillshotSlowMoRamp;
    /* 0x6C4 */ TweakFloatBinding fFrontAudibleSurrDist;
    /* 0x6D4 */ TweakFloatBinding fBackAudibleSurrDist;
    /* 0x6E4 */ TweakFloatBinding fSpeedOfSoundForDoppler;
    /* 0x6F4 */ TweakFloatBinding fEmitterDistFromListenerMaxVol;
    /* 0x704 */ TweakFloatBinding fMaxAudibleEmitterDistance;
    /* 0x714 */ TweakFloatBinding fEmitterVolToDistanceValue;
    /* 0x724 */ TweakFloatBinding fFadeFilterSlowMoInTime;
    /* 0x734 */ TweakFloatBinding fFadeFilterSlowMoOutTime;
    /* 0x744 */ TweakFloatBinding fFadeFilterFreqMin;
    /* 0x754 */ TweakFloatBinding fFadeFilterFreqMax;
    /* 0x764 */ TweakFloatBinding fFadeFilterDropoffDelayTime;
    /* 0x774 */ TweakFloatBinding fFadeFilterDropoffTime;
    /* 0x784 */ TweakFloatBinding fFadePitchMin;
    /* 0x794 */ TweakFloatBinding fFadePitchMax;
    /* 0x7A4 */ TweakFloatBinding fMinBobombMoveSFXTime;
    /* 0x7B4 */ TweakFloatBinding fPerfectPassProximityFilterDistSq;
    /* 0x7C4 */ TweakFloatBinding fPowerupSmallSizeVolCoeff;
    /* 0x7D4 */ TweakFloatBinding fPowerupMedSizeVolCoeff;
    /* 0x7E4 */ TweakFloatBinding fPowerupLargeSizeVolCoeff;
    /* 0x7F4 */ TweakFloatBinding fFadePerfectPassTrailSFXStartTime;
    /* 0x804 */ TweakFloatBinding fMinHitIntensityForHardBodyHitSFX;
    /* 0x814 */ TweakFloatBinding fSlideAttackHitReactionVolume;
    /* 0x824 */ TweakFloatBinding fShootToScoreBallHitReactionVolume;
    /* 0x834 */ TweakFloatBinding fBombHitReactionVolume;
    /* 0x844 */ TweakFloatBinding fBombShockwaveReactionVolume;
    /* 0x854 */ TweakFloatBinding fSmallShellHitReactionVolume;
    /* 0x864 */ TweakFloatBinding fMediumShellHitReactionVolume;
    /* 0x874 */ TweakFloatBinding fLargeShellHitReactionVolume;
    /* 0x884 */ TweakFloatBinding mUnidentified884;
    /* 0x894 */ TweakFloatBinding fGoalieDropKickHitReactionVolume;
    /* 0x8A4 */ TweakFloatBinding fBallHitWallMaxAudibleVelocity;
    /* 0x8B4 */ TweakFloatBinding fBallHitWallMinAudibleVelocity;
    /* 0x8C4 */ TweakFloatBinding fBallHitWallMinVolume;
    /* 0x8D4 */ TweakFloatBinding fBallHitWallMinTimeBeforeNextAudio;
    /* 0x8E4 */ TweakFloatBinding fBallHitNetMaxAudibleVelocity;
    /* 0x8F4 */ TweakFloatBinding fBallHitNetMinAudibleVelocity;
    /* 0x904 */ TweakFloatBinding fBallHitNetMinVolume;
    /* 0x914 */ TweakFloatBinding fBallHitNetMinTimeBeforeNextAudio;
    /* 0x924 */ TweakFloatBinding fShootToScoreBallBlurWidth;
    /* 0x934 */ TweakIntBinding nShootToScoreBallBlurLength;
    /* 0x944 */ TweakFloatBinding fLeftTriggerDownPressure;
    /* 0x954 */ TweakFloatBinding fIndicatorDistAboveHead;
    /* 0x964 */ TweakFloatBinding fIndicatorDistInPixels;
    /* 0x974 */ TweakFloatBinding fShotPostOffset;
    /* 0x984 */ TweakFloatBinding fShotHeightOffsetFromPost;
    /* 0x994 */ TweakFloatBinding fShotWidthVariance;
    /* 0x9A4 */ TweakFloatBinding fShotHeightVariance;
    /* 0x9B4 */ const char* mCategory;
}; // total size: 0x9B8

inline GameTweaks::~GameTweaks()
{
}

struct GameTweaksManager
{
    GameTweaksManager()
        : mTerrainType(1)
        , mTerrainTweaks(0)
        , mUnidentified08(0)
        , mUnidentified0C(false)
        , m_pGameTweaks(0)
        , mFielderTweaks(0)
    {
        mUnidentified18[0] = 0;
        mUnidentified18[1] = 0;
    }

public:
    /* 0x00 */ u32 mTerrainType;

    /* 0x04 */ TerrainTweaks* mTerrainTweaks;

    /* 0x08 */ int mUnidentified08;
    /* 0x0C */ bool mUnidentified0C;
    /* 0x0D */ u8 mPadding0D[3];

    /* 0x10 */ GameTweaks* m_pGameTweaks;
    /* 0x14 */ FielderTweaks* mFielderTweaks;

public:
    /* 0x18 */ SkillTweaks* mUnidentified18[2];
}; // total size: 0x20

extern GameTweaksManager gGameTweaks;

void InitializeGameTweaks(GameTweaksManager* state);
bool UpdateGameTweaksLoading(GameTweaksManager* state);
void DestroyGameTweaks(GameTweaksManager* state);

#endif // GAME_GAME_TWEAKS_H
