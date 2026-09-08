#include "Game/GameInfo.h"
#include "Game/FE/feHelpFuncs.h"

#include "Game/DB/SaveLoad.h"
#include "Game/DB/UserOptions.h"
#include "Game/DB/GameProgress.h"
#include "Game/NetworkSession.h"
#include "NL/nlMemory.h"
#include "NL/nlMath.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/DB/StadiumInfo.h"

#include <string.h>
#include "NL/nlstring_tmpl.h"

struct RFLCreateID;
struct DWCstAccUserData;
typedef DWCstAccUserData DWCAccUserData;

extern "C" int RFLSearchOfficialData(const RFLCreateID*, u16*);
extern "C" int DWC_CheckDirtyFlag(const DWCAccUserData*);
extern "C" void DWC_ClearDirtyFlag(DWCAccUserData*);

extern "C" void* fn_8010D668(void*);
extern "C" void fn_800FBCB0(void*, int);
extern "C" bool GetTweakBool(const char*, bool);
extern "C" bool fn_802C2DBC(const char*);
extern "C" const char* GetTweakString(const char*, const char*);
extern "C" int GetTweakInt(const char*, int);
extern "C" void nlBreak__Fv();
extern "C" void* fn_8010D6F8(void*, void*);
extern "C" void* fn_8010D794(void*, void*);
extern bool g_e3_Build;

static const GameRules kDefaultRules[12] = {
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
GameInfoManager* nlSingleton<GameInfoManager>::s_pInstance = 0;

GameInfoManager::GameInfoManager()
    : mCurrentMode(-1)
    , mIsOnlineMode(0)
    , unknown_0x121(1)
    , unknown_0x122(0)
    , unknown_0x123(0)
    , mOnlineRankedMatch(false)
    , mOnlineTwoLocalPlayers(false)
    , mOnlineFriendSelectionMode(false)
    , unknown_0x278(0)
{
    memset(mRulesTable, 0, sizeof(mRulesTable));

    unknown_0x71C8 = 0;
    mGameInfo[GM_FRIENDLY] = 0;
    mGameInfo[GM_MODE_1] = 0;
    mGameInfo[GM_MODE_2] = 0;
    mGameInfo[GM_MODE_3] = 0;
    mGameInfo[GM_MODE_4] = 0;

    mGameInfo[GM_FRIENDLY] = new (nlMalloc(sizeof(BasicGameInfo), 8, false)) BasicGameInfo;
    fn_800FBCB0(mGameInfo[GM_FRIENDLY], 0);
    mGameInfo[GM_MODE_2] = new (nlMalloc(sizeof(BasicGameInfo), 8, false)) BasicGameInfo;
    fn_800FBCB0(mGameInfo[GM_MODE_2], 0);
    mGameInfo[GM_MODE_1] = new (nlMalloc(sizeof(BasicGameInfo), 8, false)) BasicGameInfo;
    fn_800FBCB0(mGameInfo[GM_MODE_1], 0);
    mGameInfo[GM_MODE_4] = new (nlMalloc(sizeof(BasicGameInfo), 8, false)) BasicGameInfo;
    fn_800FBCB0(mGameInfo[GM_MODE_4], 0);

    if (GetTweakBool("User/skipfe", false)) {
        SetMode(GM_FRIENDLY, false);
        SetTeam(0, 3);
        SetSidekick(0, 1, -1);
        SetTeam(1, 0);
        SetSidekick(1, 0, -1);
        SetStadium(13);
    }

    mUserInfo.mSaveID = nlRandom(0xFFFFFFFF, &nlDefaultSeed);
    mUserInfo.mNumGamesPlayed = 0;
    mUserInfo.mNumGoalsScored = 0;
    mUserInfo.mNumHits = 0;
    mUserInfo.mNumPerfectPasses = 0;
    mUserInfo.mNumSTSAttempts = 0;

    for (int i = 0; i < 4; i++) {
        memset(&unknown_0x128[i], 0, sizeof(GameInfoSlotEntry));
        unknown_0x128[i].unknown_0x4C = i;
        unknown_0x128[i].unknown_0x50 = 2;
    }

    mUserInfo.mGameplayOptions.InitializeDefaults();
    mUserInfo.mPowerupOptions.InitializeDefaults();

    mDefaultSettings.SkillLevel = GameplaySettings::ROOKIE;
    mDefaultSettings.BestSeries = 3;
    mDefaultSettings.WinBy = 0;
    mDefaultSettings.GameGoals = 5;
    mDefaultSettings.GameTime = 0xB4;
    mMode1Settings.SkillLevel = GameplaySettings::ROOKIE;
    mMode1Settings.BestSeries = 1;
    mMode1Settings.WinBy = 0;
    mMode1Settings.GameGoals = 5;
    mMode1Settings.GameTime = 0xB4;
    sThis = this;

    for (int i = 0; i < 12; i++) {
        mRulesTable[i] = kDefaultRules[i];
    }

    memset(mSaveSlots, 0, sizeof(mSaveSlots));
    SaveLoad::AllocateBannerBuffer();
}

GameInfoManager::~GameInfoManager()
{
    delete mGameInfo[GM_FRIENDLY];
    delete mGameInfo[GM_MODE_2];
    delete mGameInfo[GM_MODE_1];
    sThis = 0;
    SaveLoad::FreeBannerBuffer();
}

static bool (GameInfoManager::*sModeCheck)() const = &GameInfoManager::IsInMode3;

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
    return mGameInfo[mCurrentMode]->mSidekickIndex[side][slot];
}

void GameInfoManager::SetSidekick(short side, int sidekick, int slot)
{
    BasicGameInfo* info = mGameInfo[mCurrentMode];

    for (int i = 0; i < 3; i++) {
        if (slot < 0 || slot == i) {
            info->mSidekickIndex[side][i] = sidekick;
        }
    }
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

void GameInfoManager::SetMode(int mode, u8 isOnline)
{
    mCurrentMode = mode;
    unknown_0x122 = 0;
    mIsOnlineMode = isOnline;
}

void* GameInfoManager::GetUnknown806E0F90Block() const
{
    return (u8*)fn_8010D668(g_pCupManager) + 0x144;
}

unsigned long GameInfoManager::GetMemoryCardDataSize() const
{
    return sizeof(mSaveSlots);
}

void GameInfoManager::SerializeSettings(void* data) const
{
    memcpy(data, &mUserInfo, sizeof(UserInfo));
    data = (u8*)data + sizeof(UserInfo);
    memcpy(data, mRulesTable, sizeof(mRulesTable));
    g_pStrikerChallenge->SerializeData(fn_8010D6F8(g_pCupManager, (u8*)data + sizeof(mRulesTable)));
}

void GameInfoManager::GetMemoryCardData(void* data) const
{
    memcpy(data, mSaveSlots, sizeof(mSaveSlots));
}

void GameInfoManager::DeserializeSettings(void* data)
{
    memcpy(&mUserInfo, data, sizeof(UserInfo));
    data = (u8*)data + sizeof(UserInfo);
    memcpy(mRulesTable, data, sizeof(mRulesTable));
    g_pStrikerChallenge->DeserializeData(fn_8010D794(g_pCupManager, (u8*)data + sizeof(mRulesTable)));
}

void GameInfoManager::SetMemoryCardData(const void* data)
{
    memcpy(mSaveSlots, data, sizeof(mSaveSlots));
}

u8 GameInfoManager::CheckSaveIDChanged(const void* data) const
{
    return 0;
}

bool GameInfoManager::IsInMode3() const
{
    return mCurrentMode == GM_MODE_3;
}

bool GameInfoManager::IsInOddCupMode() const
{
    return mCurrentMode == GM_MODE_1 || mCurrentMode == GM_MODE_3;
}

bool GameInfoManager::IsInMode2() const
{
    return mCurrentMode == GM_MODE_2;
}

bool GameInfoManager::IsInFriendlyMode() const
{
    return mCurrentMode == GM_FRIENDLY;
}

bool GameInfoManager::IsInMode1() const
{
    return mCurrentMode == GM_MODE_1;
}

bool GameInfoManager::IsInMode4() const
{
    return mCurrentMode == GM_MODE_4;
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
    if (unknown_0x27C) {
        const GameplaySettings* settings = &mCurGameSettings;

        return settings;
    }

    if (mIsOnlineMode != 0) {
        if (mOnlineRankedMatch != 0) {
            if (mCurrentMode == GM_MODE_1) {
                const GameplaySettings* settings = &mMode1Settings;

                return settings;
            }

            const GameplaySettings* settings = &mDefaultSettings;

            return settings;
        }

        const GameplaySettings* settings = &mNoCheatSettings;

        return settings;
    }

    if (mCurrentMode == GM_MODE_3) {
        const GameplaySettings* settings =
            &g_pCupManager->mCurrentCup->mCupSettings;

        return settings;
    }

    const GameplaySettings* settings = &mUserInfo.mGameplayOptions;

    return settings;
}

const PowerupSettings* GameInfoManager::GetActiveRules() const
{
    if (UseAltRules()) {
        return &mRulesA;
    }

    return &mUserInfo.mPowerupOptions;
}

void GameInfoManager::ResetUnknown0xA0()
{
    unknown_0x27C = 0;
    mUserInfo.mAudioOptions.ApplySettings();
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

    mCurGameSettings = *settings;
    unknown_0x27C = 1;

    for (int side = 0; side < 2; side++) {
        nlSNPrintf(name, sizeof(name), "user/team%d", side + 1);

        if (fn_802C2DBC(name)) {
            SetTeam(side, ConvertToTeamID(GetTweakString(name, kDefaultTeam)));
        }

        for (int sidekick = 0; sidekick < 3; sidekick++) {
            nlSNPrintf(name, sizeof(name), side == 0 ? "user/sidekickhome%d" : "user/sidekickaway%d", sidekick);

            if (fn_802C2DBC(name)) {
                SetSidekick(side,
                    ConvertToSidekickID(GetTweakString(name, side == 0 ? kDefaultHomeSidekick : kDefaultAwaySidekick)),
                    sidekick);
            }
        }
    }

    if (fn_802C2DBC("user/soak_diff")) {
        mCurGameSettings.SkillLevel = static_cast<GameplaySettings::eSkillLevel>(GetTweakInt("user/soak_diff", 2));
    }

    for (int pad = 0; pad < 4; pad++) {
        nlSNPrintf(padName, sizeof(padName), "user/pad%d", pad);

        if (fn_802C2DBC(padName)) {
            const char* value = GetTweakString(padName, kHomeSide);

            if (nlStrNCmp(value, kHomeSide, sizeof(padName)) == 0) {
                SetPlayingSide(pad, 0);
            } else if (nlStrNCmp(value, kAwaySide, sizeof(padName)) == 0) {
                SetPlayingSide(pad, 1);
            }
        }
    }

    if (mCurrentMode == GM_MODE_2) {
        mCurGameSettings.GameTime = 0x78;
        mCurGameSettings.SkillLevel = GameplaySettings::PROFESSIONAL;
    } else if (unknown_0x122) {
        mCurGameSettings.GameTime = 0xEA24;
    } else if (g_e3_Build) {
        mCurGameSettings.GameTime = 0xB4;
        mCurGameSettings.SkillLevel = GameplaySettings::ROOKIE;
    } else if (mCurrentMode == GM_MODE_4) {
        StrikerChallenge* other = g_pStrikerChallenge;

        mCurGameSettings.GameTime = other->mRemainingTime;
        mCurGameSettings.HomePowerUps = other->mHomePowerupsEnabled;
        mCurGameSettings.AwayPowerUps = other->mAwayPowerupsEnabled;
        mCurGameSettings.HomeShoot2Score = other->mHomeMegastrikeEnabled;
        mCurGameSettings.AwayShoot2Score = other->mAwayMegastrikeEnabled;
        mCurGameSettings.HomeSkillShots = other->mHomeSkillshotEnabled;
        mCurGameSettings.AwaySkillShots = other->mAwaySkillshotEnabled;
        mCurGameSettings.WinBy = 0;
        mCurGameSettings.SkillLevel = static_cast<GameplaySettings::eSkillLevel>(other->mAIDifficulty);
        mCurGameSettings.BestSeries = 1;
    }

    if (GetTweakBool("User/skipfe", false)) {
        mCurGameSettings.BestSeries = 11;
    }

    if (fn_802C2DBC("User/stadium")) {
        const char* value = GetTweakString("User/stadium", 0);

        SetStadium(-1);

        for (int stadium = 0; stadium < 18; stadium++) {
            if (nlStrICmp(GetStadiumName(stadium), value) == 0) {
                if (IsStadiumEnabled(stadium) == 0) {
                    nlBreak__Fv();
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
    if (unknown_0x122) {
        skillLevel = 0;
    } else {
        skillLevel = GetCurrentSettings()->SkillLevel;
    }

    mCurrentDifficulty[0] = DifficultyMap[skillLevel][humansOnSide[0] ? 0 : 1];
    mCurrentDifficulty[1] = DifficultyMap[skillLevel][humansOnSide[1] ? 0 : 1];
}

bool GameInfoManager::IsRule0x8Equal4() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mPlayerCheat == 4;
    }

    return false;
}

bool GameInfoManager::IsRule0x0Equal10() const
{
    if (unknown_0x122) {
        return false;
    }

    if (mCurrentMode == GM_MODE_4) {
        int value = g_pStrikerChallenge->mCurrentChallenge;

        switch (value) {
        case 6:
        case 7:
            return true;
        default:
            return false;
        }
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mCustomPowerups == 10;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal4() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == 4;
    }

    return false;
}

bool GameInfoManager::IsRule0x8Equal2() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mPlayerCheat == 2;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal1() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == 1;
    }

    return false;
}

bool GameInfoManager::IsRule0x8Equal3() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mPlayerCheat == 3;
    }

    return false;
}

bool GameInfoManager::IsRule0x8Equal1() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mPlayerCheat == 1;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal3() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == 3;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal2() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == 2;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal5() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mEnvironmentCheat == 5;
    }

    return false;
}

bool GameInfoManager::IsRule0x0Equal11() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mCustomPowerups == 11;
    }

    return false;
}

GameInfoSaveSlot* GameInfoManager::GetSaveSlot(int slot)
{
    return &mSaveSlots[slot];
}

void* GameInfoManager::GetUnknown0x40(int slot, int index)
{
    return mSaveSlots[slot].unknown_0x040[index];
}

void* GameInfoManager::GetUnknown0xA80(int slot)
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

void* GameInfoManager::GetUnknown0xA88(int slot)
{
    return mSaveSlots[slot].unknown_0xA88;
}

void* GameInfoManager::GetUnknown0xA40(int slot, int index)
{
    return &mSaveSlots[slot].unknown_0xA40[index];
}

int* GameInfoManager::GetUnknown0xA90(int slot)
{
    return &mSaveSlots[slot].unknown_0xA90;
}

int* GameInfoManager::GetUnknown0xA94(int slot)
{
    return &mSaveSlots[slot].unknown_0xA94;
}

int* GameInfoManager::GetUnknown0xA98(int slot)
{
    return &mSaveSlots[slot].unknown_0xA98;
}

int* GameInfoManager::GetUnknown0xA9C(int slot)
{
    return &mSaveSlots[slot].unknown_0xA9C;
}

int* GameInfoManager::GetUnknown0xAA0(int slot)
{
    return &mSaveSlots[slot].unknown_0xAA0;
}

int* GameInfoManager::GetUnknown0xAA4(int slot)
{
    return &mSaveSlots[slot].unknown_0xAA4;
}

int GameInfoManager::GetUnknown0xA90Total(int index)
{
    GameInfoSaveSlot* slot = &mSaveSlots[index];
    int total = slot->unknown_0xA90 + slot->unknown_0xA98;

    if (total >= 9999) {
        total = 9999;
    }

    return total;
}

int GameInfoManager::GetUnknown0xA94Total(int index)
{
    GameInfoSaveSlot* slot = &mSaveSlots[index];
    int total = slot->unknown_0xA94 + slot->unknown_0xA9C;

    if (total >= 9999) {
        total = 9999;
    }

    return total;
}

int GameInfoManager::GetUnknown0xAA0Total(int index)
{
    GameInfoSaveSlot* slot = &mSaveSlots[index];
    int total = slot->unknown_0xA90 + slot->unknown_0xA98 + slot->unknown_0xAA0;

    if (total >= 9999) {
        total = 9999;
    }

    return total;
}

int GameInfoManager::GetUnknown0xAA4Total(int index)
{
    GameInfoSaveSlot* slot = &mSaveSlots[index];
    int total = slot->unknown_0xA94 + slot->unknown_0xA9C + slot->unknown_0xAA4;

    if (total >= 9999) {
        total = 9999;
    }

    return total;
}

void* GameInfoManager::GetUnknown0xAA8(int slot)
{
    return mSaveSlots[slot].unknown_0xAA8;
}

void* GameInfoManager::GetUnknown0xABE(int slot)
{
    return mSaveSlots[slot].unknown_0xABE;
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

int GameInfoManager::GetMappedRule0x0() const
{
    int table[6] = { 0, 1, 2, 3, 4, 5 };
    int index;

    if (unknown_0x122) {
        index = 0;
    } else {
        index = GetCurrentSettings()->SkillLevel;
    }

    return table[index];
}

int GameInfoManager::GetRule0x0() const
{
    if (unknown_0x122) {
        return 0;
    }

    if ((mIsOnlineMode == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->mCustomPowerups;
    }

    if (mCurrentMode == GM_MODE_4) {
        return g_pStrikerChallenge->mCustomPowerups;
    }

    return 0;
}

void GameInfoManager::ResetRules(int index)
{
    mRulesTable[index] = kDefaultRules[index];
}
