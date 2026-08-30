#include "Game/CharacterTweaks.h"

extern "C" void fn_80073A48(void*, const char*, const char*);
extern "C" u8 lbl_8056BA00[];

inline TweakValueImpl_804F4DC8::TweakValueImpl_804F4DC8(float* value)
    : m_pValue(value)
{
}

GoalieTweaks::GoalieTweaks(const char* name, const char* category)
    : TweaksBase(name)
    , mUnidentified358(category)
{
    Init();
    fn_80073A48(lbl_8056BA00, mszFileName, mUnidentified358);
}

GoalieTweaks::~GoalieTweaks()
{
}
