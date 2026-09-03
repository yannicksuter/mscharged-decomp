#include "Game/TweakRegistry.h"

#include "NL/nlMemory.h"
#include "NL/nlSmallBlockAllocator.h"
#include "NL/nlString.h"

extern "C"
{
    extern u8 lbl_806E1E41;
}

nlSlotPoolFixed<0x20> lbl_8057C6E4(0x20);

inline const char* fn_802C3FDC(TweakNode_8052BEB0* node)
{
    return node->m_Value != 0 ? node->m_Value->mName : "ROOT";
}

TweakNode_8052BEB0::TweakNode_8052BEB0()
{
    m_Next = 0;
    m_Parent = 0;
    m_Unk10 = 0;
    m_Unk1C = 0;
    if (fn_802C0F04() != 0)
    {
        if (lbl_806E1E42)
        {
            m_State = 2;
        }
        else
        {
            m_State = 1;
        }
    }
    else
    {
        m_State = 0;
    }
}

TweakNode_8052BEB0::~TweakNode_8052BEB0()
{
    if (this != fn_802C0E30())
    {
        TweakEntry_8052BF00* parent = m_Parent;
        TweakNodeListRemove(&parent->m_ChildHead, this, &parent->m_ChildTail);
        if (m_Unk1C == 0 && m_Value->mUnidentified008 && (m_State == 2 || (m_State == 1 && lbl_806E1E41)))
        {
            char buffer[0x100];
            nlStrNCpy(buffer, "", sizeof(buffer));
            TweakEntry_8052BF00* path = m_Parent;
            if (path != 0)
            {
                nlStrNCpy(buffer, "", sizeof(buffer));
                TweakEntry_8052BF00* grandparent = path->m_Parent;
                if (grandparent != 0)
                {
                    nlStrNCpy(buffer, "", sizeof(buffer));
                    fn_802C46C0(grandparent, buffer, sizeof(buffer));
                    nlStrNCat(buffer, buffer, "/", sizeof(buffer));
                    nlStrNCat(buffer, buffer, fn_802C3FDC(path), sizeof(buffer));
                }
                nlStrNCat(buffer, buffer, "/", sizeof(buffer));
                nlStrNCat(buffer, buffer, fn_802C3FDC(this), sizeof(buffer));
            }
            delete m_Value;
        }
    }
}

void TweakNode_8052BEB0::operator delete(void* ptr)
{
    lbl_8057C6E4.Free(ptr);
}

TweakEntry_8052BF00* fn_802C3FF8(TweakEntry_8052BF00* entry, const char* name, int noCreate)
{
    if (entry->UnidentifiedVirtual0C() == 0)
    {
        return 0;
    }
    TweakEntry_8052BF00* folder = entry->UnidentifiedVirtual18();
    for (TweakNode_8052BEB0* child = folder->m_ChildHead; child != 0; child = child->m_Next)
    {
        if (child->UnidentifiedVirtual0C() != 0)
        {
            if (nlStrICmp(name, fn_802C3FDC(child)) == 0)
            {
                return child->UnidentifiedVirtual18();
            }
        }
    }
    if (noCreate == 0)
    {
        TweakValueName_8052BE78* value = new TweakValueName_8052BE78(fn_802C1EBC(name, kTweakStringFolder));
        return fn_802C0E4C(value, entry->UnidentifiedVirtual18());
    }
    return 0;
}

TweakEntry_8052BF00* fn_802C41B4(TweakEntry_8052BF00* entry, const char* path)
{
    char buffer[0x100];
    nlStrNCpy(buffer, "", sizeof(buffer));
    TweakEntry_8052BF00* parent = entry->m_Parent;
    if (parent != 0)
    {
        nlStrNCpy(buffer, "", sizeof(buffer));
        TweakEntry_8052BF00* grandparent = parent->m_Parent;
        if (grandparent != 0)
        {
            nlStrNCpy(buffer, "", sizeof(buffer));
            fn_802C46C0(grandparent, buffer, sizeof(buffer));
            nlStrNCat(buffer, buffer, "/", sizeof(buffer));
            nlStrNCat(buffer, buffer, fn_802C3FDC(parent), sizeof(buffer));
        }
        nlStrNCat(buffer, buffer, "/", sizeof(buffer));
        nlStrNCat(buffer, buffer, fn_802C3FDC(entry), sizeof(buffer));
    }

    const char* full = buffer;
    if (buffer[0] == '/')
    {
        full++;
    }
    const char* search = path;
    if (*path == '/')
    {
        search = path + 1;
    }
    if (nlStrICmp(search, full) == 0)
    {
        return entry;
    }
    if (entry->UnidentifiedVirtual0C() != 0)
    {
        unsigned long length = nlStrLen(full);
        if (length == 0 || nlStrNCmp(search, full, length) == 0)
        {
            TweakEntry_8052BF00* folder = entry->UnidentifiedVirtual18();
            for (TweakNode_8052BEB0* child = folder->m_ChildHead; child != 0;
                child = child->m_Next)
            {
                TweakEntry_8052BF00* found = fn_802C41B4((TweakEntry_8052BF00*)child, search);
                if (found != 0)
                {
                    return found;
                }
            }
        }
    }
    return 0;
}

TweakEntry_8052BF00* fn_802C4504(TweakEntry_8052BF00* entry, const char* path, int noCreate)
{
    TweakEntry_8052BF00* result = 0;
    const char* start = path;
    if (*path == '/')
    {
        start = path + 1;
    }
    unsigned long length = nlStrLen(path) + 1;
    char* copy = (char*)nlMalloc(length, 8, false);
    nlStrNCpy(copy, start, length);
    if (copy[nlStrLen(copy) - 1] == '/')
    {
        copy[nlStrLen(copy) - 1] = '\0';
    }

    char* rest = copy;
    int split = 0;
    while (split == 0)
    {
        char c = *rest;
        if (c == '\0')
        {
            break;
        }
        if (c == '/')
        {
            split = 1;
            *rest = '\0';
        }
        rest++;
    }

    if (split == 0)
    {
        result = fn_802C3FF8(entry, copy, noCreate);
    }
    else
    {
        TweakEntry_8052BF00* child = fn_802C3FF8(entry, copy, noCreate);
        if (child != 0)
        {
            result = fn_802C4504(child, rest, noCreate);
        }
    }
    delete copy;
    return result;
}

void fn_802C46C0(TweakNode_8052BEB0* node, char* buffer, unsigned long size)
{
    TweakEntry_8052BF00* parent = node->m_Parent;
    if (parent != 0)
    {
        nlStrNCpy(buffer, "", size);
        TweakEntry_8052BF00* grandparent = parent->m_Parent;
        if (grandparent != 0)
        {
            nlStrNCpy(buffer, "", size);
            fn_802C46C0(grandparent, buffer, size);
            nlStrNCat(buffer, buffer, "/", size);
            nlStrNCat(buffer, buffer, fn_802C3FDC(parent), size);
        }
        nlStrNCat(buffer, buffer, "/", size);
        nlStrNCat(buffer, buffer, fn_802C3FDC(node), size);
    }
}

void fn_802C47E4(TweakNode_8052BEB0* node)
{
    if (node->m_Parent != fn_802C0E3C() && node->m_Parent != 0)
    {
        char buffer[0x200];
        nlStrNCpy(buffer, "", sizeof(buffer));
        TweakEntry_8052BF00* parent = node->m_Parent;
        if (parent != 0)
        {
            nlStrNCpy(buffer, "", sizeof(buffer));
            TweakEntry_8052BF00* grandparent = parent->m_Parent;
            if (grandparent != 0)
            {
                nlStrNCpy(buffer, "", sizeof(buffer));
                fn_802C46C0(grandparent, buffer, sizeof(buffer));
                nlStrNCat(buffer, buffer, "/", sizeof(buffer));
                nlStrNCat(buffer, buffer, fn_802C3FDC(parent), sizeof(buffer));
            }
            nlStrNCat(buffer, buffer, "/", sizeof(buffer));
            nlStrNCat(buffer, buffer, fn_802C3FDC(node), sizeof(buffer));
        }
        node->m_PathHash = nlStringLowerHash(buffer);
    }
}
