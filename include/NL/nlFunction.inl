#ifndef NL_FUNCTION_INL
#define NL_FUNCTION_INL

#include "NL/nlFunction.h"

// The callable constructor of Function stays out of NL/nlFunction.h: the ISO
// template parser that NL/nlConfig.cpp is built with stops with an internal
// compiler error on its base initializer.
template <typename Signature>
template <typename Callable>
inline Function<Signature>::Function(Callable callable)
    : Base(callable)
{
}

#endif // NL_FUNCTION_INL
