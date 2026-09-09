#include "Game/SH/SHNavigation.h"
#include "Game/GameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/SaveLoad.h"
#include "Game/Render/Presentation.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feModelManager.h"

#include "Game/BasicStadium.h"
#include "Game/FE/feMusic.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Camera/animcam.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/Effects/EmissionManager.h"
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

static char sPresentationByteCode[] = "art/Scripts/fe_presentation.byte_code";
static char sBronzeFormat[] = "%sbronze";
static char sSilverFormat[] = "%ssilver";
static char sGoldFormat[] = "%sgold";
static char sWaitingSlide[] = "waiting";
static char sIdleFunctionName[] = "Idle";
static const char* idleFun = sIdleFunctionName;
static char sIdleAnimation[] = "fe_idle";

extern "C" void fn_8010E294(void*);
extern "C" void fn_802081C0();
extern "C" void fn_80208458();
extern "C" void fn_80208518();
extern "C" void fn_80208594();
extern "C" void fn_802092D0(bool);
extern "C" void fn_80276D10();
extern "C" bool fn_80276DE0();
extern "C" void fn_80276E0C();
extern "C" void fn_80277BB4(unsigned int, unsigned int, unsigned int);
extern "C" void fn_802E8A2C(EmissionManager*, EffectsGroup*);
extern "C" void fn_802E8B78(EmissionManager*, Function<void*>*);
extern "C" void fn_80341E68(BasicStadium*, unsigned int);
extern "C" void fn_802DEDE8(InterpreterCore*);

extern bool g_e3_Build;

class PresentationLookupResult
{
public:
    virtual ~PresentationLookupResult();
    virtual void UnidentifiedVirtual1() = 0;
    virtual void* GetValue() = 0;
};


inline Presentation::Presentation()
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

inline Presentation& Presentation::Instance()
{
    static Presentation instance;
    return instance;
}

static void OnCameraAnimationFinished()
{
    Presentation::Instance().mCameraFinished = true;
}

static void OnCameraTransitionFinished(eCameraMessage message)
{
    Presentation::Instance().mCameraTransitionFinished = true;
}

Presentation* Presentation::GetInstance()
{
    return &Presentation::Instance();
}

Presentation::~Presentation()
{
}

void Presentation::Update(float deltaTime)
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
        fn_802DEDE8(this);
        CallFunction(nlStringHash(functionName));
    }
}

bool Presentation::IsActive() const
{
    return nlStrCmp<char>(idleFun, mCurrentFunction) != 0;
}

void Presentation::Call(const char* functionName)
{
    mCameraFinished = false;
    mWaitTime = 0.0f;
    mCameraTransitionFinished = false;
    nlStrNCpy(mCurrentFunction, functionName, 64);
    fn_802DEDE8(this);
    CallFunction(nlStringHash(functionName));
}

void OnPresentationModelAnimationFinished(FEModelHandle* object)
{
    if (object != 0)
    {
        object->PlayAnimation(sIdleAnimation, PM_CYCLIC, 0.2f, 0.0f, false);
    }
}

struct PresentationEmissionEvent
{
    void* data;
    unsigned char unknown_0x04[0x3F];
    bool active;
};

extern "C" void fn_801FF2A8(PresentationEmissionEvent* event)
{
    Presentation& presentation = Presentation::Instance();
    unsigned int hash = nlStringLowerHash(presentation.mEmissionName);
    if (hash == *(unsigned int*)event->data)
    {
        event->active = true;
    }
}

extern "C" void fn_801FF42C(PresentationEmissionEvent* event)
{
    Presentation& presentation = Presentation::Instance();
    unsigned int hash = nlStringLowerHash(presentation.mEmissionName);
    if (hash == *(unsigned int*)event->data)
    {
        event->active = false;
    }
}

static inline cAnimCamera* GetCurrentAnimatedCamera()
{
    return (cAnimCamera*)cCameraManager::PeekCamera();
}

void Presentation::DoFunctionCall(unsigned int function)
{
    switch (function)
    {
    case 0:
        NetTournManager::Instance()->DetachTournamentTrophy();
        break;
    case 1:
    {
        unsigned int value = Pop();
        int side = (int)Pop();
        unsigned int name = Pop();
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
        }
        fn_801C27C4(FEModelManager::Instance(), FE_MODEL_IMPOSTOR, (const char*)name, side, value != 0, 0, 0, alternate);
        break;
    }
    case 2:
        fn_801C3014(FEModelManager::Instance());
        break;
    case 3:
        NetTournManager::Instance()->DestroyTournamentTrophy();
        break;
    case 4:
        fn_8010E294(g_pCupManager);
        break;
    case 5:
    {
        unsigned int value = Pop();
        BasicStadium* stadium = BasicStadium::GetCurrentStadium();
        if (stadium != 0)
        {
            fn_80341E68(stadium, value);
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
        fn_802092D0(false);
        break;
    case 8:
        StopWithUndo();
        break;
    case 9:
        *m_SP++ = g_e3_Build;
        break;
    case 10:
    {
        unsigned int original = m_SP[-2];
        unsigned int value = Pop();
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)original);
        m_SP[-1] = object != 0 && object->IsPlayingAnimation((const char*)value)
                && !object->IsAnimationFinished();
        if (m_RunState == 3)
        {
            m_SP[-1] = original;
        }
        break;
    }
    case 11:
        *m_SP++ = IsWidescreen();
        break;
    case 12:
    {
        const char* name = (const char*)Pop();
        EmissionManager* manager = EmissionManager::Instance();
        EffectsGroup* group = manager->GetEffectsGroup(name);
        if (group != 0)
        {
            fn_802E8A2C(manager, group);
        }
        break;
    }
    case 13:
        fn_80276D10();
        break;
    case 14:
    {
        const char* name = (const char*)Pop();
        Presentation& presentation = Presentation::Instance();
        nlStrNCpy(presentation.mEmissionName, name, 64);
        Function<void*> callback((void (*)(void*))fn_801FF2A8);
        fn_802E8B78(EmissionManager::Instance(), &callback);
        break;
    }
    case 15:
    {
        const char* name = (const char*)Pop();
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        camera->SelectCameraAnimation(name);
        camera->m_bCyclic = false;
        camera->m_EndOfAnimationCallback = OnCameraAnimationFinished;
        break;
    }
    case 16:
    {
        unsigned int value2 = Pop();
        unsigned int value1 = Pop();
        unsigned int value0 = Pop();
        fn_80277BB4(value0, value1, value2);
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
        unsigned int argument1 = Pop();
        const char* argument0 = (const char*)Pop();
        const char* objectName = (const char*)Pop();
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
        FEAudio::PlayAnimAudioEvent(Pop(), 0, 0, true);
        break;
    case 21:
        PopPresentationCamera(OnCameraTransitionFinished, *(float*)&m_SP[-1]);
        --m_SP;
        mCameraTransitionFinished = false;
        break;
    case 22:
    {
        unsigned int childName = Pop();
        unsigned int objectName = Pop();
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)objectName);
        if (object != 0)
        {
            PresentationLookupResult* child = (PresentationLookupResult*)fn_801C2798(
                FEModelManager::Instance(), childName);
            if (child != 0)
            {
                object->SetTransform(*(nlMatrix4*)child->GetValue());
            }
        }
        break;
    }
    case 23:
    {
        bool value = Pop() != 0;
        float duration = *(float*)&m_SP[-1];
        --m_SP;
        const char* name = (const char*)Pop();
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
        bool value = Pop() != 0;
        float duration = *(float*)&m_SP[-1];
        --m_SP;
        const char* baseName = (const char*)Pop();
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
        int value1 = (int)Pop();
        int value0 = (int)Pop();
        GameSceneManager::Instance()->Push((SceneList)value0, (ScreenMovement)value1, false);
        break;
    }
    case 26:
        GameSceneManager::Instance()->Push((SceneList)8, SCREEN_FORWARD, false);
        break;
    case 27:
    {
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)Pop());
        if (object != 0)
        {
            fn_801C2BD8(FEModelManager::Instance(), object);
        }
        break;
    }
    case 28:
        GetCurrentAnimatedCamera()->SetAnimationTime(
            *(float*)&m_SP[-1], true);
        --m_SP;
        break;
    case 29:
    {
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        float time = camera->m_pActiveCameraData != 0
                       ? (float)camera->m_pActiveCameraData->m_uKeyCount / 30.0f
                       : 0.0f;
        camera->SetAnimationTime(time, true);
        break;
    }
    case 30:
    {
        unsigned int value = Pop();
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)Pop());
        if (object != 0)
        {
            object->SetDefaultAnimation((const char*)value);
        }
        break;
    }
    case 31:
    {
        unsigned int childName = Pop();
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)Pop());
        if (object != 0)
        {
            PresentationLookupResult* child = (PresentationLookupResult*)fn_801C2798(
                FEModelManager::Instance(), childName);
            if (child != 0)
            {
                object->SetPosition(*(nlVector3*)((char*)child->GetValue() + 0x30));
            }
        }
        break;
    }
    case 32:
        mWaitTime = *(float*)&m_SP[-1];
        --m_SP;
        break;
    case 33:
    {
        bool value = Pop() != 0;
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)Pop());
        if (object != 0)
        {
            object->mEnabled = value;
        }
        break;
    }
    case 34:
    {
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)Pop());
        if (object != 0)
        {
            object->SetAnimationCompleteCallback(OnPresentationModelAnimationFinished);
        }
        break;
    }
    case 35:
    {
        bool cyclic = Pop() != 0;
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        if (camera != 0)
        {
            camera->m_bCyclic = cyclic;
        }
        break;
    }
    case 36:
        fn_802092D0(true);
        break;
    case 37:
        fn_80208594();
        break;
    case 38:
        fn_80208458();
        break;
    case 39:
        fn_802081C0();
        break;
    case 40:
        fn_80208518();
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
            gFEPointerInstances[i]->SetActiveSlide(
                sWaitingSlide, true, false);
        }
        break;
    case 43:
    {
        const char* name = (const char*)Pop();
        Presentation& presentation = Presentation::Instance();
        nlStrNCpy(presentation.mEmissionName, name, 64);
        Function<void*> callback((void (*)(void*))fn_801FF42C);
        fn_802E8B78(EmissionManager::Instance(), &callback);
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
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)Pop());
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
        if (FEModelManager::Instance()->GetModel((const char*)Pop()) == 0)
        {
            StopWithUndo();
        }
        break;
    case 50:
    {
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)Pop());
        if (object == 0 || !object->IsLoaded())
        {
            StopWithUndo();
        }
        break;
    }
    case 51:
        if (fn_80276DE0())
            fn_80276E0C();
        else
            StopWithUndo();
        break;
    default:
        nlBreak();
        break;
    }
}
