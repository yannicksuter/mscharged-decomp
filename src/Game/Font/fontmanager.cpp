#include "NL/nlPrint.h"
#include "Game/Font/fontmanager.h"
#include "NL/nlPrint.h"

class BundleFile;

struct Unidentified_80307564
{
    Unidentified_80307564()
    {
        field_0x0 = true;
        field_0x4 = 0;
        field_0x388 = 0;
        field_0x38C = 0;
        field_0x8[0] = field_0x107[0] = field_0x206[0] = '\0';
        for (int i = 0; i < 16; i++)
        {
            field_0x308[i].field_0x0 = 0;
            field_0x308[i].field_0x4 = -1;
        }
    }

    bool field_0x0;
    BundleFile* field_0x4;
    char field_0x8[255];
    char field_0x107[255];
    char field_0x206[255];
    struct Entry
    {
        void* field_0x0;
        unsigned long field_0x4;
    } field_0x308[16];
    unsigned long field_0x388;
    unsigned long field_0x38C;
};

Unidentified_80307564 lbl_80580748[16];

FontManager::~FontManager()
{
    nlDLListIterator<nlFont*> it = m_fonts.Begin();
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

nlFont* FontManager::GetFontByHashID(unsigned long hashID)
{
    nlDLListIterator<nlFont*> it = m_fonts.Begin();
    DLListEntry<nlFont*>* head = it.m_Head;
    DLListEntry<nlFont*>* entry = it.m_Curr;

    while (entry != 0)
    {
        nlFont* font = entry->entry;
        if (hashID == font->m_Metrics.FontName)
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

    nlPrintf("FontManager: Warning, failed to find font 0x%08x\n", hashID);

    nlDLListIterator<nlFont*> start = m_fonts.Begin();
    if (start.hasNext())
    {
        return *start;
    }
    return 0;
}
