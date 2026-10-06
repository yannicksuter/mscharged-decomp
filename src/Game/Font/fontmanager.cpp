#include "NL/nlDLListContainer.inl"
#include "Game/Font/fontmanager.h"
#include "NL/nlBundleFile.h"
#include "NL/nlFont.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"

template <>
FontManager* nlSingleton<FontManager>::s_pInstance = 0;

// One asynchronous LoadFont request. bComplete is set while the slot is idle
// and again once every texture read issued for the font has finished.
struct FontLoadState
{
    FontLoadState()
    {
        bComplete = true;
        pBundle = 0;
        uNumTextures = 0;
        uNumTexturesLoaded = 0;
        szBundleFilename[0] = szFileName[0] = szFontName[0] = '\0';
        for (int i = 0; i < 16; i++)
        {
            pendingTextures[i].pBuffer = 0;
            pendingTextures[i].uHashID = -1;
        }
    }

    bool bComplete;
    BundleFile* pBundle;
    char szBundleFilename[255];
    char szFileName[255];
    char szFontName[255];
    struct PendingTexture
    {
        void* pBuffer;
        unsigned long uHashID;
    } pendingTextures[16];
    unsigned long uNumTextures;
    unsigned long uNumTexturesLoaded;
};

FontLoadState gFontLoadStates[16];

static inline void LoadFontTexture(FontLoadState* state, unsigned long uFileHashID)
{
    BundleFileDirectoryEntry entry;
    if (state->pBundle->GetFileInfo(uFileHashID, &entry, true))
    {
        void* textureData = nlMalloc(entry.m_length, 0x20, true);
        state->pBundle->ReadFileAsync(uFileHashID, textureData, entry.m_length, FontManager::TextureLoadComplete, (unsigned long)state);
        state->pendingTextures[state->uNumTextures].pBuffer = textureData;
        state->pendingTextures[state->uNumTextures].uHashID = uFileHashID;
        state->uNumTextures++;
    }
}

void FontManager::BundleOpenComplete(void*, unsigned long, unsigned long uParam)
{
    FontLoadState* state = (FontLoadState*)uParam;
    unsigned long uFileHashID = nlStringHash(state->szFileName);
    BundleFileDirectoryEntry entry;

    if (state->pBundle->GetFileInfo(uFileHashID, &entry, true))
    {
        void* fileData = nlMalloc(entry.m_length, 0x20, true);
        state->pBundle->ReadFileAsync(uFileHashID, fileData, entry.m_length, FontDescriptionLoadComplete, (unsigned long)state);
    }
}

void FontManager::FontDescriptionLoadComplete(void* buffer, unsigned long, unsigned long uParam)
{
    FontLoadState* state = (FontLoadState*)uParam;
    char* fontData = (char*)buffer;
    BundleFileDirectoryEntry entry;

    nlFont* pNewFont = new (8, false) nlFont();
    pNewFont->Load(state->szFileName, fontData, nlStringHash(state->szFontName));
    FontManager::Instance()->m_fonts.AddEnd(pNewFont);
    delete[] fontData;

    for (unsigned long i = 0; i < pNewFont->m_PageCount; i++)
    {
        unsigned long uTextureHashID = pNewFont->m_TextureHandles[i];
        state->pBundle->GetFileInfo(uTextureHashID, &entry, true);
        LoadFontTexture(state, uTextureHashID);

        if (pNewFont->m_TextureType == SplitFX)
        {
            uTextureHashID = pNewFont->m_EffectTextureHandles[i];
            state->pBundle->GetFileInfo(uTextureHashID, &entry, true);
            LoadFontTexture(state, uTextureHashID);
        }
    }

    state->uNumTexturesLoaded = 0;
}

void FontManager::TextureLoadComplete(void* buffer, unsigned long uReadSize, unsigned long uParam)
{
    FontLoadState* state = (FontLoadState*)uParam;
    char* textureData = (char*)buffer;

    for (int i = 0; i < 16; i++)
    {
        if (textureData == state->pendingTextures[i].pBuffer)
        {
            glBeginResource(state->pendingTextures[i].uHashID);
            glTextureAdd(state->pendingTextures[i].uHashID, textureData, uReadSize, FontManager::Instance()->m_pResourcePool);
            glEndResource();
            state->uNumTexturesLoaded++;
            break;
        }
    }

    delete[] textureData;

    if (state->uNumTexturesLoaded == state->uNumTextures)
    {
        state->pBundle->Close();
        delete state->pBundle;
        state->bComplete = true;
    }
}

bool FontManager::IsLoadingComplete() const
{
    for (int i = 0; i < 16; i++)
    {
        if (!gFontLoadStates[i].bComplete)
        {
            return false;
        }
    }
    return true;
}

FontManager::FontManager()
    : m_fonts(8)
{
    m_pResourcePool = glGetCurrentResourcePool();
}

FontManager::~FontManager()
{
    nlDLListIterator<nlFont*> it;
    it = m_fonts.Begin();
    DLListEntry<nlFont*>* head = it.m_Head;
    DLListEntry<nlFont*>* current = it.m_Curr;

    while (current != 0)
    {
        delete current->entry;

        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
        }
    }

    m_fonts.Clear();
}

nlFont* FontManager::GetFontByHashID(unsigned long uHashID)
{
    nlDLListIterator<nlFont*> it;
    it = m_fonts.Begin();
    DLListEntry<nlFont*>* head = it.m_Head;
    DLListEntry<nlFont*>* entry = it.m_Curr;

    while (entry != 0)
    {
        nlFont* font = entry->entry;
        if (uHashID == font->m_Metrics.FontName)
        {
            return font;
        }

        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }

    nlPrintf("FontManager: Warning, failed to find font 0x%08x\n", uHashID);

    nlDLListIterator<nlFont*> start;
    start = m_fonts.Begin();
    if (start.hasNext())
    {
        return *start;
    }
    return 0;
}

bool FontManager::LoadFont(const char* szBundleFilename, const char* szFileName, const char* szFileFontName)
{
    FontLoadState* state = 0;
    for (int i = 0; i < 16; i++)
    {
        if (gFontLoadStates[i].bComplete)
        {
            state = &gFontLoadStates[i];
            break;
        }
    }

    state->bComplete = false;
    state->pBundle = new (0x20, true) BundleFile();
    nlStrNCpy(state->szBundleFilename, szBundleFilename, 0xFF);
    nlStrNCpy(state->szFileName, szFileName, 0xFF);
    nlStrNCpy(state->szFontName, szFileFontName, 0xFF);
    nlToLower(state->szFontName);
    state->uNumTextures = 0;
    state->uNumTexturesLoaded = 0;
    state->pBundle->OpenAsync(szBundleFilename, BundleOpenComplete, (unsigned long)state, false);
    return true;
}

void FontManager::SetResourcePool(GLResourcePool* pResourcePool)
{
    m_pResourcePool = pResourcePool;
}
