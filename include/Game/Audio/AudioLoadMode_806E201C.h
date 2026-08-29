#ifndef GAME_AUDIO_AUDIO_LOAD_MODE_806E201C_H
#define GAME_AUDIO_AUDIO_LOAD_MODE_806E201C_H

#include "types.h"

struct AudioValues_801098E4
{
    /* 0x00 */ u8 m_unk00[0x5C];
    /* 0x5C */ float m_unk5C;
    /* 0x60 */ float m_unk60;
    /* 0x64 */ u8 m_unk64[0x4];
    /* 0x68 */ float m_unk68;
    /* 0x6C */ float m_unk6C;
    /* 0x70 */ u8 m_unk70[0x14];
    /* 0x84 */ float m_unk84;
    /* 0x88 */ float m_unk88;
    /* 0x8C */ u8 m_unk8C[0x4];
    /* 0x90 */ float m_unk90;
    /* 0x94 */ float m_unk94;
    /* 0x98 */ u8 m_unk98[0x14];
    /* 0xAC */ float m_unkAC;
    /* 0xB0 */ float m_unkB0;
    /* 0xB4 */ u8 m_unkB4[0x4];
    /* 0xB8 */ float m_unkB8;
    /* 0xBC */ float m_unkBC;
};

struct AudioValuesOwner_801098E4
{
    /* 0x00 */ u8 m_unk00[0x8];
    /* 0x08 */ AudioValues_801098E4* m_unk08;
};

struct AudioRuntime_801098E4
{
    /* 0x00 */ u8 m_unk00[0x10];
    /* 0x10 */ AudioValuesOwner_801098E4* m_unk10;
};

class AudioLoadMode_806E201C
{
public:
    virtual bool fn_806E201C_0();

    /* 0x004 */ u8 m_unk04[0xC8];
    /* 0x0CC */ AudioRuntime_801098E4* m_unkCC;
};

extern AudioLoadMode_806E201C* lbl_806E201C;

#endif // GAME_AUDIO_AUDIO_LOAD_MODE_806E201C_H
