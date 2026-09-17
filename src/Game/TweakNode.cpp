#include "Game/TweakRegistry.h"

#include "NL/nlMemory.h"
#include "NL/nlSmallBlockAllocator.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

nlSlotPoolFixed<0x20> gTweakNodePool(0x20);

inline const char* GetTweakNodeName(TweakNode* node)
{
    return node->m_Value != 0 ? node->m_Value->mName : "ROOT";
}

TweakNode::TweakNode()
{
    m_Next = 0;
    m_Parent = 0;
    m_Depth = 0;
    m_Unk1C = 0;
    if (IsTweakRegistryInitialized() != 0)
    {
        if (gTweakStatePushed)
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

TweakNode::~TweakNode()
{
    if (this != GetTweakRoot())
    {
        TweakEntry* parent = m_Parent;
        TweakNodeListRemove(&parent->m_ChildHead, this, &parent->m_ChildTail);
        if (m_Unk1C == 0 && m_Value->mCreatedAfterRegistryInit && (m_State == 2 || (m_State == 1 && gDeletePersistentTweakValues)))
        {
            char buffer[0x100];
            nlStrNCpy(buffer, "", sizeof(buffer));
            TweakEntry* path = m_Parent;
            if (path != 0)
            {
                nlStrNCpy(buffer, "", sizeof(buffer));
                TweakEntry* grandparent = path->m_Parent;
                if (grandparent != 0)
                {
                    nlStrNCpy(buffer, "", sizeof(buffer));
                    GetTweakNodePath(grandparent, buffer, sizeof(buffer));
                    nlStrNCat(buffer, buffer, "/", sizeof(buffer));
                    nlStrNCat(buffer, buffer, GetTweakNodeName(path), sizeof(buffer));
                }
                nlStrNCat(buffer, buffer, "/", sizeof(buffer));
                nlStrNCat(buffer, buffer, GetTweakNodeName(this), sizeof(buffer));
            }
            delete m_Value;
        }
    }
}

TweakEntry* FindOrCreateTweakChildEntry(TweakEntry* entry, const char* name, int noCreate)
{
    if (entry->UnidentifiedVirtual0C() == 0)
    {
        return 0;
    }
    TweakEntry* folder = entry->UnidentifiedVirtual18();
    for (TweakNode* child = folder->m_ChildHead; child != 0; child = child->m_Next)
    {
        if (child->UnidentifiedVirtual0C() != 0)
        {
            if (nlStrICmp(name, GetTweakNodeName(child)) == 0)
            {
                return child->UnidentifiedVirtual18();
            }
        }
    }
    if (noCreate == 0)
    {
        TweakValueName* value = new TweakValueName(InternTweakString(name, kTweakStringFolder));
        return CreateTweakEntry(value, entry->UnidentifiedVirtual18());
    }
    return 0;
}

TweakNode* FindTweakNode(TweakNode* entry, const char* path)
{
    char buffer[0x100];
    nlStrNCpy(buffer, "", sizeof(buffer));
    TweakEntry* parent = entry->m_Parent;
    if (parent != 0)
    {
        nlStrNCpy(buffer, "", sizeof(buffer));
        TweakEntry* grandparent = parent->m_Parent;
        if (grandparent != 0)
        {
            nlStrNCpy(buffer, "", sizeof(buffer));
            GetTweakNodePath(grandparent, buffer, sizeof(buffer));
            nlStrNCat(buffer, buffer, "/", sizeof(buffer));
            nlStrNCat(buffer, buffer, GetTweakNodeName(parent), sizeof(buffer));
        }
        nlStrNCat(buffer, buffer, "/", sizeof(buffer));
        nlStrNCat(buffer, buffer, GetTweakNodeName(entry), sizeof(buffer));
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
    if (nlStrICmp(path, full) == 0)
    {
        return entry;
    }
    if (entry->UnidentifiedVirtual0C() != 0)
    {
        unsigned long length = nlStrLen(full);
        if (length == 0 || nlStrNICmp(search, full, length) == 0)
        {
            TweakEntry* folder = entry->UnidentifiedVirtual18();
            for (TweakNode* child = folder->m_ChildHead; child != 0;
                child = child->m_Next)
            {
                TweakNode* found = FindTweakNode(child, search);
                if (found != 0)
                {
                    return found;
                }
            }
        }
    }
    return 0;
}

TweakEntry* FindOrCreateTweakPath(TweakEntry* entry, const char* path, int noCreate)
{
    char* copy;
    TweakEntry* result = 0;
    const char* start = path;
    if (*path == '/')
    {
        start = path + 1;
    }
    unsigned long length = nlStrLen(path) + 1;
    copy = (char*)nlMalloc(length, 8, false);
    nlStrNCpy(copy, start, length);
    if (copy[nlStrLen(copy) - 1] == '/')
    {
        copy[nlStrLen(copy) - 1] = '\0';
    }

    char* rest = copy;
    int split = 0;
    while (split == 0 && *rest != '\0')
    {
        char c = *rest;
        if (c == '/')
        {
            split = 1;
            *rest = '\0';
        }
        rest++;
    }

    if (split == 0)
    {
        result = FindOrCreateTweakChildEntry(entry, copy, noCreate);
    }
    else
    {
        TweakEntry* child = FindOrCreateTweakChildEntry(entry, copy, noCreate);
        if (child != 0)
        {
            result = FindOrCreateTweakPath(child, rest, noCreate);
        }
    }
    delete copy;
    return result;
}

void GetTweakNodePath(TweakNode* node, char* buffer, unsigned long size)
{
    TweakEntry* parent = node->m_Parent;
    if (parent != 0)
    {
        nlStrNCpy(buffer, "", size);
        TweakEntry* grandparent = parent->m_Parent;
        if (grandparent != 0)
        {
            nlStrNCpy(buffer, "", size);
            GetTweakNodePath(grandparent, buffer, size);
            nlStrNCat(buffer, buffer, "/", size);
            nlStrNCat(buffer, buffer, GetTweakNodeName(parent), size);
        }
        nlStrNCat(buffer, buffer, "/", size);
        nlStrNCat(buffer, buffer, GetTweakNodeName(node), size);
    }
}

void UpdateTweakNodePathHash(TweakNode* node)
{
    if (node->m_Parent != GetTweakPriorityNode() && node->m_Parent != 0)
    {
        char buffer[0x200];
        nlStrNCpy(buffer, "", sizeof(buffer));
        TweakEntry* parent = node->m_Parent;
        if (parent != 0)
        {
            nlStrNCpy(buffer, "", sizeof(buffer));
            TweakEntry* grandparent = parent->m_Parent;
            if (grandparent != 0)
            {
                nlStrNCpy(buffer, "", sizeof(buffer));
                GetTweakNodePath(grandparent, buffer, sizeof(buffer));
                nlStrNCat(buffer, buffer, "/", sizeof(buffer));
                nlStrNCat(buffer, buffer, GetTweakNodeName(parent), sizeof(buffer));
            }
            nlStrNCat(buffer, buffer, "/", sizeof(buffer));
            nlStrNCat(buffer, buffer, GetTweakNodeName(node), sizeof(buffer));
        }
        node->m_PathHash = nlStringLowerHash(buffer);
    }
}
