#include "Game/DB/GameProgress.h"
#include "Game/GameInfo.h"
#include "Game/TweakRegistry.h"
#include "NL/nlMath.h"

bool gUnlockAll;

inline bool ChallengeUnlockRecord::IsUnlocked(int flag) const
{
    int shift = nlLog2(0x100) + 1;
    return (mUnlockedChallenges & (flag >> shift)) != 0;
}

static inline bool IsUnlockOverrideEnabled(bool includeOnline)
{
    if (GetTweakBool("/user/media_build", false))
    {
        return false;
    }
    return gUnlockAll || GetTweakBool("/user/unlock_all", false)
        || (includeOnline && GameInfoManager::Instance()->IsOnline());
}

bool GetUnlockAll()
{
    return gUnlockAll;
}

void SetUnlockAll(bool value)
{
    gUnlockAll = value;
}

void SetUnlockFlag(int flag)
{
    if (flag <= 0x100)
    {
        CupManager::Instance()->mCupRecord.mUnlockFlags |= flag;
        int index = nlLog2(flag);
        CupManager::Instance()->RecordCupUnlock(index);
    }
    else if (!IsUnlockFlagSet(flag))
    {
        RecordChallengeUnlock(&g_pStrikerChallenge->mUnlocks, flag);
    }
}

bool IsUnlockFlagSet(int flag)
{
    bool unlocked;
    if (flag <= 0x100)
    {
        unlocked = CupManager::Instance()->HasUnlockFlag(flag);
    }
    else
    {
        unlocked = g_pStrikerChallenge->mUnlocks.IsUnlocked(flag);
    }
    return unlocked;
}

void SavePreGameUnlockState()
{
    CupManager::Instance()->mPreGameUnlockedState = CupManager::Instance()->GetUnlockFlags();
}

bool WereUnlockFlagsClearBeforeGame(unsigned int flags)
{
    return (flags & CupManager::Instance()->mPreGameUnlockedState) == 0;
}

bool IsBowserJrUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(1);
}

bool IsDiddyKongUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(2);
}

bool IsPeteyUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(4);
}

bool IsWastelandsUnlocked()
{
    return IsUnlockOverrideEnabled(true) || (IsUnlockFlagSet(8) && IsUnlockFlagSet(16));
}

bool IsDumpUnlocked()
{
    return IsUnlockOverrideEnabled(true) || (IsUnlockFlagSet(32) && IsUnlockFlagSet(64));
}

bool IsGalacticStadiumUnlocked()
{
    return IsUnlockOverrideEnabled(true) || (IsUnlockFlagSet(128) && IsUnlockFlagSet(256));
}

bool HasWastelandsUnlockFlags()
{
    return IsUnlockFlagSet(8) && IsUnlockFlagSet(16);
}

bool HasDumpUnlockFlags()
{
    return IsUnlockFlagSet(32) && IsUnlockFlagSet(64);
}

bool HasGalacticStadiumUnlockFlags()
{
    return IsUnlockFlagSet(128) && IsUnlockFlagSet(256);
}

bool IsStormshipUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(4);
}

bool IsCrystalCanyonUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(2);
}

bool IsLavaPitUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(1);
}

bool IsSecureEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x8000);
}

bool IsPowerEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x20000);
}

bool IsVoltageEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x4000);
}

bool IsTiltEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x400);
}

bool IsWhiteBallEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x100000);
}

bool IsPowerupCheatsUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x80000);
}

bool IsSuperPowerupsCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x1000);
}

bool IsDevastatingPlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x800);
}

bool IsSafePlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x2000);
}

bool IsSkillShotPlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x40000);
}

bool IsGlassJawPlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x200);
}

bool IsButterfingersPlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x10000);
}

bool WasWastelandsLockedBeforeGame()
{
    return !(CupManager::Instance()->mPreGameUnlockedState & 8)
        || !(CupManager::Instance()->mPreGameUnlockedState & 16);
}

bool WasDumpLockedBeforeGame()
{
    return !(CupManager::Instance()->mPreGameUnlockedState & 32)
        || !(CupManager::Instance()->mPreGameUnlockedState & 64);
}

bool WasGalacticStadiumLockedBeforeGame()
{
    return !(CupManager::Instance()->mPreGameUnlockedState & 128)
        || !(CupManager::Instance()->mPreGameUnlockedState & 256);
}
