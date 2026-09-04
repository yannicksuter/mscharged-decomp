#include "Game/FE/feSceneManager.h"

#include "Game/FE/feInput.h"
#include "Game/FE/feRender.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"

#include "NL/MemAlloc.h"
#include "NL/nlDebug.h"
#include "NL/nlDLRing.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

extern nlDLListSlotPool<PackagePushPopMessage*> m_pushPopMessageQueue;
extern int nlPrintf(const char* format, ...);

extern "C" void fn_802FC280();
extern "C" void fn_802FEA20(FEScene* scene);

FESceneManager::FESceneManager()
    : m_sceneHandlerStack(0x14, 0)
    , m_topMostScene(0)
    , m_uDefaultRenderView(0)
{
    FERender::Initialize();
}

bool FESceneManager::AreAllScenesValid()
{
    nlDLListIterator<BaseSceneHandler*> sceneIterator = m_sceneHandlerStack.Begin();
    DLListEntry<BaseSceneHandler*>* headEntry = sceneIterator.m_Head;
    DLListEntry<BaseSceneHandler*>* currentEntry = sceneIterator.m_Curr;

    while (currentEntry != 0)
    {
        if (currentEntry->entry->mFEScene->mState != 6)
        {
            return false;
        }

        if (nlDLRingIsEnd(headEntry, currentEntry) || currentEntry == 0)
        {
            currentEntry = 0;
        }
        else
        {
            currentEntry = currentEntry->m_next;
        }
    }

    return m_pushPopMessageQueue.m_Head == 0;
}

bool FESceneManager::IsObjectQueuedForPop(BaseSceneHandler* pSceneHandler)
{
    nlDLListIterator<PackagePushPopMessage*> msgIterator = m_pushPopMessageQueue.Begin();

    while (msgIterator.hasNext())
    {
        PackagePushPopMessage* pMsg = *msgIterator;
        if (pMsg->m_pSceneHandler == pSceneHandler && pMsg->m_bPush == false)
        {
            return true;
        }
        msgIterator.next();
    }

    return false;
}

void FESceneManager::ForceImmediateStackProcessing()
{
    ProcessPushPopQueue();
}

BaseSceneHandler* FESceneManager::GetSceneHandler(unsigned long hashID)
{
    nlDLListIterator<BaseSceneHandler*> sceneIterator = m_sceneHandlerStack.Begin();
    DLListEntry<BaseSceneHandler*>* headEntry = sceneIterator.m_Head;
    DLListEntry<BaseSceneHandler*>* currentEntry = sceneIterator.m_Curr;

    while (currentEntry != 0)
    {
        if (hashID == currentEntry->entry->mHashID)
        {
            return currentEntry->entry;
        }

        if (nlDLRingIsEnd(headEntry, currentEntry) || currentEntry == 0)
        {
            currentEntry = 0;
        }
        else
        {
            currentEntry = currentEntry->m_next;
        }
    }

    return 0;
}

void FESceneManager::ProcessPushPopQueue()
{
    PackagePushPopMessage* pPackagePushPopMessage;

    while (m_pushPopMessageQueue.m_Head != 0)
    {
        m_pushPopMessageQueue.RemoveStart(&pPackagePushPopMessage);

        if (pPackagePushPopMessage->m_bPush != false)
        {
            m_sceneHandlerStack.AddStart(pPackagePushPopMessage->m_pSceneHandler);

            FESceneManager* pSceneManager = FESceneManager::Instance();
            MemoryAllocator* pAllocator = pPackagePushPopMessage->m_pAllocator;
            const char* szFilename = pPackagePushPopMessage->m_szFilename;
            BaseSceneHandler* pSceneHandler = pPackagePushPopMessage->m_pSceneHandler;
            FEScene* pFEScene = new (nlMalloc(sizeof(FEScene), 8, false)) FEScene();
            pFEScene->m_uHashID = nlStringLowerHash(szFilename);
            pFEScene->m_uRenderView = pSceneManager->m_uDefaultRenderView;
            pSceneHandler->mFEScene = pFEScene;
            pFEScene->m_pAllocator = pAllocator;

            if (!pFEScene->LoadPackage(szFilename, pAllocator))
            {
                nlPrintf("Error: failed to load package!\n");
                nlBreak();
            }
        }
        else
        {
            nlDLListIterator<BaseSceneHandler*> sceneIterator = m_sceneHandlerStack.Begin();
            DLListEntry<BaseSceneHandler*>* headEntry = sceneIterator.m_Head;
            DLListEntry<BaseSceneHandler*>* sceneEntry = sceneIterator.m_Curr;

            while (sceneEntry != 0)
            {
                if (sceneEntry->entry == pPackagePushPopMessage->m_pSceneHandler)
                {
                    nlDLRingIsEnd(headEntry, sceneEntry);
                    nlDLRingRemove(&m_sceneHandlerStack.m_Head, sceneEntry);
                    m_sceneHandlerStack.m_Allocator.Free(sceneEntry);
                    break;
                }

                if (nlDLRingIsEnd(headEntry, sceneEntry) || sceneEntry == 0)
                {
                    sceneEntry = 0;
                }
                else
                {
                    sceneEntry = sceneEntry->m_next;
                }
            }

            fn_802FEA20(pPackagePushPopMessage->m_pSceneHandler->mFEScene);
            pPackagePushPopMessage->m_pSceneHandler->mFEScene->UnloadPackage();

            FEScene* pFEScene = pPackagePushPopMessage->m_pSceneHandler->mFEScene;
            delete pPackagePushPopMessage->m_pSceneHandler;
            delete pFEScene;
        }

        PackagePushPopMessage::m_PushPopMessageSlotPool.Delete(pPackagePushPopMessage);
    }
}

void FESceneManager::QueueScenePush(
    BaseSceneHandler* pSceneHandler,
    const char* szFilename,
    MemoryAllocator* pAllocator)
{
    PackagePushPopMessage* msg = 0;

    PackagePushPopMessage::m_PushPopMessageSlotPool.Allocate(msg);

    msg->m_bPush = true;
    msg->m_pSceneHandler = pSceneHandler;
    nlStrNCpy<char>(msg->m_szFilename, szFilename, 0x40);
    msg->m_pSceneHandler->mHashID = nlStringLowerHash(szFilename);
    msg->m_pAllocator = pAllocator != 0 ? pAllocator : CurrentAllocator;

    m_pushPopMessageQueue.AddEnd(msg);
}

static inline void FindSceneForPop(
    PackagePushPopMessage* msg,
    DLListEntry<BaseSceneHandler*>* headEntry,
    DLListEntry<BaseSceneHandler*>* sceneEntry)
{
    while (sceneEntry != 0)
    {
        BaseSceneHandler* pSceneHandler = sceneEntry->entry;

        if (!FESceneManager::IsObjectQueuedForPop(pSceneHandler))
        {
            msg->m_pSceneHandler = sceneEntry->entry;
            break;
        }

        if (nlDLRingIsEnd(headEntry, sceneEntry) || sceneEntry == 0)
        {
            sceneEntry = 0;
        }
        else
        {
            sceneEntry = sceneEntry->m_next;
        }
    }
}

void FESceneManager::QueueScenePop()
{
    PackagePushPopMessage* msg = 0;

    PackagePushPopMessage::m_PushPopMessageSlotPool.Allocate(msg);

    msg->m_szFilename[0] = 0;
    msg->m_pSceneHandler = 0;
    msg->m_bPush = false;

    nlDLListIterator<BaseSceneHandler*> sceneIterator = m_sceneHandlerStack.Begin();
    FindSceneForPop(msg, sceneIterator.m_Head, sceneIterator.m_Curr);

    m_pushPopMessageQueue.AddEnd(msg);
}

void FESceneManager::RenderActiveScenes()
{
    fn_802FC280();

    if (m_topMostScene != 0)
    {
        if (!IsObjectQueuedForPop(m_topMostScene))
        {
            FEScene* scene = m_topMostScene->mFEScene;
            if (scene->mState == 6 && m_topMostScene->mVisible)
            {
                FERender::RenderScene(scene);
            }
        }
    }

    nlDLListIterator<BaseSceneHandler*> sceneIterator = m_sceneHandlerStack.Begin();

    while (sceneIterator.hasNext())
    {
        BaseSceneHandler* pSceneHandler = *sceneIterator;

        if (pSceneHandler != m_topMostScene)
        {
            if (!IsObjectQueuedForPop(pSceneHandler))
            {
                FEScene* scene = pSceneHandler->mFEScene;
                if (scene->mState == 6 && pSceneHandler->mVisible)
                {
                    FERender::RenderScene(scene);
                }
            }
        }

        sceneIterator.next();
    }
}

extern "C" void fn_802FF644(FESceneManager* pSceneManager, FEScene* pFEScene)
{
    BaseSceneHandler* pSceneHandler = pSceneManager->GetSceneHandler(pFEScene->m_uHashID);
    pSceneHandler->SetPresentation(pFEScene->m_pFEPackage->GetPresentation());
    pSceneHandler->SceneCreated();
    pSceneHandler->InitializeSubHandlers();
}

void FESceneManager::Update(float dt)
{
    ProcessPushPopQueue();

    nlDLListIterator<BaseSceneHandler*> sceneIterator = m_sceneHandlerStack.Begin();
    DLListEntry<BaseSceneHandler*>* headEntry = sceneIterator.m_Head;
    DLListEntry<BaseSceneHandler*>* currentEntry = sceneIterator.m_Curr;

    while (currentEntry != 0)
    {
        if (((FEScene*)currentEntry->entry->mFEScene)->mState == 6)
        {
            g_pFEInput->EnableInputIfSceneHasFocus(currentEntry->entry);
            currentEntry->entry->Update(dt);
        }

        if (nlDLRingIsEnd(headEntry, currentEntry) || currentEntry == 0)
        {
            currentEntry = 0;
        }
        else
        {
            currentEntry = currentEntry->m_next;
        }
    }
}

void FESceneManager::SetTopMostScene(BaseSceneHandler* pSceneHandler)
{
    if (m_topMostScene != 0)
    {
        m_topMostScene = 0;
    }
    m_topMostScene = pSceneHandler;
}

void FESceneManager::ClearTopMostScene()
{
    m_topMostScene = 0;
}
