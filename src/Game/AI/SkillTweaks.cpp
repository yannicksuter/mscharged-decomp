#include <stddef.h>

#include "Game/GameTweaks.h"

#include "Game/Player.h"
#include "Game/Team.h"

/**
 * Offset/Address/Size: 0x2ABC | 0x800A327C | size: 0xD4
 */
float SkillTweaks::fn_800A327C(unsigned long param1, cPlayer* param2)
{
    float result = -9999.9f;
    cPlayer* pPreviousPlayer = mUnidentified240;
    if (param2 != NULL)
    {
        mUnidentified240 = param2;
    }

    SkillTweak* pSkillTweak;
    SkillTweak** ppSkillTweak;
    bool found = mSkillTweaksList.FindGet(param1, &ppSkillTweak);
    if (found)
    {
        pSkillTweak = *ppSkillTweak;
    }
    if (found)
    {
        result = pSkillTweak->fn_800A0D6C();
    }

    if (param2 != NULL)
    {
        mUnidentified240 = pPreviousPlayer;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x2B90 | 0x800A3350 | size: 0xB4
 */
bool SkillTweaks::fn_800A3350(
    unsigned long param1, float* param2, cPlayer* param3)
{
    cPlayer* pPreviousPlayer = mUnidentified240;
    if (param3 != NULL)
    {
        mUnidentified240 = param3;
    }

    bool found = false;
    SkillTweak* pSkillTweak;
    SkillTweak** ppSkillTweak;
    bool tweakFound = mSkillTweaksList.FindGet(param1, &ppSkillTweak);
    if (tweakFound)
    {
        pSkillTweak = *ppSkillTweak;
    }
    if (tweakFound)
    {
        *param2 = pSkillTweak->fn_800A0D6C();
        found = true;
    }

    if (param3 != NULL)
    {
        mUnidentified240 = pPreviousPlayer;
    }
    return found;
}

/**
 * Offset/Address/Size: 0x2C44 | 0x800A3404 | size: 0x70
 */
float* SkillTweaks::fn_800A3404()
{
    if (mUnidentified20C[0] == -1.0f)
    {
        for (int i = 0; i < 4; ++i)
        {
            mUnidentified20C[i] = mUnidentified000[i]->fn_800A0D6C();
        }
    }
    return mUnidentified20C;
}

/**
 * Offset/Address/Size: 0x2CB4 | 0x800A3474 | size: 0x60
 */
float SkillTweaks::fn_800A3474(cPlayer* param1)
{
    if (param1 == NULL)
    {
        param1 = mUnidentified240;
    }

    float result = 0.0f;
    if (param1 == NULL)
    {
        return result;
    }

    switch (param1->m_pTeam->field_0x18)
    {
    case 1:
        return mUnidentified020->fn_800A0D6C();
    case 0:
        return Off_Reaction->fn_800A0D6C();
    case 2:
        return mUnidentified024->fn_800A0D6C();
    }
    return result;
}
