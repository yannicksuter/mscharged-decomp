#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "NL/nlBasicString.h"
#include "NL/nlMath.h"

enum eVariantType
{
    FT_UNSPECIFIED = -1,
    FT_BOOL = 0,
    FT_CHAR = 1,
    FT_SHORT = 2,
    FT_INT = 3,
    FT_U32 = 4,
    FT_FLOAT = 5,
    FT_VECTOR = 6,
    FT_POINTER = 7,
    FT_STRING = 8,
    NUM_V_TYPES = 9,
};

class Variant
{
public:
    Variant()
        : mType(FT_UNSPECIFIED)
    {
        Reset();
    }

    ~Variant()
    {
        Reset();
    }

    virtual void Reset()
    {
        if (mType == FT_STRING)
        {
            delete[] mData.string;
        }
        mType = FT_UNSPECIFIED;
        mData.u = 0;
    }

    virtual eVariantType GetType() const
    {
        return mType;
    }

    virtual unsigned long GetHash() const;
    virtual NLString ToString() const;

    virtual bool IsPointerType() const;

    bool IsSet() const;

    eVariantType mType;
    union
    {
        bool b;
        char c;
        signed short s;
        int i;
        unsigned long u;
        float f;
        nlVector3 vector;
        void* pointer;
        const char* string;
    } mData;
};

extern Variant gvNotSet;

#endif // _VARIANT_H_
