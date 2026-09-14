#include "unclassified/tu_801E4630.h"
#include "NL/nlFunction.inl"
#include "NL/nlBindMember.h"
#include "Game/EventRegistry.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/feScrollText.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feMusic.h"
#include "Game/FE/FEAudio.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/feInput.h"
#include "NL/nlFunctionMemory.h"
#include "NL/nlBind.h"
#include "Game/GameInfo.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feDPD.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/DB/SaveLoad.h"
#include "Game/GameSceneManager.h"
#include "Game/Render/Presentation.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOnlineInvitePlayers.h"
#include "Game/Camera/animcam.h"
#include "Game/FriendManager.h"

#include "Game/FE/tlImageInstance.h"
#include "Game/FE/feTextureResource.h"
#include "Game/Sys/movie.h"
#include "NL/gl/glState.h"
#include "NL/gl/glStruct.h"

#include "NL/nlConfig.h"
#include "NL/nlstring_tmpl.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlFormat.h"
#include <string.h>

extern "C" unsigned char SCGetSoundMode();
extern "C" int THPSimpleSetVolume(long volume, long time);

extern Config lbl_80578320;
extern const int lbl_804E8390[];

inline void TU801E68F0Control::Stop()
{
    if (mUnidentified05 && MovieStop())
    {
        if (mUnidentified08)
        {
            mUnidentified08->m_bVisible = false;
        }
        mUnidentified05 = false;
    }
}

bool TU801E68F0Control::fn_801E45C0()
{
    bool b = g_pFEInput->JustPressed(FE_ALL_PADS, 31, true, 0) || g_pFEInput->JustPressed(FE_ALL_PADS, 30, true, 0);
    return b;
}

TU801E4630Scene::TU801E4630Scene()
    : mUnidentifiedB0(0)
    , mUnidentifiedB4(false)
    , mUnidentifiedB5(false)
    , mUnidentifiedB6(false)
    , mUnidentified29C(true)
    , mUnidentified44C(0)
{
    if (GameInfoManager::Instance()->UseAltRules())
    {
        mUnidentified1C4.SetBackScene(27);
    }
    else
    {
        mUnidentified1C4.SetBackScene(4);
    }
    fn_801E4F40();
    Function<FnVoidVoid> callback(BindMember(this, &TU801E4630Scene::fn_801E68DC));
    UnidentifiedFindEvent<UnidentifiedEventNoData>("HBMHide", -1)->Add(
        callback, (unsigned int)&mUnidentifiedB8, -1);
}

TU801E4630Scene::~TU801E4630Scene()
{
    if (mUnidentifiedB0 != 0)
    {
        delete mUnidentifiedB0;
    }
    if (mUnidentifiedC0 != -1)
    {
        FEAudio::StopAnimAudioEvent(GetStadiumUnknown0x28(mUnidentifiedC8[mUnidentifiedC0]), this);
    }
    mUnidentified01C.Stop();
}

inline void TU801E68F0Control::Initialize(const char* filename)
{
    mUnidentified08 = 0;
    nlStrNCpy<char>(mUnidentified0C, filename, sizeof(mUnidentified0C));
    mUnidentified04 = false;
    mUnidentified05 = false;
    mUnidentified8C = true;
    mUnidentified8D = true;
}

void TU801E4630Scene::SceneCreated()
{
    mUnidentified01C.Initialize("art/movies/stadiumtest.thp");

    mUnidentified448 = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find(mPresentation->m_currentSlide, "Layer", "stadium_names");
    mUnidentified450 = FEFinder<TLInstance, TLAT_UNKNOWN>::Find(mPresentation->m_currentSlide, "Layer", "locked");
    mUnidentified450->m_bVisible = false;

    TLTextInstance* tickerText = FEFinder<TLTextInstance, TLAT_TEXT>::Find(mPresentation->m_currentSlide, "Layer", "TickerText");
    if (tickerText != 0)
    {
        glGetScreenInfo();
        mUnidentifiedB0 = new (8, false) FEScrollText(0);
        mUnidentifiedB0->ApplyNewTextInstancePointer(tickerText, 0, 242, 0);
        mUnidentifiedB0->SetDisplayMessage(GetStadiumTickerStringID(0));
    }

    mUnidentifiedBC = 1;
    for (int i = 0; i < 17; ++i)
    {
        if (IsStadiumEnabled(mUnidentifiedC8[i]) && IsStadiumUnlocked(mUnidentifiedC8[i]))
        {
            mUnidentifiedC4 = i;
            mUnidentifiedC0 = -1;
            break;
        }
    }

    TLComponentInstance* backButton = 0;
    TLComponentInstance* plusButton = 0;
    TLComponentInstance* minusButton = 0;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        if (GameInfoManager::Instance()->UseAltRules())
        {
            navigation->SetButtons(0x2F, true);
            mUnidentified44C = navigation->GetButton(0x20);
        }
        else
        {
            navigation->SetButtons(0x1F, true);
            mUnidentified44C = navigation->GetButton(0x10);
        }
        backButton = navigation->GetButton(4);
        plusButton = navigation->GetButton(1);
        minusButton = navigation->GetButton(2);
        navigation->HideButtons();
    }
    mUnidentified1C4.SetButtonInstance(backButton);
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    mUnidentified29C.SetPlusButton(plusButton);
    mUnidentified29C.SetMinusButton(minusButton);
    FEMusic::StopStream();
    if (GameInfoManager::Instance()->UseAltRules())
    {
        Presentation::GetInstance()->Call("FromUnrankedMatchToStadiumSelect");
    }
    else
    {
        Presentation::GetInstance()->Call("FromGrudgeMatchToStadiumSelect");
    }
}

void TU801E4630Scene::fn_801E4F40()
{
    int unlockedCount = 0;
    int lockedCount = 0;
    int lockedStadiums[17];
    for (int i = 0; i < 17; ++i)
    {
        if (IsStadiumUnlocked(lbl_804E8390[i]))
        {
            mUnidentifiedC8[unlockedCount] = lbl_804E8390[i];
            ++unlockedCount;
        }
        else
        {
            lockedStadiums[lockedCount] = lbl_804E8390[i];
            ++lockedCount;
        }
    }
    int j = 0;
    for (int i = unlockedCount; i < 17; ++i)
    {
        mUnidentifiedC8[i] = lockedStadiums[j];
        ++j;
    }
}

static const unsigned short stadiumCount[] = { '1', '7', 0 };

static void UpdateStadiumLabel(TU801E4630Scene* scene);

void TU801E4630Scene::Update(float deltaTime)
{
    switch (mUnidentifiedBC)
    {
    case 0:
        break;
    case 1:
        if (!Presentation::GetInstance()->mCameraFinished)
        {
            return;
        }
        mUnidentified01C.fn_801E68F0(GetStadiumMoviePath(mUnidentifiedC8[mUnidentifiedC4]));
        mUnidentifiedBC = 4;
        mUnidentifiedC0 = mUnidentifiedC4;
        if (mUnidentifiedB0 != 0)
        {
            mUnidentifiedB0->SetDisplayMessage(GetStadiumTickerStringID(mUnidentifiedC8[mUnidentifiedC0]));
        }
        if (mUnidentified448 != 0)
        {
            mUnidentified448->m_bVisible = true;
            mUnidentified448->SetActiveSlide(GetStadiumName(mUnidentifiedC8[mUnidentifiedC0]), true, false);
            UpdateStadiumLabel(this);
        }
        mUnidentifiedBC = 2;
        break;
    case 2:
    {
        cAnimCamera* camera = (cAnimCamera*)cCameraManager::PeekCamera();
        if (camera != 0 && camera->GetUnidentifiedAnimationTime() >= 1.0f)
        {
            mUnidentifiedBC = 4;
        }
        break;
    }
    case 3:
        mUnidentified01C.fn_801E68F0(GetStadiumMoviePath(mUnidentifiedC8[mUnidentifiedC4]));
        mUnidentifiedBC = 4;
        mUnidentifiedC0 = mUnidentifiedC4;
        if (mUnidentifiedB0 != 0)
        {
            mUnidentifiedB0->SetDisplayMessage(GetStadiumTickerStringID(mUnidentifiedC8[mUnidentifiedC0]));
        }
        if (mUnidentified448 != 0)
        {
            mUnidentified448->m_bVisible = true;
            mUnidentified448->SetActiveSlide(GetStadiumName(mUnidentifiedC8[mUnidentifiedC0]), true, false);
            UpdateStadiumLabel(this);
        }
        break;
    case 4:
        if (mUnidentifiedC0 != mUnidentifiedC4)
        {
            mUnidentifiedBC = 5;
        }
        break;
    case 5:
        mUnidentified01C.Stop();
        mUnidentifiedBC = 6;
        break;
    case 6:
        mUnidentifiedBC = 3;
        break;
    }

    mUnidentified01C.fn_801E6BA4(deltaTime);
    BaseSceneHandler::Update(deltaTime);
    if (!mUnidentifiedB4)
    {
        TLInstance* titles = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find(mPresentation->m_currentSlide, "Layer", "SCREEN_TITLES");
        TLSlide* slide = ((TLComponentInstance*)(titles == 0 ? &UnidentifiedTLComponentDefault::sInstance : titles))->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            return;
        }
        for (int i = 0; i < 4; ++i)
        {
            TLComponentInstance* pointer = GetPointerInstance(i);
            if (i == gFEControllerIndex)
            {
                pointer->SetActiveSlide("cursor", true, false);
            }
            else
            {
                pointer->SetActiveSlide("waiting", true, false);
            }
        }
        SHNavigation* navigation = GetNavigationScene();
        if (navigation != 0)
        {
            navigation->RestoreButtonVisibility();
            SetBreadcrumbs(17, mUnidentifiedC4);
        }
        fn_801E6170();
        mUnidentifiedB4 = true;
    }

    if (mUnidentifiedBC > 2)
    {
        TLSlide* activeSlide = mUnidentified448->GetActiveSlide();
        bool finished = true;
        if (activeSlide != 0)
        {
            TLSlide* slide = mUnidentified448->GetActiveSlide();
            float endTime = slide->GetStartTime() + slide->GetDuration();
            finished = mUnidentified448->GetActiveSlide()->GetCurrentTime() >= endTime;
        }
        if (finished)
        {
            unsigned char valid = 1;
            FEPointerEvent event;
            int pad = gFEControllerIndex;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 30, true, 0);
            if (mUnidentified1C4.UpdateBackButton(event, deltaTime))
            {
                if (GameInfoManager::Instance()->UseAltRules())
                {
                    FEAudio::PlayAnimAudioEvent(0xC385EFFB, 0, 0, true);
                    FEAudio::PlayAnimAudioEvent(0xBF2ED62D, 0, 0, true);
                    FEMusic::StartStreamIfDifferent(8);
                    Presentation::GetInstance()->Call("FromStadiumSelectToUnrankedMatch");
                }
                else
                {
                    FEAudio::PlayAnimAudioEvent(0xBF2ED62D, 0, 0, true);
                    FEMusic::StartStreamIfDifferent(2);
                    Presentation::GetInstance()->Call("FromStadiumSelectToGrudgeMatch");
                }
                mUnidentified01C.Stop();
                mUnidentified1C4.Disable();
                return;
            }
            mUnidentified29C.Update(event, deltaTime);
            mUnidentified110.HandlePointerEvent(&event);

            bool changed = false;
            bool previous = mUnidentified29C.mPointerPressed[1] || mUnidentified29C.mPadPressed[1];
            if (previous)
            {
                FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, true);
                mUnidentifiedC4 = (mUnidentifiedC4 + 16) % 17;
                SetBreadcrumbs(17, mUnidentifiedC4);
                changed = true;
            }
            else
            {
                bool next = mUnidentified29C.mPointerPressed[0] || mUnidentified29C.mPadPressed[0];
                if (next)
                {
                    FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, true);
                    mUnidentifiedC4 = (mUnidentifiedC4 + 1) % 17;
                    SetBreadcrumbs(17, mUnidentifiedC4);
                    changed = true;
                }
            }
            if (changed)
            {
                if (IsStadiumUnlocked(mUnidentifiedC8[mUnidentifiedC4]))
                {
                    mUnidentified110.Enable();
                    mUnidentified44C->m_bVisible = true;
                    mUnidentified450->m_bVisible = false;
                }
                else
                {
                    mUnidentified110.Disable();
                    mUnidentified44C->m_bVisible = false;
                    mUnidentified44C->SetActiveSlide("off", true, false);
                    mUnidentified450->m_bVisible = true;
                }
            }

            if (GameInfoManager::Instance()->UseAltRules()
                && !GameSceneManager::Instance()->IsOnStack((SceneList)10)
                && g_pFriendManager->FindHostInvitation())
            {
                FriendManager* manager = g_pFriendManager;
                manager->mReturnScene = 5;
                manager->mPreviousRankedMode = 0;
                Presentation::GetInstance()->Call("FromStadiumSelectToUnrankedMatch");
                GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_RESPONSE, SCREEN_FORWARD, true);
                return;
            }
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (i != gFEControllerIndex || mUnidentifiedB6 == true)
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else if (mUnidentifiedB5 || mUnidentified29C.mPointerInside[1]
            || mUnidentified29C.mPointerInside[0] || mUnidentified1C4.mPointerInside[i])
        {
            pointer->SetActiveSlide("A", true, false);
        }
        else
        {
            pointer->SetActiveSlide("cursor", true, false);
        }
    }
    if (mUnidentifiedB0 != 0)
    {
        mUnidentifiedB0->Update(deltaTime);
    }
}

static void UpdateStadiumLabel(TU801E4630Scene* scene)
{
    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", scene->mUnidentifiedC4 + 1);
    WideBasicString text(Format(WideBasicString(LookupLocString("X_OF_X")), number, stadiumCount));
    nlStrNCpy(scene->mUnidentified424, text.c_str(), 16);
    FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(scene->mUnidentified448->GetActiveSlide(), "quantity")->SetString(scene->mUnidentified424);
}

void TU801E4630Scene::fn_801E6170()
{
    typedef Detail::MemFunImpl<void, void (TU801E4630Scene::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, TU801E4630Scene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    if (GameInfoManager::Instance()->UseAltRules())
    {
        SetDoneButtonBounds(&mUnidentified110, mUnidentified44C, 0);
    }
    else
    {
        SetPlayButtonBounds(&mUnidentified110, mUnidentified44C);
    }

    Function2<void, int, void*> callback = PointerBinding(MemFun(&TU801E4630Scene::fn_801E6504), this, Placeholder<0>(), Placeholder<1>());
    mUnidentified110.SetPointerEnterCallback(callback);
    callback = PointerBinding(MemFun(&TU801E4630Scene::fn_801E6578), this, Placeholder<0>(), Placeholder<1>());
    mUnidentified110.SetPointerLeaveCallback(callback);
    Function2<void, int, void*> pressCallback = PointerBinding(MemFun(&TU801E4630Scene::fn_801E65D4), this, Placeholder<0>(), Placeholder<1>());
    mUnidentified110.SetPointerPressCallback(pressCallback);
}

void TU801E4630Scene::fn_801E6504(int index, void*)
{
    mUnidentified44C->SetActiveSlide("over", true, false);
    mUnidentified110.SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, true);
    mUnidentifiedB5 = true;
}

void TU801E4630Scene::fn_801E6578(int index, void*)
{
    mUnidentified44C->SetActiveSlide("off", true, false);
    mUnidentified110.SetPointerState(0, index);
    mUnidentifiedB5 = false;
}

void TU801E4630Scene::fn_801E65D4(int, void*)
{
    if (IsStadiumUnlocked(mUnidentifiedC8[mUnidentifiedC4]))
    {
        if (GameInfoManager::Instance()->UseAltRules())
        {
            FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, true);
            FEAudio::PlayAnimAudioEvent(0xBF2ED62D, 0, 0, true);
            Presentation::GetInstance()->Call("FromStadiumSelectToUnrankedMatch");
            GameInfoManager::Instance()->SetStadium(mUnidentifiedC8[mUnidentifiedC4]);
            SHOnlineInvitePlayers* scene = (SHOnlineInvitePlayers*)GameSceneManager::Instance()->Push((SceneList)44, SCREEN_NOTHING, true);
            scene->mIsHost = true;
            scene->mStartFriendServer = true;
        }
        else
        {
            FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, true);
            for (int i = 0; i < 4; ++i)
            {
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            }
            GameSceneManager::Instance()->PushLoadingScene(true);
            if (!GameInfoManager::Instance()->IsOnline())
            {
                SaveLoad::StartSave(false);
            }
            GameInfoManager::Instance()->SetStadium(mUnidentifiedC8[mUnidentifiedC4]);
            GetNavigationScene()->SetButtons(0, true);
        }
        mUnidentified1C4.Disable();
        mUnidentified110.Disable();
        mUnidentifiedB6 = true;
    }
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
}

void TU801E4630Scene::fn_801E68DC()
{
    mUnidentified01C.mUnidentified05 = false;
    mUnidentifiedBC = 3;
}

void TU801E68F0Control::fn_801E68F0(const char* filename)
{
    if (!mUnidentified05)
    {
        nlStrNCpy<char>(mUnidentified0C, filename, sizeof(mUnidentified0C));
        mUnidentified05 = MovieStart(mUnidentified0C, mUnidentified8C, mUnidentified8D, SCGetSoundMode() == 0);
        fn_80370E90(false);

        const char* streamName;
        if (strstr(mUnidentified0C, "nlg"))
        {
            streamName = "FE_Eggman_Movie";
        }
        else
        {
            streamName = "FE_Intro_Movie";
        }
        char key[64];
        nlSNPrintf(key, sizeof(key), "%s/Volume", streamName);
        float volume = (float)GetConfigInt(lbl_80578320, key, 100) / 100.0f;
        nlSNPrintf(key, sizeof(key), "%s/FadeIn", streamName);
        int fadeIn = GetConfigInt(lbl_80578320, key, 500);
        THPSimpleSetVolume(0, 0);
        THPSimpleSetVolume((int)(127.0f * volume), fadeIn);
        if (mUnidentified08)
        {
            mUnidentified08->m_bVisible = true;
        }
        mUnidentified90 = 0;
    }
}

const char* lbl_806DD5D4 = "movie";

void TU801E68F0Control::fn_801E6BA4(float)
{
    if (fn_803713C4())
    {
        if (!mUnidentified05)
        {
            Stop();
        }
        else
        {
            if (!mUnidentified04)
            {
                unsigned long texture = glGetTexture(lbl_806DD5D4);
                if (mUnidentified08)
                {
                    mUnidentified08->m_pTextureResource->SetTextureHandle(texture);
                }
                mUnidentified04 = true;
            }
            if (fn_803713CC())
            {
                ++mUnidentified90;
                fn_803713D4();
                if (mUnidentified90 >= 5)
                {
                    Stop();
                }
            }
            else
            {
                mUnidentified90 = 0;
            }
        }
    }
}

void FEBackButton::SetBackScene(int value)
{
    mBackScene = value;
}
