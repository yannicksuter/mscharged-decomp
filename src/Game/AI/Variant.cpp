#include "Game/AI/Variant.inl"

#include "NL/nlFormat.h"

Variant gvNotSet;

NLString Variant::ToString() const
{
    NLString result;

    if (IsSet())
    {
        NLString valueString = "???";

        switch (GetType())
        {
        case FT_BOOL:
            valueString = mData.b ? "True" : "False";
            break;
        case FT_CHAR:
            valueString = Format(NLString("{0}"), mData.c);
            break;
        case FT_SHORT:
            valueString = Format(NLString("{0}"), (int)mData.s);
            break;
        case FT_INT:
            valueString = Format(NLString("{0}"), mData.i);
            break;
        case FT_U32:
            valueString = Format(NLString("{0}"), mData.u);
            break;
        case FT_FLOAT:
            valueString = Format(NLString("{0}"), mData.f);
            break;
        case FT_POINTER:
            valueString = Format(NLString("{0}"), (unsigned long)mData.pointer);
            break;
        case FT_VECTOR:
            valueString = Format(NLString("({0},{1},{2})"), mData.vector.x, mData.vector.y, mData.vector.z);
            break;
        case FT_STRING:
            valueString = Format(NLString("{0}"), mData.string);
            break;
        }

        result = valueString;
    }
    else
    {
        result = "N/A";
    }

    return result;
}

unsigned long Variant::GetHash() const
{
    unsigned long hash = 0;

    switch (GetType())
    {
    case FT_BOOL:
        hash = mData.b;
        break;
    case FT_CHAR:
        hash = mData.c;
        break;
    case FT_SHORT:
        hash = mData.s;
        break;
    case FT_INT:
        hash = mData.i;
        break;
    case FT_U32:
        hash = mData.u;
        break;
    case FT_FLOAT:
        hash = (unsigned long)mData.f;
        break;
    case FT_POINTER:
        hash = (unsigned long)mData.pointer;
        break;
    case FT_VECTOR:
        hash = (unsigned long)mData.vector.x
             * (unsigned long)mData.vector.y;
        {
            unsigned long zHash = (unsigned long)mData.vector.z;
            zHash ^= hash;
            hash = zHash;
        }
        break;
    case FT_STRING:
        hash = nlStringHash(mData.string);
        break;
    }

    return hash;
}
