#include "NL/nlFont.h"
#include "NL/nlList.h"
#include "NL/nlPrint.h"

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

static inline nlFont::GlyphInfo* AddExtendedGlyph(nlFont* self, nlListSlotPoolHigh<nlFont::GlyphInfo>& GlyphList)
{
    ListEntry<nlFont::GlyphInfo>* pEntry = GlyphList.Allocate();
    nlListAddStart<ListEntry<nlFont::GlyphInfo> >(&GlyphList.m_Head, pEntry, &GlyphList.m_Tail);
    self->m_ExtendedGlyphCount++;
    return &pEntry->entry;
}

static inline void ParseKernPairs(nlFont* self, nlFont::GlyphInfo* pInfo, char* pToken, unsigned short Base, nlListSlotPoolHigh<nlFont::KernPair>& KernList)
{
    pInfo->HasKernPairs = 1;
    nlFont::KernPair kp;
    char* pKernToken = nlStrChr(pToken, ' ');
    ListEntry<nlFont::KernPair>** pKernHead;
    ListEntry<nlFont::KernPair>** pKernTail;
    pKernTail = &KernList.m_Tail;
    pKernHead = &KernList.m_Head;
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
        ListEntry<nlFont::KernPair>* pEntry = KernList.Allocate(kp);
        nlListAddStart<ListEntry<nlFont::KernPair> >(pKernHead, pEntry, pKernTail);
        self->m_KernTableSize++;
        pKernToken = nlStrChr(pKernToken, ' ') + 1;
    }
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
    nlFont::KernPair* pCurKP;
    nlFont::KernPair* pKP;

    float fVar_f24 = 0.0f;

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
            fVar_f24 = atof(pToken);
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

            bool usePackedGlyphs = 1.2f - fVar_f24 > 0.0001f;
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

                pInfo->Unidentified_13 = RenderAscent;
                pInfo->Unidentified_12 = RenderHeight;
            }
            else
            {
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pInfo->Unidentified_12 = (unsigned char)atoi(pCurrentLine);
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pInfo->Unidentified_13 = (unsigned char)atoi(pCurrentLine);
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                CurrentTexelX = atoi(pCurrentLine);
                pCurrentLine = nlStrChr(pCurrentLine, ' ') + 1;
                CurrentTexelY = atoi(pCurrentLine);
            }

            nlVec2Set(pInfo->uv, (float)CurrentTexelX * m_InvTexSize, (float)CurrentTexelY * m_InvTexSize);
            nlVec2Set(pInfo->Unidentified_08, pInfo->uv.x + (pInfo->RenderWidth - 1) * m_InvTexSize, pInfo->uv.y + (pInfo->Unidentified_12 - 1) * m_InvTexSize);
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
            pCurKP = pKP;
            pKP++;
            ListEntry<nlFont::KernPair>* pEntry = nlListRemoveStart<ListEntry<nlFont::KernPair> >(&KernList.m_Head, &KernList.m_Tail);
            if (pCurKP != NULL)
            {
                pCurKP->s.A = pEntry->entry.s.A;
                pCurKP->s.B = pEntry->entry.s.B;
                pCurKP->Kern = pEntry->entry.Kern;
            }

            KernList.m_Allocator.DeleteEntry(pEntry);
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

    char sHashFontName[265] = { 0 };
    unsigned long page;
    for (page = 0; page < m_PageCount; page++)
    {
        nlStrNCpy(sHashFontName, szFontName, 0x109);
        nlSNPrintf(sHashFontName, 0x109, "%s_%d", sHashFontName, page + 1);
        m_TextureHandles[page] = nlStringHash(sHashFontName);

        if (m_TextureType == SplitFX)
        {
            nlStrNCat(sHashFontName, sHashFontName, "e", 0x109);
            m_EffectTextureHandles[page] = nlStringHash(sHashFontName);
        }
    }

    return 1;
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
    const GlyphInfo* pGlyph;
    unsigned short c = FontChar;

    if (c > 0x7F)
    {
        pGlyph = &m_pExtendedGlyphs[c - 0x80];
    }
    else
    {
        pGlyph = &m_GlyphLookup[c - 0x20];
    }

    bool valid = -1.0f != pGlyph->uv.x;
    pGlyph = valid ? pGlyph : &m_GlyphLookup['?' - 0x20];

    signed char offset = pGlyph->Offset;
    unsigned short prevChar = PrevFontChar;
    unsigned char advance = pGlyph->Advance;
    unsigned long ret = advance + offset;

    if (prevChar != 0)
    {
        const GlyphInfo* pPrevGlyph;
        if (prevChar > 0x7F)
        {
            pPrevGlyph = &m_pExtendedGlyphs[prevChar - 0x80];
        }
        else
        {
            pPrevGlyph = &m_GlyphLookup[prevChar - 0x20];
        }

        bool valid = -1.0f != pPrevGlyph->uv.x;
        pPrevGlyph = valid ? pPrevGlyph : &m_GlyphLookup['?' - 0x20];

        if (pPrevGlyph->HasKernPairs)
        {
            KernPair kp = { { PrevFontChar, FontChar }, 0 };
            KernPair* pFoundKP = nlBSearch<KernPair, KernPair>(kp, m_pKernTable, m_KernTableSize);
            if (pFoundKP != 0)
            {
                ret += pFoundKP->Kern;
            }
        }
    }

    return (unsigned long)(ret * m_Metrics.Spacing);
}
