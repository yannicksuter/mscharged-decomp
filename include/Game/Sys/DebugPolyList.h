#ifndef GAME_SYS_DEBUG_POLY_LIST_H
#define GAME_SYS_DEBUG_POLY_LIST_H

#include "NL/gl/glDraw2.h"
#include "NL/nlDLListContainer.h"

extern SlotPool<glPoly2> g_DebugPolySlotPool;

struct DebugPolyList
{
    DebugPolyList()
        : mCurrent(0)
    {
    }

    ~DebugPolyList()
    {
        glPoly2* poly = 0;
        while (mPolys.m_Head != 0)
        {
            DLListEntry<glPoly2*>* entry = mPolys.m_Head;
            nlDLRingRemove(&mPolys.m_Head, entry);
            mPolys.Deallocate(entry, &poly);
            g_DebugPolySlotPool.DeleteEntry(poly);
        }
        mPolys.m_Allocator.FreeBlocks();
        g_DebugPolySlotPool.FreeBlocks();
    }

    glPoly2* mCurrent;
    nlDLListSlotPool<glPoly2*> mPolys;
};

#endif // GAME_SYS_DEBUG_POLY_LIST_H
