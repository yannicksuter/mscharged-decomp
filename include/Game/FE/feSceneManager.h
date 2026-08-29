#ifndef GAME_FE_SCENE_MANAGER_H
#define GAME_FE_SCENE_MANAGER_H

#include "types.h"

#include "Game/BaseSceneHandler.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlSingleton.h"

class MemoryAllocator;

struct PackagePushPopMessage
{
    static SlotPool<PackagePushPopMessage> m_PushPopMessageSlotPool;

    /* 0x00 */ bool m_bPush;
    /* 0x04 */ BaseSceneHandler* m_pSceneHandler;
    /* 0x08 */ char m_szFilename[64];
    /* 0x48 */ MemoryAllocator* m_pAllocator;
}; // size 0x4C

class FESceneManager : public nlSingleton<FESceneManager>
{
public:
    FESceneManager();
    ~FESceneManager();

    bool AreAllScenesValid();
    void ForceImmediateStackProcessing();
    BaseSceneHandler* GetSceneHandler(unsigned long hashID);
    void ProcessPushPopQueue();
    void QueueScenePush(BaseSceneHandler* pSceneHandler, const char* szFilename, MemoryAllocator* pAllocator);
    void QueueScenePop();
    void RenderActiveScenes();
    void Update(float dt);
    void SetTopMostScene(BaseSceneHandler* pSceneHandler);
    void ClearTopMostScene();

    static bool IsObjectQueuedForPop(BaseSceneHandler* pSceneHandler);

    /* 0x00 */ nlDLListSlotPool<BaseSceneHandler*> m_sceneHandlerStack;
    /* 0x1C */ BaseSceneHandler* m_topMostScene;
    /* 0x20 */ unsigned long m_uDefaultRenderView;
}; // size 0x24

#endif // GAME_FE_SCENE_MANAGER_H
