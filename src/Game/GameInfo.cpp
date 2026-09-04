#include "Game/GameInfo.h"

#include "Game/DB/SaveLoad.h"
#include "Game/DB/UserOptions.h"
#include "Game/DB/tu_8010A40C.h"
#include "Game/NetworkSession.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/DB/StadiumInfo.h"

extern "C" void* memcpy(void* dst, const void* src, unsigned long size);

struct RFLCreateID;
struct DWCstAccUserData;
typedef DWCstAccUserData DWCAccUserData;

extern "C" int RFLSearchOfficialData(const RFLCreateID*, u16*);
extern "C" int DWC_CheckDirtyFlag(const DWCAccUserData*);
extern "C" void DWC_ClearDirtyFlag(DWCAccUserData*);

extern "C" void* fn_8010D668(void*);
extern "C" void fn_80109890(void*);
extern "C" void fn_80109BC4(void*);
extern "C" void fn_80109C40(void*);
extern "C" void fn_80109C48(void*);
extern "C" void fn_80109C90(void*);
extern "C" void fn_80109CA8(void*);
extern "C" void fn_800FBCB0(void*, int);
extern "C" bool fn_802C2C84(const char*, bool);
extern "C" void* fn_802B6594(int, void*);
extern "C" bool fn_802C2DBC(const char*);
extern "C" const char* fn_802C2D20(const char*, const char*);
extern "C" int fn_802C2BE8(const char*, int);
extern "C" int fn_801CBED0(const char*);
extern "C" int fn_801CBEF8(const char*);
extern "C" void nlBreak__Fv();
extern "C" void* fn_8010D6F8(void*, void*);
extern "C" void* fn_8010D794(void*, void*);
extern void* lbl_806DF248;
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
{
    fn_80109BC4(&mCurGameSettings);
    fn_80109BC4(&mDefaultSettings);
    fn_80109BC4(&mMode1Settings);
    fn_80109BC4(&mNoCheatSettings);
    fn_80109C48(&mRulesA);
    unknown_0x9C = 0;
    fn_80109890(&unknown_0xA0);
    fn_80109CA8(&mVisualOptions);
    fn_80109BC4(&mBaseSettings);
    fn_80109C48(&mRulesB);
    fn_80109BC4(&unknown_0xE8);
    fn_80109C48(&unknown_0x104);
    unknown_0x110[0] = 0;
    unknown_0x110[1] = 0;
    unknown_0x110[2] = 0;
    unknown_0x110[3] = 0;
    unknown_0x110[4] = 0;
    mCurrentMode = -1;
    unknown_0x120 = 0;
    unknown_0x121 = 1;
    unknown_0x122 = 0;
    unknown_0x123 = 0;
    unknown_0x124 = 0;
    unknown_0x125 = 0;
    unknown_0x126[0] = 0;
    unknown_0x278 = 0;
    fn_80109890(&unknown_0x280);

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

    if (fn_802C2C84("User/skipfe", false)) {
        SetMode(GM_FRIENDLY, false);
        SetTeam(0, 3);
        SetSidekick(0, 1, -1);
        SetTeam(1, 0);
        SetSidekick(1, 0, -1);
        SetStadium(13);
    }

    unknown_0x9C = fn_802B6594(-1, &lbl_806DF248);
    unknown_0x110[0] = 0;
    unknown_0x110[1] = 0;
    unknown_0x110[2] = 0;
    unknown_0x110[3] = 0;
    unknown_0x110[4] = 0;

    for (int i = 0; i < 4; i++) {
        memset(&unknown_0x128[i], 0, sizeof(GameInfoSlotEntry));
        unknown_0x128[i].unknown_0x4C = i;
        unknown_0x128[i].unknown_0x50 = 2;
    }

    fn_80109C40(&mBaseSettings);
    fn_80109C90(&mRulesB);

    mDefaultSettings.unknown_0x00 = 1;
    mDefaultSettings.unknown_0x10 = 3;
    mDefaultSettings.unknown_0x04 = 0;
    mDefaultSettings.unknown_0x0C = 5;
    mDefaultSettings.unknown_0x08 = 0xB4;
    mMode1Settings.unknown_0x00 = 1;
    mMode1Settings.unknown_0x10 = 1;
    mMode1Settings.unknown_0x04 = 0;
    mMode1Settings.unknown_0x0C = 5;
    mMode1Settings.unknown_0x08 = 0xB4;
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

int GameInfoManager::GetSidekick(short side, short slot) const
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

void GameInfoManager::SetMode(int mode, u8 flag)
{
    mCurrentMode = mode;
    unknown_0x122 = 0;
    unknown_0x120 = flag;
}

void* GameInfoManager::GetUnknown806E0F90Block() const
{
    return (u8*)fn_8010D668(lbl_806E0F90) + 0x144;
}

unsigned long GameInfoManager::GetMemoryCardDataSize() const
{
    return sizeof(mSaveSlots);
}

void GameInfoManager::SerializeSettings(void* data) const
{
    memcpy(data, &unknown_0x9C, 0x80);
    data = (u8*)data + 0x80;
    memcpy(data, mRulesTable, sizeof(mRulesTable));
    lbl_806E0FA0->SerializeData(fn_8010D6F8(lbl_806E0F90, (u8*)data + sizeof(mRulesTable)));
}

void GameInfoManager::GetMemoryCardData(void* data) const
{
    memcpy(data, mSaveSlots, sizeof(mSaveSlots));
}

void GameInfoManager::DeserializeSettings(void* data)
{
    memcpy(&unknown_0x9C, data, 0x80);
    data = (u8*)data + 0x80;
    memcpy(mRulesTable, data, sizeof(mRulesTable));
    lbl_806E0FA0->DeserializeData(fn_8010D794(lbl_806E0F90, (u8*)data + sizeof(mRulesTable)));
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

void* GameInfoManager::GetUnknown0xA0()
{
    return unknown_0xA0;
}

void* GameInfoManager::GetUnknown0xB8()
{
    return &mVisualOptions;
}

const GameSettings* GameInfoManager::GetCurrentSettings() const
{
    if (unknown_0x27C) {
        const GameSettings* settings = &mCurGameSettings;

        return settings;
    }

    if (unknown_0x120 != 0) {
        if (unknown_0x124 != 0) {
            if (mCurrentMode == GM_MODE_1) {
                const GameSettings* settings = &mMode1Settings;

                return settings;
            }

            const GameSettings* settings = &mDefaultSettings;

            return settings;
        }

        const GameSettings* settings = &mNoCheatSettings;

        return settings;
    }

    if (mCurrentMode == GM_MODE_3) {
        const GameSettings* settings =
            (const GameSettings*)&lbl_806E0F90->mCurrentCup->mCupSettings;

        return settings;
    }

    const GameSettings* settings = &mBaseSettings;

    return settings;
}

const GameRules* GameInfoManager::GetActiveRules() const
{
    if (UseAltRules()) {
        return &mRulesA;
    }

    return &mRulesB;
}

void GameInfoManager::ResetUnknown0xA0()
{
    unknown_0x27C = 0;
    ((AudioSettings*)unknown_0xA0)->ApplySettings();
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
    const GameSettings* settings = GetCurrentSettings();

    mCurGameSettings.unknown_0x00 = settings->unknown_0x00;
    mCurGameSettings.unknown_0x04 = settings->unknown_0x04;
    mCurGameSettings.unknown_0x08 = settings->unknown_0x08;
    mCurGameSettings.unknown_0x0C = settings->unknown_0x0C;
    mCurGameSettings.unknown_0x10 = settings->unknown_0x10;
    mCurGameSettings.unknown_0x14 = settings->unknown_0x14;
    mCurGameSettings.unknown_0x15 = settings->unknown_0x15;
    mCurGameSettings.unknown_0x16 = settings->unknown_0x16;
    mCurGameSettings.unknown_0x17 = settings->unknown_0x17;
    mCurGameSettings.unknown_0x18 = settings->unknown_0x18;
    mCurGameSettings.unknown_0x19 = settings->unknown_0x19;
    mCurGameSettings.unknown_0x1A = settings->unknown_0x1A;
    unknown_0x27C = 1;

    for (int side = 0; side < 2; side++) {
        nlSNPrintf(name, sizeof(name), "user/team%d", side + 1);

        if (fn_802C2DBC(name)) {
            SetTeam(side, fn_801CBED0(fn_802C2D20(name, kDefaultTeam)));
        }

        for (int sidekick = 0; sidekick < 3; sidekick++) {
            nlSNPrintf(name, sizeof(name), side == 0 ? "user/sidekickhome%d" : "user/sidekickaway%d", sidekick);

            if (fn_802C2DBC(name)) {
                SetSidekick(side,
                    fn_801CBEF8(fn_802C2D20(name, side == 0 ? kDefaultHomeSidekick : kDefaultAwaySidekick)),
                    sidekick);
            }
        }
    }

    if (fn_802C2DBC("user/soak_diff")) {
        mCurGameSettings.unknown_0x00 = fn_802C2BE8("user/soak_diff", 2);
    }

    for (int pad = 0; pad < 4; pad++) {
        nlSNPrintf(padName, sizeof(padName), "user/pad%d", pad);

        if (fn_802C2DBC(padName)) {
            const char* value = fn_802C2D20(padName, kHomeSide);

            if (nlStrNCmp(value, kHomeSide, sizeof(padName)) == 0) {
                SetPlayingSide(pad, 0);
            } else if (nlStrNCmp(value, kAwaySide, sizeof(padName)) == 0) {
                SetPlayingSide(pad, 1);
            }
        }
    }

    if (mCurrentMode == GM_MODE_2) {
        mCurGameSettings.unknown_0x08 = 0x78;
        mCurGameSettings.unknown_0x00 = 2;
    } else if (unknown_0x122) {
        mCurGameSettings.unknown_0x08 = 0xEA24;
    } else if (g_e3_Build) {
        mCurGameSettings.unknown_0x08 = 0xB4;
        mCurGameSettings.unknown_0x00 = 1;
    } else if (mCurrentMode == GM_MODE_4) {
        UnidentifiedStrikerChallenge* other = lbl_806E0FA0;

        mCurGameSettings.unknown_0x08 = other->mRemainingTime;
        mCurGameSettings.unknown_0x14 = other->mHomePowerupsDisabled;
        mCurGameSettings.unknown_0x15 = other->mAwayPowerupsDisabled;
        mCurGameSettings.unknown_0x16 = other->mHomeMegastrikeDisabled;
        mCurGameSettings.unknown_0x17 = other->mAwayMegastrikeDisabled;
        mCurGameSettings.unknown_0x18 = other->mHomeSkillshotDisabled;
        mCurGameSettings.unknown_0x19 = other->mAwaySkillshotDisabled;
        mCurGameSettings.unknown_0x04 = 0;
        mCurGameSettings.unknown_0x00 = other->mAIDifficulty;
        mCurGameSettings.unknown_0x10 = 1;
    }

    if (fn_802C2C84("User/skipfe", false)) {
        mCurGameSettings.unknown_0x10 = 11;
    }

    if (fn_802C2DBC("User/stadium")) {
        const char* value = fn_802C2D20("User/stadium", 0);

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

    if (fn_80338BF0(lbl_806E20D8) > 1) {
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
        skillLevel = GetCurrentSettings()->unknown_0x00;
    }

    mCurrentDifficulty[0] = DifficultyMap[skillLevel][humansOnSide[0] ? 0 : 1];
    mCurrentDifficulty[1] = DifficultyMap[skillLevel][humansOnSide[1] ? 0 : 1];
}

bool GameInfoManager::IsRule0x8Equal4() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x8 == 4;
    }

    return false;
}

bool GameInfoManager::IsRule0x0Equal10() const
{
    if (unknown_0x122) {
        return false;
    }

    if (mCurrentMode == GM_MODE_4) {
        int value = lbl_806E0FA0->mCurrentChallenge;

        switch (value) {
        case 6:
        case 7:
            return true;
        default:
            return false;
        }
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x0 == 10;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal4() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x4 == 4;
    }

    return false;
}

bool GameInfoManager::IsRule0x8Equal2() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x8 == 2;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal1() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x4 == 1;
    }

    return false;
}

bool GameInfoManager::IsRule0x8Equal3() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x8 == 3;
    }

    return false;
}

bool GameInfoManager::IsRule0x8Equal1() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x8 == 1;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal3() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x4 == 3;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal2() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x4 == 2;
    }

    return false;
}

bool GameInfoManager::IsRule0x4Equal5() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x4 == 5;
    }

    return false;
}

bool GameInfoManager::IsRule0x0Equal11() const
{
    if (unknown_0x122) {
        return false;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x0 == 11;
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

void* GameInfoManager::GetUnknown0x340(int slot, int index)
{
    return mSaveSlots[slot].unknown_0x340[index];
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
        index = GetCurrentSettings()->unknown_0x00;
    }

    return table[index];
}

int GameInfoManager::GetRule0x0() const
{
    if (unknown_0x122) {
        return 0;
    }

    if ((unknown_0x120 == 0 && mCurrentMode == GM_FRIENDLY) || UseAltRules()) {
        return GetActiveRules()->unknown_0x0;
    }

    if (mCurrentMode == GM_MODE_4) {
        return lbl_806E0FA0->mCustomPowerups;
    }

    return 0;
}

void GameInfoManager::ResetRules(int index)
{
    mRulesTable[index] = kDefaultRules[index];
}
