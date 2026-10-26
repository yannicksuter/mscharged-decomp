#include <RVLFaceLib/RFL_Database.h>
#include <dwc/dwc_account.h>

#include "Game/GameInfo.h"

#include "Game/DB/SaveLoad.h"
#include "Game/DB/UserOptions.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/NetworkSession.h"
#include "Game/TweakRegistry.h"
#include "Game/main.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "NL/nlMath.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/DB/StadiumInfo.h"

#include <string.h>

static const int kDefaultSidekickChoices[12][3] = {
    { 1, 1, 7 },
    { 4, 5, 5 },
    { 6, 1, 5 },
    { 0, 0, 0 },
    { 7, 7, 1 },
    { 3, 3, 0 },
    { 2, 5, 2 },
    { 3, 4, 5 },
    { 2, 1, 5 },
    { 5, 1, 2 },
    { 6, 2, 6 },
    { 6, 0, 0 },
};

GameInfoManager* GameInfoManager::sThis;
template <>
GameInfoManager* nlSingleton<GameInfoManager>::s_pInstance = 0;

GameInfoManager::GameInfoManager()
    : mCurrentMode(-1)
    , mIsOnlineMode(0)
    , mSoakDemoMatchEnabled(1)
    , mIsInStrikers101Mode(0)
    , unknown_0x123(0)
    , mOnlineRankedMatch(false)
    , mOnlineTwoLocalPlayers(false)
    , mOnlineFriendSelectionMode(false)
    , mMainUserPadNumber(FE_PAD1_ID)
{
    mFrontEndReturnMode = 0;
    mGameInfo[GM_FRIENDLY] = 0;
    mGameInfo[GM_ONLINE_TOURNAMENT] = 0;
    mGameInfo[GM_DEMO] = 0;
    mGameInfo[GM_CUP] = 0;
    mGameInfo[GM_CHALLENGE] = 0;

    mGameInfo[GM_FRIENDLY] = new (nlMalloc(sizeof(BasicGameInfo), 8, false)) BasicGameInfo;
    mGameInfo[GM_FRIENDLY]->Reset(false);
    mGameInfo[GM_DEMO] = new (nlMalloc(sizeof(BasicGameInfo), 8, false)) BasicGameInfo;
    mGameInfo[GM_DEMO]->Reset(false);
    mGameInfo[GM_ONLINE_TOURNAMENT] = new (nlMalloc(sizeof(BasicGameInfo), 8, false)) BasicGameInfo;
    mGameInfo[GM_ONLINE_TOURNAMENT]->Reset(false);
    mGameInfo[GM_CHALLENGE] = new (nlMalloc(sizeof(BasicGameInfo), 8, false)) BasicGameInfo;
    mGameInfo[GM_CHALLENGE]->Reset(false);

    if (GetTweakBool("User/skipfe", false)) {
        SetMode(GM_FRIENDLY, false);
        SetTeam(0, 3);
        SetSidekick(0, 1, -1);
        SetTeam(1, 0);
        SetSidekick(1, 0, -1);
        SetStadium(STAD_VICE);
    }

    mUserInfo.mSaveID = nlRandom(0xFFFFFFFF, &nlDefaultSeed);
    mUserInfo.mNumGamesPlayed = 0;
    mUserInfo.mNumGoalsScored = 0;
    mUserInfo.mNumHits = 0;
    mUserInfo.mNumPerfectPasses = 0;
    mUserInfo.mNumSTSAttempts = 0;

    for (int i = 0; i < 4; i++) {
        memset(&mUserStats[i], 0, sizeof(GameInfoSlotEntry));
        mUserStats[i].mStats.mRecordType.mControllerID = i;
        mUserStats[i].mStats.mType = TYPE_USER;
    }

    mUserInfo.mGameplayOptions.InitializeDefaults();
    mUserInfo.mCheatOptions.InitializeDefaults();

    mDefaultSettings.SkillLevel = GameplaySettings::ROOKIE;
    mDefaultSettings.NumGames = 3;
    mDefaultSettings.GameLimitType = GAME_LIMIT_TIME;
    mDefaultSettings.GoalLimit = 5;
    mDefaultSettings.GameTime = 0xB4;
    mOnlineTournamentSettings.SkillLevel = GameplaySettings::ROOKIE;
    mOnlineTournamentSettings.NumGames = 1;
    mOnlineTournamentSettings.GameLimitType = GAME_LIMIT_TIME;
    mOnlineTournamentSettings.GoalLimit = 5;
    mOnlineTournamentSettings.GameTime = 0xB4;
    sThis = this;

    for (int i = 0; i < 12; i++) {
        mSidekickChoices[i].mValues[0] = (eSidekickID)kDefaultSidekickChoices[i][0];
        mSidekickChoices[i].mValues[1] = (eSidekickID)kDefaultSidekickChoices[i][1];
        mSidekickChoices[i].mValues[2] = (eSidekickID)kDefaultSidekickChoices[i][2];
    }

    memset(mSaveSlots, 0, sizeof(mSaveSlots));
    SaveLoad::AllocateBannerBuffer();
}

GameInfoManager::~GameInfoManager()
{
    delete mGameInfo[GM_FRIENDLY];
    delete mGameInfo[GM_DEMO];
    delete mGameInfo[GM_ONLINE_TOURNAMENT];
    sThis = 0;
    SaveLoad::FreeBannerBuffer();
}

static bool (GameInfoManager::*sModeCheck)() const = &GameInfoManager::IsInCupMode;

int GameInfoManager::GetTeam(short side) const
{
    BasicGameInfo* info = mGameInfo[mCurrentMode];

    if (info == 0) {
        return -1;
    }

    return info->mTeamIndex[side];
}

void GameInfoManager::SetTeam(short side, int team)
{
    mGameInfo[mCurrentMode]->mTeamIndex[side] = team;
}

int GameInfoManager::GetSidekick(short side, int slot) const
{
    return mGameInfo[mCurrentMode]->GetSidekick(side, slot);
}

void GameInfoManager::SetSidekick(short side, int sidekick, int slot)
{
    mGameInfo[mCurrentMode]->SetSidekick(side, sidekick, slot);
}

int GameInfoManager::GetStadium() const
{
    return mGameInfo[mCurrentMode]->mStadiumIndex;
}

void GameInfoManager::SetStadium(int stadium)
{
    mGameInfo[mCurrentMode]->mStadiumIndex = stadium;
}

short GameInfoManager::GetPlayingSide(unsigned short pad) const
{
    return mGameInfo[mCurrentMode]->mPadSides[pad];
}

void GameInfoManager::SetPlayingSide(unsigned short pad, short side)
{
    mGameInfo[mCurrentMode]->mPadSides[pad] = side;
}

void GameInfoManager::ResetPlayingSides()
{
    BasicGameInfo* info = mGameInfo[mCurrentMode];

    for (int i = 0; i < 16; i++) {
        info->mPadSides[i] = -1;
    }
}

void GameInfoManager::SetMode(int mode, bool online)
{
    mCurrentMode = mode;
    mIsInStrikers101Mode = 0;
    mIsOnlineMode = online;
}

unsigned long GameInfoManager::GetSettingsDataSize() const
{
    return CupManager::s_pInstance->GetSaveDataSize() + 0x144;
}

unsigned long GameInfoManager::GetMemoryCardDataSize() const
{
    return sizeof(mSaveSlots);
}

void GameInfoManager::SerializeSettings(void* data) const
{
    memcpy(data, &mUserInfo, sizeof(mUserInfo));
    data = (u8*)data + 0x80;
    memcpy(data, mSidekickChoices, sizeof(mSidekickChoices));
    g_pStrikerChallenge->SerializeData(CupManager::s_pInstance->SerializeData((u8*)data + sizeof(mSidekickChoices)));
}

void GameInfoManager::GetMemoryCardData(void* data) const
{
    memcpy(data, mSaveSlots, sizeof(mSaveSlots));
}

void GameInfoManager::DeserializeSettings(void* data)
{
    memcpy(&mUserInfo, data, sizeof(mUserInfo));
    data = (u8*)data + 0x80;
    memcpy(mSidekickChoices, data, sizeof(mSidekickChoices));
    g_pStrikerChallenge->DeserializeData(CupManager::s_pInstance->DeserializeData((u8*)data + sizeof(mSidekickChoices)));
}

void GameInfoManager::SetMemoryCardData(const void* data)
{
    memcpy(mSaveSlots, data, sizeof(mSaveSlots));
}

bool GameInfoManager::HasTrophy(eTrophyType trophyType) const
{
    return false;
}

bool GameInfoManager::IsInCupMode() const
{
    return mCurrentMode == GM_CUP;
}

bool GameInfoManager::IsInCupOrTournamentMode() const
{
    return mCurrentMode == GM_ONLINE_TOURNAMENT || mCurrentMode == GM_CUP;
}

bool GameInfoManager::IsInDemoMode() const
{
    return mCurrentMode == GM_DEMO;
}

bool GameInfoManager::IsInFriendlyMode() const
{
    return mCurrentMode == GM_FRIENDLY;
}

bool GameInfoManager::IsInOnlineTournamentMode() const
{
    return mCurrentMode == GM_ONLINE_TOURNAMENT;
}

bool GameInfoManager::IsInChallengeMode() const
{
    return mCurrentMode == GM_CHALLENGE;
}

AudioSettings* GameInfoManager::GetAudioSettings()
{
    return &mUserInfo.mAudioOptions;
}

VisualSettings* GameInfoManager::GetVisualOptions()
{
    return &mUserInfo.mVisualOptions;
}

const GameplaySettings* GameInfoManager::GetCurrentSettings() const
{
    if (mUseCurGameSettings) {
        const GameplaySettings* settings = &mCurGameGameplayOptions;

        return settings;
    }

    if (mIsOnlineMode != 0) {
        if (mOnlineRankedMatch != 0) {
            if (mCurrentMode == GM_ONLINE_TOURNAMENT) {
                const GameplaySettings* settings = &mOnlineTournamentSettings;

                return settings;
            }

            const GameplaySettings* settings = &mDefaultSettings;

            return settings;
        }

        const GameplaySettings* settings = &mUnrankedGameplaySettings;

        return settings;
    }

    if (mCurrentMode == GM_CUP) {
        const GameplaySettings* settings =
            &CupManager::s_pInstance->mCurrentCup->mCupSettings;

        return settings;
    }

    const GameplaySettings* settings = &mUserInfo.mGameplayOptions;

    return settings;
}

const CheatSettings* GameInfoManager::GetActiveRules() const
{
    if (UseAltRules()) {
        return &mUnrankedCheatSettings;
    }

    return &mUserInfo.mCheatOptions;
}

static char kDefaultTeam[] = "mario";
static char kDefaultHomeSidekick[] = "toad";
static char kDefaultAwaySidekick[] = "koopa";
static char kHomeSide[8] = "home";
static char kAwaySide[8] = "away";

void GameInfoManager::SetupGameFromConfig()
{
    char padName[12];
    char name[64];
    const GameplaySettings* settings = GetCurrentSettings();

    mCurGameGameplayOptions.SkillLevel = settings->SkillLevel;
    mCurGameGameplayOptions.GameLimitType = settings->GameLimitType;
    mCurGameGameplayOptions.GameTime = settings->GameTime;
    mCurGameGameplayOptions.GoalLimit = settings->GoalLimit;
    mCurGameGameplayOptions.NumGames = settings->NumGames;
    mCurGameGameplayOptions.mHomePowerupsEnabled = settings->mHomePowerupsEnabled;
    mCurGameGameplayOptions.mAwayPowerupsEnabled = settings->mAwayPowerupsEnabled;
    mCurGameGameplayOptions.mHomeMegastrikeEnabled = settings->mHomeMegastrikeEnabled;
    mCurGameGameplayOptions.mAwayMegastrikeEnabled = settings->mAwayMegastrikeEnabled;
    mCurGameGameplayOptions.mHomeSkillshotEnabled = settings->mHomeSkillshotEnabled;
    mCurGameGameplayOptions.mAwaySkillshotEnabled = settings->mAwaySkillshotEnabled;
    mCurGameGameplayOptions.m_unk1A = settings->m_unk1A;
    mUseCurGameSettings = 1;

    for (int side = 0; side < 2; side++) {
        nlSNPrintf(name, sizeof(name), "user/team%d", side + 1);

        if (TweakExists(name)) {
            SetTeam(side, ConvertToTeamID(GetTweakString(name, kDefaultTeam)));
        }

        for (int sidekick = 0; sidekick < 3; sidekick++) {
            nlSNPrintf(name, sizeof(name), side == 0 ? "user/sidekickhome%d" : "user/sidekickaway%d", sidekick);

            if (TweakExists(name)) {
                SetSidekick(side,
                    ConvertToSidekickID(GetTweakString(name, side == 0 ? kDefaultHomeSidekick : kDefaultAwaySidekick)),
                    sidekick);
            }
        }
    }

    if (TweakExists("user/soak_diff")) {
        mCurGameGameplayOptions.SkillLevel = (GameplaySettings::eSkillLevel)GetTweakInt("user/soak_diff", 2);
    }

    for (int pad = 0; pad < 4; pad++) {
        nlSNPrintf(padName, sizeof(padName), "user/pad%d", pad);

        if (TweakExists(padName)) {
            const char* value = GetTweakString(padName, kHomeSide);

            if (nlStrNCmp(value, kHomeSide, sizeof(padName)) == 0) {
                SetPlayingSide(pad, 0);
            } else if (nlStrNCmp(value, kAwaySide, sizeof(padName)) == 0) {
                SetPlayingSide(pad, 1);
            }
        }
    }

    if (mCurrentMode == GM_DEMO) {
        mCurGameGameplayOptions.GameTime = 0x78;
        mCurGameGameplayOptions.SkillLevel = GameplaySettings::PROFESSIONAL;
    } else if (mIsInStrikers101Mode) {
        mCurGameGameplayOptions.GameTime = 0xEA24;
    } else if (g_e3_Build) {
        mCurGameGameplayOptions.GameTime = 0xB4;
        mCurGameGameplayOptions.SkillLevel = GameplaySettings::ROOKIE;
    } else if (mCurrentMode == GM_CHALLENGE) {
        StrikerChallenge* challenge = g_pStrikerChallenge;

        mCurGameGameplayOptions.GameTime = challenge->mRemainingTime;
        mCurGameGameplayOptions.mHomePowerupsEnabled = challenge->mHomePowerupsEnabled;
        mCurGameGameplayOptions.mAwayPowerupsEnabled = challenge->mAwayPowerupsEnabled;
        mCurGameGameplayOptions.mHomeMegastrikeEnabled = challenge->mHomeMegastrikeEnabled;
        mCurGameGameplayOptions.mAwayMegastrikeEnabled = challenge->mAwayMegastrikeEnabled;
        mCurGameGameplayOptions.mHomeSkillshotEnabled = challenge->mHomeSkillshotEnabled;
        mCurGameGameplayOptions.mAwaySkillshotEnabled = challenge->mAwaySkillshotEnabled;
        mCurGameGameplayOptions.GameLimitType = GAME_LIMIT_TIME;
        mCurGameGameplayOptions.SkillLevel = (GameplaySettings::eSkillLevel)challenge->mAIDifficulty;
        mCurGameGameplayOptions.NumGames = 1;
    }

    if (GetTweakBool("User/skipfe", false)) {
        mCurGameGameplayOptions.NumGames = 11;
    }

    if (TweakExists("User/stadium")) {
        const char* value = GetTweakString("User/stadium", 0);

        SetStadium(STAD_INVALID);

        for (int stadium = 0; stadium < 18; stadium++) {
            if (nlStrICmp(GetStadiumName(stadium), value) == 0) {
                if (IsStadiumEnabled(stadium) == 0) {
                    nlBreak();
                }

                SetStadium(stadium);
                break;
            }
        }
    }

    if (!sModeCheck) {
        GetCurrentGameInfo()->mFinalScore[0] = 0;
        GetCurrentGameInfo()->mFinalScore[1] = 0;
    }
}

void GameInfoManager::ClearGameSettingsOverride()
{
    mUseCurGameSettings = 0;
    mUserInfo.mAudioOptions.ApplySettings();
}

void GameInfoManager::ApplyDifficultySettings()
{
    static const int DifficultyMap[6][2] = {
        { 7, 0 },
        { 7, 1 },
        { 7, 2 },
        { 7, 3 },
        { 7, 4 },
        { 7, 5 },
    };

    if (g_pNetworkSessionBase->GetNumMachines() > 1) {
        mCurrentDifficulty[0] = 7;
        mCurrentDifficulty[1] = 7;
        return;
    }

    unsigned char humansOnSide[2] = { 0, 0 };

    for (int i = 0; i < 4; i++) {
        short side = GetPlayingSide(i);

        if (side == 0) {
            humansOnSide[0] = 1;
        } else if (side == 1) {
            humansOnSide[1] = 1;
        }
    }

    int skillLevel;
    if (mIsInStrikers101Mode) {
        skillLevel = 0;
    } else {
        skillLevel = GetCurrentSettings()->SkillLevel;
    }

    mCurrentDifficulty[0] = DifficultyMap[skillLevel][humansOnSide[0] ? 0 : 1];
    mCurrentDifficulty[1] = DifficultyMap[skillLevel][humansOnSide[1] ? 0 : 1];
}

bool GameInfoManager::IsGlassJawEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mPlayerCheat == PLAYER_CHEAT_GLASS_JAW;
    }

    return false;
}

bool GameInfoManager::AreInfinitePowerupsEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if (mCurrentMode == GM_CHALLENGE) {
        int value = g_pStrikerChallenge->mCurrentChallenge;

        switch (value) {
        case TUTORIAL_SUPER_MARIO:
        case TUTORIAL_BRING_THUNDER:
            return true;
        default:
            return false;
        }
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mCustomPowerups == POWERUP_CHEAT_INFINITE;
    }

    return false;
}

bool GameInfoManager::IsScoreTiltEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == ENV_CHEAT_TILT;
    }

    return false;
}

bool GameInfoManager::IsSafeMegaStrikeEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mPlayerCheat == PLAYER_CHEAT_SAFE;
    }

    return false;
}

bool GameInfoManager::IsSecureStadiumEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == ENV_CHEAT_SECURE;
    }

    return false;
}

bool GameInfoManager::IsSkillShotCheatEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mPlayerCheat == PLAYER_CHEAT_SKILL_SHOT;
    }

    return false;
}

bool GameInfoManager::AreDevastatingHitsEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mPlayerCheat == PLAYER_CHEAT_DEVASTATING;
    }

    return false;
}

bool GameInfoManager::IsHighVoltageEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == ENV_CHEAT_VOLTAGE;
    }

    return false;
}

bool GameInfoManager::IsPowerShortageEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == ENV_CHEAT_POWER;
    }

    return false;
}

bool GameInfoManager::IsWhiteBallEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == ENV_CHEAT_WHITE_BALL;
    }

    return false;
}

bool GameInfoManager::IsButterfingersEnabled() const
{
    if (mIsInStrikers101Mode) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mCustomPowerups == POWERUP_CHEAT_BUTTERFINGERS;
    }

    return false;
}

GameInfoSaveSlot* GameInfoManager::GetSaveSlot(int slot)
{
    return &mSaveSlots[slot];
}

void* GameInfoManager::GetSavedFriendData(int slot, int index)
{
    return mSaveSlots[slot].mFriendData[index];
}

void* GameInfoManager::GetSaveSlotIdAddress(int slot)
{
    return &mSaveSlots[slot].mSaveId;
}

int GameInfoManager::GetSaveSlotName(int index) const
{
    u64 id;
    u16 name;
    bool found;

    memcpy(&id, &mSaveSlots[index].mSaveId, sizeof(id));
    name = 0;
    found = RFLSearchOfficialData((const RFLCreateID*)&id, &name);

    if (found) {
        return name;
    }

    return -1;
}

int GameInfoManager::FindSaveSlot(u64 id) const
{
    for (int i = 0; i < 10; i++) {
        if (mSaveSlots[i].mSaveId == id) {
            return i;
        }
    }

    return -1;
}

bool GameInfoManager::HasSaveSlot(u64 id) const
{
    return FindSaveSlot(id) >= 0;
}

u16* GameInfoManager::GetSavedFriendName(int slot, int index)
{
    return mSaveSlots[slot].mFriendNames[index];
}

void* GameInfoManager::GetSaveSlotLastUsedTime(int slot)
{
    return mSaveSlots[slot].mLastUsedTime;
}

void* GameInfoManager::GetSavedFriendRegion(int slot, int index)
{
    return &mSaveSlots[slot].mFriendRegions[index];
}

int* GameInfoManager::GetPastSeasonWins(int slot)
{
    return &mSaveSlots[slot].mPastSeasonWins;
}

int* GameInfoManager::GetPastSeasonLosses(int slot)
{
    return &mSaveSlots[slot].mPastSeasonLosses;
}

int* GameInfoManager::GetCurrentSeasonWins(int slot)
{
    return &mSaveSlots[slot].mCurrentSeasonWins;
}

int* GameInfoManager::GetCurrentSeasonLosses(int slot)
{
    return &mSaveSlots[slot].mCurrentSeasonLosses;
}

int* GameInfoManager::GetUnrankedWins(int slot)
{
    return &mSaveSlots[slot].mUnrankedWins;
}

int* GameInfoManager::GetUnrankedLosses(int slot)
{
    return &mSaveSlots[slot].mUnrankedLosses;
}

int GameInfoManager::GetTotalRankedWins(int index)
{
    GameInfoSaveSlot* slot = &mSaveSlots[index];
    int total = slot->mPastSeasonWins + slot->mCurrentSeasonWins;

    if (total >= 9999) {
        total = 9999;
    }

    return total;
}

int GameInfoManager::GetTotalRankedLosses(int index)
{
    GameInfoSaveSlot* slot = &mSaveSlots[index];
    int total = slot->mPastSeasonLosses + slot->mCurrentSeasonLosses;

    if (total >= 9999) {
        total = 9999;
    }

    return total;
}

int GameInfoManager::GetTotalOnlineWins(int index)
{
    GameInfoSaveSlot* slot = &mSaveSlots[index];
    int total = slot->mPastSeasonWins + slot->mCurrentSeasonWins + slot->mUnrankedWins;

    if (total >= 9999) {
        total = 9999;
    }

    return total;
}

int GameInfoManager::GetTotalOnlineLosses(int index)
{
    GameInfoSaveSlot* slot = &mSaveSlots[index];
    int total = slot->mPastSeasonLosses + slot->mCurrentSeasonLosses + slot->mUnrankedLosses;

    if (total >= 9999) {
        total = 9999;
    }

    return total;
}

void* GameInfoManager::GetSavedMiiName(int slot)
{
    return mSaveSlots[slot].mMiiName;
}

void* GameInfoManager::GetSavedMiiData(int slot)
{
    return mSaveSlots[slot].mMiiData;
}

void GameInfoManager::ClearSaveSlot(int slot)
{
    memset(&mSaveSlots[slot], 0, sizeof(GameInfoSaveSlot));
}

void GameInfoManager::ValidateSaveSlot(int index)
{
    GameInfoSaveSlot* slot = &mSaveSlots[index];

    if (DWC_CheckDirtyFlag((const DWCAccUserData*)slot)) {
        DWC_ClearDirtyFlag((DWCAccUserData*)slot);
        SaveLoad::StartSave(true);
    }
}

int GameInfoManager::GetGameplaySkillLevel()
{
    int skillToDifficulty[6] = { 0, 1, 2, 3, 4, 5 };
    GameplaySettings::eSkillLevel level;

    if (mIsInStrikers101Mode) {
        level = GameplaySettings::TRAINING;
    } else {
        level = GetCurrentSettings()->SkillLevel;
    }

    return skillToDifficulty[level];
}

int GameInfoManager::GetCustomPowerups() const
{
    if (mIsInStrikers101Mode) {
        return 0;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mCustomPowerups;
    }

    if (mCurrentMode == GM_CHALLENGE) {
        return g_pStrikerChallenge->mCustomPowerups;
    }

    return 0;
}

void GameInfoManager::ResetSidekickChoices(int index)
{
    mSidekickChoices[index].mValues[0] = (eSidekickID)kDefaultSidekickChoices[index][0];
    mSidekickChoices[index].mValues[1] = (eSidekickID)kDefaultSidekickChoices[index][1];
    mSidekickChoices[index].mValues[2] = (eSidekickID)kDefaultSidekickChoices[index][2];
}
