#ifndef GAME_GAMEINFO_H
#define GAME_GAMEINFO_H

#include "Game/DB/BasicGameInfo.h"
#include "Game/DB/UserOptions.h"
#include "NL/nlSingleton.h"
#include "types.h"

typedef unsigned long long u64;

/**
 * The 0x1C-byte option block selected by GameInfoManager::GetCurrentSettings.
 * R4QE01 copies it field by field in the config setup, which is what fixes the
 * five leading words and the seven trailing bytes.
 */
struct GameSettings
{
    /* 0x00 */ int unknown_0x00;
    /* 0x04 */ int unknown_0x04;
    /* 0x08 */ int unknown_0x08;
    /* 0x0C */ int unknown_0x0C;
    /* 0x10 */ int unknown_0x10;
    /* 0x14 */ u8 unknown_0x14;
    /* 0x15 */ u8 unknown_0x15;
    /* 0x16 */ u8 unknown_0x16;
    /* 0x17 */ u8 unknown_0x17;
    /* 0x18 */ u8 unknown_0x18;
    /* 0x19 */ u8 unknown_0x19;
    /* 0x1A */ u8 unknown_0x1A;
    /* 0x1B */ u8 unknown_0x1B;
};

/**
 * The 0xC-byte rule block selected by GameInfoManager::GetActiveRules. R4QE01
 * only ever compares its three words against small enum constants, so the
 * fields keep offset-derived names.
 */
struct GameRules
{
    /* 0x0 */ int unknown_0x0;
    /* 0x4 */ int unknown_0x4;
    /* 0x8 */ int unknown_0x8;
};

/**
 * One saved user profile. R4QE01 stores ten of them contiguously and always
 * addresses them as `base + slot * 0xB10`, which is what fixes both the record
 * size and the slot count. Field names are offset-derived: the stripped DOL
 * does not preserve them.
 */
struct GameInfoSaveSlot
{
    /* 0x000 */ u8 unknown_0x000[0x40];
    /* 0x040 */ u8 unknown_0x040[64][0xC];
    /* 0x340 */ u8 unknown_0x340[64][0x1C];
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

/**
 * Charged keeps the predecessor's GameInfoManager singleton but rebuilds its
 * storage: five per-mode BasicGameInfo slots, several rule blocks, and one
 * large contiguous save-data block.
 *
 * The sub-object types at 0x04, 0x74, 0xA0, 0xB8, 0x128 and 0x298 are owned by
 * translation units that are not reconstructed yet, so they are kept as sized
 * storage rather than invented declarations.
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
    int GetSidekick(short side, short slot) const;
    void SetSidekick(short side, int sidekick, int slot);
    int GetStadium() const;
    void SetStadium(int stadium);
    short GetPlayingSide(unsigned short pad) const;
    void SetPlayingSide(unsigned short pad, short side);
    void ResetPlayingSides();
    void SetMode(int mode, u8 flag);

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
    const GameSettings* GetCurrentSettings() const;

    int FindSaveSlot(u64 id) const;
    bool HasSaveSlot(u64 id) const;
    int GetSaveSlotName(int index) const;
    void ValidateSaveSlot(int index);
    int GetMappedRule0x0() const;
    int GetRule0x0() const;
    void ResetRules(int index);

    bool UseAltRules() const { return unknown_0x120 != 0 && unknown_0x124 == 0; }
    const GameRules* GetActiveRules() const;

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

    void* GetUnknown0xA0();
    void* GetUnknown0xB8();
    void ResetUnknown0xA0();

    GameInfoSaveSlot* GetSaveSlot(int slot);
    void* GetUnknown0x40(int slot, int index);
    void* GetUnknown0xA80(int slot);
    void* GetUnknown0x340(int slot, int index);
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

    /* 0x0004 */ GameSettings mCurGameSettings;
    /* 0x0020 */ GameSettings mDefaultSettings;
    /* 0x003C */ GameSettings mMode1Settings;
    /* 0x0058 */ GameSettings mNoCheatSettings;
    /* 0x0074 */ GameRules mRulesA;
    /* 0x0080 */ BasicGameInfo* mGameInfo[GM_NUM_MODES];
    /* 0x0094 */ int mCurrentDifficulty[2];
    /* 0x009C */ void* unknown_0x9C;
    /* 0x00A0 */ u8 unknown_0xA0[0x18];
    /* 0x00B8 */ VisualSettings mVisualOptions;
    /* 0x00C0 */ GameSettings mBaseSettings;
    /* 0x00DC */ GameRules mRulesB;
    /* 0x00E8 */ GameSettings unknown_0xE8;
    /* 0x0104 */ GameRules unknown_0x104;
    /* 0x0110 */ s16 unknown_0x110[5];
    /* 0x011A */ u8 unknown_0x11A[2];
    /* 0x011C */ int mCurrentMode;
    /* 0x0120 */ u8 unknown_0x120;
    /* 0x0121 */ u8 unknown_0x121;
    /* 0x0122 */ u8 unknown_0x122;
    /* 0x0123 */ u8 unknown_0x123;
    /* 0x0124 */ u8 unknown_0x124;
    /* 0x0125 */ u8 unknown_0x125;
    /* 0x0126 */ u8 unknown_0x126[2];
    /* 0x0128 */ GameInfoSlotEntry unknown_0x128[4];
    /* 0x0278 */ int unknown_0x278;
    /* 0x027C */ u8 unknown_0x27C;
    /* 0x027D */ u8 unknown_0x27D[3];
    /* 0x0280 */ u8 unknown_0x280[0x18];
    /* 0x0298 */ GameRules mRulesTable[12];
    /* 0x0328 */ GameInfoSaveSlot mSaveSlots[10];
    /* 0x71C8 */ int unknown_0x71C8;
    /* 0x71CC */ u8 unknown_0x71CC[4];

    static GameInfoManager* sThis;
};

#endif // GAME_GAMEINFO_H
