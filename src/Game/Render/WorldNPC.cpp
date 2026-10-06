#include "NL/nlDLListContainer.inl"
#include "Game/Render/WorldNPC.h"

#include "Game/SAnim/pnSAnimController.h"
#include "Game/TweakRegistry.h"
#include "Game/TweakConfig.h"
#include "NL/gl/glMemory.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"
#include "Game/Render/CrowdImpostors.h"
#include "NL/nlstring_tmpl.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/World/WorldObject.h"

bool gDisableWorldNPCs;
WorldNPCManager* gpWorldNPCManager;

static const char* sWorldNPCTweakPath = "/Render/WorldNPCs";

inline void WorldNPCManager::RegisterObject(WorldNPC* npc)
{
    unsigned long templateHash;
    bool found;
    int index;

    found = false;
    index = 0;
    templateHash = npc->mTemplateHash;
    for (; index < mNumTemplates; ++index)
    {
        if (templateHash
            == nlStringLowerHash(mTemplates[index].mName))
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        mSelectedTemplates[index] = true;
    }
    mPendingWorldNPCs.AddEnd(npc);
}

struct TemplateLookupResult
{
    int index;
    bool found;
};

static inline void FindLoadedTemplate(const WorldNPCManager& manager,
    unsigned long templateHash, TemplateLookupResult& result)
{
    result.found = false;
    result.index = 0;
    for (; result.index < manager.mNumLoadTemplates; ++result.index)
    {
        if (templateHash
            == nlStringLowerHash(manager.mLoadTemplates[result.index].mName))
        {
            result.found = true;
            break;
        }
    }
}

WorldNPCManager::WorldNPCManager()
    : mUnidentified004(false)
    , mNumTemplates(0)
    , mNumLoadTemplates(0)
    , mNumLoadedModels(0)
    , mTemplatesLoaded(false)
    , mModelsLoaded(false)
    , mModelCallback(0)
{
    mModelCollection = new (8, false) CrowdModelCollection;
    gpWorldNPCManager = this;
}

WorldNPCManager::~WorldNPCManager()
{
    delete mModelCollection;

    for (int i = 0; i < mNumLoadedModels; ++i)
    {
        delete mLoadedModels[i];
    }

    mWorldNPCs.Clear();
    mPendingWorldNPCs.Clear();
    gpWorldNPCManager = 0;
}

void WorldNPCManager::LoadTemplates(const char*)
{
    LoadTweakConfigFile("ini/WorldNPCs.ini", sWorldNPCTweakPath, false);
    TweakEntry* entry
        = FindOrCreateTweakPath(GetTweakRoot(), sWorldNPCTweakPath, true);

    for (TweakNode* child = entry->m_ChildHead; child != 0;
        child = child->m_Next)
    {
        AddTemplate(child, GetTweakNodeName(child));
    }
    mTemplatesLoaded = true;
}

static char sWorldNPCAnimationFileKey[] = "AnimationFile";
static char sWorldNPCHierarchyFileKey[] = "HierarchyFile";
static char sWorldNPCHierarchyNameKey[] = "HierarchyName";
static char sWorldNPCTextureBundleFileKey[] = "TextureBundleFile";
static char sWorldNPCModelFileKey[] = "ModelFile";

void WorldNPCManager::AddTemplate(
    TweakNode* entry, const char* name)
{
    const char* hierarchyFile = 0;
    const char* hierarchyName = 0;
    const char* animationFile = 0;
    const char* modelFile = 0;
    const char* textureBundleFile = 0;

    for (TweakNode* child
         = ((TweakEntry*)entry)->m_ChildHead;
         child != 0; child = child->m_Next)
    {
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCAnimationFileKey) == 0)
        {
            animationFile
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCHierarchyFileKey) == 0)
        {
            hierarchyFile
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCHierarchyNameKey) == 0)
        {
            hierarchyName
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCTextureBundleFileKey) == 0)
        {
            textureBundleFile
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCModelFileKey) == 0)
        {
            modelFile
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
    }

    if (hierarchyFile != 0 && hierarchyName != 0 && animationFile != 0
        && modelFile != 0 && textureBundleFile != 0)
    {
        CrowdCharacterDefinition& definition
            = mTemplates[mNumTemplates];
        definition.mName = name;
        definition.mAnimationFile = animationFile;
        definition.mHierarchyFile = hierarchyFile;
        definition.mHierarchyName = hierarchyName;
        definition.mTextureBundleFile = textureBundleFile;
        definition.mModelFile = modelFile;
        mSelectedTemplates[mNumTemplates] = false;
        ++mNumTemplates;
    }
}

inline void WorldNPCManager::CollectSelectedTemplates()
{
    for (int i = 0; i < mNumTemplates; ++i)
    {
        if (mSelectedTemplates[i])
        {
            mLoadTemplates[mNumLoadTemplates] = mTemplates[i];
            ++mNumLoadTemplates;
        }
    }
}

void WorldNPCManager::BeginModelLoading()
{
    CollectSelectedTemplates();
    mModelCollection->Initialize(mLoadTemplates, mNumLoadTemplates);
    if (mModelCollection->HasMoreModels())
    {
        mModelCollection->BeginNextModelLoad();
    }
}

bool WorldNPCManager::UpdateModelLoading()
{
    if (mNumLoadTemplates == 0)
    {
        return true;
    }
    if (mModelCollection->UpdateModelLoad())
    {
        mModelCollection->CreateLoadedModel();
        mLoadedModels[mNumLoadedModels] = mModelCollection->mModels[mNumLoadedModels];
        ++mNumLoadedModels;

        if (mModelCollection->HasMoreModels())
        {
            mModelCollection->BeginNextModelLoad();
            return false;
        }

        mModelsLoaded = true;
        for (ListEntry<WorldNPC*>* entry = mPendingWorldNPCs.m_Head;
            entry != 0; entry = entry->next)
        {
            WorldNPC* npc = entry->entry;
            nlMatrix4 transform = npc->mTransform;
            CreateNPC(npc->mTemplateHash, transform);
        }
        return true;
    }
    return false;
}

ImpostorModel* WorldNPCManager::CreateNPC(
    unsigned long templateHash, const nlMatrix4& transform)
{
    TemplateLookupResult lookup;
    FindLoadedTemplate(*this, templateHash, lookup);
    ImpostorModel* source
        = lookup.found ? mLoadedModels[lookup.index] : 0;
    ImpostorModel* model
        = source->Clone(glGetCurrentResourcePool());
    if (mModelCallback != 0)
    {
        model->mModelCallback = GetModelCallback();
    }

    model->PlayAnimation("idle", 0.0f, PM_CYCLIC);
    model->SetAnimationTime(nlRandomf(0.0f, 1.0f, &nlDefaultSeed));
    model->mWorldMatrix = transform;
    mWorldNPCs.AddEnd(model);
    return model;
}

void WorldNPCManager::Render(GLView* view)
{
    if (gDisableWorldNPCs)
    {
        return;
    }

    ListEntry<ImpostorModel*>* entry = mWorldNPCs.m_Head;
    while (entry != 0)
    {
        ImpostorModel* model = entry->entry;
        if (mRenderFilter != 0 && !mRenderFilter(model))
        {
            entry = entry->next;
            continue;
        }
        model->Render(view, 0);
        entry = entry->next;
    }
}

void WorldNPCManager::Update(float dt)
{
    for (ListEntry<ImpostorModel*>* entry = mWorldNPCs.m_Head;
        entry != 0; entry = entry->next)
    {
        ImpostorModel* model = entry->entry;
        model->mAnimController->Update(dt);
        model->EvaluatePose();
    }
}

void WorldNPC::Initialize(WorldObjectLoadContext*)
{
    WorldNPCManager* manager = gpWorldNPCManager;
    manager->RegisterObject(this);
}

void WorldNPC::ReleaseResources()
{
}

template void ListContainerBase<WorldNPC*,
    NewAdapter<ListEntry<WorldNPC*> > >::DeleteEntry(ListEntry<WorldNPC*>*);

inline void WorldNPCModelList::DeleteModelEntry(
    ListEntry<ImpostorModel*>* entry)
{
    delete entry->entry;
    ListContainerBase<ImpostorModel*,
        NewAdapter<ListEntry<ImpostorModel*> > >::DeleteEntry(entry);
}

WorldNPC::~WorldNPC()
{
}

nlMatrix4* WorldNPC::GetWorldMatrix()
{
    return &mTransform;
}

void WorldNPC::SetWorldMatrix(const nlMatrix4& transform)
{
    mTransform = transform;
}
