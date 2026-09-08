#include "unclassified/tu_80260990.h"
#include "Game/Sys/debug.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Render/Presentation.h"
#include "Game/tu_801360A4.h"
#include "NL/nlBind.h"
#include "NL/nlPrint.h"
#include "unclassified/tu_80252180.h"
#include <stdlib.h>

extern TLComponentInstance* lbl_80578450[4];
extern TLComponentInstance lbl_80580030;
extern int lbl_806E1900;
extern "C" void fn_801CBCA0(unsigned long hash, const char* name, void* owner, bool value);
extern "C" void fn_801CBCE4(unsigned long hash, void* owner);
extern "C" Presentation* fn_801FEEAC();
extern "C" int fn_80374308();
extern "C" int fn_80374310();
extern "C" void fn_8037433C();
extern "C" bool fn_80374398();
extern "C" int NETGetStartupErrorCode(int result);

TU80260990Scene::TU80260990Scene()
    : mUnidentified20(false)
    , mUnidentified100(0)
    , mUnidentified104(0.0f)
    , mUnidentified108(0.0f)
{
}

TU80260990Scene::~TU80260990Scene()
{
    g_pNetworkSession->mLoginListener = 0;
}

void TU80260990Scene::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    for (int i = 0; i < 4; ++i)
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    mUnidentifiedFC = FEFinder<TLComponentInstance, 4>::Find(
        presentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("INVITATION"), InlineHasher("LOGIN"));
    if (mUnidentifiedFC == 0)
        mUnidentifiedFC = &lbl_80580030;
    mUnidentifiedFC->SetActiveSlide("CONNECTING", false, false);
    TU80252180Scene* scene = fn_80253E18();
    if (scene != 0)
        fn_802534BC(scene, 0, true);
    g_pNetworkSession->mLoginListener = this;
    fn_801CBCA0(0x71D9CD2F, "FE_LOGIN", this, true);
}

void TU80260990Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    mUnidentified104 += fDeltaT;
    if (mUnidentified20 && !g_pFEInput->HasInputLock(this))
        return;
    switch (mUnidentified100)
    {
    case 0:
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (mUnidentified108 > 0.0f)
        {
            if (mUnidentified104 > mUnidentified108 + 0.3f)
            {
                fn_8037433C();
                mUnidentified100 = 1;
            }
        }
        else if (slide->m_time >= slide->m_start + slide->m_duration)
            mUnidentified108 = mUnidentified104;
        break;
    }
    case 1:
        if (fn_80374398())
        {
            if (fn_80374308())
            {
                mUnidentified100 = 3;
                g_pNetworkSession->fn_801202AC();
                mUnidentifiedFC->SetActiveSlide("LOGGING", false, false);
            }
            else
            {
                int popup = fn_80261770(NETGetStartupErrorCode(fn_80374310()), true, 0x78);
                if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)0xA)
                {
                    FEPopupMenu* menu = (FEPopupMenu*)lbl_806E1838->Push((SceneList)0xA, SCREEN_NOTHING, false);
                    menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&TU80260990Scene::fn_80261710), this)));
                    mUnidentified20 = true;
                }
                mUnidentified100 = 7;
                fn_801CBCE4(0x71D9CD2F, this);
            }
        }
        break;
    case 3:
        if (g_pNetworkSession->fn_80120338())
            mUnidentified100 = 2;
        break;
    case 2:
        if (!g_pNetworkSession->mUnidentified24A4)
        {
            int popup = fn_80261770(g_pNetworkSession->mDWCErrorCode, true, 0x74);
            if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)0xA)
            {
                FEPopupMenu* menu = (FEPopupMenu*)lbl_806E1838->Push((SceneList)0xA, SCREEN_NOTHING, false);
                menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&TU80260990Scene::fn_80261710), this)));
                mUnidentified20 = true;
            }
            mUnidentified100 = 8;
            fn_801CBCE4(0x71D9CD2F, this);
        }
        else
            mUnidentified100 = 4;
        break;
    case 6:
        lbl_806E1838->Push((SceneList)0x28, SCREEN_FORWARD, true);
        fn_801CBCE4(0x71D9CD2F, this);
        fn_801CBCA0(0x37A9934D, 0, 0, true);
        break;
    }
}

void TU80260990Scene::OnLoginResult(int result)
{
    if (result == 1)
    {
        int popup = fn_80261770(g_pNetworkSession->mDWCErrorCode, true, 0x74);
        if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)lbl_806E1838->Push((SceneList)0xA, SCREEN_NOTHING, false);
            menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&TU80260990Scene::fn_80261710), this)));
            mUnidentified20 = true;
        }
        mUnidentified100 = 8;
        fn_801CBCE4(0x71D9CD2F, this);
        return;
    }
    if (result == 2)
    {
        if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)lbl_806E1838->Push((SceneList)0xA, SCREEN_NOTHING, false);
            menu->Create((ePopupMenu)0x70, Function<FnVoidVoid>(Bind<void>(MemFun(&TU80260990Scene::fn_80261710), this)));
            mUnidentified20 = true;
        }
        mUnidentified100 = 8;
        fn_801CBCE4(0x71D9CD2F, this);
        return;
    }
    mUnidentifiedFC->SetActiveSlide("GETTING", false, false);
    if (!g_pNetworkSession->fn_80120738())
    {
        if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)lbl_806E1838->Push((SceneList)0xA, SCREEN_NOTHING, false);
            menu->Create((ePopupMenu)0x6F, Function<FnVoidVoid>(Bind<void>(MemFun(&TU80260990Scene::fn_80261710), this)));
            mUnidentified20 = true;
        }
        mUnidentified100 = 9;
        fn_801CBCE4(0x71D9CD2F, this);
        return;
    }
    lbl_806E1194->SetOwnStatusInitial_80136FA4(1);
    lbl_806E1194->SynchronizeFriends_80136F30();
    mUnidentified100 = 5;
}

void TU80260990Scene::OnStatsResult(bool success)
{
    if (!success)
    {
        g_pNetworkSession->fn_801203C0();
        int popup = fn_80261770(g_pNetworkSession->mDWCErrorCode, true, 0x6F);
        if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)lbl_806E1838->Push((SceneList)0xA, SCREEN_NOTHING, false);
            menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&TU80260990Scene::fn_80261710), this)));
            mUnidentified20 = true;
        }
        fn_801CBCE4(0x71D9CD2F, this);
        return;
    }
    mUnidentifiedFC->SetActiveSlide("SUCCESS", false, false);
    mUnidentified100 = 6;
}

void TU80260990Scene::fn_80261710()
{
    mUnidentified20 = false;
    lbl_806E1838->Pop();
    fn_801CBCA0(0x4430B152, 0, 0, true);
    fn_801FEEAC()->Call("TransitionOnlineMatchToMainMenu");
}

struct TU80260990Error
{
    int mUnidentified00;
    int mUnidentified04;
    int mUnidentified08;
};

static TU80260990Error lbl_80520358[] = {
    {20102, 20108, 0x74},
    {20109, 20109, 0x74},
    {20111, 20999, 0x74},
    {20101, 20101, 0x75},
    {23000, 23999, 0x75},
    {20110, 20110, 0x76},
    {29000, 29000, 0x77},
    {29001, 29001, 0x7C},
    {20100, 20100, 0x78},
    {50100, 50499, 0x78},
    {51000, 51099, 0x78},
    {51300, 51399, 0x78},
    {51400, 51499, 0x78},
    {52000, 52099, 0x78},
    {52100, 52199, 0x78},
    {52200, 52299, 0x78},
    {80430, 80430, 0x79},
    {24000, 24999, 0x7A},
    {25000, 25999, 0x7A},
    {31000, 31999, 0x7A},
    {54000, 54099, 0x7A},
    {60000, 99999, 0x7A},
};

extern "C" int fn_80261770(int error, bool connected, int value)
{
    error = abs(error);
    lbl_806E1900 = error;
    bool found = false;
    for (int i = 0; i < 22; ++i)
    {
        if (error >= lbl_80520358[i].mUnidentified00 && error <= lbl_80520358[i].mUnidentified04)
        {
            value = lbl_80520358[i].mUnidentified08;
            found = true;
            break;
        }
    }
    if (value == 0x7A && !connected)
        value = 0x7B;
    if (!found)
        tDebugPrintManager::Print(DC_NETWORK, "Warning failed to find standard error msg for code %d\n", error);
    if (connected)
        g_pNetworkSession->mUnidentified2494 = true;
    return value;
}
