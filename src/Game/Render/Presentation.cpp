#include "Game/Render/Presentation.h"

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

static char sPresentationByteCode[] = "art/Scripts/fe_presentation.byte_code";
static char sBronzeFormat[] = "%sbronze";
static char sSilverFormat[] = "%ssilver";
static char sGoldFormat[] = "%sgold";
static char sWaitingSlide[] = "waiting";
static char sIdleFunctionName[] = "Idle";
static const char* idleFun = sIdleFunctionName;
static char sIdleAnimation[] = "fe_idle";

extern "C" void fn_8010E294(void*);
extern "C" void fn_801C27C4(
    void*, int, unsigned int, int, bool, int, int, int);
extern "C" void fn_801C3014(void*);
extern "C" void* fn_801C2FB4(void*, unsigned int);
extern "C" void* fn_801C2798(void*, unsigned int);
extern "C" void fn_801C2BD8(void*, void*);
extern "C" bool fn_801C08C4(void*, unsigned int);
extern "C" bool fn_801C05B0(void*);
extern "C" bool fn_801C05C4(void*);
extern "C" void fn_801C05D0(void*, void*);
extern "C" void fn_801C08A0(void*, void*);
extern "C" void fn_801C08BC(void*, unsigned int);
extern "C" void fn_801C0898(void*, void (*)(void*));
extern "C" void fn_801C0704(
    void*, const char*, unsigned int, float, float, unsigned int);
extern "C" void fn_801CBCA0(unsigned int, int, int, bool);
extern "C" void fn_801FEA18(const char*, void (*)(), float, bool);
extern "C" void fn_801FEB74(void (*)(), float);
extern "C" void fn_802081C0();
extern "C" void fn_80208458();
extern "C" void fn_80208518();
extern "C" void fn_80208594();
extern "C" void fn_802092D0(bool);
class TU80252180Scene;
extern "C" void fn_80253474(TU80252180Scene* scene);
extern "C" TU80252180Scene* fn_80253E18();
extern "C" void fn_80254E3C(TU80252180Scene* scene);
extern "C" unsigned int fn_80273B00();
extern "C" void fn_80276D10();
extern "C" bool fn_80276DE0();
extern "C" void fn_80276E0C();
extern "C" void fn_80277BB4(unsigned int, unsigned int, unsigned int);
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" void fn_802E8A2C(EmissionManager*, EffectsGroup*);
extern "C" void fn_802E8B78(EmissionManager*, Function<void*>*);
extern "C" void fn_80341E68(BasicStadium*, unsigned int);
extern "C" void fn_802DEDE8(InterpreterCore*);

extern "C" void* lbl_806E17F0;
extern "C" void* lbl_806E0F90;
extern bool g_e3_Build;
extern "C" bool lbl_806DC704;
extern "C" bool lbl_806E0F8B;
extern "C" TLComponentInstance* lbl_80578450[4];

class PresentationInterface_806E1838
{
public:
    virtual ~PresentationInterface_806E1838();
    virtual void Invoke(int, int, int) = 0;
};

class PresentationLookupResult
{
public:
    virtual ~PresentationLookupResult();
    virtual void UnidentifiedVirtual1() = 0;
    virtual void* GetValue() = 0;
};

extern "C" PresentationInterface_806E1838* lbl_806E1838;

inline Presentation::Presentation()
    : InterpreterCore(100)
    , mWaitTime(0.0f)
    , mDeltaTime(0.0f)
    , mCameraFinished(false)
    , mPresentationFinished(false)
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

extern "C" void fn_801FEBEC()
{
    Presentation::Instance().mCameraFinished = true;
}

extern "C" void fn_801FED4C()
{
    Presentation::Instance().mPresentationFinished = true;
}

extern "C" Presentation* fn_801FEEAC()
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
        mPresentationFinished = false;

        const char* functionName = idleFun;
        mWaitTime = 0.0f;
        mCameraFinished = false;
        mPresentationFinished = false;
        nlStrNCpy(mCurrentFunction, functionName, 64);
        fn_802DEDE8(this);
        CallFunction(nlStringHash(functionName));
    }
}

extern "C" bool fn_801FF168(Presentation* presentation)
{
    return nlStrCmp<char>(idleFun, presentation->mCurrentFunction) != 0;
}

void Presentation::Call(const char* functionName)
{
    mCameraFinished = false;
    mWaitTime = 0.0f;
    mPresentationFinished = false;
    nlStrNCpy(mCurrentFunction, functionName, 64);
    fn_802DEDE8(this);
    CallFunction(nlStringHash(functionName));
}

extern "C" void fn_801FF284(void* object)
{
    if (object != 0)
    {
        fn_801C0704(object, sIdleAnimation, 0, 0.2f, 0.0f, 0);
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
            side = *(int*)((char*)lbl_806E0F90 + 0x8A28);
        }
        else
        {
            int captain = GameInfoManager::Instance()->GetTeam((short)side);
            int opponent = GameInfoManager::Instance()->GetTeam((short)!side);
            alternate = CaptainsNeedAlternateColour(captain, opponent);
        }
        fn_801C27C4(lbl_806E17F0, 1, name, side, value != 0, 0, 0, alternate);
        break;
    }
    case 2:
        fn_801C3014(lbl_806E17F0);
        break;
    case 3:
        NetTournManager::Instance()->DestroyTournamentTrophy();
        break;
    case 4:
        fn_8010E294(lbl_806E0F90);
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
        void* object = fn_801C2FB4(lbl_806E17F0, original);
        m_SP[-1] = object != 0 && fn_801C08C4(object, value)
                && !fn_801C05B0(object);
        if (m_RunState == 3)
        {
            m_SP[-1] = original;
        }
        break;
    }
    case 11:
        *m_SP++ = fn_80273B00();
        break;
    case 12:
    {
        const char* name = (const char*)Pop();
        EmissionManager* manager = EmissionManager::Instance();
        EffectsGroup* group = fn_802E7CDC(manager, name);
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
        camera->m_EndOfAnimationCallback = fn_801FEBEC;
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
        void* object = fn_801C2FB4(lbl_806E17F0,
            (unsigned int)objectName);
        if (object != 0)
        {
            fn_801C0704(object, argument0, argument1, 0.0f, value, 0);
        }
        break;
    }
    case 18:
    {
        unsigned int argument1 = Pop();
        const char* argument0 = (const char*)Pop();
        const char* objectName = (const char*)Pop();
        void* object = fn_801C2FB4(lbl_806E17F0,
            (unsigned int)objectName);
        if (object != 0)
        {
            fn_801C0704(object, argument0, argument1, 0.0f, 0.0f, 0);
        }
        break;
    }
    case 19:
        fn_801CBCA0(0xB60A9CC0, 0, 0, true);
        break;
    case 20:
        fn_801CBCA0(Pop(), 0, 0, true);
        break;
    case 21:
        fn_801FEB74(fn_801FED4C, *(float*)&m_SP[-1]);
        --m_SP;
        mPresentationFinished = false;
        break;
    case 22:
    {
        unsigned int childName = Pop();
        unsigned int objectName = Pop();
        void* object = fn_801C2FB4(lbl_806E17F0, objectName);
        if (object != 0)
        {
            PresentationLookupResult* child = (PresentationLookupResult*)fn_801C2798(
                lbl_806E17F0, childName);
            if (child != 0)
            {
                fn_801C05D0(object, child->GetValue());
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
        fn_801FEA18(name, fn_801FED4C, duration, value);
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        camera->m_bCyclic = false;
        camera->m_EndOfAnimationCallback = fn_801FEBEC;
        mCameraFinished = false;
        mPresentationFinished = false;
        break;
    }
    case 24:
    {
        bool value = Pop() != 0;
        float duration = *(float*)&m_SP[-1];
        --m_SP;
        const char* baseName = (const char*)Pop();
        char name[64];
        typedef int (*GetPresentationMode)(void*);
        void** vtable = *(void***)lbl_806E0F90;
        int mode = ((GetPresentationMode)vtable[8])(lbl_806E0F90);
        if (mode == 0)
            nlSNPrintf(name, sizeof(name), sBronzeFormat, baseName);
        else if (mode == 1)
            nlSNPrintf(name, sizeof(name), sSilverFormat, baseName);
        else
            nlSNPrintf(name, sizeof(name), sGoldFormat, baseName);
        fn_801FEA18(name, fn_801FED4C, duration, value);
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        camera->m_bCyclic = false;
        camera->m_EndOfAnimationCallback = fn_801FEBEC;
        mCameraFinished = false;
        mPresentationFinished = false;
        break;
    }
    case 25:
    {
        int value1 = (int)Pop();
        int value0 = (int)Pop();
        lbl_806E1838->Invoke(value0, value1, 0);
        break;
    }
    case 26:
        lbl_806E1838->Invoke(8, 1, 0);
        break;
    case 27:
    {
        void* object = fn_801C2FB4(lbl_806E17F0, Pop());
        if (object != 0)
        {
            fn_801C2BD8(lbl_806E17F0, object);
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
        void* object = fn_801C2FB4(lbl_806E17F0, Pop());
        if (object != 0)
        {
            fn_801C08BC(object, value);
        }
        break;
    }
    case 31:
    {
        unsigned int childName = Pop();
        void* object = fn_801C2FB4(lbl_806E17F0, Pop());
        if (object != 0)
        {
            PresentationLookupResult* child = (PresentationLookupResult*)fn_801C2798(
                lbl_806E17F0, childName);
            if (child != 0)
            {
                fn_801C08A0(object, (char*)child->GetValue() + 0x30);
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
        void* object = fn_801C2FB4(lbl_806E17F0, Pop());
        if (object != 0)
        {
            *((bool*)object + 0x58) = value;
        }
        break;
    }
    case 34:
    {
        void* object = fn_801C2FB4(lbl_806E17F0, Pop());
        if (object != 0)
        {
            fn_801C0898(object, fn_801FF284);
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
        if (TU80252180Scene* scene = fn_80253E18())
        {
            fn_80254E3C(scene);
        }
        break;
    case 42:
        FEMusic::StartStreamIfDifferent(1);
        if (TU80252180Scene* scene = fn_80253E18())
        {
            fn_80253474(scene);
        }
        for (int i = 0; i < 4; ++i)
        {
            lbl_80578450[i]->SetActiveSlide(
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
        void* object = fn_801C2FB4(lbl_806E17F0, Pop());
        if (object != 0 && !fn_801C05B0(object))
        {
            StopWithUndo();
        }
        break;
    }
    case 46:
        if (!mPresentationFinished)
            StopWithUndo();
        else
            mPresentationFinished = false;
        break;
    case 47:
        if (lbl_806DC704 && lbl_806E0F8B)
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
        if (fn_801C2FB4(lbl_806E17F0, Pop()) == 0)
        {
            StopWithUndo();
        }
        break;
    case 50:
    {
        void* object = fn_801C2FB4(lbl_806E17F0, Pop());
        if (object == 0 || !fn_801C05C4(object))
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
