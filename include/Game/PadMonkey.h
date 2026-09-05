#ifndef GAME_PAD_MONKEY_H
#define GAME_PAD_MONKEY_H

#include "NL/platpad.h"
#include "types.h"

extern s32* lbl_806E2288;
extern "C" int fn_802C03FC(int button);
extern "C" int fn_802C06C8(int buttonIndex);

class PadMonkey : public PadBackend
{
public:
    PadMonkey(int padIndex);
    virtual int UnidentifiedClassID() { return sClassID; }
    virtual ~PadMonkey() { }

    virtual bool IsConnected();
    virtual bool IsPressed(int button, bool remap);
    virtual float GetPressure(int button, bool remap);
    virtual float GetPressureDerivative(int button, bool remap);
    virtual bool PlatJustPressed(int button, bool remap);
    virtual bool PlatJustReleased(int button, bool remap);
    virtual int GetButtonIndex(int button, bool remap) = 0;
    virtual int GetButtonMask(int buttonIndex);
    virtual float GetButtonStateTime(int button, bool remap);
    virtual float AnalogLeftX();
    virtual float AnalogLeftY();
    virtual float AnalogRightX();
    virtual float AnalogRightY();
    virtual bool RumbleActive();
    virtual void StartRumble(float duration, float intensity, float frequency);
    virtual void StopRumble();
    virtual void Update(float dt);
    virtual int GetButtonCount() = 0;

    void SetButtonChance(int button, float pct);

    static int sClassID;

    /* 0x19 */ bool m_isConnected;
    /* 0x1C */ float* m_prevPressurePtr;
    /* 0x20 */ float* m_currPressurePtr;
    /* 0x24 */ float m_unk_0x24;
    /* 0x28 */ float m_unk_0x28;
    /* 0x2C */ float m_unk_0x2C;
    /* 0x30 */ float m_unk_0x30;
    /* 0x34 */ float m_unk_0x34;
    /* 0x38 */ float* m_buttonChance;
}; // size 0x3C

class PadMonkey_80375EEC : public PadMonkey
{
public:
    PadMonkey_80375EEC(int padIndex);
    virtual ~PadMonkey_80375EEC() { }

    virtual int GetButtonIndex(int button, bool remap)
    {
        return fn_802C03FC(remap ? lbl_806E2288[button] : button);
    }
    virtual int GetButtonMask(int buttonIndex)
    {
        return fn_802C06C8(buttonIndex);
    }
    virtual int GetButtonCount() { return 16; }

    /* 0x3C */ float m_prevPressure[16];
    /* 0x7C */ float m_currPressure[16];
    /* 0xBC */ float m_buttonChances[16];
}; // size 0xFC

#endif // GAME_PAD_MONKEY_H
