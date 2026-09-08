#ifndef GAME_FE_FEPOINTERBUTTON_H
#define GAME_FE_FEPOINTERBUTTON_H

#include "Game/FE/fePointer.h"

class FEPointerButton : public FEPointerRegion
{
public:
    FEPointerButton();
    FEPointerButton(void* context);

    bool HasOtherPointerState(int value, unsigned int which) const;
    int GetPointerState(unsigned int which)
    {
        return mPointerStates[which];
    }
    void SetPointerState(int value, unsigned int which)
    {
        mPointerStates[which] = value;
    }
    void PlayHoverFeedback(int index);
    void HandlePointerEvent(const FEPointerEvent* event);

    virtual void OnPointerUpdate(int index, void* context);
    virtual void OnPointerEnter(int index, void* context);
    virtual void OnPointerLeave(int index, void* context);
    virtual void OnPointerInside(int index, void* context);
    virtual void OnPointerPress(int index, void* context);
    virtual void OnPointerRelease(int index, void* context);
    virtual void OnPointerSecondaryAction(int index, void* context);

private:
    /* 0xA0 */ int mPointerStates[4];

public:
    /* 0xB0 */ bool mSpeakerEnabled;
}; // size 0xB4

#endif // GAME_FE_FEPOINTERBUTTON_H
