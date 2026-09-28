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
    if (!count.field_4000 && !count.field_8000)
        return;

    count.field_4000 = false;
    nlVector3 ListenerOffset;
    nlVec3Sub(ListenerOffset, GetPosition(), listener->m_Position);
    m_Unknown10 = nlVec3Length(ListenerOffset);
    if (nlNear(m_Unknown10, 0.0f))
    {
        nlPrintf("Plat3dSoundSrc::Update:  ListenerOffset distance is zero, adding offset\n");
        nlVec3Add(ListenerOffset, 0.00001f, 0.00001f, 0.00001f);
        m_Unknown10 = nlVec3Length(ListenerOffset);
    }
    g_Dist = m_Unknown10;

    nlVector3 projected;
    nlVector4 plane;
    nlVec4Set(plane, listener->m_Up.x, listener->m_Up.y, listener->m_Up.z, 0.0f);
    nlProjectPointOntoPlane(projected, ListenerOffset, plane);
    nlVec3Normalize(projected, projected);
    float pan = nlVec3DotProduct(projected, listener->m_Unknown2C);
    m_Unknown20 = pan;
    if (m_Flags44.m_UnknownFlag08)
    {
        float magnitude = fabsf(pan);
        int sign = pan < 0.0f ? -1 : 1;
        m_Unknown20 = sign * nlSqrt(magnitude, true);
    }
    g_Pan = m_Unknown20;
    m_Unknown14 = 180.0f * m_Unknown20;

    nlVec4Set(plane, listener->m_Unknown2C.x, listener->m_Unknown2C.y, listener->m_Unknown2C.z, 0.0f);
    nlProjectPointOntoPlane(projected, ListenerOffset, plane);
    nlVec3Normalize(projected, projected);
    float frontBack = nlVec3DotProduct(projected, listener->m_View);
    m_Unknown24 = frontBack;
    if (m_Flags44.m_UnknownFlag08)
    {
        float magnitude = fabsf(frontBack);
        int sign = frontBack < 0.0f ? -1 : 1;
        m_Unknown24 = sign * nlSqrt(magnitude, true);
    }
    m_Flags44.m_UnknownFlags00 = 0;

    if (count.field_2000)
    {
        nlVec3Set(m_Unknown34, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        nlVec3Sub(m_Unknown34, m_Unknown28, GetPosition());
        nlVec3Scale(m_Unknown34, 1.0f / deltaTime);
    }
    m_Unknown28 = GetPosition();

    nlVector3 relativeVelocity;
    nlVec3Sub(relativeVelocity, m_Unknown34, listener->m_Unknown38);
    float relativeSpeed = nlVec3Length(relativeVelocity);
    g_RelVel = relativeSpeed;
    if (g_RelVel != 0.0f)
    {
        m_Unknown40 = 12.0f * nlFastLog2(1.0f / (1.0f - relativeSpeed / sSpeedOfSound));
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
