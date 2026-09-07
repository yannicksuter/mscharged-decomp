#ifndef GAME_AUDIO_AUDIO_CONFIG_H
#define GAME_AUDIO_AUDIO_CONFIG_H

#include "Game/Audio/AudioResourceRuntime.h"

union AudioConfigValue
{
    unsigned long long m_Raw;
    float m_Float;
    struct
    {
        unsigned int m_Value;
        unsigned int m_Type;
    } m_Words;
};

class AudioConfigNode
{
public:
    virtual bool Has(const unsigned int& key) const;
    virtual AudioConfigValue Get(const unsigned int& key) const;
};

inline AudioConfigNode* ConfigFindDefinition(
    unsigned int definition)
{
    unsigned int definitionKey = definition;
    unsigned int key = 0xD2894EC5;
    return (AudioConfigNode*)((AudioConfigNode*)
        g_pAudioResourceRuntime->GetConfigRoot()->Get(key).m_Words.m_Value)
        ->Get(definitionKey).m_Words.m_Value;
}

#endif // GAME_AUDIO_AUDIO_CONFIG_H
