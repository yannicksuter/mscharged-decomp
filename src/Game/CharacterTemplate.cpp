#include <stddef.h>

#include "Game/CharacterTemplate.h"

#include "Game/AnimInventory.h"
#include "Game/CharacterTweaks.h"
#include "Game/Physics/CharacterPhysicsElement.h"
#include "Game/SAnim/AnimRetargeter.h"
#include "Game/SHierarchy.h"
#include "Game/Triggers/AnimTrigger.h"
#include "Game/Triggers/SebringAnimScript.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

cCharacter* g_pCharacters[10];
static tCharacterTemplate* g_aCharacterTemplates[20];
static tCharacterTemplate* g_GoalieTemplate;

extern tGoalieTemplateInfo g_GoalieTextureInfo[12];
extern tCharacterTemplateInfo g_aCharacterTemplateInfo[20];
extern tCharacterTemplateInfo g_GoalieTemplateInfo;
extern SebringAnimTagScriptInterpreter* g_pAnimScriptInterp;

SebringAnimTagScriptInterpreter* fn_80025E9C()
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

tGoalieTemplateInfo* fn_80025F48(int goalieIdx)
{
    return &g_GoalieTextureInfo[goalieIdx];
}

extern "C" tCharacterTemplate* fn_80025F5C(int arg0, bool* arg1)
{
    *arg1 = false;

    if (arg0 < 20)
    {
        if (g_aCharacterTemplates[arg0] == NULL)
        {
            g_aCharacterTemplates[arg0] = (tCharacterTemplate*)nlMalloc(
                sizeof(tCharacterTemplate), 8, false);
            *arg1 = true;
        }
        return g_aCharacterTemplates[arg0];
    }

    if (g_GoalieTemplate == NULL)
    {
        g_GoalieTemplate = (tCharacterTemplate*)nlMalloc(
            sizeof(tCharacterTemplate), 8, false);
        *arg1 = true;
    }
    return g_GoalieTemplate;
}

tCharacterTemplateInfo* fn_8002600C(eCharacterClass cc)
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

    g_GoalieTextureInfo[0].bLoaded = 0;
    g_GoalieTextureInfo[1].bLoaded = 0;
    g_GoalieTextureInfo[2].bLoaded = 0;
    g_GoalieTextureInfo[3].bLoaded = 0;
    g_GoalieTextureInfo[4].bLoaded = 0;
    g_GoalieTextureInfo[5].bLoaded = 0;
    g_GoalieTextureInfo[6].bLoaded = 0;
    g_GoalieTextureInfo[7].bLoaded = 0;
    g_GoalieTextureInfo[8].bLoaded = 0;
    g_GoalieTextureInfo[9].bLoaded = 0;
    g_GoalieTextureInfo[10].bLoaded = 0;
    g_GoalieTextureInfo[11].bLoaded = 0;

    g_aCharacterTemplateInfo[0].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[1].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[2].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[3].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[4].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[5].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[6].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[7].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[8].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[9].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[10].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[11].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[12].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[13].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[14].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[15].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[16].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[17].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[18].bUnidentified58 = 0;
    g_aCharacterTemplateInfo[19].bUnidentified58 = 0;

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
