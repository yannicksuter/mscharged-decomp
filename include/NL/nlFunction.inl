#ifndef NL_FUNCTION_INL
#define NL_FUNCTION_INL

#include "NL/nlFunction.h"

template <typename ReturnType>
template <typename Callable>
inline Function0<ReturnType>::FunctorImpl<Callable>::FunctorImpl(
    const Callable& callable)
    : mFunctor(callable)
{
}

template <typename ReturnType>
template <typename Callable>
inline ReturnType Function0<ReturnType>::FunctorImpl<Callable>::operator()()
{
    return Call(BoolToType<IsVoid<ReturnType>::value>());
}

template <typename ReturnType>
template <typename Callable>
inline typename Function0<ReturnType>::FunctorBase*
Function0<ReturnType>::FunctorImpl<Callable>::Clone() const
{
    return new FunctorImpl(*this);
}

template <typename Callable>
inline Function<FnVoidVoid>::Function(Callable callable)
    : Base(callable)
{
}

template <typename ReturnType, typename P1, typename P2>
template <typename Callable>
inline ReturnType Function2<ReturnType, P1, P2>::FunctorImpl<Callable>::operator()(P1 p1, P2 p2)
{
    return Call(p1, p2, BoolToType<IsVoid<ReturnType>::value>());
}

template <typename ReturnType, typename P1, typename P2>
template <typename Callable>
inline typename Function2<ReturnType, P1, P2>::FunctorBase*
Function2<ReturnType, P1, P2>::FunctorImpl<Callable>::Clone() const
{
    return new FunctorImpl(*this);
}

#endif // NL_FUNCTION_INL
