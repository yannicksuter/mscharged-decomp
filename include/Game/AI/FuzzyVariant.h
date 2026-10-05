#ifndef GAME_AI_FUZZYVARIANT_H
#define GAME_AI_FUZZYVARIANT_H

#include "Game/AI/Variant.h"
#include "NL/nlAVLTree.h"
#include "NL/nlList.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTimer.h"

class cTeam;
class cBall;
class cFielder;
class cGame;
class InterpreterCore;
class UnidentifiedScriptMachine;
class AIContext;

class FuzzyVariant : public Variant
{
public:
    FuzzyVariant()
        : Variant()
    {
    }

    FuzzyVariant(const FuzzyVariant& other)
        : Variant(other)
    {
    }

    FuzzyVariant(const Variant& other)
        : Variant(other)
    {
    }

    FuzzyVariant(const char* value)
        : Variant(value)
    {
    }

    template <typename T>
    FuzzyVariant(const T& value)
        : Variant(VariantTypeOf(value), value)
    {
    }

    FuzzyVariant(cPlayer* value)
        : Variant()
    {
        mType = FT_PLAYER;
        mData.pPlayer = value;
    }

    FuzzyVariant(cTeam* value)
        : Variant()
    {
        mType = FT_TEAM;
        mData.pTeam = value;
    }

    FuzzyVariant(cBall* value)
        : Variant()
    {
        mType = FT_BALL;
        mData.pointer = value;
    }

    FuzzyVariant(cGame* value)
        : Variant()
    {
        mType = FT_GAME;
        mData.pointer = value;
    }

    FuzzyVariant(eVariantType type, const nlVector3& value)
        : Variant(type, value)
    {
    }

    template <typename T>
    FuzzyVariant(eVariantType type, T value)
        : Variant(type, value)
    {
    }

    FuzzyVariant& operator=(const FuzzyVariant& other)
    {
        Variant value(other);
        Reset();
        CopyFrom(value);
        return *this;
    }

    bool operator==(const FuzzyVariant& other) const
    {
        bool bEqual = mType == other.mType;
        if (bEqual)
        {
            if (mType >= NUM_V_TYPES)
            {
                bEqual = EqualExtendedValue(other);
            }
            else
            {
                bEqual = other.Variant::operator==(*this);
            }
        }
        return bEqual;
    }

    virtual unsigned long GetHash() const;
    virtual NLString ToString() const;
    virtual bool IsPointerType() const;

private:
    bool EqualExtendedValue(const FuzzyVariant& other) const
    {
        bool equal = mType == other.mType;
        if (equal)
        {
            switch (mType)
            {
            case FT_PLAYER:
            case FT_TEAM:
            case FT_GAME:
            case FT_BALL:
                equal = mData.pointer == other.mData.pointer;
                break;
            }
        }
        return equal;
    }
};

extern FuzzyVariant fvNotSet;

inline Variant::Variant(const FuzzyVariant& other)
    : mType(FT_UNSPECIFIED)
{
    Reset();
    mType = other.mType;
    if (other.mType == FT_STRING)
    {
        int size;
        const char* source;
        char* copy;
        source = other.mData.string;
        Reset();
        mType = FT_STRING;
        size = nlStrLen(source) + 1;
        copy = (char*)nlMalloc(size, 8, false);
        mData.string = copy;
        {
            int p = 0;
            unsigned long n = size - 1;
            while (n-- != 0 && (copy[p] = source[p]) != 0)
                p++;
            copy[p] = '\0';
        }
    }
    else
    {
        mData.vector = other.mData.vector;
    }
}

#endif // GAME_AI_FUZZYVARIANT_H
