#ifndef GAME_FE_FEPOINTER_H
#define GAME_FE_FEPOINTER_H

#include "NL/nlFunction.h"
#include "NL/nlMath.h"
#include "types.h"

class TLInstance;

struct FEPointerEvent
{
    FEPointerEvent()
        : mPressed(false)
        , mReleased(false)
        , mSecondaryAction(false)
    {
        mPosition.x = -9999.9f;
        mPosition.y = -9999.9f;
        mIndex = -1;
    }

    /* 0x00 */ int mIndex;
    /* 0x04 */ nlVector2 mPosition;
    /* 0x0C */ bool mPressed;
    /* 0x0D */ bool mReleased;
    /* 0x0E */ bool mSecondaryAction;
}; // size 0x10

class FEPointerListener
{
public:
    typedef Function2<void, int, void*> Callback;

    FEPointerListener(void* context);
    virtual ~FEPointerListener();

    void ProcessPointerEvent(const FEPointerEvent* event);
    void SetPointerEnterCallback(const Callback& callback);
    void SetPointerLeaveCallback(const Callback& callback);
    void SetPointerInsideCallback(const Callback& callback);
    void SetPointerPressCallback(const Callback& callback);
    void SetPointerReleaseCallback(const Callback& callback);
    void Disable();
    void Enable() { mDisabled = false; }

    virtual void OnPointerUpdate(int index, void* context);
    virtual void OnPointerEnter(int index, void* context);
    virtual void OnPointerLeave(int index, void* context);
    virtual void OnPointerInside(int index, void* context);
    virtual void OnPointerPress(int index, void* context);
    virtual void OnPointerRelease(int index, void* context);
    virtual void OnPointerSecondaryAction(int index, void* context);
    virtual bool IsEnabled();
    virtual bool ContainsPoint(nlVector2 position) const = 0;

    /* 0x04 */ Callback mUpdateCallback;
    /* 0x0C */ Callback mEnterCallback;
    /* 0x14 */ Callback mLeaveCallback;
    /* 0x1C */ Callback mInsideCallback;
    /* 0x24 */ Callback mPressCallback;
    /* 0x2C */ Callback mReleaseCallback;
    /* 0x34 */ Callback mSecondaryActionCallback;
    /* 0x3C */ FEPointerEvent mPreviousEvents[4];
    /* 0x7C */ void* mContext;
    /* 0x80 */ bool mDisabled;
    /* 0x81 */ bool mIgnoreInputLock;
}; // size 0x84

class FEPointerRegion : public FEPointerListener
{
public:
    FEPointerRegion(void* context);
    virtual ~FEPointerRegion();

    virtual bool ContainsPoint(nlVector2 position) const;
    virtual void SetInstanceBounds(TLInstance* instance, bool useRotation, float offsetX, float offsetY, float scaleX, float scaleY);
    virtual void SetBounds(float minX, float maxX, float maxY, float minY);
    virtual void SetMinX(float value);
    virtual void SetMaxX(float value);
    virtual void SetMaxY(float value);
    virtual void SetMinY(float value);
    virtual float GetMinX() const;
    virtual float GetMaxX() const;
    virtual float GetMaxY() const;
    virtual float GetMinY() const;

private:
    /* 0x84 */ float mMinX;
    /* 0x88 */ float mMaxX;
    /* 0x8C */ float mMaxY;
    /* 0x90 */ float mMinY;
    /* 0x94 */ float mRotation;
    /* 0x98 */ float mPivotX;
    /* 0x9C */ float mPivotY;
}; // size 0xA0

inline void FEPointerRegion::SetBounds(
    float minX, float maxX, float maxY, float minY)
{
    mMinX = minX;
    mMaxX = maxX;
    mMaxY = maxY;
    mMinY = minY;
}

inline void FEPointerRegion::SetMinX(float value)
{
    mMinX = value;
}

inline void FEPointerRegion::SetMaxX(float value)
{
    mMaxX = value;
}

inline void FEPointerRegion::SetMaxY(float value)
{
    mMaxY = value;
}

inline void FEPointerRegion::SetMinY(float value)
{
    mMinY = value;
}

inline float FEPointerRegion::GetMinX() const
{
    return mMinX;
}

inline float FEPointerRegion::GetMaxX() const
{
    return mMaxX;
}

inline float FEPointerRegion::GetMaxY() const
{
    return mMaxY;
}

inline float FEPointerRegion::GetMinY() const
{
    return mMinY;
}

inline bool FEPointerListener::IsEnabled()
{
    return !mDisabled;
}

inline void FEPointerListener::Disable()
{
    mDisabled = true;
    FEPointerEvent event;
    for (int i = 0; i < 4; ++i)
        mPreviousEvents[i] = event;
}

#endif // GAME_FE_FEPOINTER_H
