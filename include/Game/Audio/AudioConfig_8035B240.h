#ifndef GAME_AUDIO_AUDIO_CONFIG_8035B240_H
#define GAME_AUDIO_AUDIO_CONFIG_8035B240_H

class ConfigNode_8035B240
{
public:
    virtual ~ConfigNode_8035B240();
    virtual unsigned long long fn_8035B240(const unsigned int& key);
};

struct ConfigSystem_8035B240
{
    unsigned char m_Pad00[0x20];
    ConfigNode_8035B240* m_Root;
};

union ConfigValue_8035B240
{
    unsigned long long m_Raw;
    float m_Float;
    struct
    {
        unsigned int m_Value;
        unsigned int m_Type;
    } m_Words;
};

extern ConfigSystem_8035B240* lbl_806E202C;

#endif // GAME_AUDIO_AUDIO_CONFIG_8035B240_H
