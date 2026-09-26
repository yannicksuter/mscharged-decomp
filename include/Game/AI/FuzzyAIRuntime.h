#ifndef GAME_AI_FUZZY_AI_RUNTIME_H
#define GAME_AI_FUZZY_AI_RUNTIME_H

#include "Game/AI/FuzzyVariant.h"
#include "Game/InterpreterCore.h"
#include "NL/nlList.h"
#include "NL/nlString.h"

class UnidentifiedFuzzyRuntimeBase;
struct UnidentifiedFielderIterator;
struct UnidentifiedRuntimeFielderReference;
struct UnidentifiedTransitionReference;

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
    UnidentifiedFuzzyRuntimeBase(AIContext*);
    virtual ~UnidentifiedFuzzyRuntimeBase();
    virtual void DoFunctionCall(unsigned int) = 0;
    virtual bool ExecuteFunction(
        FunctionEntryPoint*, unsigned int, u32, u32, u32, u32);
    virtual float UnidentifiedVirtual3(float, float);
    virtual float UnidentifiedVirtual4(float, float);
    virtual float UnidentifiedVirtual5(float);
    virtual float UnidentifiedVirtual6(float);
    virtual float UnidentifiedVirtual7(float, float, float, bool);
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
    AIContext* mValue;
    UnidentifiedRuntimeActionQueueList mCollection;
    nlListSlotPool<UnidentifiedVariant_80054AB8*> mUnidentified038;
    int mUnidentified058;
    unsigned long mUnidentified05C;
    bool mUnidentified060;
    u8 mPadding061[3];
    UnidentifiedFuzzyRuntimeValue* mUnidentified064;
};

class UnidentifiedFuzzyRuntime : public UnidentifiedFuzzyRuntimeBase
{
public:
    UnidentifiedFuzzyRuntime();
    float fn_800E34F4(unsigned long hash);
    virtual ~UnidentifiedFuzzyRuntime();
    virtual void DoFunctionCall(unsigned int);
    virtual float UnidentifiedVirtual8();
    virtual UnidentifiedVariant_80054AB8* UnidentifiedVirtual9();
    virtual void UnidentifiedVirtual12(UnidentifiedVariant_80054AB8*);
    virtual void UnidentifiedVirtual15();
};

extern UnidentifiedRuntimeTypeList lbl_806E20B0;

extern "C" int fn_80312208(unsigned long hash);
extern "C" UnidentifiedFuzzyRuntimeBase* fn_800E30A8(cFielder*);


// Shared functions and data from Game/AI/Scripts/FuzzyAIRuntime.cpp.
extern "C" UnidentifiedFuzzyRuntimeBase* fn_800E30AC(cTeam*);
extern "C" const char* fn_800E3198();
extern "C" cPlayer* fn_800E34D8();
extern "C" cBall* fn_800E34E4();
extern "C" void* fn_800E34EC();
extern "C" UnidentifiedVariant_80054AB8* fn_800E35D4(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E3700(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E382C(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E3958(UnidentifiedFuzzyRuntime*, int, float);
extern "C" void fn_800E3A84(void*, cPlayer*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E3B34(void*, cBall*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void* fn_800E3BE4(void*, void*);
extern "C" void* fn_800E3BEC(void*, void*);
extern "C" void* fn_800E3BF4(void*, void*);
extern "C" void* fn_800E3BFC(void*, void*);
extern "C" void* fn_800E3C04(void*, void*);
extern "C" void* fn_800E3C0C(void*, void*);
extern "C" void* fn_800E3C14(void*, Variant*);
extern "C" void* fn_800E3C2C(void*, Variant*);
extern "C" void* fn_800E3C44(void*, Variant*);
extern "C" void* fn_800E3C5C(void*, Variant*);
extern "C" UnidentifiedFielderIterator* fn_800E3C74(void*, cTeam*);
extern "C" UnidentifiedFielderIterator* fn_800E3D00(void*, cFielder*);
extern "C" UnidentifiedFielderIterator* fn_800E3D98(void*, cFielder*);
extern "C" UnidentifiedFielderIterator* fn_800E3E68(void*, UnidentifiedFielderIterator*);
extern "C" bool fn_800E3EDC(void*, UnidentifiedFielderIterator*);
extern "C" void fn_800E3EF8(void*, UnidentifiedFielderIterator*);
extern "C" cFielder* fn_800E3F10(void*, UnidentifiedFielderIterator*);
extern "C" cFielder* fn_800E3F1C(void*, UnidentifiedFielderIterator*);
extern "C" AIContext* fn_800E3F28(void*, UnidentifiedFielderIterator*);
extern "C" unsigned long fn_800E3FDC(const char*);
extern "C" float fn_800E3FE0();
extern "C" float fn_800E3FE4();
extern "C" float fn_800E3FE8();
extern "C" float fn_800E3FEC();
extern "C" bool fn_800E7EB4(InterpreterCore*);
extern "C" float fn_800E7ECC(void*, Variant*);
extern "C" unsigned long fn_800E7ED4(void*, Variant*);
extern "C" unsigned long fn_800E7EDC(void*, Variant*);
extern "C" unsigned long fn_800E7EE4(void*, Variant*);
extern "C" float fn_800E7EEC(void*, Variant*);
extern "C" float fn_800E7EF4(void*, UnidentifiedVariant_80054AB8*);
extern "C" float fn_800E7F48(bool);
extern "C" UnidentifiedVariant_80054AB8* fn_800E7F60(UnidentifiedFuzzyRuntime*, bool, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8090(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E81BC(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E82E8(UnidentifiedFuzzyRuntime*, float, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8414(UnidentifiedFuzzyRuntime*, float, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8540(UnidentifiedFuzzyRuntime*, UnidentifiedVariant_80054AB8*, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8B80(UnidentifiedFuzzyRuntime*, unsigned long, float);
extern "C" float fn_800E8CAC(void*, float, bool);
extern "C" unsigned long fn_800E8CB0(void*, Variant*);
extern "C" void fn_800E8CB8(void*, cPlayer*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D68(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D6C(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D70(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D74(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D78(void*, UnidentifiedRuntimeFielderReference*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8E38(UnidentifiedFuzzyRuntime*, cPlayer*, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8F8C(UnidentifiedFuzzyRuntime*, UnidentifiedRuntimeFielderReference*, float);
extern "C" bool fn_800E90EC(void*, cPlayer*);
extern "C" bool fn_800E9194(void*, cPlayer*);
extern "C" bool fn_800E923C(void*, cTeam*);
extern "C" void fn_800E92E4(UnidentifiedScriptMachine*, const char*);


extern "C" UnidentifiedFuzzyRuntimeBase* fn_80311750( UnidentifiedFuzzyRuntimeValue* value);
extern "C" void fn_80311AFC(const char* filename, bool async);
extern "C" bool fn_80311C5C();
extern "C" char fn_80312358(void*, char value);
extern "C" float fn_80314428( UnidentifiedFuzzyRuntimeBase* runtime);
extern "C" void fn_80314434(void*, UnidentifiedVariant_80054AB8*, float);
extern "C" void fn_80314438(void*, UnidentifiedVariant_80054AB8*);
extern "C" void* fn_8031443C(void*, void* value, bool);
extern "C" float fn_80314444(void*, float value, bool);
extern "C" float fn_80314448( float value, float minimum, float maximum);
extern "C" float fn_80314494( float value, float minimum, float maximum);
extern "C" float fn_803144BC( float first, float second, float amount);
extern "C" float fn_803144C8( float first, float second, float amount);
extern "C" float fn_80314504( float first, float second, float minimum, float maximum, float value);
extern "C" float fn_80314538( float first, float second, float minimum, float maximum, float value);
extern "C" void fn_80314740(void*, bool);
extern "C" void fn_80314744( UnidentifiedFuzzyRuntimeBase* runtime, int selection);
extern "C" void fn_80314750( void*, UnidentifiedTransitionReference* reference, const char* name);
extern "C" bool fn_80314798(void*);
extern "C" UnidentifiedFuzzyRuntimeValue* fn_8031479C( void*, UnidentifiedFuzzyRuntimeBase* runtime);
extern "C" int fn_803147A4( UnidentifiedFuzzyRuntimeBase* runtime);
extern "C" void fn_803148C4(float value);
extern "C" void fn_803148D0(void*, const char* value);

#endif // GAME_AI_FUZZY_AI_RUNTIME_H
