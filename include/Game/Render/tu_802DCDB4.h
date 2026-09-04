#ifndef GAME_RENDER_TU_802DCDB4_H
#define GAME_RENDER_TU_802DCDB4_H

#include "Game/Render/ImpostorCharacter.h"
#include "Game/TweakRegistry.h"
#include "NL/nlList.h"
#include "NL/nlMath.h"
#include "types.h"
#include "unclassified/tu_801A4188.h"

class GLView;
class WorldNPC_802DE058;

class WorldNPCManager_802DD4F0;

class WorldNPCModelList_802DE138
    : public ListContainerBase<ImpostorModel_802DAEE0*,
          NewAdapter<ListEntry<ImpostorModel_802DAEE0*> > >
{
public:
    typedef void (WorldNPCModelList_802DE138::*EntryCallback)(
        ListEntry<ImpostorModel_802DAEE0*>*);

    void UnidentifiedClear()
    {
        EntryCallback callback = &WorldNPCModelList_802DE138::fn_802DE138;
        nlWalkList(m_Head, this, callback);
        m_Head = 0;
        m_Tail = 0;
    }

    void fn_802DE138(ListEntry<ImpostorModel_802DAEE0*>* entry);
};

class WorldNPCManager_802DD4F0
{
public:
    WorldNPCManager_802DD4F0();
    virtual ~WorldNPCManager_802DD4F0();
    virtual ImpostorModel_802DAEE0* fn_802DDD88(
        unsigned long templateHash, const nlMatrix4& transform);

    void fn_802DD790();
    void fn_802DD818(TweakNode_8052BEB0* entry, const char* name);
    void fn_802DDB54();
    bool fn_802DDC1C();
    void Render(GLView* view);
    void fn_802DDFE8(float dt);

    /* 0x004 */ bool mUnidentified004;
    /* 0x005 */ u8 mPadding005[3];
    /* 0x008 */ CrowdModelCollection_801A4188* mModelCollection;
    /* 0x00C */ CrowdCharacterDefinition_801A4188 mTemplates[50];
    /* 0x4BC */ bool mSelectedTemplates[50];
    /* 0x4EE */ u8 mPadding4EE[2];
    /* 0x4F0 */ CrowdCharacterDefinition_801A4188 mLoadTemplates[50];
    /* 0x9A0 */ ImpostorModel_802DAEE0* mLoadedModels[50];
    /* 0xA68 */ int mNumTemplates;
    /* 0xA6C */ int mNumLoadTemplates;
    /* 0xA70 */ int mNumLoadedModels;
    /* 0xA74 */ WorldNPCModelList_802DE138 mWorldNPCs;
    /* 0xA80 */ nlListContainer<WorldNPC_802DE058*> mPendingWorldNPCs;
    /* 0xA8C */ bool mTemplatesLoaded;
    /* 0xA8D */ bool mModelsLoaded;
    /* 0xA8E */ u8 mPaddingA8E[2];
    /* 0xA90 */ void (*mModelCallback)(ImpostorModel_802DAEE0*, glModel*);
    /* 0xA94 */ bool (*mRenderFilter)(ImpostorModel_802DAEE0*);
}; // size: 0xA98

extern WorldNPCManager_802DD4F0* gpWorldNPCManager;

enum FrustumResult_802DD05C
{
    FRUSTUM_OUTSIDE = 0,
    FRUSTUM_INSIDE = 1,
    FRUSTUM_INTERSECTING = 2
};

extern "C"
{
    void fn_802DCDB4(nlVector4* pPlanes, const nlMatrix4& projection,
        const nlMatrix4& view);
    FrustumResult_802DD05C fn_802DD05C(const nlVector4* pPlanes,
        const nlVector3* pBoundsMin, const nlVector3* pBoundsMax,
        unsigned long* pPlaneMask);
    FrustumResult_802DD05C fn_802DD1EC(const nlVector4* pPlanes,
        const nlVector3* pPosition, float fRadius);
    void fn_802DD2F4(const nlVector4* pPlanes, nlVector4* pCorners);
}

#endif // GAME_RENDER_TU_802DCDB4_H
