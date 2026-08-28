#ifndef GAME_TWEAK_VALUE_H
#define GAME_TWEAK_VALUE_H

#include "types.h"

class TweakValueBase_8052BF70
{
public:
    virtual ~TweakValueBase_8052BF70();

private:
    /* 0x04 */ u8 mUnidentified004[0x08];
}; // total size: 0x0C

class TweakValueImpl_804F4DC8 : public TweakValueBase_8052BF70
{
public:
    TweakValueImpl_804F4DC8(float* value = 0);

private:
    /* 0x0C */ float* m_pValue;

    friend class TweakValue_804F4DC8;
}; // total size: 0x10

class TweakValue_804F4DC8
{
public:
    void fn_8002D078(const char*, float, const char*, bool, float, float, float);

    operator float() const
    {
        return *mValue.m_pValue;
    }

private:
    /* 0x00 */ TweakValueImpl_804F4DC8 mValue;
}; // total size: 0x10

class TweakValueIntImpl_804FD898 : public TweakValueBase_8052BF70
{
public:
    TweakValueIntImpl_804FD898(int* value = 0);

private:
    /* 0x0C */ int* m_pValue;

    friend class TweakValueInt_804F4DC8;
}; // total size: 0x10

class TweakValueInt_804F4DC8
{
public:
    void fn_800757B4(const char*, int, const char*, bool, float, float, float);

    operator int() const
    {
        return *mValue.m_pValue;
    }

private:
    /* 0x00 */ TweakValueIntImpl_804FD898 mValue;
}; // total size: 0x10

#endif // GAME_TWEAK_VALUE_H
