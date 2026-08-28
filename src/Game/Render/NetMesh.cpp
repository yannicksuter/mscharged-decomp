#include "Game/Render/NetMesh.h"

#include "Game/Ball.h"
#include "Game/Drawable/RenderObject.h"
#include "Game/Physics/NetMeshModelLoader.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlSlotPool.h"

#include <math.h>
#include <string.h>

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

struct DebugTypeState
{
    unsigned short type;
    unsigned short padding;
};

class NetMeshFrameProvider
{
public:
    virtual void V0();
    virtual void V1();
    virtual void V2();
    virtual void V3();
    virtual void V4();
    virtual void V5();
    virtual void V6();
    virtual void V7();
    virtual void V8();
    virtual void V9();
    virtual void V10();
    virtual void V11();
    virtual void V12();
    virtual unsigned int GetFrame();
};

struct NetMeshGameState
{
    u8 padding[0xAC];
    nlVector3 upVector;
};

struct BallNetmeshEventData
{
    NetMesh* netMesh;
    nlVector3 collisionVelocity;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);
extern "C" unsigned int fn_802AAC88(const void*, unsigned int);
extern "C" void* fn_80338950(void*);
extern "C" void fn_8033919C(void*, const char*);
extern "C" NetMeshFrameProvider* fn_8011166C();
extern "C" RenderObject* fn_8027725C(unsigned long);
extern "C" void fn_80146424(BallNetmeshEventData*, bool);
extern "C" PlatTexture* fn_802D064C(unsigned long);

extern void* lbl_806E2168;
extern NetMeshGameState* lbl_806E0C94;
extern float lbl_806DC7B8;
extern SlotPool<BallNetmeshEventData> lbl_80571780;

DebugTypeState s_DetMeshType = { 0xFFFF, 0 };
float NetMesh::s_fReboundForceCoefficient = 6.0f;
float NetMesh::s_fVelocityDampingCoefficient = 0.7f;
float NetMesh::s_fBallRadiusExaggerationFactor = 2.0f;
float NetMesh::s_fBallRadiusExaggerationFactor2 = 4.0f;
float NetMesh::s_fNetGravityMagnitude = 10.0f;
float NetMesh::s_fLooseness = -0.02f;
int NetMesh::s_NumConstraintIterations = 1;
float NetMesh::s_fInactivityThreshold = 0.01f;
float NetMesh::s_fIsBallMovingThreshold = 0.01f;
float NetMesh::s_fDampening = 1.0f;
float NetMesh::s_fNetStretchLimit = 1.0f;
float s_fNetMeshLogTimeScale = 2.5f;
DebugTypeState s_GenDetNetMeshType = { 0xFFFF, 0 };
static float fDeltaZero = 0.001f;
static float fGroundHeight = 0.01f;
char sJoltName[] = "mfJolt";
char sMaxYName[] = "mfMaxY";
char sMinYName[] = "mfMinY";
char sMaxXName[] = "mfMaxX";
char sMinXName[] = "mfMinX";
char sDetMeshName[] = "DetMesh";

bool NetMesh::s_bAnimatedNetMeshEnabled = false;
bool NetMesh::s_bAlwaysActive = false;
unsigned long NetMesh::sNetTextureHandle = 0;
bool NetMesh::s_bUseStretchLimit = false;
NetMesh* NetMesh::spPositiveXNetMesh = 0;
NetMesh* NetMesh::spNegativeXNetMesh = 0;
bool NetMesh::sbDontUseLowestNetTextureLOD = false;

char sResetFormat[] = "NetMesh Reset %d %d at %d\n";
char sUpdateFormat[] = "NetMesh Upd %d ts %f af %d fu %d at %d\n";
char sRestPositionCRCName[] = "m_nRestPositionCRC";
char sAccelCRCName[] = "m_nAccelCRC";
char sPrevPosCRCName[] = "m_nPrevPosCRC";
char sPosCRCName[] = "m_nPosCRC";
char sGenDetNetMeshName[] = "GenDetNetMesh";
char sMotionName[] = "mfMotion";
char sBallInsideName[] = "mbBallIsInsideNet";
char sActiveName[] = "mbIsActive";
char sPenetrationNormalName[] = "m_v3BallPenetrationNormal";
char sPenetratingFixedName[] = "m_bPenetratingFixedParticle";
char sPenetrationDepthName[] = "m_fBallPenetrationDepth";
char sAffectedParticlesName[] = "m_numAffectedParticles";
char sNumDistanceConstraintsName[] = "m_NumDistanceConstraints";
char sNumPositionConstraintsName[] = "m_NumPositionConstraints";
char sNumParticlesName[] = "m_NumParticles";
char sPositiveEndName[] = "mbPositiveEnd";

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

NetMesh::NetMesh(bool positiveEnd)
{
    m_iClosestParticle = 0;
    mbInitialized = false;
    mbFirstUpdate = true;
    mbUsePhysicsBall = false;
    mbRelaxing = false;
    m_v3Position = 0;
    m_v3PrevPosition = 0;
    m_v3Accel = 0;
    m_v3RestPosition = 0;
    m_bIsParticleFixed = 0;
    m_TriStripIndices = 0;
    m_NumTriStripIndices = 0;
    m_v2TextureCoords = 0;
    m_v3Normal = 0;
    mNetMeshDrawableObjectID = 0;
    m_aDistanceConstraints = 0;
    m_aPositionConstraints = 0;
    m_NumParticles = 0;
    mfMinX = 0.0f;
    mfMaxX = 0.0f;
    mfMinY = 0.0f;
    mfMaxY = 0.0f;
    m_NumPositionConstraints = 0;
    m_NumDistanceConstraints = 0;
    mbIsActive = true;
    m_numAffectedParticles = 0;
    m_fBallPenetrationDepth = 0.0f;
    m_bPenetratingFixedParticle = false;
    m_v3BallPenetrationNormal.x = 0.0f;
    m_v3BallPenetrationNormal.y = 0.0f;
    m_v3BallPenetrationNormal.z = 0.0f;
    mbBallIsInsideNet = false;
    mfMotion = 0.0f;
    mJolt = 0.0f;
    mbPositiveEnd = positiveEnd;

    if (positiveEnd != 0)
    {
        spPositiveXNetMesh = this;
    }
    if (positiveEnd == 0)
    {
        spNegativeXNetMesh = this;
    }
}

void NetMesh::Allocate(
    int numParticles, int numDistanceConstraints, int numPositionConstraints)
{
    unsigned long vectorBytes = numParticles * sizeof(nlVector3);
    m_v3Position = (nlVector3*)nlMalloc(vectorBytes, 8, false);
    m_v3PrevPosition = (nlVector3*)nlMalloc(vectorBytes, 8, false);
    m_v3RestPosition = (nlVector3*)nlMalloc(vectorBytes, 8, false);
    m_v3Accel = (nlVector3*)nlMalloc(vectorBytes, 8, false);
    m_v3Normal = (nlVector3*)nlMalloc(vectorBytes, 8, false);
    m_v2TextureCoords = (shortVector2*)nlMalloc(
        numParticles * sizeof(shortVector2), 8, false);
    m_bIsParticleFixed = (bool*)nlMalloc(numParticles, 8, false);
    m_aDistanceConstraints = (cDistanceConstraint*)nlMalloc(
        numDistanceConstraints * sizeof(cDistanceConstraint), 8, false);
    m_aPositionConstraints = (cPositionConstraint*)nlMalloc(
        numPositionConstraints * sizeof(cPositionConstraint), 8, false);

    for (int i = 0; i < numParticles; ++i)
    {
        m_bIsParticleFixed[i] = false;
    }
}

NetMesh::~NetMesh()
{
    delete[] m_v3Position;
    delete[] m_v3PrevPosition;
    delete[] m_v3RestPosition;
    delete[] m_v3Accel;
    delete[] m_v3Normal;
    delete[] m_v2TextureCoords;
    delete[] m_TriStripIndices;
    delete[] m_aDistanceConstraints;
    delete[] m_aPositionConstraints;
    delete[] m_bIsParticleFixed;
}

int NetMesh::SetPositionConstraint(
    int particleIndex, const nlVector3& v3Position)
{
    m_aPositionConstraints[m_NumPositionConstraints].nParticle = particleIndex;
    m_aPositionConstraints[m_NumPositionConstraints].v3Position = v3Position;
    ++m_NumPositionConstraints;
    m_bIsParticleFixed[particleIndex] = true;
    return m_NumPositionConstraints - 1;
}

void NetMesh::SetDistanceConstraint(
    int nParticleA, int nParticleB, float fDistance)
{
    m_aDistanceConstraints[m_NumDistanceConstraints].nParticleA = nParticleA;
    m_aDistanceConstraints[m_NumDistanceConstraints].nParticleB = nParticleB;
    m_aDistanceConstraints[m_NumDistanceConstraints].fDistance = fDistance;
    ++m_NumDistanceConstraints;
}

void NetMesh::Reset(bool usePhysicsBall)
{
    if (this != 0)
    {
        mbUsePhysicsBall = usePhysicsBall;
        for (int i = 0; i < m_NumParticles; ++i)
        {
            m_v3Position[i] = m_v3RestPosition[i];
            m_v3PrevPosition[i] = m_v3RestPosition[i];
            nlVec3Set(m_v3Accel[i], 0.0f, 0.0f, 0.0f);
        }

        mbIsActive = false;
        m_numAffectedParticles = 0;
        m_fBallPenetrationDepth = 0.0f;
        m_bPenetratingFixedParticle = false;
        nlVec3Set(m_v3BallPenetrationNormal, 0.0f, 0.0f, 0.0f);
        mbBallIsInsideNet = false;
        mfMotion = 0.0f;
        mJolt = 0.0f;

        void* output = fn_80338950(lbl_806E2168);
        if (output != 0)
        {
            NetMeshFrameProvider* frameProvider = fn_8011166C();
            unsigned int frame = frameProvider->GetFrame();
            char buffer[256];
            nlSNPrintf(buffer, sizeof(buffer), sResetFormat, mbPositiveEnd, mbUsePhysicsBall, frame);
            fn_8033919C(output, buffer);
        }
    }
}

void NetMesh::SyncLog(void* context, DebugWriteCache* cache)
{
    struct NetMeshCRCs
    {
        unsigned int position;
        unsigned int previousPosition;
        unsigned int accel;
        unsigned int restPosition;
    } crcs;

    crcs.position = fn_802AAC88(
        m_v3Position, m_NumParticles * sizeof(nlVector3));
    crcs.previousPosition = fn_802AAC88(
        m_v3PrevPosition, m_NumParticles * sizeof(nlVector3));
    crcs.accel = fn_802AAC88(
        m_v3Accel, m_NumParticles * sizeof(nlVector3));
    crcs.restPosition = fn_802AAC88(
        m_v3RestPosition, m_NumParticles * sizeof(nlVector3));

    if (s_GenDetNetMeshType.type == 0xFFFF)
    {
        s_GenDetNetMeshType.type = fn_80338EBC(cache, sGenDetNetMeshName);
        fn_80338F88(cache, 2, lbl_80533C98[2].size, 0, sPosCRCName);
        fn_80338F88(cache, 2, lbl_80533C98[2].size, (unsigned char*)&crcs.previousPosition - (unsigned char*)&crcs, sPrevPosCRCName);
        fn_80338F88(cache, 2, lbl_80533C98[2].size, (unsigned char*)&crcs.accel - (unsigned char*)&crcs, sAccelCRCName);
        fn_80338F88(cache, 2, lbl_80533C98[2].size, (unsigned char*)&crcs.restPosition - (unsigned char*)&crcs, sRestPositionCRCName);
        fn_80338F78(cache);
    }

    fn_80339450(cache, s_GenDetNetMeshType.type, &crcs, context);
    fn_8033930C(cache, s_GenDetNetMeshType.type, &crcs, sizeof(crcs));

    if (s_DetMeshType.type == 0xFFFF)
    {
        s_DetMeshType.type = fn_80338EBC(cache, sDetMeshName);
        fn_80338F88(cache, 16, lbl_80533C98[16].size, 0, sPositiveEndName);
        fn_80338F88(cache, 8, lbl_80533C98[8].size, (unsigned char*)&m_NumParticles - (unsigned char*)&mbPositiveEnd, sNumParticlesName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, (unsigned char*)&mfMinX - (unsigned char*)&mbPositiveEnd, sMinXName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, (unsigned char*)&mfMaxX - (unsigned char*)&mbPositiveEnd, sMaxXName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, (unsigned char*)&mfMinY - (unsigned char*)&mbPositiveEnd, sMinYName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, (unsigned char*)&mfMaxY - (unsigned char*)&mbPositiveEnd, sMaxYName);
        fn_80338F88(cache, 8, lbl_80533C98[8].size, (unsigned char*)&m_NumPositionConstraints - (unsigned char*)&mbPositiveEnd, sNumPositionConstraintsName);
        fn_80338F88(cache, 8, lbl_80533C98[8].size, (unsigned char*)&m_NumDistanceConstraints - (unsigned char*)&mbPositiveEnd, sNumDistanceConstraintsName);
        fn_80338F88(cache, 8, lbl_80533C98[8].size, (unsigned char*)&m_numAffectedParticles - (unsigned char*)&mbPositiveEnd, sAffectedParticlesName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, (unsigned char*)&m_fBallPenetrationDepth - (unsigned char*)&mbPositiveEnd, sPenetrationDepthName);
        fn_80338F88(cache, 16, lbl_80533C98[16].size, (unsigned char*)&m_bPenetratingFixedParticle - (unsigned char*)&mbPositiveEnd, sPenetratingFixedName);
        fn_80338F88(cache, 22, lbl_80533C98[22].size, (unsigned char*)&m_v3BallPenetrationNormal - (unsigned char*)&mbPositiveEnd, sPenetrationNormalName);
        fn_80338F88(cache, 16, lbl_80533C98[16].size, (unsigned char*)&mbIsActive - (unsigned char*)&mbPositiveEnd, sActiveName);
        fn_80338F88(cache, 16, lbl_80533C98[16].size, (unsigned char*)&mbBallIsInsideNet - (unsigned char*)&mbPositiveEnd, sBallInsideName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, (unsigned char*)&mfMotion - (unsigned char*)&mbPositiveEnd, sMotionName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, (unsigned char*)&mJolt - (unsigned char*)&mbPositiveEnd, sJoltName);
        fn_80338F78(cache);
    }

    fn_80339450(cache, s_DetMeshType.type, &mbPositiveEnd, context);
    fn_8033930C(cache, s_DetMeshType.type, &mbPositiveEnd, 0x44);
}

inline static void AccumForces(NetMesh* self, nlVector3& newPos)
{
    nlVector3* upVector = &lbl_806E0C94->upVector;
    float gravityMagnitude = -NetMesh::s_fNetGravityMagnitude;
    nlVec3Set(newPos, gravityMagnitude * upVector->x, gravityMagnitude * upVector->y, gravityMagnitude * upVector->z);

    for (int i = 0; i < self->m_NumParticles; ++i)
    {
        self->m_v3Accel[i] = newPos;
    }
}

inline static void Integrate(NetMesh* self, float t, nlVector3& v3Temp)
{
    for (int i = 0; i < self->m_NumParticles; ++i)
    {
        nlVector3& v3Pos = self->m_v3Position[i];
        nlVector3& v3PrevPos = self->m_v3PrevPosition[i];
        nlVector3& v3Accel = self->m_v3Accel[i];
        v3Temp = v3Pos;
        v3Pos.x = v3Pos.x
                + (NetMesh::s_fDampening * (v3Pos.x - v3PrevPos.x)
                    + t * (v3Accel.x * t));
        v3Pos.y = v3Pos.y
                + (NetMesh::s_fDampening * (v3Pos.y - v3PrevPos.y)
                    + t * (v3Accel.y * t));
        v3Pos.z = v3Pos.z
                + (NetMesh::s_fDampening * (v3Pos.z - v3PrevPos.z)
                    + t * (v3Accel.z * t));
        v3PrevPos = v3Temp;
    }
}

inline static void ComputeMotion(NetMesh* self)
{
    for (int i = 0; i < self->m_NumParticles; ++i)
    {
        nlVector3& v3Pos = self->m_v3Position[i];
        nlVector3& v3PrevPos = self->m_v3PrevPosition[i];
        float motion = ((float)fabs(v3PrevPos.x - v3Pos.x)
                           + (float)fabs(v3PrevPos.y - v3Pos.y))
                     + (float)fabs(v3PrevPos.z - v3Pos.z);
        if (motion > self->mfMotion)
        {
            self->mfMotion = motion;
        }
    }
}

inline static bool IsBallMoving(PhysicsSphere* sphere)
{
    nlVector3 vel;
    sphere->GetLinearVelocity(&vel);
    return nlVec3LengthSquared(vel)
         > NetMesh::s_fIsBallMovingThreshold;
}

void NetMesh::Update(float dt, const nlVector3& ballPosition,
    const nlVector3& ballPrevPosition, bool bExaggerateBallSize,
    PhysicsSphere* sphere)
{
    nlVector3 newPos;
    nlVector3 oldPos;
    bool noSphere = false;
    if (sphere == 0)
    {
        noSphere = true;
    }

    if (!mbRelaxing)
    {
        if (mbUsePhysicsBall)
        {
            if (!noSphere)
                return;
        }
        else
        {
            if (noSphere)
            {
                return;
            }
        }
    }
    else
    {
        noSphere = false;
    }

    float logTimeScale = dt * s_fNetMeshLogTimeScale;
    bool appliedForces = false;
    if (mbIsActive || s_bAlwaysActive)
    {
        appliedForces = true;
        AddForcesToBall(ballPosition, sphere);
        AccumForces(this, newPos);
        mfMotion = 0.0f;
        Integrate(this, dt, oldPos);
        SatisfyConstraints(ballPosition, bExaggerateBallSize);
        ComputeMotion(this);
    }
    else
    {
        m_numAffectedParticles = 0;
    }

    if (!mbFirstUpdate)
    {
        UpdateActivity(ballPosition, ballPrevPosition, sphere);
    }

    if (!mbRelaxing && !noSphere)
    {
        void* output = fn_80338950(lbl_806E2168);
        if (output != 0)
        {
            NetMeshFrameProvider* frameProvider = fn_8011166C();
            unsigned int frame = frameProvider->GetFrame();
            char buffer[256];
            nlSNPrintf(buffer, sizeof(buffer), sUpdateFormat, mbPositiveEnd, logTimeScale, appliedForces, mbFirstUpdate, frame);
            fn_8033919C(output, buffer);
        }
    }

    mbFirstUpdate = false;
}

void NetMesh::JoltNet()
{
}

void NetMesh::SatisfyConstraints(
    const nlVector3& ballPosition, bool bExaggerateBallSize)
{
    for (int j = 0; j < s_NumConstraintIterations; ++j)
    {
        int i;
        for (i = 0; i < m_NumDistanceConstraints; ++i)
        {
            cDistanceConstraint& c = m_aDistanceConstraints[i];
            nlVector3& x1 = m_v3Position[c.nParticleA];
            nlVector3& x2 = m_v3Position[c.nParticleB];

            nlVector3 d;
            nlVec3Sub(d, x1, x2);
            float dy = d.y;
            float dyy = dy * dy;
            float dx = d.x;
            float dz = d.z;
            float length = nlSqrt(dyy + (dx * dx) + (dz * dz), true);

            if ((float)fabs(length) > fDeltaZero)
            {
                float restLength = c.fDistance * (1.0f + s_fLooseness);
                float diff = (length - restLength) / length;
                float halfDiff = 0.5f * diff;

                d.x *= halfDiff;
                d.y *= halfDiff;
                d.z *= halfDiff;

                x1.x -= d.x;
                x1.y -= d.y;
                x1.z -= d.z;

                x2.x += d.x;
                x2.y += d.y;
                x2.z += d.z;
            }
        }

        m_fBallPenetrationDepth = 0.0f;
        m_bPenetratingFixedParticle = false;

        if (mbBallIsInsideNet)
        {
            int numParticlesAffected = 0;
            int iClosestParticle;
            float closestParticleDistSq;

            for (i = 0; i < m_NumParticles; ++i)
            {
                if (!m_bPenetratingFixedParticle || m_bIsParticleFixed[i])
                {
                    nlVector3& particlePosition = m_v3Position[i];
                    const nlVector3& particleNormal = m_v3Normal[i];

                    float radius = s_fBallRadiusExaggerationFactor * g_pBall->m_pPhysicsBall->GetRadius();

                    if (bExaggerateBallSize)
                    {
                        radius = s_fBallRadiusExaggerationFactor2 * g_pBall->m_pPhysicsBall->GetRadius();
                    }

                    const float ballY = ballPosition.y;
                    const float particleY = particlePosition.y;
                    float dy = ballY - particleY;
                    float dx = ballPosition.x - particlePosition.x;
                    float dz = ballPosition.z - particlePosition.z;
                    float dot = (dx * particleNormal.x)
                              + (dy * particleNormal.y)
                              + (dz * particleNormal.z);
                    nlVector3 perp;
                    nlVec3Set(perp, (-dot * particleNormal.x) + dx, (-dot * particleNormal.y) + dy, (-dot * particleNormal.z) + dz);
                    float perpDistSq = nlVec3LengthSquared(perp);

                    if ((perpDistSq < closestParticleDistSq) || (i == 0))
                    {
                        closestParticleDistSq = perpDistSq;
                        iClosestParticle = i;
                    }

                    float radiusSq = radius * radius;
                    if (perpDistSq < (4.0f * radiusSq))
                    {
                        nlVector3 pointOnOutsideOfBall = ballPosition;
                        nlVec3ScaleAdd(pointOnOutsideOfBall, radius, particleNormal, pointOnOutsideOfBall);
                        nlVec3Sub(
                            perp, pointOnOutsideOfBall, particlePosition);

                        numParticlesAffected++;

                        float falloffFactor = 1.0f;
                        if (perpDistSq > radiusSq)
                        {
                            float dist = nlSqrt(perpDistSq, false);
                            falloffFactor = 1.0f - ((dist - radius) / radius);
                        }

                        float penetration
                            = nlVec3DotProduct(perp, particleNormal);

                        if (penetration > 0.0f)
                        {
                            if (penetration > m_fBallPenetrationDepth)
                            {
                                m_fBallPenetrationDepth = penetration;
                                m_v3BallPenetrationNormal = particleNormal;
                            }

                            float displacementMag = penetration * falloffFactor;
                            nlVec3ScaleAdd(particlePosition, displacementMag, particleNormal, particlePosition);
                        }
                    }
                }
            }

            m_iClosestParticle = iClosestParticle;
            m_numAffectedParticles = numParticlesAffected;

            if (mbBallIsInsideNet && m_numAffectedParticles == 0)
            {
                nlVector3 center;
                center.x = 0.5f * (mfMinX + mfMaxX);
                center.y = 0.5f * (mfMinY + mfMaxY);
                center.z = 0.0f;
                nlVector3 displacement;
                nlVec3Sub(displacement, ballPosition, center);
                m_fBallPenetrationDepth = nlVec3Length(displacement);
            }
        }

        for (i = 0; i < m_NumPositionConstraints; ++i)
        {
            cPositionConstraint& c = m_aPositionConstraints[i];
            int particle = c.nParticle;
            nlVector3& x = m_v3Position[particle];
            nlVec3Set(x, c.v3Position.x, c.v3Position.y, c.v3Position.z);
        }

        {
            const float groundHeight = fGroundHeight;
            for (i = 0; i < m_NumParticles; ++i)
            {
                float height = m_v3Position[i].z;
                if (height < groundHeight)
                {
                    m_v3Position[i].z = groundHeight;
                }
            }
        }
    }
}

void NetMesh::AddForcesToBall(
    const nlVector3& position, PhysicsSphere* sphere)
{
    nlVector3 vel;
    float forceMagnitude;
    nlVector3 force;
    nlVector3 velocity;
    nlVector3 crossProduct;
    nlVector3 currentAngularVelocity;

    if (m_fBallPenetrationDepth > 0.0f)
    {
        if (s_bUseStretchLimit && sphere
            && m_fBallPenetrationDepth > s_fNetStretchLimit)
        {
            nlVector3& v3LinearVelocity = sphere->GetLinearVelocity();

            forceMagnitude
                = (m_v3BallPenetrationNormal.x * v3LinearVelocity.x)
                + (m_v3BallPenetrationNormal.y * v3LinearVelocity.y)
                + (m_v3BallPenetrationNormal.z * v3LinearVelocity.z);

            nlVec3Set(vel,
                forceMagnitude * m_v3BallPenetrationNormal.x,
                forceMagnitude * m_v3BallPenetrationNormal.y,
                forceMagnitude * m_v3BallPenetrationNormal.z);

            nlVector3& v3CurrentVelocity = sphere->GetLinearVelocity();
            nlVec3Set(vel, v3CurrentVelocity.x - vel.x, v3CurrentVelocity.y - vel.y, v3CurrentVelocity.z - vel.z);

            sphere->SetLinearVelocity(vel);
        }

        float forceMagnitude
            = -(m_fBallPenetrationDepth * s_fReboundForceCoefficient);
        force = m_v3BallPenetrationNormal;
        nlVec3Scale(force, force, forceMagnitude);

        if (sphere != 0)
        {
            if (m_bPenetratingFixedParticle)
            {
                sphere->SetLinearVelocity(v3Zero);
                sphere->SetAngularVelocity(v3Zero);
            }
            sphere->AddForceAtCentreOfMass(force);
            velocity = sphere->GetLinearVelocity();
            if (((velocity.x * m_v3BallPenetrationNormal.x)
                    + (velocity.y * m_v3BallPenetrationNormal.y)
                    + (velocity.z * m_v3BallPenetrationNormal.z))
                > 0.0f)
            {
                float dampedZ
                    = s_fVelocityDampingCoefficient * velocity.z;
                float dampedY
                    = s_fVelocityDampingCoefficient * velocity.y;
                float dampedX
                    = s_fVelocityDampingCoefficient * velocity.x;
                nlVec3Set(velocity, dampedX, dampedY, dampedZ);
                sphere->SetLinearVelocity(velocity);
            }

            sphere->GetAngularVelocity(&currentAngularVelocity);
            nlVec3CrossProduct(
                crossProduct, velocity, m_v3BallPenetrationNormal);
            nlVec3Scale(crossProduct, crossProduct, 1.0f / g_pBall->m_pPhysicsBall->GetRadius());
            nlVec3Sub(
                crossProduct, crossProduct, currentAngularVelocity);
            nlVec3Scale(crossProduct, crossProduct, 0.1f);
            dBodyAddTorque(sphere->m_bodyID,
                crossProduct.x,
                crossProduct.y,
                crossProduct.z);
        }

        BallNetmeshEventData* eventData = 0;
        lbl_80571780.Allocate(eventData);
        eventData->netMesh = this;
        eventData->collisionVelocity = g_pBall->m_v3Velocity;
        fn_80146424(eventData, sphere == 0);
    }
}

void NetMesh::Initialize(unsigned long netMeshDrawableObjectID)
{
    mNetMeshDrawableObjectID = netMeshDrawableObjectID;
    mJolt = 0.0f;
    RenderObject* dobj = fn_8027725C(netMeshDrawableObjectID);
    dobj->m_uObjectFlags = 0;

    int i;
    for (i = 0; i < m_NumParticles; ++i)
    {
        m_bIsParticleFixed[i] = false;
    }

    {
        NetMeshModelLoader loader(*this, netMeshDrawableObjectID);
    }

    mfMinX = 10000.0f;
    mfMinY = 10000.0f;
    mfMaxX = -10000.0f;
    mfMaxY = -10000.0f;
    for (i = 0; i < m_NumParticles; ++i)
    {
        nlVector3& p = m_v3Position[i];
        m_v3PrevPosition[i] = p;
        if (p.x > mfMaxX)
            mfMaxX = p.x;
        if (p.y > mfMaxY)
            mfMaxY = p.y;
        if (p.x < mfMinX)
            mfMinX = p.x;
        if (p.y < mfMinY)
            mfMinY = p.y;
        nlVec3Set(m_v3Accel[i], 0.0f, 0.0f, 0.0f);
    }

    mbIsActive = true;
    mbRelaxing = true;
    while (mbIsActive)
    {
        Update(lbl_806DC7B8, v3Zero, v3Zero, false, 0);
    }

    for (i = 0; i < m_NumParticles; i++)
    {
        m_v3RestPosition[i] = m_v3Position[i];
    }
    mbRelaxing = false;
    mbInitialized = true;
}

void NetMesh::UpdateActivity(const nlVector3& ballPosition,
    const nlVector3& ballPrevPosition, PhysicsSphere* sphere)
{
    if (g_pBall == 0)
    {
        mbIsActive = mfMotion > s_fInactivityThreshold;
        return;
    }

    if (PhysicsAIBall::IsBallOutsideNet(ballPosition))
    {
        mbBallIsInsideNet = false;
    }
    else
    {
        nlVector3 currentPosition = ballPosition;
        if (PhysicsAIBall::DidBallJustEnterNet(
                ballPrevPosition, currentPosition))
        {
            if ((ballPosition.x > 0.0f && mbPositiveEnd)
                || (ballPosition.x < 0.0f && !mbPositiveEnd))
            {
                mbBallIsInsideNet = true;
            }
        }
    }

    if (mfMotion > s_fInactivityThreshold
        || (mbBallIsInsideNet
            && (sphere == 0 || IsBallMoving(sphere)
                || (m_numAffectedParticles == 0 && mbIsActive))))
    {
        mbIsActive = true;
    }
    else
    {
        mbIsActive = false;
        m_fBallPenetrationDepth = 0.0f;
    }
}

void NetMesh::SetTriStripIndices(
    int numIndices, const unsigned short* indices)
{
    m_NumTriStripIndices = numIndices;
    unsigned long size = numIndices * sizeof(unsigned short);
    m_TriStripIndices = (u16*)nlMalloc(size, 8, false);
    memcpy(m_TriStripIndices, indices, size);
}

void NetMesh::SetDontUseLowestNetTextureLOD(bool value)
{
    sbDontUseLowestNetTextureLOD = value;
}

void NetMesh::SetTexture(unsigned long texture)
{
    sNetTextureHandle = texture;
    if (sbDontUseLowestNetTextureLOD)
    {
        PlatTexture* tex = fn_802D064C(texture);
        tex->m_MaxLevel = tex->m_Levels - 1;
        tex->Prepare();
    }
}
