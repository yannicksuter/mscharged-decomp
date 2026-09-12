#ifndef UNCLASSIFIED_UNIDENTIFIEDDEBUGPOLYSTORAGE_802BB300_H
#define UNCLASSIFIED_UNIDENTIFIEDDEBUGPOLYSTORAGE_802BB300_H

#include "NL/gl/glDraw2.h"
#include "NL/nlDLListContainer.h"

extern SlotPool<glPoly2> lbl_8057C1E8;

struct UnidentifiedDebugPolyStorage_802BB300
{
    UnidentifiedDebugPolyStorage_802BB300()
        : mCurrent(0)
    {
    }

    ~UnidentifiedDebugPolyStorage_802BB300()
    {
        glPoly2* poly = 0;
        while (mPolys.m_Head != 0)
        {
            DLListEntry<glPoly2*>* entry = mPolys.m_Head;
            nlDLRingRemove(&mPolys.m_Head, entry);
            mPolys.Deallocate(entry, &poly);
            lbl_8057C1E8.DeleteEntry(poly);
        }
        mPolys.m_Allocator.FreeBlocks();
        lbl_8057C1E8.FreeBlocks();
    }

    glPoly2* mCurrent;
    nlDLListSlotPool<glPoly2*> mPolys;
};

#endif // UNCLASSIFIED_UNIDENTIFIEDDEBUGPOLYSTORAGE_802BB300_H
