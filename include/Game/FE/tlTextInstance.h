#ifndef _TLTEXTINSTANCE_H_
#define _TLTEXTINSTANCE_H_

#include "Game/FE/tlInstance.h"
#include "NL/gl/gl.h"
#include "NL/nlFont.h"
#include "NL/nlTextBox.h"

struct FETextLibObjectAttributes
{
    /* 0x0 */ nlColour EffectColour;
    /* 0x4 */ nlVector2 BoxSize;
};

class TLTextInstance : public TLInstance
{
public:
    void SetMatrix(nlMatrix4* pMatrix)
    {
        m_DrawInfo.pMatrix = pMatrix;
    }

    void SetStringId(const char* id);
    const unsigned short* GetString() const;
    void Render(eGLView view, const nlColour& colour) const;
    void SetString(const unsigned short* utf16);
    void SetScissorBox(u16 left, u16 top, u16 width, u16 height);

    /* 0x090 */ unsigned long m_LocStrId;
    /* 0x094 */ FETextLibObjectAttributes m_OverloadedAttributes;
    /* 0x0A0 */ unsigned long m_OverloadFlags;
    /* 0x0A4 */ mutable nlTextBox::StringDrawInfo m_DrawInfo;
    /* 0x0FC */ FontCharString* m_pFontString;
    /* 0x100 */ unsigned long m_DrawOptions;
    /* 0x104 */ const unsigned short* m_wcUserString;
    /* 0x108 */ bool m_UseScissorRect;
    /* 0x109 */ u8 pad_109;
    /* 0x10A */ nlFont::ScissorBox m_ScissorRect;
};

extern TLTextInstance UnidentifiedFallbackTextInstance;

#endif // _TLTEXTINSTANCE_H_
