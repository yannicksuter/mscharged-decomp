#ifndef GAME_TWEAK_VALUE_H
#define GAME_TWEAK_VALUE_H

#include "types.h"

class InterpreterCore;

class TweakValueBase_8052BF70
{
public:
    TweakValueBase_8052BF70();
    virtual ~TweakValueBase_8052BF70();
    virtual void UnidentifiedVirtual0C();
    virtual void UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14();
    virtual void UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();
    virtual void UnidentifiedVirtual20();
    virtual void UnidentifiedVirtual24();
    virtual void UnidentifiedVirtual28();
    virtual void UnidentifiedVirtual2C();

private:
    /* 0x04 */ u8 mUnidentified004[0x08];
}; // total size: 0x0C

class TweakValueImpl_804F4DC8 : public TweakValueBase_8052BF70
{
public:
    TweakValueImpl_804F4DC8(float* value = 0);
    virtual void UnidentifiedVirtual30();
    virtual void UnidentifiedVirtual34();
    virtual void UnidentifiedVirtual38();
    virtual float UnidentifiedVirtual3C();

public:
    /* 0x0C */ float* m_pValue;

    friend class InterpreterCore;
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
    virtual void UnidentifiedVirtual30();
    virtual void UnidentifiedVirtual34();
    virtual void UnidentifiedVirtual38();
    virtual int UnidentifiedVirtual3C();

public:
    /* 0x0C */ int* m_pValue;

    friend class InterpreterCore;
    friend class TweakValueInt_804F4DC8;
}; // total size: 0x10

class TweakValueBoolImpl_804F4538 : public TweakValueBase_8052BF70
{
public:
    TweakValueBoolImpl_804F4538(bool* value = 0);
    virtual void UnidentifiedVirtual30();
    virtual void UnidentifiedVirtual34();
    virtual void UnidentifiedVirtual38();
    virtual bool UnidentifiedVirtual3C();

public:
    /* 0x0C */ bool* m_pValue;

    friend class InterpreterCore;
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
