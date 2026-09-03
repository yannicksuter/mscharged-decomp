#ifndef UNCLASSIFIED_TU_80300104_H
#define UNCLASSIFIED_TU_80300104_H

#include "NL/nlFunction.h"
#include "NL/nlMath.h"
#include "types.h"

class TLInstance;

struct TU80300104Event
{
    TU80300104Event()
        : mFlag0(false)
        , mFlag1(false)
        , mFlag2(false)
    {
        mPosition.x = -9999.9f;
        mPosition.y = -9999.9f;
        mIndex = -1;
    }

    /* 0x00 */ int mIndex;
    /* 0x04 */ nlVector2 mPosition;
    /* 0x0C */ bool mFlag0;
    /* 0x0D */ bool mFlag1;
    /* 0x0E */ bool mFlag2;
}; // size 0x10

class TU80300104Base
{
public:
    typedef Function2<void, int, void*> Callback;

    TU80300104Base(void* context);
    virtual ~TU80300104Base();

    void fn_80300594(const TU80300104Event* event);
    void fn_803007C0(const Callback& callback);
    void fn_80300864(const Callback& callback);
    void fn_80300908(const Callback& callback);
    void fn_803009AC(const Callback& callback);
    void fn_80300A50(const Callback& callback);
    void fn_80206B54();

    virtual void fn_80301BE8(int index, void* context);
    virtual void fn_80301BA8(int index, void* context);
    virtual void fn_80301CE8(int index, void* context);
    virtual void fn_80301C28(int index, void* context);
    virtual void fn_80301C68(int index, void* context);
    virtual void fn_80301D28(int index, void* context);
    virtual void fn_80301CA8(int index, void* context);
    virtual bool fn_80219668();
    virtual bool fn_8030131C(nlVector2 position) const = 0;

    /* 0x04 */ Callback mCallback0;
    /* 0x0C */ Callback mCallback1;
    /* 0x14 */ Callback mCallback2;
    /* 0x1C */ Callback mCallback3;
    /* 0x24 */ Callback mCallback4;
    /* 0x2C */ Callback mCallback5;
    /* 0x34 */ Callback mCallback6;
    /* 0x3C */ TU80300104Event mPreviousEvents[4];
    /* 0x7C */ void* mContext;
    /* 0x80 */ bool mDisabled;
    /* 0x81 */ bool mIgnoreInputLock;
}; // size 0x84

class TU80300104Component : public TU80300104Base
{
public:
    TU80300104Component(void* context);
    virtual ~TU80300104Component();

    virtual bool fn_8030131C(nlVector2 position) const;
    virtual void fn_80300D74(TLInstance* instance, bool useRotation, float offsetX, float offsetY, float scaleX, float scaleY);
    virtual void fn_801CD9D0(float minX, float maxX, float maxY, float minY);
    virtual void fn_802196A8(float value);
    virtual void fn_802141F4(float value);
    virtual void fn_802196A0(float value);
    virtual void fn_80219698(float value);
    virtual float fn_80219690() const;
    virtual float fn_80219688() const;
    virtual float fn_80219680() const;
    virtual float fn_80219678() const;

private:
    /* 0x84 */ float mMinX;
    /* 0x88 */ float mMaxX;
    /* 0x8C */ float mMaxY;
    /* 0x90 */ float mMinY;
    /* 0x94 */ float mRotation;
    /* 0x98 */ float mPivotX;
    /* 0x9C */ float mPivotY;
}; // size 0xA0

inline void TU80300104Component::fn_801CD9D0(
    float minX, float maxX, float maxY, float minY)
{
    mMinX = minX;
    mMaxX = maxX;
    mMaxY = maxY;
    mMinY = minY;
}

inline void TU80300104Component::fn_802196A8(float value)
{
    mMinX = value;
}

inline void TU80300104Component::fn_802141F4(float value)
{
    mMaxX = value;
}

inline void TU80300104Component::fn_802196A0(float value)
{
    mMaxY = value;
}

inline void TU80300104Component::fn_80219698(float value)
{
    mMinY = value;
}

inline float TU80300104Component::fn_80219690() const
{
    return mMinX;
}

inline float TU80300104Component::fn_80219688() const
{
    return mMaxX;
}

inline float TU80300104Component::fn_80219680() const
{
    return mMaxY;
}

inline float TU80300104Component::fn_80219678() const
{
    return mMinY;
}

inline bool TU80300104Base::fn_80219668()
{
    return !mDisabled;
}

#endif // UNCLASSIFIED_TU_80300104_H
