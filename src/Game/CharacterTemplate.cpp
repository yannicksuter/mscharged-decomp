#include <stddef.h>

#include "Game/CharacterTemplate.h"

#include "Game/AnimInventory.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

cCharacter* g_pCharacters[10];
static tCharacterTemplate* g_aCharacterTemplates[20];
static tCharacterTemplate* g_GoalieTemplate;

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

int GetCharacterIndex(const cCharacter* character)
{
    if (character != NULL)
    {
        return character->mUnidentified120;
    }
    return -1;
}
