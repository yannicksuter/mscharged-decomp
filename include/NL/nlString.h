#ifndef _NLSTRING_H_
#define _NLSTRING_H_

namespace Detail
{
extern void* gTempStringPool[3];

class TempStringAllocator
{
public:
    template <typename T>
    static T* New(int, const char*)
    {
        return (T*)Alloc();
    }

    template <typename T>
    static void Delete(T* ptr)
    {
        Free(ptr);
    }

    static void* Alloc()
    {
        void* ptr;
        if (gTempStringPool[0] == 0)
        {
            ptr = 0;
        }
        else
        {
            ptr = gTempStringPool[0];
            gTempStringPool[0] = *(void**)ptr;
        }
        return ptr;
    }

    static void Free(void* ptr)
    {
        *(void**)ptr = gTempStringPool[0];
        gTempStringPool[0] = ptr;
    }
};
} // namespace Detail

unsigned long nlStringHash(const char* string);

#endif // _NLSTRING_H_
