#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "NL/nlLocalization.h"

extern nlLocalization::nlLanguage g_Language;
extern int g_BuildNumber;
extern bool g_e3_Build;
extern bool lbl_806E1091;

int GetRegion();
int GetOnlineRegion();
bool IsAlternateOnlineCountryGroup();

#endif // GAME_MAIN_H
