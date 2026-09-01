#ifndef NL_FUNCTION_H
#define NL_FUNCTION_H

#include "NL/nlMemory.h"

extern "C" void* fn_802B1C4C(unsigned long);
extern "C" void fn_802B1D4C(void*, unsigned long);

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
    R operator()(T* object, P1 p1, P2 p2) const
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
Detail::MemFunImpl<R, R (T::*)()> MemFun(R (T::*function)())
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

template <typename ReturnType>
class Function0
{
    friend class Function<FnVoidVoid>;

public:
    struct FunctorBase
    {
        void* operator new(unsigned long size) { return fn_802B1C4C(size); }
        void operator delete(void* ptr, unsigned long size)
        {
            fn_802B1D4C(ptr, size);
        }

        virtual ~FunctorBase() { }
        virtual ReturnType operator()() = 0;
        virtual FunctorBase* Clone() const = 0;
    };

    template <typename Callable>
    struct FunctorImpl : public FunctorBase
    {
    private:
        Callable mFunctor;

    public:
        FunctorImpl(const Callable& callable)
            : mFunctor(callable)
        {
        }

        virtual ReturnType operator()()
        {
            return Call(BoolToType<IsVoid<ReturnType>::value>());
        }

        virtual FunctorBase* Clone() const
        {
            return new FunctorImpl(*this);
        }

    private:
        ReturnType Call(BoolToType<false>)
        {
            return mFunctor();
        }

        void Call(BoolToType<true>)
        {
            mFunctor();
        }
    };

    Function0()
        : mTag(FUNCTION_EMPTY)
    {
    }

    Function0(ReturnType (*function)())
        : mTag(FUNCTION_FREE)
        , mFreeFunction(function)
    {
    }

    template <typename Callable>
    Function0(const Callable& callable)
        : mTag(FUNCTION_FUNCTOR)
    {
        typedef FunctorImpl<Callable> Impl;
        mFunctor = new Impl(callable);
    }

    Function0(const Function0& other)
        : mTag(other.mTag)
    {
        if (mTag == FUNCTION_FREE)
        {
            mFreeFunction = other.mFreeFunction;
        }
        else if (mTag == FUNCTION_FUNCTOR)
        {
            mFunctor = other.mFunctor->Clone();
        }
    }

    ~Function0()
    {
        Clear();
    }

    Function0& operator=(const Function0& other)
    {
        Clear();
        mTag = other.mTag;
        if (mTag == FUNCTION_FREE)
        {
            mFreeFunction = other.mFreeFunction;
        }
        else if (mTag == FUNCTION_FUNCTOR)
        {
            mFunctor = other.mFunctor->Clone();
        }
        return *this;
    }

    void Clear()
    {
        if (mTag == FUNCTION_FUNCTOR)
        {
            delete mFunctor;
        }
        mTag = FUNCTION_EMPTY;
    }

    operator bool() const
    {
        return mTag != FUNCTION_EMPTY;
    }

    ReturnType operator()() const
    {
        if (mTag == FUNCTION_FREE)
        {
            return mFreeFunction();
        }
        return (*mFunctor)();
    }

private:
    FunctionTag mTag;
    union
    {
        ReturnType (*mFreeFunction)();
        FunctorBase* mFunctor;
    };
};

template <typename ReturnType, typename P1>
class Function1
{
    friend class Function<P1>;

public:
    struct FunctorBase
    {
        void* operator new(unsigned long size) { return fn_802B1C4C(size); }
        void operator delete(void* ptr, unsigned long size)
        {
            fn_802B1D4C(ptr, size);
        }

        virtual ~FunctorBase() { }
        virtual ReturnType operator()(P1) = 0;
        virtual FunctorBase* Clone() const = 0;
    };

    template <typename Callable>
    struct FunctorImpl : public FunctorBase
    {
    private:
        Callable mFunctor;

    public:
        FunctorImpl(const Callable& callable);

        virtual ReturnType operator()(P1 p0);
        virtual FunctorBase* Clone() const;

    private:
        ReturnType Call(BoolToType<false>, P1 p0)
        {
            return mFunctor(p0);
        }

        void Call(BoolToType<true>, P1 p0)
        {
            mFunctor(p0);
        }
    };

    Function1()
        : mTag(FUNCTION_EMPTY)
    {
    }

    Function1(ReturnType (*function)(P1))
        : mTag(FUNCTION_FREE)
        , mFreeFunction(function)
    {
    }

    template <typename Callable>
    Function1(const Callable& callable)
        : mTag(FUNCTION_FUNCTOR)
    {
        typedef FunctorImpl<Callable> Impl;
        mFunctor = new Impl(callable);
    }

    Function1(const Function1& other)
        : mTag(other.mTag)
    {
        if (mTag == FUNCTION_FREE)
        {
            mFreeFunction = other.mFreeFunction;
        }
        else if (mTag == FUNCTION_FUNCTOR)
        {
            mFunctor = other.mFunctor->Clone();
        }
    }

    ~Function1()
    {
        Clear();
    }

    void Clear()
    {
        if (mTag == FUNCTION_FUNCTOR)
        {
            delete mFunctor;
        }
        mTag = FUNCTION_EMPTY;
    }

    Function1& operator=(const Function1& other)
    {
        Clear();
        mTag = other.mTag;
        if (mTag == FUNCTION_FREE)
        {
            mFreeFunction = other.mFreeFunction;
        }
        else if (mTag == FUNCTION_FUNCTOR)
        {
            mFunctor = other.mFunctor->Clone();
        }
        return *this;
    }

    void UnidentifiedTransfer(Function1& other)
    {
        mTag = other.mTag;
        mFreeFunction = other.mFreeFunction;
        other.mTag = FUNCTION_EMPTY;
        other.mFreeFunction = 0;
    }

    operator bool() const
    {
        return mTag != FUNCTION_EMPTY;
    }

    ReturnType operator()(P1 p0) const
    {
        if (mTag == FUNCTION_FREE)
        {
            return mFreeFunction(p0);
        }
        return (*mFunctor)(p0);
    }

    void* UnidentifiedTarget() const
    {
        return (void*)mFreeFunction;
    }

private:
    FunctionTag mTag;
    union
    {
        ReturnType (*mFreeFunction)(P1);
        FunctorBase* mFunctor;
    };
};

template <typename ReturnType, typename P1>
template <typename Callable>
inline Function1<ReturnType, P1>::FunctorImpl<Callable>::FunctorImpl(
    const Callable& callable)
    : mFunctor(callable)
{
}

template <typename ReturnType, typename P1>
template <typename Callable>
inline ReturnType Function1<ReturnType, P1>::FunctorImpl<Callable>::operator()(
    P1 p0)
{
    return Call(BoolToType<IsVoid<ReturnType>::value>(), p0);
}

template <typename ReturnType, typename P1>
template <typename Callable>
inline typename Function1<ReturnType, P1>::FunctorBase*
Function1<ReturnType, P1>::FunctorImpl<Callable>::Clone() const
{
    return new FunctorImpl(*this);
}

template <typename P1>
class Function : public Function1<void, P1>
{
    typedef Function1<void, P1> Base;

public:
    Function()
        : Base()
    {
    }

    Function(void (*function)(P1))
        : Base(function)
    {
    }
    template <typename Callable>
    Function(Callable callable)
        : Base(callable)
    {
    }

    operator bool() const
    {
        return this->mTag != FUNCTION_EMPTY;
    }

    Function& operator=(const Function& other)
    {
        Base::operator=(other);
        return *this;
    }
};

template <>
class Function<FnVoidVoid> : public Function0<void>
{
    typedef Function0<void> Base;

public:
    Function()
        : Base()
    {
    }

    Function(void (*function)())
        : Base(function)
    {
    }

    template <typename Callable>
    Function(Callable callable)
        : Base(callable)
    {
    }

    operator bool() const
    {
        return this->mTag != FUNCTION_EMPTY;
    }

    template <typename Other>
    Function& operator=(const Other& other)
    {
        Base::operator=(other);
        return *this;
    }
};

template <typename ReturnType, typename P1>
class Function<ReturnType(P1)> : public Function1<ReturnType, P1>
{
    typedef Function1<ReturnType, P1> Base;

public:
    Function()
        : Base()
    {
    }

    Function(ReturnType (*function)(P1))
        : Base(function)
    {
    }

    template <typename Callable>
    Function(Callable callable)
        : Base(callable)
    {
    }

    Function& operator=(const Function& other)
    {
        Base::operator=(other);
        return *this;
    }
};

template <typename ReturnType, typename P1, typename P2>
class Function2
{
public:
    struct FunctorBase
    {
        void* operator new(unsigned long size) { return fn_802B1C4C(size); }
        void operator delete(void* ptr, unsigned long size)
        {
            fn_802B1D4C(ptr, size);
        }

        virtual ~FunctorBase() { }
        virtual ReturnType operator()(P1, P2) = 0;
        virtual FunctorBase* Clone() const = 0;
    };

    template <typename Callable>
    struct FunctorImpl : public FunctorBase
    {
    private:
        Callable mFunctor;

    public:
        FunctorImpl(const Callable& callable)
            : mFunctor(callable)
        {
        }

        virtual ReturnType operator()(P1 p1, P2 p2)
        {
            return Call(p1, p2, BoolToType<IsVoid<ReturnType>::value>());
        }

        virtual FunctorBase* Clone() const
        {
            return new FunctorImpl(*this);
        }

    private:
        ReturnType Call(P1 p1, P2 p2, BoolToType<false>)
        {
            return mFunctor(p1, p2);
        }

        void Call(P1 p1, P2 p2, BoolToType<true>)
        {
            mFunctor(p1, p2);
        }
    };

    Function2()
        : mTag(FUNCTION_EMPTY)
    {
    }

    template <typename Callable>
    Function2(Callable callable)
        : mTag(FUNCTION_FUNCTOR)
    {
        typedef FunctorImpl<Callable> Impl;
        mFunctor = new Impl(callable);
    }

    Function2(ReturnType (*function)(P1, P2))
        : mTag(FUNCTION_FREE)
        , mFreeFunction(function)
    {
    }

    Function2(const Function2& other)
        : mTag(other.mTag)
    {
        if (mTag == FUNCTION_FREE)
        {
            mFreeFunction = other.mFreeFunction;
        }
        else if (mTag == FUNCTION_FUNCTOR)
        {
            mFunctor = other.mFunctor->Clone();
        }
    }

    ~Function2()
    {
        Clear();
    }

    Function2& operator=(const Function2& other)
    {
        Clear();
        mTag = other.mTag;
        if (mTag == FUNCTION_FREE)
        {
            mFreeFunction = other.mFreeFunction;
        }
        else if (mTag == FUNCTION_FUNCTOR)
        {
            mFunctor = other.mFunctor->Clone();
        }
        return *this;
    }

    void Clear()
    {
        if (mTag == FUNCTION_FUNCTOR)
        {
            delete mFunctor;
        }
        mTag = FUNCTION_EMPTY;
    }

    operator bool() const
    {
        return mTag != FUNCTION_EMPTY;
    }

    ReturnType operator()(P1 p0, P2 p1) const
    {
        if (mTag == FUNCTION_FREE)
        {
            return mFreeFunction(p0, p1);
        }
        return (*mFunctor)(p0, p1);
    }

private:
    FunctionTag mTag;
    union
    {
        ReturnType (*mFreeFunction)(P1, P2);
        FunctorBase* mFunctor;
    };
};

#endif // NL_FUNCTION_H
