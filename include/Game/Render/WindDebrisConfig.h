#ifndef GAME_RENDER_WINDDEBRISCONFIG_H
#define GAME_RENDER_WINDDEBRISCONFIG_H

struct WindDebrisConfig
{
    /* 0x00 */ int mUnidentified000;
    /* 0x04 */ const char* mName;
    /* 0x08 */ float mRadius;
    /* 0x0C */ unsigned long mCueId;
    /* 0x10 */ unsigned long mUnidentified010;
}; // total size: 0x14

extern "C" WindDebrisConfig* GetWindDebrisConfig(const int& index);

#endif // GAME_RENDER_WINDDEBRISCONFIG_H
