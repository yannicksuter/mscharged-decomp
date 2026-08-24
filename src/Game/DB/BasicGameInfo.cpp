#include "Game/DB/BasicGameInfo.h"

#include "Game/DB/CharacterInfo.h"
#include "NL/nlString.h"
#include "types.h"

enum
{
    NUM_CHARACTERS = 32,
};

static CharacterValuePair kGoalieValues = { 1.5f, 2.0f };

static CharacterInfo sCharacterInfo[NUM_CHARACTERS + 1] = {
    { 0, "mario", "NAME_MARIO", 2, 0, 9, -1, 7, 7, 1, 0, 0,
      { 0.25f, 1.0f }, 0.6f, 0.6f, 0.6f, 0.6f, 4, 0x00000001, 1, 0x00ED0012, 0x00000000 },
    { 1, "bowser", "NAME_BOWSER", 2, 1, 12, -1, 1, 0, 1, 0, 2,
      { 0.25f, 1.4f }, 0.25f, 0.95f, 0.25f, 0.95f, 3, 0x00000001, 7, 0x00E6431A, 0x00FFF100 },
    { 2, "daisy", "NAME_DAISY", 2, 2, 14, -1, 3, 3, 1, 0, 1,
      { 0.25f, 1.0f }, 0.95f, 0.25f, 0.25f, 0.95f, 1, 0x00000011, 13, 0x00DD6B08, 0x0059C6A7 },
    { 3, "donkeykong", "NAME_DK", 2, 3, 11, -1, 5, 5, 1, 0, 2,
      { 0.25f, 1.4f }, 0.25f, 0.95f, 0.25f, 0.95f, 3, 0x00000010, 6, 0x00E9C435, 0x008B00CF },
    { 4, "luigi", "NAME_LUIGI", 2, 4, 13, -1, 6, 6, 1, 0, 0,
      { 0.25f, 1.0f }, 0.6f, 0.6f, 0.6f, 0.6f, 4, 0x00000002, 2, 0x002B7117, 0x00000000 },
    { 5, "peach", "NAME_PEACH", 2, 5, 10, -1, 8, 8, 1, 0, 1,
      { 0.25f, 1.0f }, 0.95f, 0.25f, 0.95f, 0.25f, 2, 0x00000004, 5, 0x00E3578C, 0x005DB9FF },
    { 6, "waluigi", "NAME_WALUIGI", 2, 6, 16, -1, 9, 9, 1, 0, 1,
      { 0.4f, 1.0f }, 0.95f, 0.25f, 0.25f, 0.95f, 1, 0x00000008, 4, 0x00501592, 0x00000000 },
    { 7, "wario", "NAME_WARIO", 2, 7, 15, -1, 10, 10, 1, 0, 0,
      { 0.25f, 1.25f }, 0.25f, 0.95f, 0.95f, 0.25f, 0, 0x00000010, 3, 0x00FFCB05, 0x00000000 },
    { 8, "yoshi", "NAME_YOSHI", 2, 8, 19, -1, 11, 11, 1, 0, 0,
      { 0.25f, 1.0f }, 0.6f, 0.6f, 0.6f, 0.6f, 4, 0x00000002, 9, 0x0034A631, 0x00F75D05 },
    { 9, "bowserjr", "NAME_BOWSERJR", 2, 9, 17, -1, 2, 1, 1, 0, 0,
      { 0.25f, 1.0f }, 0.25f, 0.95f, 0.95f, 0.25f, 0, 0x00000001, 10, 0x00E6431A, 0x00FFF100 },
    { 10, "diddykong", "NAME_DIDDYKONG", 2, 10, 18, -1, 4, 4, 1, 0, 0,
      { 0.25f, 1.0f }, 0.95f, 0.25f, 0.95f, 0.25f, 2, 0x00000011, 14, 0x00FFA800, 0x009000D5 },
    { 11, "petey", "NAME_PETEY", 2, 11, 20, -1, 21, 12, 1, 0, 2,
      { 0.75f, 1.4f }, 0.25f, 0.95f, 0.25f, 0.95f, 3, 0x00000001, 12, 0x00E6431A, 0x00FFF100 },
    { 12, "birdo", "NAME_BIRDO", 2, -1, -1, 3, 20, 18, 1, 0, 0,
      { 0.25f, 1.25f }, 0.25f, 0.95f, 0.25f, 0.95f, 3, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 13, "hammerbro", "NAME_HAMMERBROS", 2, -1, -1, 2, 15, 17, 1, 0, 0,
      { 0.25f, 1.0f }, 0.25f, 0.95f, 0.95f, 0.25f, 0, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 14, "koopa", "NAME_KOOPA", 2, -1, -1, 1, 16, 13, 1, 0, 0,
      { 0.25f, 1.0f }, 0.6f, 0.6f, 0.6f, 0.6f, 4, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 15, "toad", "NAME_TOAD", 2, -1, -1, 0, 19, 14, 1, 0, 0,
      { 0.25f, 1.0f }, 0.95f, 0.25f, 0.95f, 0.25f, 2, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 16, "boo", "NAME_BOO", 2, -1, -1, 4, 12, 15, 1, 0, 0,
      { 0.25f, 1.0f }, 0.95f, 0.25f, 0.95f, 0.25f, 2, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 17, "drybones", "NAME_DRYBONES", 2, -1, -1, 5, 14, 16, 1, 0, 0,
      { 0.25f, 1.0f }, 0.95f, 0.25f, 0.25f, 0.95f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 18, "montymole", "NAME_MONTY", 2, -1, -1, 6, 17, 19, 1, 0, 0,
      { 0.25f, 1.25f }, 0.25f, 0.95f, 0.25f, 0.95f, 3, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 19, "shyguy", "NAME_SHYGUY", 2, -1, -1, 7, 18, 20, 1, 0, 0,
      { 0.25f, 1.0f }, 0.6f, 0.6f, 0.6f, 0.6f, 4, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 20, "mariogoalie", "NAME_MARIO", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 21, "bowsergoalie", "NAME_BOWSER", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 22, "daisygoalie", "NAME_DAISY", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 23, "donkeykonggoalie", "NAME_DK", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 24, "luigigoalie", "NAME_LUIGI", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 25, "peachgoalie", "NAME_PEACH", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 26, "waluigigoalie", "NAME_WALUIGI", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 27, "wariogoalie", "NAME_WARIO", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 28, "yoshigoalie", "NAME_YOSHI", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 29, "bowserjrgoalie", "NAME_BOWSERJR", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 30, "diddykonggoalie", "NAME_DIDDYKONG", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { 31, "peteygoalie", "NAME_UNKNOWN", 3, -1, -1, -1, 13, -1, 0, 0, 0,
      kGoalieValues, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
    { -1, "INVALID", "OFF", 0, -1, -1, -1, 0, -1, 0, 0, 0,
      { 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0x00000000, 0, 0x00000000, 0x00000000 },
};

BasicGameInfo::BasicGameInfo()
{
    mTeamIndex[0] = 3;
    mSidekickIndex[0][0] = 1;
    mSidekickIndex[0][1] = 1;
    mSidekickIndex[0][2] = 1;
    mTeamIndex[1] = 0;
    mSidekickIndex[1][0] = 0;
    mSidekickIndex[1][1] = 0;
    mSidekickIndex[1][2] = 0;
    unknown_0x126 = 0;
    unknown_0x124 = 0;

    for (int pad = 0; pad < 16; pad++) {
        mPadSides[pad] = -1;
    }

    mStadiumIndex = 13;
}

void BasicGameInfo::Reset(bool clearTeams)
{
    if (clearTeams) {
        mTeamIndex[0] = -1;
    } else {
        mTeamIndex[0] = 3;
    }

    mSidekickIndex[0][0] = 1;
    mSidekickIndex[0][1] = 1;
    mSidekickIndex[0][2] = 1;

    if (clearTeams) {
        mTeamIndex[1] = -1;
    } else {
        mTeamIndex[1] = 0;
    }

    mSidekickIndex[1][0] = 0;
    mSidekickIndex[1][1] = 0;
    mSidekickIndex[1][2] = 0;
    unknown_0x126 = 0;
    unknown_0x124 = 0;

    for (int pad = 0; pad < 16; pad++) {
        mPadSides[pad] = -1;
    }

    mStadiumIndex = 13;
}

const CharacterInfo& GetCharacterInfo(int index)
{
    if (index < 0 || index >= NUM_CHARACTERS) {
        return sCharacterInfo[NUM_CHARACTERS];
    }

    return sCharacterInfo[index];
}

int GetCharacterIndexFromCaptain(int captain)
{
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        if (captain == sCharacterInfo[i].mCaptainId) {
            return sCharacterInfo[i].mIndex;
        }
    }

    return -1;
}

int GetCharacterIndexFromSidekick(int sidekick)
{
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        if (sidekick == sCharacterInfo[i].mSidekickId) {
            return sCharacterInfo[i].mIndex;
        }
    }

    return -1;
}

int GetCharacterIndexFromName(const char* name)
{
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        if (nlStrICmp(sCharacterInfo[i].mName, name) == 0) {
            return sCharacterInfo[i].mIndex;
        }
    }

    return -1;
}

int GetGoalieCharacterIndex(const CharacterInfo& character)
{
    switch (character.mIndex) {
    case 0:
        return 20;
    case 1:
        return 21;
    case 2:
        return 22;
    case 3:
        return 23;
    case 4:
        return 24;
    case 5:
        return 25;
    case 6:
        return 26;
    case 7:
        return 27;
    case 8:
        return 28;
    case 9:
        return 29;
    case 10:
        return 30;
    case 11:
        return 31;
    default:
        return -1;
    }
}

u32 GetTeamColour(const CharacterInfo& team, const CharacterInfo& opponent, bool useAlternate)
{
    u8 red;
    u8 green;
    u8 blue;
    u8 rgba[4];

    if (useAlternate && NeedsAlternateColour(team, opponent)) {
        red = (u8)((team.mAlternateColour >> 16) & 0xFF);
        green = (u8)((team.mAlternateColour >> 8) & 0xFF);
        blue = (u8)(team.mAlternateColour & 0xFF);
    } else {
        red = (u8)((team.mPrimaryColour >> 16) & 0xFF);
        green = (u8)((team.mPrimaryColour >> 8) & 0xFF);
        blue = (u8)(team.mPrimaryColour & 0xFF);
    }

    rgba[0] = red;
    rgba[1] = green;
    rgba[2] = blue;
    rgba[3] = 0xFF;

    return *(u32*)rgba;
}

bool NeedsAlternateColour(const CharacterInfo& team, const CharacterInfo& opponent)
{
    if ((team.mColourMask & opponent.mColourMask) != 0 && team.mColourRank > opponent.mColourRank) {
        return true;
    }

    return false;
}

bool CaptainsNeedAlternateColour(int captain, int opponentCaptain)
{
    CharacterInfo team = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    CharacterInfo opponent = GetCharacterInfo(GetCharacterIndexFromCaptain(opponentCaptain));

    if ((team.mColourMask & opponent.mColourMask) != 0 && team.mColourRank > opponent.mColourRank) {
        return true;
    }

    return false;
}
