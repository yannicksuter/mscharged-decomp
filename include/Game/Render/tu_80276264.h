#ifndef GAME_RENDER_TU_80276264_H
#define GAME_RENDER_TU_80276264_H

class DrawableObject;

struct StadiumEntry_80521F68
{
    int mUnidentified000;
    const char* mUnidentified004;
    int mUnidentified008;
    DrawableObject** mUnidentified00C;
    int mUnidentified010;
    int mUnidentified014;
    int mUnidentified018;
};

struct StadiumLoadResult_8057A7B8
{
    void* mData;
    unsigned long mSize;
    bool mProcessed;
};

extern "C"
{
    bool fn_80276724();
    void fn_80276758();
    void fn_80276890();
    void fn_80276990(bool visible);
    bool fn_802769B4();
    void fn_80276B4C();
    bool fn_80276C4C();
}

#endif // GAME_RENDER_TU_80276264_H
