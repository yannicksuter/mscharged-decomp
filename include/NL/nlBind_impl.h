#ifndef NL_BIND_IMPL_H
#define NL_BIND_IMPL_H

// Out-of-line bind expression template. Units that instantiate one include this
// file after their code so the instantiation is emitted with the unit's
// deferred weak functions instead of being placed after its first caller;
// every other unit calls the retained copy through the NL/nlBind.h
// declaration.

#include "NL/nlBind.h"

template <typename R, typename F, typename A, typename B, typename C, typename D>
BindExp4<R, F, A, B, C, D> Bind(F fn, const A& t0, const B& t1, const C& t2, const D& t3)
{
    return BindExp4<R, F, A, B, C, D>(fn, t0, t1, t2, t3);
}

#endif // NL_BIND_IMPL_H
