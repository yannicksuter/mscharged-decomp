#ifndef UNCLASSIFIED_TU_80199880_H
#define UNCLASSIFIED_TU_80199880_H

#include "Game/Character.h"
#include "Game/Inventory.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "Game/SHierarchy.h"
#include "NL/nlTimer.h"

class UnidentifiedSkinAnimatedNPC_80199880 : public SkinAnimatedNPC
{
public:
    UnidentifiedSkinAnimatedNPC_80199880(cSHierarchy& hierarchy, int modelID,
        cInventory<cSAnim>& animInventory, void* resource);
    virtual ~UnidentifiedSkinAnimatedNPC_80199880();
    virtual SkinAnimatedNPC_Type GetSkinAnimatedNPC_Type() const;
    virtual void Update(float dt);
    virtual void DrawShadow(const cPoseAccumulator& poseAccumulator,
        const nlMatrix4& worldMatrix);

    void fn_80199A88();

    /* 0x6C */ cCharacter* mpCharacter;
    /* 0x70 */ cSAnim* mpZip;
    /* 0x74 */ cSAnim* mpUnzip;
    /* 0x78 */ Timer mTimer;
    /* 0x80 */ bool mUnidentified80;
    /* 0x81 */ unsigned char mPadding81[3];
}; // total size: 0x84

#endif // UNCLASSIFIED_TU_80199880_H
