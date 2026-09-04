#include "unclassified/tu_80332770.h"
#include "unclassified/tu_80332DC0.h"
#include "unclassified/tu_80336B2C.h"

#include "Game/NetworkSession.h"
#include "NL/nlMemory.h"
#include "NL/nlTicker.h"

UnidentifiedNetworkManager* lbl_806E2138;

static NetworkMessageFactory<NetworkMessageType0_80533B7C> sFactoryType0;
static NetworkMessageFactory<NetworkMessageType1_80533B68> sFactoryType1;
static NetworkMessageFactory<NetworkMessageType8_80533BA4> sFactoryType8;
static NetworkMessageFactory<NetworkMessageType9_80533B90> sFactoryType9;

extern void* lbl_806E2100;

extern "C" void fn_8032CA40(
    void* registry, int type, UnidentifiedNetworkMessageFactory* factory);
extern "C" void fn_8004F594(int category, const char* format, ...);

extern "C" bool fn_80332770()
{
    UnidentifiedNetworkOnlineInterface& online = *lbl_806E20D8;
    int mode = online.OnlineVirtual0C();
    if (mode == 1 || mode == 2)
    {
        return true;
    }

    if (lbl_806E2164->mRecording != 0
        || lbl_806E2164->mPlaybackReady != 0)
    {
        return true;
    }
    return false;
}

extern "C" void fn_803327DC()
{
    UnidentifiedNetworkManager* manager
        = (UnidentifiedNetworkManager*)nlMalloc(
            sizeof(UnidentifiedNetworkManager), 8, false);
    if (manager != 0)
    {
        manager->mFrameProvider = 0;
        manager->mEnabled = false;

        fn_8032CA40(lbl_806E2100, 0, &sFactoryType0);
        fn_8032CA40(lbl_806E2100, 1, &sFactoryType1);
        fn_8032CA40(lbl_806E2100, 8, &sFactoryType8);
        fn_8032CA40(lbl_806E2100, 9, &sFactoryType9);

        manager->mUnidentified08 = 0;
        manager->mUnidentified0C = 0.0f;
        manager->mUnidentified10 = 0;
        manager->mUnidentified14 = 0.0f;
        manager->mUnidentified18 = false;
    }
    lbl_806E2138 = manager;
}

void UnidentifiedNetworkManager::fn_8033288C()
{
    mUnidentified08 = 0;
    mUnidentified0C = 0.0f;
    mUnidentified10 = 0;
    mUnidentified14 = 0.0f;
    mUnidentified18 = false;
}

void UnidentifiedNetworkManager::fn_803328AC(
    UnidentifiedFixedUpdateTaskBase* frameProvider)
{
    mFrameProvider = frameProvider;
}

int UnidentifiedNetworkManager::fn_803328B4()
{
    UnidentifiedInputRouter* router = fn_803330AC();
    int inputCount = router->RouterVirtual10();
    return inputCount + fn_80338340(lbl_806E2164);
}

void UnidentifiedNetworkManager::fn_803328FC()
{
    if (lbl_806E2164->mPlaybackReady != 0)
    {
        return;
    }

    UnidentifiedInputRouter* router = fn_803330AC();
    if (!router->mOutgoingQueueOverflowed)
    {
        if (!router->RouterVirtual18())
        {
            return;
        }

        UnidentifiedNetworkPeer* peer
            = fn_80338C0C(lbl_806E20D8);
        int numControllers = peer->mUnidentified004;
        for (int i = 0; i < numControllers; ++i)
        {
            fn_80336DF4(fn_80336B6C(peer, i));
        }
        router->RouterVirtual1C();
    }
    else if (!mUnidentified18)
    {
        fn_8004F594(0x10, "Overflowed Queues just occured\n");
        mFrameProvider->UnidentifiedVirtual14();
        mUnidentified18 = true;
    }
}

bool UnidentifiedNetworkManager::fn_80332A00()
{
    UnidentifiedInputRouter* router = fn_803330AC();
    if (lbl_806E2164->mPlaybackReady != 0)
    {
        if (router->RouterVirtual34() != 0)
        {
            return true;
        }
    }
    else if (!router->mOutgoingQueueOverflowed)
    {
        router->RouterVirtual24();
        if (router->RouterVirtual14())
        {
            router->mStarvedForInput = false;
            if (mUnidentified08 != 0)
            {
                mUnidentified0C
                    = nlGetTickerDifference(mUnidentified08, nlGetTicker());
                mUnidentified08 = 0;
                ++mUnidentified10;
                fn_8004F594(0x10,
                    "Congested for %dth time for %f ms at game frame %d\n",
                    mUnidentified10, mUnidentified0C,
                    mFrameProvider->GetFrame());
                mUnidentified14 += mUnidentified0C;
            }
            router->RouterVirtual20();
            return true;
        }

        router->mStarvedForInput = true;
        mUnidentified08 = nlGetTicker();
    }
    else if (!mUnidentified18)
    {
        fn_8004F594(0x10, "Overflowed Queues just occured\n");
        mFrameProvider->UnidentifiedVirtual14();
        mUnidentified18 = true;
    }
    return false;
}
