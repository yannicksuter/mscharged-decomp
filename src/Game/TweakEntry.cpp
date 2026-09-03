#include "Game/TweakRegistry.h"

#include "NL/nlMemory.h"
#include "NL/nlString.h"

nlSlotPoolFixed<0x2C> lbl_8057C734(0x20);

TweakEntry_8052BF00::TweakEntry_8052BF00()
{
    m_ChildHead = 0;
    m_ChildTail = 0;
    m_Unk28 = false;
    m_Unk29 = false;
}

TweakEntry_8052BF00::~TweakEntry_8052BF00()
{
    fn_802C5B84(this);
}

void fn_802C54CC(TweakEntry_8052BF00* entry, TweakNode_8052BEB0* child)
{
    TweakNode_8052BEB0* current = entry->m_ChildHead;
    TweakNode_8052BEB0* next = current->m_Next;
    bool found = false;

    while (!found)
    {
        if (next == 0)
        {
            found = true;
        }
        else if (current == fn_802C0E3C())
        {
            found = false;
        }
        else if (current == fn_802C0E44())
        {
            if (nlStrICmp(fn_802C3FDC(child), fn_802C3FDC(next)) < 0)
            {
                found = true;
            }
        }
        else if (nlStrICmp(fn_802C3FDC(child), fn_802C3FDC(current)) >= 0
            && nlStrICmp(fn_802C3FDC(child), fn_802C3FDC(next)) < 0)
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

void fn_802C56E8(TweakEntry_8052BF00* entry, TweakNode_8052BEB0* child)
{
    if (entry->m_ChildHead == 0)
    {
        entry->m_ChildHead = child;
        entry->m_ChildTail = child;
    }
    else if (entry == fn_802C0E30() || entry->m_Unk29)
    {
        fn_802C54CC(entry, child);
    }
    else
    {
        entry->m_ChildTail->m_Next = child;
        entry->m_ChildTail = child;
    }

    child->m_Parent = entry;
    fn_802C47E4(child);
    child->m_Unk10 = entry->m_Unk10 + 1;
}

void fn_802C5780(TweakEntry_8052BF00* entry, TweakValueBase_8052BF70* value)
{
    TweakNode_8052BEB0* child
        = new (lbl_8057C6E4.Allocate()) TweakNode_8052BEB0;
    child->m_Value = value;
    fn_802C47E4(child);
    fn_802C56E8(entry, child);
}

TweakNode_8052BEB0* fn_802C5884(TweakEntry_8052BF00* entry, const char* name)
{
    for (TweakNode_8052BEB0* child = entry->m_ChildHead; child != 0;
        child = child->m_Next)
    {
        if (nlStrNICmp(name, fn_802C3FDC(child), 0x40) == 0)
        {
            return child;
        }
    }
    return 0;
}

void fn_802C595C(TweakEntry_8052BF00* entry, TweakValueBase_8052BF70* value)
{
    if (entry->UnidentifiedVirtual0C())
    {
        TweakEntry_8052BF00* folder = entry->UnidentifiedVirtual18();
        for (TweakNode_8052BEB0* child = folder->m_ChildHead; child != 0;)
        {
            TweakNode_8052BEB0* next = child->m_Next;
            if (child->m_Value == value)
            {
                delete child;
            }
            else if (child->UnidentifiedVirtual0C())
            {
                fn_802C595C(child->UnidentifiedVirtual18(), value);
            }
            child = next;
        }
    }
}

void fn_802C5B84(TweakEntry_8052BF00* entry)
{
    for (TweakNode_8052BEB0* child = entry->m_ChildHead; child != 0;)
    {
        TweakNode_8052BEB0* next = child->m_Next;
        if (child->UnidentifiedVirtual0C())
        {
            fn_802C5B84(child->UnidentifiedVirtual18());
        }
        delete child;
        child = next;
    }
    entry->m_ChildHead = 0;
}

void fn_802C5D74(TweakEntry_8052BF00* entry)
{
    for (TweakNode_8052BEB0* child = entry->m_ChildHead; child != 0;)
    {
        TweakNode_8052BEB0* next = child->m_Next;
        if (child->m_State == 2)
        {
            delete child;
        }
        else if (child->UnidentifiedVirtual0C())
        {
            fn_802C5D74(child->UnidentifiedVirtual18());
        }
        child = next;
    }
}
