#include "Game/Blinker.h"

#include "Game/Effects/EffectsTemplate.h"
#include "Game/NisPlayer.h"

#include <string.h>

extern "C"
{
    bool lbl_806DCD60 = true;
    extern unsigned long lbl_806E1F0C;

    bool fn_8027C12C(const NisPlayer* player);
    void* fn_802CDF0C();
    unsigned long fn_802CE1B8(void* manager, unsigned long texture);
    unsigned long fn_802CC7E4(
        glModelPacket* packet, unsigned long textureType);
    void fn_802CC458(glModelPacket* packet, unsigned long textureType,
        unsigned long texture);
    void fn_802CC4FC(glModelPacket* packet, unsigned long textureType,
        const unsigned long* texture);
}

const float BlinkTimes[4] = {
    1.25f, 0.016666667f, 0.033333335f, 0.016666667f
};

void Blinker::Blink(glModel* model)
{
    if (m_State != Blink_Open)
    {
        unsigned long texture;
        glModelPacket* packet = model->packets;
        for (; packet < model->packets + model->numPackets; ++packet)
        {
            texture = m_Textures[0];
            if (texture == fn_802CC7E4(packet, lbl_806E1F0C))
            {
                fn_802CC458(packet, lbl_806E1F0C, texture);
                unsigned long resolvedTexture = m_ResolvedTextures[(unsigned long)m_State];
                fn_802CC4FC(packet, lbl_806E1F0C, &resolvedTexture);
            }
        }
    }
}

void Blinker::Update(float fDeltaT)
{
    if (!lbl_806DCD60 || fDeltaT < 0.0f)
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
                    m_bJustDoubleBlinked = 1;
                    return;
                }
            }
            m_fBlinkTimes[0] = RandomizedValue(1.2f, 1.0f);
            m_bJustDoubleBlinked = 0;
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

Blinker::Blinker(unsigned long texture0, unsigned long texture1,
    unsigned long texture2)
{
    m_Textures[0] = texture0;
    m_Textures[1] = texture1;
    m_Textures[2] = texture2;
    m_Textures[3] = texture1;

    m_ResolvedTextures[0] = fn_802CE1B8(fn_802CDF0C(), m_Textures[0]);
    m_ResolvedTextures[1] = fn_802CE1B8(fn_802CDF0C(), m_Textures[1]);
    m_ResolvedTextures[2] = fn_802CE1B8(fn_802CDF0C(), m_Textures[2]);
    m_ResolvedTextures[3] = fn_802CE1B8(fn_802CDF0C(), m_Textures[3]);

    memcpy(m_fBlinkTimes, BlinkTimes, sizeof(m_fBlinkTimes));
    m_State = Blink_Open;
    m_fTime = 0.0f;
    m_bJustDoubleBlinked = false;
}
