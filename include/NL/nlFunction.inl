#ifndef NL_FUNCTION_INL
#define NL_FUNCTION_INL

#include "NL/nlFunction.h"

template <typename Callable>
inline Function<FnVoidVoid>::Function(Callable callable)
    : Base(callable)
{
}

#endif // NL_FUNCTION_INL
