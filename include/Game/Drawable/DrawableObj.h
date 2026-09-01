#ifndef GAME_DRAWABLE_DRAWABLE_OBJ_H
#define GAME_DRAWABLE_DRAWABLE_OBJ_H

#include "NL/nlMath.h"
#include "NL/gl/glModel.h"

class GLView;

struct UnidentifiedDrawableViewState
{
    char _000[0x68];
    /* 0x68 */ GLView* mView68;
    /* 0x6C */ GLView* mView6C;
};

class DrawableObject
{
public:
    virtual ~DrawableObject();
    virtual void V1();
    virtual nlMatrix4& GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4*);
    virtual void V4();
    virtual void Draw();
    virtual void V6();
    virtual void V7();
    virtual void V8(void*);
    virtual DrawableObject* Clone(unsigned long) const;

    unsigned long GetHashID() const { return m_uHashID; }

    /* 0x04 */ unsigned long m_uHashID;
    /* 0x08 */ u8 mUnidentified08[0x08];
    /* 0x10 */ UnidentifiedDrawableViewState* mUnidentified10;
    /* 0x14 */ u8 mUnidentified14[0x0C];
    /* 0x20 */ nlMatrix4 mWorldMatrix;
    /* 0x60 */ u8 mUnidentified60[0x04];
    /* 0x64 */ glModel* m_pModel;
    /* 0x68 */ u8 mUnidentified68[0x08];
    /* 0x70 */ unsigned long m_uObjectFlags;
    /* 0x74 */ int mUnidentified074;
    /* 0x78 */ u32 renderFlags;
    /* 0x7C */ nlQuaternion orientation;
    /* 0x8C */ nlVector3 translation;
    /* 0x98 */ float modelScale;
    /* 0x9C */ float snapshotScale;
    /* 0xA0 */ char _0A0[0x40];
    /* 0xE0 */ bool worldMatrixUpToDate;
};

#endif // GAME_DRAWABLE_DRAWABLE_OBJ_H
