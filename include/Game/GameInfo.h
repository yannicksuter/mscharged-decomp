#ifndef GAME_GAMEINFO_H
#define GAME_GAMEINFO_H

#include "Game/TrophyInfo.h"
#include "Game/FE/feInput.h"
#include "Game/DB/BasicGameInfo.h"
#include "Game/DB/UserOptions.h"
#include "NL/nlSingleton.h"
#include "types.h"

typedef unsigned long long u64;

struct UserInfo
{
    UserInfo()
        : mSaveID(0)
        , mNumGamesPlayed(0)
        , mNumGoalsScored(0)
        , mNumSTSAttempts(0)
        , mNumPerfectPasses(0)
        , mNumHits(0)
    {
    }

    bool IsWidescreen() const;

    /* 0x00 */ unsigned long mSaveID;
    /* 0x04 */ AudioSettings mAudioOptions;
    /* 0x1C */ VisualSettings mVisualOptions;
    /* 0x24 */ GameplaySettings mGameplayOptions;
    /* 0x40 */ CheatSettings mCheatOptions;
    /* 0x4C */ GameplaySettings mAltGameplayOptions;
    /* 0x68 */ CheatSettings mAltCheatOptions;
    /* 0x74 */ unsigned short mNumGamesPlayed;
    /* 0x76 */ unsigned short mNumGoalsScored;
    /* 0x78 */ unsigned short mNumSTSAttempts;
    /* 0x7A */ unsigned short mNumPerfectPasses;
    /* 0x7C */ unsigned short mNumHits;
}; // size 0x80

/**
 * One saved user profile. R4QE01 stores ten of them contiguously and always
 * addresses them as `base + slot * 0xB10`, which is what fixes both the record
 * size and the slot count. Unknown fields use offset-derived names; semantic
 * names describe their reconstructed use, since the stripped DOL preserves neither.
 */
struct GameInfoSaveSlot
{
    /* 0x000 */ u8 unknown_0x000[0x1C];
    /* 0x01C */ int mProfileId;
    /* 0x020 */ u8 unknown_0x020[0x20];
    /* 0x040 */ u8 mFriendData[64][0xC];
    /* 0x340 */ u16 mFriendNames[64][14];
    /* 0xA40 */ u8 mFriendRegions[0x40];
    /* 0xA80 */ u64 mSaveId;
    /* 0xA88 */ u8 mLastUsedTime[0x8];
    /* 0xA90 */ int mPastSeasonWins;
    /* 0xA94 */ int mPastSeasonLosses;
    /* 0xA98 */ int mCurrentSeasonWins;
    /* 0xA9C */ int mCurrentSeasonLosses;
    /* 0xAA0 */ int mUnrankedWins;
    /* 0xAA4 */ int mUnrankedLosses;
    /* 0xAA8 */ u8 mMiiName[0x16];
    /* 0xABE */ u8 mMiiData[0x52];
};

struct GameInfoSlotEntry
{
    /* 0x00 */ PlayerStats mStats;
};

/**
 * Charged keeps the predecessor's GameInfoManager singleton but rebuilds its
 * storage: five per-mode BasicGameInfo slots, several rule blocks, and one
 * large contiguous save-data block.
 *
 * UserInfo retains the serialized settings record. The per-controller and
 * saved-profile records still contain fields whose individual roles are unknown.
 */
class GameInfoManager : public nlSingleton<GameInfoManager>
{
public:
    enum eGameMode
    {
        GM_FRIENDLY = 0,
        GM_ONLINE_TOURNAMENT = 1,
        GM_DEMO = 2,
        GM_CUP = 3,
        GM_CHALLENGE = 4,
        GM_NUM_MODES = 5,
    };

    GameInfoManager();
    virtual ~GameInfoManager();

    int GetTeam(short side) const;
    void SetTeam(short side, int team);
    int GetSidekick(short side, int slot) const;
    void SetSidekick(short side, int sidekick, int slot);
    int GetStadium() const;
    void SetStadium(int stadium);
    short GetPlayingSide(unsigned short pad) const;
    void SetPlayingSide(unsigned short pad, short side);
    void ResetPlayingSides();
    void SetMode(int mode, bool online);

    unsigned long GetMemoryCardDataSize() const;
    void GetMemoryCardData(void* data) const;
    void SetMemoryCardData(const void* data);
    bool HasTrophy(eTrophyType trophyType) const;

    bool IsInCupMode() const;
    bool IsInCupOrTournamentMode() const;
    bool IsInDemoMode() const;
    bool IsInFriendlyMode() const;
    bool IsInOnlineTournamentMode() const;
    bool IsInChallengeMode() const;

    unsigned long GetSettingsDataSize() const;
    void SerializeSettings(void* data) const;
    void DeserializeSettings(void* data);
    const GameplaySettings* GetCurrentSettings() const;
    int GetDifficulty(short side) const { return mCurrentDifficulty[side]; }

    int FindSaveSlot(u64 id) const;
    bool HasSaveSlot(u64 id) const;
    int GetSaveSlotName(int index) const;
    void ValidateSaveSlot(int index);
    int GetGameplaySkillLevel();
    int GetCustomPowerups() const;
    void ResetSidekickChoices(int index);
    void SetSidekickChoices(int index, CupSidekicks choices) { mSidekickChoices[index] = choices; }

    bool IsOnline() const { return mIsOnlineMode; }

    bool UseAltRules() const { return mIsOnlineMode != 0 && mOnlineRankedMatch == 0; }
    const CheatSettings* GetActiveRules() const;

    void SetupGameFromConfig();
    void ApplyDifficultySettings();

    bool IsGlassJawEnabled() const;
    bool AreInfinitePowerupsEnabled() const;
    bool IsScoreTiltEnabled() const;
    bool IsSafeMegaStrikeEnabled() const;
    bool IsSecureStadiumEnabled() const;
    bool IsSkillShotCheatEnabled() const;
    bool AreDevastatingHitsEnabled() const;
    bool IsHighVoltageEnabled() const;
    bool IsPowerShortageEnabled() const;
    bool IsWhiteBallEnabled() const;
    bool IsButterfingersEnabled() const;

    UserInfo& GetUserInfo();
    int GetCurrentMode() const;
    AudioSettings* GetAudioSettings();
    VisualSettings* GetVisualOptions();
    void ClearGameSettingsOverride();

    GameInfoSaveSlot* GetSaveSlot(int slot);
    void* GetSavedFriendData(int slot, int index);
    void* GetSaveSlotIdAddress(int slot);
    u16* GetSavedFriendName(int slot, int index);
    void* GetSaveSlotLastUsedTime(int slot);
    void* GetSavedFriendRegion(int slot, int index);
    int* GetPastSeasonWins(int slot);
    int* GetPastSeasonLosses(int slot);
    int* GetCurrentSeasonWins(int slot);
    int* GetCurrentSeasonLosses(int slot);
    int* GetUnrankedWins(int slot);
    int* GetUnrankedLosses(int slot);
    int GetTotalRankedWins(int index);
    int GetTotalRankedLosses(int index);
    int GetTotalOnlineWins(int index);
    int GetTotalOnlineLosses(int index);
    void* GetSavedMiiName(int slot);
    void* GetSavedMiiData(int slot);
    void ClearSaveSlot(int slot);

    BasicGameInfo* GetCurrentGameInfo() const { return mGameInfo[mCurrentMode]; }

    /* 0x0004 */ GameplaySettings mCurGameGameplayOptions;
    /* 0x0020 */ GameplaySettings mDefaultSettings;
    /* 0x003C */ GameplaySettings mOnlineTournamentSettings;
    /* 0x0058 */ GameplaySettings mUnrankedGameplaySettings;
    /* 0x0074 */ CheatSettings mUnrankedCheatSettings;
    /* 0x0080 */ BasicGameInfo* mGameInfo[GM_NUM_MODES];
    /* 0x0094 */ int mCurrentDifficulty[2];
    /* 0x009C */ UserInfo mUserInfo;
    /* 0x011C */ int mCurrentMode;
    /* 0x0120 */ bool mIsOnlineMode;
    /* 0x0121 */ u8 mSoakDemoMatchEnabled;
    /* 0x0122 */ u8 mIsInStrikers101Mode;
    /* 0x0123 */ u8 unknown_0x123;
    /* 0x0124 */ bool mOnlineRankedMatch;
    /* 0x0125 */ bool mOnlineTwoLocalPlayers;
    /* 0x0126 */ bool mOnlineFriendSelectionMode;
    /* 0x0127 */ u8 unknown_0x127;
    /* 0x0128 */ GameInfoSlotEntry mUserStats[4];
    /* 0x0278 */ eFEINPUT_PAD mMainUserPadNumber;
    /* 0x027C */ u8 mUseCurGameSettings;
    /* 0x027D */ u8 unknown_0x27D[3];
    /* 0x0280 */ AudioSettings mCurGameAudioSettings;
    /* 0x0298 */ CupSidekicks mSidekickChoices[12];
    /* 0x0328 */ GameInfoSaveSlot mSaveSlots[10];
    /* 0x71C8 */ int mFrontEndReturnMode;
    /* 0x71CC */ u8 unknown_0x71CC[4];

    static GameInfoManager* sThis;
};

void SetOnlineRankedMatch(bool value);

bool IsOnlineRankedMatch();

void SetOnlineTwoLocalPlayers(bool value);

bool HasOnlineTwoLocalPlayers();

void SetOnlineFriendSelectionMode(bool value);

bool IsOnlineFriendSelectionMode();

void SetOnlineFriendSelectionContext(void* context);

void FormatFriendKey(unsigned long long friendKey, u16* output);

extern void* gOnlineFriendSelectionContext;

#endif // GAME_GAMEINFO_H
