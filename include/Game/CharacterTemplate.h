#ifndef GAME_CHARACTER_TEMPLATE_H
#define GAME_CHARACTER_TEMPLATE_H

#include "Game/Character.h"
#include "Game/Inventory.h"

class AnimRetargetList;
class CharacterPhysicsData;
class GoalieTweaks;
class PlayerTweaks;
class cAnimInventory;
class cSHierarchy;
class cSAnim;
class SebringAnimTagScriptInterpreter;
struct AnimProperties;

struct tCharacterTemplate
{
    /* 0x00 */ int nCharacterModelID[4];
    /* 0x10 */ unsigned long uAnimInventoryHashID;
    /* 0x14 */ unsigned char bAnimInventoryCopy;
    /* 0x15 */ unsigned char pad_0x15[3];
    /* 0x18 */ cInventory<cSHierarchy>* pHierarchyInventory;
    /* 0x1C */ cAnimInventory* pAnimInventory;
    /* 0x20 */ const CharacterPhysicsData* pPhysicsData;
    /* 0x24 */ cInventory<AnimRetargetList>* pAnimRetargetListInventory;
    /* 0x28 */ PlayerTweaks* pPlayerTweaks;
    /* 0x2C */ PlayerTweaks* pSuperPlayerTweaks;
    /* 0x30 */ GoalieTweaks* pGoalieTweaks;
}; // total size: 0x34

struct tGoalieTemplateInfo
{
    /* 0x00 */ const char* szCharName;
    /* 0x04 */ const char* szTextureFilename;
    /* 0x08 */ const char* szAlternateTextureFilename;
    /* 0x0C */ unsigned char bTexturesLoaded;
    /* 0x0D */ unsigned char pad_0x0D[3];
}; // total size: 0x10

struct tCharacterTemplateInfo
{
    /* 0x00 */ eCharacterClass cc;
    /* 0x04 */ const char* szModelFilename;
    /* 0x08 */ const char* szShockModelFilename;
    /* 0x0C */ const char* szLowPolyModelFilename;
    /* 0x10 */ const char* szShadowModelFilename;
    /* 0x14 */ const char* szTextureFilename;
    /* 0x18 */ const char* szAlternateTextureFilename;
    /* 0x1C */ const char* szTriggerFilename;
    /* 0x20 */ void (*pTriggerCallback)(cSAnim*, unsigned int);
    /* 0x24 */ const char* szHierarchyFilename;
    /* 0x28 */ const char* szHierarchy;
    /* 0x2C */ const AnimProperties* pAnimProperties;
    /* 0x30 */ int nNumAnimProperties;
    /* 0x34 */ const char* szAnimFilename;
    /* 0x38 */ const char* szFEAnimFilename;
    /* 0x3C */ const char* szEffectsName;
    /* 0x40 */ const char* szPhysicsFilename;
    /* 0x44 */ const char* szTweaksFilename;
    /* 0x48 */ const char* szTweaksCategory;
    /* 0x4C */ const char* szSuperTweaksFilename;
    /* 0x50 */ const char* szSuperTweaksCategory;
    /* 0x54 */ const char* szAnimRetargetFilename;
    /* 0x58 */ unsigned char bTexturesLoaded;
    /* 0x59 */ unsigned char pad_0x59[3];
}; // total size: 0x5C

SebringAnimTagScriptInterpreter* GetAnimScriptInterpreter();
tGoalieTemplateInfo* GetGoalieTemplateInfo(int goalieIdx);
tCharacterTemplateInfo* GetCharacterTemplateInfo(eCharacterClass cc);
tCharacterTemplate* GetCharacterTemplate(int nIndex, bool* pbCreated);
cAnimInventory* FindDuplicateAnimInventory(int nCurIndex, unsigned long uHashID);
unsigned long GetHashFromTextureFile(const char* szTextureFileName);
void DestroyCharacters();
int GetCharacterIndex(const cCharacter* character);

extern cCharacter* g_pCharacters[10];


extern cCharacter* g_pCurrentlyUpdatingCharacter;

#endif // GAME_CHARACTER_TEMPLATE_H
