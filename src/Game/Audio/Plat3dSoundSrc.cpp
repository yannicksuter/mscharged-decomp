#include "NL/nlPrint.h"
#include "Game/Audio/Plat3dSoundSrc.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"

static float sSpeedOfSound = 343.5f;

float g_Pan;
float g_Dist;
float g_RelVel;

static TweakFloatBinding sPanTweak("g_Pan", "audio/Stats", &g_Pan, true);
static TweakFloatBinding sDistanceTweak("g_Dist", "audio/Stats", &g_Dist, true);
static TweakFloatBinding sRelativeVelocityTweak("g_RelVel", "audio/Stats", &g_RelVel, true);

void Plat3dSoundSrc::Update(PlatAudioListener* listener, float deltaTime)
{
    if ((m_Unknown1C & 0x4000) == 0 && (m_Unknown1C & 0x8000) == 0)
        return;

    m_Unknown1C &= ~0x4000;
    nlVector3 ListenerOffset;
    nlVec3Sub(ListenerOffset,
        (m_Unknown1C & 0x8000) ? *m_Unknown04.m_Pointer : m_Unknown04.m_Value,
        listener->m_Position);
    m_Unknown10 = nlVec3Length(ListenerOffset);
    if (nlNear(m_Unknown10, 0.0f))
    {
        nlPrintf("Plat3dSoundSrc::Update:  ListenerOffset distance is zero, adding offset\n");
        nlVec3Add(ListenerOffset, 0.00001f, 0.00001f, 0.00001f);
        m_Unknown10 = nlVec3Length(ListenerOffset);
    }
    g_Dist = m_Unknown10;

    nlVector4 plane;
    nlVector3 projected;
    nlVec4Set(plane, listener->m_Up.x, listener->m_Up.y, listener->m_Up.z, 0.0f);
    nlProjectPointOntoPlane(projected, ListenerOffset, plane);
    nlVec3Scale(projected, nlRecipSqrt(nlVec3LengthSquared(projected), true));
    m_Unknown20 = nlVec3DotProduct(projected, listener->m_Unknown2C);
    if (m_Unknown44 & 0x00800000)
    {
        int sign = m_Unknown20 < 0.0f ? -1 : 1;
        m_Unknown20 = sign * nlSqrt(nlAbs(m_Unknown20), true);
    }
    g_Pan = m_Unknown20;
    m_Unknown14 = 180.0f * m_Unknown20;

    nlVec4Set(plane, listener->m_Unknown2C.x, listener->m_Unknown2C.y, listener->m_Unknown2C.z, 0.0f);
    nlProjectPointOntoPlane(projected, ListenerOffset, plane);
    nlVec3Scale(projected, nlRecipSqrt(nlVec3LengthSquared(projected), true));
    m_Unknown24 = nlVec3DotProduct(projected, listener->m_View);
    if (m_Unknown44 & 0x00800000)
    {
        int sign = m_Unknown24 < 0.0f ? -1 : 1;
        m_Unknown24 = sign * nlSqrt(nlAbs(m_Unknown24), true);
    }
    m_Unknown44 &= 0x00FFFFFF;

    if (m_Unknown1C & 0x2000)
    {
        nlVec3Set(m_Unknown34, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        nlVector3 velocity;
        nlVec3Sub(velocity, m_Unknown28, (m_Unknown1C & 0x8000) ? *m_Unknown04.m_Pointer : m_Unknown04.m_Value);
        nlVec3Scale(m_Unknown34, velocity, 1.0f / deltaTime);
    }
    m_Unknown28 = (m_Unknown1C & 0x8000) ? *m_Unknown04.m_Pointer : m_Unknown04.m_Value;

    nlVector3 relativeVelocity;
    nlVec3Sub(relativeVelocity, m_Unknown34, listener->m_Unknown38);
    g_RelVel = nlVec3Length(relativeVelocity);
    if (g_RelVel != 0.0f)
    {
        m_Unknown40 = 12.0f * nlFastLog2(1.0f / (1.0f - g_RelVel / sSpeedOfSound));
    }
}

void PlatAudioListener::Update(float deltaTime)
{
    if (m_HasTransform)
    {
        nlVec3CrossProduct(m_Unknown2C, m_View, m_Up);
        if (m_Enabled)
        {
            nlVec3Set(m_Unknown38, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            nlVector3 velocity;
            nlVec3Sub(velocity, m_Unknown44, m_Position);
            nlVec3Scale(m_Unknown38, velocity, 1.0f / deltaTime);
            m_Unknown44 = m_Position;
        }
    }
    m_HasTransform = false;
}
