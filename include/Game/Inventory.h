#ifndef GAME_INVENTORY_H
#define GAME_INVENTORY_H

#include "Game/SAnim.h"
#include "NL/nlList.h"
#include "NL/nlString.h"

int nlPrintf(const char*, ...);

template <typename T>
class cInventory
{
public:
    cInventory()
        : m_nItemCount(0)
    {
    }

    void ParseChunks(nlChunk* chunk, nlChunk* end)
    {
        while (chunk != end)
        {
            if (T::IsValidChunkID(chunk->GetID()))
            {
                T* item = T::Initialize(chunk);
                m_lItemList.AddStart(item);
                m_nItemCount++;
            }
            else
            {
                nlPrintf(
                    "Warning: inventory encountered an unknown chunk type\n");
            }
            chunk = chunk->GetNextChunk();
        }
    }

    void AddFile(char* memory, unsigned long length)
    {
        m_lMemList.AddStart(memory);
        ParseChunks((nlChunk*)memory, (nlChunk*)(memory + length));
    }

    nlListIterator<T*> Begin()
    {
        return m_lItemList.Begin();
    }

    T* Find(unsigned int hashID)
    {
        for (nlListIterator<T*> iterator = Begin(); iterator.IsValid();
            iterator.Next())
        {
            if (hashID == iterator.Current()->GetHashID())
            {
                return iterator.Current();
            }
        }
        return 0;
    }

    T* Find(char* name)
    {
        return Find(nlStringHash(name));
    }

    ~cInventory();

    void Clear();

private:
    nlListContainer<T*> m_lItemList;
    nlListContainer<typename T::MemType> m_lMemList;
    int m_nItemCount;
};

template <typename T>
inline cInventory<T>::~cInventory()
{
    Clear();
}

template <typename T>
inline void cInventory<T>::Clear()
{
    for (nlListIterator<T*> iterator = m_lItemList.Begin();
        iterator.IsValid();
        iterator.Next())
    {
        iterator.Current()->Destroy();
    }

    m_lItemList.Clear();

    while (m_lMemList.m_Head != 0)
    {
        typename T::MemType mesh;
        m_lMemList.RemoveStart(&mesh);
        ::operator delete(mesh);
    }

    m_nItemCount = 0;
}

#endif // GAME_INVENTORY_H
