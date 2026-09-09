#include "Game/BaseGameSceneManager.h"

#include "Game/FE/feResourceManager.h"
#include "Game/FE/feSceneManager.h"
#include "Game/SH/SHLoading.h"
#include "NL/gl/glMemory.h"
#include "NL/nlLocalization.h"

FEMiniBundle* gFEMiniBundle;
GLResourcePool* gFEResourcePool;
unsigned long gFEResourceMarker;

void BaseGameSceneManager::PushLoadingScene(bool popfirst)
{
    if (popfirst)
    {
        this->Pop();
    }

    SuperLoadingScene* scene
        = (SuperLoadingScene*)Push(SCENE_SUPER_LOADING, SCREEN_FORWARD, false);
    scene->mType = SuperLoadingScene::TT_3D_TRANSITION;
}

const char* BaseGameSceneManager::GetFileName(SceneList scene)
{
    return SceneEntryTable[scene].mFenFileName;
}

bool BaseGameSceneManager::IsOnStack(SceneList scene)
{
    for (int i = 0; i < mCurrentStackDepth; ++i)
    {
        if (m_sceneStack[i] == scene)
            return true;
    }
    return false;
}

SceneList BaseGameSceneManager::GetSceneType(BaseSceneHandler* scene)
{
    for (int i = 0; i < mCurrentStackDepth; ++i)
    {
        if (mBaseSceneHandlerStack[i] == scene)
        {
            return m_sceneStack[i];
        }
    }
    return SCENE_INVALID;
}

void BaseGameSceneManager::PopToScene(SceneList scene)
{
    while (mCurrentStackDepth != 0)
    {
        if (GetSceneType(GetCurrentScene()) == scene)
        {
            return;
        }

        Pop();
    }
}

void BaseGameSceneManager::PopEntireStack()
{
    while (mCurrentStackDepth != 0)
    {
        this->Pop();
    }
}

void BaseGameSceneManager::Pop()
{
    FESceneManager::Instance()->QueueScenePop();
    mBaseSceneHandlerStack[mCurrentStackDepth] = 0;
    mCurrentStackDepth = (mCurrentStackDepth - 1);
}

BaseSceneHandler* BaseGameSceneManager::GetScene(SceneList scene)
{
    BaseSceneHandler* returnValue = 0;

    for (int i = 0; i < mCurrentStackDepth; ++i)
    {
        if (m_sceneStack[i] == scene)
        {
            returnValue = mBaseSceneHandlerStack[i];
            break;
        }
    }

    return returnValue;
}

BaseGameSceneManager::~BaseGameSceneManager()
{
    while (mCurrentStackDepth != 0)
    {
        this->Pop();
    }
}

BaseGameSceneManager::BaseGameSceneManager()
{
    mCurrentStackDepth = 0;
    for (int i = 0; i < MAX_SCENE_DEPTH; ++i)
    {
        m_sceneStack[i] = SCENE_INVALID;
        mBaseSceneHandlerStack[i] = 0;
    }
}

GLResourcePool* GetFEResourcePool()
{
    return gFEResourcePool;
}

bool UnloadFEMiniBundle()
{
    return FEResourceManager::Instance()->UnloadMiniBundle(gFEMiniBundle);
}

void LoadFEMiniBundle(const char* bundleFileName)
{
    gFEMiniBundle
        = FEResourceManager::Instance()->LoadMiniBundle(bundleFileName);
}

void DestroyFEResourcePool()
{
    if (gFEResourcePool != 0)
    {
        gFEResourcePool->ReleaseResource(gFEResourceMarker);
        FEResourceManager::Instance()->SetResourcePool(0);
        glDestroyResourcePool(gFEResourcePool);
        gFEResourcePool = 0;
    }
}

void CreateFEResourcePool()
{
    if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        GLMemoryRequirement requirements[2] = {
            { GLM_Header, 0x5000 },
            { GLM_TextureData, 0x2CCCCC },
        };
        gFEResourcePool
            = glCreateResourcePool(requirements, 2, "FEResourceManagerPool");
    }
    else
    {
        GLMemoryRequirement requirements[2] = {
            { GLM_Header, 0x5000 },
            { GLM_TextureData, 0x200000 },
        };
        gFEResourcePool
            = glCreateResourcePool(requirements, 2, "FEResourceManagerPool");
    }

    gFEResourceMarker = gFEResourcePool->MarkResource();
    FEResourceManager::Instance()->SetResourcePool(gFEResourcePool);
}

void CreateLargeFEResourcePool()
{
    GLMemoryRequirement requirements[2] = {
        { GLM_Header, 0xC800 },
        { GLM_TextureData, 0xA00000 },
    };
    gFEResourcePool
        = glCreateResourcePool(requirements, 2, "FEResourceManagerPool");
    gFEResourceMarker = gFEResourcePool->MarkResource();
    FEResourceManager::Instance()->SetResourcePool(gFEResourcePool);
}
