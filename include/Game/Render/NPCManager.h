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
struct YoshiEggObject;

struct NPCTemplate
{
    NPCTemplate()
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
    static NPCManager* fn_801948A0();
    DiddyBanana* fn_801919A4() const
    {
        return mpDiddyBanana;
    }
    NPCManager();
    virtual ~NPCManager();

    void CreateNPCTemplate(const char* pName, bool bPersistent);
    bool SelectNextNPCTemplate();
    ChainChomp* GetChainChomp() const { return mpChainChomp; }
    void CreateChainChomp();
    void CreateYoshiEgg();
    void CreateBirdoEgg();
    void CreateKoopaShell();
    void CreateDaisyFists();
    DaisyFistObject* GetDaisyFist(int nIndex);
    unsigned int GetNumBulletBills() const { return mNumBulletBills; }
    BulletBillObject* GetBulletBill(int nIndex);
    BulletBillObject* fn_801A9D20();
    UnidentifiedNPC_801B43F8* fn_801A9DE0(int nIndex);
    void CreateWindDebris();
    void CreateDiddyBanana();
    void CreateHammers();
    int GetNumHammers();
    void ResetActiveHammers();
    HammerObject* GetHammer(int nIndex);
    void CreateThwomps();
    ThwompObject* GetThwomp(int nIndex);
    void BeginLoadNPCTemplate();
    bool FinishLoadNPCTemplate();
    void UnloadTransientNPCTemplates();
    void DestroyNPCs();
    NPCTemplate* FindNPCTemplate(const char* pName);

    NPCTemplate* fn_801ABBDC_inline(const char* pName)
    {
        for (int i = 0; i < 2; ++i)
        {
            nlDLListIterator<NPCTemplate*> iterator;
            iterator = i == 0 ? mPersistentTemplates.Begin()
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
    void ResetNPCs();

    /* 0x04 */ cInventory<cSHierarchy>* mPersistentHierarchies;
    /* 0x08 */ cInventory<cSHierarchy>* mTransientHierarchies;
    /* 0x0C */ nlDLListContainer<NPCTemplate*>
        mPersistentTemplates;
    /* 0x14 */ nlDLListContainer<NPCTemplate*>
        mTransientTemplates;
    /* 0x1C */ NPCTemplate* mPendingTemplate;
    /* 0x20 */ ChainChomp* mpChainChomp;
    /* 0x24 */ YoshiEggObject* mpYoshiEgg;
    /* 0x28 */ BirdoEggObject* mpBirdoEgg;
    /* 0x2C */ KoopaShellObject* mpKoopaShell;
    /* 0x30 */ unsigned int mNumVisibleDaisyFists;
    /* 0x34 */ DaisyFistObject* mDaisyFists[8];
    /* 0x54 */ unsigned int mNumBulletBills;
    /* 0x58 */ BulletBillObject* mBulletBills[6];
    /* 0x70 */ HammerObject* mHammers[15];
    /* 0xAC */ ThwompObject* mThwomps[8];
    /* 0xCC */ UnidentifiedNPC_801B43F8* mWindDebris[3];
    /* 0xD8 */ DiddyBanana* mpDiddyBanana;
}; // total size: 0xDC

extern NPCManager* gNPCManager;
extern NPCManager* gNPCManagerInstance;

#endif // GAME_RENDER_NPCMANAGER_H
