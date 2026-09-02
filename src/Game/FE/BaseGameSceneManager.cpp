#include "Game/BaseGameSceneManager.h"

#include "Game/FE/feResourceManager.h"
#include "Game/FE/feSceneManager.h"
#include "Game/ResourceInterface_802CC094.h"
#include "Game/SH/SHLoading.h"
#include "NL/nlLocalization.h"

extern "C"
{
    ResourceInterface_802CC094* fn_802CBFD8(
        const void* configuration, int count, const char* name);
    void fn_802CC02C(ResourceInterface_802CC094* resourceInterface);
}

FEMiniBundle* lbl_806E1828;
ResourceInterface_802CC094* lbl_806E182C;
unsigned long lbl_806E1830;

struct ResourceConfiguration_801C4B7C
{
    unsigned long mUnidentified00;
    unsigned long mUnidentified04;
    unsigned long mUnidentified08;
    unsigned long mUnidentified0C;
};

extern const ResourceConfiguration_801C4B7C lbl_804E7320
    = { 0, 0x5000, 4, 0x2CCCCC };
extern const ResourceConfiguration_801C4B7C lbl_804E7330
    = { 0, 0x5000, 4, 0x200000 };

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

void BaseGameSceneManager::fn_801C5FB8(SceneList scene)
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

extern "C" ResourceInterface_802CC094* fn_801C4D4C()
{
    return lbl_806E182C;
}

extern "C" bool fn_801C4D40()
{
    return FEResourceManager::Instance()->fn_802FDF3C(lbl_806E1828);
}

extern "C" void fn_801C4D14(const char* bundleFileName)
{
    lbl_806E1828
        = FEResourceManager::Instance()->fn_802FDD8C(bundleFileName);
}

extern "C" void fn_801C4CBC()
{
    if (lbl_806E182C != 0)
    {
        lbl_806E182C->ReleaseResource(lbl_806E1830);
        FEResourceManager::Instance()->fn_802FD26C(0);
        fn_802CC02C(lbl_806E182C);
        lbl_806E182C = 0;
    }
}

extern "C" void fn_801C4C44()
{
    ResourceConfiguration_801C4B7C configuration
        = { 0, 0xC800, 4, 0xA00000 };
    lbl_806E182C
        = fn_802CBFD8(&configuration, 2, "FEResourceManagerPool");
    lbl_806E1830 = lbl_806E182C->MarkResource();
    FEResourceManager::Instance()->fn_802FD26C(lbl_806E182C);
}

extern "C" void fn_801C4B7C()
{
    if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        ResourceConfiguration_801C4B7C configuration = lbl_804E7320;
        lbl_806E182C
            = fn_802CBFD8(&configuration, 2, "FEResourceManagerPool");
    }
    else
    {
        ResourceConfiguration_801C4B7C configuration = lbl_804E7330;
        lbl_806E182C
            = fn_802CBFD8(&configuration, 2, "FEResourceManagerPool");
    }

    lbl_806E1830 = lbl_806E182C->MarkResource();
    FEResourceManager::Instance()->fn_802FD26C(lbl_806E182C);
}
