#include "NL/nlDLListContainer.inl"
#include "NL/nlPrint.h"
#include "Game/Audio/Plat3dSoundSrc.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "math.h"

float sSpeedOfSound = 343.5f;

float g_Pan;
float g_Dist;
float g_RelVel;

static TweakFloatBinding sPanTweak("g_Pan", "audio/Stats", &g_Pan, true);
static TweakFloatBinding sDistanceTweak("g_Dist", "audio/Stats", &g_Dist, true);
static TweakFloatBinding sRelativeVelocityTweak("g_RelVel", "audio/Stats", &g_RelVel, true);

void Plat3dSoundSrc::Update(PlatAudioListener* listener, float deltaTime)
{
    if (!count.updatePending && !count.positionIsPointer)
        return;

    count.updatePending = false;
    nlVector3 ListenerOffset;
    nlVec3Sub(ListenerOffset, GetPosition(), listener->m_Position);
    m_Distance = nlVec3Length(ListenerOffset);
    if (nlNear(m_Distance, 0.0f))
    {
        nlPrintf("Plat3dSoundSrc::Update:  ListenerOffset distance is zero, adding offset\n");
        nlVec3Set(ListenerOffset, ListenerOffset.x + 0.00001f, ListenerOffset.y + 0.00001f, ListenerOffset.z + 0.00001f);
        m_Distance = nlVec3Length(ListenerOffset);
    }
    g_Dist = m_Distance;

    nlVector3 projected;
    nlVector4 plane;
    nlVec4Set(plane, listener->m_Up.x, listener->m_Up.y, listener->m_Up.z, 0.0f);
    nlProjectPointOntoPlane(projected, ListenerOffset, plane);
    nlVec3Normalize(projected, projected);
    float pan = nlVec3DotProduct(projected, listener->m_Right);
    m_Pan = pan;
    if (m_Spatial.m_SquareRootPan)
    {
        float magnitude = fabsf(pan);
        int sign = pan < 0.0f ? -1 : 1;
        m_Pan = sign * nlSqrt(magnitude, true);
    }
    g_Pan = m_Pan;
    m_ScaledPan = 180.0f * m_Pan;

    nlVec4Set(plane, listener->m_Right.x, listener->m_Right.y, listener->m_Right.z, 0.0f);
    nlProjectPointOntoPlane(projected, ListenerOffset, plane);
    nlVec3Normalize(projected, projected);
    float surroundPan = nlVec3DotProduct(projected, listener->m_View);
    m_SurroundPan = surroundPan;
    if (m_Spatial.m_SquareRootPan)
    {
        float magnitude = fabsf(surroundPan);
        int sign = surroundPan < 0.0f ? -1 : 1;
        m_SurroundPan = sign * nlSqrt(magnitude, true);
    }
    m_Spatial.m_InterauralDelay = 0;

    if (count.zeroVelocity)
    {
        nlVec3Set(m_Velocity, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        nlVec3Sub(m_Velocity, m_PrevPosition, GetPosition());
        nlVec3Scale(m_Velocity, 1.0f / deltaTime);
    }
    m_PrevPosition = GetPosition();

    nlVector3 relativeVelocity;
    nlVec3Sub(relativeVelocity, m_Velocity, listener->m_Velocity);
    float relativeSpeed = nlVec3Length(relativeVelocity);
    g_RelVel = relativeSpeed;
    // R4QE01 compares the speed read back from g_RelVel (rounded to single
    // precision, while the division below uses the unrounded value) and keeps
    // that read in a variable of its own; only a local reproduces its register
    // allocation. The original spelling is not recoverable.
    float relVel = g_RelVel;
    if (relVel != 0.0f)
    {
        m_DopplerPitch = 12.0f * nlFastLog2(1.0f / (1.0f - relativeSpeed / sSpeedOfSound));
    }
}

void PlatAudioListener::Update(float deltaTime)
{
    if (m_HasTransform)
    {
        nlVec3CrossProduct(m_Right, m_View, m_Up);
        if (m_Enabled)
        {
            nlVec3Set(m_Velocity, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            nlVector3 displacement;
            nlVec3Sub(displacement, m_PrevPosition, m_Position);
            nlVec3Scale(m_Velocity, displacement, 1.0f / deltaTime);
            m_PrevPosition = m_Position;
        }
    }
    m_HasTransform = false;
}
