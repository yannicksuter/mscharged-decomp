#ifndef NL_BIND_H
#define NL_BIND_H

template <int N>
struct Placeholder
{
};

extern Placeholder<0> placeholder0;

// Bound arguments are selected per call: a placeholder yields the call
// argument, any other bound value yields itself.
template <typename P, typename T>
inline const T& UnidentifiedBindArg(P&, const T& bound)
{
    return bound;
}

template <typename P>
inline P& UnidentifiedBindArg(P& p0, const Placeholder<0>&)
{
    return p0;
}

template <typename P0, typename P1, typename T>
inline const T& UnidentifiedBindArg(P0&, P1&, const T& bound)
{
    return bound;
}

template <typename P0, typename P1>
inline P0& UnidentifiedBindArg(P0& p0, P1&, const Placeholder<0>&)
{
    return p0;
}

template <typename P0, typename P1>
inline P1& UnidentifiedBindArg(P0&, P1& p1, const Placeholder<1>&)
{
    return p1;
}

template <typename R, typename F, typename A>
struct BindExp1
{
private:
    F mFunction;
    A mT0;

public:
    BindExp1() { }
    BindExp1(F function, const A& t0)
        : mFunction(function)
        , mT0(t0)
    {
    }

    R operator()()
    {
        return mFunction(mT0);
    }

    template <typename P>
    R operator()(P& p0)
    {
        return mFunction(UnidentifiedBindArg(p0, mT0));
    }
};

template <typename R, typename F, typename A>
inline BindExp1<R, F, A> Bind(F fn, const A& arg)
{
    return BindExp1<R, F, A>(fn, arg);
}

template <typename R, typename F, typename A, typename B>
struct BindExp2
{
private:
    F mFunction;
    A mT0;
    B mT1;

public:
    BindExp2() { }
    BindExp2(F function, const A& t0, const B& t1)
        : mFunction(function)
        , mT0(t0)
        , mT1(t1)
    {
    }

    R operator()()
    {
        return mFunction(mT0, mT1);
    }

    template <typename P>
    R operator()(P& p0)
    {
        return mFunction(UnidentifiedBindArg(p0, mT0), UnidentifiedBindArg(p0, mT1));
    }
};

template <typename R, typename F, typename A, typename B>
inline BindExp2<R, F, A, B> Bind(F fn, const A& t0, const B& t1)
{
    return BindExp2<R, F, A, B>(fn, t0, t1);
}

template <typename R, typename F, typename A, typename B, typename C>
struct BindExp3
{
private:
    F mFunction;
    A mT0;
    B mT1;
    C mT2;

public:
    BindExp3() { }
    BindExp3(F function, const A& t0, const B& t1, const C& t2)
        : mFunction(function)
        , mT0(t0)
        , mT1(t1)
        , mT2(t2)
    {
    }

    R operator()()
    {
        return mFunction(mT0, mT1, mT2);
    }

    template <typename P0, typename P1>
    R operator()(P0& p0, P1& p1)
    {
        return mFunction(UnidentifiedBindArg(p0, p1, mT0),
            UnidentifiedBindArg(p0, p1, mT1),
            UnidentifiedBindArg(p0, p1, mT2));
    }

    template <typename P>
    R operator()(P& p0)
    {
        return mFunction(UnidentifiedBindArg(p0, mT0), UnidentifiedBindArg(p0, mT1), UnidentifiedBindArg(p0, mT2));
    }
};

template <typename R, typename F, typename A, typename B, typename C>
inline BindExp3<R, F, A, B, C> Bind(F fn, const A& t0, const B& t1,
    const C& t2)
{
    return BindExp3<R, F, A, B, C>(fn, t0, t1, t2);
}

template <typename R, typename F, typename A, typename B, typename C, typename D>
struct BindExp4
{
private:
    F mFunction;
    A mT0;
    B mT1;
    C mT2;
    D mT3;

public:
    BindExp4() { }
    BindExp4(F function, const A& t0, const B& t1, const C& t2, const D& t3)
        : mFunction(function)
        , mT0(t0)
        , mT1(t1)
        , mT2(t2)
        , mT3(t3)
    {
    }

    R operator()()
    {
        return mFunction(mT0, mT1, mT2, mT3);
    }

    template <typename P>
    R operator()(P& p0)
    {
        return mFunction(UnidentifiedBindArg(p0, mT0), UnidentifiedBindArg(p0, mT1), UnidentifiedBindArg(p0, mT2), UnidentifiedBindArg(p0, mT3));
    }

    template <typename P0, typename P1>
    R operator()(P0& p0, P1& p1)
    {
        return mFunction(UnidentifiedBindArg(p0, p1, mT0),
            UnidentifiedBindArg(p0, p1, mT1),
            UnidentifiedBindArg(p0, p1, mT2),
            UnidentifiedBindArg(p0, p1, mT3));
    }
};

template <typename R, typename F, typename A, typename B, typename C, typename D>
BindExp4<R, F, A, B, C, D> Bind(F fn, const A& t0, const B& t1, const C& t2, const D& t3);

#endif // NL_BIND_H
