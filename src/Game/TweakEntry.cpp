#include "Game/TweakRegistry.h"

#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

nlSlotPoolFixed<0x2C> gTweakEntryPool(0x20);

TweakEntry::TweakEntry()
{
    m_ChildHead = 0;
    m_ChildTail = 0;
    m_Unk28 = false;
    m_SortChildren = false;
}

TweakEntry::~TweakEntry()
{
    ClearTweakChildren(this);
}

TweakEntry* TweakEntry::UnidentifiedVirtual18()
{
    return this;
}

void InsertTweakChildSorted(TweakEntry* entry, TweakNode* child)
{
    TweakNode* current = entry->m_ChildHead;
    TweakNode* next = current->m_Next;
    bool found = false;

    while (!found)
    {
        if (next == 0)
        {
            found = true;
        }
        else if (current == GetTweakPriorityNode())
        {
            found = false;
        }
        else if (current == GetUserTweakEntry())
        {
            if (nlStrICmp(GetTweakNodeName(child), GetTweakNodeName(next)) < 0)
            {
                found = true;
            }
        }
        else if (nlStrICmp(GetTweakNodeName(child), GetTweakNodeName(current)) >= 0
            && nlStrICmp(GetTweakNodeName(child), GetTweakNodeName(next)) < 0)
        {
            found = true;
        }

        if (!found)
        {
            current = current->m_Next;
            next = current->m_Next;
        }
    }

    if (current == entry->m_ChildTail)
    {
        entry->m_ChildTail = child;
    }
    current->m_Next = child;
    child->m_Next = next;
}

void AddTweakChild(TweakEntry* entry, TweakNode* child)
{
    if (entry->m_ChildHead == 0)
    {
        entry->m_ChildHead = child;
        entry->m_ChildTail = child;
    }
    else if (entry == GetTweakRoot() || entry->m_SortChildren)
    {
        InsertTweakChildSorted(entry, child);
    }
    else
    {
        entry->m_ChildTail->m_Next = child;
        entry->m_ChildTail = child;
    }

    child->m_Parent = entry;
    UpdateTweakNodePathHash(child);
    child->m_Depth = entry->m_Depth + 1;
}

void AddTweakValue(TweakEntry* entry, TweakValueBase* value)
{
    TweakNode* child
        = new (gTweakNodePool.Allocate()) TweakNode;
    child->m_Value = value;
    UpdateTweakNodePathHash(child);
    AddTweakChild(entry, child);
}

TweakNode* FindTweakChild(TweakEntry* entry, const char* name)
{
    for (TweakNode* child = entry->m_ChildHead; child != 0;
        child = child->m_Next)
    {
        if (nlStrNICmp(name, GetTweakNodeName(child), 0x40) == 0)
        {
            return child;
        }
    }
    return 0;
}

void RemoveTweakValue(TweakEntry* entry, TweakValueBase* value)
{
    if (entry->UnidentifiedVirtual0C())
    {
        TweakEntry* folder = entry->UnidentifiedVirtual18();
        for (TweakNode* child = folder->m_ChildHead; child != 0;)
        {
            TweakNode* next = child->m_Next;
            if (value == child->m_Value)
            {
                delete child;
            }
            else if (child->UnidentifiedVirtual0C())
            {
                RemoveTweakValue(child->UnidentifiedVirtual18(), value);
            }
            child = next;
        }
    }
}

void ClearTweakChildren(TweakEntry* entry)
{
    for (TweakNode* child = entry->m_ChildHead; child != 0;)
    {
        TweakNode* next = child->m_Next;
        if (child->UnidentifiedVirtual0C())
        {
            ClearTweakChildren(child->UnidentifiedVirtual18());
        }
        delete child;
        child = next;
    }
    entry->m_ChildHead = 0;
}

void RemoveDynamicTweakChildren(TweakEntry* entry)
{
    TweakNode* next;
    for (TweakNode* child = entry->m_ChildHead; child != 0;)
    {
        next = child->m_Next;
        if (child->m_State == 2)
        {
            delete child;
        }
        else if (child->UnidentifiedVirtual0C())
        {
            RemoveDynamicTweakChildren(child->UnidentifiedVirtual18());
        }
        child = next;
    }
}

int TweakEntry::UnidentifiedVirtual0C()
{
    return 1;
}
