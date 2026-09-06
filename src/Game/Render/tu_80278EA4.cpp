#include "Game/Render/tu_80278EA4.h"

#include "NL/nlPrint.h"
#include "unclassified/tu_80073898.h"

extern "C" void fn_80073A48(
    UnidentifiedTweakLoadState*, const char*, const char*);

StadiumTweakValues_80278EA4::StadiumTweakValues_80278EA4(
    const char* category, const char* szBaseName)
{
    fGoalpostRadius.fn_8002D078(
        "goalpost radius", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fGoalpostOffset.fn_8002D078(
        "goalpost offset", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fNetWidth.fn_8002D078(
        "net width", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fNetHeight.fn_8002D078(
        "net height", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fPhysNetWidth.fn_8002D078(
        "physics net width", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fPhysNetHeight.fn_8002D078(
        "physics net height", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fPhysNetDepth.fn_8002D078(
        "physics net depth", 0.0f, category, false, 0.0f, 0.0f, 0.0f);

    fSoftness.fn_8002D078(
        "physics net softness", -1.0f, category, false, 0.0f, 0.0f, 0.0f);

    bool registered = bDontUseLowest.fn_802C4FEC(
        "dont use lowest net texture LOD", 0.0f, category, false, 0.0f, 0.0f);
    if (!registered)
    {
        *bDontUseLowest.m_pValue = bDontUseLowest.UnidentifiedVirtual3C();
    }
    if (!registered)
    {
        *bDontUseLowest.m_pValue = false;
    }

    fShadowHeight.fn_8002D078(
        "Shadow Height", 0.1f, category, false, 0.0f, 0.0f, 0.0f);

    fShadowOpacity.fn_8002D078(
        "Shadow Opacity", 0.3f, category, false, 0.0f, 0.0f, 0.0f);

    char szFileName[128];
    nlSNPrintf(szFileName, sizeof(szFileName), "ini/stadiums/%s.ini", szBaseName);
    fn_80073A48(&lbl_8056BA00, "ini/stadiums/default.ini", "");
    fn_80073A48(&lbl_8056BA00, szFileName, "");
}
