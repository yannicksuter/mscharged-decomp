#ifndef _DRAWABLEKOOPASHELL_H_
#define _DRAWABLEKOOPASHELL_H_

#include "types.h"
#include "NL/nlMath.h"

struct KoopaShellObject;

class DrawableKoopaShell
{
public:
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

#endif // _DRAWABLEKOOPASHELL_H_
