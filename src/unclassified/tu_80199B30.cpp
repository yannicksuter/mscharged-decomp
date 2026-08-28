#include "Game/Blinker.h"

#include "Game/NisPlayer.h"
#include "NL/nlMath.h"
#include "NL/gl/glModel.h"

#include "string.h"

extern "C" void* fn_802CDF0C();
extern "C" unsigned long fn_802CE1B8(void*, unsigned long texture);
extern "C" bool fn_8027C12C(NisPlayer*);
extern "C" unsigned long fn_802CC7E4(
    glModelPacket*, unsigned long state);
extern "C" void fn_802CC458(
    glModelPacket*, unsigned long state, unsigned long texture);
extern "C" void fn_802CC4FC(
    glModelPacket*, unsigned long state, const unsigned long* texture);

extern u8 lbl_806DCD60;
extern unsigned long lbl_806E1CEC;

static const float BlinkTimes[4] = {
    1.25f, 0.016666667f, 0.033333335f, 0.016666667f
};

static inline float RandomizedValue(float base, float range)
{
    float randomOffset = nlRandomf(0.5f * range, &nlDefaultSeed);
    unsigned int randomSign = nlRandom(0x7FFFFFFF, &nlDefaultSeed);

    if (randomSign & 1)
    {
        return base + randomOffset;
    }
    else
    {
        return base - randomOffset;
    }
}

Blinker::Blinker(unsigned long arg0, unsigned long arg1, unsigned long arg2)
{
    unknown_0x00[0] = arg0;
    unknown_0x00[1] = arg1;
    unknown_0x00[2] = arg2;
    unknown_0x00[3] = arg1;

    m_Textures[0] = fn_802CE1B8(fn_802CDF0C(), unknown_0x00[0]);
    m_Textures[1] = fn_802CE1B8(fn_802CDF0C(), unknown_0x00[1]);
    m_Textures[2] = fn_802CE1B8(fn_802CDF0C(), unknown_0x00[2]);
    m_Textures[3] = fn_802CE1B8(fn_802CDF0C(), unknown_0x00[3]);

    memcpy(m_fBlinkTimes, BlinkTimes, sizeof(m_fBlinkTimes));
    m_fTime = 0.0f;
    m_State = Blink_Open;
    m_bJustDoubleBlinked = false;
}

void Blinker::Update(float fDeltaT)
{
    if (lbl_806DCD60 == 0 || fDeltaT < 0.0f)
    {
        m_State = Blink_Open;
        m_fTime = 0.0f;
        return;
    }

    float deltaTime = fDeltaT;
    if (fn_8027C12C(NisPlayer::Instance()))
    {
        deltaTime = 0.0f;
    }
    m_fTime += deltaTime;

    if (m_fTime > m_fBlinkTimes[m_State])
    {
        m_fTime = 0.0f;
        switch (m_State)
        {
        case Blink_Open:
            m_State = Blink_HalfClosed;
            if (m_bJustDoubleBlinked == 0)
            {
                if (RandomizedValue(0.5f, 1.0f) < 0.2f)
                {
                    m_fBlinkTimes[0] = RandomizedValue(0.06666667f, 0.06666667f);
                    m_bJustDoubleBlinked = true;
                    return;
                }
            }
            m_fBlinkTimes[0] = RandomizedValue(1.2f, 1.0f);
            m_bJustDoubleBlinked = false;
            break;
        case Blink_HalfClosed:
            m_State = Blink_Closed;
            break;
        case Blink_Closed:
            m_State = Blink_HalfOpen;
            break;
        case Blink_HalfOpen:
            m_State = Blink_Open;
            break;
        }
    }
}

void Blinker::Blink(glModel* model)
{
    if (m_State == Blink_Open)
    {
        return;
    }

    glModelPacket* packet = model->packets;
    while (packet < model->packets + model->numPackets)
    {
        if (unknown_0x00[0] == fn_802CC7E4(packet, lbl_806E1CEC))
        {
            fn_802CC458(packet, lbl_806E1CEC, unknown_0x00[0]);
            unsigned long texture = m_Textures[m_State];
            fn_802CC4FC(packet, lbl_806E1CEC, &texture);
        }
        ++packet;
    }
}
