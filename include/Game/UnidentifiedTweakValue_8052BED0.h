#ifndef GAME_UNIDENTIFIED_TWEAK_VALUE_8052BED0_H
#define GAME_UNIDENTIFIED_TWEAK_VALUE_8052BED0_H

#include "Game/TweakValue.h"

class UnidentifiedTweakValue_8052BED0 : public TweakValueBool_804F4578
{
public:
    UnidentifiedTweakValue_8052BED0(const char* name, const char* category,
        void (*callback)(), bool arg3)
        : TweakValueBool_804F4578(name, category, false, false)
        , next(0)
        , mUnidentified010(arg3)
        , mUnidentified014(callback)
    {
        mUnidentified009 = false;
        fn_802C4E78();
    }

    virtual ~UnidentifiedTweakValue_8052BED0();
    virtual int UnidentifiedVirtual0C();
    void fn_802C4E78();

    static void operator delete(void* pointer)
    {
        lbl_806E1E58->m_Pool2.Free(pointer);
    }

    /* 0x0C */ UnidentifiedTweakValue_8052BED0* next;
    /* 0x10 */ bool mUnidentified010;
    /* 0x14 */ void (*mUnidentified014)();
}; // size: 0x18

#endif // GAME_UNIDENTIFIED_TWEAK_VALUE_8052BED0_H
