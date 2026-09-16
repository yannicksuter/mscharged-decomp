#ifndef GAME_TWEAK_CALLBACK_H
#define GAME_TWEAK_CALLBACK_H

#include "Game/TweakValue.h"

class TweakCallback : public TweakValueBool
{
public:
    TweakCallback(const char* name, const char* category,
        void (*callback)(), bool arg3)
        : TweakValueBool(name, category, false, false)
        , next(0)
        , mUnidentified010(arg3)
        , mCallback(callback)
    {
        mFormatName = false;
        Register();
    }

    virtual ~TweakCallback();
    virtual int GetValueType()
    {
        int result = 6;
        if (mUnidentified010)
        {
            result = 7;
        }
        return result;
    }
    void Register();

    static void operator delete(void* pointer)
    {
        gTweakValueAllocator->m_Pool2.Free(pointer);
    }

    /* 0x0C */ TweakCallback* next;
    /* 0x10 */ bool mUnidentified010;
    /* 0x14 */ void (*mCallback)();
}; // size: 0x18

#endif // GAME_TWEAK_CALLBACK_H
