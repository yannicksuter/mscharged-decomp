#ifndef GAME_RENDER_TU_801B43F8_H
#define GAME_RENDER_TU_801B43F8_H

#include "Game/Render/SkinAnimatedMovableNPC.h"

template <class T>
class cInventory;

class UnidentifiedNPC_801B43F8 : public SkinAnimatedMovableNPC
{
public:
    UnidentifiedNPC_801B43F8(cSHierarchy& pHierarchy, int nModelID,
        unsigned long param1, unsigned long param2, PhysicsNPC& rPhysObj,
        cInventory<cSAnim>* pInventorySAnim, void* resource);
    virtual ~UnidentifiedNPC_801B43F8();
    virtual SkinAnimatedNPC_Type GetSkinAnimatedNPC_Type() const
    {
        return (SkinAnimatedNPC_Type)4;
    }
    virtual void Update(float fDeltaT);
    virtual void Move(float fDeltaT);
    virtual void DrawShadow(
        const cPoseAccumulator& pa, const nlMatrix4& worldMatrix);

    static void fn_801B4830(PhysicsObject* pPhysObj,
        PhysicsObject* pObjA, const nlVector3& v3Pos);
    void fn_801B4AD0();
    void fn_801B4B24(bool param);
    void fn_801B4B9C();
    void fn_801B4C14(float param);

    /* 0x84 */ cSAnim* mUnidentified084;
    /* 0x88 */ unsigned long mUnidentified088;
    /* 0x8C */ unsigned long mUnidentified08C;
    /* 0x90 */ bool mUnidentified090;
    /* 0x94 */ float mUnidentified094;
}; // total size: 0x98

#endif // GAME_RENDER_TU_801B43F8_H
