#pragma pool_data off

#include "Game/CharacterTweaks.h"
#include "Game/Terrain.h"

#include "Game/AI/AiUtil.h"
#include "Game/Game.h"
#include "Game/GameTweaks.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/TweakConfig.h"
#include "Game/TweakFileLoader.h"

extern "C" void fn_8002B934(PlayerTweaks*, const char*, const char*, bool);

float g_pTweaks[2] = {
    10.0f,
    0.0f,
};

FielderTweaks::FielderTweaks(const char* name, const char* category)
    : TweaksBase(name)
    , mUnidentified4E0(category)
{
    Init();
    gTweakFileLoader.LoadFileAsync(mszFileName, mUnidentified4E0);
}

FielderTweaks::~FielderTweaks()
{
}

void FielderTweaks::Init()
{
    mUnidentified474 = 4000.0f;
    mUnidentified478 = 12.5f;
    mUnidentified47C = 9.5f;
    mUnidentified480 = 4000.0f;
    mUnidentified484 = 15.0f;
    mUnidentified488 = 15.0f;
    mUnidentified48C = 30000.0f;
    mUnidentified490 = 40000.0f;
    mUnidentified494 = 0.85f;
    mUnidentified498 = 120000.0f;
    mUnidentified49C = 0.9f;
    mUnidentified4A0 = 2500.0f;
    mUnidentified4A4 = 22.5f;
    mUnidentified4A8 = 18.0f;
    mUnidentified4AC = 3200.0f;
    mUnidentified4B0 = 18.0f;
    mUnidentified4B4 = 10.0f;
    mUnidentified4B8 = 12.0f;
    mUnidentified4BC = 12.0f;
    mUnidentified4C0 = 75000.0f;
    mUnidentified4C4 = 4000.0f;
    mUnidentified4C8 = 6.0f;
    mUnidentified4CC = 26.0f;
    mUnidentified4D0 = 106.0f;
    mUnidentified4D4 = 27.0f;
    mUnidentified4D8 = 3.0f;
    mUnidentified4DC = g_pTweaks[0];

    mUnidentified044.BindWithDefault("Run Speed Min", 6.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified054.BindWithDefault("Run Speed Max", 6.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified064.BindWithDefault("Jog Speed Min", 4.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified074.BindWithDefault("Jog Speed Max", 4.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified084.BindWithDefault("Run Accel Min", 6.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified094.BindWithDefault("Run Accel Max", 6.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0A4.BindWithDefault("Run Turn Speed Min", 100000.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0B4.BindWithDefault("Run Turn Speed Max", 100000.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0C4.BindWithDefault("Turbo Turn Speed Min", 65000.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0D4.BindWithDefault("Turbo Turn Speed Max", 65000.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0E4.BindWithDefault("Turbo Speed Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0F4.BindWithDefault("Turbo Speed Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified104.BindWithDefault("Run WB Turn Speed Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified114.BindWithDefault("Run WB Turn Speed Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified124.BindWithDefault("Run WB Speed Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified134.BindWithDefault("Run WB Speed Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified144.BindWithDefault("Run WB Accel Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified154.BindWithDefault("Run WB Accel Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified164.BindWithDefault("Turbo WB Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified174.BindWithDefault("Turbo WB Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified184.BindWithDefault("Fastest Ground Pass Speed Min", 20.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified194.BindWithDefault("Fastest Ground Pass Speed Max", 20.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1A4.BindWithDefault("Fastest Volley Pass Speed Min", 11.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1B4.BindWithDefault("Fastest Volley Pass Speed Max", 11.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1C4.BindWithDefault("Slowest Shot Speed Min", 22.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1D4.BindWithDefault("Slowest Shot Speed Max", 22.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1E4.BindWithDefault("One Timer Max Speed Min", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1F4.BindWithDefault("One Timer Max Speed Max", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified204.BindWithDefault("Clear Ball Min Z Speed", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified214.BindWithDefault("Clear Min Speed", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified224.BindWithDefault("Fastest Shot Speed Min", 28.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified234.BindWithDefault("Fastest Shot Speed Max", 32.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified244.BindWithDefault("Fastest Chip Shot Speed Min", 8.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified254.BindWithDefault("Fastest Chip Shot Speed Max", 14.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified264.BindWithDefault("Fastest Clear Speed Min", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified274.BindWithDefault("Fastest Clear Speed Max", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified284.BindWithDefault("Shot Net Open Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified294.BindWithDefault("Shot Player Distance Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2A4.BindWithDefault("Chip Shot Goalie Out Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2B4.BindWithDefault("Chip Shot Net Open Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2C4.BindWithDefault("Shot Net Open Angle", 45.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2D4.BindWithDefault("Shot Ratings Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2E4.BindWithDefault("STS Yellow Distance", 0.05f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2F4.BindWithDefault("Slide Time Min", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified304.BindWithDefault("Slide Time Max", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified314.BindWithDefault("Slide Speed Min", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified324.BindWithDefault("Slide Speed Max", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified334.BindWithDefault("Slide Decel Time Min", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified344.BindWithDefault("Slide Decel Time Max", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified354.BindWithDefault("Slide Decel", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified364.BindWithDefault("Super Slide Speed Bonus", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified374.BindWithDefault("Hit Effective Max Frame", 7.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified384.BindWithDefault("Hit Effective First Frame", 4.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified394.BindWithDefault("Hit Effective Last Frame Min", 14.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3A4.BindWithDefault("Hit Effective Last Frame Max", 14.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3B4.BindWithDefault("Mushroom Effect Time", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3C4.BindWithDefault("Mushroom Speed Boost", 15.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3D4.BindWithDefault("Star Effect Time", 2.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3E4.BindWithDefault("Star Speed Boost", 15.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    fGreenShellSpeed.BindWithDefault("Shell Speed", 12.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified404.BindWithDefault("Shell Time", 1.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified414.BindWithDefault("Bowser Explode Radius", 5.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified424.BindWithDefault("Terrain Min Speed Adjust", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified434.BindWithDefault("Terrain Max Speed Adjust", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified444.BindWithDefault("Terrain Min Slippery Adjust", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified454.BindWithDefault("Terrain Max Slippery Adjust", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified464.BindWithDefault("Terrain Max Slippery Momentum", 0.9f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
}

PlayerTweaks::PlayerTweaks(const char* name, const char* category)
{
    fn_8002B934(this, name, category, true);
}

PlayerTweaks::~PlayerTweaks()
{
}

void fn_8002B934(PlayerTweaks* tweaks, const char* name,
    const char* category, bool registerTweaks)
{
    tweaks->mUnidentified004.BindWithDefault("mfHeight", 0.5f, category, true, 0.0f, 0.0f, 0.0f);
    tweaks->mUnidentified014.BindWithDefault("mfWidth", 0.5f, category, true, 0.0f, 0.0f, 0.0f);
    tweaks->mUnidentified024.BindWithDefault("mfMovement_TurningRadius", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->mUnidentified034.BindWithDefault("mfMovement_Speed", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->mUnidentified044.BindWithDefault("mfMovement_Acceleration", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->mUnidentified054.BindWithDefault("mfDefense_SlideTackle", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->mUnidentified064.BindWithDefault("mfDefense_Size", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->mUnidentified074.BindWithDefault("mfDefense_HittingDistance", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->mUnidentified084.BindWithDefault("mfOffense_ShootingWindupTime", 0.5f, category, true, 0.0f, 4.0f, 0.05f);
    tweaks->mUnidentified094.BindWithDefault("mfOffense_ShootingWindupTotalTime", 0.5f, category, true, 0.0f, 4.0f, 0.05f);
    tweaks->fShooting.BindWithDefault("mfOffense_Shooting", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->fPassing.BindWithDefault("mfOffense_Passing", 0.5f, category, true, -4.0f, 4.0f, 0.05f);

    if (registerTweaks)
    {
        gTweakFileLoader.LoadFileAsync(name, category);
    }
    else
    {
        LoadTweakConfigFile(name, category, true);
    }
}

extern "C" float fn_8002BE18(PlayerTweaks* tweaks)
{
    return (tweaks->fPassing.UnidentifiedGetValue()
               + tweaks->fShooting.UnidentifiedGetValue())
         / 2.0f;
}

extern "C" float fn_8002BE38(PlayerTweaks* tweaks)
{
    float result = tweaks->mUnidentified054.UnidentifiedGetValue()
                 + tweaks->mUnidentified034.UnidentifiedGetValue();
    return (result + tweaks->mUnidentified074.UnidentifiedGetValue()) / 3.0f;
}

extern "C" float fn_8002BE64(PlayerTweaks* tweaks)
{
    return (tweaks->fPassing.UnidentifiedGetValue()
               + tweaks->mUnidentified034.UnidentifiedGetValue())
         / 2.0f;
}

extern "C" float fn_8002BE84(PlayerTweaks* tweaks)
{
    float result = tweaks->mUnidentified074.UnidentifiedGetValue()
                 + tweaks->mUnidentified054.UnidentifiedGetValue();
    return (result + tweaks->fShooting.UnidentifiedGetValue()) / 3.0f;
}

float PlayerTweaks::GetSkillRating(unsigned int index)
{
    float result = -9999.9f;
    switch (index)
    {
    case 1:
        result = mUnidentified034;
        break;
    case 2:
        result = mUnidentified054;
        break;
    case 3:
        result = mUnidentified074;
        break;
    case 4:
        result = fShooting;
        break;
    case 5:
        result = fPassing;
        break;
    case 6:
        result = mUnidentified054.UnidentifiedGetValue()
               + mUnidentified034.UnidentifiedGetValue();
        result = (result + mUnidentified074.UnidentifiedGetValue())
               / 3.0f;
        break;
    case 7:
        result = (fPassing.UnidentifiedGetValue()
                     + fShooting.UnidentifiedGetValue())
               / 2.0f;
        break;
    case 8:
        result = (fPassing.UnidentifiedGetValue()
                     + mUnidentified034.UnidentifiedGetValue())
               / 2.0f;
        break;
    case 9:
        result = mUnidentified074.UnidentifiedGetValue()
               + mUnidentified054.UnidentifiedGetValue();
        result = (result + fShooting.UnidentifiedGetValue()) / 3.0f;
        break;
    }
    return result;
}

extern "C" float fn_8002BFA8(PlayerTweaks* tweaks, float value)
{
    return value * tweaks->mUnidentified014;
}

extern "C" float fn_8002BFB8(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified054;
    float minimum = fielderTweaks->mUnidentified044;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C08C(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified024;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    return Interpolate(fielderTweaks->mUnidentified48C,
        fielderTweaks->mUnidentified490,
        playerValue);
}

extern "C" float fn_8002C0AC(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified024;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified0B4;
    float minimum = fielderTweaks->mUnidentified0A4;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C180(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified044;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified094;
    float minimum = fielderTweaks->mUnidentified084;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C254(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified0F4;
    float minimum = fielderTweaks->mUnidentified0E4;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C328(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified134;
    float minimum = fielderTweaks->mUnidentified124;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C3FC(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified044;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified154;
    float minimum = fielderTweaks->mUnidentified144;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C4D0(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified024;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified114;
    float minimum = fielderTweaks->mUnidentified104;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C5A4(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified174;
    float minimum = fielderTweaks->mUnidentified164;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C678(PlayerTweaks* tweaks)
{
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float scale = 0.25f * terrain + 0.75f;
    return Interpolate(
        scale * gGameTweaks.mFielderTweaks->mUnidentified184.UnidentifiedGetValue(),
        scale * (float)gGameTweaks.mFielderTweaks->mUnidentified194,
        tweaks->fPassing);
}

extern "C" float fn_8002C6E8(PlayerTweaks*)
{
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    return gGameTweaks.mFielderTweaks->mUnidentified4B8
         * (0.25f * terrain + 0.75f);
}

extern "C" float fn_8002C730(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->mUnidentified1A4,
        gGameTweaks.mFielderTweaks->mUnidentified1B4,
        tweaks->fPassing);
}

extern "C" float fn_8002C758(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->mUnidentified224,
        gGameTweaks.mFielderTweaks->mUnidentified234,
        tweaks->fShooting);
}

extern "C" float fn_8002C780(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->mUnidentified1C4,
        gGameTweaks.mFielderTweaks->mUnidentified1D4,
        tweaks->fShooting);
}

extern "C" float fn_8002C7A8(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->mUnidentified1E4,
        gGameTweaks.mFielderTweaks->mUnidentified1F4,
        tweaks->fShooting);
}

extern "C" float fn_8002C7D0()
{
    return gGameTweaks.mFielderTweaks->mUnidentified3B4;
}

extern "C" float fn_8002C7E8(PlayerTweaks* tweaks)
{
    return tweaks->mUnidentified084;
}

extern "C" float fn_8002C7F4(PlayerTweaks* tweaks)
{
    return tweaks->mUnidentified094;
}

extern "C" float fn_8002C800(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified054;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified304;
    float minimum = fielderTweaks->mUnidentified2F4;
    float terrainMaximum = fielderTweaks->mUnidentified454;
    float terrainMinimum = fielderTweaks->mUnidentified444;
    float terrain = g_pGame->mpTerrain->GetSlideFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C8D4(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified054;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified344;
    float minimum = fielderTweaks->mUnidentified334;
    float terrainMaximum = fielderTweaks->mUnidentified454;
    float terrainMinimum = fielderTweaks->mUnidentified444;
    float terrain = g_pGame->mpTerrain->GetSlideFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C9A8(PlayerTweaks* tweaks)
{
    float result = fn_8002C254(tweaks);
    result *= Interpolate(gGameTweaks.mFielderTweaks->mUnidentified314,
        gGameTweaks.mFielderTweaks->mUnidentified324,
        tweaks->mUnidentified054);
    if (fn_8002BE84(tweaks) > 0.9f)
    {
        result *= 1.175f;
    }
    return result * Interpolate(gGameTweaks.mFielderTweaks->mUnidentified424, gGameTweaks.mFielderTweaks->mUnidentified434, g_pGame->mpTerrain->GetSpeedFactor());
}

extern "C" float fn_8002CB2C(PlayerTweaks*)
{
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * (float)gGameTweaks.mFielderTweaks->mUnidentified3C4;
}

extern "C" float fn_8002CBB8(PlayerTweaks*)
{
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * (float)gGameTweaks.mFielderTweaks->mUnidentified3E4;
}

extern "C" float fn_8002CC44(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified054;
    float minimum = fielderTweaks->mUnidentified044;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return gGameTweaks.mFielderTweaks->mUnidentified494 * terrainScale
         * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002CD2C(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified054;
    float minimum = fielderTweaks->mUnidentified044;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return gGameTweaks.mFielderTweaks->mUnidentified49C * terrainScale
         * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002CE14(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->mUnidentified074;
    float minimum = fielderTweaks->mUnidentified064;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002CEE8(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified488;
}

extern "C" float fn_8002CEFC(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified498;
}

extern "C" float fn_8002CF10(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified474;
}

extern "C" float fn_8002CF24(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified478;
}

extern "C" float fn_8002CF38(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified480;
}

extern "C" float fn_8002CF4C(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified484;
}

extern "C" float fn_8002CF60(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified4AC;
}

extern "C" float fn_8002CF74(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified4B0;
}

extern "C" float fn_8002CF88(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified4C0;
}

extern "C" float fn_8002CF9C(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified4C4;
}

extern "C" float fn_8002CFB0(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified4C8;
}

extern "C" float fn_8002CFC4(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified4BC;
}

extern "C" float fn_8002CFD8(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified3D4;
}

extern "C" float fn_8002CFF0(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fGreenShellSpeed;
}

extern "C" float fn_8002D008(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified364;
}

extern "C" float fn_8002D020(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified384;
}

extern "C" float fn_8002D038(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->mUnidentified374;
}

extern "C" float fn_8002D050(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->mUnidentified394,
        gGameTweaks.mFielderTweaks->mUnidentified3A4,
        tweaks->mUnidentified074);
}

