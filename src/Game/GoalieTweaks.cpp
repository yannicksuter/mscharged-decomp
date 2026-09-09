#include "Game/CharacterTweaks.h"

#include "Game/TweakFileLoader.h"

GoalieTweaks::GoalieTweaks(const char* name, const char* category)
    : TweaksBase(name)
    , mUnidentified358(category)
{
    Init();
    gTweakFileLoader.LoadFileAsync(mszFileName, mUnidentified358);
}

GoalieTweaks::~GoalieTweaks()
{
}

inline float max_float(float a, float b)
{
    if (a >= b)
    {
        return a;
    }
    return b;
}

void GoalieTweaks::fn_800277A0()
{
    // Find the maximum fatigue value
    fShotFatigueMax = max_float(fShotFatigueDefault, fShotFatigueStandCatch);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueDiveCatch);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueStandDeflect);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueDiveDeflect);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueStandPunch);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueLegSave);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueSTSSave);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueSTSStun);
}
