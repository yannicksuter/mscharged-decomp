#ifndef GAME_RENDER_NPCMANAGER_H
#define GAME_RENDER_NPCMANAGER_H

#include "Game/Inventory.h"
#include "Game/SHierarchy.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlString.h"
#include "types.h"

#include <string.h>

class ChainChomp;
class SkinAnimatedNPC;
class UnidentifiedSkinAnimatedNPC_80199880;
struct HammerObject;
struct Object_8017FFF4;
struct State_8019A710;
struct State_80199E84;
struct KoopaShellObject;
struct ThwompObject;
struct UnidentifiedObject_801B535C;

struct NPCTemplate
{
    NPCTemplate(const char* pName, bool bType)
        : mUnidentified000(false)
        , mUnidentified001(false)
        , mUnidentified002(false)
        , mUnidentified003(false)
        , loaded(false)
        , mUnidentified005(false)
        , modelID(-1)
        , hierarchy(0)
        , mUnidentified010(0)
    {
        mName[0] = '\0';
        strcpy(mName, pName);
        mUnidentified005 = bType;
    }

    /* 0x00 */ bool mUnidentified000;
    /* 0x01 */ bool mUnidentified001;
    /* 0x02 */ bool mUnidentified002;
    /* 0x03 */ bool mUnidentified003;
    /* 0x04 */ bool loaded;
    /* 0x05 */ bool mUnidentified005;
    /* 0x06 */ u8 mPadding006[2];
    /* 0x08 */ int modelID;
    /* 0x0C */ cSHierarchy* hierarchy;
    /* 0x10 */ void* mUnidentified010;
    /* 0x14 */ cInventory<cSAnim> mUnidentified014;
    /* 0x30 */ char mName[40];
}; // total size: 0x58

class NPCManager
{
public:
    NPCManager();
    virtual ~NPCManager();

    void CreateNPCTemplate(const char* pName, bool bType);
    bool fn_801A977C();
    void fn_801A9874();
    void fn_801A9AF8();
    void fn_801A9B64();
    void fn_801A9BD0();
    void fn_801A9C3C();
    Object_8017FFF4* fn_801A9CA4(int nIndex);
    State_8019A710* fn_801A9D10(int nIndex);
    State_8019A710* fn_801A9D20();
    SkinAnimatedNPC* fn_801A9DE0(int nIndex);
    void fn_801A9DF0();
    void fn_801AA088();
    void fn_801AA2C0();
    int fn_801AA32C();
    void fn_801AA348();
    HammerObject* fn_801AA3AC(int nIndex);
    void fn_801AA4C0();
    ThwompObject* fn_801AA528(int nIndex);
    void fn_801AA9D8();
    bool fn_801AABB0();
    void fn_801AAD0C();
    void fn_801AB9D4();
    NPCTemplate* fn_801ABBDC(const char* pName);

    NPCTemplate* fn_801ABBDC_inline(const char* pName)
    {
        for (int i = 0; i < 2; ++i)
        {
            nlDLListIterator<NPCTemplate*> iterator
                = i == 0 ? mUnidentified00C.Begin()
                         : mUnidentified014.Begin();
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

    /* 0x04 */ cInventory<cSHierarchy>* mUnidentified004;
    /* 0x08 */ cInventory<cSHierarchy>* mUnidentified008;
    /* 0x0C */ nlDLListContainer<NPCTemplate*>
        mUnidentified00C;
    /* 0x14 */ nlDLListContainer<NPCTemplate*>
        mUnidentified014;
    /* 0x1C */ NPCTemplate* mUnidentified01C;
    /* 0x20 */ ChainChomp* mpChainChomp;
    /* 0x24 */ UnidentifiedObject_801B535C* mUnidentified024;
    /* 0x28 */ State_80199E84* mUnidentified028;
    /* 0x2C */ KoopaShellObject* mUnidentified02C;
    /* 0x30 */ unsigned int mUnidentified030;
    /* 0x34 */ Object_8017FFF4* mUnidentified034[8];
    /* 0x54 */ unsigned int mUnidentified054;
    /* 0x58 */ State_8019A710* mUnidentified058[6];
    /* 0x70 */ HammerObject* mUnidentified070[15];
    /* 0xAC */ ThwompObject* mUnidentified0AC[8];
    /* 0xCC */ SkinAnimatedNPC* mUnidentified0CC[3];
    /* 0xD8 */ UnidentifiedSkinAnimatedNPC_80199880* mUnidentified0D8;
}; // total size: 0xDC

extern NPCManager* lbl_806E1608;
extern NPCManager* lbl_806E160C;

#endif // GAME_RENDER_NPCMANAGER_H
