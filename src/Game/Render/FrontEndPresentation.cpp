#include "Game/Render/FrontEndPresentation.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/SaveLoad.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feModelManager.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feCupFlow.h"

#include "Game/BasicStadium.h"
#include "Game/FE/feMusic.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Camera/animcam.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EffectsGroup.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/GameInfo.h"
#include "Game/NetTournManager.h"
#include "NL/nlDebug.h"
#include "NL/nlFile.h"
#include "NL/nlFunction.h"
#include "NL/nlPrint.h"
#include "NL/nlSingleton.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "NL/nlstring_tmpl.h"
#include "Game/SH/SHNavigation.h"
#include "Game/UnidentifiedStaticStorage.h"

static char sPresentationByteCode[] = "art/Scripts/fe_presentation.byte_code";
static char sBronzeFormat[] = "%sbronze";
static char sSilverFormat[] = "%ssilver";
static char sGoldFormat[] = "%sgold";
static char sWaitingSlide[] = "waiting";
static char sIdleFunctionName[] = "Idle";
static const char* idleFun = sIdleFunctionName;
static char sIdleAnimation[] = "fe_idle";

extern "C" void ShowRoundNews(void*);
extern "C" void ShowCupRulesPopup();
extern "C" void ShowCupBrickWallNews();
extern "C" void ShowCupGoldenBootNews();
extern "C" void ShowCupAwardRewardsPopup();
extern "C" void SetCupTrophiesVisible(bool);
extern "C" void BeginLoadTournamentTrophy();
extern "C" bool IsTournamentTrophyLoaded();
extern "C" void FinishLoadTournamentTrophy();
extern "C" void SetWorldAnimation(unsigned int, unsigned int, unsigned int);
extern "C" void TriggerEffects(BasicStadium*, unsigned int);

extern bool g_e3_Build;

class PresentationLookupResult
{
public:
    virtual ~PresentationLookupResult();
    virtual void UnidentifiedVirtual1() = 0;
    virtual void* GetValue() = 0;
};


inline FrontEndPresentation::FrontEndPresentation()
    : InterpreterCore(100)
    , mWaitTime(0.0f)
    , mDeltaTime(0.0f)
    , mCameraFinished(false)
    , mCameraTransitionFinished(false)
{
    unsigned long fileSize = 0;
    void* byteCode = nlLoadEntireFile(sPresentationByteCode, &fileSize, 0x20, AllocateStart, 0, 0, 0);
    LoadByteCode(byteCode);
    nlStrNCpy(mCurrentFunction, idleFun, 64);
    CallFunction(nlStringHash(mCurrentFunction));
}

inline FrontEndPresentation& FrontEndPresentation::Instance()
{
    static FrontEndPresentation instance;
    return instance;
}

static void OnCameraAnimationFinished()
{
    FrontEndPresentation::Instance().mCameraFinished = true;
}

static void OnCameraTransitionFinished(eCameraMessage message)
{
    FrontEndPresentation::Instance().mCameraTransitionFinished = true;
}

FrontEndPresentation* FrontEndPresentation::GetInstance()
{
    return &FrontEndPresentation::Instance();
}

FrontEndPresentation::~FrontEndPresentation()
{
}

void FrontEndPresentation::Update(float deltaTime)
{
    mDeltaTime = deltaTime;
    if (m_RunState == 3)
    {
        Run();
    }

    if (m_RunState == 2)
    {
        mCameraFinished = false;
        mWaitTime = 0.0f;
        mCameraTransitionFinished = false;

        const char* functionName = idleFun;
        mWaitTime = 0.0f;
        mCameraFinished = false;
        mCameraTransitionFinished = false;
        nlStrNCpy(mCurrentFunction, functionName, 64);
        Reset();
        CallFunction(nlStringHash(functionName));
    }
}

bool FrontEndPresentation::IsActive() const
{
    return nlStrCmp<char>(idleFun, mCurrentFunction) != 0;
}

void FrontEndPresentation::Call(const char* functionName)
{
    mCameraFinished = false;
    mWaitTime = 0.0f;
    mCameraTransitionFinished = false;
    nlStrNCpy(mCurrentFunction, functionName, 64);
    Reset();
    CallFunction(nlStringHash(functionName));
}

static void DisableFrontEndPresentationEmission(EmissionController& controller)
{
    FrontEndPresentation& presentation = FrontEndPresentation::Instance();
    unsigned int hash = nlStringLowerHash(presentation.mEmissionName);
    if (hash == controller.m_pGroup->GetHashID())
    {
        controller.m_bDisabled = true;
    }
}

static void EnableFrontEndPresentationEmission(EmissionController& controller)
{
    FrontEndPresentation& presentation = FrontEndPresentation::Instance();
    unsigned int hash = nlStringLowerHash(presentation.mEmissionName);
    if (hash == controller.m_pGroup->GetHashID())
    {
        controller.m_bDisabled = false;
    }
}

static inline cAnimCamera* GetCurrentAnimatedCamera()
{
    return (cAnimCamera*)cCameraManager::PeekCamera();
}

void FrontEndPresentation::DoFunctionCall(unsigned int function)
{
    switch (function)
    {
    case 0:
        NetTournManager::Instance()->DetachTournamentTrophy();
        break;
    case 1:
    {
        bool value = m_SP[-1] != 0;
        int side = (int)m_SP[-2];
        unsigned int name = m_SP[-3];
        m_SP -= 3;
        bool alternate = false;
        if (side == -1 && GameInfoManager::Instance()->mCurrentMode == 3)
        {
            side = g_pCupManager->unknown_0x8A28;
        }
        else
        {
            int captain = GameInfoManager::Instance()->GetTeam((short)side);
            int opponent = GameInfoManager::Instance()->GetTeam((short)!side);
            alternate = CaptainsNeedAlternateColour(captain, opponent);
            side = captain;
        }
        FEModelManager::Instance()->CreateModel(FE_MODEL_IMPOSTOR, (const char*)name, side, value, 0, 0, alternate);
        break;
    }
    case 2:
        FEModelManager::Instance()->ReleaseImpostors();
        break;
    case 3:
        NetTournManager::Instance()->DestroyTournamentTrophy();
        break;
    case 4:
        g_pCupManager->ShowRoundNews();
        break;
    case 5:
    {
        unsigned int value = m_SP[-1];
        --m_SP;
        BasicStadium* stadium = BasicStadium::GetCurrentStadium();
        if (stadium != 0)
        {
            stadium->TriggerEffects(value);
        }
        break;
    }
    case 6:
    {
        unsigned int value = m_SP[-1];
        m_SP[-1] = nlStringHash((const char*)value);
        if (m_RunState == 3)
        {
            m_SP[-1] = value;
        }
        break;
    }
    case 7:
        SetCupTrophiesVisible(false);
        break;
    case 8:
        StopWithUndo();
        break;
    case 9:
        ++m_SP;
        m_SP[-1] = g_e3_Build;
        break;
    case 10:
    {
        const char* animationName;
        FEModelHandle* object;
        const char* objectName;
        objectName = (const char*)m_SP[-2];
        animationName = (const char*)m_SP[-1];
        --m_SP;
        object = FEModelManager::Instance()->GetModel(objectName);
        unsigned int isPlaying;
        if (object != 0 && object->IsPlayingAnimation(animationName)
            && !object->IsAnimationFinished())
        {
            isPlaying = 1;
        }
        else
        {
            isPlaying = 0;
        }
        m_SP[-1] = isPlaying;
        if (m_RunState == 3)
        {
            m_SP[-1] = (unsigned int)objectName;
        }
        break;
    }
    case 11:
        ++m_SP;
        m_SP[-1] = IsWidescreen();
        break;
    case 12:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        EmissionManager* manager = EmissionManager::Instance();
        EffectsGroup* group = manager->GetEffectsGroup(name);
        if (group != 0)
        {
            manager->Destroy(group);
        }
        break;
    }
    case 13:
        BeginLoadTournamentTrophy();
        break;
    case 14:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FrontEndPresentation& presentation = FrontEndPresentation::Instance();
        nlStrNCpy(presentation.mEmissionName, name, 64);
        Function1<void, EmissionController&> callback(DisableFrontEndPresentationEmission);
        EmissionManager::Instance()->ForEachController(callback);
        break;
    }
    case 15:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        camera->SelectCameraAnimation(name);
        camera->m_bCyclic = false;
        camera->m_EndOfAnimationCallback = OnCameraAnimationFinished;
        break;
    }
    case 16:
    {
        ePlayMode playMode = (ePlayMode)m_SP[-1];
        const char* animationName = (const char*)m_SP[-2];
        const char* objectName = (const char*)m_SP[-3];
        m_SP -= 3;
        SetWorldAnimation(objectName, animationName, playMode);
        break;
    }
    case 17:
    {
        float value = *(float*)&m_SP[-1];
        unsigned int argument1 = m_SP[-2];
        const char* argument0 = (const char*)m_SP[-3];
        const char* objectName = (const char*)m_SP[-4];
        m_SP -= 4;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(objectName);
        if (object != 0)
        {
            object->PlayAnimation(argument0, (ePlayMode)argument1, 0.0f, value, false);
        }
        break;
    }
    case 18:
    {
        unsigned int argument1 = m_SP[-1];
        const char* argument0 = (const char*)m_SP[-2];
        const char* objectName = (const char*)m_SP[-3];
        m_SP -= 3;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(objectName);
        if (object != 0)
        {
            object->PlayAnimation(argument0, (ePlayMode)argument1, 0.0f, 0.0f, false);
        }
        break;
    }
    case 19:
        FEAudio::PlayAnimAudioEvent(0xB60A9CC0, 0, 0, true);
        break;
    case 20:
    {
        unsigned int event = m_SP[-1];
        --m_SP;
        FEAudio::PlayAnimAudioEvent(event, 0, 0, true);
        break;
    }
    case 21:
    {
        float duration = *(float*)&m_SP[-1];
        --m_SP;
        PopPresentationCamera(OnCameraTransitionFinished, duration);
        mCameraTransitionFinished = false;
        break;
    }
    case 22:
    {
        unsigned int childName = m_SP[-1];
        unsigned int objectName = m_SP[-2];
        m_SP -= 2;
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)objectName);
        if (object != 0)
        {
            PresentationLookupResult* child = (PresentationLookupResult*)FEModelManager::Instance()->GetObject(childName);
            if (child != 0)
            {
                object->SetTransform(*(nlMatrix4*)child->GetValue());
            }
        }
        break;
    }
    case 23:
    {
        bool value = m_SP[-1] != 0;
        float duration = *(float*)&m_SP[-2];
        const char* name = (const char*)m_SP[-3];
        m_SP -= 3;
        PushPresentationCamera(name, OnCameraTransitionFinished, duration, value);
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        camera->m_bCyclic = false;
        camera->m_EndOfAnimationCallback = OnCameraAnimationFinished;
        mCameraFinished = false;
        mCameraTransitionFinished = false;
        break;
    }
    case 24:
    {
        bool value = m_SP[-1] != 0;
        float duration = *(float*)&m_SP[-2];
        const char* baseName = (const char*)m_SP[-3];
        m_SP -= 3;
        char name[64];
        int mode = g_pCupManager->GetCurrentMode();
        if (mode == 0)
            nlSNPrintf(name, sizeof(name), sBronzeFormat, baseName);
        else if (mode == 1)
            nlSNPrintf(name, sizeof(name), sSilverFormat, baseName);
        else
            nlSNPrintf(name, sizeof(name), sGoldFormat, baseName);
        PushPresentationCamera(name, OnCameraTransitionFinished, duration, value);
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        camera->m_bCyclic = false;
        camera->m_EndOfAnimationCallback = OnCameraAnimationFinished;
        mCameraFinished = false;
        mCameraTransitionFinished = false;
        break;
    }
    case 25:
    {
        int value1 = (int)m_SP[-1];
        int value0 = (int)m_SP[-2];
        m_SP -= 2;
        GameSceneManager::Instance()->Push((SceneList)value0, (ScreenMovement)value1, false);
        break;
    }
    case 26:
        GameSceneManager::Instance()->Push((SceneList)8, SCREEN_FORWARD, false);
        break;
    case 27:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            FEModelManager::Instance()->DestroyModel(object);
        }
        break;
    }
    case 28:
    {
        float time = *(float*)&m_SP[-1];
        --m_SP;
        GetCurrentAnimatedCamera()->SetAnimationTime(time, true);
        break;
    }
    case 29:
    {
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        float time = camera->GetUnidentifiedDuration();
        camera->SetAnimationTime(time, true);
        break;
    }
    case 30:
    {
        unsigned int value = m_SP[-1];
        const char* name = (const char*)m_SP[-2];
        m_SP -= 2;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            object->SetDefaultAnimation((const char*)value);
        }
        break;
    }
    case 31:
    {
        unsigned int childName = m_SP[-1];
        const char* name = (const char*)m_SP[-2];
        m_SP -= 2;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            PresentationLookupResult* child = (PresentationLookupResult*)FEModelManager::Instance()->GetObject(childName);
            if (child != 0)
            {
                object->SetPosition(*(nlVector3*)((char*)child->GetValue() + 0x30));
            }
        }
        break;
    }
    case 32:
    {
        float time = *(float*)&m_SP[-1];
        --m_SP;
        mWaitTime = time;
        break;
    }
    case 33:
    {
        bool value = m_SP[-1] != 0;
        const char* name = (const char*)m_SP[-2];
        m_SP -= 2;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            object->mEnabled = value;
        }
        break;
    }
    case 34:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            object->SetAnimationCompleteCallback(OnFrontEndPresentationModelAnimationFinished);
        }
        break;
    }
    case 35:
    {
        bool cyclic = m_SP[-1] != 0;
        --m_SP;
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        if (camera != 0)
        {
            camera->m_bCyclic = cyclic;
        }
        break;
    }
    case 36:
        SetCupTrophiesVisible(true);
        break;
    case 37:
        ShowCupAwardRewardsPopup();
        break;
    case 38:
        ShowCupBrickWallNews();
        break;
    case 39:
        ShowCupRulesPopup();
        break;
    case 40:
        ShowCupGoldenBootNews();
        break;
    case 41:
        if (SHNavigation* scene = GetNavigationScene())
        {
            scene->StartTransition();
        }
        break;
    case 42:
        FEMusic::StartStreamIfDifferent(1);
        if (SHNavigation* scene = GetNavigationScene())
        {
            scene->HideButtons();
        }
        for (int i = 0; i < 4; ++i)
        {
            GetPointerInstance(i)->SetActiveSlide(
                sWaitingSlide, true, false);
        }
        break;
    case 43:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FrontEndPresentation& presentation = FrontEndPresentation::Instance();
        nlStrNCpy(presentation.mEmissionName, name, 64);
        Function1<void, EmissionController&> callback(EnableFrontEndPresentationEmission);
        EmissionManager::Instance()->ForEachController(callback);
        break;
    }
    case 44:
        if (!mCameraFinished)
            StopWithUndo();
        else
            mCameraFinished = false;
        break;
    case 45:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0 && !object->IsAnimationFinished())
        {
            StopWithUndo();
        }
        break;
    }
    case 46:
        if (!mCameraTransitionFinished)
            StopWithUndo();
        else
            mCameraTransitionFinished = false;
        break;
    case 47:
        if (SaveEnabled && InOperation)
        {
            StopWithUndo();
        }
        break;
    case 48:
        mWaitTime -= mDeltaTime;
        if (mWaitTime > 0.0f)
            StopWithUndo();
        else
            mWaitTime = 0.0f;
        break;
    case 49:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        if (FEModelManager::Instance()->GetModel(name) == 0)
        {
            StopWithUndo();
        }
        break;
    }
    case 50:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            if (!object->IsLoaded())
            {
                StopWithUndo();
            }
        }
        else
        {
            StopWithUndo();
        }
        break;
    }
    case 51:
        if (IsTournamentTrophyLoaded())
            FinishLoadTournamentTrophy();
        else
            StopWithUndo();
        break;
    default:
        nlBreak();
        break;
    }
}

void OnFrontEndPresentationModelAnimationFinished(FEModelHandle* object)
{
    if (object != 0)
    {
        object->PlayAnimation(sIdleAnimation, PM_CYCLIC, 0.2f, 0.0f, false);
    }
}
