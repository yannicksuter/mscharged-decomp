#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "NL/nlBasicString.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

class cPlayer;
class cTeam;

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
    FT_PLAYER = NUM_V_TYPES,
    FT_TEAM,
    FT_GAME,
    FT_BALL,
};

class Variant
{
public:
    Variant(eVariantType type, const nlVector3& value)
    {
        mData.vector = value;
        mType = type;
    }

    template <typename T>
    Variant(eVariantType type, T value)
    {
        *(T*)&mData = value;
        mType = type;
    }

    Variant()
        : mType(FT_UNSPECIFIED)
    {
        Reset();
    }

    Variant(const Variant& other)
        : mType(FT_UNSPECIFIED)
    {
        Reset();
        CopyFrom(other);
    }

    ~Variant()
    {
        Reset();
    }

    Variant& operator=(Variant other)
    {
        Reset();
        CopyFrom(other);
        return *this;
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

    bool operator==(const Variant& other) const
    {
        bool bEqual = mType == other.mType;
        if (bEqual)
        {
            switch (mType)
            {
            case FT_UNSPECIFIED:
                break;
            case FT_BOOL:
                bEqual = mData.b == other.mData.b;
                break;
            case FT_CHAR:
                bEqual = mData.c == other.mData.c;
                break;
            case FT_SHORT:
                bEqual = mData.s == other.mData.s;
                break;
            case FT_INT:
                bEqual = mData.i == other.mData.i;
                break;
            case FT_U32:
                bEqual = mData.u == other.mData.u;
                break;
            case FT_FLOAT:
                bEqual = nlAbs(mData.f - other.mData.f) < 0.000001f;
                break;
            case FT_VECTOR:
                bEqual = nlNear(other.mData.vector.x, mData.vector.x)
                      && nlNear(other.mData.vector.y, mData.vector.y)
                      && nlNear(other.mData.vector.z, mData.vector.z);
                break;
            case FT_POINTER:
                bEqual = mData.pointer == other.mData.pointer;
                break;
            case FT_STRING:
                bEqual = nlStrCmp(mData.string, other.mData.string) == 0;
                break;
            case FT_PLAYER:
            case FT_TEAM:
            case FT_GAME:
            case FT_BALL:
                bEqual = mData.pointer == other.mData.pointer;
                break;
            }
        }
        return bEqual;
    }

protected:
    void CopyFrom(const Variant& other)
    {
        mType = other.mType;
        if (other.mType == FT_STRING)
        {
            const char* source = other.mData.string;
            Reset();
            mType = FT_STRING;

            int size = nlStrLen(source) + 1;
            char* copy = (char*)nlMalloc(size, 8, false);
            mData.string = copy;
            nlStrNCpy(copy, source, size);
        }
        else
        {
            mData.vector = other.mData.vector;
        }
    }

public:

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
        cPlayer* pPlayer;
        cTeam* pTeam;
        const char* string;
    } mData;
};

extern Variant gvNotSet;

#endif // _VARIANT_H_
