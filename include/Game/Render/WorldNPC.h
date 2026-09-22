#ifndef GAME_RENDER_WORLD_NPC_H
#define GAME_RENDER_WORLD_NPC_H

#include "Game/Render/ImpostorCharacter.h"
#include "Game/Render/Frustum.h"
#include "Game/TweakRegistry.h"
#include "NL/nlList.h"
#include "NL/nlMath.h"
#include "types.h"
#include "Game/Render/CrowdModelCollection.h"
#include "Game/World/WorldObject.h"

class GLView;
struct WorldObjectLoadContext;
// Serialized world-NPC record used by the stadium/world object stream.
class WorldNPC : public WorldObject
{
public:
    virtual ~WorldNPC();
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);
    virtual void Initialize(WorldObjectLoadContext* context);

    /* 0x04 */ u8 mUnidentified004[0x1C];
    /* 0x20 */ nlMatrix4 mTransform;
    /* 0x60 */ unsigned long mTemplateHash;
    /* 0x64 */ u8 mUnidentified064[0x0C];
}; // size: 0x70

class WorldNPCManager;

class WorldNPCModelList
    : public ListContainerBase<ImpostorModel*,
          NewAdapter<ListEntry<ImpostorModel*> > >
{
public:
    typedef void (WorldNPCModelList::*EntryCallback)(
        ListEntry<ImpostorModel*>*);

    void Clear()
    {
        EntryCallback callback = &WorldNPCModelList::DeleteModelEntry;
        nlWalkList(m_Head, this, callback);
        m_Head = 0;
        m_Tail = 0;
    }

    void DeleteModelEntry(ListEntry<ImpostorModel*>* entry);
};

class WorldNPCManager
{
public:
    WorldNPCManager();
    virtual ~WorldNPCManager();
    virtual ImpostorModel* CreateNPC(
        unsigned long templateHash, const nlMatrix4& transform);

    void LoadTemplates(const char* filename);
    void AddTemplate(TweakNode* entry, const char* name);
    void BeginModelLoading();
    bool UpdateModelLoading();
    void Render(GLView* view);
    void Update(float dt);
    void RegisterObject(WorldNPC* npc);
    void (*GetModelCallback() const)(ImpostorModel*, glModel*)
    {
        return mModelCallback;
    }

    /* 0x004 */ bool mUnidentified004;
    /* 0x005 */ u8 mPadding005[3];
    /* 0x008 */ CrowdModelCollection* mModelCollection;
    /* 0x00C */ CrowdCharacterDefinition mTemplates[50];
    /* 0x4BC */ bool mSelectedTemplates[50];
    /* 0x4EE */ u8 mPadding4EE[2];
    /* 0x4F0 */ CrowdCharacterDefinition mLoadTemplates[50];
    /* 0x9A0 */ ImpostorModel* mLoadedModels[50];
    /* 0xA68 */ int mNumTemplates;
    /* 0xA6C */ int mNumLoadTemplates;
    /* 0xA70 */ int mNumLoadedModels;
    /* 0xA74 */ WorldNPCModelList mWorldNPCs;
    /* 0xA80 */ nlListContainer<WorldNPC*> mPendingWorldNPCs;
    /* 0xA8C */ bool mTemplatesLoaded;
    /* 0xA8D */ bool mModelsLoaded;
    /* 0xA8E */ u8 mPaddingA8E[2];
    /* 0xA90 */ void (*mModelCallback)(ImpostorModel*, glModel*);
    /* 0xA94 */ bool (*mRenderFilter)(ImpostorModel*);
}; // size: 0xA98

extern WorldNPCManager* gpWorldNPCManager;

#endif // GAME_RENDER_WORLD_NPC_H
