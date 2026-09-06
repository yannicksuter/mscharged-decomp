#include "Game/Render/tu_8027AE14.h"

extern "C"
{
    float lbl_806DEE88 = 0.5f;
    float lbl_806DEE8C = 0.1f;
    float lbl_806DEE90 = 0.3f;
    float lbl_806DEE94 = 0.2f;
}

UnidentifiedObject_8027AE14::UnidentifiedObject_8027AE14(const nlVector3& param1)
    : UnidentifiedObject_80188884(lbl_806DEE88
          + nlRandomf(-lbl_806DEE8C, lbl_806DEE8C, &nlDefaultSeed))
    , mUnidentified010(param1)
    , mUnidentified020(false)
{
    mUnidentified01C = lbl_806DEE90
        + nlRandomf(-lbl_806DEE94, lbl_806DEE94, &nlDefaultSeed);
}

UnidentifiedObject_8027AE14::~UnidentifiedObject_8027AE14()
{
}
