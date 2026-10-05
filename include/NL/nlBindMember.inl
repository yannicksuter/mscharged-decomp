#ifndef NL_BIND_MEMBER_INL
#define NL_BIND_MEMBER_INL

#include "NL/nlBindMember.h"

template <typename T, typename Method>
inline typename BindMemberTraits<T, Method>::Result BindMember(T* owner, Method method)
{
    typedef BindMemberTraits<T, Method> Traits;
    return BindMemberExp<typename Traits::ReturnType>(MemFun(method), owner, Traits::Arg());
}

#endif // NL_BIND_MEMBER_INL
