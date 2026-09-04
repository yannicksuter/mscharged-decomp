#include "Game/GameTweaks.h"
#include "Game/GameInfo.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/DB/StadiumInfo.h"
#include "NL/nlConfig.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "unclassified/tu_80073898.h"

extern "C" bool fn_80073BC0(void*);
extern "C" int fn_800A9154(int, char*, unsigned long);
extern "C" unk_800A9274* fn_800A9888(
    unk_800A9274*, const char*, const char*);
extern "C" GameTweaks* fn_80074F04(
    GameTweaks*, const char*, const char*);
extern "C" void fn_800A2290(SkillTweaks*, int, bool, bool);

extern const char sUnidentifiedHomeSkillCategory[];
extern const char sUnidentifiedAwaySkillCategory[];
extern char sUnidentifiedTerrainCategory[];

static const char* sSkillTweakCategories[2] = {
    sUnidentifiedHomeSkillCategory,
    sUnidentifiedAwaySkillCategory,
};

UnidentifiedTweakLoadState lbl_8056BA00;
unk_8056CF08 lbl_8056CF08;

extern "C" void fn_80073C98(unk_8056CF08* state)
{
    lbl_8056BA00.mCount = 0;

    int stadium = GameInfoManager::Instance()->GetStadium();
    int terrain = GetStadiumUnknown0x08(stadium);
    state->mUnidentified00 = terrain;

    char terrainName[100];
    fn_800A9154(terrain, terrainName, sizeof(terrainName));

    unk_800A9274* terrainTweaks =
        (unk_800A9274*)nlMalloc(sizeof(unk_800A9274), 8, false);
    if (terrainTweaks != 0)
    {
        terrainTweaks =
            fn_800A9888(
                terrainTweaks, terrainName, sUnidentifiedTerrainCategory);
    }
    state->mUnidentified04 = terrainTweaks;

    state->mUnidentified08 = GetStadiumUnknown0x0C(stadium);
    state->mUnidentified0C = GetStadiumUnknown0x11(stadium);

    GameTweaks* gameTweaks =
        (GameTweaks*)nlMalloc(sizeof(GameTweaks), 8, false);
    if (gameTweaks != 0)
    {
        gameTweaks = fn_80074F04(
            gameTweaks, "/ini/GameTweaks.ini", "/Game/GameTweaks");
    }
    state->m_pGameTweaks = gameTweaks;

    state->m_unk14 =
        new (nlMalloc(sizeof(FielderTweaks), 8, false))
            FielderTweaks("/ini/CharTweaks.ini", "/Game/Characters");

    state->mUnidentified18 =
        new (nlMalloc(sizeof(SkillTweaks), 8, false))
            SkillTweaks(sSkillTweakCategories[0]);
    state->mUnidentified1C =
        new (nlMalloc(sizeof(SkillTweaks), 8, false))
            SkillTweaks(sSkillTweakCategories[1]);

    GameInfoManager::Instance()->GetMappedRule0x0();
    int difficulties[2] = {
        GameInfoManager::Instance()->mCurrentDifficulty[0],
        GameInfoManager::Instance()->mCurrentDifficulty[1],
    };

    for (int i = 0; i < 2; i++)
    {
        if (difficulties[i] == 7
            || !Config::Global().Exists("DifficultyOverride"))
        {
            continue;
        }

        int difficulty = difficulties[i];
        const char* overrideName =
            Config::Global().Get<const char*>(
                "DifficultyOverride", "Professional");

        if (nlStrCmp<char>(overrideName, "Braindead") == 0)
        {
            difficulty = 0;
        }
        else if (nlStrCmp<char>(overrideName, "Rookie") == 0)
        {
            difficulty = 1;
        }
        else if (nlStrCmp<char>(overrideName, "Professional") == 0)
        {
            difficulty = 2;
        }
        else if (nlStrCmp<char>(overrideName, "Superstar") == 0)
        {
            difficulty = 3;
        }
        else if (nlStrCmp<char>(overrideName, "Legendary") == 0)
        {
            difficulty = 4;
        }
        else if (nlStrCmp<char>(overrideName, "Megastriker") == 0)
        {
            difficulty = 5;
        }
        else if (nlStrCmp<char>(overrideName, "Human") == 0)
        {
            difficulty = 7;
        }
        else if (nlStrCmp<char>(overrideName, "Superhuman") == 0)
        {
            difficulty = 6;
        }
        else if (nlStrCmp<char>(overrideName, "Baseline") == 0)
        {
            difficulty = 8;
        }

        difficulties[i] = difficulty;
        GameInfoManager::Instance()->mCurrentDifficulty[0] = difficulties[0];
        GameInfoManager::Instance()->mCurrentDifficulty[1] = difficulties[1];
    }
    fn_800A2290(state->mUnidentified18, difficulties[0], true, false);
    fn_800A2290(state->mUnidentified1C, difficulties[1], true, false);
}

extern "C" bool fn_80074198(void*)
{
    return fn_80073BC0(&lbl_8056BA00);
}

extern "C" void fn_800741A4(unk_8056CF08* state)
{
    if (state->mUnidentified04 != 0)
    {
        delete state->mUnidentified04;
        state->mUnidentified04 = 0;
    }

    if (state->m_pGameTweaks != 0)
    {
        delete state->m_pGameTweaks;
        state->m_pGameTweaks = 0;
    }

    if (state->m_unk14 != 0)
    {
        delete state->m_unk14;
        state->m_unk14 = 0;
    }

    if (state->mUnidentified18 != 0)
    {
        delete state->mUnidentified18;
        state->mUnidentified18 = 0;
    }

    if (state->mUnidentified1C != 0)
    {
        delete state->mUnidentified1C;
        state->mUnidentified1C = 0;
    }
}

char sUnidentifiedTerrainCategory[] = "Game/Terrain/";

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
