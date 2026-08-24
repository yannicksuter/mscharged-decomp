#ifndef GAME_DB_CHARACTERINFO_H
#define GAME_DB_CHARACTERINFO_H

#include "types.h"

/**
 * The eight-byte pair at CharacterInfo+0x30. R4QE01 copies it as two raw words
 * rather than element-wise, which is what shows it is a struct and not a float
 * array: the twelve goalie rows share one runtime-initialised value.
 */
struct CharacterValuePair
{
    /* 0x0 */ float unknown_0x0;
    /* 0x4 */ float unknown_0x4;
};

/**
 * Charged's static character database entry. R4QE01 holds 33 of them: twelve
 * captains, eight sidekicks, twelve goalies and one INVALID fallback that every
 * out-of-range lookup returns.
 *
 * Only the fields the retained lookups actually read are named; the rest keep
 * offset-derived names because the stripped DOL does not preserve them.
 */
struct CharacterInfo
{
    /* 0x00 */ int mIndex;
    /* 0x04 */ const char* mName;
    /* 0x08 */ const char* mDisplayNameKey;
    /* 0x0C */ int unknown_0x0C;
    /* 0x10 */ int mCaptainId;
    /* 0x14 */ int unknown_0x14;
    /* 0x18 */ int mSidekickId;
    /* 0x1C */ int unknown_0x1C;
    /* 0x20 */ int unknown_0x20;
    /* 0x24 */ int unknown_0x24;
    /* 0x28 */ int unknown_0x28;
    /* 0x2C */ int unknown_0x2C;
    /* 0x30 */ CharacterValuePair unknown_0x30;
    /* 0x38 */ float unknown_0x38;
    /* 0x3C */ float unknown_0x3C;
    /* 0x40 */ float unknown_0x40;
    /* 0x44 */ float unknown_0x44;
    /* 0x48 */ int unknown_0x48;
    /* 0x4C */ int mColourMask;
    /* 0x50 */ int mColourRank;
    /* 0x54 */ int mPrimaryColour;
    /* 0x58 */ int mAlternateColour;
};

const CharacterInfo& GetCharacterInfo(int index);
int GetCharacterIndexFromCaptain(int captain);
int GetCharacterIndexFromSidekick(int sidekick);
int GetCharacterIndexFromName(const char* name);
int GetGoalieCharacterIndex(const CharacterInfo& character);
u32 GetTeamColour(const CharacterInfo& team, const CharacterInfo& opponent, bool useAlternate);
bool NeedsAlternateColour(const CharacterInfo& team, const CharacterInfo& opponent);
bool CaptainsNeedAlternateColour(int captain, int opponentCaptain);

#endif // GAME_DB_CHARACTERINFO_H
