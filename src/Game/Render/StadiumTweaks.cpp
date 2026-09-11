#include "Game/Render/StadiumTweaks.h"

#include "NL/nlPrint.h"
#include "Game/TweakFileLoader.h"
#include "Game/UnidentifiedStaticStorage.h"

StadiumTweaks::StadiumTweaks(
    const char* category, const char* szBaseName)
{
    fGoalpostRadius.BindWithDefault(
        "goalpost radius", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fGoalpostOffset.BindWithDefault(
        "goalpost offset", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fNetWidth.BindWithDefault(
        "net width", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fNetHeight.BindWithDefault(
        "net height", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fPhysNetWidth.BindWithDefault(
        "physics net width", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fPhysNetHeight.BindWithDefault(
        "physics net height", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fPhysNetDepth.BindWithDefault(
        "physics net depth", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fSoftness.BindWithDefault(
        "physics net softness", -1.0f, category, false, 0.0f, 0.0f, 0.0f);

    bool registered = bDontUseLowest.Bind(
        "dont use lowest net texture LOD", 0.0f, category, false, 0.0f, 0.0f);
    if (!registered)
    {
        *bDontUseLowest.m_pValue = bDontUseLowest.UnidentifiedVirtual3C();
    }
    if (!registered)
    {
        *bDontUseLowest.m_pValue = false;
    }

    fShadowHeight.BindWithDefault(
        "Shadow Height", 0.1f, category, false, 0.0f, 0.0f, 0.0f);

    fShadowOpacity.BindWithDefault(
        "Shadow Opacity", 0.3f, category, false, 0.0f, 0.0f, 0.0f);

    char szFileName[128];
    nlSNPrintf(szFileName, sizeof(szFileName), "ini/stadiums/%s.ini", szBaseName);
    gTweakFileLoader.LoadFileAsync("ini/stadiums/default.ini", "");
    gTweakFileLoader.LoadFileAsync(szFileName, "");
}
