#include "Game/FE/tlTextInstance.h"

#include "Game/FE/feText.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"

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

    GetPosition().GetNLVector3(position);

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

        pFont = component->m_pFeFontResource->m_pFontReference;
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
        x = 0.5f * -m_OverloadedAttributes.BoxSize.x;
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
        y = 0.5f * m_OverloadedAttributes.BoxSize.y;
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
