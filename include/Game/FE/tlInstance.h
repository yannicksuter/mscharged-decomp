#ifndef _TLINSTANCE_H_
#define _TLINSTANCE_H_

#include "Game/FE/feLibObject.h"
#include "types.h"

enum eTimeLineAssetType
{
    TLAT_UNKNOWN = -1,
    TLAT_SLIDE = 0,
    TLAT_LAYER = 1,
    TLAT_IMAGE = 2,
    TLAT_TEXT = 3,
    TLAT_COMPONENT = 4,
    TLAT_GROUP = 5,
    TLAT_MAX_TYPES = 6,
};

class TLComponent;

class TLInstance
{
public:
    eTimeLineAssetType GetType() const;
    bool IsValidAtTime(float fCurrentTime);

    feVector3& GetAssetPosition() const;
    feVector3& GetAssetRotation() const;
    feVector3& GetAssetScale() const;
    nlColour& GetAssetColour() const;
    feVector3& GetPosition() const;
    feVector3& GetRotation() const;
    feVector3& GetScale() const;
    feVector3& GetPivot() const;
    nlColour& GetColour() const;

    void SetAssetPosition(float x, float y, float z);
    void SetAssetRotation(float x, float y, float z);
    void SetAssetScale(float x, float y, float z);
    void SetAssetColour(const nlColour& color);

    /* 0x00 */ TLInstance* m_next;
    /* 0x04 */ TLInstance* m_prev;
    /* 0x08 */ TLInstance* pChildren;
    /* 0x0C */ TLComponent* m_component;
    /* 0x10 */ f32 m_fStartTime;
    /* 0x14 */ f32 m_fDuration;
    /* 0x18 */ char m_szName[32];
    /* 0x38 */ u32 m_hash;
    /* 0x3C */ FELibObjectAttributes m_overloadedAttributes;
    /* 0x84 */ u32 m_overloadFlags;
    /* 0x88 */ eTimeLineAssetType m_type;
    /* 0x8C */ u16 m_priority;
    /* 0x8E */ bool m_bVisible;
};

#endif // _TLINSTANCE_H_
