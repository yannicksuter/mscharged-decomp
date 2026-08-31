#ifndef NL_BIND_H
#define NL_BIND_H

template <int N>
struct Placeholder
{
};

extern Placeholder<0> placeholder0;

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
    R operator()(P&)
    {
        return mFunction(mT0);
    }
};

template <typename R, typename F, typename A>
BindExp1<R, F, A> Bind(F fn, const A& arg)
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
        return DoCall(p0, mT0, mT1);
    }

private:
    template <typename P, typename B1>
    R DoCall(P& p0, const Placeholder<0>&, const B1& t1)
    {
        return mFunction(p0, t1);
    }

    template <typename P, typename A1>
    R DoCall(P& p0, const A1& t0, const Placeholder<0>&)
    {
        return mFunction(t0, p0);
    }

    template <typename P, typename A1, typename B1>
    R DoCall(P&, const A1& t0, const B1& t1)
    {
        return mFunction(t0, t1);
    }
};

template <typename R, typename F, typename A, typename B>
inline BindExp2<R, F, A, B> Bind(F fn, const A& t0, const B& t1)
{
    return BindExp2<R, F, A, B>(fn, t0, t1);
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
        return DoCall(p0, mT0, mT1, mT2, mT3);
    }

private:
    template <typename P, typename B1, typename C1, typename D1>
    R DoCall(P& p0, const Placeholder<0>&, const B1& t1, const C1& t2,
        const D1& t3)
    {
        return mFunction(p0, t1, t2, t3);
    }

    template <typename P, typename A1, typename C1, typename D1>
    R DoCall(P& p0, const A1& t0, const Placeholder<0>&, const C1& t2,
        const D1& t3)
    {
        return mFunction(t0, p0, t2, t3);
    }

    template <typename P, typename A1, typename B1, typename D1>
    R DoCall(P& p0, const A1& t0, const B1& t1,
        const Placeholder<0>&, const D1& t3)
    {
        return mFunction(t0, t1, p0, t3);
    }

    template <typename P, typename A1, typename B1, typename C1>
    R DoCall(P& p0, const A1& t0, const B1& t1, const C1& t2,
        const Placeholder<0>&)
    {
        return mFunction(t0, t1, t2, p0);
    }

    template <typename P, typename A1, typename B1, typename C1, typename D1>
    R DoCall(P&, const A1& t0, const B1& t1, const C1& t2, const D1& t3)
    {
        return mFunction(t0, t1, t2, t3);
    }
};

template <typename R, typename F, typename A, typename B, typename C, typename D>
BindExp4<R, F, A, B, C, D> Bind(F fn, const A& t0, const B& t1, const C& t2, const D& t3)
{
    return BindExp4<R, F, A, B, C, D>(fn, t0, t1, t2, t3);
}

#endif // NL_BIND_H
