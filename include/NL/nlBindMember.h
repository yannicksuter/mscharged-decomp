#ifndef NL_BIND_MEMBER_H
#define NL_BIND_MEMBER_H

#include "NL/nlBind.h"
#include "NL/nlFunction.h"

// Stands in for the missing call argument of a nullary member function.
struct BindMemberNoArg
{
};

// Never defined: it is only bound by reference and the reference vanishes
// once BindMember is inlined. A temporary instead would take stack space.
extern const BindMemberNoArg bindMemberNoArg;

template <typename R, typename F, typename A>
inline BindExp1<R, F, A> BindMemberExp(F fn, const A& owner, const BindMemberNoArg&)
{
    return BindExp1<R, F, A>(fn, owner);
}

template <typename R, typename F, typename A>
inline BindExp2<R, F, A, Placeholder<0> > BindMemberExp(F fn, const A& owner, const Placeholder<0>& p0)
{
    return BindExp2<R, F, A, Placeholder<0> >(fn, owner, p0);
}

// BindMember(owner, &Class::Method) binds a member function to its owner.
// The traits give the result type and the trailing Bind argument for the
// method's arity; the single BindMember template is defined in
// NL/nlBindMember.inl.
template <typename T, typename Method>
struct BindMemberTraits;

template <typename T, typename R>
struct BindMemberTraits<T, R (T::*)()>
{
    typedef R ReturnType;
    typedef BindExp1<R, Detail::MemFunImpl<R, R (T::*)()>, T*> Result;

    static const BindMemberNoArg& Arg()
    {
        return bindMemberNoArg;
    }
};

template <typename T, typename R, typename P1>
struct BindMemberTraits<T, R (T::*)(P1)>
{
    typedef R ReturnType;
    typedef BindExp2<R, Detail::MemFunImpl<R, R (T::*)(P1)>, T*, Placeholder<0> > Result;

    static const Placeholder<0>& Arg()
    {
        return placeholder0;
    }
};

template <typename T, typename Method>
inline typename BindMemberTraits<T, Method>::Result BindMember(T* owner, Method method);

#endif // NL_BIND_MEMBER_H
