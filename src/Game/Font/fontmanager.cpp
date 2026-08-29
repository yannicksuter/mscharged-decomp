#include "Game/Font/fontmanager.h"

extern void nlPrintf(const char*, ...);

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
