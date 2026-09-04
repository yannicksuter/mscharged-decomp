#include "Game/AI/HeadTrack.h"

void cHeadTrack::Update(const nlMatrix4& m4HeadMatrix,
    const nlMatrix4& m4ConstraintMatrix, float fDeltaT,
    unsigned short aOOIConstraint, int nHeadSpinMax, int nHeadTiltMax)
{
    nlMatrix4 m4Constrain;
    nlMatrix4 m4WorldSpaceToConstraintSpace;
    nlVector3 v3OOIConstraintSpace;
    int nHeadSpin;
    int nHeadTilt;
    int nAmountOfDeadZoneBehindHeadtrack;

    if (m_bTrackOOI)
    {
        m4Constrain = m4ConstraintMatrix;
        float headM41 = m4HeadMatrix.m41;
        float headM42 = m4HeadMatrix.m42;
        float headM43 = m4HeadMatrix.m43;

        m4Constrain.m41 = headM41;
        m4Constrain.m42 = headM42;
        m4Constrain.m43 = headM43;
        m4Constrain.m44 = 1.0f;

        nlInvertRotTransMatrix(
            m4WorldSpaceToConstraintSpace, m4Constrain);
        nlMultPosVectorMatrix(v3OOIConstraintSpace, m_v3OOI, m4WorldSpaceToConstraintSpace);

        {
            float invLen = nlRecipSqrt(
                v3OOIConstraintSpace.x * v3OOIConstraintSpace.x
                    + v3OOIConstraintSpace.y * v3OOIConstraintSpace.y
                    + v3OOIConstraintSpace.z * v3OOIConstraintSpace.z,
                true);
            nlVec3Scale(v3OOIConstraintSpace, invLen);
        }

        nHeadSpin = ((int)(10430.378f
                           * nlATan2f(v3OOIConstraintSpace.z,
                               v3OOIConstraintSpace.y))
                        << 16)
                 >> 16;
        nHeadTilt = 0x4000 - nlACos(-v3OOIConstraintSpace.x);
        nHeadTilt = (nHeadTilt << 16) >> 16;

        {
            int absSpin = (nHeadSpin < 0) ? -nHeadSpin : nHeadSpin;

            if ((absSpin < (int)(unsigned int)aOOIConstraint)
                || (m_v3OOI.z > 1.33f))
            {
                nHeadSpin = (int)(float)nHeadSpin;
                nHeadTilt = (int)(float)nHeadTilt;
                nAmountOfDeadZoneBehindHeadtrack
                    = (((int)(unsigned int)aOOIConstraint - nHeadSpinMax)
                          * 3)
                    / 4;

                absSpin = (nHeadSpin < 0) ? -nHeadSpin : nHeadSpin;
                if ((unsigned int)absSpin >= (unsigned int)nHeadSpinMax)
                {
                    if (nHeadSpin > 0)
                    {
                        if (m_fDesiredHeadSpin < 0.0f)
                        {
                            if (nHeadSpin
                                > (nHeadSpinMax
                                    + nAmountOfDeadZoneBehindHeadtrack))
                                nHeadSpin = -nHeadSpinMax;
                            else
                                nHeadSpin = nHeadSpinMax;
                        }
                        else
                            nHeadSpin = nHeadSpinMax;
                    }
                    else if (m_fDesiredHeadSpin > 0.0f)
                    {
                        if (nHeadSpin
                            < -(nHeadSpinMax
                                + nAmountOfDeadZoneBehindHeadtrack))
                            nHeadSpin = nHeadSpinMax;
                        else
                            nHeadSpin = -nHeadSpinMax;
                    }
                    else
                        nHeadSpin = -nHeadSpinMax;
                }
            }
            else
            {
                nHeadSpin = 0;
                nHeadTilt = 0;
            }
        }

        if (nHeadTilt > nHeadTiltMax)
            nHeadTilt = nHeadTiltMax;
        else if (nHeadTilt < -nHeadTiltMax)
            nHeadTilt = -nHeadTiltMax;

        if (&m4HeadMatrix != &m4ConstraintMatrix)
        {
            nlVector3 v3Constrain;
            nlVector3 v3Head;
            nlVec3Set(v3Constrain, m4Constrain.m21, m4Constrain.m22, m4Constrain.m23);
            nlVec3Set(v3Head, m4HeadMatrix.m21, m4HeadMatrix.m22, m4HeadMatrix.m23);
            unsigned short spinConstraint
                = (unsigned short)(int)(10430.378f
                                        * nlATan2f(v3Constrain.y, v3Constrain.x));
            unsigned short spinHead
                = (unsigned short)(int)(10430.378f
                                        * nlATan2f(v3Head.y, v3Head.x));
            nHeadSpin += (short)(spinConstraint - spinHead);
        }
        m_fDesiredHeadSpin = nHeadSpin;
        m_fDesiredHeadTilt = nHeadTilt;
    }
    else
    {
        m_fDesiredHeadSpin = 0.0f;
        m_fDesiredHeadTilt = 0.0f;
    }

    if (m_fSmoothTime > 0.0001f)
    {
        float spinChange;
        float x;
        float spinVel;
        float omega = 2.0f / m_fSmoothTime;
        x = omega * fDeltaT;
        float exp = 1.0f
                  / (((0.48f * x * x) + (1.0f + x))
                      + (x * (0.235f * x * x)));

        spinChange = m_fHeadSpin - m_fDesiredHeadSpin;
        spinVel = m_fHeadSpinSeekVel;

        m_fHeadSpinSeekVel = exp
                           * (spinVel
                               - (omega
                                   * (fDeltaT * ((omega * spinChange) + spinVel))));
        m_fHeadSpin
            = (exp
                  * (spinChange
                      + (fDeltaT * ((omega * spinChange) + spinVel))))
            + m_fDesiredHeadSpin;

        float tiltChange = m_fHeadTilt - m_fDesiredHeadTilt;
        float tiltVel = m_fHeadTiltSeekVel;

        m_fHeadTiltSeekVel = exp
                           * (tiltVel
                               - (omega
                                   * (fDeltaT * ((omega * tiltChange) + tiltVel))));
        m_fHeadTilt
            = (exp
                  * (tiltChange
                      + (fDeltaT * ((omega * tiltChange) + tiltVel))))
            + m_fDesiredHeadTilt;
    }
}

extern "C" void fn_802B53EC(nlQuaternion&, unsigned short);
extern "C" void fn_802B549C(nlQuaternion&, unsigned short);

void CalcHeadTrackMatrix(unsigned short spin, unsigned short tilt,
    cPoseAccumulator* cPoseAccumulator, int headNodeIndex)
{
    nlMatrix4 m4RotMatrix;
    nlMatrix4 m4NewHeadMatrix;
    nlQuaternion qNewHead;
    nlQuaternion qRotation;
    nlQuaternion qTilt;
    nlQuaternion qSpin;

    const nlMatrix4& m4AnimatedHeadMatrix
        = cPoseAccumulator->GetNodeMatrix(headNodeIndex);
    nlQuaternion& qAnimatedHead
        = cPoseAccumulator->m_pQuaternions[headNodeIndex];

    fn_802B53EC(qSpin, spin);
    fn_802B549C(qTilt, tilt);
    nlMultQuat(qRotation, qSpin, qTilt);
    nlMultQuat(qNewHead, qAnimatedHead, qRotation);
    nlQuatToMatrix(m4RotMatrix, qRotation, true);
    nlMultMatrices(
        m4NewHeadMatrix, m4RotMatrix, m4AnimatedHeadMatrix);

    cPoseAccumulator->m_pQuaternions[headNodeIndex] = qNewHead;
    cPoseAccumulator->m_NodeMatrices[headNodeIndex] = m4NewHeadMatrix;
}
