#ifndef GAME_GAME_TWEAKS_H
#define GAME_GAME_TWEAKS_H

#include "Game/CharacterTweaks.h"
#include "Game/AI/SkillTweaks.h"

struct TweakVector2
{
    TweakValueImpl_804F4DC8 x;
    TweakValueImpl_804F4DC8 y;
};

class GameTweaks : public TweaksBase
{
public:
    GameTweaks(const char* name, const char* category);
    virtual ~GameTweaks();
    virtual void Init();
    void RegisterTweaks(bool registerTweaks);

public:
    /* 0x044 */ TweakValueImpl_804F4DC8 fGameDuration;
    /* 0x054 */ TweakValueImpl_804F4DC8 fFielderAttributeWeight;
    /* 0x064 */ TweakVector2 vGetInPositionKeyFielderDist;
    /* 0x084 */ TweakVector2 vGetInPositionInRadius;
    /* 0x0A4 */ TweakVector2 vGetInPositionOutRadius;
    /* 0x0C4 */ TweakValueImpl_804F4DC8 nStrafeToRunInDirectionDelta;
    /* 0x0D4 */ TweakValueImpl_804F4DC8 nBackwardsToStrafeRunInDirectionDelta;
    /* 0x0E4 */ TweakValueImpl_804F4DC8 nStrafeToRunOutDirectionDelta;
    /* 0x0F4 */ TweakValueImpl_804F4DC8 nBackwardsToStrafeRunOutDirectionDelta;
    /* 0x104 */ TweakValueImpl_804F4DC8 fSlideAttackRadius;
    /* 0x114 */ TweakValueImpl_804F4DC8 fArrivalInRadius;
    /* 0x124 */ TweakValueImpl_804F4DC8 fArrivalOutRadius;
    /* 0x134 */ TweakValueImpl_804F4DC8 fNearSeekInRadius;
    /* 0x144 */ TweakValueImpl_804F4DC8 fNearSeekOutRadius;
    /* 0x154 */ TweakValueImpl_804F4DC8 fSwapControllerTime;
    /* 0x164 */ TweakValueImpl_804F4DC8 fSwapFacingTime;
    /* 0x174 */ TweakValueImpl_804F4DC8 fFreezeShellFrozenTime;
    /* 0x184 */ TweakValueImpl_804F4DC8 fBananaActiveTime;
    /* 0x194 */ TweakValueImpl_804F4DC8 fBobombActiveTime;
    /* 0x1A4 */ TweakValueImpl_804F4DC8 fChainChompActiveTime;
    /* 0x1B4 */ TweakValueImpl_804F4DC8 fChainChompFallTime;
    /* 0x1C4 */ TweakValueImpl_804F4DC8 fChainChompSpeed;
    /* 0x1D4 */ TweakValueIntImpl_804FD898 nScoreDifferenceMaximum;
    /* 0x1E4 */ TweakValueIntImpl_804FD898 nScoreDifferenceMinimum;
    /* 0x1F4 */ TweakValueIntImpl_804FD898 nPowerupsNumForPenalty;
    /* 0x204 */ TweakValueImpl_804F4DC8 fBananaResistance;
    /* 0x214 */ TweakValueImpl_804F4DC8 fShellBounceGround;
    /* 0x224 */ TweakValueImpl_804F4DC8 fShellBounce;
    /* 0x234 */ TweakValueImpl_804F4DC8 fShellSmallRadius;
    /* 0x244 */ TweakValueImpl_804F4DC8 fShellMediumRadius;
    /* 0x254 */ TweakValueImpl_804F4DC8 fShellBigRadius;
    /* 0x264 */ TweakValueImpl_804F4DC8 fShellMediumChance;
    /* 0x274 */ TweakValueImpl_804F4DC8 fShellBigChance;
    /* 0x284 */ TweakValueImpl_804F4DC8 fShellFiveChance;
    /* 0x294 */ TweakValueImpl_804F4DC8 fShellThreeChance;
    /* 0x2A4 */ TweakValueImpl_804F4DC8 fShellExplodeChance;
    /* 0x2B4 */ TweakValueImpl_804F4DC8 fBananaSmallRadius;
    /* 0x2C4 */ TweakValueImpl_804F4DC8 fBananaMediumRadius;
    /* 0x2D4 */ TweakValueImpl_804F4DC8 fBananaBigRadius;
    /* 0x2E4 */ TweakValueImpl_804F4DC8 fBananaMediumChance;
    /* 0x2F4 */ TweakValueImpl_804F4DC8 fBananaBigChance;
    /* 0x304 */ TweakValueImpl_804F4DC8 fBananaFiveChance;
    /* 0x314 */ TweakValueImpl_804F4DC8 fBananaThreeChance;
    /* 0x324 */ TweakValueImpl_804F4DC8 fBananaExplodeChance;
    /* 0x334 */ TweakValueImpl_804F4DC8 fBobombSmallRadius;
    /* 0x344 */ TweakValueImpl_804F4DC8 fBobombMediumRadius;
    /* 0x354 */ TweakValueImpl_804F4DC8 fBobombBigRadius;
    /* 0x364 */ TweakValueImpl_804F4DC8 fBobombMediumChance;
    /* 0x374 */ TweakValueImpl_804F4DC8 fBobombBigChance;
    /* 0x384 */ TweakValueImpl_804F4DC8 fBobombFiveChance;
    /* 0x394 */ TweakValueImpl_804F4DC8 fBobombThreeChance;
    /* 0x3A4 */ TweakValueImpl_804F4DC8 fBobombMineChance;
    /* 0x3B4 */ TweakValueImpl_804F4DC8 fPowerupExplosionRadiusMin;
    /* 0x3C4 */ TweakValueImpl_804F4DC8 fPowerupExplosionRadiusMax;
    /* 0x3D4 */ TweakValueImpl_804F4DC8 fPowerupArrowThrowChance;
    /* 0x3E4 */ TweakValueImpl_804F4DC8 fPowerupSpreadThrowChance;
    /* 0x3F4 */ TweakValueImpl_804F4DC8 fPowerupSurroundThrowChance;
    /* 0x404 */ TweakValueImpl_804F4DC8 fPowerupHorizontalLineThrowChance;
    /* 0x414 */ TweakValueImpl_804F4DC8 fPowerupHitWithBallMinAmount;
    /* 0x424 */ TweakValueImpl_804F4DC8 fPowerupHitWithBallMaxAmount;
    /* 0x434 */ TweakValueImpl_804F4DC8 fPowerupHitNoBallMinAmount;
    /* 0x444 */ TweakValueImpl_804F4DC8 fPowerupHitNoBallMaxAmount;
    /* 0x454 */ TweakValueImpl_804F4DC8 fPowerupSlideWithBallMinAmount;
    /* 0x464 */ TweakValueImpl_804F4DC8 fPowerupSlideWithBallMaxAmount;
    /* 0x474 */ TweakValueImpl_804F4DC8 fPowerupSlideNoBallMinAmount;
    /* 0x484 */ TweakValueImpl_804F4DC8 fPowerupSlideNoBallMaxAmount;
    /* 0x494 */ TweakValueImpl_804F4DC8 fPowerupPowerShotMinAmount;
    /* 0x4A4 */ TweakValueImpl_804F4DC8 fPowerupPowerShotMaxAmount;
    /* 0x4B4 */ TweakValueImpl_804F4DC8 fPowerupInterceptPassMinAmount;
    /* 0x4C4 */ TweakValueImpl_804F4DC8 fPowerupInterceptPassMaxAmount;
    /* 0x4D4 */ TweakValueImpl_804F4DC8 fPowerupPerfectPassMinAmount;
    /* 0x4E4 */ TweakValueImpl_804F4DC8 fPowerupPerfectPassMaxAmount;
    /* 0x4F4 */ TweakValueImpl_804F4DC8 fPowerupContextDekeMinAmount;
    /* 0x504 */ TweakValueImpl_804F4DC8 fPowerupContextDekeMaxAmount;
    /* 0x514 */ TweakValueImpl_804F4DC8 fPowerupIconSpeed;
    /* 0x524 */ TweakValueIntImpl_804FD898 nPowerupBoxMaxActiveOnField;
    /* 0x534 */ TweakValueImpl_804F4DC8 fPowerupBoxDelayDropTime;
    /* 0x544 */ TweakValueImpl_804F4DC8 fChainChompRadius;
    /* 0x554 */ TweakValueImpl_804F4DC8 fDrawPowerupIconTime;
    /* 0x564 */ TweakValueImpl_804F4DC8 fHitAngleWeighting;
    /* 0x574 */ TweakValueImpl_804F4DC8 fAngleWeighting;
    /* 0x584 */ TweakValueImpl_804F4DC8 fDekeAngleWeighting;
    /* 0x594 */ TweakValueImpl_804F4DC8 fPassDistanceWeight;
    /* 0x5A4 */ TweakValueImpl_804F4DC8 fPassIdealMinDistance;
    /* 0x5B4 */ TweakValueImpl_804F4DC8 fPassIdealMinRange;
    /* 0x5C4 */ TweakValueImpl_804F4DC8 fPassIdealMaxDistance;
    /* 0x5D4 */ TweakValueImpl_804F4DC8 fPassIdealMaxRange;
    /* 0x5E4 */ TweakValueIntImpl_804FD898 nPassMinimumAngle;
    /* 0x5F4 */ TweakValueIntImpl_804FD898 nPassMaximumAngle;
    /* 0x604 */ TweakValueImpl_804F4DC8 fVolleyPassDistanceWeight;
    /* 0x614 */ TweakValueImpl_804F4DC8 fVolleyPassIdealMinDistance;
    /* 0x624 */ TweakValueImpl_804F4DC8 fVolleyPassIdealMinRange;
    /* 0x634 */ TweakValueImpl_804F4DC8 fVolleyPassIdealMaxDistance;
    /* 0x644 */ TweakValueImpl_804F4DC8 fVolleyPassIdealMaxRange;
    /* 0x654 */ TweakValueIntImpl_804FD898 nVolleyPassMinimumAngle;
    /* 0x664 */ TweakValueIntImpl_804FD898 nVolleyPassMaximumAngle;
    /* 0x674 */ TweakValueImpl_804F4DC8 fPerfectPassSlowMo;
    /* 0x684 */ TweakValueImpl_804F4DC8 fSkillshotSlowMoRate;
    /* 0x694 */ TweakValueImpl_804F4DC8 fSkillshotSlowMoTime;
    /* 0x6A4 */ TweakValueImpl_804F4DC8 fSkillshotSlowMoDuration;
    /* 0x6B4 */ TweakValueImpl_804F4DC8 fSkillshotSlowMoRamp;
    /* 0x6C4 */ TweakValueImpl_804F4DC8 fFrontAudibleSurrDist;
    /* 0x6D4 */ TweakValueImpl_804F4DC8 fBackAudibleSurrDist;
    /* 0x6E4 */ TweakValueImpl_804F4DC8 fSpeedOfSoundForDoppler;
    /* 0x6F4 */ TweakValueImpl_804F4DC8 fEmitterDistFromListenerMaxVol;
    /* 0x704 */ TweakValueImpl_804F4DC8 fMaxAudibleEmitterDistance;
    /* 0x714 */ TweakValueImpl_804F4DC8 fEmitterVolToDistanceValue;
    /* 0x724 */ TweakValueImpl_804F4DC8 fFadeFilterSlowMoInTime;
    /* 0x734 */ TweakValueImpl_804F4DC8 fFadeFilterSlowMoOutTime;
    /* 0x744 */ TweakValueImpl_804F4DC8 fFadeFilterFreqMin;
    /* 0x754 */ TweakValueImpl_804F4DC8 fFadeFilterFreqMax;
    /* 0x764 */ TweakValueImpl_804F4DC8 fFadeFilterDropoffDelayTime;
    /* 0x774 */ TweakValueImpl_804F4DC8 fFadeFilterDropoffTime;
    /* 0x784 */ TweakValueImpl_804F4DC8 fFadePitchMin;
    /* 0x794 */ TweakValueImpl_804F4DC8 fFadePitchMax;
    /* 0x7A4 */ TweakValueImpl_804F4DC8 fMinBobombMoveSFXTime;
    /* 0x7B4 */ TweakValueImpl_804F4DC8 fPerfectPassProximityFilterDistSq;
    /* 0x7C4 */ TweakValueImpl_804F4DC8 fPowerupSmallSizeVolCoeff;
    /* 0x7D4 */ TweakValueImpl_804F4DC8 fPowerupMedSizeVolCoeff;
    /* 0x7E4 */ TweakValueImpl_804F4DC8 fPowerupLargeSizeVolCoeff;
    /* 0x7F4 */ TweakValueImpl_804F4DC8 fFadePerfectPassTrailSFXStartTime;
    /* 0x804 */ TweakValueImpl_804F4DC8 fMinHitIntensityForHardBodyHitSFX;
    /* 0x814 */ TweakValueImpl_804F4DC8 fSlideAttackHitReactionVolume;
    /* 0x824 */ TweakValueImpl_804F4DC8 fShootToScoreBallHitReactionVolume;
    /* 0x834 */ TweakValueImpl_804F4DC8 fBombHitReactionVolume;
    /* 0x844 */ TweakValueImpl_804F4DC8 fBombShockwaveReactionVolume;
    /* 0x854 */ TweakValueImpl_804F4DC8 fSmallShellHitReactionVolume;
    /* 0x864 */ TweakValueImpl_804F4DC8 fMediumShellHitReactionVolume;
    /* 0x874 */ TweakValueImpl_804F4DC8 fLargeShellHitReactionVolume;
    /* 0x884 */ TweakValueImpl_804F4DC8 mUnidentified884;
    /* 0x894 */ TweakValueImpl_804F4DC8 fGoalieDropKickHitReactionVolume;
    /* 0x8A4 */ TweakValueImpl_804F4DC8 fBallHitWallMaxAudibleVelocity;
    /* 0x8B4 */ TweakValueImpl_804F4DC8 fBallHitWallMinAudibleVelocity;
    /* 0x8C4 */ TweakValueImpl_804F4DC8 fBallHitWallMinVolume;
    /* 0x8D4 */ TweakValueImpl_804F4DC8 fBallHitWallMinTimeBeforeNextAudio;
    /* 0x8E4 */ TweakValueImpl_804F4DC8 fBallHitNetMaxAudibleVelocity;
    /* 0x8F4 */ TweakValueImpl_804F4DC8 fBallHitNetMinAudibleVelocity;
    /* 0x904 */ TweakValueImpl_804F4DC8 fBallHitNetMinVolume;
    /* 0x914 */ TweakValueImpl_804F4DC8 fBallHitNetMinTimeBeforeNextAudio;
    /* 0x924 */ TweakValueImpl_804F4DC8 fShootToScoreBallBlurWidth;
    /* 0x934 */ TweakValueIntImpl_804FD898 nShootToScoreBallBlurLength;
    /* 0x944 */ TweakValueImpl_804F4DC8 fLeftTriggerDownPressure;
    /* 0x954 */ TweakValueImpl_804F4DC8 fIndicatorDistAboveHead;
    /* 0x964 */ TweakValueImpl_804F4DC8 fIndicatorDistInPixels;
    /* 0x974 */ TweakValueImpl_804F4DC8 fShotPostOffset;
    /* 0x984 */ TweakValueImpl_804F4DC8 fShotHeightOffsetFromPost;
    /* 0x994 */ TweakValueImpl_804F4DC8 fShotWidthVariance;
    /* 0x9A4 */ TweakValueImpl_804F4DC8 fShotHeightVariance;
    /* 0x9B4 */ const char* mCategory;
}; // total size: 0x9B8

struct unk_800A9274
{
    virtual ~unk_800A9274();

    /* 0x04 */ TweakValueImpl_804F4DC8 mUnidentified04;
    /* 0x14 */ TweakValueImpl_804F4DC8 mUnidentified14;
    /* 0x24 */ TweakValueImpl_804F4DC8 mUnidentified24;
    /* 0x34 */ TweakValueImpl_804F4DC8 mUnidentified34;
}; // total size: 0x44

struct unk_8056CF08
{
    unk_8056CF08()
        : mUnidentified00(1)
        , mUnidentified04(0)
        , mUnidentified08(0)
        , mUnidentified0C(false)
        , m_pGameTweaks(0)
        , m_unk14(0)
    {
        mUnidentified18[0] = 0;
        mUnidentified18[1] = 0;
    }

public:
    /* 0x00 */ u32 mUnidentified00;

    /* 0x04 */ unk_800A9274* mUnidentified04;

    /* 0x08 */ int mUnidentified08;
    /* 0x0C */ bool mUnidentified0C;
    /* 0x0D */ u8 mPadding0D[3];

    /* 0x10 */ GameTweaks* m_pGameTweaks;
    /* 0x14 */ FielderTweaks* m_unk14;

public:
    /* 0x18 */ SkillTweaks* mUnidentified18[2];
}; // total size: 0x20

extern unk_8056CF08 gGameTweaks;

#endif // GAME_GAME_TWEAKS_H
