#ifndef NL_BIND_MEMBER_H
#define NL_BIND_MEMBER_H

#include "NL/nlBind.h"
#include "NL/nlFunction.h"

template <typename T, typename R>
inline BindExp1<R, Detail::MemFunImpl<R, R (T::*)()>, T*>
BindMember(T* owner, R (T::*method)())
{
    return Bind<R>(MemFun(method), owner);
}

template <typename T, typename R, typename P1>
inline BindExp2<R, Detail::MemFunImpl<R, R (T::*)(P1)>, T*, Placeholder<0> >
BindMember(T* owner, R (T::*method)(P1))
{
    return Bind<R>(MemFun(method), owner, placeholder0);
}

#endif // NL_BIND_MEMBER_H
