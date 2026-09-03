#ifndef GAME_DB_STADIUMINFO_H
#define GAME_DB_STADIUMINFO_H

#include "types.h"

/**
 * Charged's static stadium table. R4QE01 holds 18 entries: the fifteen
 * playable stadiums, the tutorial field, and a low-poly test stadium that the
 * start-up EnableAllStadiums call leaves disabled.
 *
 * Only the fields the retained lookups establish are named; the rest keep
 * offset-derived names because the stripped DOL does not preserve them.
 */
struct StadiumInfo
{
    /* 0x00 */ int mIndex;
    /* 0x04 */ const char* mName;
    /* 0x08 */ int unknown_0x08;
    /* 0x0C */ int unknown_0x0C;
    /* 0x10 */ bool unknown_0x10;
    /* 0x11 */ bool unknown_0x11;
    /* 0x12 */ bool unknown_0x12;
    /* 0x13 */ bool unknown_0x13;
    /* 0x14 */ int unknown_0x14;
    /* 0x18 */ const char* mTerrain;
    /* 0x1C */ const char* mTickerStringID;
    /* 0x20 */ const char* mMoviePath;
    /* 0x24 */ const char* mMoviePathPAL;
    /* 0x28 */ int unknown_0x28;
    /* 0x2C */ bool unknown_0x2C;
    /* 0x2D */ bool mEnabled;
    /* 0x2E */ bool unknown_0x2E;
    /* 0x2F */ bool unknown_0x2F;
    /* 0x30 */ unsigned long mSoundID;
    /* 0x34 */ bool unknown_0x34;
    /* 0x35 */ bool unknown_0x35;
    /* 0x36 */ bool unknown_0x36;
    /* 0x37 */ bool unknown_0x37;
};

const char* GetStadiumName(int stadium);
int GetStadiumUnknown0x14(int stadium);
bool IsStadiumEnabled(int stadium);
void EnableAllStadiums();
int GetStadiumUnknown0x08(int stadium);
int GetStadiumUnknown0x0C(int stadium);
bool GetStadiumUnknown0x11(int stadium);
bool GetStadiumUnknown0x10(int stadium);
const char* GetStadiumTerrain(int stadium);
bool GetStadiumUnknown0x2C(int stadium);
bool SetStadiumUnknown0x2C(int stadium, bool value);
const char* GetStadiumTickerStringID(int stadium);
const char* GetStadiumMoviePath(int stadium);
int GetStadiumUnknown0x28(int stadium);
unsigned long GetStadiumSoundID(int stadium);
bool GetStadiumUnknown0x34(int stadium);
bool IsStadiumUnlocked(int stadium);

#endif // GAME_DB_STADIUMINFO_H
