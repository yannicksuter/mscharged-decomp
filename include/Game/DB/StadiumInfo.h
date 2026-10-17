#ifndef GAME_DB_STADIUMINFO_H
#define GAME_DB_STADIUMINFO_H

#include "types.h"

/**
 * Charged's static stadium table. R4QE01 holds 18 entries: the sixteen
 * playable stadiums, the tutorial field, and a low-poly test stadium that the
 * start-up EnableAllStadiums call leaves disabled.
 *
 * Field names describe the retained lookups and their consumers.
 */
struct StadiumInfo
{
    /* 0x00 */ int mIndex;
    /* 0x04 */ const char* mName;
    /* 0x08 */ int mTerrainType;
    /* 0x0C */ int mWeatherType;
    /* 0x10 */ bool mHasCrowdAudio;
    /* 0x11 */ bool mCrowdRiotEnabled;
    /* 0x14 */ int mAudioBank;
    /* 0x18 */ const char* mTerrain;
    /* 0x1C */ const char* mTickerStringID;
    /* 0x20 */ const char* mMoviePath;
    /* 0x24 */ const char* mMoviePathPAL;
    /* 0x28 */ int mPreviewAudioEvent;
    /* 0x2C */ bool mHasHighRangeDrawables;
    /* 0x2D */ bool mEnabled;
    /* 0x30 */ unsigned long mSoundID;
    /* 0x34 */ bool mUsesAlternateCrowdView;
};

const char* GetStadiumName(int stadium);
int GetStadiumAudioBank(int stadium);
bool IsStadiumEnabled(int stadium);
void EnableAllStadiums();
int GetStadiumTerrainType(int stadium);
int GetStadiumWeatherType(int stadium);
bool IsStadiumCrowdRiotEnabled(int stadium);
bool StadiumHasCrowdAudio(int stadium);
const char* GetStadiumTerrain(int stadium);
bool StadiumHasHighRangeDrawables(int stadium);
bool SetStadiumHasHighRangeDrawables(int stadium, bool value);
const char* GetStadiumTickerStringID(int stadium);
const char* GetStadiumMoviePath(int stadium);
int GetStadiumPreviewAudioEvent(int stadium);
unsigned long GetStadiumSoundID(int stadium);
bool StadiumUsesAlternateCrowdView(int stadium);
bool IsStadiumUnlocked(int stadium);

#endif // GAME_DB_STADIUMINFO_H
