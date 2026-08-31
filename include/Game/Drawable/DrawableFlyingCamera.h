#ifndef _DRAWABLEFLYINGCAMERA_H_
#define _DRAWABLEFLYINGCAMERA_H_

#include "types.h"
#include "NL/nlMath.h"

class DrawableFlyingCamera
{
public:
    DrawableFlyingCamera();
    void Grab();
    void Render() const;
    void Blend(const float*, const DrawableFlyingCamera&, const DrawableFlyingCamera&);

    nlQuaternion mOrientation;
    nlVector3 mPosition;
    float mScale;
    int mIndex;
    bool mVisible;
    char _025[3];
};

#endif // _DRAWABLEFLYINGCAMERA_H_
