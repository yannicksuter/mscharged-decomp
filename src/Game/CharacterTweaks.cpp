#pragma pool_data off

#include "Game/CharacterTweaks.h"

#include "Game/AI/AiUtil.h"
#include "Game/Game.h"
#include "Game/GameTweaks.h"
#include "Game/UnidentifiedStaticStorage.h"

extern "C" void fn_8002B934(PlayerTweaks*, const char*, const char*, bool);
extern "C" void fn_80073A48(void*, const char*, const char*);
extern "C" void fn_802C6CAC(const char*, const char*, bool);
extern "C" float fn_800A9274(void*);
extern "C" float fn_800A928C(void*);
extern "C" u8 lbl_8056BA00[];

float g_pTweaks[2] = {
    10.0f,
    0.0f,
};

inline TweakValueImpl_804F4DC8::TweakValueImpl_804F4DC8(float* value)
    : m_pValue(value)
{
}

FielderTweaks::FielderTweaks(const char* name, const char* category)
    : TweaksBase(name)
    , mUnidentified4E0(category)
{
    Init();
    fn_80073A48(lbl_8056BA00, mszFileName, mUnidentified4E0);
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

    mUnidentified044.fn_8002D078("Run Speed Min", 6.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified054.fn_8002D078("Run Speed Max", 6.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified064.fn_8002D078("Jog Speed Min", 4.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified074.fn_8002D078("Jog Speed Max", 4.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified084.fn_8002D078("Run Accel Min", 6.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified094.fn_8002D078("Run Accel Max", 6.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0A4.fn_8002D078("Run Turn Speed Min", 100000.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0B4.fn_8002D078("Run Turn Speed Max", 100000.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0C4.fn_8002D078("Turbo Turn Speed Min", 65000.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0D4.fn_8002D078("Turbo Turn Speed Max", 65000.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0E4.fn_8002D078("Turbo Speed Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified0F4.fn_8002D078("Turbo Speed Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified104.fn_8002D078("Run WB Turn Speed Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified114.fn_8002D078("Run WB Turn Speed Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified124.fn_8002D078("Run WB Speed Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified134.fn_8002D078("Run WB Speed Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified144.fn_8002D078("Run WB Accel Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified154.fn_8002D078("Run WB Accel Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified164.fn_8002D078("Turbo WB Min", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified174.fn_8002D078("Turbo WB Max", 7.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified184.fn_8002D078("Fastest Ground Pass Speed Min", 20.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified194.fn_8002D078("Fastest Ground Pass Speed Max", 20.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1A4.fn_8002D078("Fastest Volley Pass Speed Min", 11.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1B4.fn_8002D078("Fastest Volley Pass Speed Max", 11.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1C4.fn_8002D078("Slowest Shot Speed Min", 22.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1D4.fn_8002D078("Slowest Shot Speed Max", 22.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1E4.fn_8002D078("One Timer Max Speed Min", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified1F4.fn_8002D078("One Timer Max Speed Max", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified204.fn_8002D078("Clear Ball Min Z Speed", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified214.fn_8002D078("Clear Min Speed", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified224.fn_8002D078("Fastest Shot Speed Min", 28.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified234.fn_8002D078("Fastest Shot Speed Max", 32.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified244.fn_8002D078("Fastest Chip Shot Speed Min", 8.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified254.fn_8002D078("Fastest Chip Shot Speed Max", 14.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified264.fn_8002D078("Fastest Clear Speed Min", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified274.fn_8002D078("Fastest Clear Speed Max", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified284.fn_8002D078("Shot Net Open Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified294.fn_8002D078("Shot Player Distance Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2A4.fn_8002D078("Chip Shot Goalie Out Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2B4.fn_8002D078("Chip Shot Net Open Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2C4.fn_8002D078("Shot Net Open Angle", 45.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2D4.fn_8002D078("Shot Ratings Weight", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2E4.fn_8002D078("STS Yellow Distance", 0.05f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2F4.fn_8002D078("Slide Time Min", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified304.fn_8002D078("Slide Time Max", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified314.fn_8002D078("Slide Speed Min", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified324.fn_8002D078("Slide Speed Max", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified334.fn_8002D078("Slide Decel Time Min", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified344.fn_8002D078("Slide Decel Time Max", 0.35f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified354.fn_8002D078("Slide Decel", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified364.fn_8002D078("Super Slide Speed Bonus", 0.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified374.fn_8002D078("Hit Effective Max Frame", 7.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified384.fn_8002D078("Hit Effective First Frame", 4.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified394.fn_8002D078("Hit Effective Last Frame Min", 14.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3A4.fn_8002D078("Hit Effective Last Frame Max", 14.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3B4.fn_8002D078("Mushroom Effect Time", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3C4.fn_8002D078("Mushroom Speed Boost", 15.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3D4.fn_8002D078("Star Effect Time", 2.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified3E4.fn_8002D078("Star Speed Boost", 15.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    fGreenShellSpeed.fn_8002D078("Shell Speed", 12.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified404.fn_8002D078("Shell Time", 1.5f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified414.fn_8002D078("Bowser Explode Radius", 5.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified424.fn_8002D078("Terrain Min Speed Adjust", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified434.fn_8002D078("Terrain Max Speed Adjust", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified444.fn_8002D078("Terrain Min Slippery Adjust", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified454.fn_8002D078("Terrain Max Slippery Adjust", 1.0f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
    mUnidentified464.fn_8002D078("Terrain Max Slippery Momentum", 0.9f, mUnidentified4E0, false, 0.0f, 0.0f, 0.0f);
}

PlayerTweaks::PlayerTweaks(const char* name, const char* category)
{
    fn_8002B934(this, name, category, true);
}

PlayerTweaks::~PlayerTweaks()
{
}

static inline void UnidentifiedInitPlayerTweak(TweakValue_804F4DC8& tweak,
    const char* tweakName, float defaultValue, const char* category,
    float value, float min, float max)
{
    bool found = tweak.fn_802C4FEC(tweakName, value, category, true, min, max);
    if (!found)
    {
        tweak = tweak.GetDefaultValue();
    }
    if (!found)
    {
        tweak = defaultValue;
    }
}

void fn_8002B934(PlayerTweaks* tweaks, const char* name,
    const char* category, bool registerTweaks)
{
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified004, "mfHeight", 0.5f, category, 0.0f, 0.0f, 0.0f);
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified014, "mfWidth", 0.5f, category, 0.0f, 0.0f, 0.0f);
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified024, "mfMovement_TurningRadius", 0.5f, category, -4.0f, 4.0f, 0.05f);
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified034, "mfMovement_Speed", 0.5f, category, -4.0f, 4.0f, 0.05f);
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified044, "mfMovement_Acceleration", 0.5f, category, -4.0f, 4.0f, 0.05f);
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified054, "mfDefense_SlideTackle", 0.5f, category, -4.0f, 4.0f, 0.05f);
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified064, "mfDefense_Size", 0.5f, category, -4.0f, 4.0f, 0.05f);
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified074, "mfDefense_HittingDistance", 0.5f, category, -4.0f, 4.0f, 0.05f);
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified084, "mfOffense_ShootingWindupTime", 0.5f, category, 0.0f, 4.0f, 0.05f);
    UnidentifiedInitPlayerTweak(tweaks->mUnidentified094, "mfOffense_ShootingWindupTotalTime", 0.5f, category, 0.0f, 4.0f, 0.05f);
    UnidentifiedInitPlayerTweak(tweaks->fShooting, "mfOffense_Shooting", 0.5f, category, -4.0f, 4.0f, 0.05f);
    UnidentifiedInitPlayerTweak(tweaks->fPassing, "mfOffense_Passing", 0.5f, category, -4.0f, 4.0f, 0.05f);

    if (registerTweaks)
    {
        fn_80073A48(lbl_8056BA00, name, category);
    }
    else
    {
        fn_802C6CAC(name, category, true);
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

extern "C" float fn_8002BEB0(PlayerTweaks* tweaks, unsigned int index)
{
    float result = -9999.9f;
    switch (index)
    {
    case 1:
        result = tweaks->mUnidentified034;
        break;
    case 2:
        result = tweaks->mUnidentified054;
        break;
    case 3:
        result = tweaks->mUnidentified074;
        break;
    case 4:
        result = tweaks->fShooting;
        break;
    case 5:
        result = tweaks->fPassing;
        break;
    case 6:
        result = tweaks->mUnidentified054.UnidentifiedGetValue()
               + tweaks->mUnidentified034.UnidentifiedGetValue();
        result = (result + tweaks->mUnidentified074.UnidentifiedGetValue())
               / 3.0f;
        break;
    case 7:
        result = (tweaks->fPassing.UnidentifiedGetValue()
                     + tweaks->fShooting.UnidentifiedGetValue())
               / 2.0f;
        break;
    case 8:
        result = (tweaks->fPassing.UnidentifiedGetValue()
                     + tweaks->mUnidentified034.UnidentifiedGetValue())
               / 2.0f;
        break;
    case 9:
        result = tweaks->mUnidentified074.UnidentifiedGetValue()
               + tweaks->mUnidentified054.UnidentifiedGetValue();
        result = (result + tweaks->fShooting.UnidentifiedGetValue()) / 3.0f;
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
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified054;
    float minimum = fielderTweaks->mUnidentified044;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C08C(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified024;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    return Interpolate(fielderTweaks->mUnidentified48C,
        fielderTweaks->mUnidentified490,
        playerValue);
}

extern "C" float fn_8002C0AC(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified024;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified0B4;
    float minimum = fielderTweaks->mUnidentified0A4;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C180(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified044;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified094;
    float minimum = fielderTweaks->mUnidentified084;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C254(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified0F4;
    float minimum = fielderTweaks->mUnidentified0E4;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C328(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified134;
    float minimum = fielderTweaks->mUnidentified124;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C3FC(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified044;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified154;
    float minimum = fielderTweaks->mUnidentified144;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C4D0(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified024;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified114;
    float minimum = fielderTweaks->mUnidentified104;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C5A4(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified174;
    float minimum = fielderTweaks->mUnidentified164;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C678(PlayerTweaks* tweaks)
{
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float scale = 0.25f * terrain + 0.75f;
    return Interpolate(
        scale * (float)lbl_8056CF08.m_unk14->mUnidentified184,
        scale * (float)lbl_8056CF08.m_unk14->mUnidentified194,
        tweaks->fPassing);
}

extern "C" float fn_8002C6E8(PlayerTweaks*)
{
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    return lbl_8056CF08.m_unk14->mUnidentified4B8
         * (0.25f * terrain + 0.75f);
}

extern "C" float fn_8002C730(PlayerTweaks* tweaks)
{
    return Interpolate(lbl_8056CF08.m_unk14->mUnidentified1A4,
        lbl_8056CF08.m_unk14->mUnidentified1B4,
        tweaks->fPassing);
}

extern "C" float fn_8002C758(PlayerTweaks* tweaks)
{
    return Interpolate(lbl_8056CF08.m_unk14->mUnidentified224,
        lbl_8056CF08.m_unk14->mUnidentified234,
        tweaks->fShooting);
}

extern "C" float fn_8002C780(PlayerTweaks* tweaks)
{
    return Interpolate(lbl_8056CF08.m_unk14->mUnidentified1C4,
        lbl_8056CF08.m_unk14->mUnidentified1D4,
        tweaks->fShooting);
}

extern "C" float fn_8002C7A8(PlayerTweaks* tweaks)
{
    return Interpolate(lbl_8056CF08.m_unk14->mUnidentified1E4,
        lbl_8056CF08.m_unk14->mUnidentified1F4,
        tweaks->fShooting);
}

extern "C" float fn_8002C7D0()
{
    return lbl_8056CF08.m_unk14->mUnidentified3B4;
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
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified304;
    float minimum = fielderTweaks->mUnidentified2F4;
    float terrainMaximum = fielderTweaks->mUnidentified454;
    float terrainMinimum = fielderTweaks->mUnidentified444;
    float terrain = fn_800A928C(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C8D4(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified054;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified344;
    float minimum = fielderTweaks->mUnidentified334;
    float terrainMaximum = fielderTweaks->mUnidentified454;
    float terrainMinimum = fielderTweaks->mUnidentified444;
    float terrain = fn_800A928C(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C9A8(PlayerTweaks* tweaks)
{
    float result = fn_8002C254(tweaks);
    result *= Interpolate(lbl_8056CF08.m_unk14->mUnidentified314,
        lbl_8056CF08.m_unk14->mUnidentified324,
        tweaks->mUnidentified054);
    if (fn_8002BE84(tweaks) > 0.9f)
    {
        result *= 1.175f;
    }
    return result * Interpolate(lbl_8056CF08.m_unk14->mUnidentified424, lbl_8056CF08.m_unk14->mUnidentified434, fn_800A9274(g_pGame->mUnidentified10D8));
}

extern "C" float fn_8002CB2C(PlayerTweaks*)
{
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * (float)lbl_8056CF08.m_unk14->mUnidentified3C4;
}

extern "C" float fn_8002CBB8(PlayerTweaks*)
{
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * (float)lbl_8056CF08.m_unk14->mUnidentified3E4;
}

extern "C" float fn_8002CC44(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified054;
    float minimum = fielderTweaks->mUnidentified044;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return lbl_8056CF08.m_unk14->mUnidentified494 * terrainScale
         * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002CD2C(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified054;
    float minimum = fielderTweaks->mUnidentified044;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return lbl_8056CF08.m_unk14->mUnidentified49C * terrainScale
         * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002CE14(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->mUnidentified034;
    FielderTweaks* fielderTweaks = lbl_8056CF08.m_unk14;
    float maximum = fielderTweaks->mUnidentified074;
    float minimum = fielderTweaks->mUnidentified064;
    float terrainMaximum = fielderTweaks->mUnidentified434;
    float terrainMinimum = fielderTweaks->mUnidentified424;
    float terrain = fn_800A9274(g_pGame->mUnidentified10D8);
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002CEE8(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified488;
}

extern "C" float fn_8002CEFC(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified498;
}

extern "C" float fn_8002CF10(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified474;
}

extern "C" float fn_8002CF24(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified478;
}

extern "C" float fn_8002CF38(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified480;
}

extern "C" float fn_8002CF4C(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified484;
}

extern "C" float fn_8002CF60(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified4AC;
}

extern "C" float fn_8002CF74(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified4B0;
}

extern "C" float fn_8002CF88(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified4C0;
}

extern "C" float fn_8002CF9C(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified4C4;
}

extern "C" float fn_8002CFB0(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified4C8;
}

extern "C" float fn_8002CFC4(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified4BC;
}

extern "C" float fn_8002CFD8(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified3D4;
}

extern "C" float fn_8002CFF0(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->fGreenShellSpeed;
}

extern "C" float fn_8002D008(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified364;
}

extern "C" float fn_8002D020(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified384;
}

extern "C" float fn_8002D038(PlayerTweaks*)
{
    return lbl_8056CF08.m_unk14->mUnidentified374;
}

extern "C" float fn_8002D050(PlayerTweaks* tweaks)
{
    return Interpolate(lbl_8056CF08.m_unk14->mUnidentified394,
        lbl_8056CF08.m_unk14->mUnidentified3A4,
        tweaks->mUnidentified074);
}

bool TweakValue_804F4DC8::fn_8002D078(const char* name, float defaultValue,
    const char* group, bool reload, float value, float min, float max)
{
    bool found = fn_802C4FEC(name, value, group, reload, min, max);
    if (!found)
    {
        *mValue.m_pValue = GetDefaultValue();
    }
    if (!found)
    {
        *mValue.m_pValue = defaultValue;
    }
    return found;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
