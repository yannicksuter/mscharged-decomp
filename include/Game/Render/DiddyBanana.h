#ifndef GAME_RENDER_DIDDYBANANA_H
#define GAME_RENDER_DIDDYBANANA_H

#include "Game/Character.h"
#include "Game/Inventory.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "Game/SHierarchy.h"
#include "NL/nlTimer.h"

class DiddyBanana : public SkinAnimatedNPC
{
public:
    DiddyBanana(cSHierarchy& hierarchy, int modelID,
        cInventory<cSAnim>& animInventory, void* resource);
    virtual ~DiddyBanana();
    virtual SkinAnimatedNPC_Type GetSkinAnimatedNPC_Type() const;
    virtual void Update(float dt);
    virtual void DrawShadow(const cPoseAccumulator& poseAccumulator,
        const nlMatrix4& worldMatrix);

    void Hide();

    /* 0x6C */ cCharacter* mpCharacter;
    /* 0x70 */ cSAnim* mpZip;
    /* 0x74 */ cSAnim* mpUnzip;
    /* 0x78 */ Timer mTimer;
    /* 0x80 */ bool mUnidentified80;
    /* 0x81 */ unsigned char mPadding81[3];
}; // total size: 0x84

#endif // GAME_RENDER_DIDDYBANANA_H
