#include "unclassified/tu_80262B54.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/NetworkSession.h"
#include "Game/Render/Presentation.h"
#include "Game/tu_801360A4.h"
#include "NL/nlBind.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80252180.h"
#include "unclassified/tu_8025D170.h"
#include "unclassified/tu_80260990.h"

extern TLComponentInstance* lbl_80578450[4];
extern TLComponentInstance lbl_80580030;
extern TLTextInstance UnidentifiedFallbackTextInstance;
extern unsigned long lbl_806E18B0;
extern u8 lbl_806E1009;
extern "C" void fn_801CBCA0(unsigned long hash, const char* name, void* owner, bool value);
extern "C" Presentation* fn_801FEEAC();

UnidentifiedScene_8025AF0C::UnidentifiedScene_8025AF0C()
    : mUnidentified1C(0)
    , mUnidentified20(0.0f)
    , mUnidentifiedFC(false)
    , mUnidentifiedFD(false)
    , mUnidentified100(0.0f)
    , mUnidentified104(0)
{
}

UnidentifiedScene_8025AF0C::~UnidentifiedScene_8025AF0C()
{
}

void UnidentifiedScene_8025AF0C::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* title = FEFinder<TLTextInstance, 3>::Find(mPresentation->GetActiveSlide(),
        InlineHasher("Layer"), InlineHasher("INVITATION"), InlineHasher("TITLE"));
    if (title == 0)
        title = &UnidentifiedFallbackTextInstance;
    title->SetStringId("ONLINE_INVITATION_TITLE");
    mUnidentified108 = FEFinder<TLComponentInstance, 4>::Find(presentation->GetActiveSlide(),
        InlineHasher("Layer"), InlineHasher("INVITATION"), InlineHasher("LOGIN"));
    if (mUnidentified108 == 0)
        mUnidentified108 = &lbl_80580030;
    switch (mUnidentified1C)
    {
    case 1:
        mUnidentified108->SetActiveSlide("ENTERING LOBBY", false, false);
        break;
    case 2:
    {
        mUnidentified108->SetActiveSlide("DECLINED", false, false);
        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(mUnidentified108->GetActiveSlide(), InlineHasher("INVITE"));
        if (text == 0)
            text = &UnidentifiedFallbackTextInstance;
        text->SetStringId("LOC_ONLINE_CANCELED_INVITATION");
        break;
    }
    case 3:
        mUnidentified108->SetActiveSlide("DECLINED", false, false);
        break;
    default:
        mUnidentified108->SetActiveSlide("CONNECTING", false, false);
        break;
    }
    TU80252180Scene* scene = fn_80253E18();
    bool canCancel = false;
    if (mUnidentified1C == 1)
    {
        NetworkLobby_80133634* lobby = g_pNetworkSession->fn_801216F0();
        canCancel = lobby != 0 && lobby->CanCancelMatchmaking();
    }
    mUnidentifiedFC = canCancel;
    if (mUnidentifiedFC)
    {
        mUnidentified24.fn_801E6CB4(lbl_806E1194->mUnidentified00C);
        fn_802534BC(scene, 4, true);
    }
    else
        fn_802534BC(scene, 0, true);
    mUnidentified24.fn_8022F194(fn_80253D70(scene, 4));
    if (mUnidentifiedFC)
        mUnidentified24.fn_80260748();
    else
        mUnidentified24.fn_80206B54();
    for (int i = 0; i < 4; ++i)
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
}

void UnidentifiedScene_8025AF0C::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mUnidentifiedFD && !g_pFEInput->HasInputLock(this))
        return;
    mUnidentified100 += fDeltaT;
    NetworkLobby_80133634* lobby = g_pNetworkSession->fn_801216F0();
    switch (mUnidentified1C)
    {
    case 1:
        if (lobby->mMatchFailed || lobby->mCancelRequested)
        {
            lobby->CloseConnectionsAndReset();
            mUnidentified1C = 2;
            mUnidentified20 = 2.0f;
            mUnidentified100 = 0.0f;
            mUnidentified108->SetActiveSlide("DECLINED", false, false);
            TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(mUnidentified108->GetActiveSlide(), InlineHasher("INVITE"));
            if (text == 0)
                text = &UnidentifiedFallbackTextInstance;
            text->SetStringId("LOC_ONLINE_CANCELED_INVITATION");
        }
        else if (lbl_806E1009 && lobby->AreAllConnectionsReady())
        {
            SHOnlineInvitePlayers* scene = (SHOnlineInvitePlayers*)lbl_806E1838->Push((SceneList)0x2C, SCREEN_NOTHING, true);
            scene->mUnidentified01C = false;
            scene->mUnidentified01D = true;
        }
        break;
    case 3:
        if (!lbl_806E1194->ValidateHostInvitation_80136BC0() || mUnidentified100 >= 30.0f)
        {
            lbl_806E1194->SetOwnStatusInitial_80136FA4(1);
            lbl_806E1194->SetOwnStatusAvailable_801374A4();
            lbl_806E1838->Push((SceneList)lbl_806E1194->mUnidentified00C, SCREEN_BACK, true);
            fn_801CBCA0(0x37A9934D, 0, 0, true);
        }
        break;
    }
    if (mUnidentified104 == 0)
    {
        if (mUnidentifiedFC)
        {
            for (int i = 0; i < 4; ++i)
            {
                if ((unsigned int)i == lbl_806E18B0)
                    lbl_80578450[i]->SetActiveSlide("cursor", true, false);
                else
                    lbl_80578450[i]->SetActiveSlide("waiting", true, false);
            }
        }
        mUnidentified104 = 1;
    }
    if (mUnidentifiedFC)
    {
        bool canCancel = false;
        if (mUnidentified1C == 1)
        {
            NetworkLobby_80133634* lobby = g_pNetworkSession->fn_801216F0();
            canCancel = lobby != 0 && lobby->CanCancelMatchmaking();
        }
        if (!canCancel)
        {
            fn_802534BC(fn_80253E18(), 0, true);
            mUnidentified24.fn_80206B54();
            mUnidentifiedFC = false;
        }
    }
    else
    {
        bool canCancel = false;
        if (mUnidentified1C == 1)
        {
            NetworkLobby_80133634* lobby = g_pNetworkSession->fn_801216F0();
            canCancel = lobby != 0 && lobby->CanCancelMatchmaking();
        }
        if (canCancel)
        {
            fn_802534BC(fn_80253E18(), 4, true);
            mUnidentified24.fn_80260748();
            mUnidentifiedFC = true;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        u8 valid = true;
        TU80300104Event event;
        event.mIndex = i;
        event.mPosition = fn_802197FC(i, &valid);
        event.mFlag0 = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);
        event.mFlag1 = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 0x1E, true, 0);
        if (mUnidentified24.fn_8022F2E0(event, fDeltaT))
        {
            lbl_806E1194->SetOwnStatusAvailable_801374A4();
            NetworkLobby_80133634* lobby = g_pNetworkSession->fn_801216F0();
            if (lobby != 0 && lobby->CanCancelMatchmaking())
                lobby->CancelMatchmaking();
            return;
        }
    }
    if (mUnidentified20 != 0.0f && mUnidentified100 >= mUnidentified20)
    {
        if (g_pNetworkSession->fn_80120368())
        {
            int popup = fn_80261770(g_pNetworkSession->mDWCErrorCode, true, 0x5B);
            if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)0xA)
            {
                FEPopupMenu* menu = (FEPopupMenu*)lbl_806E1838->Push((SceneList)0xA, SCREEN_NOTHING, false);
                menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&UnidentifiedScene_8025AF0C::fn_8026388C), this)));
                mUnidentifiedFD = true;
            }
        }
        else
        {
            lbl_806E1194->SetOwnStatusAvailable_801374A4();
            lbl_806E1838->Push((SceneList)lbl_806E1194->mUnidentified00C, SCREEN_BACK, true);
            fn_801CBCA0(0x37A9934D, 0, 0, true);
        }
    }
}

void UnidentifiedScene_8025AF0C::fn_8026388C()
{
    mUnidentifiedFD = false;
    lbl_806E1838->Pop();
    fn_801CBCA0(0x4430B152, 0, 0, true);
    fn_801FEEAC()->Call("TransitionOnlineMatchToMainMenu");
}
