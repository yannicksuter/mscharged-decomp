#ifndef NL_FUNCTION_H
#define NL_FUNCTION_H

enum FunctionTag
{
    FUNCTION_EMPTY = 0,
    FUNCTION_FREE = 1,
    FUNCTION_FUNCTOR = 2,
};

template <typename ReturnType>
class Function0
{
public:
    struct FunctorBase
    {
        virtual ~FunctorBase() { }
        virtual ReturnType operator()() = 0;
        virtual FunctorBase* Clone() const = 0;
    };

    Function0()
        : mTag(FUNCTION_EMPTY)
    {
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
        if (mTag == FUNCTION_FUNCTOR && mFunctor != 0)
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

template <typename Signature>
class Function;

template <typename ReturnType, typename P1>
class Function1
{
    friend class Function<P1>;

public:
    struct FunctorBase
    {
        virtual ~FunctorBase() { }
        virtual ReturnType operator()(P1) = 0;
        virtual FunctorBase* Clone() const = 0;
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

private:
    FunctionTag mTag;
    union
    {
        ReturnType (*mFreeFunction)(P1);
        FunctorBase* mFunctor;
    };
};

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

    operator bool() const
    {
        return this->mTag != FUNCTION_EMPTY;
    }
};

template <typename ReturnType, typename P1, typename P2>
class Function2
{
public:
    struct FunctorBase
    {
        virtual ~FunctorBase() { }
        virtual ReturnType operator()(P1, P2) = 0;
        virtual FunctorBase* Clone() const = 0;
    };

    Function2()
        : mTag(FUNCTION_EMPTY)
    {
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
