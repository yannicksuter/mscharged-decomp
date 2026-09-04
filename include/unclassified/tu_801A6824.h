#ifndef UNCLASSIFIED_TU_801A6824_H
#define UNCLASSIFIED_TU_801A6824_H

class ScreenOverlay_801A6824
{
public:
    ScreenOverlay_801A6824()
        : mRate(0.0f)
        , mValue(0.0f)
        , mTarget(0.0f)
        , mMode(-1)
        , mActive(false)
    {
    }

    void Start(float rate, float target, int mode);
    void UpdateAndRender(float deltaTime);

    /* 0x00 */ float mRate;
    /* 0x04 */ float mValue;
    /* 0x08 */ float mTarget;
    /* 0x0C */ int mMode;
    /* 0x10 */ bool mActive;
}; // size: 0x14

extern ScreenOverlay_801A6824 gScreenOverlay_801A6824;

#endif // UNCLASSIFIED_TU_801A6824_H
