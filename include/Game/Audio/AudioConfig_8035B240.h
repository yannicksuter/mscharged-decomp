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

inline ConfigNode_8035B240* ConfigFindDefinition_8035B240(
    unsigned int definition)
{
    unsigned int definitionKey = definition;
    unsigned int key = 0xD2894EC5;
    ConfigValue_8035B240 value;
    value.m_Raw = lbl_806E202C->m_Root->fn_8035B240(key);
    ConfigNode_8035B240* node = (ConfigNode_8035B240*)value.m_Words.m_Value;
    value.m_Raw = node->fn_8035B240(definitionKey);
    return (ConfigNode_8035B240*)value.m_Words.m_Value;
}

#endif // GAME_AUDIO_AUDIO_CONFIG_8035B240_H
