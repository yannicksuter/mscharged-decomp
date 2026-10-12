#include "Game/DB/StadiumInfo.h"

#include "Game/Audio/AudioBankID.h"
#include "Game/DB/GameProgress.h"
#include "NL/gl/glPlat.h"

enum
{
    NUM_STADIUMS = 18,
};

static StadiumInfo sStadiumInfo[NUM_STADIUMS] = {
    { STAD_BATTLEDOME, "battledome", 1, 0, true, false, false, false, AUDIO_BANK_STAD_VICE,
        "cement", "STADIUM_TICKER_BATTLEDOME",
        "art/movies/stad_battledome.thp",
        "art/movies/stad_battledome_pal.thp", 0,
        false, false, false, false, 0x8B6C14E4,
        true, false, false, false },
    { STAD_BOWSER_STADIUM, "bowserstadium", 0, 0, true, false, false, false, AUDIO_BANK_STAD_BOWSER,
        "metal", "STADIUM_TICKER_BOWSER",
        "art/movies/stad_bowserstadium.thp",
        "art/movies/stad_bowserstadium_pal.thp", 0,
        false, false, false, false, 0x862772EC,
        true, false, false, false },
    { STAD_CRATERFIELD, "craterfield", 2, 0, true, false, false, false, AUDIO_BANK_STAD_CRATERFIELD,
        "dark_grass", "STADIUM_TICKER_CRATERFIELD",
        "art/movies/stad_craterfield.thp",
        "art/movies/stad_craterfield_pal.thp", 0,
        false, false, false, false, 0x215ED3A8,
        true, false, false, false },
    { STAD_CRYSTAL_CANYON, "crystalcanyon", 0, 4, true, false, false, false, AUDIO_BANK_STAD_CRYSTALCANYON,
        "desert", "STADIUM_TICKER_CRYSTALCANYON",
        "art/movies/stad_crystalcanyon.thp",
        "art/movies/stad_crystalcanyon_pal.thp", 0,
        false, false, false, false, 0xA82E624D,
        true, false, false, false },
    { STAD_DUMP, "dump", 3, 0, true, false, false, false, AUDIO_BANK_STAD_DUMP,
        "sludge", "STADIUM_TICKER_DUMP",
        "art/movies/stad_dump.thp",
        "art/movies/stad_dump_pal.thp", 0,
        false, false, false, false, 0x437A5819,
        true, false, false, false },
    { STAD_GALACTIC_STADIUM, "galacticstadium", 1, 0, false, false, false, false, AUDIO_BANK_STAD_GALACTIC,
        "metal", "STADIUM_TICKER_GALACTIC",
        "art/movies/stad_galactic.thp",
        "art/movies/stad_galactic_pal.thp", 0,
        false, false, false, false, 0x68B728DB,
#if defined(VERSION_R4QP01_REV1)
        true, false, false, false },
#else
        false, false, false, false },
#endif
    { STAD_KONGA_COLISEUM, "kongacoliseum", 1, 0, true, false, false, false, AUDIO_BANK_STAD_KONGA,
        "wood", "STADIUM_TICKER_KONGA",
        "art/movies/stad_kongacoliseum.thp",
        "art/movies/stad_kongacoliseum_pal.thp", 0,
        false, false, false, false, 0x97040F14,
        true, false, false, false },
    { STAD_LAVA_PIT, "lavapit", 0, 5, true, false, false, false, AUDIO_BANK_STAD_LAVAPIT,
        "lavarock", "STADIUM_TICKER_LAVAPIT",
        "art/movies/stad_lavapit.thp",
        "art/movies/stad_lavapit_pal.thp", 0,
        false, false, false, false, 0xB34FCC14,
        true, false, false, false },
    { STAD_PIPELINE_CENTRAL, "pipelinecentral", 1, 0, true, false, false, false, AUDIO_BANK_STAD_PIPELINE,
        "cement", "STADIUM_TICKER_PIPELINE",
        "art/movies/stad_pipelinecentral.thp",
        "art/movies/stad_pipelinecentral_pal.thp", 0,
        false, false, false, false, 0x4F188DC2,
        true, false, false, false },
    { STAD_STORM_SHIP, "stormship", 1, 6, true, false, false, false, AUDIO_BANK_STAD_STORMSHIP,
        "metal", "STADIUM_TICKER_STORMSHIP",
        "art/movies/stad_stormship.thp",
        "art/movies/stad_stormship_pal.thp", 0,
        false, false, false, false, 0x8D8D9F6C,
        true, false, false, false },
    { STAD_THE_PALACE, "thepalace", 2, 0, true, false, false, false, AUDIO_BANK_STAD_PALACE,
        "grass", "STADIUM_TICKER_PALACE",
        "art/movies/stad_thepalace.thp",
        "art/movies/stad_thepalace_pal.thp", 0,
        false, false, false, false, 0x9D12C1CA,
        true, false, false, false },
    { STAD_THUNDER_ISLAND, "thunderisland", 5, 2, false, false, false, false, AUDIO_BANK_STAD_THUNDERISLAND,
        "rock", "STADIUM_TICKER_THUNDER_ISLAND",
        "art/movies/stad_thunderisland.thp",
        "art/movies/stad_thunderisland_pal.thp", 0,
        false, false, false, false, 0x20B59138,
        true, false, false, false },
    { STAD_UNDERGROUND, "underground", 1, 0, true, false, false, false, AUDIO_BANK_STAD_UNDERGROUND,
        "astro", "STADIUM_TICKER_UNDERGROUND",
        "art/movies/stad_underground.thp",
        "art/movies/stad_underground_pal.thp", 0,
        true, false, false, false, 0xC97F4330,
        true, false, false, false },
    { STAD_VICE, "vice", 2, 0, true, false, false, false, AUDIO_BANK_STAD_VICE,
        "grass", "STADIUM_TICKER_VICE",
        "art/movies/stad_vice.thp",
        "art/movies/stad_vice_pal.thp", 0,
        false, false, false, false, 0x4384028A,
        true, false, false, false },
    { STAD_WASTELANDS, "wastelands", 4, 1, true, false, false, false, AUDIO_BANK_STAD_WASTELANDS,
        "ice", "STADIUM_TICKER_WASTELANDS",
        "art/movies/stad_wastelands.thp",
        "art/movies/stad_wastelands_pal.thp", 0,
        false, false, false, false, 0xE4429879,
        true, false, false, false },
    { STAD_SAND_TOMB, "sandtomb", 0, 7, false, false, false, false, AUDIO_BANK_STAD_SANDTOMB,
        "sand", "STADIUM_TICKER_SANDTOMB",
        "art/movies/stad_sandtomb.thp",
        "art/movies/stad_sandtomb_pal.thp", 0,
        false, false, false, false, 0x80A56FBB,
        false, false, false, false },
    { STAD_TUTORIAL_FIELD, "tutorialfield", 0, 0, false, false, false, false, AUDIO_BANK_STAD_TUTORIAL,
        "unobtanium", "STADIUM_TICKER_TUTORIAL",
        "art/movies/stad_classroom.thp",
        "art/movies/stad_classroom_pal.thp", 0,
        false, false, false, false, 0x7F4A5D57,
#if defined(VERSION_R4QP01_REV1)
        true, false, false, false },
#else
        false, false, false, false },
#endif
    { STAD_LOW_POLY_STADIUM, "lowpolystadium", 1, 0, true, false, false, false, AUDIO_BANK_STAD_VICE,
        "grass", "STADIUM_TICKER_LOWPOLYSTADIUM",
        "art/movies/stad_wastelands.thp",
        "art/movies/stad_wastelands_pal.thp", 0,
        false, false, false, false, 0,
        true, false, false, false },
};

const char* GetStadiumName(int stadium)
{
    return sStadiumInfo[stadium].mName;
}

int GetStadiumUnknown0x14(int stadium)
{
    return sStadiumInfo[stadium].unknown_0x14;
}

bool IsStadiumEnabled(int stadium)
{
    return sStadiumInfo[stadium].mEnabled;
}

void EnableAllStadiums()
{
    for (int i = 0; i < 17; ++i)
    {
        sStadiumInfo[i].mEnabled = true;
    }
}

int GetStadiumUnknown0x08(int stadium)
{
    return sStadiumInfo[stadium].unknown_0x08;
}

int GetStadiumUnknown0x0C(int stadium)
{
    return sStadiumInfo[stadium].unknown_0x0C;
}

bool GetStadiumUnknown0x11(int stadium)
{
    return sStadiumInfo[stadium].unknown_0x11;
}

bool GetStadiumUnknown0x10(int stadium)
{
    return sStadiumInfo[stadium].unknown_0x10;
}

const char* GetStadiumTerrain(int stadium)
{
    return sStadiumInfo[stadium].mTerrain;
}

bool StadiumHasHighRangeDrawables(int stadium)
{
    return sStadiumInfo[stadium].mHasHighRangeDrawables;
}

bool SetStadiumHasHighRangeDrawables(int stadium, bool value)
{
    bool previous = sStadiumInfo[stadium].mHasHighRangeDrawables;
    sStadiumInfo[stadium].mHasHighRangeDrawables = value;
    return previous;
}

const char* GetStadiumTickerStringID(int stadium)
{
    return sStadiumInfo[stadium].mTickerStringID;
}

const char* GetStadiumMoviePath(int stadium)
{
    if (IsStadiumUnlocked(stadium))
    {
        if (glx_GetVideoMode() == 1)
        {
            return sStadiumInfo[stadium].mMoviePathPAL;
        }
        return sStadiumInfo[stadium].mMoviePath;
    }

    if (glx_GetVideoMode() == 1)
    {
        return "art/movies/stad_locked_pal.thp";
    }
    return "art/movies/stad_locked.thp";
}

int GetStadiumUnknown0x28(int stadium)
{
    return sStadiumInfo[stadium].unknown_0x28;
}

unsigned long GetStadiumSoundID(int stadium)
{
    return sStadiumInfo[stadium].mSoundID;
}

bool GetStadiumUnknown0x34(int stadium)
{
    return sStadiumInfo[stadium].unknown_0x34;
}

bool IsStadiumUnlocked(int stadium)
{
    switch (stadium)
    {
    case STAD_WASTELANDS:
        return IsWastelandsUnlocked();
    case STAD_DUMP:
        return IsDumpUnlocked();
    case STAD_GALACTIC_STADIUM:
        return IsGalacticStadiumUnlocked();
    case STAD_LAVA_PIT:
        return IsLavaPitUnlocked();
    case STAD_CRYSTAL_CANYON:
        return IsCrystalCanyonUnlocked();
    case STAD_STORM_SHIP:
        return IsStormshipUnlocked();
    default:
        return true;
    }
}
