#include "Game/TrophyInfo.h"

const char* gCupPersonaStringIDs[10] = {
    "CUP_PERSONA_MUSHROOM",
    "CUP_PERSONA_FLOWER",
    "CUP_PERSONA_STAR",
    "CUP_PERSONA_SUNSHINE",
    "CUP_PERSONA_BANANA",
    "CUP_PERSONA_NEXT_LEVEL",
    "CUP_PERSONA_KONGA",
    "CUP_PERSONA_SAND",
    "CUP_PERSONA_LAVA",
    "CUP_PERSONA_NINTENDO",
};

static const char* sCupTrophyNames[4] = {
    "FireCup",
    "CrystalCup",
    "StrikerCup",
    0,
};

int gCupPersonaStadiums[10][2] = {
    { 10, 11 },
    { 13, 0 },
    { 5, 1 },
    { 3, 9 },
    { 8, 6 },
    { 12, 14 },
    { 6, 6 },
    { 15, 4 },
    { 7, 2 },
    { -1, -1 },
};

static const char* sCupPersonaTrophyNames[10] = {
    "MushroomCup",
    "FlowerCup",
    "StarCup",
    "SunshineCup",
    "BananaCup",
    "NextLevelCup",
    "KongaCup",
    "SandCup",
    "LavaCup",
    "NintendoCup",
};

/**
 * Offset/Address/Size: 0x0 | 0x8010A3F4 | size: 0xC
 */
const char** GetCupTrophyNames()
{
    return sCupTrophyNames;
}

/**
 * Offset/Address/Size: 0xC | 0x8010A400 | size: 0xC
 */
const char** GetCupPersonaTrophyNames()
{
    return sCupPersonaTrophyNames;
}
