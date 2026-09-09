#ifndef GAME_BASE_GAME_SCENE_MANAGER_H
#define GAME_BASE_GAME_SCENE_MANAGER_H

#include "Game/BaseSceneHandler.h"

enum SceneList
{
    SCENE_INVALID = -2,
    SCENE_TITLE = 0,
    SCENE_MAIN_MENU = 1,
    SCENE_CHOOSE_CAPTAINS_DOMINATION = 2,
    SCENE_CHOOSE_SIDEKICKS_DOMINATION = 3,
    SCENE_CHOOSE_CAPTAINS_STRIKER_CUP = 6,
    SCENE_CHOOSE_SIDEKICKS_STRIKER_CUP = 7,
    SCENE_SUPER_LOADING = 17,
    SCENE_BOOT_LOADING = 18,
    SCENE_BOOT_LOADING_JPN = 19,
    SCENE_ASYNC_LOADING = 25,
    SCENE_WIDESCREEN_LOADING = 26,
    SCENE_ONLINE_GUEST_CONTROLLER_SELECT = 43,
    SCENE_ONLINE_INVITE_PREVIEW = 45,
    SCENE_ONLINE_FRIEND_CODE_ENTRY = 48,
    SCENE_ONLINE_LOGIN = 51,
    SCENE_ONLINE_INVITE_RESPONSE = 52,
    SCENE_ONLINE_INVITE_STATUS = 53,
    SCENE_ONLINE_MII_SELECT = 54,
    SCENE_ONLINE_MII_SELECT_OVERLAY = 55,
    OVERLAY_HUD = 89,
};

enum ScreenMovement
{
    SCREEN_NOTHING = 0,
    SCREEN_FORWARD = 1,
    SCREEN_BACK = 2,
};

struct SceneEntry
{
    SceneList mSceneID;
    const char* mFenFileName;
}; // size 0x8

extern SceneEntry SceneEntryTable[];

class BaseGameSceneManager
{
public:
    BaseGameSceneManager();
    virtual ~BaseGameSceneManager();
    virtual BaseSceneHandler* Push(SceneList newscene, ScreenMovement movement, bool popfirst);
    BaseSceneHandler* GetScene(SceneList scene);
    BaseSceneHandler* GetCurrentScene() const
    {
        return mCurrentStackDepth != 0 ? mBaseSceneHandlerStack[mCurrentStackDepth - 1] : 0;
    }
    virtual void Pop();
    void PopEntireStack();
    void PopToScene(SceneList scene);
    SceneList GetSceneType(BaseSceneHandler* scene);
    bool IsOnStack(SceneList scene);
    const char* GetFileName(SceneList scene);
    void PushLoadingScene(bool popfirst);

    static const u32 MAX_SCENE_DEPTH = 32;

    /* 0x04 */ unsigned long mCurrentStackDepth;
    /* 0x08 */ SceneList m_sceneStack[MAX_SCENE_DEPTH];
    /* 0x88 */ BaseSceneHandler* mBaseSceneHandlerStack[MAX_SCENE_DEPTH];
}; // size 0x108


BaseGameSceneManager* GetOverlayManager();
extern BaseGameSceneManager* g_pOverlayManager;

class GLResourcePool;

void CreateFEResourcePool();
void CreateLargeFEResourcePool();
void DestroyFEResourcePool();
void LoadFEMiniBundle(const char* bundleFileName);
bool UnloadFEMiniBundle();
GLResourcePool* GetFEResourcePool();

#endif // GAME_BASE_GAME_SCENE_MANAGER_H
