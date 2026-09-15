#ifndef _DRAWABLEKOOPASHELL_H_
#define _DRAWABLEKOOPASHELL_H_

#include "types.h"
#include "Game/Replay.h"
#include "NL/nlMath.h"

struct KoopaShellObject;

class DrawableKoopaShell
{
public:
    template <typename T>
    void Replay(T& frame);
    DrawableKoopaShell();
    void Grab(const KoopaShellObject*);
    void Render(const KoopaShellObject*) const;
    void Blend(const float*, const DrawableKoopaShell&, const DrawableKoopaShell&);

    u16 mSpin;
    char _002[2];
    nlVector3 mPosition;
    float mScale;
    bool mVisible;
    char _015[3];
};

template <typename T>
inline void DrawableKoopaShell::Replay(T& frame)
{
    Replayable<3>(frame, mVisible);
    if (mVisible)
    {
        Replayable<3>(frame, mSpin);
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.x));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.y));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.z));
        Replayable<3>(frame, FloatCompressor<0, 15, 12>(mScale));
    }
}

#endif // _DRAWABLEKOOPASHELL_H_
