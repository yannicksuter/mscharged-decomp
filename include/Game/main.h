#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "NL/nlLocalization.h"

extern nlLocalization::nlLanguage g_Language;
extern int g_BuildNumber;
extern bool g_e3_Build;
extern bool gUseAlternateTitleScreen;

enum eGameRegion
{
    GAME_REGION_US = 0,
    GAME_REGION_EU = 1,
    GAME_REGION_JAPAN = 2,
#if defined(VERSION_R4QK01)
    GAME_REGION_KOREA = 3,
    GAME_REGION_DEFAULT = 4,
#else
    GAME_REGION_DEFAULT = 3,
#endif
};

int GetRegion();
int GetOnlineRegion();
bool IsAlternateOnlineCountryGroup();

#endif // GAME_MAIN_H
