#include "unclassified/tu_801B6188.h"

struct UnidentifiedStadiumData
{
    /* 0x00 */ int unknown_0x00;
    /* 0x04 */ const char* unknown_0x04;
    /* 0x08 */ int unknown_0x08;
    /* 0x0C */ int unknown_0x0C;
    /* 0x10 */ bool unknown_0x10;
    /* 0x11 */ bool unknown_0x11;
    /* 0x12 */ bool unknown_0x12;
    /* 0x13 */ bool unknown_0x13;
    /* 0x14 */ int unknown_0x14;
    /* 0x18 */ const char* unknown_0x18;
    /* 0x1C */ const char* unknown_0x1C;
    /* 0x20 */ const char* unknown_0x20;
    /* 0x24 */ const char* unknown_0x24;
    /* 0x28 */ int unknown_0x28;
    /* 0x2C */ bool unknown_0x2C;
    /* 0x2D */ bool unknown_0x2D;
    /* 0x2E */ bool unknown_0x2E;
    /* 0x2F */ bool unknown_0x2F;
    /* 0x30 */ unsigned long unknown_0x30;
    /* 0x34 */ bool unknown_0x34;
    /* 0x35 */ bool unknown_0x35;
    /* 0x36 */ bool unknown_0x36;
    /* 0x37 */ bool unknown_0x37;
};

static UnidentifiedStadiumData sStadiumData[18] = {
    { 0, "battledome", 1, 0, true, false, false, false, 0x2F,
        "cement", "STADIUM_TICKER_BATTLEDOME",
        "art/movies/stad_battledome.thp",
        "art/movies/stad_battledome_pal.thp", 0,
        false, false, false, false, 0x8B6C14E4,
        true, false, false, false },
    { 1, "bowserstadium", 0, 0, true, false, false, false, 0x21,
        "metal", "STADIUM_TICKER_BOWSER",
        "art/movies/stad_bowserstadium.thp",
        "art/movies/stad_bowserstadium_pal.thp", 0,
        false, false, false, false, 0x862772EC,
        true, false, false, false },
    { 2, "craterfield", 2, 0, true, false, false, false, 0x22,
        "dark_grass", "STADIUM_TICKER_CRATERFIELD",
        "art/movies/stad_craterfield.thp",
        "art/movies/stad_craterfield_pal.thp", 0,
        false, false, false, false, 0x215ED3A8,
        true, false, false, false },
    { 3, "crystalcanyon", 0, 4, true, false, false, false, 0x23,
        "desert", "STADIUM_TICKER_CRYSTALCANYON",
        "art/movies/stad_crystalcanyon.thp",
        "art/movies/stad_crystalcanyon_pal.thp", 0,
        false, false, false, false, 0xA82E624D,
        true, false, false, false },
    { 4, "dump", 3, 0, true, false, false, false, 0x24,
        "sludge", "STADIUM_TICKER_DUMP",
        "art/movies/stad_dump.thp",
        "art/movies/stad_dump_pal.thp", 0,
        false, false, false, false, 0x437A5819,
        true, false, false, false },
    { 5, "galacticstadium", 1, 0, false, false, false, false, 0x25,
        "metal", "STADIUM_TICKER_GALACTIC",
        "art/movies/stad_galactic.thp",
        "art/movies/stad_galactic_pal.thp", 0,
        false, false, false, false, 0x68B728DB,
        false, false, false, false },
    { 6, "kongacoliseum", 1, 0, true, false, false, false, 0x26,
        "wood", "STADIUM_TICKER_KONGA",
        "art/movies/stad_kongacoliseum.thp",
        "art/movies/stad_kongacoliseum_pal.thp", 0,
        false, false, false, false, 0x97040F14,
        true, false, false, false },
    { 7, "lavapit", 0, 5, true, false, false, false, 0x27,
        "lavarock", "STADIUM_TICKER_LAVAPIT",
        "art/movies/stad_lavapit.thp",
        "art/movies/stad_lavapit_pal.thp", 0,
        false, false, false, false, 0xB34FCC14,
        true, false, false, false },
    { 8, "pipelinecentral", 1, 0, true, false, false, false, 0x28,
        "cement", "STADIUM_TICKER_PIPELINE",
        "art/movies/stad_pipelinecentral.thp",
        "art/movies/stad_pipelinecentral_pal.thp", 0,
        false, false, false, false, 0x4F188DC2,
        true, false, false, false },
    { 9, "stormship", 1, 6, true, false, false, false, 0x2A,
        "metal", "STADIUM_TICKER_STORMSHIP",
        "art/movies/stad_stormship.thp",
        "art/movies/stad_stormship_pal.thp", 0,
        false, false, false, false, 0x8D8D9F6C,
        true, false, false, false },
    { 10, "thepalace", 2, 0, true, false, false, false, 0x2B,
        "grass", "STADIUM_TICKER_PALACE",
        "art/movies/stad_thepalace.thp",
        "art/movies/stad_thepalace_pal.thp", 0,
        false, false, false, false, 0x9D12C1CA,
        true, false, false, false },
    { 11, "thunderisland", 5, 2, false, false, false, false, 0x2C,
        "rock", "STADIUM_TICKER_THUNDER_ISLAND",
        "art/movies/stad_thunderisland.thp",
        "art/movies/stad_thunderisland_pal.thp", 0,
        false, false, false, false, 0x20B59138,
        true, false, false, false },
    { 12, "underground", 1, 0, true, false, false, false, 0x2E,
        "astro", "STADIUM_TICKER_UNDERGROUND",
        "art/movies/stad_underground.thp",
        "art/movies/stad_underground_pal.thp", 0,
        true, false, false, false, 0xC97F4330,
        true, false, false, false },
    { 13, "vice", 2, 0, true, false, false, false, 0x2F,
        "grass", "STADIUM_TICKER_VICE",
        "art/movies/stad_vice.thp",
        "art/movies/stad_vice_pal.thp", 0,
        false, false, false, false, 0x4384028A,
        true, false, false, false },
    { 14, "wastelands", 4, 1, true, false, false, false, 0x30,
        "ice", "STADIUM_TICKER_WASTELANDS",
        "art/movies/stad_wastelands.thp",
        "art/movies/stad_wastelands_pal.thp", 0,
        false, false, false, false, 0xE4429879,
        true, false, false, false },
    { 15, "sandtomb", 0, 7, false, false, false, false, 0x29,
        "sand", "STADIUM_TICKER_SANDTOMB",
        "art/movies/stad_sandtomb.thp",
        "art/movies/stad_sandtomb_pal.thp", 0,
        false, false, false, false, 0x80A56FBB,
        false, false, false, false },
    { 16, "tutorialfield", 0, 0, false, false, false, false, 0x2D,
        "unobtanium", "STADIUM_TICKER_TUTORIAL",
        "art/movies/stad_classroom.thp",
        "art/movies/stad_classroom_pal.thp", 0,
        false, false, false, false, 0x7F4A5D57,
        false, false, false, false },
    { 17, "lowpolystadium", 1, 0, true, false, false, false, 0x2F,
        "grass", "STADIUM_TICKER_LOWPOLYSTADIUM",
        "art/movies/stad_wastelands.thp",
        "art/movies/stad_wastelands_pal.thp", 0,
        false, false, false, false, 0,
        true, false, false, false },
};

extern "C" bool fn_801100E0();
extern "C" bool fn_80110188();
extern "C" bool fn_80110230();
extern "C" bool fn_80110338();
extern "C" bool fn_801103D4();
extern "C" bool fn_80110470();
extern "C" int fn_803693B4();

extern "C" const char* fn_801B6188(int stadium)
{
    return sStadiumData[stadium].unknown_0x04;
}

extern "C" int fn_801B61A0(int stadium)
{
    return sStadiumData[stadium].unknown_0x14;
}

extern "C" bool fn_801B61B8(int stadium)
{
    return sStadiumData[stadium].unknown_0x2D;
}

extern "C" void fn_801B61D0()
{
    for (int i = 0; i < 17; ++i)
    {
        sStadiumData[i].unknown_0x2D = true;
    }
}

extern "C" int fn_801B6230(int stadium)
{
    return sStadiumData[stadium].unknown_0x08;
}

extern "C" int fn_801B6248(int stadium)
{
    return sStadiumData[stadium].unknown_0x0C;
}

extern "C" bool fn_801B6260(int stadium)
{
    return sStadiumData[stadium].unknown_0x11;
}

extern "C" bool fn_801B6278(int stadium)
{
    return sStadiumData[stadium].unknown_0x10;
}

extern "C" const char* fn_801B6290(int stadium)
{
    return sStadiumData[stadium].unknown_0x18;
}

extern "C" bool fn_801B62A8(int stadium)
{
    return sStadiumData[stadium].unknown_0x2C;
}

extern "C" bool fn_801B62C0(int stadium, bool value)
{
    bool previous = sStadiumData[stadium].unknown_0x2C;
    sStadiumData[stadium].unknown_0x2C = value;
    return previous;
}

extern "C" const char* fn_801B62DC(int stadium)
{
    return sStadiumData[stadium].unknown_0x1C;
}

extern "C" const char* fn_801B62F4(int stadium)
{
    if (fn_801B63C8(stadium))
    {
        if (fn_803693B4() == 1)
        {
            return sStadiumData[stadium].unknown_0x24;
        }
        return sStadiumData[stadium].unknown_0x20;
    }

    if (fn_803693B4() == 1)
    {
        return "art/movies/stad_locked_pal.thp";
    }
    return "art/movies/stad_locked.thp";
}

extern "C" int fn_801B6380(int stadium)
{
    return sStadiumData[stadium].unknown_0x28;
}

extern "C" unsigned long fn_801B6398(int stadium)
{
    return sStadiumData[stadium].unknown_0x30;
}

extern "C" bool fn_801B63B0(int stadium)
{
    return sStadiumData[stadium].unknown_0x34;
}

extern "C" bool fn_801B63C8(int stadium)
{
    switch (stadium)
    {
    case 14:
        return fn_801100E0();
    case 4:
        return fn_80110188();
    case 5:
        return fn_80110230();
    case 7:
        return fn_80110470();
    case 3:
        return fn_801103D4();
    case 9:
        return fn_80110338();
    default:
        return true;
    }
}
