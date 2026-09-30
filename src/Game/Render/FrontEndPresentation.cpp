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
#include "Game/main.h"

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

static void OnCameraAnimationFinished()
{
    FrontEndPresentation::GetInstance()->mCameraFinished = true;
}

static void OnCameraTransitionFinished(eCameraMessage message)
{
    FrontEndPresentation::GetInstance()->mCameraTransitionFinished = true;
}

FrontEndPresentation* FrontEndPresentation::GetInstance()
{
    static FrontEndPresentation instance;
    return &instance;
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

static void DisableFrontEndPresentationEmission(EmissionController& controller);
static void EnableFrontEndPresentationEmission(EmissionController& controller);

static inline cAnimCamera* GetCurrentAnimatedCamera()
{
    return (cAnimCamera*)cCameraManager::PeekCamera();
}

static void DestroyFrontEndPresentationEmission(const char* name)
{
    EmissionManager* manager = EmissionManager::Instance();
    EffectsGroup* group = manager->GetEffectsGroup(name);
    if (group != 0)
    {
        manager->Destroy(group);
    }
}

static void DisableFrontEndPresentationEmission(const char* name)
{
    FrontEndPresentation& presentation = *FrontEndPresentation::GetInstance();
    nlStrNCpy(presentation.mEmissionName, name, 64);
    Function1<void, EmissionController&> callback(DisableFrontEndPresentationEmission);
    EmissionManager::Instance()->ForEachController(callback);
}

static void EnableFrontEndPresentationEmission(const char* name)
{
    FrontEndPresentation& presentation = *FrontEndPresentation::GetInstance();
    nlStrNCpy(presentation.mEmissionName, name, 64);
    Function1<void, EmissionController&> callback(EnableFrontEndPresentationEmission);
    EmissionManager::Instance()->ForEachController(callback);
}

static void StartFrontEndPresentationMusic()
{
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
}

#include "src/Game/Render/FrontEndPresentation_interp.cpp"

void OnFrontEndPresentationModelAnimationFinished(FEModelHandle* object)
{
    if (object != 0)
    {
        object->PlayAnimation(sIdleAnimation, PM_CYCLIC, 0.2f, 0.0f, false);
    }
}

static void DisableFrontEndPresentationEmission(EmissionController& controller)
{
    FrontEndPresentation& presentation = *FrontEndPresentation::GetInstance();
    unsigned int hash = nlStringLowerHash(presentation.mEmissionName);
    if (hash == controller.m_pGroup->GetHashID())
    {
        controller.m_bDisabled = true;
    }
}

static void EnableFrontEndPresentationEmission(EmissionController& controller)
{
    FrontEndPresentation& presentation = *FrontEndPresentation::GetInstance();
    unsigned int hash = nlStringLowerHash(presentation.mEmissionName);
    if (hash == controller.m_pGroup->GetHashID())
    {
        controller.m_bDisabled = false;
    }
}
