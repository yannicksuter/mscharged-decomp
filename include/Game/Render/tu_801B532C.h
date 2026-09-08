#ifndef GAME_RENDER_TU_801B532C_H
#define GAME_RENDER_TU_801B532C_H

struct UnidentifiedNPCConfig_801B532C
{
    /* 0x00 */ int mUnidentified000;
    /* 0x04 */ const char* mName;
    /* 0x08 */ float mUnidentified008;
    /* 0x0C */ unsigned long mUnidentified00C;
    /* 0x10 */ unsigned long mUnidentified010;
}; // total size: 0x14

extern "C" UnidentifiedNPCConfig_801B532C* fn_801B532C(const int& index);

#endif // GAME_RENDER_TU_801B532C_H
