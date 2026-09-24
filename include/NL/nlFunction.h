#ifndef NL_FUNCTION_H
#define NL_FUNCTION_H

#include "NL/nlFunctionCommon.h"
#include "NL/nlFunctionMemory.h"

template <typename ReturnType>
class Function0
{
    friend class Function<FnVoidVoid>;

public:
    struct FunctorBase
    {
        void* operator new(unsigned long size) { return AllocateFunctionMemory(size); }
        void operator delete(void* ptr, unsigned long size)
        {
            FreeFunctionMemory(ptr, size);
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
        FunctorImpl(const Callable& callable);

        virtual ReturnType operator()();
        virtual FunctorBase* Clone() const;

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

    // Transfers the callable out of the source, which the caller
    // holds by const reference while handing over ownership.
    void* UnidentifiedTransfer(const Function0& other)
    {
        Function0& source = const_cast<Function0&>(other);
        void* target = (void*)source.mFreeFunction;
        mTag = source.mTag;
        mFunctor = (FunctorBase*)target;
        source.mTag = FUNCTION_EMPTY;
        source.mFreeFunction = 0;
        return target;
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

    void* UnidentifiedTarget() const
    {
        return (void*)mFreeFunction;
    }

private:
    FunctionTag mTag;
    union
    {
        ReturnType (*mFreeFunction)();
        FunctorBase* mFunctor;
    };
};

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

template <typename ReturnType, typename P1>
class Function1
{
    friend class Function<P1>;

public:
    struct FunctorBase
    {
        void* operator new(unsigned long size) { return AllocateFunctionMemory(size); }
        void operator delete(void* ptr, unsigned long size)
        {
            FreeFunctionMemory(ptr, size);
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

    // Transfers the callable out of the source, which the caller
    // holds by const reference while handing over ownership.
    void* UnidentifiedTransfer(const Function1& other)
    {
        Function1& source = const_cast<Function1&>(other);
        void* target = (void*)source.mFreeFunction;
        mTag = source.mTag;
        mFunctor = (FunctorBase*)target;
        source.mTag = FUNCTION_EMPTY;
        source.mFreeFunction = 0;
        return target;
    }

    operator bool() const
    {
        return mTag != FUNCTION_EMPTY;
    }

    bool Empty() const
    {
        return mTag == FUNCTION_EMPTY;
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
    Function(Callable callable);

    operator bool() const
    {
        return this->mTag != FUNCTION_EMPTY;
    }

    Function& operator=(const Function& other)
    {
        Base::operator=(other);
        return *this;
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

    template <typename Other>
    Function& operator=(const Other& other)
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
        void* operator new(unsigned long size) { return AllocateFunctionMemory(size); }
        void operator delete(void* ptr, unsigned long size)
        {
            FreeFunctionMemory(ptr, size);
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

        virtual ReturnType operator()(P1 p1, P2 p2);
        virtual FunctorBase* Clone() const;

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

template <typename ReturnType, typename P1, typename P2>
class Function<ReturnType(P1, P2)> : public Function2<ReturnType, P1, P2>
{
    typedef Function2<ReturnType, P1, P2> Base;

public:
    Function()
        : Base()
    {
    }

    Function(ReturnType (*function)(P1, P2))
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

    template <typename Other>
    Function& operator=(const Other& other)
    {
        Base::operator=(other);
        return *this;
    }
};

template <typename ReturnType, typename P1, typename P2, typename P3>
class Function3
{
public:
    struct FunctorBase
    {
        void* operator new(unsigned long size) { return AllocateFunctionMemory(size); }
        void operator delete(void* ptr, unsigned long size)
        {
            FreeFunctionMemory(ptr, size);
        }

        virtual ~FunctorBase() { }
        virtual ReturnType operator()(P1, P2, P3) = 0;
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

        virtual ReturnType operator()(P1 p1, P2 p2, P3 p3)
        {
            return Call(
                BoolToType<IsVoid<ReturnType>::value>(), p1, p2, p3);
        }

        virtual FunctorBase* Clone() const
        {
            return new FunctorImpl(*this);
        }

    private:
        ReturnType Call(BoolToType<false>, P1 p1, P2 p2, P3 p3)
        {
            return mFunctor(p1, p2, p3);
        }

        void Call(BoolToType<true>, P1 p1, P2 p2, P3 p3)
        {
            mFunctor(p1, p2, p3);
        }
    };

    Function3()
        : mTag(FUNCTION_EMPTY)
    {
    }

    Function3(ReturnType (*function)(P1, P2, P3))
        : mTag(FUNCTION_FREE)
        , mFreeFunction(function)
    {
    }

    template <typename Callable>
    Function3(const Callable& callable)
        : mTag(FUNCTION_FUNCTOR)
    {
        typedef FunctorImpl<Callable> Impl;
        mFunctor = new Impl(callable);
    }

    Function3(const Function3& other)
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

    ~Function3()
    {
        Clear();
    }

    Function3& operator=(const Function3& other)
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

    void* UnidentifiedTransfer(Function3& other)
    {
        void* target = (void*)other.mFreeFunction;
        mTag = other.mTag;
        mFunctor = (FunctorBase*)target;
        other.mTag = FUNCTION_EMPTY;
        other.mFreeFunction = 0;
        return target;
    }

    operator bool() const
    {
        return mTag != FUNCTION_EMPTY;
    }

    ReturnType operator()(P1 p1, P2 p2, P3 p3) const
    {
        if (mTag == FUNCTION_FREE)
        {
            return mFreeFunction(p1, p2, p3);
        }
        return (*mFunctor)(p1, p2, p3);
    }

    void* UnidentifiedTarget() const
    {
        return (void*)mFreeFunction;
    }

private:
    FunctionTag mTag;
    union
    {
        ReturnType (*mFreeFunction)(P1, P2, P3);
        FunctorBase* mFunctor;
    };
};

template <typename ReturnType, typename P1, typename P2, typename P3>
class Function<ReturnType(P1, P2, P3)>
    : public Function3<ReturnType, P1, P2, P3>
{
    typedef Function3<ReturnType, P1, P2, P3> Base;

public:
    Function()
        : Base()
    {
    }

    Function(ReturnType (*function)(P1, P2, P3))
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

#endif // NL_FUNCTION_H
