#include "NL/nlTextBox.h"
#include "NL/nlString.h"
#include "NL/nlTextEscape.h"

void nlTextBox::ProcessString(const FontCharString* pString, const nlFont* pFont, const nlVector2& BoxSize, unsigned long DrawOptions, const nlMatrix4* pMatrix, nlTextBox::StringDrawInfo& DrawInfo)
{
    unsigned long CurrentRowWidth;
    const unsigned short* pLastSpace;
    const unsigned short* pLastNonEsc;
    unsigned long WidthAtLastSpace;
    unsigned char FirstChar;
    unsigned char IsNewParagraph;
    const unsigned short* pCurrentChar;
    unsigned long CharWidth;

    DrawInfo.pFont = pFont;
    pCurrentChar = DrawInfo.String = pString->m_pString;
    DrawInfo.DrawOptions = DrawOptions;
    DrawInfo.RowCount = 0;
    DrawInfo.pMatrix = pMatrix;

    CurrentRowWidth = 0;
    pLastSpace = 0;
    pLastNonEsc = 0;
    WidthAtLastSpace = 0;
    FirstChar = 1;
    IsNewParagraph = 0;

    while (*pCurrentChar != 0)
    {
        if (*pCurrentChar == nlEscapeSequence::ESCAPE_BEGIN)
        {
            nlEscapeSequence esc(pCurrentChar);

            if (esc.m_Type == ESC_NON_BREAKING_SPACE)
            {
                unsigned long prevChar = FirstChar ? 0 : (pLastNonEsc != 0 ? (unsigned long)*pLastNonEsc : 0);
                CharWidth = pFont->GetCharWidth(' ', (unsigned short)prevChar);
            }
            else if (esc.m_Type == ESC_PARAGRAPH)
            {
                CharWidth = (unsigned long)BoxSize.x + 1;
                WidthAtLastSpace = CurrentRowWidth;
                IsNewParagraph = 1;
                pLastSpace = esc.m_pEnd - 1;
            }
            else
            {
                CharWidth = 0;
            }
            pCurrentChar = esc.m_pEnd - 1;
        }
        else
        {
            unsigned long prevChar = FirstChar ? 0 : (pLastNonEsc != 0 ? (unsigned long)*pLastNonEsc : 0);
            CharWidth = pFont->GetCharWidth(*pCurrentChar, (unsigned short)prevChar);
            pLastNonEsc = pCurrentChar;
            if (*pCurrentChar == ' ')
            {
                pLastSpace = pCurrentChar;
                WidthAtLastSpace = CurrentRowWidth;
            }
        }

        FirstChar = 0;

        if ((float)(CurrentRowWidth + CharWidth) > BoxSize.x && !(DrawOptions & 0x1000))
        {
            if (!(DrawOptions & DontWrapOnSpaces) && pLastSpace != 0)
            {
                CurrentRowWidth = WidthAtLastSpace;
                pCurrentChar = pLastSpace + 1;
                pLastSpace = 0;
                WidthAtLastSpace = 0;
            }

            int xOffset;
            if (DrawOptions & 0x3)
            {
                int remaining = (int)(BoxSize.x - (float)CurrentRowWidth);
                xOffset = remaining >> ((DrawOptions & AlignCenter) ? 1u : 0u);
            }
            else
            {
                xOffset = 0;
            }

            DrawInfo.Rows[DrawInfo.RowCount].XOffset = xOffset;
            unsigned short charIdx = (unsigned short)(pCurrentChar - pString->m_pString);
            DrawInfo.RowCount++;
            DrawInfo.Rows[DrawInfo.RowCount].FirstChar = charIdx;

            if (*pCurrentChar != ' ')
            {
                pCurrentChar--;
            }
            CurrentRowWidth = 0;
        }
        else
        {
            CurrentRowWidth += CharWidth;
        }

        if (IsNewParagraph)
        {
            CurrentRowWidth = 0;
            IsNewParagraph = 0;
        }

        pCurrentChar++;
    }

    int xOffset;
    if (DrawOptions & 0x3)
    {
        xOffset = (int)(BoxSize.x - (float)CurrentRowWidth) >> ((DrawOptions & AlignCenter) ? 1u : 0u);
    }
    else
    {
        xOffset = 0;
    }

    DrawInfo.Rows[DrawInfo.RowCount].XOffset = xOffset;
    DrawInfo.RowCount++;

    s32 strLen = nlStrLen(pString->m_pString);
    DrawInfo.Rows[DrawInfo.RowCount].FirstChar = (unsigned short)strLen;
    DrawInfo.Rows[0].FirstChar = 0;

    if (DrawOptions & 0x30)
    {
        int TotalHeight = (int)DrawInfo.RowCount * (int)pFont->m_Metrics.Height;
        if ((float)TotalHeight > BoxSize.y)
        {
            DrawInfo.YOffset = 0;
        }
        else if (DrawOptions & VAlignCenter)
        {
            DrawInfo.YOffset = (signed short)((int)(BoxSize.y / 2.0f) - (TotalHeight >> 1));
        }
        else
        {
            DrawInfo.YOffset = (signed short)((int)BoxSize.y - TotalHeight);
        }
    }
    else
    {
        DrawInfo.YOffset = 0;
    }
}

void nlTextBox::DrawString(const nlTextBox::StringDrawInfo& DrawInfo, const nlVector2& DrawAt, const nlColour& Color, eGLView View)
{
    int yDir = 1;
    const nlFont* pFont = DrawInfo.pFont;
    nlVector2 CurrentPos = DrawAt;
    if (DrawInfo.DrawOptions & FlipY)
    {
        yDir = -1;
    }
    CurrentPos.y += (float)(yDir * DrawInfo.YOffset);

    int ascentAdj;
    if (DrawInfo.DrawOptions & UseFullHeight)
    {
        ascentAdj = pFont->m_Metrics.InternalLeading;
    }
    else
    {
        ascentAdj = 0;
    }

    const Row* pIter = DrawInfo.Rows;
    int vertOffset = yDir * (int)pFont->m_Metrics.Ascent - ascentAdj;
    nlColour overridecolour;
    overridecolour.c[0] = Color.c[0];
    overridecolour.c[1] = Color.c[1];
    overridecolour.c[2] = Color.c[2];
    overridecolour.c[3] = Color.c[3];
    unsigned long hMatrix;

    CurrentPos.y += (float)vertOffset;
    overridecolour.c[3] = 0;
    unsigned long row = 0;

    while (row < DrawInfo.RowCount)
    {
        const Row& CurrentRow = *pIter;
        CurrentPos.x = DrawAt.x + (float)CurrentRow.XOffset;

        unsigned long h;
        const nlMatrix4* pMatrix = DrawInfo.pMatrix;
        if (pMatrix)
        {
            h = glAllocMatrix();
            if (h + 0x10000 != 0xFFFF)
            {
                glSetMatrix(h, *pMatrix);
            }
            hMatrix = h;
        }

        unsigned short startIdx = CurrentRow.FirstChar;
        unsigned long* matArg = DrawInfo.pMatrix ? &hMatrix : 0;
        {
            FontCharString fontCharStr;
            fontCharStr.m_InternalBuffer = 0;
            fontCharStr.m_pString = (unsigned short*)(DrawInfo.String + startIdx);

            int length = (&CurrentRow + 1)->FirstChar - startIdx;

            DrawInfo.pFont->DrawString(View, fontCharStr, CurrentPos, Color, Color, length, nlFont::PASS_TextAndEffect, (DrawInfo.DrawOptions & FlipY) != 0, matArg, &overridecolour);
        }

        if (overridecolour.c[3] == 0)
        {
            overridecolour.c[0] = Color.c[0];
            overridecolour.c[1] = Color.c[1];
            overridecolour.c[2] = Color.c[2];
            overridecolour.c[3] = Color.c[3];
        }

        pIter++;
        row++;
        CurrentPos.y += (float)(yDir * pFont->m_Metrics.Height);
    }
}
