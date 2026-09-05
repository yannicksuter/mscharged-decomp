#ifndef GAME_AI_FIELDER_ABILITY_H
#define GAME_AI_FIELDER_ABILITY_H

class WaluigiWallManager_80178400;

struct UnidentifiedFielderAbility3E8
{
    UnidentifiedFielderAbility3E8()
        : nextFireballTime(0.0f)
        , fireballStageTime(0.0f)
        , fireballStageNum(0)
    {
    }

    void fn_800504A4();

    /* 0x00 */ float nextFireballTime;
    /* 0x04 */ float fireballStageTime;
    /* 0x08 */ int fireballStageNum;
}; // size: 0xC

struct UnidentifiedAbilityEffect
{
    UnidentifiedAbilityEffect()
        : mUnidentified00(0.0f)
        , mUnidentified04(0.0f)
    {
    }

    void fn_800504A8();

    /* 0x00 */ float mUnidentified00;
    /* 0x04 */ float mUnidentified04;
    /* 0x08 */ WaluigiWallManager_80178400* mUnidentified08;
}; // size: 0xC

#endif // GAME_AI_FIELDER_ABILITY_H
