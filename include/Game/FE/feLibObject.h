#ifndef _FELIBOBJECT_H_
#define _FELIBOBJECT_H_

#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "types.h"

class feVector3
{
public:
    void GetNLVector3(nlVector3& out) const
    {
        out.x = f.x;
        out.y = f.y;
        out.z = f.z;
    }

    union
    {
        float e[3];
        struct
        {
            float x;
            float y;
            float z;
        } f;
    };
};

enum eFELibObjectType
{
    FEOT_UNKNOWN = -1,
    FEOT_LAYER = 0,
    FEOT_IMAGE = 1,
    FEOT_TEXT = 2,
    FEOT_COMPONENT = 3,
    FEOT_GROUP = 4,
    FEOT_OBJECT_TYPE_ANY = -2147483647 - 1,
};

struct FELibObjectAttributes
{
    /* 0x00 */ feVector3 v3Position;
    /* 0x0C */ feVector3 v3Rotation;
    /* 0x18 */ feVector3 v3Scale;
    /* 0x24 */ feVector3 v3Pivot;
    /* 0x30 */ bool bVisible;
    /* 0x31 */ nlColour colour;
    /* 0x35 */ u8 pad_35[3];
    /* 0x38 */ float field_0x38;
    /* 0x3C */ float field_0x3C;
    /* 0x40 */ float field_0x40;
    /* 0x44 */ float field_0x44;
};

class FELibObject
{
public:
    FELibObject();

    feVector3& GetPosition() const;
    feVector3& GetRotation() const;
    feVector3& GetScale() const;
    feVector3& GetPivot() const;
    nlColour& GetColour() const;

    /* 0x00 */ FELibObject* next;
    /* 0x04 */ FELibObject* prev;
    /* 0x08 */ FELibObjectAttributes m_attributes;
    /* 0x50 */ unsigned long m_hashID;
    /* 0x54 */ char m_szName[32];
    /* 0x74 */ eFELibObjectType m_type;
};

#endif // _FELIBOBJECT_H_
