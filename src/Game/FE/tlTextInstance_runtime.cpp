#include "Game/FE/tlTextInstance.h"

#include "Game/FE/feText.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

const unsigned short* TLTextInstance::GetString() const
{
    const unsigned short* pWideTextString;
    if (m_OverloadFlags & 0x8)
    {
        unsigned long key = m_LocStrId;
        nlLocalization* loc = g_pLocalization;
        const unsigned short* locString;

        if (loc->m_LookupTable == 0)
        {
            locString = LocalizationTableNotFound;
        }
        else
        {
            nlLocalization::StringLookup* result = nlBSearch<nlLocalization::StringLookup, unsigned long>(key, loc->m_LookupTable, loc->m_pFile->StringCount);
            if (result != 0)
            {
                locString = loc->m_FirstString + result->StringOffset;
            }
            else
            {
                locString = MissingLocString;
            }
        }
        pWideTextString = locString;
    }
    else
    {
        pWideTextString = m_wcUserString;
    }

    return pWideTextString;
}

void TLTextInstance::Render(eGLView view, const nlColour& colour) const
{
    nlVector2 drawAt;
    const nlFont* pFont;
    nlVector3 position;

    // Retail loads the three components in reverse offset order (0x8, 0x4, 0x0) and
    // numbers the FPRs ascending with that order (z->f0, y->f1, x->f2), then stores
    // them to three dead frame slots. Declaring all three as named locals in reverse
    // order is what reproduces that here: the FPR numbers follow declaration order
    // while the loads follow the initialiser order.
    // A plain copy through GetNLVector3 leaves four rows differing only in the order of
    // two loads and two stores; a plain nlVec3Set, e[] arguments and e[] assignment are
    // the same four, while naming only one or two components or moving the ordering into
    // GetNLVector3 itself is no better (four to six rows).
    // The identifiers carry no semantic content, and the stripped DOL cannot show whether
    // retail named them - do not read this as evidence that it did. The predecessor's own
    // byte-exact Render has no component copy at this point, so it makes no statement
    // either way.
    const feVector3& tlPosition = GetPosition();
    float posZ = tlPosition.f.z;
    float posY = tlPosition.f.y;
    float posX = tlPosition.f.x;
    nlVec3Set(position, posX, posY, posZ);

    const FEText* component = (const FEText*)m_component;
    const FEFontResource* resource = component->m_pFeFontResource;
    if (!resource->m_bValid)
    {
        return;
    }

    const unsigned short* pWideTextString;
    if (m_OverloadFlags & 0x8)
    {
        unsigned long key = m_LocStrId;
        nlLocalization* loc = g_pLocalization;
        const unsigned short* locString;

        if (loc->m_LookupTable == 0)
        {
            locString = LocalizationTableNotFound;
        }
        else
        {
            nlLocalization::StringLookup* result = nlBSearch<nlLocalization::StringLookup, unsigned long>(key, loc->m_LookupTable, loc->m_pFile->StringCount);
            if (result != 0)
            {
                locString = loc->m_FirstString + result->StringOffset;
            }
            else
            {
                locString = MissingLocString;
            }
        }
        pWideTextString = locString;
    }
    else
    {
        pWideTextString = m_wcUserString;
    }

    if (pWideTextString != 0)
    {
        unsigned short* buffer = (unsigned short*)__alloca((nlStrLen<unsigned short>(pWideTextString) + 1) * sizeof(unsigned short));

        pFont = component->m_pFeFontResource->GetFontReference();
        FontCharString charString(pWideTextString, pFont, buffer);

        m_DrawInfo.String = charString.m_pString;
        nlTextBox::ProcessString(&charString, pFont, m_OverloadedAttributes.BoxSize, m_DrawOptions | nlTextBox::FlipY, m_DrawInfo.pMatrix, m_DrawInfo);
    }
    else if (m_pFontString == 0)
    {
        return;
    }

    float x;
    switch (m_DrawOptions & 0xF)
    {
    case nlTextBox::AlignLeft:
        x = 0.0f;
        break;
    case nlTextBox::AlignCenter:
        x = -m_OverloadedAttributes.BoxSize.x / 2.0f;
        break;
    case nlTextBox::AlignRight:
        x = -m_OverloadedAttributes.BoxSize.x;
        break;
    }

    float y;
    switch (m_DrawOptions & 0xF0)
    {
    case nlTextBox::VAlignTop:
        y = 0.0f;
        break;
    case nlTextBox::VAlignCenter:
        y = m_OverloadedAttributes.BoxSize.y / 2.0f;
        break;
    case nlTextBox::VAlignBottom:
        y = m_OverloadedAttributes.BoxSize.y;
        break;
    }

    drawAt.x = x;
    drawAt.y = y;

    if (m_UseScissorRect)
    {
        m_DrawInfo.pFont->SetScissorBox(m_ScissorRect);
    }

    nlTextBox::DrawString(m_DrawInfo, drawAt, colour, view);

    if (m_UseScissorRect)
    {
        m_DrawInfo.pFont->DisableScissorBox();
    }
}

void TLTextInstance::SetString(const unsigned short* utf16)
{
    m_wcUserString = utf16;
    m_pFontString = 0;
    m_OverloadFlags &= 0xFFFFFFF7;
}

void TLTextInstance::SetScissorBox(u16 left, u16 top, u16 width, u16 height)
{
    m_UseScissorRect = true;
    m_ScissorRect.X = left;
    m_ScissorRect.Y = top;
    m_ScissorRect.Width = width;
    m_ScissorRect.Height = height;
}
