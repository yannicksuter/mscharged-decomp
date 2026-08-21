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

#endif // NL_FUNCTION_H
