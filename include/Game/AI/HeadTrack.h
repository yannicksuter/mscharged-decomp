#ifndef GAME_AI_HEADTRACK_H
#define GAME_AI_HEADTRACK_H

#include "Game/PoseAccumulator.h"
#include "NL/nlMath.h"

void CalcHeadTrackMatrix(unsigned short spin, unsigned short tilt,
    cPoseAccumulator* cPoseAccumulator, int headNodeIndex);

class cHeadTrack
{
public:
    cHeadTrack();
    virtual ~cHeadTrack();

    void UnidentifiedReset()
    {
        m_m4HeadMatrix.SetIdentity();
        nlVec3Set(m_v3OOI, 0.0f, 0.0f, 0.0f);
        m_bTrackOOI = true;
        m_fHeadSpin = 0.0f;
        m_fHeadTilt = 0.0f;
        m_fDesiredHeadSpin = 0.0f;
        m_fDesiredHeadTilt = 0.0f;
        m_fHeadSpinSeekVel = 0.0f;
        m_fHeadTiltSeekVel = 0.0f;
        m_fSmoothTime = 0.0f;
    }

    void Update(const nlMatrix4& m4HeadMatrix,
        const nlMatrix4& m4ConstraintMatrix, float fDeltaT,
        unsigned short aOOIConstraint, int nHeadSpinMax,
        int nHeadTiltMax);

    nlMatrix4 m_m4HeadMatrix;
    nlVector3 m_v3OOI;
    bool m_bTrackOOI;
    float m_fHeadSpin;
    float m_fHeadTilt;
    float m_fDesiredHeadSpin;
    float m_fDesiredHeadTilt;
    float m_fHeadSpinSeekVel;
    float m_fHeadTiltSeekVel;
    float m_fSmoothTime;
};

#endif // GAME_AI_HEADTRACK_H
