#ifndef GAME_AI_FUZZY_AI_RUNTIME_H
#define GAME_AI_FUZZY_AI_RUNTIME_H

#include "Game/AI/FuzzyVariant.h"
#include "Game/InterpreterCore.h"
#include "NL/nlList.h"
#include "NL/nlString.h"

class UnidentifiedFuzzyRuntimeBase;

struct UnidentifiedRuntimeActionQueue
{
    UnidentifiedActionQueue* mQueue;
    bool mOwnsQueue;
    u8 mPadding005[3];
    int mUnidentified008;
    float mConfidence;
    UnidentifiedRuntimeActionQueue* next;
};

struct UnidentifiedRuntimeActionQueueList
{
    UnidentifiedRuntimeActionQueueList(
        UnidentifiedRuntimeActionQueue* head,
        UnidentifiedRuntimeActionQueue* tail)
    {
        mTail = tail;
        mHead = head;
    }

    UnidentifiedRuntimeActionQueue* mHead;
    UnidentifiedRuntimeActionQueue* mTail;
};

struct UnidentifiedRuntimeTypeEntry
{
    UnidentifiedRuntimeTypeEntry(const char* name, int type)
        : mType(type)
        , mHash(nlStringLowerHash(name))
    {
    }

    int mType;
    unsigned long mHash;
    UnidentifiedRuntimeTypeEntry* next;
};

struct UnidentifiedRuntimeTypeList
{
    UnidentifiedRuntimeTypeList()
    {
        mTail = 0;
        mHead = 0;
    }

    void AddEnd(UnidentifiedRuntimeTypeEntry* entry)
    {
        nlListAddEnd(&mHead, &mTail, entry);
    }

    UnidentifiedRuntimeTypeEntry* mHead;
    UnidentifiedRuntimeTypeEntry* mTail;
};

class UnidentifiedFuzzyRuntimeValue : public FuzzyVariant
{
public:
    UnidentifiedFuzzyRuntimeBase* mRuntime;
    u32 mUnidentified018;
    UnidentifiedVariantCollection ExtraData;
};

class UnidentifiedFuzzyRuntimeBase : public InterpreterCore
{
public:
    UnidentifiedFuzzyRuntimeBase(UnidentifiedFielderInput*);
    virtual ~UnidentifiedFuzzyRuntimeBase();
    virtual void DoFunctionCall(unsigned int) = 0;
    virtual bool UnidentifiedVirtual2(
        FunctionEntryPoint*, unsigned int, u32, u32, u32, u32);
    virtual float UnidentifiedVirtual3(float, float);
    virtual float UnidentifiedVirtual4(float, float);
    virtual float UnidentifiedVirtual5(float);
    virtual float UnidentifiedVirtual6(float);
    virtual float UnidentifiedVirtual7(float, float, float);
    virtual float UnidentifiedVirtual8();
    virtual UnidentifiedVariant_80054AB8* UnidentifiedVirtual9();
    virtual float UnidentifiedVirtual10(float);
    virtual float UnidentifiedVirtual11();
    virtual void UnidentifiedVirtual12(UnidentifiedVariant_80054AB8*);
    virtual UnidentifiedVariant_80054AB8* UnidentifiedReturn(
        UnidentifiedVariant_80054AB8*, float);
    virtual void UnidentifiedVirtual14(
        UnidentifiedVariant_80054AB8*, int, const Variant&);
    virtual void UnidentifiedVirtual15();

    UnidentifiedFuzzyRuntimeBase* next;
    UnidentifiedFielderInput* mValue;
    UnidentifiedRuntimeActionQueueList mCollection;
    nlListSlotPool<UnidentifiedVariant_80054AB8*> mUnidentified038;
    int mUnidentified058;
    unsigned long mUnidentified05C;
    bool mUnidentified060;
    u8 mPadding061[3];
    UnidentifiedFuzzyRuntimeValue* mUnidentified064;
};

extern UnidentifiedRuntimeTypeList lbl_806E20B0;

extern "C" int fn_80312208(unsigned long hash);

#endif // GAME_AI_FUZZY_AI_RUNTIME_H
