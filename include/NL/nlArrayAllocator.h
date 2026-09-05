#ifndef _NLARRAYALLOCATOR_H_
#define _NLARRAYALLOCATOR_H_

#include "NL/nlMemory.h"

template <typename T>
class nlArrayAllocator
{
public:
    nlArrayAllocator() { }

    nlArrayAllocator(T* mem, unsigned long count)
    {
        Init(mem, count);
    }

    void Init(T* mem, unsigned long count)
    {
        m_pFree = mem;
        m_Unknown04 = mem;
        for (unsigned long i = 0; i < count - 1; ++i)
        {
            *(T**)(m_Unknown04 + i) = m_Unknown04 + i + 1;
        }
        *(T**)(m_Unknown04 + count - 1) = 0;
    }

    T* Allocate()
    {
        T* entry = m_pFree;
        if (entry == 0)
            return 0;
        m_pFree = *(T**)entry;
        return entry;
    }

    T* New(const T& data)
    {
        return new (Allocate()) T(data);
    }

    void DeleteEntry(T* entry)
    {
        *(T**)entry = m_pFree;
        m_pFree = entry;
    }

    T* m_pFree;
    T* m_Unknown04;
};

template <typename T, int N>
class nlStaticArrayAllocator : public nlArrayAllocator<T>
{
public:
    nlStaticArrayAllocator()
    {
        this->m_pFree = 0;
        this->Init(reinterpret_cast<T*>(m_Memory), N);
    }

    unsigned char m_Memory[sizeof(T) * N];
};

#endif // _NLARRAYALLOCATOR_H_
