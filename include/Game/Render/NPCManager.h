#ifndef GAME_RENDER_NPCMANAGER_H
#define GAME_RENDER_NPCMANAGER_H

#include "Game/Inventory.h"
#include "Game/SHierarchy.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"
#include "types.h"

#include <string.h>

class GLResourcePool;
class ChainChomp;
class SkinAnimatedNPC;
class UnidentifiedNPC_801B43F8;
class DiddyBanana;
struct HammerObject;
struct DaisyFistObject;
struct BulletBillObject;
struct BirdoEggObject;
struct KoopaShellObject;
struct ThwompObject;
struct UnidentifiedObject_801B535C;

struct NPCTemplate
{
    NPCTemplate(const char* pName, bool bPersistent)
        : mAnimationLoadStarted(false)
        , mAnimationsLoaded(false)
        , mHierarchyLoaded(false)
        , mTexturesLoaded(false)
        , loaded(false)
        , mPersistent(false)
        , modelID(-1)
        , hierarchy(0)
        , mResourcePool(0)
    {
        mName[0] = '\0';
        strcpy(mName, pName);
        mPersistent = bPersistent;
    }

    /* 0x00 */ bool mAnimationLoadStarted;
    /* 0x01 */ bool mAnimationsLoaded;
    /* 0x02 */ bool mHierarchyLoaded;
    /* 0x03 */ bool mTexturesLoaded;
    /* 0x04 */ bool loaded;
    /* 0x05 */ bool mPersistent;
    /* 0x06 */ u8 mPadding006[2];
    /* 0x08 */ unsigned long modelID;
    /* 0x0C */ cSHierarchy* hierarchy;
    /* 0x10 */ GLResourcePool* mResourcePool;
    /* 0x14 */ cInventory<cSAnim> mInventorySAnim;
    /* 0x30 */ char mName[40];
}; // total size: 0x58

class NPCManager
{
public:
    NPCManager();
    virtual ~NPCManager();

    void CreateNPCTemplate(const char* pName, bool bPersistent);
    bool SelectNextNPCTemplate();
    ChainChomp* GetChainChomp() const { return mpChainChomp; }
    void CreateChainChomp();
    void fn_801A9AF8();
    void CreateBirdoEgg();
    void fn_801A9BD0();
    void fn_801A9C3C();
    DaisyFistObject* fn_801A9CA4(int nIndex);
    unsigned int UnidentifiedCount054() const { return mUnidentified054; }
    BulletBillObject* fn_801A9D10(int nIndex);
    BulletBillObject* fn_801A9D20();
    UnidentifiedNPC_801B43F8* fn_801A9DE0(int nIndex);
    void fn_801A9DF0();
    void CreateDiddyBanana();
    void fn_801AA2C0();
    int fn_801AA32C();
    void fn_801AA348();
    HammerObject* fn_801AA3AC(int nIndex);
    void fn_801AA4C0();
    ThwompObject* fn_801AA528(int nIndex);
    void BeginLoadNPCTemplate();
    bool FinishLoadNPCTemplate();
    void UnloadTransientNPCTemplates();
    void DestroyNPCs();
    NPCTemplate* fn_801ABBDC(const char* pName);

    NPCTemplate* fn_801ABBDC_inline(const char* pName)
    {
        for (int i = 0; i < 2; ++i)
        {
            nlDLListIterator<NPCTemplate*> iterator
                = i == 0 ? mPersistentTemplates.Begin()
                         : mTransientTemplates.Begin();
            while (iterator.hasNext())
            {
                char name[40];
                unsigned long length = nlStrLen((*iterator)->mName) + 1;
                unsigned long copyLength = sizeof(name);
                if (length <= sizeof(name))
                {
                    copyLength = length;
                }
                nlStrNCpy(name, pName, copyLength);
                if (nlStrICmp(name, (*iterator)->mName) == 0)
                {
                    return *iterator;
                }
                iterator.next();
            }
        }
        return 0;
    }
    void UpdateNPCs(float dt);
    void RenderNPCs();
    void UpdateAINPCs(float dt);
    void fn_801ABF8C();

    /* 0x04 */ cInventory<cSHierarchy>* mPersistentHierarchies;
    /* 0x08 */ cInventory<cSHierarchy>* mTransientHierarchies;
    /* 0x0C */ nlDLListContainer<NPCTemplate*>
        mPersistentTemplates;
    /* 0x14 */ nlDLListContainer<NPCTemplate*>
        mTransientTemplates;
    /* 0x1C */ NPCTemplate* mPendingTemplate;
    /* 0x20 */ ChainChomp* mpChainChomp;
    /* 0x24 */ UnidentifiedObject_801B535C* mUnidentified024;
    /* 0x28 */ BirdoEggObject* mpBirdoEgg;
    /* 0x2C */ KoopaShellObject* mUnidentified02C;
    /* 0x30 */ unsigned int mUnidentified030;
    /* 0x34 */ DaisyFistObject* mDaisyFists[8];
    /* 0x54 */ unsigned int mUnidentified054;
    /* 0x58 */ BulletBillObject* mUnidentified058[6];
    /* 0x70 */ HammerObject* mUnidentified070[15];
    /* 0xAC */ ThwompObject* mUnidentified0AC[8];
    /* 0xCC */ UnidentifiedNPC_801B43F8* mUnidentified0CC[3];
    /* 0xD8 */ DiddyBanana* mpDiddyBanana;
}; // total size: 0xDC

extern NPCManager* gNPCManager;
extern NPCManager* gNPCManagerInstance;

#endif // GAME_RENDER_NPCMANAGER_H
