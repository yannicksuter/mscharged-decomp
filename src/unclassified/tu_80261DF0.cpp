#include "unclassified/tu_80261DF0.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/Sys/debug.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/NetworkSession.h"
#include "Game/tu_801360A4.h"
#include "NL/nlBind.h"
#include "NL/nlBasicString.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80252180.h"
#include "unclassified/tu_80254084.h"
#include "unclassified/tu_80259B88.h"

extern TLComponentInstance* lbl_80578450[4];
extern TLComponentInstance lbl_80580030;
extern unsigned long lbl_806E18B0;
extern int lbl_806E20E0;
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_8025BD7C(bool value);
extern "C" void fn_8025BD94(bool value);
extern "C" bool fn_8025BDA0();
extern "C" const unsigned short* fn_801CA950(nlLocalization*, const char*);

TU80261DF0Scene::~TU80261DF0Scene()
{
}

void TU80261DF0Scene::fn_802619A8()
{
    TU80300104Base::Callback over(Bind<void>(MemFun(&TU80261DF0Scene::fn_80262614), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback off(Bind<void>(MemFun(&TU80261DF0Scene::fn_802626D8), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback select(Bind<void>(MemFun(&TU80261DF0Scene::fn_80262784), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified1C[i].fn_80300D74(mUnidentified2EC[i], true, 0.0f, 0.0f, 1.0f, 0.5f);
        mUnidentified1C[i].fn_803007C0(over);
        mUnidentified1C[i].fn_80300864(off);
        mUnidentified1C[i].fn_803009AC(select);
    }
}

extern "C" void fn_80261CE0()
{
    int index = lbl_806E1194->mHostInvitationIndex;
    UnidentifiedFriendStatusPayload* payload = lbl_806E1194->GetFriendStatusPayload_80136A88(index);
    GameSettings* settings = &GameInfoManager::Instance()->mNoCheatSettings;
    settings->unknown_0x00 = payload->mGameplaySettings.SkillLevel;
    settings->unknown_0x04 = payload->mGameplaySettings.m_unk04;
    settings->unknown_0x08 = payload->mGameplaySettings.GameTime;
    settings->unknown_0x0C = payload->mGameplaySettings.m_unk0C;
    settings->unknown_0x10 = payload->mGameplaySettings.m_unk10;
    settings->unknown_0x14 = payload->mGameplaySettings.PowerUps;
    settings->unknown_0x15 = payload->mGameplaySettings.Shoot2Score;
    settings->unknown_0x16 = payload->mGameplaySettings.BowserAttackEnabled;
    settings->unknown_0x17 = payload->mGameplaySettings.RumbleEnabled;
    settings->unknown_0x18 = payload->mGameplaySettings.m_unk18;
    settings->unknown_0x19 = payload->mGameplaySettings.m_unk19;
    settings->unknown_0x1A = payload->mGameplaySettings.m_unk1A;
    GameRules* rules = &GameInfoManager::Instance()->mRulesA;
    rules->unknown_0x0 = payload->mPowerupSettings.m_unk0;
    rules->unknown_0x4 = payload->mPowerupSettings.m_unk4;
    rules->unknown_0x8 = payload->mPowerupSettings.m_unk8;
    fn_80257D0C(fn_8025BDA0(), 0, false);
    g_pNetworkSession->fn_801216F0()->ConnectToFriendServer(index);
    lbl_806E1838->Push((SceneList)0x35, SCREEN_FORWARD, true);
    UnidentifiedScene_8025AF0C* scene = (UnidentifiedScene_8025AF0C*)lbl_806E1838->GetScene((SceneList)0x35);
    scene->mUnidentified1C = 1;
    scene->mUnidentified20 = 0.0f;
}

TU80261DF0Scene::TU80261DF0Scene()
    : mUnidentified40C(0)
    , mUnidentified410(0)
    , mUnidentified414(0)
{
    for (int i = 0; i < 4; ++i)
        mUnidentified1C[i].mContext = (void*)i;
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
        mUnidentified3FC[i] = 0;
    }
}

void TU80261DF0Scene::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    for (int i = 0; i < 4; ++i)
    {
        char name[6];
        nlSNPrintf(name, sizeof(name), "BTN_%d", i + 1);
        mUnidentified2EC[i] = FEFinder<TLComponentInstance, 4>::Find(presentation->GetActiveSlide(),
            InlineHasher("Layer"), InlineHasher("INVITATION"), InlineHasher(name));
        if (mUnidentified2EC[i] == 0)
            mUnidentified2EC[i] = &lbl_80580030;
    }
    int index = lbl_806E1194->mHostInvitationIndex;
    const unsigned short* name = (const unsigned short*)GameInfoManager::Instance()->GetUnknown0x340(lbl_806E20E0, index);
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;
    WideBasicString string = Format(WideBasicString(fn_801CA950(g_pLocalization, "ONLINE_INVITATION_INVITATION")), name);
    nlStrNCpy(mUnidentified2FC, string.c_str(), 0x80);
    FEFinder<TLTextInstance, 3>::Find(presentation->GetActiveSlide(), InlineHasher("Layer"),
        InlineHasher("INVITATION"), InlineHasher("INVITE"))->SetString(mUnidentified2FC);
    lbl_806E1194->SetOwnStatusReceivedInvitation_80137118(index);
    TU80252180Scene* scene = fn_80253E18();
    if (scene != 0)
        fn_802534BC(scene, 0, true);
    fn_8025BD7C(false);
    fn_801CBCA0(0xBB142B94, 0, 0, 1);
}

void TU80261DF0Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mUnidentified414 == 0 || mUnidentified414 == 2 || mUnidentified414 == 3)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->m_time < slide->m_start + slide->m_duration)
        {
            for (int i = 0; i < 4; ++i)
                lbl_80578450[i]->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mUnidentified414 == 0)
        {
            fn_802619A8();
            mUnidentified410 = 1;
            mUnidentified414 = 1;
        }
        else if (mUnidentified414 == 2)
        {
            fn_80262844();
            return;
        }
    }
    if (!lbl_806E1194->ValidateHostInvitation_80136BC0())
    {
        for (int i = 0; i < 4; ++i)
            lbl_80578450[i]->SetActiveSlide("waiting", true, false);
        lbl_806E1838->Push((SceneList)0x35, SCREEN_FORWARD, true);
        UnidentifiedScene_8025AF0C* scene = (UnidentifiedScene_8025AF0C*)lbl_806E1838->GetScene((SceneList)0x35);
        scene->mUnidentified1C = 2;
        scene->mUnidentified20 = 2.0f;
        fn_801CBCA0(0xBB142B94, 0, 0, 1);
        return;
    }
    for (int pad = 0; pad < 4; ++pad)
    {
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if ((unsigned int)pad != lbl_806E18B0)
            {
                lbl_80578450[pad]->SetActiveSlide("waiting", true, false);
                continue;
            }
            lbl_80578450[pad]->SetActiveSlide("cursor", true, false);
        }
        u8 valid = true;
        TU80300104Event event;
        event.mIndex = pad;
        event.mPosition = fn_802197FC(pad, &valid);
        event.mFlag0 = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        event.mFlag1 = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);
        for (int i = 0; i < 4; ++i)
            mUnidentified1C[i].fn_80219608(&event);
    }
}

void TU80261DF0Scene::fn_80262614(int index, void* context)
{
    ++mUnidentified3FC[index];
    if (!mUnidentified1C[(int)context].fn_802192FC(1, index))
    {
        mUnidentified1C[(int)context].mValues[index] = 1;
        mUnidentified2EC[(int)context]->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xDE912775, 0, 0, 1);
    }
}

void TU80261DF0Scene::fn_802626D8(int index, void* context)
{
    --mUnidentified3FC[index];
    if (!mUnidentified1C[(int)context].fn_802192FC(1, index))
    {
        mUnidentified1C[(int)context].mValues[index] = 0;
        mUnidentified2EC[(int)context]->SetActiveSlide("off", true, false);
    }
}

void TU80261DF0Scene::fn_80262784(int, void* context)
{
    for (int i = 0; i < 4; ++i)
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    mUnidentified40C = (int)context;
    mUnidentified414 = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
    fn_801CBCA0(0xF0AFD586, 0, 0, 1);
    fn_801CBCA0(0xBB142B94, 0, 0, 1);
}

void TU80261DF0Scene::fn_80262844()
{
    switch (mUnidentified40C)
    {
    case 0:
        tDebugPrintManager::Print(DC_NETWORK, "Respond invitation Alone\n");
        fn_8025BD94(false);
        fn_80261CE0();
        break;
    case 1:
        lbl_806E1838->Push((SceneList)0x2B, SCREEN_FORWARD, true);
        ((TU80259B88Scene*)lbl_806E1838->GetScene((SceneList)0x2B))->mUnidentified1C = true;
        break;
    case 2:
    {
        lbl_806E1194->SetOwnStatusDecline_80137068(lbl_806E1194->mHostInvitationIndex);
        lbl_806E1838->Push((SceneList)0x35, SCREEN_FORWARD, true);
        UnidentifiedScene_8025AF0C* scene = (UnidentifiedScene_8025AF0C*)lbl_806E1838->GetScene((SceneList)0x35);
        scene->mUnidentified1C = 3;
        scene->mUnidentified20 = 0.0f;
        tDebugPrintManager::Print(DC_NETWORK, "Respond invitation Decline\n");
        break;
    }
    case 3:
        tDebugPrintManager::Print(DC_NETWORK, "Preview Invitation\n");
        lbl_806E1838->Push((SceneList)0x2D, SCREEN_FORWARD, true);
        break;
    }
}
