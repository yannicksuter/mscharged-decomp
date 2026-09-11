#include "Game/GameTweaks.h"
#include "Game/Terrain.h"
#include "Game/GameInfo.h"
#include "Game/TweakFileLoader.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/DB/StadiumInfo.h"
#include "NL/nlConfig.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

extern const char sUnidentifiedHomeSkillCategory[];
extern const char sUnidentifiedAwaySkillCategory[];
extern char sUnidentifiedTerrainCategory[];

static const char* sSkillTweakCategories[2] = {
    sUnidentifiedHomeSkillCategory,
    sUnidentifiedAwaySkillCategory,
};

TweakFileLoader gTweakFileLoader;
GameTweaksManager gGameTweaks;

void InitializeGameTweaks(GameTweaksManager* state)
{
    gTweakFileLoader.mCount = 0;

    int stadium = GameInfoManager::Instance()->GetStadium();
    int terrain = GetStadiumUnknown0x08(stadium);
    state->mTerrainType = terrain;

    char terrainName[100];
    GetTerrainConfigFilename(terrain, terrainName, sizeof(terrainName));

    TerrainTweaks* terrainTweaks = new (8, false)
        TerrainTweaks(terrainName, sUnidentifiedTerrainCategory);
    state->mTerrainTweaks = terrainTweaks;

    state->mUnidentified08 = GetStadiumUnknown0x0C(stadium);
    state->mUnidentified0C = GetStadiumUnknown0x11(stadium);

    GameTweaks* gameTweaks = new (8, false)
        GameTweaks("/ini/GameTweaks.ini", "/Game/GameTweaks");
    state->m_pGameTweaks = gameTweaks;

    state->mFielderTweaks =
        new (nlMalloc(sizeof(FielderTweaks), 8, false))
            FielderTweaks("/ini/CharTweaks.ini", "/Game/Characters");

    state->mUnidentified18[0] =
        new (nlMalloc(sizeof(SkillTweaks), 8, false))
            SkillTweaks(sSkillTweakCategories[0]);
    state->mUnidentified18[1] =
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
    state->mUnidentified18[0]->Init(difficulties[0], true, false);
    state->mUnidentified18[1]->Init(difficulties[1], true, false);
}

bool UpdateGameTweaksLoading(GameTweaksManager*)
{
    return gTweakFileLoader.ProcessLoadedFiles();
}

void DestroyGameTweaks(GameTweaksManager* state)
{
    if (state->mTerrainTweaks != 0)
    {
        delete state->mTerrainTweaks;
        state->mTerrainTweaks = 0;
    }

    if (state->m_pGameTweaks != 0)
    {
        delete state->m_pGameTweaks;
        state->m_pGameTweaks = 0;
    }

    if (state->mFielderTweaks != 0)
    {
        delete state->mFielderTweaks;
        state->mFielderTweaks = 0;
    }

    if (state->mUnidentified18[0] != 0)
    {
        delete state->mUnidentified18[0];
        state->mUnidentified18[0] = 0;
    }

    if (state->mUnidentified18[1] != 0)
    {
        delete state->mUnidentified18[1];
        state->mUnidentified18[1] = 0;
    }
}

char sUnidentifiedTerrainCategory[] = "Game/Terrain/";

