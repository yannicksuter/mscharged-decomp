#ifndef GAME_RENDER_NET_MESH_H
#define GAME_RENDER_NET_MESH_H

#include "NL/nlMath.h"
#include "types.h"

class DebugWriteCache;
class PhysicsSphere;

class cDistanceConstraint
{
public:
    /* 0x0 */ int nParticleA;
    /* 0x4 */ int nParticleB;
    /* 0x8 */ float fDistance;
}; // size: 0xC

class cPositionConstraint
{
public:
    /* 0x0 */ int nParticle;
    /* 0x4 */ nlVector3 v3Position;
}; // size: 0x10

struct shortVector2
{
    union
    {
        signed short e[2];
        struct
        {
            s16 x;
            s16 y;
        };
    };
}; // size: 0x4

class NetMesh
{
public:
    static bool s_bAnimatedNetMeshEnabled;
    static bool s_bAlwaysActive;
    static float s_fReboundForceCoefficient;
    static float s_fVelocityDampingCoefficient;
    static float s_fBallRadiusExaggerationFactor;
    static float s_fBallRadiusExaggerationFactor2;
    static float s_fNetGravityMagnitude;
    static float s_fLooseness;
    static int s_NumConstraintIterations;
    static float s_fInactivityThreshold;
    static float s_fIsBallMovingThreshold;
    static float s_fDampening;
    static float s_fNetStretchLimit;
    static bool s_bUseStretchLimit;
    static unsigned long sNetTextureHandle;
    static NetMesh* spPositiveXNetMesh;
    static NetMesh* spNegativeXNetMesh;
    static bool sbDontUseLowestNetTextureLOD;

    NetMesh(bool positiveEnd);
    ~NetMesh();

    void Allocate(int numParticles, int numDistanceConstraints,
        int numPositionConstraints);
    int SetPositionConstraint(
        int particleIndex, const nlVector3& v3Position);
    void SetDistanceConstraint(
        int nParticleA, int nParticleB, float fDistance);
    void Reset(bool usePhysicsBall);
    void SyncLog(void* context, DebugWriteCache* cache);
    void Update(float dt, const nlVector3& ballPosition,
        const nlVector3& ballPrevPosition, bool bExaggerateBallSize,
        PhysicsSphere* sphere);
    void JoltNet();
    void SatisfyConstraints(
        const nlVector3& ballPosition, bool bExaggerateBallSize);
    void AddForcesToBall(
        const nlVector3& position, PhysicsSphere* sphere);
    void Initialize(unsigned long netMeshDrawableObjectID);
    void UpdateActivity(const nlVector3& ballPosition,
        const nlVector3& ballPrevPosition, PhysicsSphere* sphere);
    void SetTriStripIndices(
        int numIndices, const unsigned short* indices);
    static void SetDontUseLowestNetTextureLOD(bool value);
    void SetTexture(unsigned long texture);

    /* 0x00 */ int m_iClosestParticle;
    /* 0x04 */ bool mbInitialized;
    /* 0x05 */ bool mbFirstUpdate;
    /* 0x06 */ bool mbUsePhysicsBall;
    /* 0x07 */ bool mbRelaxing;
    /* 0x08 */ nlVector3* m_v3Position;
    /* 0x0C */ nlVector3* m_v3PrevPosition;
    /* 0x10 */ nlVector3* m_v3Accel;
    /* 0x14 */ nlVector3* m_v3RestPosition;
    /* 0x18 */ bool* m_bIsParticleFixed;
    /* 0x1C */ u16* m_TriStripIndices;
    /* 0x20 */ int m_NumTriStripIndices;
    /* 0x24 */ shortVector2* m_v2TextureCoords;
    /* 0x28 */ nlVector3* m_v3Normal;
    /* 0x2C */ unsigned long mNetMeshDrawableObjectID;
    /* 0x30 */ bool mbPositiveEnd;
    /* 0x31 */ u8 mPadding031[3];
    /* 0x34 */ int m_NumParticles;
    /* 0x38 */ float mfMinX;
    /* 0x3C */ float mfMaxX;
    /* 0x40 */ float mfMinY;
    /* 0x44 */ float mfMaxY;
    /* 0x48 */ int m_NumPositionConstraints;
    /* 0x4C */ int m_NumDistanceConstraints;
    /* 0x50 */ int m_numAffectedParticles;
    /* 0x54 */ float m_fBallPenetrationDepth;
    /* 0x58 */ unsigned char m_bPenetratingFixedParticle;
    /* 0x59 */ u8 mPadding059[3];
    /* 0x5C */ nlVector3 m_v3BallPenetrationNormal;
    /* 0x68 */ bool mbIsActive;
    /* 0x69 */ bool mbBallIsInsideNet;
    /* 0x6A */ u8 mPadding06A[2];
    /* 0x6C */ float mfMotion;
    /* 0x70 */ float mJolt;
    /* 0x74 */ u8 mUnidentified074[8];
    /* 0x7C */ cDistanceConstraint* m_aDistanceConstraints;
    /* 0x80 */ cPositionConstraint* m_aPositionConstraints;
}; // size: 0x84

#endif // GAME_RENDER_NET_MESH_H
