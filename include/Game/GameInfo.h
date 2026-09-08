#ifndef GAME_GAMEINFO_H
#define GAME_GAMEINFO_H

#include "Game/DB/BasicGameInfo.h"
#include "Game/DB/UserOptions.h"
#include "NL/nlSingleton.h"
#include "types.h"

typedef unsigned long long u64;

// Serialized rule presets.
struct GameRules
{
    /* 0x0 */ int unknown_0x0;
    /* 0x4 */ int unknown_0x4;
    /* 0x8 */ int unknown_0x8;
};

/**
 * One saved user profile. R4QE01 stores ten of them contiguously and always
 * addresses them as `base + slot * 0xB10`, which is what fixes both the record
 * size and the slot count. Unidentified fields retain offset-derived names.
 */
struct GameInfoSaveSlot
{
    /* 0x000 */ u8 unknown_0x000[0x1C];
    /* 0x01C */ int unknown_0x01C;
    /* 0x020 */ u8 unknown_0x020[0x20];
    /* 0x040 */ u8 unknown_0x040[64][0xC];
    /* 0x340 */ u16 mFriendNames[64][14];
    /* 0xA40 */ u8 unknown_0xA40[0x40];
    /* 0xA80 */ u64 mSaveId;
    /* 0xA88 */ u8 unknown_0xA88[0x8];
    /* 0xA90 */ int unknown_0xA90;
    /* 0xA94 */ int unknown_0xA94;
    /* 0xA98 */ int unknown_0xA98;
    /* 0xA9C */ int unknown_0xA9C;
    /* 0xAA0 */ int unknown_0xAA0;
    /* 0xAA4 */ int unknown_0xAA4;
    /* 0xAA8 */ u8 unknown_0xAA8[0x16];
    /* 0xABE */ u8 unknown_0xABE[0x52];
};

/**
 * The 0x54-byte per-controller record at GameInfoManager+0x128. Only the two
 * trailing words the constructor seeds are known.
 */
struct GameInfoSlotEntry
{
    /* 0x00 */ u8 unknown_0x00[0x4C];
    /* 0x4C */ int unknown_0x4C;
    /* 0x50 */ int unknown_0x50;
};

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
    /* 0x40 */ PowerupSettings mPowerupOptions;
    /* 0x4C */ GameplaySettings mUnidentified4C;
    /* 0x68 */ PowerupSettings mUnidentified68;
    /* 0x74 */ unsigned short mNumGamesPlayed;
    /* 0x76 */ unsigned short mNumGoalsScored;
    /* 0x78 */ unsigned short mNumSTSAttempts;
    /* 0x7A */ unsigned short mNumPerfectPasses;
    /* 0x7C */ unsigned short mNumHits;
}; // size: 0x80

/**
 * Charged keeps the predecessor's GameInfoManager singleton but rebuilds its
 * storage: five per-mode BasicGameInfo slots, several rule blocks, and one
 * large contiguous save-data block.
 *
 * User option blocks use the shared settings types.
 */
class GameInfoManager : public nlSingleton<GameInfoManager>
{
public:
    enum eGameMode
    {
        GM_FRIENDLY = 0,
        GM_MODE_1 = 1,
        GM_MODE_2 = 2,
        GM_MODE_3 = 3,
        GM_MODE_4 = 4,
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
    void SetMode(int mode, u8 isOnline);

    unsigned long GetMemoryCardDataSize() const;
    void GetMemoryCardData(void* data) const;
    void SetMemoryCardData(const void* data);
    u8 CheckSaveIDChanged(const void* data) const;

    bool IsInMode3() const;
    bool IsInOddCupMode() const;
    bool IsInMode2() const;
    bool IsInFriendlyMode() const;
    bool IsInMode1() const;
    bool IsInMode4() const;

    void* GetUnknown806E0F90Block() const;
    void SerializeSettings(void* data) const;
    void DeserializeSettings(void* data);
    const GameplaySettings* GetCurrentSettings() const;

    int FindSaveSlot(u64 id) const;
    bool HasSaveSlot(u64 id) const;
    int GetSaveSlotName(int index) const;
    void ValidateSaveSlot(int index);
    int GetMappedRule0x0() const;
    int GetRule0x0() const;
    void ResetRules(int index);

    bool UseAltRules() const { return mIsOnlineMode != 0 && mOnlineRankedMatch == 0; }
    const PowerupSettings* GetActiveRules() const;

    void SetupGameFromConfig();
    void ApplyDifficultySettings();

    bool IsRule0x8Equal4() const;
    bool IsRule0x0Equal10() const;
    bool IsRule0x4Equal4() const;
    bool IsRule0x8Equal2() const;
    bool IsRule0x4Equal1() const;
    bool IsRule0x8Equal3() const;
    bool IsRule0x8Equal1() const;
    bool IsRule0x4Equal3() const;
    bool IsRule0x4Equal2() const;
    bool IsRule0x4Equal5() const;
    bool IsRule0x0Equal11() const;

    UserInfo* GetUserInfo();
    AudioSettings* GetAudioSettings();
    VisualSettings* GetVisualOptions();
    void ResetUnknown0xA0();

    GameInfoSaveSlot* GetSaveSlot(int slot);
    void* GetUnknown0x40(int slot, int index);
    void* GetUnknown0xA80(int slot);
    u16* GetSavedFriendName(int slot, int index);
    void* GetUnknown0xA88(int slot);
    void* GetUnknown0xA40(int slot, int index);
    int* GetUnknown0xA90(int slot);
    int* GetUnknown0xA94(int slot);
    int* GetUnknown0xA98(int slot);
    int* GetUnknown0xA9C(int slot);
    int* GetUnknown0xAA0(int slot);
    int* GetUnknown0xAA4(int slot);
    int GetUnknown0xA90Total(int index);
    int GetUnknown0xA94Total(int index);
    int GetUnknown0xAA0Total(int index);
    int GetUnknown0xAA4Total(int index);
    void* GetUnknown0xAA8(int slot);
    void* GetUnknown0xABE(int slot);
    void ClearSaveSlot(int slot);

    BasicGameInfo* GetCurrentGameInfo() const { return mGameInfo[mCurrentMode]; }

    /* 0x0004 */ GameplaySettings mCurGameSettings;
    /* 0x0020 */ GameplaySettings mDefaultSettings;
    /* 0x003C */ GameplaySettings mMode1Settings;
    /* 0x0058 */ GameplaySettings mNoCheatSettings;
    /* 0x0074 */ PowerupSettings mRulesA;
    /* 0x0080 */ BasicGameInfo* mGameInfo[GM_NUM_MODES];
    /* 0x0094 */ int mCurrentDifficulty[2];
    /* 0x009C */ UserInfo mUserInfo;
    /* 0x011C */ int mCurrentMode;
    /* 0x0120 */ u8 mIsOnlineMode;
    /* 0x0121 */ u8 unknown_0x121;
    /* 0x0122 */ u8 unknown_0x122;
    /* 0x0123 */ u8 unknown_0x123;
    /* 0x0124 */ bool mOnlineRankedMatch;
    /* 0x0125 */ bool mOnlineTwoLocalPlayers;
    /* 0x0126 */ bool mOnlineFriendSelectionMode;
    /* 0x0127 */ u8 unknown_0x127;
    /* 0x0128 */ GameInfoSlotEntry unknown_0x128[4];
    /* 0x0278 */ int unknown_0x278;
    /* 0x027C */ u8 unknown_0x27C;
    /* 0x027D */ u8 unknown_0x27D[3];
    /* 0x0280 */ AudioSettings unknown_0x280;
    /* 0x0298 */ GameRules mRulesTable[12];
    /* 0x0328 */ GameInfoSaveSlot mSaveSlots[10];
    /* 0x71C8 */ int unknown_0x71C8;
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
extern void* gOnlineFriendSelectionContext;

#endif // GAME_GAMEINFO_H
