#ifndef NL_FUNCTION2_H
#define NL_FUNCTION2_H

#include "NL/nlFunctionCommon.h"
#include "NL/nlFunctionMemory.h"

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
        FunctorImpl(const Callable& callable);

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
    Function2(const Callable& callable)
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

#endif // NL_FUNCTION2_H
