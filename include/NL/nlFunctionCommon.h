#ifndef NL_FUNCTION_COMMON_H
#define NL_FUNCTION_COMMON_H

#include "NL/nlMemory.h"

enum FunctionTag
{
    FUNCTION_EMPTY = 0,
    FUNCTION_FREE = 1,
    FUNCTION_FUNCTOR = 2,
};

template <typename T>
struct IsVoid
{
    enum
    {
        value = false,
    };
};

template <>
struct IsVoid<void>
{
    enum
    {
        value = true,
    };
};

template <bool Value>
struct BoolToType
{
};

namespace Detail
{
template <typename R, typename MemPtr>
struct MemFunImpl
{
private:
    MemPtr mMemFun;

public:
    MemFunImpl(MemPtr function)
        : mMemFun(function)
    {
    }

    template <typename T>
    R operator()(T* object) const
    {
        return (object->*mMemFun)();
    }

    template <typename T, typename P1>
    R operator()(T* object, P1& p1) const
    {
        return (object->*mMemFun)(p1);
    }

    template <typename T, typename P1, typename P2>
    R operator()(T* object, P1& p1, P2& p2) const
    {
        return (object->*mMemFun)(p1, p2);
    }

    template <typename T, typename P1, typename P2, typename P3>
    R operator()(T* object, P1& p1, P2& p2, P3& p3) const
    {
        return (object->*mMemFun)(p1, p2, p3);
    }
};
} // namespace Detail

template <typename T, typename R>
inline Detail::MemFunImpl<R, R (T::*)()> MemFun(R (T::*function)())
{
    return Detail::MemFunImpl<R, R (T::*)()>(function);
}

template <typename T, typename R, typename P1>
inline Detail::MemFunImpl<R, R (T::*)(P1)> MemFun(R (T::*function)(P1))
{
    return Detail::MemFunImpl<R, R (T::*)(P1)>(function);
}

template <typename T, typename R, typename P1, typename P2>
inline Detail::MemFunImpl<R, R (T::*)(P1, P2)> MemFun(R (T::*function)(P1, P2))
{
    return Detail::MemFunImpl<R, R (T::*)(P1, P2)>(function);
}

template <typename T, typename R, typename P1, typename P2, typename P3>
Detail::MemFunImpl<R, R (T::*)(P1, P2, P3)> MemFun(R (T::*function)(P1, P2, P3))
{
    return Detail::MemFunImpl<R, R (T::*)(P1, P2, P3)>(function);
}

template <typename Signature>
class Function;

typedef void FnVoidVoid();

#endif // NL_FUNCTION_COMMON_H
