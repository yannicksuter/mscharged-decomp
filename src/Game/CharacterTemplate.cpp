#include <stddef.h>

#include "Game/CharacterTemplate.h"

#include "Game/AnimInventory.h"
#include "Game/AnimProps/globalanimproperties.h"
#include "Game/AnimProps/goalieanimproperties.h"
#include "Game/CharacterTweaks.h"
#include "Game/Physics/CharacterPhysicsElement.h"
#include "Game/SAnim/AnimRetargeter.h"
#include "Game/SHierarchy.h"
#include "Game/Triggers/AnimTrigger.h"
#include "Game/Triggers/SebringAnimScript.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

extern "C" void fn_801BE234(cSAnim*, unsigned int);

SebringAnimTagScriptInterpreter* g_pAnimScriptInterp;
cCharacter* lbl_806E0C34;
cCharacter* g_pCharacters[10];
static tCharacterTemplate* g_aCharacterTemplates[20];
static tCharacterTemplate* g_GoalieTemplate;

static tCharacterTemplateInfo g_aCharacterTemplateInfo[20] = {
    {
        (eCharacterClass)0,
        "art/characters/mario/mario.rlg",
        "art/characters/mario/mario_shock.rlg",
        NULL,
        "art/characters/mario/mario_shadow.rlg",
        "art/characters/mario/mario.rlt",
        "art/characters/mario/mario_alt.rlt",
        "art/animation/mario.trg",
        fn_801BE234,
        "art/animation/mario.shier",
        "mario",
        GLOBALAnimProperties,
        130,
        "art/animation/mario.sanim.zlib",
        "art/animation/mariofe.sanim",
        "mario",
        "art/animation/mario.cph",
        "/ini/characters/mario.ini",
        "/Game/Gameplay/ini/chars/Mario",
        "/ini/characters/supermario.ini",
        "/Game/Gameplay/ini/chars/SuperMario",
        "art/animation/mario/animretarget/mario.bin",
        0
    },
    {
        (eCharacterClass)1,
        "art/characters/bowser/bowser.rlg",
        "art/characters/bowser/bowser_shock.rlg",
        NULL,
        "art/characters/bowser/bowser_shadow.rlg",
        "art/characters/bowser/bowser.rlt",
        "art/characters/bowser/bowser_alt.rlt",
        "art/animation/bowser.trg",
        fn_801BE234,
        "art/animation/bowser.shier",
        "bowser",
        GLOBALAnimProperties,
        130,
        "art/animation/bowser.sanim.zlib",
        "art/animation/bowserfe.sanim",
        "bowser",
        "art/animation/bowser.cph",
        "/ini/characters/bowser.ini",
        "/Game/Gameplay/ini/chars/Bowser",
        "/ini/characters/superbowser.ini",
        "/Game/Gameplay/ini/chars/SuperBowser",
        "art/animation/bowser/animretarget/bowser.bin",
        0
    },
    {
        (eCharacterClass)2,
        "art/characters/daisy/daisy.rlg",
        "art/characters/daisy/daisy_shock.rlg",
        NULL,
        "art/characters/daisy/daisy_shadow.rlg",
        "art/characters/daisy/daisy.rlt",
        "art/characters/daisy/daisy_alt.rlt",
        "art/animation/daisy.trg",
        fn_801BE234,
        "art/animation/daisy.shier",
        "daisy",
        GLOBALAnimProperties,
        130,
        "art/animation/daisy.sanim.zlib",
        "art/animation/daisyfe.sanim",
        "daisy",
        "art/animation/daisy.cph",
        "/ini/characters/daisy.ini",
        "/Game/Gameplay/ini/chars/Daisy",
        NULL,
        NULL,
        "art/animation/daisy/animretarget/daisy.bin",
        0
    },
    {
        (eCharacterClass)3,
        "art/characters/donkeykong/donkeykong.rlg",
        "art/characters/donkeykong/donkeykong_shock.rlg",
        NULL,
        "art/characters/donkeykong/donkeykong_shadow.rlg",
        "art/characters/donkeykong/donkeykong.rlt",
        "art/characters/donkeykong/donkeykong_alt.rlt",
        "art/animation/donkeykong.trg",
        fn_801BE234,
        "art/animation/donkeykong.shier",
        "donkeykong",
        GLOBALAnimProperties,
        130,
        "art/animation/donkeykong.sanim.zlib",
        "art/animation/donkeykongfe.sanim",
        "donkeykong",
        "art/animation/DonkeyKong.cph",
        "/ini/characters/dk.ini",
        "/Game/Gameplay/ini/chars/DK",
        NULL,
        NULL,
        "art/animation/donkeykong/animretarget/donkeykong.bin",
        0
    },
    {
        (eCharacterClass)4,
        "art/characters/luigi/luigi.rlg",
        "art/characters/luigi/luigi_shock.rlg",
        NULL,
        "art/characters/luigi/luigi_shadow.rlg",
        "art/characters/luigi/luigi.rlt",
        "art/characters/luigi/luigi_alt.rlt",
        "art/animation/luigi.trg",
        NULL,
        "art/animation/luigi.shier",
        "luigi",
        GLOBALAnimProperties,
        130,
        "art/animation/luigi.sanim.zlib",
        "art/animation/luigife.sanim",
        "luigi",
        "art/animation/mario.cph",
        "/ini/characters/luigi.ini",
        "/Game/Gameplay/ini/chars/Luigi",
        "/ini/characters/superluigi.ini",
        "/Game/Gameplay/ini/chars/SuperLuigi",
        "art/animation/luigi/animretarget/luigi.bin",
        0
    },
    {
        (eCharacterClass)5,
        "art/characters/peach/peach.rlg",
        "art/characters/peach/peach_shock.rlg",
        NULL,
        "art/characters/peach/peach_shadow.rlg",
        "art/characters/peach/peach.rlt",
        "art/characters/peach/peach_alt.rlt",
        "art/animation/peach.trg",
        fn_801BE234,
        "art/animation/peach.shier",
        "peach",
        GLOBALAnimProperties,
        130,
        "art/animation/peach.sanim.zlib",
        "art/animation/peachfe.sanim",
        "peach",
        "art/animation/peach.cph",
        "/ini/characters/peach.ini",
        "/Game/Gameplay/ini/chars/Peach",
        NULL,
        NULL,
        "art/animation/peach/animretarget/peach.bin",
        0
    },
    {
        (eCharacterClass)6,
        "art/characters/waluigi/waluigi.rlg",
        "art/characters/waluigi/waluigi_shock.rlg",
        NULL,
        "art/characters/waluigi/waluigi_shadow.rlg",
        "art/characters/waluigi/waluigi.rlt",
        "art/characters/waluigi/waluigi_alt.rlt",
        "art/animation/waluigi.trg",
        NULL,
        "art/animation/waluigi.shier",
        "waluigi",
        GLOBALAnimProperties,
        130,
        "art/animation/waluigi.sanim.zlib",
        "art/animation/waluigife.sanim",
        "waluigi",
        "art/animation/waluigi.cph",
        "/ini/characters/waluigi.ini",
        "/Game/Gameplay/ini/chars/Waluigi",
        "/ini/characters/superwaluigi.ini",
        "/Game/Gameplay/ini/chars/SuperWaluigi",
        "art/animation/waluigi/animretarget/waluigi.bin",
        0
    },
    {
        (eCharacterClass)7,
        "art/characters/wario/wario.rlg",
        "art/characters/wario/wario_shock.rlg",
        NULL,
        "art/characters/wario/wario_shadow.rlg",
        "art/characters/wario/wario.rlt",
        "art/characters/wario/wario_alt.rlt",
        "art/animation/wario.trg",
        NULL,
        "art/animation/wario.shier",
        "wario",
        GLOBALAnimProperties,
        130,
        "art/animation/wario.sanim.zlib",
        "art/animation/wariofe.sanim",
        "wario",
        "art/animation/wario.cph",
        "/ini/characters/wario.ini",
        "/Game/Gameplay/ini/chars/Wario",
        "/ini/characters/superwario.ini",
        "/Game/Gameplay/ini/chars/SuperWario",
        "art/animation/wario/animretarget/wario.bin",
        0
    },
    {
        (eCharacterClass)8,
        "art/characters/yoshi/yoshi.rlg",
        "art/characters/yoshi/yoshi_shock.rlg",
        NULL,
        "art/characters/yoshi/yoshi_shadow.rlg",
        "art/characters/yoshi/yoshi.rlt",
        "art/characters/yoshi/yoshi_alt.rlt",
        "art/animation/yoshi.trg",
        fn_801BE234,
        "art/animation/yoshi.shier",
        "yoshi",
        GLOBALAnimProperties,
        130,
        "art/animation/yoshi.sanim.zlib",
        "art/animation/yoshife.sanim",
        "yoshi",
        "art/animation/yoshi.cph",
        "/ini/characters/yoshi.ini",
        "/Game/Gameplay/ini/chars/Yoshi",
        "/ini/characters/superyoshi.ini",
        "/Game/Gameplay/ini/chars/SuperYoshi",
        "art/animation/yoshi/animretarget/yoshi.bin",
        0
    },
    {
        (eCharacterClass)9,
        "art/characters/bowserjr/bowserjr.rlg",
        "art/characters/bowserjr/bowserjr_shock.rlg",
        NULL,
        "art/characters/bowserjr/bowserjr_shadow.rlg",
        "art/characters/bowserjr/bowserjr.rlt",
        "art/characters/bowserjr/bowserjr_alt.rlt",
        "art/animation/bowserjr.trg",
        fn_801BE234,
        "art/animation/bowserjr.shier",
        "bowserjr",
        GLOBALAnimProperties,
        130,
        "art/animation/bowserjr.sanim.zlib",
        "art/animation/bowserjrfe.sanim",
        "bowserjr",
        "art/animation/bowserjr.cph",
        "/ini/characters/bowserjr.ini",
        "/Game/Gameplay/ini/chars/Bowserjr",
        "/ini/characters/bowserjrshrink.ini",
        "/Game/Gameplay/ini/chars/BowserjrShrink",
        "art/animation/bowserjr/animretarget/bowserjr.bin",
        0
    },
    {
        (eCharacterClass)10,
        "art/characters/diddykong/diddykong.rlg",
        "art/characters/diddykong/diddykong_shock.rlg",
        NULL,
        "art/characters/diddykong/diddykong_shadow.rlg",
        "art/characters/diddykong/diddykong.rlt",
        "art/characters/diddykong/diddykong_alt.rlt",
        "art/animation/diddykong.trg",
        fn_801BE234,
        "art/animation/diddykong.shier",
        "diddykong",
        GLOBALAnimProperties,
        130,
        "art/animation/diddykong.sanim.zlib",
        "art/animation/diddykongfe.sanim",
        "diddykong",
        "art/animation/diddykong.cph",
        "/ini/characters/diddykong.ini",
        "/Game/Gameplay/ini/chars/Diddykong",
        NULL,
        NULL,
        "art/animation/diddykong/animretarget/diddykong.bin",
        0
    },
    {
        (eCharacterClass)11,
        "art/characters/petey/petey.rlg",
        "art/characters/petey/petey_shock.rlg",
        NULL,
        "art/characters/petey/petey_shadow.rlg",
        "art/characters/petey/petey.rlt",
        "art/characters/petey/petey_alt.rlt",
        "art/animation/petey.trg",
        fn_801BE234,
        "art/animation/petey.shier",
        "petey",
        GLOBALAnimProperties,
        130,
        "art/animation/petey.sanim.zlib",
        "art/animation/peteyfe.sanim",
        "petey",
        "art/animation/petey.cph",
        "/ini/characters/petey.ini",
        "/Game/Gameplay/ini/chars/Petey",
        "/ini/characters/superpetey.ini",
        "/Game/Gameplay/ini/chars/SuperPetey",
        "art/animation/petey/animretarget/petey.bin",
        0
    },
    {
        (eCharacterClass)12,
        "art/characters/birdo/birdo.rlg",
        "art/characters/birdo/birdo_shock.rlg",
        "art/characters/birdo/birdo_lowpoly.rlg",
        "art/characters/birdo/birdo_shadow.rlg",
        "art/characters/birdo/birdo.rlt",
        NULL,
        "art/animation/birdo.trg",
        fn_801BE234,
        "art/animation/birdo.shier",
        "birdo",
        GLOBALAnimProperties,
        130,
        "art/animation/birdo.sanim.zlib",
        "art/animation/birdofe.sanim",
        "birdo",
        "art/animation/birdo.cph",
        "/ini/characters/birdo.ini",
        "/Game/Gameplay/ini/chars/Birdo",
        NULL,
        NULL,
        "art/animation/birdo/animretarget/birdo.bin",
        0
    },
    {
        (eCharacterClass)13,
        "art/characters/hammerbro/hammerbro.rlg",
        "art/characters/hammerbro/hammerbro_shock.rlg",
        "art/characters/hammerbro/hammerbro_lowpoly.rlg",
        "art/characters/hammerbro/hammerbro_shadow.rlg",
        "art/characters/hammerbro/hammerbro.rlt",
        NULL,
        "art/animation/hammerbro.trg",
        fn_801BE234,
        "art/animation/hammerbro.shier",
        "hammerbro",
        GLOBALAnimProperties,
        130,
        "art/animation/hammerbro.sanim.zlib",
        "art/animation/hammerbrofe.sanim",
        "hammerbro",
        "art/animation/hammerbro.cph",
        "/ini/characters/hammerbros.ini",
        "/Game/Gameplay/ini/chars/Hammer Bros",
        NULL,
        NULL,
        "art/animation/hammerbro/animretarget/hammerbro.bin",
        0
    },
    {
        (eCharacterClass)14,
        "art/characters/koopa/koopa.rlg",
        "art/characters/koopa/koopa_shock.rlg",
        "art/characters/koopa/koopa_lowpoly.rlg",
        "art/characters/koopa/koopa_shadow.rlg",
        "art/characters/koopa/koopa.rlt",
        NULL,
        "art/animation/koopa.trg",
        fn_801BE234,
        "art/animation/koopa.shier",
        "koopa",
        GLOBALAnimProperties,
        130,
        "art/animation/koopa.sanim.zlib",
        "art/animation/koopafe.sanim",
        "koopa",
        "art/animation/koopa.cph",
        "/ini/characters/koopa.ini",
        "/Game/Gameplay/ini/chars/Koopa",
        NULL,
        NULL,
        "art/animation/koopa/animretarget/koopa.bin",
        0
    },
    {
        (eCharacterClass)15,
        "art/characters/toad/toad.rlg",
        "art/characters/toad/toad_shock.rlg",
        "art/characters/toad/toad_lowpoly.rlg",
        "art/characters/toad/toad_shadow.rlg",
        "art/characters/toad/toad.rlt",
        NULL,
        "art/animation/toad.trg",
        fn_801BE234,
        "art/animation/toad.shier",
        "toad",
        GLOBALAnimProperties,
        130,
        "art/animation/toad.sanim.zlib",
        "art/animation/toadfe.sanim",
        "toad",
        "art/animation/toad.cph",
        "/ini/characters/toad.ini",
        "/Game/Gameplay/ini/chars/Toad",
        NULL,
        NULL,
        "art/animation/toad/animretarget/toad.bin",
        0
    },
    {
        (eCharacterClass)16,
        "art/characters/boo/boo.rlg",
        NULL,
        "art/characters/boo/boo_lowpoly.rlg",
        "art/characters/boo/boo_shadow.rlg",
        "art/characters/boo/boo.rlt",
        NULL,
        "art/animation/boo.trg",
        fn_801BE234,
        "art/animation/boo.shier",
        "boo",
        GLOBALAnimProperties,
        130,
        "art/animation/boo.sanim.zlib",
        "art/animation/boofe.sanim",
        "boo",
        "art/animation/boo.cph",
        "/ini/characters/boo.ini",
        "/Game/Gameplay/ini/chars/Boo",
        NULL,
        NULL,
        "art/animation/boo/animretarget/boo.bin",
        0
    },
    {
        (eCharacterClass)17,
        "art/characters/drybones/drybones.rlg",
        "art/characters/drybones/drybones_shock.rlg",
        "art/characters/drybones/drybones_lowpoly.rlg",
        "art/characters/drybones/drybones_shadow.rlg",
        "art/characters/drybones/drybones.rlt",
        NULL,
        "art/animation/drybones.trg",
        fn_801BE234,
        "art/animation/drybones.shier",
        "drybones",
        GLOBALAnimProperties,
        130,
        "art/animation/drybones.sanim.zlib",
        "art/animation/drybonesfe.sanim",
        "drybones",
        "art/animation/drybones.cph",
        "/ini/characters/drybones.ini",
        "/Game/Gameplay/ini/chars/Drybones",
        NULL,
        NULL,
        "art/animation/drybones/animretarget/drybones.bin",
        0
    },
    {
        (eCharacterClass)18,
        "art/characters/montymole/montymole.rlg",
        "art/characters/montymole/montymole_shock.rlg",
        "art/characters/montymole/montymole_lowpoly.rlg",
        "art/characters/montymole/montymole_shadow.rlg",
        "art/characters/montymole/montymole.rlt",
        NULL,
        "art/animation/montymole.trg",
        fn_801BE234,
        "art/animation/montymole.shier",
        "montymole",
        GLOBALAnimProperties,
        130,
        "art/animation/montymole.sanim.zlib",
        "art/animation/montymolefe.sanim",
        "montymole",
        "art/animation/montymole.cph",
        "/ini/characters/monty.ini",
        "/Game/Gameplay/ini/chars/Monty",
        NULL,
        NULL,
        "art/animation/montymole/animretarget/montymole.bin",
        0
    },
    {
        (eCharacterClass)19,
        "art/characters/shyguy/shyguy.rlg",
        "art/characters/shyguy/shyguy_shock.rlg",
        "art/characters/shyguy/shyguy_lowpoly.rlg",
        "art/characters/shyguy/shyguy_shadow.rlg",
        "art/characters/shyguy/shyguy.rlt",
        NULL,
        "art/animation/shyguy.trg",
        fn_801BE234,
        "art/animation/shyguy.shier",
        "shyguy",
        GLOBALAnimProperties,
        130,
        "art/animation/shyguy.sanim.zlib",
        "art/animation/shyguyfe.sanim",
        "shyguy",
        "art/animation/shyguy.cph",
        "/ini/characters/shyguy.ini",
        "/Game/Gameplay/ini/chars/Shyguy",
        NULL,
        NULL,
        "art/animation/shyguy/animretarget/shyguy.bin",
        0
    },
};

static tCharacterTemplateInfo g_GoalieTemplateInfo = {
    (eCharacterClass)20,
    "art/characters/mariogoalie/mariogoalie.rlg",
    "art/characters/mariogoalie/mariogoalie_shock.rlg",
    "art/characters/mariogoalie/mariogoalie_lowpoly.rlg",
    "art/characters/mariogoalie/mariogoalie_shadow.rlg",
    "art/characters/mariogoalie/mariogoalie.rlt",
    NULL,
    "art/animation/mariogoalie.trg",
    NULL,
    "art/animation/mariogoalie.shier",
    "mariogoalie",
    GOALIEAnimProperties,
    178,
    "art/animation/mariogoalie.sanim.zlib",
    NULL,
    "mariogoalie",
    "art/animation/mariogoalie.cph",
    "/ini/goalie.ini",
    "/Game/Gameplay/ini/chars/Goalie",
    NULL,
    NULL,
    NULL,
    0
};

static tGoalieTemplateInfo g_GoalieTextureInfo[12] = {
    { "mariogoalie", "art/characters/mariogoalie/mariogoalie.rlt", "art/characters/mariogoalie_alt/mariogoalie_alt.rlt", 0 },
    { "bowsergoalie", "art/characters/bowsergoalie/bowsergoalie.rlt", "art/characters/bowsergoalie_alt/bowsergoalie_alt.rlt", 0 },
    { "daisygoalie", "art/characters/daisygoalie/daisygoalie.rlt", "art/characters/daisygoalie_alt/daisygoalie_alt.rlt", 0 },
    { "donkeykonggoalie", "art/characters/donkeykonggoalie/donkeykonggoalie.rlt", "art/characters/donkeykonggoalie_alt/donkeykonggoalie_alt.rlt", 0 },
    { "luigigoalie", "art/characters/luigigoalie/luigigoalie.rlt", "art/characters/luigigoalie_alt/luigigoalie_alt.rlt", 0 },
    { "peachgoalie", "art/characters/peachgoalie/peachgoalie.rlt", "art/characters/peachgoalie_alt/peachgoalie_alt.rlt", 0 },
    { "waluigigoalie", "art/characters/waluigigoalie/waluigigoalie.rlt", "art/characters/waluigigoalie_alt/waluigigoalie_alt.rlt", 0 },
    { "wariogoalie", "art/characters/wariogoalie/wariogoalie.rlt", "art/characters/wariogoalie_alt/wariogoalie_alt.rlt", 0 },
    { "yoshigoalie", "art/characters/yoshigoalie/yoshigoalie.rlt", "art/characters/yoshigoalie_alt/yoshigoalie_alt.rlt", 0 },
    { "bowserjrgoalie", "art/characters/bowserjrgoalie/bowserjrgoalie.rlt", "art/characters/bowserjrgoalie_alt/bowserjrgoalie_alt.rlt", 0 },
    { "diddykonggoalie", "art/characters/diddykonggoalie/diddykonggoalie.rlt", "art/characters/diddykonggoalie_alt/diddykonggoalie_alt.rlt", 0 },
    { "peteygoalie", "art/characters/peteygoalie/peteygoalie.rlt", "art/characters/peteygoalie_alt/peteygoalie_alt.rlt", 0 },
};

SebringAnimTagScriptInterpreter* GetAnimScriptInterpreter()
{
    if (g_pAnimScriptInterp == NULL)
    {
        SebringAnimTagScriptInterpreter* pInterp =
            new (nlMalloc(sizeof(SebringAnimTagScriptInterpreter), 8, false))
                SebringAnimTagScriptInterpreter();
        g_pAnimScriptInterp = pInterp;
    }
    return g_pAnimScriptInterp;
}

tGoalieTemplateInfo* GetGoalieTemplateInfo(int goalieIdx)
{
    return &g_GoalieTextureInfo[goalieIdx];
}

tCharacterTemplate* GetCharacterTemplate(int nIndex, bool* pbCreated)
{
    *pbCreated = false;

    if (nIndex < 20)
    {
        if (g_aCharacterTemplates[nIndex] == NULL)
        {
            g_aCharacterTemplates[nIndex] = (tCharacterTemplate*)nlMalloc(
                sizeof(tCharacterTemplate), 8, false);
            *pbCreated = true;
        }
        return g_aCharacterTemplates[nIndex];
    }

    if (g_GoalieTemplate == NULL)
    {
        g_GoalieTemplate = (tCharacterTemplate*)nlMalloc(
            sizeof(tCharacterTemplate), 8, false);
        *pbCreated = true;
    }
    return g_GoalieTemplate;
}

tCharacterTemplateInfo* GetCharacterTemplateInfo(eCharacterClass cc)
{
    if (cc < 20)
    {
        return &g_aCharacterTemplateInfo[cc];
    }
    return &g_GoalieTemplateInfo;
}

cAnimInventory* FindDuplicateAnimInventory(
    int nCurIndex, unsigned long uHashID)
{
    for (int index = 0; index < 20; index++)
    {
        if (index == nCurIndex)
            continue;
        if (g_aCharacterTemplates[index] == NULL)
            continue;
        if (uHashID
            != g_aCharacterTemplates[index]->uAnimInventoryHashID)
            continue;
        return g_aCharacterTemplates[index]->pAnimInventory;
    }
    return NULL;
}

unsigned long GetHashFromTextureFile(const char* szTextureFileName)
{
    char name[200];
    char* pDest = name;
    const char* pSrc = NULL;
    int count = 0;

    for (count = 0; count < 100; count++)
    {
        if (szTextureFileName[count] == '\\'
            || szTextureFileName[count] == '/')
        {
            pSrc = &szTextureFileName[count + 1];
            break;
        }
    }

    for (int k = 0; k < 100; k++)
    {
        if (*pSrc != '\0' && *pSrc != '.')
        {
            *pDest = *pSrc;
            pDest++;
            pSrc++;
        }
        else
        {
            *pDest = '\0';
            return nlStringLowerHash(name);
        }
    }
    return 0;
}

void DestroyCharacters()
{
    int i;

    delete g_pAnimScriptInterp;
    g_pAnimScriptInterp = NULL;

    for (i = 0; i < 10; i++)
    {
        delete g_pCharacters[i];
        g_pCharacters[i] = NULL;
    }

    for (i = 0; i < 20; i++)
    {
        if (g_aCharacterTemplates[i] != NULL)
        {
            delete g_aCharacterTemplates[i]->pHierarchyInventory;

            if (!g_aCharacterTemplates[i]->bAnimInventoryCopy)
            {
                delete g_aCharacterTemplates[i]->pAnimInventory;
            }

            delete g_aCharacterTemplates[i]->pPhysicsData;
            if (g_aCharacterTemplates[i]->pAnimRetargetListInventory != NULL)
            {
                delete g_aCharacterTemplates[i]->pAnimRetargetListInventory;
            }
            if (g_aCharacterTemplates[i]->pUnidentified28 != NULL)
            {
                delete g_aCharacterTemplates[i]->pUnidentified28;
            }
            if (g_aCharacterTemplates[i]->pUnidentified2C != NULL)
            {
                delete g_aCharacterTemplates[i]->pUnidentified2C;
            }
            delete g_aCharacterTemplates[i];
            g_aCharacterTemplates[i] = NULL;
        }
    }

    if (g_GoalieTemplate != NULL)
    {
        delete g_GoalieTemplate->pHierarchyInventory;

        if (!g_GoalieTemplate->bAnimInventoryCopy)
        {
            delete g_GoalieTemplate->pAnimInventory;
        }

        delete g_GoalieTemplate->pPhysicsData;
        if (g_GoalieTemplate->pAnimRetargetListInventory != NULL)
        {
            delete g_GoalieTemplate->pAnimRetargetListInventory;
        }
        if (g_GoalieTemplate->pUnidentified30 != NULL)
        {
            delete g_GoalieTemplate->pUnidentified30;
        }
        delete g_GoalieTemplate;
        g_GoalieTemplate = NULL;
    }

    for (i = 0; i < 12; i++)
    {
        g_GoalieTextureInfo[i].bLoaded = 0;
    }
    for (i = 0; i < 20; i++)
    {
        g_aCharacterTemplateInfo[i].bUnidentified58 = 0;
    }

    AnimTriggerCallbackInfo::m_AnimTriggerCallbackInfoSlotPool.FreeBlocks();
}

int GetCharacterIndex(const cCharacter* character)
{
    if (character != NULL)
    {
        return character->mUnidentified120;
    }
    return -1;
}
