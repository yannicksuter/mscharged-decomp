#include "NL/nlFont.h"
#include "NL/nlList.h"
#include "NL/nlPrint.h"
#include "NL/nlstring_tmpl.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glStruct.h"

nlFont::nlFont()
{
}

nlFont::~nlFont()
{
    ::operator delete[](m_pKernTable);
    m_pKernTable = 0;
    if (m_pExtendedGlyphs != 0)
    {
        delete[] m_pExtendedGlyphs;
    }
}

static inline void ParseKernPairs(nlFont* self, nlFont::GlyphInfo* pInfo, char* pToken, unsigned short Base, nlListSlotPoolHigh<nlFont::KernPair>& KernList)
{
    pInfo->HasKernPairs = 1;
    nlFont::KernPair kp;
    char* pKernToken = nlStrChr(pToken, ' ');
    pKernToken++;
    while ((unsigned long)pKernToken != 1)
    {
        kp.s.A = Base;
        int nB;
        if (pKernToken[1] != ' ')
        {
            nB = atoi(pKernToken);
        }
        else
        {
            nB = pKernToken[0];
        }
        kp.s.B = (unsigned short)nB;
        pKernToken = nlStrChr(pKernToken, ' ') + 1;
        kp.Kern = atoi(pKernToken);
        KernList.AddEntry(kp);
        self->m_KernTableSize++;
        pKernToken = nlStrChr(pKernToken, ' ') + 1;
    }
}

static inline nlFont::GlyphInfo* AddExtendedGlyph(nlFont* self, nlListSlotPoolHigh<nlFont::GlyphInfo>& GlyphList)
{
    ListEntry<nlFont::GlyphInfo>* pEntry = GlyphList.Allocate();
    nlListAddStart<ListEntry<nlFont::GlyphInfo> >(&GlyphList.m_Head, pEntry, &GlyphList.m_Tail);
    self->m_ExtendedGlyphCount++;
    return &pEntry->entry;
}

unsigned char nlFont::Load(const char* szFontName, char* pFontDescData, unsigned long HashId)
{
    unsigned long CurrentPage;
    unsigned long CurrentTexelX;
    unsigned long CurrentTexelY;
    unsigned long RenderHeight;
    unsigned short RenderAscent;
    char* pCurrentLine;

    nlStrNCpy(m_FontName, szFontName, 0x20);

    pCurrentLine = pFontDescData;

    nlListSlotPoolHigh<nlFont::KernPair> KernList(0x10, 0x10);
    m_KernTableSize = 0;

    nlListSlotPoolHigh<nlFont::GlyphInfo> ExtendedGlyphList(0x10, 0x10);
    m_ExtendedGlyphCount = 0;
    m_pExtendedGlyphs = NULL;

    char* pEOL;
    char* pToken;
    unsigned short Character;
    nlFont::GlyphInfo* pInfo;
    unsigned short Base;
    nlFont::KernPair* pKP;

    float FormatVersion = 0.0f;

    CurrentPage = 0;
    CurrentTexelX = 0;
    CurrentTexelY = 0;
    RenderHeight = 0;
    RenderAscent = 0;
    m_bScissorBox = false;
    m_Metrics.FontName = HashId;

    for (;;)
    {
        if (nlToUpper(*pCurrentLine) == 'E')
            break;
        pEOL = nlStrChr(pCurrentLine, '\r');
        if (pEOL != NULL)
        {
            *pEOL = 0;
        }

        pToken = nlStrChr(pCurrentLine, ' ') + 1;

        switch (nlToUpper(*pCurrentLine))
        {
        case 'V':
            FormatVersion = atof(pToken);
            break;

        case 'P':
        {
            if (nlToUpper(pCurrentLine[4]) == 'S')
            {
                m_PageSize = atoi(pToken);
                m_InvTexSize = 1.0f / (float)m_PageSize;

                pCurrentLine = nlStrChr(pToken, ' ');
                pCurrentLine++;
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                m_PageCount = atoi(pCurrentLine);

                pCurrentLine = nlStrChr(pCurrentLine, ' ');
                pCurrentLine++;
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                switch (nlToLower(*pCurrentLine))
                {
                case 'c':
                    m_TextureType = Colour;
                    break;
                case 'g':
                    m_TextureType = Greyscale;
                    break;
                case 's':
                    m_TextureType = SplitFX;
                    break;
                default:
                    break;
                }

                pCurrentLine = nlStrChr(pCurrentLine, ' ');
                pCurrentLine++;
                pCurrentLine = nlStrChr(pCurrentLine, ' ');
                switch (nlToLower(pCurrentLine[1]))
                {
                case 'e':
                    m_Distribution = English;
                    break;
                case 'i':
                    m_Distribution = InOrder;
                    break;
                default:
                    break;
                }
            }
            else if (nlToUpper(pCurrentLine[4]) == 'B')
            {
                CurrentPage++;
            }
            break;
        }

        case 'H':
        {
            m_Metrics.Height = (unsigned short)atoi(pToken);

            pCurrentLine = nlStrChr(pToken, ' ');
            pCurrentLine++;
            pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
            RenderHeight = atoi(pCurrentLine);

            pCurrentLine = nlStrChr(pCurrentLine, ' ');
            pCurrentLine++;
            pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
            m_Metrics.Ascent = (unsigned short)atoi(pCurrentLine);

            pCurrentLine = nlStrChr(pCurrentLine, ' ');
            pCurrentLine++;
            pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
            RenderAscent = (unsigned short)atoi(pCurrentLine);

            pCurrentLine = nlStrChr(pCurrentLine, ' ');
            pCurrentLine++;
            pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
            m_Metrics.InternalLeading = (unsigned short)atoi(pCurrentLine);
            break;
        }

        case 'C':
        {
            m_Metrics.Spacing = (float)atoi(pToken) / 100.0f;

            pCurrentLine = nlStrChr(pToken, ' ');
            pCurrentLine++;
            pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
            m_Metrics.LineHeight = (float)atoi(pCurrentLine) / 100.0f;
            break;
        }

        case 'G':
        {
            int nChar;
            if (pToken[1] != ' ')
            {
                nChar = atoi(pToken);
            }
            else
            {
                nChar = pToken[0];
            }
            Character = (unsigned short)nChar;

            if (Character < 0x7F)
            {
                pInfo = m_GlyphLookup + Character - 0x20;
            }
            else
            {
                pInfo = AddExtendedGlyph(this, ExtendedGlyphList);
            }

            pInfo->UnicodeChar = Character;
            pInfo->HasKernPairs = 0;

            pCurrentLine = nlStrChr(pToken, ' ');
            pCurrentLine++;
            pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
            pInfo->Advance = (unsigned char)atoi(pCurrentLine);

            pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
            pInfo->RenderWidth = (unsigned char)atoi(pCurrentLine);

            pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
            pInfo->Offset = (signed char)atoi(pCurrentLine);

            bool usePackedGlyphs = 1.2f - FormatVersion > 0.0001f;
            if (usePackedGlyphs)
            {
                if ((CurrentTexelX + pInfo->RenderWidth) > m_PageSize)
                {
                    CurrentTexelX = 0;
                    CurrentTexelY += RenderHeight;
                    if ((CurrentTexelY + RenderHeight) > m_PageSize)
                    {
                        CurrentTexelX = 0;
                        CurrentTexelY = 0;
                        CurrentPage++;
                    }
                }

                pInfo->RenderAscent = RenderAscent;
                pInfo->RenderHeight = RenderHeight;
            }
            else
            {
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pInfo->RenderHeight = (unsigned char)atoi(pCurrentLine);
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pInfo->RenderAscent = (unsigned char)atoi(pCurrentLine);
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                CurrentTexelX = atoi(pCurrentLine);
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                CurrentTexelY = atoi(pCurrentLine);
            }

            nlVec2Set(pInfo->uv, (float)CurrentTexelX * m_InvTexSize, (float)CurrentTexelY * m_InvTexSize);
            nlVec2Set(pInfo->uvEnd, pInfo->uv.x + (pInfo->RenderWidth - 1) * m_InvTexSize, pInfo->uv.y + (pInfo->RenderHeight - 1) * m_InvTexSize);
            pInfo->Page = CurrentPage;
            CurrentTexelX += pInfo->RenderWidth;
            break;
        }

        case 'K':
        {
            if (m_ExtendedGlyphCount != 0 && m_pExtendedGlyphs == NULL)
            {
                m_pExtendedGlyphs = new (8, false) GlyphInfo[m_ExtendedGlyphCount];
                pInfo = m_pExtendedGlyphs;
                while (ExtendedGlyphList.m_Head != NULL)
                {
                    ExtendedGlyphList.RemoveStart(pInfo);
                    pInfo++;
                }
                nlQSort<nlFont::GlyphInfo>(m_pExtendedGlyphs, m_ExtendedGlyphCount, nlFont::GlyphInfo::SortProc);
            }

            int nBase;
            if (pToken[1] != ' ')
            {
                nBase = atoi(pToken);
            }
            else
            {
                nBase = pToken[0];
            }
            Base = (unsigned short)nBase;

            unsigned short c;
            if (Base <= 0x7F)
            {
                c = Base;
            }
            else
            {
                c = GetExtendedFontChar(Base);
            }
            if (c > 0x7F)
            {
                pInfo = &m_pExtendedGlyphs[c - 0x80];
            }
            else
            {
                pInfo = &m_GlyphLookup[c - 0x20];
            }

            ParseKernPairs(this, pInfo, pToken, Base, KernList);
            break;
        }

        default:
            break;
        }

        pCurrentLine = pEOL + 2;
    }

    if (m_KernTableSize != 0)
    {
        m_pKernTable = (KernPair*)nlMalloc(m_KernTableSize * sizeof(KernPair), 8, false);
        pKP = m_pKernTable;

        while (KernList.m_Head != NULL)
        {
            KernList.RemoveStart(pKP++);
        }

        nlQSort<nlFont::KernPair>(m_pKernTable, m_KernTableSize, nlFont::KernPair::SortProc);
    }
    else
    {
        m_pKernTable = NULL;
    }

    if (m_ExtendedGlyphCount != 0 && m_pExtendedGlyphs == NULL)
    {
        m_pExtendedGlyphs = new (8, false) GlyphInfo[m_ExtendedGlyphCount];
        pInfo = m_pExtendedGlyphs;
        while (ExtendedGlyphList.m_Head != NULL)
        {
            ExtendedGlyphList.RemoveStart(pInfo);
            pInfo++;
        }
        nlQSort<nlFont::GlyphInfo>(m_pExtendedGlyphs, m_ExtendedGlyphCount, nlFont::GlyphInfo::SortProc);
    }

    static char TextureNameFormat[] = "%s_%d";
    char sHashFontName[265] = { 0 };
    unsigned long page;
    for (page = 0; page < m_PageCount; page++)
    {
        nlStrNCpy(sHashFontName, szFontName, 0x109);
        nlSNPrintf(sHashFontName, 0x109, TextureNameFormat, sHashFontName, page + 1);
        m_TextureHandles[page] = nlStringHash(sHashFontName);

        if (m_TextureType == SplitFX)
        {
            nlStrNCat(sHashFontName, sHashFontName, "e", 0x109);
            m_EffectTextureHandles[page] = nlStringHash(sHashFontName);
        }
    }

    return 1;
}

template <typename GlyphLookup>
static inline unsigned long CalculateCharWidth(const nlFont& font, const GlyphLookup& glyphLookup, unsigned short FontChar, unsigned short PrevFontChar)
{
    const nlFont::GlyphInfo* pGlyph = &glyphLookup.GetGlyphInfo(FontChar);

    signed char offset = pGlyph->Offset;
    unsigned short prevChar = PrevFontChar;
    unsigned char advance = pGlyph->Advance;
    unsigned long ret = advance + offset;

    if (prevChar != 0)
    {
        const nlFont::GlyphInfo* pPrevGlyph = &glyphLookup.GetGlyphInfo(prevChar);

        if (pPrevGlyph->HasKernPairs)
        {
            nlFont::KernPair kp = { { PrevFontChar, FontChar }, 0 };
            nlFont::KernPair* pFoundKP = nlBSearch<nlFont::KernPair, nlFont::KernPair>(kp, font.m_pKernTable, font.m_KernTableSize);
            if (pFoundKP != 0)
            {
                ret += pFoundKP->Kern;
            }
        }
    }

    return (unsigned long)(ret * font.m_Metrics.Spacing);
}

// Shared lookup state for one string traversal.
class FontStringContext
{
public:
    typedef nlFont::GlyphInfo GlyphInfo;

    FontStringContext(const nlFont& font)
        : m_Font(font)
        , m_EscapeBegin(nlEscapeSequence::ESCAPE_BEGIN)
        , m_Glyphs(font.m_GlyphLookup)
        , m_Fallback(m_Glyphs['?' - 0x20])
    {
    }

    unsigned long GetEscapeBegin() const { return m_EscapeBegin; }

    const GlyphInfo& GetGlyphInfo(unsigned short c) const
    {
        const GlyphInfo& glyph = c > 0x7F ? m_Font.m_pExtendedGlyphs[c - 0x80] : m_Glyphs[c - 0x20];
        return glyph.IsValid() ? glyph : m_Fallback;
    }

    unsigned long GetCharWidth(unsigned short FontChar, unsigned short PrevFontChar) const
    {
        return CalculateCharWidth(m_Font, *this, FontChar, PrevFontChar);
    }

private:
    const nlFont& m_Font;
    unsigned long m_EscapeBegin;
    const GlyphInfo* m_Glyphs;
    const GlyphInfo& m_Fallback;
};

void nlFont::DrawString(GLView* View, const FontCharString& Text, const nlVector2& Position, const nlColour& Colour, const nlColour& EffectColour, int Length, nlFont::TextPass Passes, bool FlipY, unsigned long* pMatrix, nlColour* pOverrideColour) const
{
    gl_ScreenInfo* pScreenInfo = glGetScreenInfo();
    float PixelCenter = pScreenInfo->PixelCentre;
    float StartingX = Position.x + PixelCenter;
    float StartingY = Position.y + PixelCenter;
    nlVector2 CurrentPosition;

    int StringLength = Length == -1 ? nlStrLen(Text.m_pString) : Length;

    const GlyphInfo* pGlyph;
    glPoly2* pQuads = (glPoly2*)__alloca((unsigned long)(StringLength * sizeof(glPoly2)));
    glPoly2* pCurrentQuad = pQuads;

    unsigned long HandledChars = 0;
    unsigned long CurrentPage = 0;
    const unsigned short* pCurrentChar;

    glSetDefaultState(false);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetRasterState(GLS_AlphaTest, 1);
    glSetRasterState(GLS_AlphaTestRef, 0);
    glSetRasterState(GLS_Culling, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    nlVector4 Scissor;
    Scissor.x = m_scissorBox.X;
    Scissor.y = m_scissorBox.Y;
    Scissor.z = m_scissorBox.Width;
    Scissor.w = m_scissorBox.Height;

    nlColour OverrideColour = Colour;
    if (pOverrideColour != 0 && pOverrideColour->c[3] != 0)
    {
        OverrideColour = *pOverrideColour;
    }

    FontStringContext glyphLookup(*this);
    const unsigned short* PushColourIndex = 0;
    nlColour LastPushedColour = OverrideColour;
    nlVec2Set(CurrentPosition, StartingX, StartingY);

    while (HandledChars < (unsigned long)StringLength)
    {
        bool useEffectTextures = false;
        if (Passes == PASS_Effect && m_TextureType == SplitFX)
        {
            useEffectTextures = true;
        }

        const unsigned long* textureHandles = useEffectTextures ? m_EffectTextureHandles : m_TextureHandles;
        glSetCurrentTexture(textureHandles[CurrentPage], GLTT_Diffuse);

        CurrentPosition.x = StartingX;
        int i = 0;
        pCurrentChar = Text.m_pString;

        while (*pCurrentChar != 0 && HandledChars < (unsigned long)StringLength && i < (unsigned long)StringLength)
        {
            unsigned long Char = *pCurrentChar;
            if (Char == glyphLookup.GetEscapeBegin())
            {
                nlEscapeSequence escape(pCurrentChar);
                switch (escape.m_Type)
                {
                case ESC_COLOUR:
                    OverrideColour = escape.GetExtendedColour();

                    if (PushColourIndex < pCurrentChar)
                    {
                        if (OverrideColour.c[3] == 0)
                        {
                            LastPushedColour = Colour;
                            LastPushedColour.c[3] = 0;
                        }
                        else
                        {
                            LastPushedColour = OverrideColour;
                            LastPushedColour.c[3] = 0xFF;
                        }
                        PushColourIndex = pCurrentChar;
                    }

                    if (OverrideColour.c[3] == 0)
                    {
                        OverrideColour = Colour;
                    }
                    break;

                case ESC_NON_BREAKING_SPACE:
                {
                    const GlyphInfo* pSpaceGlyph = &glyphLookup.GetGlyphInfo(' ');
                    CurrentPosition.x += (float)((int)pSpaceGlyph->Offset + (int)pSpaceGlyph->Advance);
                }
                    break;
                default:
                    break;
                }

                i += (escape.m_pEnd - pCurrentChar) - 1;
                if (CurrentPage == 0)
                {
                    HandledChars += escape.m_pEnd - pCurrentChar;
                }
                pCurrentChar = escape.m_pEnd - 1;
            }
            else
            {
                pGlyph = &glyphLookup.GetGlyphInfo(Char);
                unsigned long Page = pGlyph->Page;
                CurrentPosition.x += (float)pGlyph->Offset;
                if (Page == CurrentPage)
                {
                    int renderAscent = FlipY ? -pGlyph->RenderAscent : pGlyph->RenderAscent;
                    CurrentPosition.y = StartingY - (float)renderAscent;
                    pCurrentQuad->m_pos[1].x = CurrentPosition.x;
                    pCurrentQuad->m_pos[0].x = CurrentPosition.x;

                    pCurrentQuad->m_pos[2].x = pCurrentQuad->m_pos[3].x = CurrentPosition.x + (float)pGlyph->RenderWidth - 1.0f;

                    pCurrentQuad->m_pos[3].y = CurrentPosition.y;
                    pCurrentQuad->m_pos[0].y = CurrentPosition.y;

                    int renderHeight;
                    if (FlipY)
                    {
                        renderHeight = -(pGlyph->RenderHeight - 1);
                    }
                    else
                    {
                        renderHeight = pGlyph->RenderHeight - 1;
                    }

                    pCurrentQuad->m_pos[1].y = pCurrentQuad->m_pos[2].y = CurrentPosition.y + (float)renderHeight;

                    pCurrentQuad->depth = 0.0f;

                    pCurrentQuad->m_uv[0].x = pCurrentQuad->m_uv[1].x = pGlyph->uv.x;

                    pCurrentQuad->m_uv[2].x = pCurrentQuad->m_uv[3].x = pGlyph->uvEnd.x;

                    pCurrentQuad->m_uv[0].y = pCurrentQuad->m_uv[3].y = pGlyph->uv.y;

                    pCurrentQuad->m_uv[1].y = pCurrentQuad->m_uv[2].y = pGlyph->uvEnd.y;

                    nlColour QuadColour = OverrideColour;
                    QuadColour.c[3] = Colour.c[3];
                    pCurrentQuad->SetColour(QuadColour);

                    pCurrentQuad++;
                    HandledChars++;
                }

                int FinalAdvance = (int)pGlyph->Advance;
                if (pGlyph->HasKernPairs && pCurrentChar[1] != 0)
                {
                    unsigned short* pKern = (unsigned short*)pCurrentChar;
                    KernPair kp = { { pKern[0], pKern[1] }, 0 };
                    KernPair* pValidKp = nlBSearch<KernPair, KernPair>(kp, m_pKernTable, m_KernTableSize);
                    if (pValidKp != 0)
                    {
                        FinalAdvance += pValidKp->Kern;
                    }
                }

                CurrentPosition.x += (float)FinalAdvance * m_Metrics.Spacing;
            }

            pCurrentChar++;
            i++;
        }

        if (pOverrideColour != 0 && pOverrideColour->c[3] != 0)
        {
            OverrideColour = *pOverrideColour;
        }
        else
        {
            OverrideColour = Colour;
        }

        unsigned long UsedQuads = (unsigned long)(pCurrentQuad - pQuads);
        if (UsedQuads != 0)
        {
            if (m_bScissorBox)
            {
                glAttachPoly2(View, 0, UsedQuads, pQuads, &Scissor, pMatrix);
            }
            else
            {
                glAttachPoly2(View, 0, UsedQuads, pQuads, pMatrix);
            }
        }

        pCurrentQuad = pQuads;
        CurrentPage++;
    }

    if (m_TextureType == SplitFX && Passes == PASS_TextAndEffect)
    {
        DrawString(View, Text, Position, EffectColour, EffectColour, Length, PASS_Effect, View != 0, pMatrix, 0);
    }

    if (pOverrideColour != 0)
    {
        if (PushColourIndex != 0 && LastPushedColour.c[3] != 0)
        {
            *pOverrideColour = LastPushedColour;
            pOverrideColour->c[3] = 0xFF;
        }
        else
        {
            pOverrideColour->c[3] = 0;
        }
    }
}

void nlFont::SetScissorBox(const ScissorBox& other) const
{
    m_scissorBox = other;
    m_bScissorBox = true;
}

void nlFont::DisableScissorBox() const
{
    m_bScissorBox = false;
}

unsigned long nlFont::GetCharWidth(unsigned short FontChar, unsigned short PrevFontChar) const
{
    return CalculateCharWidth(*this, *this, FontChar, PrevFontChar);
}

unsigned long nlFont::GetStringWidth(const FontCharString& Text, bool SingleLine, unsigned long Width, bool WordWrap) const
{
    if (SingleLine)
    {
        unsigned short PrevChar = 0;
        unsigned long StringWidth = 0;
        const unsigned short* pCurrentChar = Text.m_pString;
        FontStringContext glyphLookup(*this);
        while (*pCurrentChar != 0)
        {
            if (*pCurrentChar == glyphLookup.GetEscapeBegin())
            {
                nlEscapeSequence esc(pCurrentChar);
                pCurrentChar = esc.m_pEnd - 1;
            }
            else
            {
                StringWidth += glyphLookup.GetCharWidth(*pCurrentChar, PrevChar);
                PrevChar = *pCurrentChar;
            }
            pCurrentChar++;
        }
        return StringWidth;
    }

    unsigned long CurrentRowWidth = 0;
    unsigned long MaxWidth = 0;
    const unsigned short* pLastSpace = 0;
    const unsigned short* pLastNonEsc = 0;
    unsigned long WidthAtLastSpace = 0;
    unsigned char FirstChar = 1;
    unsigned char IsNewParagraph = 0;
    const unsigned short* pCurrentChar = Text.m_pString;
    unsigned long CharWidth;
    FontStringContext glyphLookup(*this);

    while (*pCurrentChar != 0)
    {
        if (*pCurrentChar == glyphLookup.GetEscapeBegin())
        {
            nlEscapeSequence esc(pCurrentChar);
            if (esc.GetType() == ESC_NON_BREAKING_SPACE)
            {
                unsigned short prevChar = FirstChar ? 0 : (pLastNonEsc != 0 ? *pLastNonEsc : 0);
                CharWidth = glyphLookup.GetCharWidth(' ', prevChar);
            }
            else if (esc.GetType() == ESC_PARAGRAPH)
            {
                CharWidth = Width + 1;
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
            unsigned short prevChar = FirstChar ? 0 : (pLastNonEsc != 0 ? *pLastNonEsc : 0);
            CharWidth = glyphLookup.GetCharWidth(*pCurrentChar, prevChar);
            pLastNonEsc = pCurrentChar;
            if (*pCurrentChar == ' ')
            {
                pLastSpace = pCurrentChar;
                WidthAtLastSpace = CurrentRowWidth;
            }
        }
        FirstChar = 0;
        if (CurrentRowWidth + CharWidth > Width)
        {
            if (WordWrap && pLastSpace != 0)
            {
                CurrentRowWidth = WidthAtLastSpace;
                pCurrentChar = pLastSpace + 1;
                pLastSpace = 0;
                WidthAtLastSpace = 0;
            }
            if (*pCurrentChar != ' ')
            {
                pCurrentChar--;
            }
            bool useWidth = CurrentRowWidth < Width && CurrentRowWidth > MaxWidth;
            if (useWidth)
            {
                MaxWidth = CurrentRowWidth;
            }
            CurrentRowWidth = 0;
        }
        CurrentRowWidth += CharWidth;
        if (IsNewParagraph)
        {
            bool useWidth = CurrentRowWidth < Width && CurrentRowWidth > MaxWidth;
            if (useWidth)
            {
                MaxWidth = CurrentRowWidth;
            }
            CurrentRowWidth = 0;
            IsNewParagraph = 0;
        }
        pCurrentChar++;
    }
    bool useWidth = CurrentRowWidth < Width && CurrentRowWidth > MaxWidth;
    if (useWidth)
    {
        MaxWidth = CurrentRowWidth;
    }
    return MaxWidth;
}

unsigned long nlFont::GetStringHeight(const FontCharString& Text, unsigned long Width, bool WordWrap) const
{
    unsigned long LineCount = GetStringLineCount(Text, Width, WordWrap);
    return (unsigned long)(m_Metrics.Spacing * (m_Metrics.Height * LineCount));
}

unsigned long nlFont::GetStringLineCount(const FontCharString& Text, unsigned long Width, bool WordWrap) const
{
    unsigned long RowCount = 0;
    unsigned long CurrentRowWidth = 0;
    const unsigned short* pLastSpace = 0;
    const unsigned short* pLastNonEsc = 0;
    unsigned char FirstChar = 1;
    unsigned char IsNewParagraph = 0;
    const unsigned short* pCurrentChar = Text.m_pString;
    unsigned long CharWidth;
    FontStringContext glyphLookup(*this);

    while (*pCurrentChar != 0)
    {
        if (*pCurrentChar == glyphLookup.GetEscapeBegin())
        {
            nlEscapeSequence esc(pCurrentChar);
            if (esc.GetType() == ESC_NON_BREAKING_SPACE)
            {
                unsigned long prevChar = FirstChar ? 0 : (pLastNonEsc != 0 ? (unsigned long)*pLastNonEsc : 0);
                CharWidth = glyphLookup.GetCharWidth(' ', (unsigned short)prevChar);
            }
            else if (esc.GetType() == ESC_PARAGRAPH)
            {
                CharWidth = Width + 1;
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
            CharWidth = glyphLookup.GetCharWidth(*pCurrentChar, (unsigned short)prevChar);
            pLastNonEsc = pCurrentChar;
            if (*pCurrentChar == ' ')
            {
                pLastSpace = pCurrentChar;
            }
        }
        FirstChar = 0;
        if (CurrentRowWidth + CharWidth > Width)
        {
            if (WordWrap && pLastSpace != 0)
            {
                pCurrentChar = pLastSpace + 1;
                pLastSpace = 0;
            }
            RowCount++;
            if (*pCurrentChar != ' ')
            {
                pCurrentChar--;
            }
            CurrentRowWidth = 0;
        }
        CurrentRowWidth += CharWidth;
        if (IsNewParagraph)
        {
            CurrentRowWidth = 0;
            IsNewParagraph = 0;
        }
        pCurrentChar++;
    }
    return RowCount + 1;
}
