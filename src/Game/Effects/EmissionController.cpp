#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Effects/ParticleSystem.h"
#include "Game/Effects/EffectsGroup.h"
#include "Game/PoseAccumulator.h"
#include "Game/SHierarchy.h"
#include "Game/SAnim/pnSAnimController.h"

#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "Game/UnidentifiedStaticStorage.h"

static int numLingeringSystems;

EmissionController::EmissionController(EffectsGroup* pEffectsGroup, EmissionManager* arg5, unsigned short id, void* arg7, int view)
    : m_pGroup(pEffectsGroup)
    , m_pContext(arg7)
    , m_Replaying(false)
    , m_Age(0.0f)
    , m_TimeScale(1.0f)
    , m_ReplayDeltaTime(0.0f)
    , m_bDying(false)
    , m_Id(id)
    , m_bPoseErrorDisplayed(false)
    , m_pManager(arg5)
    , m_View(view)
{
    m_bLingering = m_pGroup->mUnidentified0C != 0;
    m_uUserData = 0;

    InitializeSystemsFromGroup();

    m_fGround = 0.015625f;
    m_aFacing = 0;
    m_vPosition.x = 0.0f;
    m_vPosition.y = 0.0f;
    m_vPosition.z = 0.0f;
    m_vDirection.x = 0.0f;
    m_vDirection.y = 0.0f;
    m_vDirection.z = 1.0f;
    m_vVelocity.x = 0.0f;
    m_vVelocity.y = 0.0f;
    m_vVelocity.z = 0.0f;
    m_pPose = 0;
    m_pAnimController = 0;
    m_uJointIDOverride = 0;
    m_bVisible = true;
    m_bDisabled = false;

    if (m_bLingering)
    {
        numLingeringSystems++;
    }

    m_pManager->KillOldest(numLingeringSystems - 12, true);
}

void EmissionController::InitializeSystemsFromGroup()
{
    EffectsSpec* pSpec = m_pGroup->m_specs;
    EffectsSpec* pEndSpec = pSpec + m_pGroup->m_numSpecs;

    while (pSpec < pEndSpec)
    {
        if (pSpec->m_eAttach == FXBind_Joint && pSpec->m_uJointID == 0xFFFFFFFF)
        {
            pSpec++;
            continue;
        }

        if (pSpec->mUnidentified038 == 0 || pSpec->mUnidentified038 == fxGetTerrain())
        {
            ParticleSystem* pSys = new (nlMalloc(sizeof(ParticleSystem), 8, false))
                ParticleSystem(pSpec->m_pTemplate, &m_pManager->mParticles, pSpec, m_View);

            pSys->m_fDelay = pSpec->m_fDelay;
            m_Systems.AddEnd(pSys);
        }

        pSpec++;
    }

    m_fGround = 0.015625f;
    m_aFacing = 0;
    m_vPosition.x = 0.0f;
    m_vPosition.y = 0.0f;
    m_vPosition.z = 0.0f;
    m_vDirection.x = 0.0f;
    m_vDirection.y = 0.0f;
    m_vDirection.z = 1.0f;
    m_vVelocity.x = 0.0f;
    m_vVelocity.y = 0.0f;
    m_vVelocity.z = 0.0f;
    m_pPose = 0;
    m_pAnimController = 0;
    m_pUserEffects = 0;
    m_nUserEffects = m_pGroup->m_userSpecs;

    if (m_nUserEffects > 0)
    {
        int i;
        UserEffectSpec** pUserSpecs = m_pGroup->GetUserSpecs();
        m_pUserEffects = (UserEffectSpec**)nlMalloc(m_nUserEffects * sizeof(UserEffectSpec*), 8, false);

        for (i = 0; i < m_nUserEffects; i++)
        {
            if (pUserSpecs[i] == 0)
            {
                m_pUserEffects[i] = 0;
            }
            else
            {
                m_pUserEffects[i] = pUserSpecs[i]->Clone();
            }
        }
    }
}

void EmissionController::ClearParticles()
{
    nlDLListIterator<ParticleSystem*> iterator = m_Systems.Begin();
    while (iterator.hasNext())
    {
        ParticleSystem* p = *iterator;
        p->ClearParticles();
        iterator.Step();
    }

    if (mFinishedCallback)
    {
        mFinishedCallback(*this, 0);
    }
}

EmissionController::~EmissionController()
{
    if (mFinishedCallback)
    {
        mFinishedCallback(*this, 2);
        mFinishedCallback.Clear();
    }

    while (!m_Systems.IsEmpty())
    {
        ParticleSystem* pSys;
        m_Systems.RemoveStart(&pSys);
        delete pSys;
    }

    if (m_pUserEffects != 0)
    {
        for (int i = 0; i < m_nUserEffects; i++)
        {
            if (m_pUserEffects[i] != 0)
            {
                delete m_pUserEffects[i];
            }
        }
        delete[] m_pUserEffects;
    }

    if (m_bLingering)
    {
        numLingeringSystems--;
    }
}

void EmissionController::SetPosition(const nlVector3& pos)
{
    m_vPosition = pos;
}

void EmissionController::SetDirection(const nlVector3& dir)
{
    m_vDirection = dir;
}

void EmissionController::SetVelocity(const nlVector3& velocity)
{
    m_vVelocity = velocity;
}

void EmissionController::SetPoseAccumulator(
    const cPoseAccumulator& pose)
{
    m_pPose = &pose;
}

void EmissionController::SetAnimController(
    const cPN_SAnimController& animController)
{
    m_pAnimController = &animController;
}

void EmissionController::Die()
{
    if (m_bDying)
    {
        return;
    }

    m_bDying = true;

    nlDLListIterator<ParticleSystem*> iterator = m_Systems.Begin();
    while (iterator.hasNext())
    {
        ParticleSystem* p = *iterator;
        p->Die();
        iterator.Step();
    }

    if (mFinishedCallback)
    {
        mFinishedCallback(*this, 0);
    }
}

float EmissionController::GetRemainingTime() const
{
    float maxTime = 0.0f;
    nlDLListIterator<ParticleSystem*> node = m_Systems.Begin();

    while (node.hasNext())
    {
        ParticleSystem* system = *node;
        float remainingTime = system->GetRemainingTime();
        if (remainingTime > maxTime)
        {
            maxTime = remainingTime;
        }
        node.Step();
    }

    return maxTime;
}

bool EmissionController::IsLingering() const
{
    return m_bLingering;
}

extern "C" void fn_802E4C78(nlVector3& out, const cPoseAccumulator* pPose,
    u32 uJointID, float fVelocity, float fcurrentTime)
{
    float fsetDistance = fVelocity * fcurrentTime;
    cSHierarchy* pHier = pPose->m_BaseSHierarchy;
    int jointIndex = pHier->GetNodeIndexByID(uJointID);
    int parentIndex = pHier->GetParent(jointIndex);

    while (parentIndex != -1)
    {
        const nlMatrix4& jointMat = pPose->GetNodeMatrix(jointIndex);
        const nlMatrix4& parentMat = pPose->GetNodeMatrix(parentIndex);
        nlVector3 v;
        nlVec3Sub(v, parentMat.GetTranslation(), jointMat.GetTranslation());
        float dist = nlVec3Length(v);

        if (dist >= fsetDistance)
        {
            float ratio = fsetDistance / dist;
            nlVecLerp(out, jointMat.GetTranslation(), parentMat.GetTranslation(), ratio);
            break;
        }

        fsetDistance -= dist;
        jointIndex = parentIndex;
        parentIndex = pHier->GetParent(parentIndex);
    }

    if (parentIndex == -1)
    {
        const nlMatrix4& jointMat = pPose->GetNodeMatrix(jointIndex);
        out = jointMat.GetTranslation();
    }
}

extern "C" bool fn_802E502C(
    EmissionController* controller, ParticleSystem* pSys, int& numSys, float dt)
{
    pSys->m_aFacing = controller->m_aFacing;
    EffectsSpec* pSpec = pSys->m_pSpec;

    if (pSpec->mUnidentified038 != 0 && pSpec->mUnidentified038 != fxGetTerrain())
    {
        return true;
    }

    numSys++;
    pSys->m_uLayer = pSpec->m_uLayer;

    nlVector3 pos = controller->m_vPosition;
    nlVector3 vel = controller->m_vVelocity;
    controller->fn_802E4DF8(*pSpec, pos, vel);
    pSys->m_vPosition = pos;
    pSys->m_vVelocity = vel;
    controller->fn_802E5164(pSpec, pSys);

    pSys->UpdateCoordSys();
    pSys->m_bVisible = controller->m_bVisible;
    return pSys->Update(dt);
}

void EmissionController::fn_802E4DF8(EffectsSpec& spec, nlVector3& pos, nlVector3& vel)
{
    pos = m_vPosition;
    vel = m_vVelocity;

    if (mUnidentified020)
    {
        pos = mUnidentified020(*this, spec);
    }
    else if (spec.m_eAttach == FXBind_Joint || spec.m_eAttach == 3)
    {
        if (spec.m_eJointBinding == JB_Ascend && m_pPose != 0)
        {
            vel.x = 0.0f;
            vel.y = 0.0f;
            vel.z = 0.0f;

            fn_802E4C78(pos, m_pPose,
                m_uJointIDOverride == 0 ? spec.m_uJointID : m_uJointIDOverride,
                spec.m_fJointVelocity, m_Age);
        }
        else if (m_pPose != 0)
        {
            unsigned int jointID = spec.m_uJointID;
            if (m_pAnimController != 0 && m_pAnimController->m_bMirror)
            {
                cSHierarchy* pHier = m_pPose->m_BaseSHierarchy;
                int nodeIndex = pHier->GetNodeIndexByID(jointID);
                jointID = pHier->GetNodeID(pHier->GetMirroredNode(nodeIndex));
            }

            const nlMatrix4& mat = m_pPose->GetNodeMatrixByHashID(
                m_uJointIDOverride == 0 ? jointID : m_uJointIDOverride);
            pos = mat.GetTranslation();
        }
        else if (!m_bPoseErrorDisplayed)
        {
            m_pManager->AddError("No Pose Buffer To Play Effect - playing at default position\n");
            m_bPoseErrorDisplayed = true;
        }
    }

    if (m_pManager->unknown_0x1F8 && spec.m_bGround)
    {
        pos.z = m_fGround + m_pManager->GetUnidentified1F4();
    }
    pos.z += spec.m_fOffset;
}

void EmissionController::fn_802E5164(EffectsSpec* pSpec, ParticleSystem* pSys)
{
    if (pSpec->mUnidentified048 == 0 || m_pPose == 0)
    {
        pSys->m_vForward = m_vDirection;
        return;
    }

    nlVector4 dir;
    switch (pSpec->mUnidentified048)
    {
    case 1:
        nlVec4Set(dir, 1.0f, 0.0f, 0.0f, 0.0f);
        break;
    case 2:
        nlVec4Set(dir, 0.0f, 1.0f, 0.0f, 0.0f);
        break;
    case 3:
        nlVec4Set(dir, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    case 4:
        nlVec4Set(dir, -1.0f, 0.0f, 0.0f, 0.0f);
        break;
    case 5:
        nlVec4Set(dir, 0.0f, -1.0f, 0.0f, 0.0f);
        break;
    case 6:
        nlVec4Set(dir, 0.0f, 0.0f, -1.0f, 0.0f);
        break;
    }

    u32 jointID = pSpec->m_uJointID;
    if (m_pAnimController != 0 && m_pAnimController->m_bMirror)
    {
        cSHierarchy* pHier = m_pPose->m_BaseSHierarchy;
        int nodeIndex = pHier->GetNodeIndexByID(jointID);
        jointID = pHier->GetNodeID(pHier->GetMirroredNode(nodeIndex));
    }

    const nlMatrix4& mat = m_pPose->GetNodeMatrixByHashID(
        m_uJointIDOverride == 0 ? jointID : m_uJointIDOverride);
    nlMultVectorMatrix(dir, mat);
    pSys->m_vForward = *(nlVector3*)&dir;
}

bool EmissionController::Update(float dt)
{
    if (m_bDisabled)
    {
        return true;
    }

    dt *= m_TimeScale;

    int numDel;
    int numSys;
    UserEffectInfo info;
    numSys = 0;
    numDel = 0;

    if (m_Replaying)
    {
        dt = m_ReplayDeltaTime;
    }
    else
    {
        m_Age += dt;
    }

    bool positionChanged = false;
    if (mUpdateCallback)
    {
        nlVector3 oldPosition = m_vPosition;
        mUpdateCallback(*this);
        if (!nlNear(m_vPosition, oldPosition))
        {
            positionChanged = true;
        }
    }

    if (dt <= 0.0f)
    {
        if (positionChanged)
        {
            dt = 0.0f;
        }
        else
        {
            return true;
        }
    }

    nlDLListIterator<ParticleSystem*> iterator = m_Systems.Begin();
    while (iterator.hasNext())
    {
        ParticleSystem* pSys = *iterator;
        if (!fn_802E502C(this, pSys, numSys, dt))
        {
            m_Systems.Remove(&iterator);
            delete pSys;
            numDel++;
        }
        else
        {
            iterator.Step();
        }
    }

    bool isFinished = numSys == numDel;
    if (isFinished && mFinishedCallback)
    {
        mFinishedCallback(*this, 1);
    }

    if (m_nUserEffects > 0)
    {
        info.pv3Position = &m_vPosition;
        info.pv3Direction = &m_vDirection;
        for (int i = 0; i < m_nUserEffects; i++)
        {
            if (m_pUserEffects[i] != 0)
            {
                m_pUserEffects[i]->Update(dt, &info);
                isFinished &= m_pUserEffects[i]->IsFinished();
            }
        }
    }

    return !isFinished;
}

void* fxLoadEntireFileHigh(const char* filename, unsigned long* fileSize)
{
    void* buffer = 0;
    u32 datasize = 0;

    nlFile* file = nlOpen(filename);
    if (file != 0)
    {
        unsigned int size;
        datasize = nlFileSize(file, &size);
        buffer = nlMalloc(size, 0x20, true);
        nlRead(file, buffer, datasize, 0);
        nlClose(file);
    }

    if (fileSize != 0)
    {
        *fileSize = datasize;
    }

    return buffer;
}

int EmissionController::Render()
{
    if (m_bDisabled)
    {
        return 0;
    }

    int numParticles = 0;
    nlDLListIterator<ParticleSystem*> iterator = m_Systems.Begin();

    while (iterator.hasNext())
    {
        ParticleSystem* sys = *iterator;
        GLView* view = (GLView*)m_pContext;
        numParticles += sys->m_bVisible ? sys->RenderAllParticles(view) : 0;
        iterator.Step();
    }

    if (m_nUserEffects > 0)
    {
        UserEffectInfo info;
        info.pv3Position = &m_vPosition;
        info.pv3Direction = &m_vDirection;

        for (int i = 0; i < m_nUserEffects; i++)
        {
            if (m_pUserEffects[i] != 0 && !m_pUserEffects[i]->IsFinished())
            {
                m_pUserEffects[i]->Render(&info, (GLView*)m_pContext);
            }
        }
    }

    return numParticles;
}

void EmissionController::SetUpdateCallback(
    const Function1<void, EmissionController&>& callback)
{
    mUpdateCallback = callback;
}

void EmissionController::SetFinishedCallback(
    const Function2<void, EmissionController&, int>& callback)
{
    mFinishedCallback = callback;
}

float EmissionController::GetBoundingRadius() const
{
    float maxTime = 0.0f;
    nlDLListIterator<ParticleSystem*> node = m_Systems.Begin();

    while (node.hasNext())
    {
        ParticleSystem* system = *node;
        float remainingTime = system->m_pTemplate->GetBoundingRadius();
        if (remainingTime > maxTime)
        {
            maxTime = remainingTime;
        }
        node.Step();
    }

    return maxTime;
}
