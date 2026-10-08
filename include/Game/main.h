#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "NL/nlLocalization.h"

extern nlLocalization::nlLanguage g_Language;
extern int g_BuildNumber;
extern bool g_e3_Build;
extern bool lbl_806E1091;

enum eGameRegion
{
    GAME_REGION_US = 0,
    GAME_REGION_EU = 1,
    GAME_REGION_JAPAN = 2,
    GAME_REGION_DEFAULT = 3,
};

int GetRegion();
int GetOnlineRegion();
bool IsAlternateOnlineCountryGroup();

#endif // GAME_MAIN_H
