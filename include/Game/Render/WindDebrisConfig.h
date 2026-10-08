#ifndef GAME_RENDER_WINDDEBRISCONFIG_H
#define GAME_RENDER_WINDDEBRISCONFIG_H

enum eWindDebrisType
{
    WIND_DEBRIS_NONE = -1,
    WIND_DEBRIS_COW = 0,
    WIND_DEBRIS_CATFISH = 1,
    WIND_DEBRIS_TRACTOR = 2,
};

struct WindDebrisConfig
{
    /* 0x00 */ eWindDebrisType mDebrisType;
    /* 0x04 */ const char* mName;
    /* 0x08 */ float mRadius;
    /* 0x0C */ unsigned long mCueId;
    /* 0x10 */ unsigned long mImpactCueId;
}; // total size: 0x14

extern "C" WindDebrisConfig* GetWindDebrisConfig(const int& index);

#endif // GAME_RENDER_WINDDEBRISCONFIG_H
